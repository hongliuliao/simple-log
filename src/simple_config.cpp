/*
 * simple_config.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: liao
 */
#include <fstream>
#include <sstream>
#include "simple_config.h"

int get_config_map(char *config_file, std::map<std::string, std::string> &configs) {
    std::fstream fs(config_file);
    if(!fs.is_open()) {
        return -1;
    }

    while(fs.good()) {
        std::string line;
        std::getline(fs, line);

        std::stringstream ss;
        ss << line;
        std::string key, value;
        std::getline(ss, key, '=');
        std::getline(ss, value, '=');

        configs[key] = value;
    }
    fs.close();
    return 0;
}
