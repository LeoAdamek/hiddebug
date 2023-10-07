#pragma once

namespace Ui {
	class RootWindow
	{
	public:
		RootWindow();
		void initialize();
		~RootWindow();

	private:
		bool active;
	};

	typedef enum InitResult {
		UI_INIT_OK,
		ERR_D3D_FAILURE
	} InitResult;

	InitResult Startup();
}

