//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Terminator
//
//      This file contains ELENA SM functions
//                                              (C)2011-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenasmH
#define elenasmH 1

extern "C" __declspec(dllimport) int TranslateLVMTape(const wchar_t* script);

extern "C" __declspec(dllimport) int TranslateLVMFile(const wchar_t* path, int encoding, bool autoDetect);

extern "C" __declspec(dllimport) const wchar_t* GetLSMStatus();

#endif // elenasmH