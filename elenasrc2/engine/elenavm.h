//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Terminator
//
//      This file contains ELENA VM functions
//                                              (C)2011, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenavmH
#define elenavmH 1

// Should be used from stand-alone application
extern "C" __declspec(dllimport) int Interpret(void* tape);

// Should be used from ELENA program
extern "C" __declspec(dllimport) int Evaluate(void* tape);

extern "C" __declspec(dllimport) const char* GetLVMStatus();

#endif // elenavmH