COMPILER = cc
LIB_OPTS = 
INCLUDE_PATHS =
OUT = -o out
CFILES = src/*.c
PLATFORM := $(shell uname)

ifeq ($(PLATFORM), Darwin)
	COMPILER = clang
endif

BUILD_CMD = $(COMPILER) $(INCLUDE_PATHS) $(CFILES) $(OUT) $(LIB_OPTS)

build:
	$(BUILD_CMD)

run:
	$(BUILD_CMD) && ./out && rm -rf ./out

clean:
	rm -rf ./out

