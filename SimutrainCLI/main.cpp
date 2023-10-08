#include <cstdlib>
#include <iostream>

#include "DeviceManager.h"
#include "ui.h"

int main(const int argc, const char** argv) {

	if (Ui::Startup() == Ui::UI_INIT_OK) {
		
		Ui::Loop();

		Ui::Teardown();
	}

	return 0;
}
 