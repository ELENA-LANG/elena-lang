//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Graphic Engine
//
//                                              (C)2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenagm_widgetH
#define elenagm_widgetH 1

#include "elena.h"

namespace _ELENA_
{
		
enum WidgetType
{
	wtNone = 0,
	wtRectangle = 1,
	wtText = 2
};

class BaseWidget;

typedef List<BaseWidget*> WidgetList;

class BaseWidget
{
	BaseWidget*	_parent;
	WidgetList	_children;

public:
	virtual WidgetType Type() const { return wtNone; }

	WidgetList::Iterator Children() { return _children.start(); }

	BaseWidget(BaseWidget* parent)
		: _children(NULL, freeobj)
	{
		_parent = parent;
	}

	virtual ~BaseWidget() {}

	void Add(BaseWidget* widget)
	{
		_children.add(widget);
	}
};

class RectangleWidget : public BaseWidget
{
public:
	virtual WidgetType Type() const { return wtRectangle; }

	RectangleWidget(BaseWidget* parent)
		: BaseWidget(parent)
	{
	}
};

class TextWidget : public BaseWidget
{
public:
	virtual WidgetType Type() const { return wtText; }

	TextWidget(BaseWidget* parent)
		: BaseWidget(parent)
	{
	}
};

} // _ELENA_

#endif // elenagm_widgetH