#include "elena.h"
// --------------------------------------------------------------------------
#include "session.h"

#include <windows.h>

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

_ELENA_::Session* session = NULL;

void newSession(HMODULE hModule)
{
   _ELENA_::Path rootPath;

   TCHAR path[MAX_PATH + 1];

   ::GetModuleFileName(hModule, path, MAX_PATH);

   rootPath.copySubPath(path);

   session = new _ELENA_::Session(rootPath.c_str());
}

void freeSession()
{
   _ELENA_::freeobj(session);
   session = NULL;
}

// === dll entries ===

EXTERN_DLL_EXPORT void* InterpretScript(_ELENA_::ident_t script)
{
   return session->translate(script);
}

EXTERN_DLL_EXPORT void* InterpretFile(const char* pathStr, int encoding, bool autoDetect)
{
   _ELENA_::Path path(pathStr);

   return session->translate(path.c_str(), encoding, autoDetect);
}

EXTERN_DLL_EXPORT void Release(void* tape)
{
   session->free(tape);
}

EXTERN_DLL_EXPORT int GetStatus(char* buffer, int maxLength)
{
   if (session) {
      _ELENA_::ident_t error = session->getLastError();
      size_t length = _ELENA_::getlength(error);
      if ((int)length > maxLength)
         length = maxLength;

      _ELENA_::Convertor::copy(buffer, error, length, length);

      return length;
   }
   else return 0;
}

// --- dllmain ---

extern "C"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
               )
{
   switch (ul_reason_for_call)
   {
   case DLL_PROCESS_ATTACH:
      newSession(hModule);
      return TRUE;
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      return TRUE;
   case DLL_PROCESS_DETACH:
      freeSession();
      break;
   }
   return TRUE;
}
