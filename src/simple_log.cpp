#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "stdarg.h"

#include "simple_log.h"

// log context
char log_config_file[100] = "conf/simple_log.conf";
const int load_cycle = 3;
int last_load_sec = 0;
time_t config_file_last_modify = 0;
bool is_load_config = false;
const int max_single_log_size = 2048;
char single_log[max_single_log_size];

// log config
int log_level = DEBUG_LEVEL;
std::string log_file;

void init_log_config(char *config_file) {
	strcpy(log_config_file, config_file);
}

std::string _get_show_time() {
	char show_time[40];
	memset(show_time, 0, 40);

	struct timeval tv;
	gettimeofday(&tv, NULL);

	struct tm *tm;
	tm = localtime(&tv.tv_sec);

	sprintf(show_time, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec/1000));
	return std::string(show_time);
}

int _get_log_level(const char *level_str) {
	if(strcmp(level_str, "ERROR") == 0) {
		return ERROR_LEVEL;
	}
	if(strcmp(level_str, "WARN") == 0) {
		return WARN_LEVEL;
	}
	if(strcmp(level_str, "INFO") == 0) {
		return INFO_LEVEL;
	}
	if(strcmp(level_str, "DEBUG") == 0) {
		return DEBUG_LEVEL;
	}
	return DEBUG_LEVEL;
}

std::map<std::string, std::string> _get_config_map() {
	std::map<std::string, std::string> result;

	std::fstream fs(log_config_file);
	if(!fs.is_open()) {
		return result;
	}

	while(fs.good()) {
		std::string line;
		std::getline(fs, line);

		std::stringstream ss;
		ss << line;
		std::string key, value;
		std::getline(ss, key, '=');
		std::getline(ss, value, '=');

		result[key] = value;
	}
	fs.close();
	return result;
}

void _check_config_file() {
	// check config file
	time_t now;
	time(&now);
	if(now - last_load_sec < load_cycle) {
		return;
	}

	last_load_sec = now;

	std::map<std::string, std::string> configs = _get_config_map();
	if(!configs.empty()) {
		is_load_config = true;
		// read log level
		std::string log_level_str = configs["log_level"];
		log_level = _get_log_level(log_level_str.c_str());

		// read log file
		log_file = configs["log_file"];
	}
}

void _log(const char *format, va_list ap) {
	if(!is_load_config) { // if no config, send log to stdout
		vprintf(format, ap);
		printf("\n");
		return;
	}

	std::fstream fs(log_file.c_str(), std::fstream::out | std::fstream::app);
	if(fs.is_open()) {
		vsprintf(single_log, format, ap);
		fs << single_log << "\n";
		fs.close();
	}
}

void log_error(const char *format, ...) {
	_check_config_file();
	if(log_level < ERROR_LEVEL) {
		return;
	}

	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}

void log_warn(const char *format, ...) {
	_check_config_file();
	if(log_level < WARN_LEVEL) {
		return;
	}

	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}

void log_info(const char *format, ...) {
	_check_config_file();
	if(log_level < INFO_LEVEL) {
		return;
	}

	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}

void log_debug(const char *format, ...) {
	_check_config_file();
	if(log_level < DEBUG_LEVEL) {
		return;
	}

	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}
