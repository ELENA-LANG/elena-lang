#include "elena.h"
// --------------------------------------------------------------------------

#include <windows.h>
#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_4.h>

using Microsoft::WRL::ComPtr;

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

// global declarations
IDXGISwapChain1*	swapchain;             // the pointer to the swap chain interface
ID3D12Device*		devD12;                     // the pointer to our Direct3D device interface
ID3D12CommandQueue*	commandQueue;
//ID3D11Device* dev;
//ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context

// === dll entries ===

/// sets up and initializes Direct3D
EXTERN_DLL_EXPORT void InitD3D(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);

	UINT dxgiFactoryFlags = 0;

	ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));

	ComPtr<IDXGIAdapter> warpAdapter;
	factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));

	D3D12CreateDevice(
		warpAdapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&devD12)
	);

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	devD12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	//NAME_D3D12_OBJECT(commandQueue);

	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC1 scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC1));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.Width = rect.right - rect.left;
	scd.Height = rect.bottom - rect.top;
	scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.SampleDesc.Count = 4;                               // how many multisamples
	//scd.OutputWindow = hWnd;                                // the window to be used
	//scd.Windowed = TRUE;                                    // windowed/full-screen mode

	factory->CreateSwapChainForHwnd(
		commandQueue,		// Swap chain needs the queue so that it can force a flush on it.
		hWnd,
		&scd,
		nullptr,
		nullptr,
		&swapchain
	);
}

/// closes Direct3D and releases memory
EXTERN_DLL_EXPORT void CleanD3D()
{
	// close and release all existing COM objects
	swapchain->Release();
	devD12->Release();
	commandQueue->Release();
	//devcon->Release();
}

// --- dllmain ---

extern "C"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
               )
{
/*   switch (ul_reason_for_call)
   {
   case DLL_PROCESS_ATTACH:
      newSession(hModule);
      return TRUE;
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      return TRUE;
   case DLL_PROCESS_DETACH:
      freeSession();
      break;
   }*/
   return TRUE;
}
