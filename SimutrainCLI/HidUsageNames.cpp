#include "HidUsageNames.h"


char usageUnknown[32] = "UNKNOWN";

char* getUsageName(USHORT usagePage, USAGE usageId) {
	for (size_t i = 0; i < USAGES_LENGTH; i++) {
		LUT_ENTRY entry = usages[i];

		if (entry.usagePage == usagePage && entry.min >= usageId && entry.max <= usageId) {
			return usages[i].value;
		}
	}
	return usageUnknown;
}
