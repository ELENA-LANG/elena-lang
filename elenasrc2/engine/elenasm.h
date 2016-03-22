//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Terminator
//
//      This file contains ELENA SM functions
//                                              (C)2011-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenasmH
#define elenasmH 1

// should be used from stand-alone application
extern "C" __declspec(dllimport) void* InterpretScript(const char* script);

// should be used from stand-alone application
extern "C" __declspec(dllimport) void* InterpretFile(const char* path, int encoding, bool autoDetect);

// should be used from ELENA program
extern "C" __declspec(dllimport) void Release(void* tape);

extern "C" __declspec(dllimport) int GetStatus(char* error, int maxLength);

#endif // elenasmH