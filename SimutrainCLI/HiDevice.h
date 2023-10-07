#pragma once

#include <windows.h>
#include <string>
#include <hidusage.h>
#include <hidsdi.h>
#include <SetupAPI.h>

class HiDevice
{
public:
	HiDevice(HDEVINFO deviceInfo, SP_DEVICE_INTERFACE_DATA interfaceData);
	~HiDevice();
	bool close();
	bool open();
	std::wstring getProductString();
	void getUsages(USAGE usagePage, USHORT linkCollection);
	HIDP_CAPS getCaps();
	bool isOpen();
protected:
	bool _isOpen;
private:
	SP_DEVICE_INTERFACE_DATA interfaces;
	PSP_INTERFACE_DEVICE_DETAIL_DATA detail;
	PHIDP_PREPARSED_DATA ppd;
	HANDLE handle;
	HDEVINFO deviceInfo;
	HIDP_CAPS caps;
	void setup();
};

