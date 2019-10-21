BUILD_DIR = ./build
RM = rm -rf

all: build
	cd $(BUILD_DIR); make

build:
	mkdir $(BUILD_DIR); cd $(BUILD_DIR); cmake -D CMAKE_CXX_COMPILER=/usr/bin/g++ ..

clean:
	-$(RM) $(BUILD_DIR)