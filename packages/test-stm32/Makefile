DEPS += build-scripts
DEPS += hw-test-agent
DEPS += rlm3-hardware
DEPS += rlm3-base
DEPS += logger
include ../build-scripts/release/include.make

TOOLCHAIN_PATH = /opt/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-
CC = $(TOOLCHAIN_PATH)gcc
AS = $(TOOLCHAIN_PATH)gcc -x assembler-with-cpp
SZ = $(TOOLCHAIN_PATH)size
HX = $(TOOLCHAIN_PATH)objcopy -O ihex
BN = $(TOOLCHAIN_PATH)objcopy -O binary -S

MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
OPTIONS = -fdata-sections -ffunction-sections -fexceptions -Wall -Werror -DUSE_FULL_ASSERT=1

LIBRARIES = \
	-lc \
	-lm \
	-lstdc++

DEFINES = \
	-DUSE_HAL_DRIVER \
	-DSTM32F427xx \
	-DTEST
	
TEST_BUILD_DIR = $(BUILD_DIR)/test

TEST_SOURCE_DIR = test
TEST_SOURCE_DIRS = $(LIBRARY_DIR) $(TEST_SOURCE_DIR) $(PKG_RLM3_HARDWARE_DIR) $(PKG_RLM3_BASE_DIR) $(PKG_LOGGER_DIR)
TEST_SOURCE_FILES = $(notdir $(wildcard $(TEST_SOURCE_DIRS:%=%/*.c) $(TEST_SOURCE_DIRS:%=%/*.cpp) $(TEST_SOURCE_DIRS:%=%/*.s)))
TEST_O_FILES = $(addsuffix .o,$(basename $(TEST_SOURCE_FILES)))
TEST_LD_FILE = $(wildcard $(PKG_RLM3_HARDWARE_DIR)/*.ld)
TEST_INCLUDES = $(TEST_SOURCE_DIRS:%=-I%)

VPATH = $(TEST_SOURCE_DIRS)

.PHONY: test

default : test

test : $(TEST_BUILD_DIR)/test.bin $(TEST_BUILD_DIR)/test.hex
	$(PKG_HW_TEST_AGENT_DIR)/sr-hw-test-agent --run --test-timeout=15 --trace-frequency=2m --board RLM36 --file $(TEST_BUILD_DIR)/test.bin	

$(TEST_BUILD_DIR)/test.bin : $(TEST_BUILD_DIR)/test.elf
	$(BN) $< $@

$(TEST_BUILD_DIR)/test.hex : $(TEST_BUILD_DIR)/test.elf
	$(HX) $< $@

$(TEST_BUILD_DIR)/test.elf : $(TEST_O_FILES:%=$(TEST_BUILD_DIR)/%)
	$(CC) $(MCU) $(TEST_LD_FILE:%=-T%) -Wl,--gc-sections $^ $(LIBRARIES) -o $@ -Wl,-Map=$@.map,--cref
	$(SZ) $@

$(TEST_BUILD_DIR)/%.o : %.c Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(MCU) $(OPTIONS) $(DEFINES) $(TEST_INCLUDES) -MMD -g -Og -gdwarf-2 $< -o $@

$(TEST_BUILD_DIR)/%.o : %.cpp Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(MCU) $(OPTIONS) $(DEFINES) $(TEST_INCLUDES) -std=c++11 -MMD -g -Og -gdwarf-2 $< -o $@

$(TEST_BUILD_DIR)/%.o : %.s Makefile | $(TEST_BUILD_DIR)
	$(AS) -c $(MCU) $(OPTIONS) $(DEFINES) -MMD $< -o $@

$(TEST_BUILD_DIR) :
	mkdir -p $@

-include $(wildcard $(TEST_BUILD_DIR)/*.d)

