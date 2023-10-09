#include "HiDevice.h"

#include <string>
#include <assert.h>
#include <hidpi.h>

HiDevice::HiDevice(HDEVINFO deviceInfo, SP_DEVICE_INTERFACE_DATA interfaceData) {
	this->deviceInfo = deviceInfo;
	this->interfaces = interfaceData;
	this->handle = INVALID_HANDLE_VALUE;

	this->setup();
}

HiDevice::~HiDevice() {
	if (_isOpen) {
		close();
	}

	if (ppd != NULL) {
		HidD_FreePreparsedData(ppd);
	}
}

void HiDevice::setup() {
	ULONG reqSz = 0, bufferSz = 0;
	SetupDiGetDeviceInterfaceDetail(deviceInfo, &interfaces, NULL, 0, &reqSz, NULL);
	bufferSz = reqSz;

	//spdlog::trace("Loading device interface detail");

	detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(bufferSz);
	detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	ZeroMemory(detail->DevicePath, bufferSz);
	SetupDiGetDeviceInterfaceDetail(deviceInfo, &interfaces, detail, bufferSz, &reqSz, NULL);
	assert(bufferSz == reqSz);
	//spdlog::trace("Loaded device interface detail (Size {})", bufferSz);

	//spdlog::trace("Opening device handle");
	if (open()) {
		HidD_GetPreparsedData(handle, &ppd);
		HidP_GetCaps(ppd, &caps);
	}
}

HIDP_CAPS HiDevice::getCaps() {
	return caps;
}

std::wstring HiDevice::getProductString() {
	if (!_isOpen) {
		return L"";

	}

	//spdlog::trace("Getting product string for device");

	ULONG bufferSz = 255 * sizeof(CHAR);
	LPWSTR buffer = (LPWSTR)malloc(bufferSz);
	std::wstring str;
	ZeroMemory(buffer, bufferSz);

	if (HidD_GetProductString(handle, buffer, bufferSz)) {
		str = buffer;
		free(buffer);
	}
	return str;
}

bool HiDevice::isOpen() {
	return _isOpen && handle != INVALID_HANDLE_VALUE;
}

bool HiDevice::close() {
	if (this->handle != INVALID_HANDLE_VALUE) {
		if (CloseHandle(this->handle)) {
			this->handle = INVALID_HANDLE_VALUE;
			_isOpen = false;
			return true;
		}
	}

	return false;
}

bool HiDevice::open() {
	DWORD accessMode = GENERIC_READ | GENERIC_WRITE,
		shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	if (detail->DevicePath == NULL) {
		//spdlog::error("Unable to open device. No path to device.");
		return false;
	}

	handle = CreateFile(
		detail->DevicePath,
		accessMode,
		shareMode,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if (handle == INVALID_HANDLE_VALUE) {
		//spdlog::error("Unable to open device: {}", GetLastError());
		return false;
	}

	_isOpen = true;

	return true;
}

bool HiDevice::getUsages(
	HIDP_REPORT_TYPE reportType,
	USAGE usagePage,
	USHORT linkCollection,
	USAGE **usageList,
	PULONG usageListSz,
	CHAR** report,
	PULONG reportSz
) {
	return false;
}

NTSTATUS HiDevice::getButtonCaps(PHIDP_BUTTON_CAPS* buffer, PUSHORT bufferSz) {

	*bufferSz = caps.NumberInputButtonCaps;
	*buffer = (PHIDP_BUTTON_CAPS)calloc(*bufferSz, sizeof(HIDP_BUTTON_CAPS));
	
	return HidP_GetButtonCaps(HidP_Input, *buffer, bufferSz, ppd);
}

PHIDP_PREPARSED_DATA HiDevice::getPreparsedData() {
	return ppd;
}
