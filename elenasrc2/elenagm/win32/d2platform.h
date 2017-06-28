//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Graphic Engine
//
//                                              (C)2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenagm_d2H
#define elenagm_d2H 1

#include "elena.h"
#include "win32_common.h"

using namespace DirectX;

namespace _ELENA_
{

class D2Platform : public GraphicPlatform
{
	ComPtr<ID2D1Factory3>				_d2dFactory;
	ComPtr<ID2D1HwndRenderTarget>		_renderTarget;
	ComPtr<IDWriteFactory>				_dWriteFactory;

	// Initialize device-dependent resources.
	void CreateDeviceResources(HWND hWnd);

	// Release device-dependent resource.
	void DiscardDeviceResources();

	void RenderWidget(BaseWidget* widget, D2D1_RECT_F parentRect);
	void RenderWidget();

public:
	D2Platform(Model* model);
	virtual ~D2Platform() {}

	virtual void Init(HWND hWnd);

	virtual void OnRender(HWND hWnd);
	virtual void OnDestroy();
};

} // _ELENA_

#endif // elenagm_d2H