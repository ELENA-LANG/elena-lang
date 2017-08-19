//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Graphic Engine
//
//                                              (C)2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "model.h"

using namespace _ELENA_;

void* Model :: NewWidget(void* parentRef, int type)
{
	BaseWidget* parent = static_cast<BaseWidget*>(parentRef);
	BaseWidget* widget = nullptr;

	switch ((WidgetType)type) {
		case wtRectangle:
			widget = new RectangleWidget(parent);
			break;
		case wtText:
			widget = new TextWidget(parent);
			break;
		default:
			return NULL;
	}

	if (parent) {
		parent->Add(widget);
	}
	else _root = widget;

	return widget;
}

int Model :: SetLocation(void* handle, int x, int y)
{
	BaseWidget* widget = static_cast<BaseWidget*>(handle);

	int left, right, top, bottom;
	widget->readRect(left, top, right, bottom);
	right = x + right - left;
	bottom = y + bottom - top;
	left = x;
	top = y;

	widget->setRect(left, top, right, bottom);

	return 0;
}

int Model :: SetSize(void* handle, int width, int height)
{
	BaseWidget* widget = static_cast<BaseWidget*>(handle);

	int left, right, top, bottom;
	widget->readRect(left, top, right, bottom);
	right = left + width;
	bottom = top + height;

	widget->setRect(left, top, right, bottom);

	return 0;
}

int Model :: SetText(void* handle, const wchar_t* text)
{
	BaseWidget* widget = static_cast<BaseWidget*>(handle);

	widget->setText(text);

	return 0;
}

int Model :: SetNProperty(void* handle, int prop, int value)
{
   BaseWidget* widget = static_cast<BaseWidget*>(handle);

   return widget->setNProperty((WidgetProperty)prop, value);
}

int Model :: GetNProperty(void* handle, int prop, int defValue)
{
   BaseWidget* widget = static_cast<BaseWidget*>(handle);

   return widget->getNProperty((WidgetProperty)prop, defValue);
}


int Model :: CloseWidget(void* /*handle*/)
{
	return -1;
}
