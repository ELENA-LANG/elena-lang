//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Terminator
//
//      This file contains ELENA VM functions
//                                              (C)2011, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenavmH
#define elenavmH 1

extern "C" __declspec(dllimport) int InterpretLVM(void* tape);

extern "C" __declspec(dllimport) wchar_t* GetLVMStatus();

#endif // elenavmH