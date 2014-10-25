ps -ef|grep simple_log_test|grep -v grep|awk '{print $2}'|xargs kill
