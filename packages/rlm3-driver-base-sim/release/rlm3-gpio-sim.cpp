#include "rlm3-gpio.h"
#include "rlm3-sim.hpp"
#include "Test.hpp"


struct GPIO_Pin_Config
{
	GPIO_Mode mode;
	GPIO_Pull pull;
	GPIO_Speed speed;
	GPIO_AlternateFunction alternate;
};

struct GPIO_Port_State
{
	bool clock_enabled;
	uint16_t enabled_pins;
	uint16_t output_pins;
	uint16_t input_pins;
	uint16_t interrupt_pins;
	uint16_t pin_values;
	GPIO_Pin_Config pin_configs[16];
};

static GPIO_Port_State g_port_states[GPIO_COUNT];
static uint16_t g_interrupt_pins = 0;


static void ValidatePort(GPIO_Port port)
{
	ASSERT(port < GPIO_COUNT);
}

static void ValidatePins(GPIO_Port port, uint32_t pins)
{
	ValidatePort(port);
	ASSERT(pins != 0);
	ASSERT((pins & ~0xFFFF) == 0);
}

static void ValidatePin(GPIO_Port port, uint32_t pin)
{
	ValidatePins(port, pin);
	ASSERT(__builtin_popcount(pin) == 1);
}

static void ValidateInit(GPIO_InitTypeDef* init)
{
	ASSERT(init != nullptr);
	// We currently only support digital input and output pins.  NOT alternate functions.
	ASSERT(init->Mode == GPIO_MODE_INPUT || init->Mode == GPIO_MODE_IT_FALLING || init->Mode == GPIO_MODE_IT_RISING || init->Mode == GPIO_MODE_IT_RISING_FALLING || init->Mode == GPIO_MODE_OUTPUT_PP || init->Mode == GPIO_MODE_OUTPUT_OD);

	ASSERT(init->Pull == GPIO_NOPULL || init->Pull == GPIO_PULLUP || init->Pull == GPIO_PULLDOWN);
	ASSERT(init->Speed == GPIO_SPEED_FREQ_LOW || init->Speed == GPIO_SPEED_FREQ_MEDIUM || init->Speed == GPIO_SPEED_FREQ_HIGH || init->Speed == GPIO_SPEED_FREQ_VERY_HIGH);
	ASSERT(init->Alternate == GPIO_AF_DISABLED);
}


extern void GPIO_CLOCK_ENABLE(GPIO_Port port)
{
	ValidatePort(port);
	ASSERT(!g_port_states[port].clock_enabled);
	g_port_states[port].clock_enabled = true;
}

extern void GPIO_CLOCK_DISABLE(GPIO_Port port)
{
	ValidatePort(port);
	ASSERT(g_port_states[port].clock_enabled);
	g_port_states[port].clock_enabled = false;
}

extern void HAL_GPIO_Init(GPIO_Port port, GPIO_InitTypeDef* init)
{
	ValidateInit(init);
	ValidatePins(port, init->Pin);

	GPIO_Port_State& port_state = g_port_states[port];

	// Make sure the port clock is enabled.
	ASSERT(port_state.clock_enabled);

	// Make sure these pins are not already enabled.
	ASSERT((port_state.enabled_pins & init->Pin) == 0);

	// Enable these pins.
	port_state.enabled_pins |= init->Pin;
	if (init->Mode == GPIO_MODE_INPUT || init->Mode == GPIO_MODE_IT_FALLING || init->Mode == GPIO_MODE_IT_RISING || init->Mode == GPIO_MODE_IT_RISING_FALLING)
		port_state.input_pins |= init->Pin;
	else
		port_state.output_pins |= init->Pin;

	if (init->Mode == GPIO_MODE_IT_FALLING || init->Mode == GPIO_MODE_IT_RISING || init->Mode == GPIO_MODE_IT_RISING_FALLING)
	{
		// Only one pin between all the ports can have interrupt mode enabled.
		ASSERT((g_interrupt_pins & init->Pin) == 0);
		port_state.interrupt_pins |= init->Pin;
		g_interrupt_pins |= init->Pin;
	}

	// Save the configuration for each affected pin.
	for (size_t pin_index = 0; pin_index < 16; pin_index++)
	{
		uint32_t pin = 1 << pin_index;
		if ((init->Pin & pin) != 0)
		{
			GPIO_Pin_Config& pin_state = port_state.pin_configs[pin_index];
			pin_state.mode = init->Mode;
			pin_state.pull = init->Pull;
			pin_state.speed = init->Speed;
			pin_state.alternate = init->Alternate;
		}
	}
}

extern void HAL_GPIO_DeInit(GPIO_Port port, uint32_t pin)
{
	ValidatePins(port, pin);

	// Make sure these pins are actually enabled.
	GPIO_Port_State& port_state = g_port_states[port];
	ASSERT(port_state.clock_enabled == true);
	ASSERT((pin & ~port_state.enabled_pins) == 0);

	// Clear any global interrupt pins set by these pins.
	g_interrupt_pins &= ~(port_state.interrupt_pins & pin);

	// Clear all the enabled bits for these pins.
	port_state.enabled_pins &= ~pin;
	port_state.output_pins &= ~pin;
	port_state.input_pins &= ~pin;
	port_state.interrupt_pins &= ~pin;

	// Reset the pin state.
	for (size_t pin_index = 0; pin_index < 16; pin_index++)
	{
		if ((pin & (1 << pin_index)) != 0)
		{
			GPIO_Pin_Config& pin_state = port_state.pin_configs[pin_index];
			pin_state.mode = GPIO_MODE_DISABLED;
			pin_state.pull = GPIO_PULL_DISABLED;
			pin_state.speed = GPIO_SPEED_DISABLED;
			pin_state.alternate = GPIO_AF_DISABLED;
		}
	}
}

extern GPIO_PinState HAL_GPIO_ReadPin(GPIO_Port port, uint32_t pin)
{
	ValidatePin(port, pin);

	// Make sure this pin is actually enabled for input.
	GPIO_Port_State& port_state = g_port_states[port];
	ASSERT(port_state.clock_enabled == true);
	ASSERT((pin & ~port_state.enabled_pins) == 0);
	ASSERT((pin & ~port_state.input_pins) == 0);

	if ((pin & port_state.pin_values) != 0)
		return GPIO_PIN_SET;
	else
		return GPIO_PIN_RESET;
}

void HAL_GPIO_WritePin(GPIO_Port port, uint32_t pin, GPIO_PinState state)
{
	ValidatePins(port, pin);
	ASSERT(state == GPIO_PIN_SET || state == GPIO_PIN_RESET);

	// Make sure this pin is available.  The pin can be written as long as it is not in input mode.
	GPIO_Port_State& port_state = g_port_states[port];
	ASSERT(port_state.clock_enabled == true);
	ASSERT((pin & port_state.input_pins) == 0);

	if (state == GPIO_PIN_SET)
		port_state.pin_values |= pin;
	else
		port_state.pin_values &= ~pin;
}

extern bool SIM_GPIO_Read(GPIO_Port port, uint32_t pin)
{
	ValidatePin(port, pin);

	GPIO_Port_State& port_state = g_port_states[port];
	return ((pin & port_state.pin_values) != 0);
}

void SIM_GPIO_Write(GPIO_Port port, uint32_t pin, bool value)
{
	ValidatePins(port, pin);

	GPIO_Port_State& port_state = g_port_states[port];
	if (value)
		port_state.pin_values |= pin;
	else
		port_state.pin_values &= ~pin;
}

extern bool SIM_GPIO_IsClockEnabled(GPIO_Port port)
{
	ValidatePort(port);
	GPIO_Port_State& port_state = g_port_states[port];
	return port_state.clock_enabled;
}

extern bool SIM_GPIO_IsEnabled(GPIO_Port port, uint32_t pin)
{
	ValidatePin(port, pin);
	GPIO_Port_State& port_state = g_port_states[port];
	if (!port_state.clock_enabled)
		return false;
	return (port_state.enabled_pins & pin) != 0;
}

extern GPIO_Mode SIM_GPIO_GetMode(GPIO_Port port, uint32_t pin)
{
	ValidatePin(port, pin);
	GPIO_Port_State& port_state = g_port_states[port];
	if (port_state.clock_enabled)
		for (size_t pin_index = 0; pin_index < 16; pin_index++)
			if ((pin & (1 << pin_index)) != 0)
				return port_state.pin_configs[pin_index].mode;
	return GPIO_MODE_DISABLED;

}

extern GPIO_Pull SIM_GPIO_GetPull(GPIO_Port port, uint32_t pin)
{
	ValidatePin(port, pin);
	GPIO_Port_State& port_state = g_port_states[port];
	if (port_state.clock_enabled)
		for (size_t pin_index = 0; pin_index < 16; pin_index++)
			if ((pin & (1 << pin_index)) != 0)
				return port_state.pin_configs[pin_index].pull;
	return GPIO_PULL_DISABLED;
}

extern GPIO_Speed SIM_GPIO_GetSpeed(GPIO_Port port, uint32_t pin)
{
	ValidatePin(port, pin);
	GPIO_Port_State& port_state = g_port_states[port];
	if (port_state.clock_enabled)
		for (size_t pin_index = 0; pin_index < 16; pin_index++)
			if ((pin & (1 << pin_index)) != 0)
				return port_state.pin_configs[pin_index].speed;
	return GPIO_SPEED_DISABLED;
}

extern GPIO_AlternateFunction SIM_GPIO_GetAlt(GPIO_Port port, uint32_t pin)
{
	ValidatePin(port, pin);
	GPIO_Port_State& port_state = g_port_states[port];
	if (port_state.clock_enabled)
		for (size_t pin_index = 0; pin_index < 16; pin_index++)
			if ((pin & (1 << pin_index)) != 0)
				return port_state.pin_configs[pin_index].alternate;
	return GPIO_AF_DISABLED;
}

extern void SIM_GPIO_Interrupt(GPIO_Port port, uint32_t pin)
{
	ValidatePin(port, pin);

	SIM_AddInterrupt([=]() {
		GPIO_Port_State& port_state = g_port_states[port];
		ASSERT(port_state.clock_enabled);
		ASSERT((port_state.interrupt_pins & pin) != 0);
		switch (pin)
		{
		case GPIO_PIN_12:
			RLM3_EXTI12_Callback();
			break;
		default:
			// We do not support interrupts on this pin.
			ASSERT(false);
			break;
		}
	});
}

extern __attribute((weak)) void RLM3_EXTI12_Callback()
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}


TEST_SETUP(SIM_GPIO_INIT)
{
	g_interrupt_pins = 0;
	for (auto& port : g_port_states)
	{
		port.clock_enabled = false;
		port.enabled_pins = 0;
		port.output_pins = 0;
		port.input_pins = 0;
		port.interrupt_pins = 0;
		port.pin_values = 0;
		for (auto& pin : port.pin_configs)
		{
			pin.mode = GPIO_MODE_DISABLED;
			pin.pull = GPIO_PULL_DISABLED;
			pin.speed = GPIO_SPEED_DISABLED;
			pin.alternate = GPIO_AF_DISABLED;
		}
	}
}

const char* ToString(GPIO_Port port)
{
	switch (port)
	{
	case GPIOA: return "GPIOA";
	case GPIOB: return "GPIOB";
	case GPIOC: return "GPIOC";
	case GPIOD: return "GPIOD";
	case GPIOE: return "GPIOE";
	case GPIOF: return "GPIOF";
	case GPIOG: return "GPIOG";
	case GPIOH: return "GPIOH";
	case GPIOI: return "GPIOI";
	case GPIOJ: return "GPIOJ";
	case GPIOK: return "GPIOK";
	default: return "<invalid>";
	}
}

const char* ToString(GPIO_Mode mode)
{
	switch (mode)
	{
	case GPIO_MODE_DISABLED:	return "GPIO_MODE_UNINITIALIZED";
	case GPIO_MODE_INPUT:	return "GPIO_MODE_INPUT";
	case GPIO_MODE_OUTPUT_PP:	return "GPIO_MODE_OUTPUT_PP";
	case GPIO_MODE_OUTPUT_OD:	return "GPIO_MODE_OUTPUT_OD";
	case GPIO_MODE_AF_PP:	return "GPIO_MODE_AF_PP";
	case GPIO_MODE_AF_OD:	return "GPIO_MODE_AF_OD";
	case GPIO_MODE_ANALOG:	return "GPIO_MODE_ANALOG";
	case GPIO_MODE_IT_RISING:	return "GPIO_MODE_IT_RISING";
	case GPIO_MODE_IT_FALLING:	return "GPIO_MODE_IT_FALLING";
	case GPIO_MODE_IT_RISING_FALLING:	return "GPIO_MODE_IT_RISING_FALLING";
	case GPIO_MODE_EVT_RISING:	return "GPIO_MODE_EVT_RISING";
	case GPIO_MODE_EVT_FALLING:	return "GPIO_MODE_EVT_FALLING";
	case GPIO_MODE_EVT_RISING_FALLING:	return "GPIO_MODE_EVT_RISING_FALLING";
	default: return "<invalid>";
	}
}

const char* ToString(GPIO_Pull pull)
{
	switch (pull)
	{
	case GPIO_PULL_DISABLED:	return "GPIO_PULL_UNINITIALIZED";
	case GPIO_NOPULL:	return "GPIO_NOPULL";
	case GPIO_PULLUP:	return "GPIO_PULLUP";
	case GPIO_PULLDOWN:	return "GPIO_PULLDOWN";
	default: return "<invalid>";
	}
}

const char* ToString(GPIO_Speed speed)
{
	switch (speed)
	{
	case GPIO_SPEED_DISABLED:	return "GPIO_SPEED_UNINITIALIZED";
	case GPIO_SPEED_FREQ_LOW:	return "GPIO_SPEED_FREQ_LOW";
	case GPIO_SPEED_FREQ_MEDIUM:	return "GPIO_SPEED_FREQ_MEDIUM";
	case GPIO_SPEED_FREQ_HIGH:	return "GPIO_SPEED_FREQ_HIGH";
	case GPIO_SPEED_FREQ_VERY_HIGH:	return "GPIO_SPEED_FREQ_VERY_HIGH";
	default: return "<invalid>";
	}
}

const char* ToString(GPIO_AlternateFunction alternate)
{
	switch (alternate)
	{
	case GPIO_AF_DISABLED:	return "GPIO_AF_UNINITIALIZED";
	case GPIO_AF0_RTC_50Hz:	return "GPIO_AF0_RTC_50Hz";
	case GPIO_AF0_MCO:	return "GPIO_AF0_MCO";
	case GPIO_AF0_TAMPER:	return "GPIO_AF0_TAMPER";
	case GPIO_AF0_SWJ:	return "GPIO_AF0_SWJ";
	case GPIO_AF0_TRACE:	return "GPIO_AF0_TRACE";
	case GPIO_AF1_TIM1:	return "GPIO_AF1_TIM1";
	case GPIO_AF1_TIM2:	return "GPIO_AF1_TIM2";
	case GPIO_AF2_TIM3:	return "GPIO_AF2_TIM3";
	case GPIO_AF2_TIM4:	return "GPIO_AF2_TIM4";
	case GPIO_AF2_TIM5:	return "GPIO_AF2_TIM5";
	case GPIO_AF3_TIM8:	return "GPIO_AF3_TIM8";
	case GPIO_AF3_TIM9:	return "GPIO_AF3_TIM9";
	case GPIO_AF3_TIM10:	return "GPIO_AF3_TIM10";
	case GPIO_AF3_TIM11:	return "GPIO_AF3_TIM11";
	case GPIO_AF4_I2C1:	return "GPIO_AF4_I2C1";
	case GPIO_AF4_I2C2:	return "GPIO_AF4_I2C2";
	case GPIO_AF4_I2C3:	return "GPIO_AF4_I2C3";
	case GPIO_AF5_SPI1:	return "GPIO_AF5_SPI1";
	case GPIO_AF5_SPI2:	return "GPIO_AF5_SPI2";
	case GPIO_AF5_SPI3:	return "GPIO_AF5_SPI3";
	case GPIO_AF5_SPI4:	return "GPIO_AF5_SPI4";
	case GPIO_AF5_SPI5:	return "GPIO_AF5_SPI5";
	case GPIO_AF5_SPI6:	return "GPIO_AF5_SPI6";
	case GPIO_AF6_SPI3:	return "GPIO_AF6_SPI3";
	case GPIO_AF6_SAI1:	return "GPIO_AF6_SAI1";
	case GPIO_AF7_USART1:	return "GPIO_AF7_USART1";
	case GPIO_AF7_USART2:	return "GPIO_AF7_USART2";
	case GPIO_AF7_USART3:	return "GPIO_AF7_USART3";
	case GPIO_AF8_UART4:	return "GPIO_AF8_UART4";
	case GPIO_AF8_UART5:	return "GPIO_AF8_UART5";
	case GPIO_AF8_USART6:	return "GPIO_AF8_USART6";
	case GPIO_AF8_UART7:	return "GPIO_AF8_UART7";
	case GPIO_AF8_UART8:	return "GPIO_AF8_UART8";
	case GPIO_AF9_CAN1:	return "GPIO_AF9_CAN1";
	case GPIO_AF9_CAN2:	return "GPIO_AF9_CAN2";
	case GPIO_AF9_TIM12:	return "GPIO_AF9_TIM12";
	case GPIO_AF9_TIM13:	return "GPIO_AF9_TIM13";
	case GPIO_AF9_TIM14:	return "GPIO_AF9_TIM14";
	case GPIO_AF10_OTG_FS:	return "GPIO_AF10_OTG_FS";
	case GPIO_AF10_OTG_HS:	return "GPIO_AF10_OTG_HS";
	case GPIO_AF11_ETH:	return "GPIO_AF11_ETH";
	case GPIO_AF12_FMC:	return "GPIO_AF12_FMC";
	case GPIO_AF12_OTG_HS_FS:	return "GPIO_AF12_OTG_HS_FS";
	case GPIO_AF12_SDIO:	return "GPIO_AF12_SDIO";
	case GPIO_AF13_DCMI:	return "GPIO_AF13_DCMI";
	case GPIO_AF15_EVENTOUT:	return "GPIO_AF15_EVENTOUT";
	default: return "<invalid>";
	}
}

const char* ToString(GPIO_PinState state)
{
	switch (state)
	{
	case GPIO_PIN_RESET:	return "GPIO_PIN_RESET";
	case GPIO_PIN_SET:	return "GPIO_PIN_SET";
	default: return "<invalid>";
	}
}

std::ostream& operator<<(std::ostream& out, GPIO_Port port)
{
	out << ToString(port);
	return out;
}

std::ostream& operator<<(std::ostream& out, GPIO_Mode mode)
{
	out << ToString(mode);
	return out;
}

std::ostream& operator<<(std::ostream& out, GPIO_Pull pull)
{
	out << ToString(pull);
	return out;
}

std::ostream& operator<<(std::ostream& out, GPIO_Speed speed)
{
	out << ToString(speed);
	return out;
}

std::ostream& operator<<(std::ostream& out, GPIO_AlternateFunction alternate)
{
	out << ToString(alternate);
	return out;
}

std::ostream& operator<<(std::ostream& out, GPIO_PinState state)
{
	out << ToString(state);
	return out;
}
