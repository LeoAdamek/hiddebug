#pragma once
#pragma comment (lib, "SetupAPI.lib")
#pragma comment (lib, "HID.lib")

#include <windows.h>
#include <hidusage.h>
#include <hidsdi.h>
#include <SetupAPI.h>
#include <string>
#include <vector>

#include "HiDevice.h"

static GUID gHidGuid;

class DeviceManager
{
public:
	DeviceManager();
	~DeviceManager();
	size_t loadDevices();
	std::vector<HiDevice*> devices;
private:
	HMODULE hidModuleHandle;
};

