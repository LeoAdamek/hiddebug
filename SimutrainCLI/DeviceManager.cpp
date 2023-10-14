#include "DeviceManager.h"
#include "HiDevice.h"
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT 1

#include <vector>
#include <stringapiset.h>
#include <iostream>

DeviceManager::DeviceManager() {
	hidModuleHandle = LoadLibrary(L"hid.dll");

	HidD_GetHidGuid(&gHidGuid);
}

DeviceManager::~DeviceManager() {
	if (hidModuleHandle != NULL) {
		FreeLibrary(hidModuleHandle);
	}

	for (auto d : devices) {
		d->close();
	}
}

size_t DeviceManager::loadDevices() {
	//spdlog::enable_backtrace(5);
	//spdlog::debug("Initializing HID class devices");

	HDEVINFO deviceInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DATA deviceData = {};
	ULONG i = 0;

	deviceInfo = SetupDiGetClassDevs(&gHidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (INVALID_HANDLE_VALUE == deviceInfo) {
		//spdlog::error("Unable to get HID class");
		return 0;
	}

	deviceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	for (i = 0; ERROR_NO_MORE_ITEMS != GetLastError(); i++) {

		//spdlog::debug("Opening HID Endpoint #{}", i);

		if (!SetupDiEnumDeviceInterfaces(deviceInfo, 0, &gHidGuid, i, &deviceData)) {
			//spdlog::error("Unable to enumerate interfaces for device HID #{}, due to error code {}", i, GetLastError());
			continue;
		}

		auto device = new HiDevice(deviceInfo, deviceData);

		if (device->isOpen()) {
			auto caps = device->getCaps();
			devices.push_back(device);
			//spdlog::trace("------------ Device Information ---------");
			//spdlog::trace(L"Name: {}", device->getProductString());
			//spdlog::trace(L"Input Report Length: {}", caps.InputReportByteLength);
			//spdlog::trace(L"Output Report Length: {}", caps.OutputReportByteLength);
			//spdlog::trace(L"Feature Report Length: {}", caps.FeatureReportByteLength);
			//spdlog::trace(L"Link Collection Nodes: {}", caps.NumberLinkCollectionNodes);
			//spdlog::trace(L"Input Button Caps: {}", caps.NumberInputButtonCaps);
			//spdlog::trace(L"Output Button Caps: {}", caps.NumberOutputButtonCaps);
		}

		//spdlog::debug("Finished with HID endpoint #{}", i);

		// Check that there is a "next" device to probe.
		if (!SetupDiEnumDeviceInterfaces(deviceInfo, 0, &gHidGuid, i+1, &deviceData) && ERROR_NO_MORE_ITEMS == GetLastError()) {
			//spdlog::info("Finished enumerating all devices");
			break;
		}


	}

	return devices.size();
}
