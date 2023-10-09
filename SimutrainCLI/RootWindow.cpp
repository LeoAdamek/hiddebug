#include <cstdio>

#include "imgui.h"
#include "RootWindow.h"
#include "DeviceManager.h"


#define KV_PAIR(label, fmt, val) \
	ImGui::TableNextColumn(); \
	ImGui::Text(label); \
	ImGui::TableNextColumn(); \
	ImGui::Text(fmt, val);

static DeviceManager* deviceManager = nullptr;

// Table of device labels
static char labels[255][255];

namespace Ui {
	namespace RootWindow {

		static State state;

		inline void Menu();
		inline void WindowContent();
		inline void ButtonCaps(HIDP_BUTTON_CAPS cap);

		void InitDevices();

		void Render() {


			static ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize

				| ImGuiWindowFlags_NoSavedSettings
				| ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoDecoration
				| ImGuiWindowFlags_MenuBar;

			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);

			if (ImGui::Begin("Simutrian Console", &state.open, flags)) {

				if (deviceManager == nullptr) {
					InitDevices();
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
			static size_t selected = 0;
			{
				if (ImGui::BeginChild("device_list", ImVec2(200, 0), true)) {
					ImGui::Text("Total Devices: %d", state.deviceCount);
					ImGui::Separator();

					size_t i = 0;
					for (i = 0; i < state.deviceCount; i++) {
						if (ImGui::Selectable(labels[i], selected == i)) {
							selected = i;
						}
					}

					ImGui::EndChild();
				}
			}

			ImGui::SameLine();

			{
				ImGui::BeginGroup();
				if (ImGui::BeginChild("device_view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true)) {

					ImGui::Text("Device: %s", labels[selected]);
					ImGui::Separator();

					HiDevice* device = deviceManager->devices.at(selected);
					auto caps = device->getCaps();

					// Capabilities table
					if (ImGui::CollapsingHeader("Capabilities")) {
						if (ImGui::BeginTable("capabilities", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {

							ImGui::TableNextColumn(); ImGui::Text("Input Report Length"); ImGui::TableNextColumn(); ImGui::Text("%d", caps.InputReportByteLength);
							ImGui::TableNextRow();
							ImGui::TableNextColumn(); ImGui::Text("Feature Report Length"); ImGui::TableNextColumn(); ImGui::Text("%d", caps.FeatureReportByteLength);
							ImGui::TableNextRow();
							ImGui::TableNextColumn(); ImGui::Text("Output Report Length"); ImGui::TableNextColumn(); ImGui::Text("%d", caps.OutputReportByteLength);

							ImGui::TableNextRow();
							ImGui::TableNextColumn(); ImGui::Text("Input Button Caps"); ImGui::TableNextColumn(); ImGui::Text("%d", caps.NumberInputButtonCaps);
							ImGui::TableNextRow();
							ImGui::TableNextColumn(); ImGui::Text("Input Value Caps"); ImGui::TableNextColumn(); ImGui::Text("%d", caps.NumberInputValueCaps);
							ImGui::TableNextRow();
							ImGui::TableNextColumn(); ImGui::Text("Input Data Indicies"); ImGui::TableNextColumn(); ImGui::Text("%d", caps.NumberInputDataIndices);

							ImGui::TableNextRow();
							ImGui::TableNextColumn(); ImGui::Text("Output Button Caps"); ImGui::TableNextColumn(); ImGui::Text("%d", caps.NumberOutputButtonCaps);
							ImGui::TableNextRow();
							ImGui::TableNextColumn(); ImGui::Text("Output Value Caps"); ImGui::TableNextColumn(); ImGui::Text("%d", caps.NumberOutputValueCaps);
							ImGui::TableNextRow();
							ImGui::TableNextColumn(); ImGui::Text("Output Data Indicies"); ImGui::TableNextColumn(); ImGui::Text("%d", caps.NumberOutputDataIndices);

							ImGui::EndTable();
						}
					}

					if (caps.NumberInputButtonCaps > 0) {
						char label[128];

						sprintf_s(label, "Input Buttons: (%d)", caps.NumberInputButtonCaps);

						if (ImGui::CollapsingHeader(label)) {
							USHORT bufferSz;
							PHIDP_BUTTON_CAPS buttonCaps = nullptr;

							device->getButtonCaps(&buttonCaps, &bufferSz);

							for (USHORT i = 0; i < bufferSz; i++) {
								HIDP_BUTTON_CAPS cap = buttonCaps[i];
								char label[32];

								sprintf_s(label, "Usage Page: % d", cap.UsagePage);
								if (ImGui::TreeNode(label)) {

									if (ImGui::TreeNode("Button Capabilities")) {
										ButtonCaps(cap);
										ImGui::TreePop();
									}

									if (ImGui::TreeNode("Input Report")) {
										ImGui::TreePop();
									}

									ImGui::TreePop();
								}
							}

							free(buttonCaps);
						}
					}

					//HidP_GetUsages(HidP_Input, 1)

					ImGui::EndChild();
				}
				ImGui::EndGroup();
			}
		}

		void InitDevices() {
			deviceManager = new DeviceManager();
			state.deviceCount = deviceManager->loadDevices();
			size_t i = 0;
			for (auto device : deviceManager->devices) {
				std::wstring deviceName = device->getProductString();
				sprintf_s(labels[i], "#%zu %ls", i, deviceName.c_str());

				i++;
			}
		}


		void Teardown() {
			for (auto device : deviceManager->devices) {
				device->close();
			}

			deviceManager->devices.clear();
			deviceManager->~DeviceManager();
		}

		inline void ButtonCaps(HIDP_BUTTON_CAPS cap) {
			if (ImGui::BeginTable("button_caps_input", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {

				KV_PAIR("Report ID", "%d", cap.ReportID);
				ImGui::TableNextRow();
				KV_PAIR("Report Count", "%d", cap.ReportCount);
				ImGui::TableNextRow();
				KV_PAIR("Bit Field", "0x%x", cap.BitField);
				ImGui::TableNextRow();
				KV_PAIR("Usage Page", "%d", cap.UsagePage);
				ImGui::TableNextRow();
				KV_PAIR("Is Absolute?", "%d", cap.IsAbsolute);
				ImGui::TableNextRow();
				KV_PAIR("Is Range?", "%d", cap.IsRange);

				if (cap.IsRange) {
					ImGui::TableNextRow();
					KV_PAIR("Usage Minimum", "%d", cap.Range.UsageMin);
					ImGui::TableNextRow();
					KV_PAIR("Usage Maximum", "%d", cap.Range.UsageMax);
					ImGui::TableNextRow();
					KV_PAIR("String Minimum", "%d", cap.Range.StringMin);
					ImGui::TableNextRow();
					KV_PAIR("String Maximum", "%d", cap.Range.StringMax);
				}

				ImGui::EndTable();
			}
		}

	}
}

