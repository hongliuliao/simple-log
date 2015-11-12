## simple_log是一个轻量的c++版本日志组件
## 特点

  * 简洁但是功能实用
  * 只支持linux
  * 线程不安全

## 功能列表
  * 支持动态加载配置文件,修改日志等级
  * 支持使用脚本进行日志文件的切分

## 性能
  * 10万/s 日志写入

## 构建 && 测试

```
  make && make test
```

## 配置文件
 * 默认会找conf/simple_log.conf,如果没有找到,会将日志以debug级别输出到控制台
 * 如果配置了log_file,需要保证log目录已经存在
```
 log_level=INFO
 log_file=log/simple.log
```

## 例子:
```c++

#include "simple_log.h"

int main() {
    int ret = log_init();
    if (ret != 0) {
    	printf("log_init error!\n");
	return 1;
    }
    LOG_INFO("%s", "this is a info log");
    return 0;
}
```

## 编译
```
 g++ -I bin/include test/simple_log_test.cpp bin/lib/libsimplelog.a -o bin/simple_log_test
```

## 输出
```
 2014-10-25 15:43:29.216 INFO test/simple_log_test.cpp(5): this is a info log
```
