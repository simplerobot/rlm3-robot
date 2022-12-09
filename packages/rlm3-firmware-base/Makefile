DEPS += build-scripts
DEPS += logger
DEPS += test
DEPS += rlm3-base
DEPS += rlm3-driver-base-sim
DEPS += rlm3-driver-flash-sim
include ../build-scripts/release/include.make

CPU_CC = g++
CPU_CFLAGS = -Wall -Werror -pthread -DTEST -fsanitize=address -static-libasan -g -Og

CPU_TEST_BUILD_DIR = $(BUILD_DIR)/test-cpu
CPU_TEST_SOURCE_DIR = test-cpu
CPU_TEST_SOURCE_DIRS = $(LIBRARY_DIR) $(CPU_TEST_SOURCE_DIR) $(PKG_LOGGER_DIR) $(PKG_TEST_DIR) $(PKG_RLM3_BASE_DIR) $(PKG_RLM3_DRIVER_BASE_SIM_DIR) $(PKG_RLM3_DRIVER_FLASH_SIM_DIR)
CPU_TEST_SOURCE_FILES = $(notdir $(wildcard $(CPU_TEST_SOURCE_DIRS:%=%/*.c) $(CPU_TEST_SOURCE_DIRS:%=%/*.cpp)))
CPU_TEST_O_FILES = $(addsuffix .o,$(basename $(CPU_TEST_SOURCE_FILES)))
CPU_INCLUDES = $(CPU_TEST_SOURCE_DIRS:%=-I%)

VPATH = $(CPU_TEST_SOURCE_DIRS)

.PHONY: test-cpu

default : test

test : test-cpu

test-cpu : deps $(CPU_TEST_BUILD_DIR)/a.out
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
