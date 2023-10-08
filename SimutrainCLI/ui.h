#pragma once

namespace Ui {
	typedef enum InitResult {
		UI_INIT_OK,
		ERR_D3D_FAILURE
	} InitResult;

	// Initalize the UI system
	InitResult Startup();

	// Teardown the UI system in preparation for application exit
	void Teardown();

	// UI Main Loop
	void Loop();
}

