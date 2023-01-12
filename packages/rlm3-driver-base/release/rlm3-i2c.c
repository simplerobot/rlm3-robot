#include "rlm3-i2c.h"
#include "rlm3-lock.h"
#include "rlm3-task.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "main.h"
#include "logger.h"
#include "i2c.h"
#include "Assert.h"


LOGGER_ZONE(I2C);


enum
{
	I2C_STATE_IDLE,
	I2C_STATE_ERROR,
	I2C_STATE_TX_WAIT,
	I2C_STATE_TX_DONE,
	I2C_STATE_RX_WAIT,
	I2C_STATE_RX_DONE,
};


static RLM3_MutexLock g_lock;

static uint8_t g_active_devices = 0;

static volatile uint8_t g_state = I2C_STATE_IDLE;
static volatile RLM3_Task g_waiting_thread = NULL;


static __attribute__((constructor)) void Init_I2C()
{
	RLM3_MutexLock_Init(&g_lock);
}


extern void RLM3_I2C_Init(RLM3_I2C_DEVICE device)
{
	LOG_TRACE("Init %d", device);

	ASSERT(device < RLM3_I2C_DEVICE_COUNT);
	ASSERT(READ_BIT(g_active_devices, 1 << device) == 0);

	RLM3_MutexLock_Enter(&g_lock);
	if (g_active_devices == 0)
		MX_I2C1_Init();
	SET_BIT(g_active_devices, 1 << device);
	RLM3_MutexLock_Leave(&g_lock);
}

extern void RLM3_I2C_Deinit(RLM3_I2C_DEVICE device)
{
	LOG_TRACE("Deinit %d", device);

	ASSERT(device < RLM3_I2C_DEVICE_COUNT);
	ASSERT(READ_BIT(g_active_devices, 1 << device) != 0);

	RLM3_MutexLock_Enter(&g_lock);
	CLEAR_BIT(g_active_devices, 1 << device);
	if (g_active_devices == 0)
		HAL_I2C_DeInit(&hi2c1);
	RLM3_MutexLock_Leave(&g_lock);
}

extern bool RLM3_I2C_IsInit(RLM3_I2C_DEVICE device)
{
	return (READ_BIT(g_active_devices, 1 << device) != 0);
}

extern bool RLM3_I2C_Transmit(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* data, size_t size)
{
	LOG_TRACE("TX(%x) %d", (int)addr, size);

	ASSERT(RLM3_I2C_IsInit(device));
	ASSERT(addr <= 0x7F);
	ASSERT(data != NULL);
	ASSERT(size > 0);

	RLM3_MutexLock_Enter(&g_lock);
	ASSERT(g_waiting_thread == NULL);
	ASSERT(g_state == I2C_STATE_IDLE);
	g_waiting_thread = RLM3_Task_GetCurrent();
	g_state = I2C_STATE_TX_WAIT;

	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_IT(&hi2c1, (addr << 1) | 0x00, (uint8_t*)data, size);
	while (status == HAL_OK && g_state == I2C_STATE_TX_WAIT)
		RLM3_Task_Take();
	bool result = (status == HAL_OK && g_state == I2C_STATE_TX_DONE);

	g_waiting_thread = NULL;
	g_state = I2C_STATE_IDLE;
	RLM3_MutexLock_Leave(&g_lock);

	return result;
}

extern bool RLM3_I2C_Receive(RLM3_I2C_DEVICE device, uint32_t addr, uint8_t* data, size_t size)
{
	LOG_TRACE("RX(%x) %d", (int)addr, size);

	ASSERT(RLM3_I2C_IsInit(device));
	ASSERT(addr <= 0x7F);
	ASSERT(data != NULL);
	ASSERT(size > 0);

	RLM3_MutexLock_Enter(&g_lock);
	ASSERT(g_waiting_thread == NULL);
	ASSERT(g_state == I2C_STATE_IDLE);
	g_waiting_thread = RLM3_Task_GetCurrent();
	g_state = I2C_STATE_RX_WAIT;

	HAL_StatusTypeDef status = HAL_I2C_Master_Receive_IT(&hi2c1, (addr << 1) | 0x01, data, size);
	while (status == HAL_OK && hi2c1.State == HAL_I2C_STATE_BUSY_RX)
		RLM3_Task_Take();
	bool result = (status == HAL_OK && g_state == I2C_STATE_RX_DONE);

	g_waiting_thread = NULL;
	g_state = I2C_STATE_IDLE;
	RLM3_MutexLock_Leave(&g_lock);

	return result;
}

extern bool RLM3_I2C_TransmitReceive(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* tx_data, size_t tx_size, uint8_t* rx_data, size_t rx_size)
{
	LOG_TRACE("TR(%x) %d %d", (int)addr, tx_size, rx_size);

	ASSERT(RLM3_I2C_IsInit(device));
	ASSERT(addr <= 0x7F);
	ASSERT(tx_data != NULL && rx_data != NULL);
	ASSERT(tx_size > 0 && rx_size > 0);

	RLM3_MutexLock_Enter(&g_lock);
	ASSERT(g_waiting_thread == NULL);
	ASSERT(g_state == I2C_STATE_IDLE);
	g_waiting_thread = RLM3_Task_GetCurrent();

	HAL_StatusTypeDef status = HAL_OK;

	if (status == HAL_OK)
	{
		g_state = I2C_STATE_TX_WAIT;
		status = HAL_I2C_Master_Seq_Transmit_IT(&hi2c1, (addr << 1) | 0x00, (uint8_t*)tx_data, tx_size, I2C_FIRST_FRAME);
	}
	while (status == HAL_OK && g_state == I2C_STATE_TX_WAIT)
		RLM3_Task_Take();

	if (status == HAL_OK && g_state == I2C_STATE_TX_DONE)
	{
		g_state = I2C_STATE_RX_WAIT;
		status = HAL_I2C_Master_Seq_Receive_IT(&hi2c1, (addr << 1) | 0x01, rx_data, rx_size, I2C_LAST_FRAME);
	}
	while (status == HAL_OK && g_state == I2C_STATE_RX_WAIT)
		RLM3_Task_Take();

	bool result = (status == HAL_OK && g_state == I2C_STATE_RX_DONE);

	g_waiting_thread = NULL;
	g_state = I2C_STATE_IDLE;
	RLM3_MutexLock_Leave(&g_lock);

	return result;
}

static void WakeupWaitingThreadFromISR(I2C_HandleTypeDef *hi2c, uint8_t new_state, const char* type)
{
	LOG_TRACE("ISR %s", type);
	if (hi2c == &hi2c1)
	{
		g_state = new_state;
		RLM3_Task_GiveISR(g_waiting_thread);
	}
}

extern void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	WakeupWaitingThreadFromISR(hi2c, I2C_STATE_TX_DONE, "TX");
}

extern void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	WakeupWaitingThreadFromISR(hi2c, I2C_STATE_RX_DONE, "RX");
}

extern void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	WakeupWaitingThreadFromISR(hi2c, I2C_STATE_ERROR, "ER");
}
