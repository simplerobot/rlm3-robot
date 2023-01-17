DEPS += build-scripts
DEPS += logger
DEPS += test
include ../build-scripts/release/include.make

TEST_BUILD_DIR = $(BUILD_DIR)/test
MAIN_BUILD_DIR = $(BUILD_DIR)/main

TEST_SOURCE_DIRS = $(LIBRARY_DIR) test main $(PKG_LOGGER_DIR) $(PKG_TEST_DIR)
TEST_SOURCE_FILES = $(notdir $(wildcard $(TEST_SOURCE_DIRS:%=%/*.cpp) $(TEST_SOURCE_DIRS:%=%/*.c)))
TEST_O_FILES = $(addsuffix .o,$(basename $(TEST_SOURCE_FILES)))

MAIN_SOURCE_DIRS = $(LIBRARY_DIR) main $(PKG_LOGGER_DIR)
MAIN_SOURCE_FILES = $(notdir $(wildcard $(MAIN_SOURCE_DIRS:%=%/*.cpp) $(MAIN_SOURCE_DIRS:%=%/*.c)))
MAIN_O_FILES = $(addsuffix .o,$(basename $(MAIN_SOURCE_FILES)))

RESOURCE_DIR = resources

CC = g++
CFLAGS = -Wall -Werror -pthread -MMD
TEST_CFLAGS = $(CFLAGS) $(TEST_SOURCE_DIRS:%=-I%) -DTEST -fsanitize=address -static-libasan -g -Og
MAIN_CFLAGS = $(CFLAGS) $(MAIN_SOURCE_DIRS:%=-I%) -O3

VPATH = $(TEST_SOURCE_DIRS) $(MAIN_SOURCE_DIRS)

.PHONY: test main release

default : test main release

test : deps $(TEST_BUILD_DIR)/a.out
	$(TEST_BUILD_DIR)/a.out

$(TEST_BUILD_DIR)/a.out : $(TEST_O_FILES:%=$(TEST_BUILD_DIR)/%)
	$(CC) $(TEST_CFLAGS) -o $@ $^

$(TEST_BUILD_DIR)/%.o : %.c Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(TEST_CFLAGS) -o $@ $<

$(TEST_BUILD_DIR)/%.o : %.cpp Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(TEST_CFLAGS) -o $@ $<

$(TEST_BUILD_DIR) :
	mkdir -p $@

main : build-deps $(MAIN_BUILD_DIR)/a.out

$(MAIN_BUILD_DIR)/a.out : $(MAIN_O_FILES:%=$(MAIN_BUILD_DIR)/%)
	$(CC) $(MAIN_CFLAGS) -o $@ $^

$(MAIN_BUILD_DIR)/%.o : %.cpp Makefile | $(MAIN_BUILD_DIR)
	$(CC) -c $(MAIN_CFLAGS) -o $@ $<

$(MAIN_BUILD_DIR)/%.o : %.c Makefile | $(MAIN_BUILD_DIR)
	$(CC) -c $(MAIN_CFLAGS) -o $@ $<

$(MAIN_BUILD_DIR) :
	mkdir -p $@

release : $(RELEASE_DIR)/sr-hw-test-agent

$(RELEASE_DIR)/sr-hw-test-agent : $(MAIN_BUILD_DIR)/a.out | $(RELEASE_DIR)
	cp $< $@

$(RELEASE_DIR) :
	mkdir -p $@

.PHONY : install-service service-user

install-service :
	if [ ! -f $(RELEASE_DIR)/sr-hw-test-agent ] ; then echo "Project not built."; /bin/false; fi
	- systemctl stop sr-hw-test-agent
	cp $(RELEASE_DIR)/sr-hw-test-agent /usr/local/bin/sr-hw-test-agent
	id -u sr-hardware-test || useradd -r sr-hardware-test
	cp $(RESOURCE_DIR)/sr-hw-test-agent.service /etc/systemd/system/sr-hw-test-agent.service
	systemctl daemon-reload
	systemctl enable sr-hw-test-agent
	systemctl start sr-hw-test-agent

uninstall-service :
	systemctl stop sr-hw-test-agent
	systemctl disable sr-hw-test-agent
	rm /etc/systemd/system/sr-hw-test-agent.service
	systemctl daemon-reload
	systemctl reset-failed
	rm /usr/local/bin/sr-hw-test-agent

-include $(wildcard $(TEST_BUILD_DIR)/*.d)
-include $(wildcard $(MAIN_BUILD_DIR)/*.d)

