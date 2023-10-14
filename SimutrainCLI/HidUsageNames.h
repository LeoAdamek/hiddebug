#pragma once

#include <Windows.h>
#include <hidusage.h>

static char UsagePages[][32] {
	"RESERVED",
	"Generic Desktop",
	"Simulation Controls",
	"VR Controls",
	"Sport Controls",
	"Game Controls",
	"Generic Device Controls",
	"Keyboard/Keypad",
	"LED",
	"Button",
	"Ordinal",
	"Telephony",
	"Consumer",
	"Digitizer",
	"Haptics",
	"PID",
	"Unicode",
	"RESERVED",
	"Eye & Head Tracker",
	"RESERVED",
	"Auxiliary Display",
	"RESERVED",
	"Sensors",
	"RESERVED",
	"Medical Instrument",
	"Braille Display",
	"RESERVED",
	"Lighting and Illumination Page",
	"RESERVED"
};


typedef struct {
	USHORT usagePage;
	USHORT min, max;
	char value[32];
} LUT_ENTRY;

static LUT_ENTRY usages[] {
	{ 0, 0, 0, "UNDEFINED" },
	
	// Generic Desktop
	{ 0x01, 0x00, 0x00, "UNDEFINED" },
	{ 0x01, 0x01, 0x01, "Pointer" },
	{ 0x01, 0x02, 0x02, "Mouse" },
	{ 0x01, 0x03, 0x03, "RESERVED" },
	{ 0x01, 0x04, 0x04, "Joystick" },
	{ 0x01, 0x05, 0x05, "Gamepad" },
	{ 0x01, 0x06, 0x06, "Keyboard" },
	{ 0x01, 0x07, 0x07, "Keypad" },
	{ 0x01, 0x08, 0x08, "Multi-Axis Controler" },
	{ 0x01, 0x09, 0x09, "Tablet PC Controls" },
	{ 0x01, 0x0A, 0x0A, "Water Cooling Device" },
	{ 0x01, 0x0B, 0x0B, "Computer Chassis" },
	{ 0x01, 0x0C, 0x0C, "Wirelress Radio Controls" },
	{ 0x01, 0x0D, 0x0D, "Portable Device Controls" },
	{ 0x01, 0x0E, 0x0E, "System Multi-Axis Controller" },
	{ 0x01, 0x0F, 0x0F, "Spatial Controller" },
	{ 0x01, 0x10, 0x10, "Assistive Control" },
	{ 0x01, 0x11, 0x11, "Device Dock" },
	{ 0x01, 0x12, 0x12, "Dockable Device" },
	{ 0x01, 0x13, 0x2F, "RESERVED" },
	{ 0x01, 0x30, 0x30, "X"},
	{ 0x01, 0x31, 0x31, "Y"},
	{ 0x01, 0x32, 0x32, "Z"},
	{ 0x01, 0x33, 0x33, "Rx"},
	{ 0x01, 0x34, 0x34, "Ry"},
	{ 0x01, 0x35, 0x35, "Rz"},
	{ 0x01, 0x36, 0x36, "Slider"},
	{ 0x01, 0x37, 0x37, "Dial"},
	{ 0x01, 0x38, 0x38, "Wheel" },
	{ 0x01, 0x39, 0x39, "Hat Switch" },

	// Buttons, all usage IDs for buttons are just their indices
	{ 0x09, 0x00, 0xFFFF, "Button" },

	// Consumer Controls
	{ 0x0C, 0x00, 0x00, "UNDEFINED" },
	{ 0x0C, 0x01, 0x01, "Consumer Control" },
	{ 0x0C, 0x02, 0x02, "Numeric Key Pad" },
	{ 0x0C, 0x03, 0x03, "Programmable Buttons" },
	{ 0x0C, 0x04, 0x04, "Microphone" },
	{ 0x0C, 0x05, 0x05, "Headphone" },
	{ 0x0C, 0x06, 0x06, "Graphic Equalizer" },
	{ 0x0C, 0x07, 0x1F, "RESERVED" },
	{ 0x0C, 0x20, 0x20, "+10" },
	{ 0x0C, 0x21, 0x21, "+100" },
	{ 0x0C, 0x22, 0x22, "AM/PM" },
	{ 0x0C, 0x23, 0x2F, "RESERVED" },
	{ 0x0C, 0x30, 0x30, "Power" },
	{ 0x0C, 0x31, 0x31, "Reset" },
	{ 0x0C, 0x32, 0x32, "Sleep" },

	// Physical Input Devices (PIDs)
	{ 0x0F, 0x01, 0x01, "Physical Interface Device" },
	{ 0x0F, 0x02, 0x1F, "RESERVED" },
	{ 0x0F, 0x20, 0x20, "Normal" },
	{ 0x0F, 0x21, 0x21, "Effect Set Report" },
	{ 0x0F, 0x22, 0x22, "Effect Block Index" },
	{ 0x0F, 0x23, 0x23, "Parameter Block Offset" },
	{ 0x0F, 0x24, 0x24, "ROM Flag" },
	{ 0x0F, 0x25, 0x25, "Effect Type" },
	{ 0x0F, 0x26, 0x26, "ET Constant Force" },
	{ 0x0F, 0x27, 0x27, "ET Ramp" },
	{ 0x0F, 0x28, 0x28, "ET Custom Force Data" },
	{ 0x0F, 0x29, 0x2F, "RESERVED" },
	//...
	{ 0x0F, 0x9D, 0x9E, "RESERVED" },
	{ 0x0F, 0x9F, 0x9F, "Device Paused" },
	{ 0x0F, 0xA0, 0xA0, "Actuators Enabled" },
	{ 0x0F, 0xA1, 0xA3, "RESERVED" },
	{ 0x0F, 0xA4, 0xA4, "Safety Switch" },
	{ 0x0F, 0xA5, 0xA5, "Actuator Override Switch" },
	{ 0x0F, 0xA6, 0xA6, "Actuator Power" },
	{ 0x0F, 0xAB, 0xFFFF, "RESERVED" },
};

#define USAGES_LENGTH (sizeof(usages) / sizeof(LUT_ENTRY))

char* getUsageName(USHORT usagePage, USAGE usageId);
