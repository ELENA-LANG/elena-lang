//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Graphic Engine
//
//                                              (C)2017, by Alexei Rakov
//											    (C)2015, Microsoft	
//---------------------------------------------------------------------------

//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "d2platform.h"

using namespace _ELENA_;

D2Platform :: D2Platform(Model* model)
{
	_model = model;
}

void D2Platform:: Init(HWND hWnd)
{
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

	ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &_d2dFactory));
	
	// Query the desktop's dpi settings, which will be used to create
	// D2D's render targets.
	float dpiX;
	float dpiY;
	_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
	//D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
	//	D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
	//	D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
	//	dpiX,
	//	dpiY
	//);
	
//	D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
	ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &_dWriteFactory));
}

void D2Platform :: CreateDeviceResources(HWND hWnd)
{
	if (!_renderTarget) {
		RECT rc;
		GetClientRect(hWnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		ThrowIfFailed(_d2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hWnd,
				size
			),
			&_renderTarget));
	}
}

void D2Platform :: DiscardDeviceResources()
{
	_renderTarget.Reset();
}

void D2Platform :: OnRender(HWND hWnd)
{
	CreateDeviceResources(hWnd);

	_renderTarget->BeginDraw();
	_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Aqua));

	RenderWidget();

	//static const WCHAR text[] = L"11On12";

	//D2D1_SIZE_F rtSize = _renderTarget->GetSize();

	//D2D1_RECT_F textRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);

	//ComPtr<ID2D1SolidColorBrush>		textBrush;
	//ComPtr<IDWriteTextFormat>			textFormat;

	//// Create a blue brush.
	//ThrowIfFailed(_renderTarget->CreateSolidColorBrush(
	//	D2D1::ColorF(D2D1::ColorF::Black),
	//	&textBrush
	//));

	//ThrowIfFailed(_dWriteFactory->CreateTextFormat(
	//	L"Verdana",
	//	NULL,
	//	DWRITE_FONT_WEIGHT_NORMAL,
	//	DWRITE_FONT_STYLE_NORMAL,
	//	DWRITE_FONT_STRETCH_NORMAL,
	//	50,
	//	L"en-us",
	//	&textFormat
	//));

	//_renderTarget->DrawText(
	//	text,
	//	_countof(text) - 1,
	//	textFormat.Get(),
	//	&textRect,
	//	textBrush.Get()
	//);

	//// Draw a grid background.
	//int width = static_cast<int>(rtSize.width);
	//int height = static_cast<int>(rtSize.height);

	//ComPtr<ID2D1SolidColorBrush> lightSlateGrayBrush;
	//ComPtr<ID2D1SolidColorBrush> cornflowerBlueBrush;

	//// Create a gray brush.
	//ThrowIfFailed(_renderTarget->CreateSolidColorBrush(
	//	D2D1::ColorF(D2D1::ColorF::LightSlateGray),
	//	&lightSlateGrayBrush
	//));
	//// Create a blue brush.
	//ThrowIfFailed(_renderTarget->CreateSolidColorBrush(
	//	D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
	//	&cornflowerBlueBrush
	//));

	//D2D1_RECT_F rectangle1 = D2D1::RectF(
	//	rtSize.width / 2 - 50.0f,
	//	rtSize.height / 2 - 50.0f,
	//	rtSize.width / 2 + 50.0f,
	//	rtSize.height / 2 + 50.0f
	//);

	//D2D1_RECT_F rectangle2 = D2D1::RectF(
	//	rtSize.width / 2 - 100.0f,
	//	rtSize.height / 2 - 100.0f,
	//	rtSize.width / 2 + 100.0f,
	//	rtSize.height / 2 + 100.0f
	//);

	//_renderTarget->FillRectangle(&rectangle1, lightSlateGrayBrush.Get());
	//_renderTarget->DrawRectangle(&rectangle2, cornflowerBlueBrush.Get());

	//for (int x = 0; x < width; x += 10)
	//{
	//	_renderTarget->DrawLine(
	//		D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
	//		D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
	//		lightSlateGrayBrush.Get(),
	//		0.5f
	//	);
	//}

	//for (int y = 0; y < height; y += 10)
	//{
	//	_renderTarget->DrawLine(
	//		D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
	//		D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
	//		lightSlateGrayBrush.Get(),
	//		0.5f
	//	);
	//}

	HRESULT hr = _renderTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET) {
		hr = S_OK;
		DiscardDeviceResources();
	}
	ThrowIfFailed(hr);
}

void D2Platform :: RenderWidget()
{
	D2D1_SIZE_F rtSize = _renderTarget->GetSize();
	D2D1_RECT_F rect = D2D1::RectF(
		0, 0,
		rtSize.width,
		rtSize.height
	);

	RenderWidget(_model->First(), rect);
}

void D2Platform :: RenderWidget(BaseWidget* widget, D2D1_RECT_F parentRect)
{
	if (!widget)
		return;

	WidgetType type = widget->Type();
	switch (type)
	{
		case _ELENA_::wtRectangle:
		{
			int left, top, right, bottom;
			widget->readRect(left, top, right, bottom);

			parentRect.left += left;
			parentRect.top += top;
			parentRect.right = parentRect.left + right;
			parentRect.bottom = parentRect.top + bottom;

			ComPtr<ID2D1SolidColorBrush> lightSlateGrayBrush;

			ThrowIfFailed(_renderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
				&lightSlateGrayBrush
			));

			_renderTarget->FillRectangle(&parentRect, lightSlateGrayBrush.Get());
			break;
		}
		case _ELENA_::wtText:
		{
			const WCHAR* text = dynamic_cast<TextWidget*>(widget)->Text();

			ComPtr<ID2D1SolidColorBrush>		textBrush;
			ComPtr<IDWriteTextFormat>			textFormat;

			// Create a blue brush.
			ThrowIfFailed(_renderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&textBrush
			));

			ThrowIfFailed(_dWriteFactory->CreateTextFormat(
				L"Verdana",
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				15,
				L"en-us",
				&textFormat
			));

			_renderTarget->DrawText(
				text,
				getlength(text),
				textFormat.Get(),
				&parentRect,
				textBrush.Get()
			);

			break;
		}
		default:
			break;
	}

	for (WidgetList::Iterator it = widget->Children(); !it.Eof(); it++) {
		RenderWidget(*it, parentRect);
	}
}

void D2Platform :: OnDestroy()
{
}