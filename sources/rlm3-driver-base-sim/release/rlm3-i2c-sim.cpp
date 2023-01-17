#include "rlm3-i2c.h"
#include "rlm3-sim.hpp"
#include "Test.hpp"
#include <queue>


struct ExpectedCallI2C
{
	bool transmit = false;
	bool receive = false;
	RLM3_I2C_DEVICE device;
	uint32_t address = 0;
	std::vector<uint8_t> input;

	bool failure = false;

	std::vector<uint8_t> output;
};

static uint32_t g_active_devices = 0;
std::queue<ExpectedCallI2C> g_expected_calls;


extern void RLM3_I2C_Init(RLM3_I2C_DEVICE device)
{
	ASSERT(device < RLM3_I2C_DEVICE_COUNT);
	ASSERT(!SIM_IsISR());
	ASSERT((g_active_devices & (1 << device)) == 0);
	g_active_devices |= (1 << device);
}

extern void RLM3_I2C_Deinit(RLM3_I2C_DEVICE device)
{
	ASSERT(device < RLM3_I2C_DEVICE_COUNT);
	ASSERT(!SIM_IsISR());
	ASSERT((g_active_devices & (1 << device)) != 0);
	g_active_devices &= ~(1 << device);
}

extern bool RLM3_I2C_IsInit(RLM3_I2C_DEVICE device)
{
	ASSERT(device < RLM3_I2C_DEVICE_COUNT);
	ASSERT(!SIM_IsISR());
	return ((g_active_devices & (1 << device)) != 0);
}

static ExpectedCallI2C GetNextExpectedCall()
{
	ASSERT(!g_expected_calls.empty());
	ExpectedCallI2C call = g_expected_calls.front();
	g_expected_calls.pop();
	return call;
}

extern bool RLM3_I2C_Transmit(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* data, size_t size)
{
	ASSERT(RLM3_I2C_IsInit(device));

	ExpectedCallI2C call = GetNextExpectedCall();
	ASSERT(call.transmit && !call.receive);
	ASSERT(call.device == device);
	ASSERT(call.address == addr);
	ASSERT(call.input == std::vector<uint8_t>(data, data + size));
	return !call.failure;
}

extern bool RLM3_I2C_Receive(RLM3_I2C_DEVICE device, uint32_t addr, uint8_t* data, size_t size)
{
	ASSERT(RLM3_I2C_IsInit(device));

	ExpectedCallI2C call = GetNextExpectedCall();
	ASSERT(!call.transmit && call.receive);
	ASSERT(call.device == device);
	ASSERT(call.address == addr);
	if (call.failure)
		return false;
	ASSERT(call.output.size() == size);
	for (size_t i = 0; i < size; i++)
		data[i] = call.output[i];
	return true;
}

extern bool RLM3_I2C_TransmitReceive(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* tx_data, size_t tx_size, uint8_t* rx_data, size_t rx_size)
{
	ASSERT(RLM3_I2C_IsInit(device));

	ExpectedCallI2C call = GetNextExpectedCall();
	ASSERT(call.transmit && call.receive);
	ASSERT(call.device == device);
	ASSERT(call.address == addr);
	ASSERT(call.input == std::vector<uint8_t>(tx_data, tx_data + tx_size));
	if (call.failure)
		return false;
	ASSERT(call.output.size() == rx_size);
	for (size_t i = 0; i < rx_size; i++)
		rx_data[i] = call.output[i];
	return true;
}

extern void SIM_I2C_Transmit(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* data, size_t size)
{
	ExpectedCallI2C call;
	call.transmit = true;
	call.device = device;
	call.address = addr;
	call.input.assign(data, data + size);
	g_expected_calls.push(call);
}

extern void SIM_I2C_TransmitFailure(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* data, size_t size)
{
	ExpectedCallI2C call;
	call.transmit = true;
	call.device = device;
	call.address = addr;
	call.input.assign(data, data + size);
	call.failure = true;
	g_expected_calls.push(call);
}

extern void SIM_I2C_Receive(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* data, size_t size)
{
	ExpectedCallI2C call;
	call.receive = true;
	call.device = device;
	call.address = addr;
	call.output.assign(data, data + size);
	g_expected_calls.push(call);
}

extern void SIM_I2C_ReceiveFailure(RLM3_I2C_DEVICE device, uint32_t addr)
{
	ExpectedCallI2C call;
	call.receive = true;
	call.device = device;
	call.address = addr;
	call.failure = true;
	g_expected_calls.push(call);
}

extern void SIM_I2C_TransmitReceive(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* tx_data, size_t tx_size, const uint8_t* rx_data, size_t rx_size)
{
	ExpectedCallI2C call;
	call.transmit = true;
	call.receive = true;
	call.device = device;
	call.address = addr;
	call.input.assign(tx_data, tx_data + tx_size);
	call.output.assign(rx_data, rx_data + rx_size);
	g_expected_calls.push(call);
}

extern void SIM_I2C_TransmitReceiveFailure(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* tx_data, size_t tx_size)
{
	ExpectedCallI2C call;
	call.transmit = true;
	call.receive = true;
	call.device = device;
	call.address = addr;
	call.input.assign(tx_data, tx_data + tx_size);
	call.failure = true;
	g_expected_calls.push(call);
}

TEST_SETUP(SIM_I2C_INIT)
{
	g_active_devices = 0;
	while (!g_expected_calls.empty())
		g_expected_calls.pop();
}
