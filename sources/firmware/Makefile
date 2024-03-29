DEPS += build-scripts
DEPS += assert
DEPS += logger
DEPS += test
DEPS += rlm-base
DEPS += hash
DEPS += rlm3-hardware
DEPS += rlm3-driver-base
DEPS += rlm3-driver-base-sim
DEPS += rlm3-driver-flash
DEPS += rlm3-driver-flash-sim
DEPS += rlm3-driver-wifi
DEPS += rlm3-driver-wifi-sim
DEPS += firmware-base
DEPS += hw-test-agent
include ../build-scripts/release/include.make

CPU_CC = g++
CPU_CFLAGS = -Wall -Werror -pthread -DTEST -fsanitize=address -static-libasan -g -Og

MCU_TOOLCHAIN_PATH = /opt/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-
MCU_CC = $(MCU_TOOLCHAIN_PATH)gcc
MCU_AS = $(MCU_TOOLCHAIN_PATH)gcc -x assembler-with-cpp
MCU_SZ = $(MCU_TOOLCHAIN_PATH)size
MCU_HX = $(MCU_TOOLCHAIN_PATH)objcopy -O ihex
MCU_BN = $(MCU_TOOLCHAIN_PATH)objcopy -O binary -S

MCU_CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fdata-sections -ffunction-sections -fno-exceptions -Wall -Werror -DUSE_HAL_DRIVER -DSTM32F427xx -DUSE_FULL_ASSERT=1 -g -Og -gdwarf-2
MCU_CLIBS = -lc -lm -lnosys

CPU_TEST_CASE_FILES = $(wildcard test-cpu/*-Tests.cpp)
CPU_TEST_CASES = $(CPU_TEST_CASE_FILES:test-cpu/%-Tests.cpp=%)
CPU_TEST_BUILD_DIR = $(BUILD_DIR)/test-cpu
CPU_TEST_SOURCE_DIRS = $(PKG_ASSERT_DIR) $(PKG_TEST_DIR) $(PKG_LOGGER_DIR) $(PKG_RLM_BASE_DIR) $(PKG_HASH_DIR) $(PKG_RLM3_DRIVER_BASE_SIM_DIR) $(PKG_RLM3_DRIVER_FLASH_SIM_DIR) $(PKG_RLM3_DRIVER_WIFI_SIM_DIR) $(PKG_FIRMWARE_BASE_DIR)
CPU_TEST_SOURCE_FILES = $(notdir $(wildcard $(CPU_TEST_SOURCE_DIRS:%=%/*.c) $(CPU_TEST_SOURCE_DIRS:%=%/*.cpp)))
CPU_TEST_O_FILES = $(addsuffix .o,$(basename $(CPU_TEST_SOURCE_FILES)))
CPU_INCLUDES = $(CPU_TEST_SOURCE_DIRS:%=-I%) -I main

MCU_BUILD_DIR = $(BUILD_DIR)/release
MCU_SOURCE_DIRS = main $(PKG_RLM3_HARDWARE_DIR) $(PKG_RLM_BASE_DIR) $(PKG_ASSERT_DIR) $(PKG_LOGGER_DIR) $(PKG_TEST_STM32_DIR) $(PKG_HASH_DIR) $(PKG_RLM3_DRIVER_BASE_DIR) $(PKG_RLM3_DRIVER_FLASH_DIR) $(PKG_RLM3_DRIVER_WIFI_DIR) $(PKG_FIRMWARE_BASE_DIR)
MCU_SOURCE_FILES = $(notdir $(wildcard $(MCU_SOURCE_DIRS:%=%/*.c) $(MCU_SOURCE_DIRS:%=%/*.cpp) $(MCU_SOURCE_DIRS:%=%/*.s)))
MCU_O_FILES = $(addsuffix .o,$(basename $(MCU_SOURCE_FILES)))
MCU_LD_FILE = $(wildcard $(PKG_RLM3_HARDWARE_DIR)/*.ld)
MCU_INCLUDES = $(MCU_SOURCE_DIRS:%=-I%)

VPATH = $(MCU_SOURCE_DIRS) $(CPU_TEST_SOURCE_DIRS) main test-cpu

.PHONY: test-cpu main-mcu

default : test release

test : test-cpu

release : main-mcu

test-cpu : deps $(CPU_TEST_CASES:%=test-cpu-%)

test-cpu-% : $(CPU_TEST_BUILD_DIR)/a-%.out
	$<
	
$(CPU_TEST_BUILD_DIR)/a-%.out : $(CPU_TEST_O_FILES:%=$(CPU_TEST_BUILD_DIR)/%) $(CPU_TEST_BUILD_DIR)/%.o $(CPU_TEST_BUILD_DIR)/%-Tests.o
	$(CPU_CC) $(CPU_CFLAGS) $^ -o $@

$(CPU_TEST_BUILD_DIR)/%.o : %.cpp Makefile | $(CPU_TEST_BUILD_DIR)
	$(CPU_CC) -c $(CPU_CFLAGS) $(CPU_INCLUDES) -MMD $< -o $@
	
$(CPU_TEST_BUILD_DIR)/%.o : %.c Makefile | $(CPU_TEST_BUILD_DIR)
	$(CPU_CC) -c $(CPU_CFLAGS) $(CPU_INCLUDES) -MMD $< -o $@

$(CPU_TEST_BUILD_DIR) :
	mkdir -p $@

main-mcu : deps $(MCU_BUILD_DIR)/test.bin $(MCU_BUILD_DIR)/test.hex
	$(PKG_HW_TEST_AGENT_DIR)/sr-hw-test-agent --run --test-timeout=60 --system-frequency=180m --trace-frequency=2m --board RLM36 --file $(MCU_BUILD_DIR)/test.bin

$(MCU_BUILD_DIR)/test.bin : $(MCU_BUILD_DIR)/test.elf
	$(MCU_BN) $< $@

$(MCU_BUILD_DIR)/test.hex : $(MCU_BUILD_DIR)/test.elf
	$(MCU_HX) $< $@

$(MCU_BUILD_DIR)/test.elf : $(MCU_O_FILES:%=$(MCU_BUILD_DIR)/%)
	$(MCU_CC) $(MCU_CFLAGS) $(MCU_LD_FILE:%=-T%) -Wl,--gc-sections $^ $(MCU_CLIBS) -o $@ -Wl,-Map=$@.map,--cref
	$(MCU_SZ) $@

$(MCU_BUILD_DIR)/%.o : %.c Makefile | $(MCU_BUILD_DIR)
	$(MCU_CC) -c $(MCU_CFLAGS) $(MCU_INCLUDES) -MMD $< -o $@

$(MCU_BUILD_DIR)/%.o : %.cpp Makefile | $(MCU_BUILD_DIR)
	$(MCU_CC) -c $(MCU_CFLAGS) $(MCU_INCLUDES) -MMD -std=c++11 $< -o $@

$(MCU_BUILD_DIR)/%.o : %.s Makefile | $(MCU_BUILD_DIR)
	$(MCU_AS) -c $(MCU_CFLAGS) -MMD $< -o $@

$(MCU_BUILD_DIR) :
	mkdir -p $@

-include $(wildcard $(CPU_TEST_BUILD_DIR)/*.d $(MCU_BUILD_DIR)/*.d)

