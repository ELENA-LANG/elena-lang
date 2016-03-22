//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Terminator
//
//      This file contains ELENA VM functions
//                                              (C)2011-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenavmH
#define elenavmH 1

// Should be used from stand-alone application
extern "C" __declspec(dllimport) int InterpretTape(void* tape);

extern "C" __declspec(dllimport) const char* GetVMLastError();

#endif // elenavmH