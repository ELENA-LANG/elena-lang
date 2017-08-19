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

enum WidgetProperty
{
   wprX      = 0x00001,
   wprY      = 0x00002,
   wprWidth  = 0x00003,
   wprHeight = 0x00004,
};

class BaseWidget;

typedef List<BaseWidget*> WidgetList;

class BaseWidget
{
protected:
	BaseWidget*	_parent;
	WidgetList	_children;

public:
	virtual void readRect(int& left, int& top, int& right, int& bottom)
	{
		if (_parent) {
			_parent->readRect(left, top, right, bottom);
		}		
	}
	virtual int setRect(int/* left*/, int/* top*/, int/* right*/, int/* bottom*/)
	{
		return -1;
	}
	virtual int setText(const wchar_t* /*text*/)
	{
		return -1;
	}
   virtual int setNProperty(WidgetProperty prop, int value)
   {
      if (_parent) {
         return _parent->setNProperty(prop, value);
      }
      else return -1;
   }
   virtual int getNProperty(WidgetProperty prop, int defValue)
   {
      if (_parent) {
         return _parent->getNProperty(prop, defValue);
      }
      else return defValue;
   }

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

class BaseRectangleWidget : public BaseWidget
{
	int left, top;
	int right, bottom;

protected:
	BaseRectangleWidget(BaseWidget* parent)
		: BaseWidget(parent)
	{
		left = top = 0;
		right = bottom = 50;
	}
public:
   virtual int setNProperty(WidgetProperty prop, int value)
   {
      switch (prop)
      {
         case wprX:
            left = value;
            break;
         case wprY:
            top = value;
            break;
         case wprWidth:
            right = left + value;
            break;
         case wprHeight:
            bottom = top + value;
            break;
         default:
            return BaseWidget::setNProperty(prop, value);
      }

      return 0;
   }
   virtual int getNProperty(WidgetProperty prop, int defValue)
   {
      switch (prop)
      {
         case wprX:
            return left;
         case wprY:
            return top;
         case wprWidth:
            return right - left;
         case wprHeight:
            return bottom - top;
         default:
            return BaseWidget::getNProperty(prop, defValue);
      }
   }

   virtual void readRect(int& left, int& top, int& right, int& bottom)
	{
		left = this->left;
		top = this->top;
		right = this->right;
		bottom = this->bottom;
	}
	virtual int setRect(int left, int top, int right, int bottom)
	{
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;

		return 0;
	}
};

class RectangleWidget : public BaseRectangleWidget
{
public:
	virtual WidgetType Type() const { return wtRectangle; }

	RectangleWidget(BaseWidget* parent)
		: BaseRectangleWidget(parent)
	{
	}
};

typedef DynamicString<wchar_t> TextStr;

class TextWidget : public BaseWidget
{
	TextStr _text;

public:
	virtual WidgetType Type() const { return wtText; }

	const wchar_t* Text() const { return _text; }

	virtual int setText(const wchar_t* text)
	{
		_text.copy(text);

		return 0;
	}

	TextWidget(BaseWidget* parent)
		: BaseWidget(parent), _text(L"Label")
	{
	}
};

} // _ELENA_

#endif // elenagm_widgetH