DEPS += build-scripts
DEPS += test
include ../build-scripts/release/include.make

TEST_BUILD_DIR = $(BUILD_DIR)/test

TEST_SOURCE_DIRS = $(LIBRARY_DIR) test $(PKG_TEST_DIR)
TEST_SOURCE_FILES = $(notdir $(wildcard $(TEST_SOURCE_DIRS:%=%/*.cpp) $(TEST_SOURCE_DIRS:%=%/*.c)))
TEST_O_FILES = $(addsuffix .o,$(basename $(TEST_SOURCE_FILES)))

CC = g++
CFLAGS = -Wall -Werror -DTEST $(TEST_SOURCE_DIRS:%=-I%) -fsanitize=address -static-libasan -g -Og

VPATH = $(TEST_SOURCE_DIRS)

.PHONY: test

default : test

test : $(TEST_BUILD_DIR)/a.out
	$(TEST_BUILD_DIR)/a.out

$(TEST_BUILD_DIR)/a.out : $(TEST_O_FILES:%=$(TEST_BUILD_DIR)/%)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_BUILD_DIR)/%.o : %.c Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(CFLAGS) -MMD -o $@ $<

$(TEST_BUILD_DIR)/%.o : %.cpp Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(CFLAGS) -MMD -o $@ $<

$(TEST_BUILD_DIR) :
	mkdir -p $@

-include $(wildcard $(TEST_BUILD_DIR)/*.d)

