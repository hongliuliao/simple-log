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
#include <signal.h> 
#include <errno.h>  

#include "simple_config.h"
#include "simple_log.h"

// log context
const int max_single_log_size = 2048;
char single_log[max_single_log_size];

int log_level = DEBUG_LEVEL;
std::string g_dir;
std::string g_config_file;
bool use_file_appender = false;
FileAppender g_file_appender;

FileAppender::FileAppender() {
    _is_inited = false;
}
FileAppender::~FileAppender() {
    if (_fs.is_open()) {
        _fs.close();
    }
}
int FileAppender::init(std::string dir, std::string log_file) {
    bzero(&_last_tm, sizeof(_last_tm));
    if (!dir.empty()) {
        int ret = mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0 && errno != EEXIST) {
            printf("mkdir error which dir:%s err:%s\n", dir.c_str(), strerror(errno));
            return -1;
        }
    } else {
        dir = "."; // current dir
    }
    _log_dir = dir;
    _log_file = log_file;
    _log_file_path = dir + "/" + log_file;
    _fs.open(_log_file_path.c_str(), std::fstream::out | std::fstream::app);
    _is_inited = true;
    return 0;
}

int FileAppender::write_log(char *log, const char *format, va_list ap) {
    if (_fs.is_open()) {
        vsprintf(single_log, format, ap);
        _fs << single_log << "\n";
    }
    return 0;
}

int FileAppender::shift_file_if_need() {
    if (_last_tm.tm_year == 0) {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        struct tm *tm;
        tm = localtime(&tv.tv_sec);
        _last_tm = *tm;
        return 0;
    }
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm *tm;
    tm = localtime(&tv.tv_sec);
    if (_last_tm.tm_year != tm->tm_year ||
            _last_tm.tm_mon != tm->tm_mon ||
            _last_tm.tm_mday != tm->tm_mday) {
        _fs.close();    
        char new_file[100];
        memset(new_file, 0, 100);
        sprintf(new_file, "%s.%04d-%02d-%02d",
                _log_file.c_str(), tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
        std::string new_file_path = _log_dir + "/" + new_file;
        rename(_log_file_path.c_str(), new_file_path.c_str());
        _fs.open(_log_file_path.c_str(), std::fstream::out | std::fstream::app);
    }
    _last_tm = *tm;
    return 0; 
}

bool FileAppender::is_inited() {
    return _is_inited;
}

int _check_config_file() {
	std::map<std::string, std::string> configs;
    std::string log_config_file = g_dir + "/" + g_config_file;
	get_config_map(log_config_file.c_str(), configs);
	if (configs.empty()) {
        return 0;
    }
    // read log level
    std::string log_level_str = configs["log_level"];
    set_log_level(log_level_str.c_str());

    // read log file
    std::string dir = configs["log_dir"];
    std::string log_file = configs["log_file"];
    if (!log_file.empty()) {
        use_file_appender = true;
        if (!g_file_appender.is_inited()) {
            g_file_appender.init(dir, log_file);
        }
    }
    return 0;
}

void sigreload(int sig) {
    //printf("receive sig:%d \n", sig);
    _check_config_file();
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
	if(strcasecmp(level_str, "ERROR") == 0) {
		return ERROR_LEVEL;
	}
	if(strcasecmp(level_str, "WARN") == 0) {
		return WARN_LEVEL;
	}
	if(strcasecmp(level_str, "INFO") == 0) {
		return INFO_LEVEL;
	}
	if(strcasecmp(level_str, "DEBUG") == 0) {
		return DEBUG_LEVEL;
	}
	return DEBUG_LEVEL;
}

void set_log_level(const char *level) {
    log_level = _get_log_level(level);
}

void _log(const char *format, va_list ap) {
	if (!use_file_appender) { // if no config, send log to stdout
		vprintf(format, ap);
		printf("\n");
		return;
	}
    g_file_appender.shift_file_if_need();
    g_file_appender.write_log(single_log, format, ap);
}

int log_init(std::string dir, std::string file) {
    g_dir = dir;
    g_config_file = file;
    signal(SIGUSR1, sigreload);
    return _check_config_file();
}

void log_error(const char *format, ...) {
	if (log_level < ERROR_LEVEL) {
		return;
	}

	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}

void log_warn(const char *format, ...) {
	if (log_level < WARN_LEVEL) {
		return;
	}

	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}

void log_info(const char *format, ...) {
	if (log_level < INFO_LEVEL) {
		return;
	}

	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}

void log_debug(const char *format, ...) {
	if (log_level < DEBUG_LEVEL) {
		return;
	}

	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}
