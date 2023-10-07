#include "RootWindow.h"

#pragma comment (lib, "d3d11.lib")

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>

#include <tchar.h> 

static ID3D11Device* d3dDevice = nullptr;
static ID3D11DeviceContext* d3dDeviceContext = nullptr;
static IDXGISwapChain* d3dSwapChain = nullptr;
static UINT resizeWidth = 0, resizeHeight = 0;
static ID3D11RenderTargetView* mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND handle);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam);

Ui::RootWindow::RootWindow() {
}

Ui::RootWindow::~RootWindow() { 

}

void Ui::RootWindow::initialize() {
}

Ui::InitResult Ui::Startup() {
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Simutrain Console", nullptr };
	::RegisterClassExW(&wc);
	HWND handle = ::CreateWindowW(wc.lpszClassName, L"Simutrian Console", WS_OVERLAPPEDWINDOW, 100, 100, 1200, 800, nullptr, nullptr, wc.hInstance, nullptr);

	if (!CreateDeviceD3D(handle)) {
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return Ui::InitResult::ERR_D3D_FAILURE;
	}

	::ShowWindow(handle, SW_SHOWDEFAULT);
	::UpdateWindow(handle);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplDX11_Init(d3dDevice, d3dDeviceContext);

	return Ui::InitResult::UI_INIT_OK;
}


// Forward declare message handler from imgui_impl_win32.cpp
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/** Window Message Handler
 *
 * Handles messages sent to the application from Windows.
 */
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
		// Handle window resize messages
	case WM_SIZE:
		// Handle minimizing by doing nothing
		if (wParam == SIZE_MINIMIZED) {
			return 0;
		}

		// Make resize
		resizeWidth = (UINT)LOWORD(lParam);
		resizeHeight = (UINT)HIWORD(lParam);
		return 0;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool CreateDeviceD3D(HWND hWnd) {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &d3dSwapChain, &d3dDevice, &featureLevel, &d3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &d3dSwapChain, &d3dDevice, &featureLevel, &d3dDeviceContext);
    if (res != S_OK)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (d3dSwapChain) { d3dSwapChain->Release(); d3dSwapChain = nullptr; }
    if (d3dDeviceContext) { d3dDeviceContext->Release(); d3dDeviceContext = nullptr; }
    if (d3dDevice) { d3dDevice->Release(); d3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    d3dSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (mainRenderTargetView) { mainRenderTargetView->Release(); mainRenderTargetView = nullptr; }
}

