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

   session = new _ELENA_::Session(rootPath);
}

void freeSession()
{
   _ELENA_::freeobj(session);
   session = NULL;
}

// === dll entries ===

EXTERN_DLL_EXPORT int InterpretScript(_ELENA_::ident_t script)
{
   return session->translate(script, true);
}

EXTERN_DLL_EXPORT int InterpretFile(_ELENA_::ident_t  pathStr, int encoding, bool autoDetect)
{
   _ELENA_::Path path;
   _ELENA_::Path::loadPath(path, pathStr);

   return session->translate(path, encoding, autoDetect, true);
}

EXTERN_DLL_EXPORT int EvaluateScript(_ELENA_::ident_t script)
{
   return session->translate(script, false);
}

EXTERN_DLL_EXPORT int EvaluateFile(_ELENA_::ident_t  pathStr, int encoding, bool autoDetect)
{
   _ELENA_::Path path;
   _ELENA_::Path::loadPath(path, pathStr);

   return session->translate(path, encoding, autoDetect, false);
}

EXTERN_DLL_EXPORT int GetStatus(_ELENA_::ident_c* buffer, int maxLength)
{
   if (session) {
      _ELENA_::ident_t error = session->getLastError();
      size_t length = _ELENA_::getlength(error);
      if (length > maxLength)
         length = maxLength;

      _ELENA_::StringHelper::copy(buffer, error, length, length);

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
