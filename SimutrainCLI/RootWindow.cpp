#include <cstdio>

#include "imgui.h"
#include "RootWindow.h"
#include "DeviceManager.h"

#include "HidUsageNames.h"

#define KV_PAIR(label, fmt, ...) \
	ImGui::TableNextColumn(); \
	ImGui::Text(label); \
	ImGui::TableNextColumn(); \
	ImGui::Text(fmt, __VA_ARGS__);

static DeviceManager* deviceManager = nullptr;

// Table of device labels
static char labels[255][255];

static ImVec4 COLOR_TOMATO = { 0.9f, 0.3f, 0.2f, 1.0f };

std::string GetErrorString(DWORD errorId);

namespace Ui {
	namespace RootWindow {

		static State state;

		inline void Menu();
		inline void WindowContent();
		inline void ButtonCaps(HIDP_BUTTON_CAPS cap);
		inline void ValueCaps(const PHIDP_VALUE_CAPS cap);

		inline void HexTable(const char *id, const char* data, size_t dataSz);

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

			if (ImGui::Begin("Simutrain Console", &state.open, flags)) {

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

							for (USHORT i = 0; i < caps.NumberInputButtonCaps; i++) {
								device->flush();
								HIDP_BUTTON_CAPS cap = buttonCaps[i];
								char label[32];

								sprintf_s(label, "Input Button Cap: %d", i);
								if (ImGui::TreeNode(label)) {

									if (ImGui::TreeNode("Button Capabilities")) {
										ButtonCaps(cap);
										ImGui::TreePop();
									}

									if (ImGui::TreeNode("Input Report")) {

										size_t inputReportSz = caps.InputReportByteLength;

										if (cap.ReportID > 0) {
											inputReportSz++;
										}

										char* inputReport = (char*)malloc(inputReportSz);
										ZeroMemory(inputReport, inputReportSz);

										if (cap.ReportID > 0) {
											inputReport[0] = cap.ReportID;
										}
										
										if (device->getInputReport(inputReport, inputReportSz)) {
											HexTable("input_report", inputReport, inputReportSz);
										} else {
											auto errorId = GetLastError();
											ImGui::TextColored(COLOR_TOMATO, "Unable to get Input Report: %d %s", errorId, GetErrorString(errorId).c_str());
										}
	
										free(inputReport);

										ImGui::TreePop();
									}

									if (ImGui::TreeNode("Indexed Strings")) {

										if (cap.IsStringRange) {
											for (auto i = cap.Range.StringMin; i < cap.Range.StringMax; i++) {
												char stringLabel[16];

												sprintf_s(stringLabel, "%d", i);
												if (ImGui::TreeNode(stringLabel)) {
													char buffer[256];
													device->getIndexedString(i, buffer, 256);
													ImGui::TreePop();
												}
											}
										}
										ImGui::TreePop();
									}

									ImGui::TreePop();
								}
							}

							free(buttonCaps);
						}
					}

					if (caps.NumberInputValueCaps > 0) {
						if (ImGui::CollapsingHeader("Input Values")) {
							USHORT bufferSz;
							PHIDP_VALUE_CAPS valueCaps = nullptr;

							device->getValueCaps(&valueCaps, &bufferSz);

							for (USHORT i = 0; i < caps.NumberInputValueCaps; i++) {
								char label[32];
								HIDP_VALUE_CAPS cap = valueCaps[i];

								sprintf_s(label, "Value Input Cap: %d", i);
								
								if (ImGui::TreeNode(label)) {

									if (ImGui::TreeNode("Capability Description")) {
										ValueCaps(&cap);
										ImGui::TreePop();
									}

									if (ImGui::TreeNode("Input Report")) {
										auto inputReportSz = caps.InputReportByteLength + 1;
										char* inputReport = (char*)malloc(inputReportSz);
										ZeroMemory(inputReport, inputReportSz);
										if (cap.ReportID > 0) inputReport[0] = cap.ReportID;
										
										if (device->getInputReport(inputReport, inputReportSz)) {
											HexTable("input_report", inputReport, inputReportSz);
										} else {
											auto errorCode = GetLastError();
											ImGui::TextColored(COLOR_TOMATO, "Unable to get Input Report: (%d) %s", errorCode, GetErrorString(errorCode).c_str());
										}
	
										free(inputReport);
										ImGui::TreePop();
									}

									if (ImGui::TreeNode("Indexed Strings")) {

										if (cap.IsStringRange) {
											for (auto i = cap.Range.StringMin; i < cap.Range.StringMax; i++) {
												char stringLabel[16];

												sprintf_s(stringLabel, "%d", i);
												if (ImGui::TreeNode(stringLabel)) {
													char buffer[256];
													device->getIndexedString(i, buffer, 256);
													ImGui::TreePop();
												}
											}
										}

										ImGui::TreePop();
									}

									ImGui::TreePop();
								}
							}

							free(valueCaps);
						}
					}

					if (ImGui::TreeNode("Default Input Report")) {
						char* inputReport = (char*)malloc(1 + caps.InputReportByteLength);
						ZeroMemory(inputReport, 1 + caps.InputReportByteLength);

						if (device->getInputReport(inputReport, caps.InputReportByteLength + 1)) {
							HexTable("input_report", inputReport, caps.InputReportByteLength + 1);
						}
						else {
							auto errorCode = GetLastError();
							ImGui::TextColored(COLOR_TOMATO, "Unable to get Input Report: (%d) %s", errorCode, GetErrorString(errorCode).c_str());
						}

						free(inputReport);
						ImGui::TreePop();
					}

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
				KV_PAIR("Usage Page", "%s (%d)", cap.UsagePage < 0x10 ? UsagePages[cap.UsagePage] : "Unknown", cap.UsagePage);
				ImGui::TableNextRow();
				KV_PAIR("Is Absolute?", "%d", cap.IsAbsolute);
				ImGui::TableNextRow();
				KV_PAIR("Is Range?", "%d", cap.IsRange);
				ImGui::TableNextRow();
				KV_PAIR("Is Alias?", "%d", cap.IsAlias);

				if (cap.IsRange) {
					ImGui::TableNextRow();
					KV_PAIR("Usage Minimum", "%s (0x%x)", getUsageName(cap.UsagePage, cap.Range.UsageMin),  cap.Range.UsageMin);
					ImGui::TableNextRow();
					KV_PAIR("Usage Maximum", "%s (0x%x)", getUsageName(cap.UsagePage, cap.Range.UsageMax), cap.Range.UsageMax);
					ImGui::TableNextRow();


					KV_PAIR("String Minimum", "%d", cap.Range.StringMin);
					ImGui::TableNextRow();
					KV_PAIR("String Maximum", "%d", cap.Range.StringMax);
				} else {
					ImGui::TableNextRow();
					KV_PAIR("Usage ID", "%s (0x%x)", getUsageName(cap.UsagePage, cap.NotRange.Usage), cap.NotRange.Usage);
					ImGui::TableNextRow();
					KV_PAIR("String Index", "0x%x", cap.NotRange.StringIndex);
					ImGui::TableNextRow();
					KV_PAIR("Data Index", "0x%x", cap.NotRange.DataIndex);
					ImGui::TableNextRow();
					KV_PAIR("Designator Index", "0x%x", cap.NotRange.DesignatorIndex);
				}

				ImGui::EndTable();
			}
		}

		inline void ValueCaps(const PHIDP_VALUE_CAPS cap) {
			if (ImGui::BeginTable("value_caps_input", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {

				KV_PAIR("Report ID", "%d", cap->ReportID);
				ImGui::TableNextRow();
				KV_PAIR("Report Count", "%d", cap->ReportCount);
				ImGui::TableNextRow();
				KV_PAIR("Usage Page", "%s (%d)", cap->UsagePage < 0x10 ? UsagePages[cap->UsagePage] : "UNKNOWN", cap->UsagePage);
				ImGui::TableNextRow();
				KV_PAIR("Is Absolute?", "%d", cap->IsAbsolute);
				ImGui::TableNextRow();
				KV_PAIR("Is Range?", "%d", cap->IsRange);
				ImGui::TableNextRow();
				KV_PAIR("Is Alias?", "%d", cap->IsAlias);

				if (cap->IsRange) {
					ImGui::TableNextRow();
					KV_PAIR("Usage Minimum", "%s 0x%x", getUsageName(cap->UsagePage, cap->Range.UsageMin), cap->Range.UsageMin);
					ImGui::TableNextRow();
					KV_PAIR("Usage Maximum", "%s 0x%x", getUsageName(cap->UsagePage, cap->Range.UsageMax), cap->Range.UsageMax);
					ImGui::TableNextRow();
					KV_PAIR("String Minimum", "%d", cap->Range.StringMin);
					ImGui::TableNextRow();
					KV_PAIR("String Maximum", "%d", cap->Range.StringMax);
				} else {
					ImGui::TableNextRow();
					KV_PAIR("Usage ID", "%s (0x%x)", getUsageName(cap->UsagePage, cap->NotRange.Usage), cap->NotRange.Usage);
					ImGui::TableNextRow();
					KV_PAIR("String Index", "0x%x", cap->NotRange.StringIndex);
					ImGui::TableNextRow();
					KV_PAIR("Data Index", "0x%x", cap->NotRange.DataIndex);
					ImGui::TableNextRow();
					KV_PAIR("Designator Index", "0x%x", cap->NotRange.DesignatorIndex);
				}

				ImGui::EndTable();
			}
		}

		inline void HexTable(const char *id, const char* data, size_t dataSz) {
			if (ImGui::BeginTable(id, 17)) {

				ImGui::TableNextColumn();
				for (size_t i = 0; i < 16; i++) {
					ImGui::TableNextColumn();
					ImGui::Text("%X", i);
				}

				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::Text("0x0000");

				for (size_t i = 0; i < dataSz; i++) {
					ImGui::TableNextColumn();
					ImGui::Text("%02x", data[i]);
					
					if (i % 16 == 0 && i > 0) {
						ImGui::TableNextRow();
						ImGui::TableNextColumn() ;
						ImGui::Text("0x%04x", i);
					}
				}

				ImGui::EndTable();
			}
		}
	}
}
 
std::string GetErrorString(DWORD errorId) {
	if (errorId == 0) {
		return std::string();
	}

	LPSTR messageBuffer = nullptr;

	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&messageBuffer,
		0,
		NULL
	);

	std::string message(messageBuffer, size);
	
	LocalFree(messageBuffer);
	return message;
}
