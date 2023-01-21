DEPS += build-scripts
DEPS += hw-test-agent
DEPS += rlm-base
DEPS += assert
DEPS += logger
DEPS += rlm3-hardware
DEPS += test-stm32
include ../build-scripts/release/include.make

TOOLCHAIN_PATH = /opt/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-
CC = $(TOOLCHAIN_PATH)gcc
AS = $(TOOLCHAIN_PATH)gcc -x assembler-with-cpp
SZ = $(TOOLCHAIN_PATH)size
HX = $(TOOLCHAIN_PATH)objcopy -O ihex
BN = $(TOOLCHAIN_PATH)objcopy -O binary -S

MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
OPTIONS = -fdata-sections -ffunction-sections -fno-exceptions -Wall -Werror -DUSE_FULL_ASSERT=1
LIBRARIES = -lc -lm -lnosys

DEFINES = \
	-DUSE_HAL_DRIVER \
	-DSTM32F427xx \
	-DTEST \
	-DUSE_FULL_ASSERT=1
	

TEST_SOURCE_DIR = test
TEST_SOURCE_DIRS = $(LIBRARY_DIR) $(TEST_SOURCE_DIR) $(PKG_RLM3_HARDWARE_DIR) $(PKG_RLM_BASE_DIR) $(PKG_LOGGER_DIR) $(PKG_ASSERT_DIR) $(PKG_TEST_STM32_DIR)
TEST_SOURCE_FILES = $(notdir $(wildcard $(TEST_SOURCE_DIRS:%=%/*.c) $(TEST_SOURCE_DIRS:%=%/*.cpp) $(TEST_SOURCE_DIRS:%=%/*.s)))
TEST_BUILD_DIR = $(BUILD_DIR)/test
TEST_O_FILES = $(addsuffix .o,$(basename $(TEST_SOURCE_FILES)))
TEST_LD_FILE = $(wildcard $(PKG_RLM3_HARDWARE_DIR)/*.ld)

STRESS_SOURCE_DIR = stress
STRESS_SOURCE_DIRS = $(LIBRARY_DIR) $(STRESS_SOURCE_DIR) $(PKG_RLM3_HARDWARE_DIR) $(PKG_RLM_BASE_DIR) $(PKG_LOGGER_DIR) $(PKG_ASSERT_DIR) $(PKG_TEST_STM32_DIR)
STRESS_SOURCE_FILES = $(notdir $(wildcard $(STRESS_SOURCE_DIRS:%=%/*.c) $(STRESS_SOURCE_DIRS:%=%/*.cpp) $(STRESS_SOURCE_DIRS:%=%/*.s)))
STRESS_BUILD_DIR = $(BUILD_DIR)/stress
STRESS_O_FILES = $(addsuffix .o,$(basename $(STRESS_SOURCE_FILES)))
STRESS_LD_FILE = $(wildcard $(PKG_RLM3_HARDWARE_DIR)/*.ld)

VPATH = $(TEST_SOURCE_DIRS) $(STRESS_SOURCE_DIRS)


.PHONY: test stress

default : test

test : deps $(TEST_BUILD_DIR)/test.bin $(TEST_BUILD_DIR)/test.hex
	$(PKG_HW_TEST_AGENT_DIR)/sr-hw-test-agent --run --test-timeout=15 --system-frequency=180m --trace-frequency=2m --board RLM36 --file $(TEST_BUILD_DIR)/test.bin	

$(TEST_BUILD_DIR)/test.bin : $(TEST_BUILD_DIR)/test.elf
	$(BN) $< $@

$(TEST_BUILD_DIR)/test.hex : $(TEST_BUILD_DIR)/test.elf
	$(HX) $< $@

$(TEST_BUILD_DIR)/test.elf : $(TEST_O_FILES:%=$(TEST_BUILD_DIR)/%)
	$(CC) $(MCU) $(TEST_LD_FILE:%=-T%) -Wl,--gc-sections $^ $(LIBRARIES) -o $@ -Wl,-Map=$@.map,--cref
	$(SZ) $@

$(TEST_BUILD_DIR)/%.o : %.c Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(MCU) $(OPTIONS) $(DEFINES) $(TEST_SOURCE_DIRS:%=-I%) -MMD -g -Og -gdwarf-2 $< -o $@

$(TEST_BUILD_DIR)/%.o : %.cpp Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(MCU) $(OPTIONS) $(DEFINES) $(TEST_SOURCE_DIRS:%=-I%) -std=c++11 -MMD -g -Og -gdwarf-2 $< -o $@

$(TEST_BUILD_DIR)/%.o : %.s Makefile | $(TEST_BUILD_DIR)
	$(AS) -c $(MCU) $(OPTIONS) $(DEFINES) -MMD $< -o $@

$(TEST_BUILD_DIR) :
	mkdir -p $@

stress : deps $(STRESS_BUILD_DIR)/stress.bin $(STRESS_BUILD_DIR)/stress.hex
	$(PKG_HW_TEST_AGENT_DIR)/sr-hw-test-agent --run --test-timeout=330 --system-frequency=180m --trace-frequency=2m --board RLM36 --file $(STRESS_BUILD_DIR)/stress.bin	

$(STRESS_BUILD_DIR)/stress.bin : $(STRESS_BUILD_DIR)/stress.elf
	$(BN) $< $@

$(STRESS_BUILD_DIR)/stress.hex : $(STRESS_BUILD_DIR)/stress.elf
	$(HX) $< $@

$(STRESS_BUILD_DIR)/stress.elf : $(STRESS_O_FILES:%=$(STRESS_BUILD_DIR)/%)
	$(CC) $(MCU) $(STRESS_LD_FILE:%=-T%) -Wl,--gc-sections $^ $(LIBRARIES) -o $@ -Wl,-Map=$@.map,--cref
	$(SZ) $@

$(STRESS_BUILD_DIR)/%.o : %.c Makefile | $(STRESS_BUILD_DIR)
	$(CC) -c $(MCU) $(OPTIONS) $(DEFINES) $(STRESS_SOURCE_DIRS:%=-I%) -MMD -g -Og -gdwarf-2 $< -o $@

$(STRESS_BUILD_DIR)/%.o : %.cpp Makefile | $(STRESS_BUILD_DIR)
	$(CC) -c $(MCU) $(OPTIONS) $(DEFINES) $(STRESS_SOURCE_DIRS:%=-I%) -std=c++11 -MMD -g -Og -gdwarf-2 $< -o $@

$(STRESS_BUILD_DIR)/%.o : %.s Makefile | $(STRESS_BUILD_DIR)
	$(AS) -c $(MCU) $(OPTIONS) $(DEFINES) -MMD $< -o $@

$(STRESS_BUILD_DIR) :
	mkdir -p $@

-include $(wildcard $(TEST_BUILD_DIR)/*.d $(STRESS_BUILD_DIR)/*.d)


