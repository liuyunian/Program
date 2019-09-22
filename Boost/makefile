BUILD_DIR = ./build

all: build
	cd $(BUILD_DIR); make

build:
	mkdir $(BUILD_DIR); cd $(BUILD_DIR); cmake ..

clean:
	-$(RM) -r $(BUILD_DIR)