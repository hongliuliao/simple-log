## simple_log是一个轻量的c++版本日志组件
## 特点

  * 简洁但是功能实用
  * 线程安全

## 功能列表
  * 支持动态加载配置文件,修改日志等级 (kill -USR1 <pid>)
  * Auto shift file log by daily
  * Auto delete old log file if you need

## 性能
  * 10万/s 日志写入

## 配置文件
 * 如果没有找到,会将日志以debug级别输出到控制台
```
 log_level=INFO
 log_dir=log
 log_file=simple.log
 retain_day=7
```

## 例子:
```c++

#include "simple_log.h"

int main() {
    int ret = log_init("conf", "simple_log.conf");
    if (ret != 0) {
    	printf("log_init error!\n");
        return 1;
    }
    LOG_INFO("%s", "this is a info log");
    return 0;
}
```

## build && test
```
 make && make test && ./output/bin/simple_log_test
```

## 输出
```
 2014-10-25 15:43:29.216 INFO test/simple_log_test.cpp(5): this is a info log
```
