/***************************************************************************************************
*Copyright(C): 2019, Sumscope
*FileName    : lava_config.h
*Author      : lei.wang@sumscope.com
*Version     : 1.0
*Date        : 2019-09-20
*Desc        : This class completes config file loading and provides an interface
*            : for users to query.
*History     : Add by Lei.Wang 2019-09-20
***************************************************************************************************/
#ifndef __LAVA_CONFIG_H__
#define __LAVA_CONFIG_H__
#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "lava_base.h"

typedef lava::base::scopelocker<std::mutex>		conf_autolock;
const unsigned int								CHECK_INTERVAL = 5; //load config every 5 sec


namespace lava {
namespace utils {

//Interfaces of lava config module
struct i_lava_config
{
	/*
	 * init
	 * @param   conf_file_name : config file name
	 * @return  init result : true->success | false-failed
	 */
	virtual bool init(__lv_in const char *conf_file_name = nullptr) = 0;

	/*
	 * Indicates whether the config file was loaded successfully
	 * @return  true->failed | false-success
	 */
	virtual bool fail() const = 0;

	/*
	 * get value by the key
	 * @param   key   [in] key in config file
	 * @return  value, if not find, return nullptr, if multi value, return the first
	 *          value_len, the length of the value.
	 * @desc    value could be nullptr and it will be new in this function.
	 *          when you used the value completely, please delete the value ptr yourself.
	 */
	virtual void value(__lv_in const char *key, __lv_out char *&value, 
		__lv_out int &value_len) const = 0;

	/*
	 * get all values by the key
	 * @param   key   [in] key in config file
	 * @return  all values, if the value is empty, return empty vector
	 */
	virtual const std::vector<std::string> values(__lv_in const char *key) const = 0;

	/*
	 * get the count of the values which are pointed by the key
	 * @param   key   [in] key in config file
	 * @return  the count of the values
	 */
	virtual int count(__lv_in const char *key) const = 0;
};

class lava_config : public lava::base::noncopyable, 
					public i_lava_config
{
public:
	lava_config();
	~lava_config();

	virtual bool init(__lv_in const char *conf_file_name = nullptr) override;
	virtual bool fail() const override;
	virtual void value(__lv_in const char *key, __lv_out char *&value, 
		__lv_out int &value_len) const override;
	virtual const std::vector<std::string> values(__lv_in const char *key) const override;
	virtual int count(__lv_in const char *key) const override;

private:
	class ConfigImpl;
	ConfigImpl			*configs_;
};

}
}

#endif /* __LAVA_CONFIG_H__ */

