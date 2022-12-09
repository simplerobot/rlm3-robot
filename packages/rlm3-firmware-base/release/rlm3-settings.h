#pragma once

#include "rlm3-base.h"


#ifdef __cplusplus
extern "C" {
#endif


// This structure defines the layout of data in the 2KB EEPROM module.
typedef struct FlashLayout
{
	uint32_t magic; // Magic value 'RLM3' to indicate that the flash has been configured correctly.
	uint32_t version; // Right now this should be zero.

	// Identity Configuration
	uint32_t id;
	uint8_t key[32];

	// General Configuration
	uint32_t board_flags;
	uint32_t startup_count; // Incremented at beginning of startup.
	uint32_t shutdown_count; // Set to startup_count at the end of a clean shutdown.

	// Origin Configuration
	double origin_x; // ECEF m
	double origin_y; // ECEF m
	double origin_z; // ECEF m

	// Base Station Configuration
	int16_t base_x; // ENU mm
	int16_t base_y; // ENU mm
	int16_t base_z; // ENU mm
	uint16_t base_azimuth; // Fixed 0.16 where 0.00 = North, 0.25 = East, 0.50 = South, and 0.75 = West.

	// WIFI Configurations
	char wifi_ssid[32];
	char wifi_password[64];
	uint8_t wifi_server_ip[4];
	uint8_t wifi_static_ip[4];
	uint8_t wifi_gateway_ip[4];

	// Sensor Calibrations
	int16_t calibration_voltage_bias; // Fixed point TBD
	int16_t calibration_voltage_scale; // Fixed point TBD
	int16_t calibration_current_bias; // Fixed point TBD
	int16_t calibration_current_scale; // Fixed point TBD
	int16_t calibration_acceleration_x_bias; // Fixed point TBD
	int16_t calibration_acceleration_x_scale; // Fixed point TBD
	int16_t calibration_acceleration_y_bias; // Fixed point TBD
	int16_t calibration_acceleration_y_scale; // Fixed point TBD
	int16_t calibration_acceleration_z_bias; // Fixed point TBD
	int16_t calibration_acceleration_z_scale; // Fixed point TBD
	int16_t calibration_gyroscope_x_bias; // Fixed point TBD
	int16_t calibration_gyroscope_x_scale; // Fixed point TBD
	int16_t calibration_gyroscope_y_bias; // Fixed point TBD
	int16_t calibration_gyroscope_y_scale; // Fixed point TBD
	int16_t calibration_gyroscope_z_bias; // Fixed point TBD
	int16_t calibration_gyroscope_z_scale; // Fixed point TBD
	int16_t calibration_magnet_x_bias; // Fixed point TBD
	int16_t calibration_magnet_x_scale; // Fixed point TBD
	int16_t calibration_magnet_y_bias; // Fixed point TBD
	int16_t calibration_magnet_y_scale; // Fixed point TBD
	int16_t calibration_magnet_z_bias; // Fixed point TBD
	int16_t calibration_magnet_z_scale; // Fixed point TBD
	int16_t calibration_magnet_temperature_bias; // Fixed point TBD
	int16_t calibration_magnet_temperature_scale; // Fixed point TBD
	int16_t calibration_battery_temperature_bias; // Fixed point TBD
	int16_t calibration_battery_temperature_scale; // Fixed point TBD
	int16_t calibration_inertia_temperature_bias; // Fixed point TBD
	int16_t calibration_inertia_temperature_scale; // Fixed point TBD
	int16_t calibration_current_blade_motor_bias; // Fixed point TBD
	int16_t calibration_current_blade_motor_scale; // Fixed point TBD
	int16_t calibration_current_left_motor_bias; // Fixed point TBD
	int16_t calibration_current_left_motor_scale; // Fixed point TBD
	int16_t calibration_current_right_motor_bias; // Fixed point TBD
	int16_t calibration_current_right_motor_scale; // Fixed point TBD

	// Long term statistics:  error counts.  Usage stats.  Battery characteristics.

	uint8_t reserved[1784];

} FlashLayout;

// This structure defines the layout of data in the external 8MB SDRAM module.
typedef struct ExternalMemoryLayout
{
	// Firmware update / file download.
	uint32_t firmware_magic; // Magic value that claims a firmware update is pending. Values: 'SRFW', 'SGPS', 'SESP'
	uint32_t firmware_size;
	uint8_t firmware_signature[32]; // SHA3 signature of firmware
	uint8_t firmware_data[2 * 1024 * 1024];

	// Camera data
	uint8_t image_buffer[1024 * 1024];

	// Flash configuration live copy.
	FlashLayout configuration;

	// Software block box.
	uint32_t fault_magic; // Magic value that indicates the fault information is valid. 'FOUL'
	char fault_cause[32]; // Text reason for the fault.
	char fault_communication_thread_state[16]; // Keeps track of what the communication thread was doing with the last failure occurred.

	// Log data/buffer
	uint32_t log_magic; // Magic value to keep log information over a restart.  'LOGM'
	uint32_t log_head; // Next index (mod buffer size) to write data into the log buffer.
	uint32_t log_tail; // Next index (mod buffer size) to read data from the log buffer.
	char log_buffer[1024 * 1024];

	// Additional heap?
	// Mapping data?

	uint8_t reserved[4192152];
} ExternalMemoryLayout;


#ifdef __cplusplus
}
#endif
