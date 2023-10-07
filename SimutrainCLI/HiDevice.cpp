#include "HiDevice.h"
#include "spdlog/spdlog.h"

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

	spdlog::trace("Loading device interface detail");

	detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(bufferSz);
	detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	ZeroMemory(detail->DevicePath, bufferSz);
	SetupDiGetDeviceInterfaceDetail(deviceInfo, &interfaces, detail, bufferSz, &reqSz, NULL);
	assert(bufferSz == reqSz);
	spdlog::trace("Loaded device interface detail (Size {})", bufferSz);

	spdlog::trace("Opening device handle");
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

	spdlog::trace("Getting product string for device");

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

void HiDevice::getUsages(USAGE usagePage, USHORT linkCollection) {
	ULONG reportSz = caps.FeatureReportByteLength,
		usageSz = reportSz * sizeof(USAGE);

	USAGE *usageList = (USAGE*)malloc(usageSz);
	PCHAR report = (PCHAR)malloc(reportSz);

	ZeroMemory(report, reportSz);

	NTSTATUS usages = HidP_GetUsages(HidP_Feature, usagePage, linkCollection, usageList, &usageSz, ppd, report, 0);

	if (usages == HIDP_STATUS_SUCCESS) {
		spdlog::info("Got usages report: (R: {}, U: {})", reportSz, usageSz);
	} else {
		std::string errorStr;

		switch (usages) {
		case HIDP_STATUS_INVALID_REPORT_LENGTH:
			errorStr = "invalid report length";
			break;
		case HIDP_STATUS_INVALID_REPORT_TYPE:
			errorStr = "invalid report type";
			break;
		case HIDP_STATUS_BUFFER_TOO_SMALL:
			errorStr = "buffer too small";
			break;
		case HIDP_STATUS_INVALID_PREPARSED_DATA:
			errorStr = "invalid preparsed data";
			break;
		case HIDP_STATUS_INCOMPATIBLE_REPORT_ID:
			errorStr = "incompatible report id";
			break;
		case HIDP_STATUS_USAGE_NOT_FOUND:
			errorStr = "usage not found";
			break;
		default:
			errorStr = "some other error";
			break;
		}

		spdlog::warn("Unable to get usages: {}", errorStr);
	}
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
		spdlog::error("Unable to open device. No path to device.");
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
		spdlog::error("Unable to open device: {}", GetLastError());
		return false;
	}

	_isOpen = true;

	return true;
}
