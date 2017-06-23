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

int Model :: CloseWidget(void* handle)
{
	return 0;
}
