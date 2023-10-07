#include <cstdlib>
#include <iostream>

#include "spdlog/spdlog.h"
#include "DeviceManager.h"
#include "RootWindow.h"

static bool done;

int main(const int argc, const char** argv) {

	if (Ui::Startup() == Ui::UI_INIT_OK) {
		while (!done) {
			MSG msg;

			while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT) {
					done = true;
				}
			}

			if (done) {
				break;
			}
		}
	}

	return 0;
}
 