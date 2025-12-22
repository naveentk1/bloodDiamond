.PHONY: all build clean test run install

all: build

build:
	mkdir -p build
	cd build && cmake .. && make

clean:
	rm -rf build

test: build
	cd build && ctest --output-on-failure

run: build
	./build/$(PROJECT_NAME) examples/test.src

install: build
	cd build && sudo make install

format:
	find src include -name "*.cpp" -o -name "*.h" | xargs clang-format -i
