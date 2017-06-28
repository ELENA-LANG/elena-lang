//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Graphic Engine
//
//                                              (C)2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenagm_modelH
#define elenagm_modelH 1

#include "elena.h"
#include "widget.h"

namespace _ELENA_
{

class Model
{
	BaseWidget* _root;

public:
	BaseWidget* First() { return _root; }

	void* NewWidget(void* parent, int type);
	int CloseWidget(void* handle);

	int SetLocation(void* handle, int x, int y);
	int SetSize(void* handle, int width, int height);
	int SetText(void* handle, const wchar_t* text);
   int SetNProperty(void* handle, int prop, int value);
   int GetNProperty(void* handle, int prop, int defValue);

	Model() { _root = NULL; }
	virtual ~Model() { freeobj(_root); }
};

} // _ELENA_

#endif // elenagm_modelH