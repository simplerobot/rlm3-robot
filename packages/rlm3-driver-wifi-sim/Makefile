DEPS += build-scripts
DEPS += logger
DEPS += test
DEPS += rlm3-driver-base-sim
include ../build-scripts/release/include.make

CC = g++
CFLAGS = -Wall -Werror -DTEST -fsanitize=address -static-libasan -g -Og

TEST_BUILD_DIR = $(BUILD_DIR)/test
TEST_SOURCE_DIR = test
TEST_SOURCE_DIRS = $(LIBRARY_DIR) $(TEST_SOURCE_DIR) $(PKG_LOGGER_DIR)  $(PKG_TEST_DIR) $(PKG_RLM3_DRIVER_BASE_SIM_DIR)
TEST_SOURCE_FILES = $(notdir $(wildcard $(TEST_SOURCE_DIRS:%=%/*.cpp) $(TEST_SOURCE_DIRS:%=%/*.c)))
TEST_O_FILES = $(addsuffix .o,$(basename $(TEST_SOURCE_FILES)))
TEST_INCLUDES = $(TEST_SOURCE_DIRS:%=-I%)

VPATH = $(TEST_SOURCE_DIRS) 

.PHONY: test

default : test

test : $(TEST_BUILD_DIR)/a.out
	$(TEST_BUILD_DIR)/a.out

$(TEST_BUILD_DIR)/a.out : $(TEST_O_FILES:%=$(TEST_BUILD_DIR)/%)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_BUILD_DIR)/%.o : %.cpp Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(CFLAGS) $(TEST_INCLUDES) -MMD -o $@ $<

$(TEST_BUILD_DIR)/%.o : %.c Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(CFLAGS) $(TEST_INCLUDES) -MMD -o $@ $<

$(TEST_BUILD_DIR) :
	mkdir -p $@

-include $(wildcard $(TEST_BUILD_DIR)/*.d)
