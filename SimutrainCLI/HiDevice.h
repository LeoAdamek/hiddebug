#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <hidusage.h>
#include <hidsdi.h>
#include <SetupAPI.h>

typedef enum _HID_DATA_TYPE {
	DATA_TYPE_BUTTON,
	DATA_TYPE_VALUE
} HID_DATA_TYPE;

typedef struct _HID_DATA {
	HID_DATA_TYPE isbutton;
	UCHAR reserved;
	USAGE usagePage;
	ULONG status;
	ULONG reportId;
	bool isSet;

	union {
		struct {
			ULONG usageMin;
			ULONG usageMax;
			ULONG maxUsageLength;
			PUSAGE usages;
		} ButtonData;

		struct {
			USAGE usage;
			USHORT reserved;
			ULONG value;
			LONG scaledValue;
		} ValueData;
	};

} HID_DATA, *PHID_DATA;

class HiDevice
{
public:
	HiDevice(HDEVINFO deviceInfo, SP_DEVICE_INTERFACE_DATA interfaceData);
	~HiDevice();
	bool close();
	bool open();

	// Get the device product string
	std::wstring getProductString();

	// Get the device capabilities
	HIDP_CAPS getCaps();

	// Check if the device is open
	bool isOpen();

	NTSTATUS getButtonCaps(
		OUT _Out_writes_bytes_(bufferSz * sizeof(HIDP_BUTTON_CAPS)) PHIDP_BUTTON_CAPS* buffer,
		IN OUT PUSHORT bufferSz
	);

	NTSTATUS getValueCaps(
		OUT _Out_writes_bytes_(bufferSz * sizeof(HIDP_VALUE_CAPS)) PHIDP_VALUE_CAPS* buffer,
		IN OUT PUSHORT bufferSz
	);

	bool flush(void);

	// Get Usages
	bool getUsages(
		IN HIDP_REPORT_TYPE reportType,
		IN USAGE usagePage,
		IN USHORT linkCollection,
		OUT USAGE** usageList,
		IN OUT PULONG usageListSz,
		OUT CHAR** report,
		IN OUT PULONG reportSz
	);


	bool getInputReport(OUT void *buffer, ULONG bufferSz);
	bool getFeature(OUT void* buffer, ULONG bufferSz);
	bool getIndexedString(ULONG index, OUT PVOID buffer, IN ULONG bufferSz);
protected:
	bool _isOpen;
private:
	void setup();

	SP_DEVICE_INTERFACE_DATA interfaces;
	PSP_INTERFACE_DEVICE_DETAIL_DATA detail;
	PHIDP_PREPARSED_DATA ppd;
	HANDLE handle;
	HDEVINFO deviceInfo;
	HIDP_CAPS caps;
	struct _HID_DATA *data;
};


