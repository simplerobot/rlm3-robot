DEPS += build-scripts
DEPS += logger
DEPS += test
include ../build-scripts/release/include.make

CPU_CC = g++
CPU_CFLAGS = -Wall -Werror -pthread -DTEST -DMOCK_COMMS -fsanitize=address -static-libasan -g -Og

CPU_TEST_BUILD_DIR = $(BUILD_DIR)/test
CPU_TEST_SOURCE_DIR = test
CPU_TEST_SOURCE_DIRS = $(LIBRARY_DIR) $(CPU_TEST_SOURCE_DIR) $(PKG_LOGGER_DIR) $(PKG_TEST_DIR)
CPU_TEST_SOURCE_FILES = $(notdir $(wildcard $(CPU_TEST_SOURCE_DIRS:%=%/*.c) $(CPU_TEST_SOURCE_DIRS:%=%/*.cpp)))
CPU_TEST_O_FILES = $(addsuffix .o,$(basename $(CPU_TEST_SOURCE_FILES)))
CPU_INCLUDES = $(CPU_TEST_SOURCE_DIRS:%=-I%)

VPATH = $(CPU_TEST_SOURCE_DIRS)

.PHONY: test

default : test

test : deps $(CPU_TEST_BUILD_DIR)/a.out
	$(CPU_TEST_BUILD_DIR)/a.out

$(CPU_TEST_BUILD_DIR)/a.out : $(CPU_TEST_O_FILES:%=$(CPU_TEST_BUILD_DIR)/%)
	$(CPU_CC) $(CPU_CFLAGS) $^ -o $@

$(CPU_TEST_BUILD_DIR)/%.o : %.cpp Makefile | $(CPU_TEST_BUILD_DIR)
	$(CPU_CC) -c $(CPU_CFLAGS) $(CPU_INCLUDES) -MMD $< -o $@
	
$(CPU_TEST_BUILD_DIR)/%.o : %.c Makefile | $(CPU_TEST_BUILD_DIR)
	$(CPU_CC) -c $(CPU_CFLAGS) $(CPU_INCLUDES) -MMD $< -o $@

$(CPU_TEST_BUILD_DIR) :
	mkdir -p $@

-include $(wildcard $(CPU_TEST_BUILD_DIR)/*.d)

