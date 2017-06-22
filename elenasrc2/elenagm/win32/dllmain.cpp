#include "elena.h"
// --------------------------------------------------------------------------
#include "win32_common.h"
#include "directx12.h"

//
//using Microsoft::WRL::ComPtr;

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

D12Platform* d12Platform = NULL;

//// global declarations
//IDXGISwapChain1*	swapchain;             // the pointer to the swap chain interface
//ID3D12Device*		devD12;                     // the pointer to our Direct3D device interface
//ID3D12CommandQueue*	commandQueue;
////ID3D11Device* dev;
////ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context

// === dll entries ===

/// sets up and initializes Direct3D
EXTERN_DLL_EXPORT int InitD3D(HWND hWnd)
{
	try
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);

		d12Platform = new D12Platform(rect.right - rect.left, rect.bottom - rect.top);

		d12Platform->Init(hWnd, 1);
		d12Platform->LoadAssets();

		return -1;
	}
	catch (...)
	{
		return 0;
	}
}

EXTERN_DLL_EXPORT void Render3D(HWND hWnd)
{
	d12Platform->OnRender();
}

/// closes Direct3D and releases memory
EXTERN_DLL_EXPORT void CleanD3D(HWND hWnd)
{
	d12Platform->OnDestroy();

	delete d12Platform;
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
