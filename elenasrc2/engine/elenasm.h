//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Terminator
//
//      This file contains ELENA SM functions
//                                              (C)2011-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenasmH
#define elenasmH 1

// should be used from stand-alone application
extern "C" __declspec(dllimport) int InterpretScript(const wchar_t* script);

// should be used from stand-alone application
extern "C" __declspec(dllimport) int InterpretFile(const wchar_t* path, int encoding, bool autoDetect);

// should be used from ELENA program
extern "C" __declspec(dllimport) int EvaluateScript(const wchar_t* script);

// should be used from ELENA program
extern "C" __declspec(dllimport) int EvaluateFile(const wchar_t* path, int encoding, bool autoDetect);

extern "C" __declspec(dllimport) const wchar_t* GetStatus();

#endif // elenasmH