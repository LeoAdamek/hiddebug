#pragma once

#include "imgui.h"

namespace Ui {
	namespace RootWindow {
		void Render();
		void Teardown();

		typedef struct State {
			bool open = 0;
			bool debugEnabled = 0;
			bool statsEnabled = 0;
			bool imguiDemo = 0;
			int selectedDevice = -1;
			size_t deviceCount = 0;
		} State;
	}
}
