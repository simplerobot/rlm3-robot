
.PHONY : default all clean

default : all

all :
	$(MAKE) -C sources/build-scripts
	$(MAKE) -C sources/test
	$(MAKE) -C sources/logger
	$(MAKE) -C sources/hash
	$(MAKE) -C sources/hw-test-agent
	$(MAKE) -C sources/test-stm32
	$(MAKE) -C sources/rlm3-hardware
	$(MAKE) -C sources/rlm-base
	$(MAKE) -C sources/rlm3-driver-base
	$(MAKE) -C sources/rlm3-driver-base-sim
	$(MAKE) -C sources/rlm3-driver-flash
	$(MAKE) -C sources/rlm3-driver-flash-sim
	$(MAKE) -C sources/rlm3-driver-wifi
	$(MAKE) -C sources/rlm3-driver-wifi-sim
	$(MAKE) -C sources/firmware
	$(MAKE) -C sources/firmware-base
	

clean :
	rm -rf build

