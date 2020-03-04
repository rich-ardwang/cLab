#include <cstdio>
#include <iostream>
#include <thread>
#include <chrono>
#include "lava_config.h"

extern lava::utils::i_lava_config *get_lava_config();

int main()
{
	get_lava_config()->init();

	while (get_lava_config()->fail()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::cout << "wait..." << std::endl;
	}
	std::cout << "load config complete." << std::endl;

	char *conf_buf = nullptr;
	int conf_len = 0;
	get_lava_config()->value("http.server.addr", conf_buf, conf_len);
	std::cout << "http.server.addr:" << conf_buf << std::endl;
	get_lava_config()->value("http.server.port", conf_buf, conf_len);
	std::cout << "http.server.port:" << conf_buf << std::endl;
	get_lava_config()->value("http.server.root_path", conf_buf, conf_len);
	std::cout << "http.server.root_path:" << conf_buf << std::endl;
	get_lava_config()->value("http.server.name", conf_buf, conf_len);
	std::cout << "http.server.name:" << conf_buf << std::endl;
	get_lava_config()->value("http.server.pwd", conf_buf, conf_len);
	std::cout << "http.server.pwd:" << conf_buf << std::endl;
	if (conf_buf) {
		delete[] conf_buf;
		conf_buf = nullptr;
	}

    printf("hello from lava_config_test!\n");
    return 0;
}