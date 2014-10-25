all:
	echo "make all"
	mkdir -p bin/include
	mkdir -p bin/lib
	g++ -c src/simple_log.cpp -o bin/simple_log.o
	ar -rcs libsimplelog.a bin/simple_log.o
	
	cp src/simple_log.h bin/include/
	mv libsimplelog.a bin/lib/
	
test: src/simple_log.cpp test/simple_log_test.cpp
	g++ -I bin/include test/simple_log_test.cpp bin/lib/libsimplelog.a -o bin/simple_log_test
	bin/simple_log_test