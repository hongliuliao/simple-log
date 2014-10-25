simple_log
这是一个c++版本的日志组件
## 特点

  * 简洁但是功能实用
  * 只支持linux
  * 线程不安全

## 功能列表
  * 支持动态加载配置文件,修改日志等级
  * 支持使用脚本进行日志文件的切分

## 性能
  * 10万/s 日志写入

## 例子:
```c++

#include "simple_log.h"

int main(int argc, char **argv) {
  // 默认会找conf/simple_log.conf,如果没有找到,会将日志输出到控制台
  // init_log_config("conf/simple_log.conf"); 
  LOG_INFO("%s", "this is a info log");
  return 0;
}
```
