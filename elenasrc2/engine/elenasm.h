//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Terminator
//
//      This file contains ELENA SM functions
//                                              (C)2011-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenasmH
#define elenasmH 1

// translate mode constants
#define MASK_MODE        0x000FF

// translate attribute constants
#define TRACE_MODE       0x20000
#define SYMBOLIC_MODE    0x00100

extern "C" __declspec(dllimport) void* TranslateLVMTape(const wchar_t* name, const wchar_t* script, int mode);

extern "C" __declspec(dllimport) void* TranslateLVMFile(const wchar_t* name, const wchar_t* path, int encoding, bool autoDetect, int mode);

extern "C" __declspec(dllimport) void FreeLVMTape(void* tape);

extern "C" __declspec(dllimport) const wchar_t* GetLSMStatus();

#endif // elenasmH