#include <cstdlib>
#include <iostream>

#include "spdlog/spdlog.h"
#include "DeviceManager.h"

int main(const int argc, const char** argv) {
	spdlog::set_level(spdlog::level::trace);
	spdlog::info("Human Interface Device Debuggerator");

	auto deviceManager = new DeviceManager();

	size_t deviceCount = deviceManager->loadDevices();

	if (deviceCount == 0) {
		spdlog::error("No HIDs were loaded");
	} else {
		spdlog::info("Loaded {} HIDs", deviceCount);
	}

	return 0;
}
 