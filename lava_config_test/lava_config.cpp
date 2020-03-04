#include <map>
#include <time.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <fstream>
#include <iostream>
#include "lava_crt.h"
#include "singleton.h"
#include "lava_config.h"

#ifdef LAVA_WIN
#include <filesystem>
#else
#include <sys/stat.h>
#endif

//need extern using out of this file
lava::utils::i_lava_config *get_lava_config()
{
	return singleton_lazy<lava::utils::lava_config, lava::utils::i_lava_config>::instance();
}

void release_lava_config()
{
	singleton_lazy<lava::utils::lava_config, lava::utils::i_lava_config>::release();
}


namespace lava {
namespace utils {

class lava_config::ConfigImpl
{
public:
	ConfigImpl();

public:
	bool init(const char *conf_file_name);
	inline bool fail() { return !loaded_; }
	bool load();
	const std::string get(const std::string &key) const;
	const std::vector<std::string> values(const std::string &key);
	int count(const std::string &key);

	void run();
	void stop();

private:
	void clear();

private:
	bool												loaded_;
#ifdef LAVA_WIN
	time_t												lastUpdateTime_;
#else
	long												lastUpdateTime_;
#endif
	std::string											settingName_;
	std::atomic<bool>									thread_exit_;
	std::shared_ptr<std::thread>						worker_thread_;
	std::map<std::string, std::vector<std::string> >	keyvalues_;
	static std::string									emptyString_; //for false return value
	static std::vector<std::string>						emptyArray_;
};

std::string lava_config::ConfigImpl::emptyString_;
std::vector<std::string> lava_config::ConfigImpl::emptyArray_;
std::mutex mutex_;

lava_config::ConfigImpl::ConfigImpl()
	: loaded_(false)
	, lastUpdateTime_(0) {}

bool lava_config::ConfigImpl::init(const char *conf_file_name)
{
	if (nullptr == conf_file_name)
		settingName_ = "setting.ini";
	else
		settingName_ = conf_file_name;
	worker_thread_ = std::make_shared<std::thread>(std::bind(&lava_config::ConfigImpl::run, this));
	if (worker_thread_)
		return true;
	return false;
}

bool lava_config::ConfigImpl::load()
{
#ifdef LAVA_WIN
	time_t updateTime = 0;
	std::tr2::sys::path path(settingName_);
#else
	long updateTime = 0;
#endif

#ifdef LAVA_WIN
	try
	{
		updateTime = std::tr2::sys::last_write_time(path);
#else
		struct stat stat_buf;
		int result = stat(settingName_.c_str(), &stat_buf);
		if (0 != result)
			std::cout << "get config file stat error! result:" << result << std::endl;
		else
			updateTime = stat_buf.st_mtime;
#endif
		if (lastUpdateTime_ == updateTime)
			return true;
		else
			lastUpdateTime_ = updateTime;
#ifdef LAVA_WIN
	}
	catch (std::tr2::sys::filesystem_error e)
	{
		log_error(lava_config_module, "Config::ConfigImpl::init: %s", e.what());
	}
#endif

	std::ifstream in(settingName_.c_str(), std::ios::in);
	if (in.fail())
	{
		loaded_ = false;
		std::cout << "Config::ConfigImpl::init: open fail" << settingName_ << std::endl;
		return false;
	}

	clear();
	std::string line;
	while (!in.eof())
	{
		std::getline(in, line);
		std::string::size_type pos = line.find("=");
		if (pos == std::string::npos)
			continue;
		std::string title = line.substr(0, pos);
		std::string content = line.substr(pos + 1);
		trim(title);
		trim(content);
		keyvalues_[title].push_back(content);
	}

	loaded_ = true;
	std::cout << "Config::ConfigImpl::init: updated" << settingName_ << std::endl;
	return true;
}

void lava_config::ConfigImpl::run()
{
	thread_exit_.store(false);
	while (true)
	{
		if (thread_exit_)
			break;
		load();
		std::this_thread::sleep_for(std::chrono::seconds(CHECK_INTERVAL));
	}
}

void lava_config::ConfigImpl::stop()
{
	thread_exit_.store(true);
	worker_thread_->join();
}

void lava_config::ConfigImpl::clear()
{
	conf_autolock write_guard(mutex_);
	keyvalues_.clear();
}

const std::string lava_config::ConfigImpl::get(const std::string &key) const
{
	conf_autolock write_guard(mutex_);
	std::map<std::string, std::vector<std::string> >::const_iterator it = keyvalues_.find(key);
	if (it == keyvalues_.end())
		return emptyString_;
	else
		return it->second[0];
}

const std::vector<std::string> lava_config::ConfigImpl::values(const std::string &key)
{
	conf_autolock write_guard(mutex_);
	if (keyvalues_.count(key))
		return keyvalues_[key];
	return emptyArray_;
}

int lava_config::ConfigImpl::count(const std::string &key)
{
	conf_autolock write_guard(mutex_);
	if (0 == keyvalues_.count(key))
		return 0;
	return keyvalues_[key].size();
}

///////////////////////////////////////////////////////////////////////////////
//above is the implementation of class lava_config, 
//and this is lava_config which charge for loading config file
///////////////////////////////////////////////////////////////////////////////
lava_config::lava_config() : configs_(new ConfigImpl()) {}

lava_config::~lava_config()
{
	configs_->stop();
	if (configs_)
	{
		delete configs_;
		configs_ = nullptr;
	}
}

bool lava_config::init(__lv_in const char *conf_file_name/* = nullptr */)
{
	return configs_->init(conf_file_name);
}

bool lava_config::fail() const
{
	return configs_->fail();
}

void lava_config::value(__lv_in const char *key, __lv_out char *&value, 
	__lv_out int &value_len) const
{
	//release the ptr client input
	if (value)
	{
		delete value;
		value = nullptr;
		value_len = 0;
	}

	std::string ret = configs_->get(key);
	if (!ret.empty())
	{
		value_len = ret.length();
		value = new char[value_len + 1];
		memset(value, 0, value_len + 1);
		memcpy(value, ret.c_str(), value_len + 1);
	}
}

const std::vector<std::string> lava_config::values(__lv_in const char *key) const
{
	return configs_->values(key);
}

int lava_config::count(__lv_in const char *key) const
{
	return configs_->count(key);
}

}
}
