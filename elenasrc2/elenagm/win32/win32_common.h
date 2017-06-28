//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Graphic Engine
//
//                                              (C)2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenagm_commonH
#define elenagm_commonH 1

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d2d1_3.h>
#include <d3d11on12.h>
#include "d3dx12.h"
#include <dwrite.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <pix.h>

#include "gplatform.h"

using Microsoft::WRL::ComPtr;

// --- Exception base class ---

struct GMException
{
};

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw GMException();
	}
}

#endif // elenagm_commonH