DEPS += build-scripts
DEPS += logger
DEPS += hw-test-agent
include ../build-scripts/release/include.make

TOOLCHAIN_PATH = /opt/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-

LIBRARY_DIRS = \
	cube/Core/Inc \
	cube/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	cube/Drivers/CMSIS/Include \
	cube/Drivers/STM32F4xx_HAL_Driver/Inc \
	cube/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
	cube/Middlewares/Third_Party/FreeRTOS/Source/include \
	cube/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F \
	cube/Core/Src \
	cube/Drivers/STM32F4xx_HAL_Driver/Src \
	cube/Middlewares/Third_Party/FreeRTOS/Source \
	cube/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang \
	cube \

LIBRARY_H_FILES = \
	FreeRTOSConfig.h \
	main.h \
	stm32f4xx_hal_conf.h \
	stm32f4xx_it.h \
	stm32f427xx.h \
	stm32f4xx.h \
	system_stm32f4xx.h \
	cmsis_compiler.h \
	cmsis_gcc.h \
	cmsis_version.h \
	core_cm4.h \
	mpu_armv7.h \
	Legacy/stm32_hal_legacy.h \
	stm32f4xx_hal.h \
	stm32f4xx_hal_adc.h \
	stm32f4xx_hal_adc_ex.h \
	stm32f4xx_hal_cortex.h \
	stm32f4xx_hal_dcmi.h \
	stm32f4xx_hal_dcmi_ex.h \
	stm32f4xx_hal_def.h \
	stm32f4xx_hal_dma.h \
	stm32f4xx_hal_dma_ex.h \
	stm32f4xx_hal_exti.h \
	stm32f4xx_hal_flash.h \
	stm32f4xx_hal_flash_ex.h \
	stm32f4xx_hal_flash_ramfunc.h \
	stm32f4xx_hal_gpio.h \
	stm32f4xx_hal_gpio_ex.h \
	stm32f4xx_hal_i2c.h \
	stm32f4xx_hal_i2c_ex.h \
	stm32f4xx_hal_pwr.h \
	stm32f4xx_hal_pwr_ex.h \
	stm32f4xx_hal_rcc.h \
	stm32f4xx_hal_rcc_ex.h \
	stm32f4xx_hal_sdram.h \
	stm32f4xx_hal_tim.h \
	stm32f4xx_hal_tim_ex.h \
	stm32f4xx_hal_uart.h \
	stm32f4xx_ll_adc.h \
	stm32f4xx_ll_fmc.h \
	cmsis_os.h \
	cmsis_os2.h \
	freertos_mpool.h \
	freertos_os2.h \
	FreeRTOS.h \
	croutine.h \
	deprecated_definitions.h \
	event_groups.h \
	list.h \
	mpu_wrappers.h \
	portable.h \
	projdefs.h \
	queue.h \
	semphr.h \
	stack_macros.h \
	stream_buffer.h \
	task.h \
	timers.h \
	portmacro.h \
	adc.h \
	dcmi.h \
	fmc.h \
	gpio.h \
	i2c.h \
	tim.h \

LIBRARY_C_FILES = \
	freertos.c \
	main.c \
	stm32f4xx_hal_msp.c \
	stm32f4xx_hal_timebase_tim.c \
	stm32f4xx_it.c \
	system_stm32f4xx.c \
	stm32f4xx_hal.c \
	stm32f4xx_hal_adc.c \
	stm32f4xx_hal_adc_ex.c \
	stm32f4xx_hal_cortex.c \
	stm32f4xx_hal_dcmi.c \
	stm32f4xx_hal_dcmi_ex.c \
	stm32f4xx_hal_dma.c \
	stm32f4xx_hal_dma_ex.c \
	stm32f4xx_hal_exti.c \
	stm32f4xx_hal_flash.c \
	stm32f4xx_hal_flash_ex.c \
	stm32f4xx_hal_flash_ramfunc.c \
	stm32f4xx_hal_gpio.c \
	stm32f4xx_hal_i2c.c \
	stm32f4xx_hal_i2c_ex.c \
	stm32f4xx_hal_pwr.c \
	stm32f4xx_hal_pwr_ex.c \
	stm32f4xx_hal_rcc.c \
	stm32f4xx_hal_rcc_ex.c \
	stm32f4xx_hal_sdram.c \
	stm32f4xx_hal_tim.c \
	stm32f4xx_hal_tim_ex.c \
	stm32f4xx_hal_uart.c \
	stm32f4xx_ll_adc.c \
	stm32f4xx_ll_fmc.c \
	cmsis_os2.c \
	croutine.c \
	event_groups.c \
	list.c \
	port.c \
	heap_4.c \
	queue.c \
	stream_buffer.c \
	tasks.c \
	timers.c \
	adc.c \
	dcmi.c \
	fmc.c \
	gpio.c \
	i2c.c \
	tim.c \

LIBRARY_S_FILES = \
	startup_stm32f427xx.s \

LIBRARY_LD_FILES = \
	STM32F427ZITx_FLASH.ld \

LIBRARY_FILES = $(LIBRARY_H_FILES) $(LIBRARY_C_FILES) $(LIBRARY_S_FILES) $(LIBRARY_LD_FILES)

LIBRARY_BUILD_DIR = $(BUILD_DIR)/test
TEST_BUILD_DIR = $(BUILD_DIR)/test
RELEASE_BUILD_DIR = $(RELEASE_DIR)

TEST_SOURCE_DIR = test
TEST_SOURCE_C_FILES = $(notdir $(wildcard $(TEST_SOURCE_DIR)/*.c $(PKG_LOGGER_DIR)/*.c)) 

CC = $(TOOLCHAIN_PATH)gcc
AS = $(TOOLCHAIN_PATH)gcc -x assembler-with-cpp
SZ = $(TOOLCHAIN_PATH)size
HX = $(TOOLCHAIN_PATH)objcopy -O ihex
BN = $(TOOLCHAIN_PATH)objcopy -O binary -S

MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
OPTIONS = -fdata-sections -ffunction-sections -Wall -Werror -DUSE_FULL_ASSERT=1

VPATH = $(LIBRARY_DIRS)

LIBRARIES = \
	-lc \
	-lm \
	-lnosys

DEFINES = \
	-DUSE_HAL_DRIVER \
	-DSTM32F427xx
	
INCLUDES = \
	-I$(LIBRARY_BUILD_DIR) \
	-I$(PKG_LOGGER_DIR)

.PHONY: default all library test release clean

default : library test release

library : $(LIBRARY_FILES:%=$(LIBRARY_BUILD_DIR)/%)

$(LIBRARY_BUILD_DIR)/% : % | $(LIBRARY_BUILD_DIR) $(LIBRARY_BUILD_DIR)/Legacy
	cp $< $@
	
$(LIBRARY_BUILD_DIR) :
	mkdir -p $@

$(LIBRARY_BUILD_DIR)/Legacy :
	mkdir -p $@

test : library $(TEST_BUILD_DIR)/test.bin $(TEST_BUILD_DIR)/test.hex
	$(PKG_HW_TEST_AGENT_DIR)/sr-hw-test-agent --run --test-timeout=15 --trace-frequency=2m --system-frequency=180m --board RLM36 --file $(TEST_BUILD_DIR)/test.bin	
	
$(TEST_BUILD_DIR)/test.bin : $(TEST_BUILD_DIR)/test.elf
	$(BN) $< $@

$(TEST_BUILD_DIR)/test.hex : $(TEST_BUILD_DIR)/test.elf
	$(HX) $< $@

$(TEST_BUILD_DIR)/test.elf : $(patsubst %,$(TEST_BUILD_DIR)/%.o,$(basename $(LIBRARY_C_FILES) $(LIBRARY_S_FILES) $(TEST_SOURCE_C_FILES)))
	$(CC) $(MCU) -specs=nano.specs -T$(LIBRARY_BUILD_DIR)/$(LIBRARY_LD_FILES) $(LIBRARIES) -Wl,--gc-sections $^ -o $@ -Wl,-Map=$@.map,--cref
	$(SZ) $@

$(TEST_BUILD_DIR)/%.o : $(LIBRARY_BUILD_DIR)/%.c Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(MCU) $(OPTIONS) $(DEFINES) $(INCLUDES) -MMD -g -Og -gdwarf-2 $< -o $@

$(TEST_BUILD_DIR)/%.o : $(LIBRARY_BUILD_DIR)/%.s Makefile | $(TEST_BUILD_DIR)
	$(AS) -c $(MCU) $(OPTIONS) $(DEFINES) $(INCLUDES) -MMD $< -o $@

$(TEST_BUILD_DIR)/%.o : $(TEST_SOURCE_DIR)/%.c Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(MCU) $(OPTIONS) $(DEFINES) $(INCLUDES) -MMD -g -Og -gdwarf-2 $< -o $@

$(TEST_BUILD_DIR)/%.o : $(PKG_LOGGER_DIR)/%.c Makefile | $(TEST_BUILD_DIR)
	$(CC) -c $(MCU) $(OPTIONS) $(DEFINES) $(INCLUDES) -MMD -g -Og -gdwarf-2 $< -o $@

$(TEST_BUILD_DIR) :
	mkdir -p $@

release : library $(LIBRARY_FILES:%=$(RELEASE_BUILD_DIR)/%)

$(RELEASE_BUILD_DIR)/% : $(LIBRARY_BUILD_DIR)/% | $(RELEASE_BUILD_DIR) $(RELEASE_BUILD_DIR)/Legacy
	cp $< $@
	
$(RELEASE_BUILD_DIR) :
	mkdir -p $@

$(RELEASE_BUILD_DIR)/Legacy :
	mkdir -p $@

-include $(wildcard $(TEST_BUILD_DIR)/*.d)

