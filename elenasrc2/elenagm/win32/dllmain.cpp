#include "elena.h"
// --------------------------------------------------------------------------
#include "win32_common.h"
#include "directx12.h"
#include "d2platform.h"

using namespace _ELENA_;

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

//Path			rootPath;
GraphicPlatform*	platform = NULL;
Model               model;

//void loadDLLPath(HMODULE hModule, Path& rootPath)
//{
//	TCHAR path[MAX_PATH + 1];
//
//	::GetModuleFileName(hModule, path, MAX_PATH);
//
//	rootPath.copySubPath(path);
//	rootPath.lower();
//}

// === dll entries ===

/// sets up and initializes Direct3D
EXTERN_DLL_EXPORT int InitD3D(HWND hWnd)
{
	try
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);

		platform = new D2Platform(&model);

		platform->Init(hWnd);

		return -1;
	}
	catch (...)
	{
		return 0;
	}
}

EXTERN_DLL_EXPORT void Render3D(HWND hWnd)
{
	platform->OnRender(hWnd);
}

/// closes Direct3D and releases memory
EXTERN_DLL_EXPORT void CleanD3D(HWND hWnd)
{
	platform->OnDestroy();

	delete platform;

	platform = nullptr;
}

EXTERN_DLL_EXPORT void* NewWidget(void* parent, int type)
{
	return model.NewWidget(parent, type);
}

EXTERN_DLL_EXPORT int CloseWidget(void* handle)
{
	return model.CloseWidget(handle);
}

EXTERN_DLL_EXPORT int SetLocation(void* handle, int x, int y)
{
	return model.SetLocation(handle, x, y);
}

EXTERN_DLL_EXPORT int SetSize(void* handle, int width, int height)
{
	return model.SetSize(handle, width, height);
}

EXTERN_DLL_EXPORT int SetText(void* handle, const wchar_t* text)
{
	return model.SetText(handle, text);
}

EXTERN_DLL_EXPORT int SetNProperty(void* handle, int property, int value)
{
   return model.SetNProperty(handle, property, value);
}

EXTERN_DLL_EXPORT int GetNProperty(void* handle, int property, int defValue)
{
   return model.GetNProperty(handle, property, defValue);
}

// --- dllmain ---

extern "C"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
               )
{
   switch (ul_reason_for_call)
   {
//	   case DLL_PROCESS_ATTACH:
//		  loadDLLPath(hModule, rootPath);
//		  return TRUE;
//	   case DLL_PROCESS_DETACH:
//		  freeobj(d12Platform);
//		  break;
   }
   return TRUE;
}
