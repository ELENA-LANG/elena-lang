//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Graphic Engine
//
//                                              (C)2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenagm_graphicsH
#define elenagm_graphicsH 1

namespace _ELENA_
{

class GraphicPlatform
{
public:
	virtual void* NewWidget(void* parent, int type) = 0;
	virtual int CloseWidget(void* handle) = 0;

	virtual ~GraphicPlatform() {}

	virtual void Init(HWND hWnd) = 0;
	virtual void OnRender(HWND hWnd) = 0;
	virtual void OnDestroy() = 0;
};

} // _ELENA_

#endif //  elenagm_graphicsH

