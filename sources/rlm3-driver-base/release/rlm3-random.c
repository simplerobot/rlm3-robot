#include "rlm3-random.h"
#include "rlm3-task.h"
#include "rlm3-helper.h"
#include "logger.h"
#include "main.h"


LOGGER_ZONE(RANDOM);


extern void RLM3_Random_Init()
{
	__HAL_RCC_RNG_CLK_ENABLE();
    HAL_NVIC_EnableIRQ(HASH_RNG_IRQn);

	SET_REGISTER_FLAGS(RNG->CR,
			FLAG(RNG_CR_RNGEN, 1),  // Enable Random Number Generator
			FLAG(RNG_CR_IE,    1)); // Enable Random Number Interrupt
}

extern void RLM3_Random_Deinit()
{
	SET_REGISTER_FLAGS(RNG->CR,
		FLAG(RNG_CR_RNGEN, 0),  // Disable Random Number Generator
		FLAG(RNG_CR_IE,    0)); // Disable Random Number Interrupt

    HAL_NVIC_DisableIRQ(HASH_RNG_IRQn);
	__HAL_RCC_RNG_CLK_DISABLE();
}

extern bool RLM3_Random_IsInit()
{
	return __HAL_RCC_RNG_IS_CLK_ENABLED();
}

extern __attribute__((weak)) void RLM3_Random_CB_ISR(uint32_t entropy)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

extern void HASH_RNG_IRQHandler(void)
{
	uint32_t status = RNG->SR;
	uint32_t entropy = RNG->DR;

	if ((status & (RNG_SR_CEIS | RNG_SR_SEIS)) != 0)
	{
		if ((status & RNG_SR_CEIS) != 0)
			LOG_WARN("CLOCK ERROR");
		if ((status & RNG_SR_SEIS) != 0)
			LOG_WARN("SEED ERROR");
		SET_REGISTER_FLAGS(RNG->SR,
			FLAG(RNG_SR_CEIS, 0),  // Clear clock error
			FLAG(RNG_SR_SEIS, 0)); // Clear seed error
	}
	else if ((status & RNG_SR_DRDY) != 0)
	{
		RLM3_Random_CB_ISR(entropy);
	}
}
