cmake:
	mkdir -p bin
	cd bin && cmake -DTESTS_ENABLED=1 -DCMAKE_BUILD_TYPE=Debug .. && cd ..

build: cmake
	cd bin && make && cd ..

test: build
	./bin/c_utils_test
