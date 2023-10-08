#include <cstdio>

#include "imgui.h"
#include "RootWindow.h"
#include "DeviceManager.h"

static DeviceManager* deviceManager = nullptr;

namespace Ui {
	namespace RootWindow {

		static State state;

		inline void Menu();
		inline void WindowContent();

		void Render() {


			static ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize

				| ImGuiWindowFlags_NoSavedSettings
				| ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoDecoration
				| ImGuiWindowFlags_MenuBar;

			// We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
			// Based on your use case you may want one or the other.
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);

			if (ImGui::Begin("Simutrian Console", &state.open, flags)) {

				if (deviceManager == nullptr) {
					deviceManager = new DeviceManager();
					state.deviceCount = deviceManager->loadDevices();
				}

				Menu();
				WindowContent();
				ImGui::End();

				if (state.imguiDemo) ImGui::ShowDemoWindow(&state.imguiDemo);
				if (state.debugEnabled) ImGui::ShowDebugLogWindow(&state.debugEnabled);
				if (state.statsEnabled) ImGui::ShowMetricsWindow(&state.statsEnabled);
			}
		}

		inline void Menu() {
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("Debug")) {
					ImGui::MenuItem("ImGui Debugger", NULL, &state.debugEnabled);
					ImGui::MenuItem("ImGui Stats", NULL, &state.statsEnabled);
					ImGui::Separator();
					ImGui::MenuItem("ImGui Demo", NULL, &state.imguiDemo);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
		}

		inline void WindowContent() {

			// Left Side
			static int selected = 0;
			{
				if (ImGui::BeginChild("device_list", ImVec2(200, 0), true)) {
					ImGui::Text("Total Devices: %d", state.deviceCount);
					ImGui::Separator();

					size_t i = 0;
					for (auto device : deviceManager->devices) {
						char label[256];

						sprintf_s(label, "Device #%d", i);

						if (ImGui::Selectable(label, selected == i)) {
							selected = i;
						}
						i++;
					}

					ImGui::EndChild();
				}
			}

			ImGui::SameLine();

			{
				ImGui::BeginGroup();
				if (ImGui::BeginChild("device_view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true)) {

					ImGui::Text("Device: %d", selected);
					ImGui::Separator();

					HiDevice* device = deviceManager->devices.at(selected);
					std::wstring productName = device->getProductString();
					ImGui::Text("Device Name: %ls", productName.c_str());

					ImGui::EndChild();
				}
				ImGui::EndGroup();
			}
		}

	}
}
