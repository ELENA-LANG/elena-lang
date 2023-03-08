#include "elena.h"
// --------------------------------------------------------------------------
#include "scriptmachine.h"
#include <windows.h>

using namespace elena_lang;

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

static ScriptEngine* engine = nullptr;

void loadDLLPath(PathString& rootPath, HMODULE hModule)
{
   TCHAR path[MAX_PATH + 1];

   ::GetModuleFileName(hModule, path, MAX_PATH);

   rootPath.copySubPath(path, true);
   rootPath.lower();
}

void init(HMODULE hModule)
{
   PathString rootPath;
   loadDLLPath(rootPath, hModule);

   engine = new ScriptEngine(*rootPath);
}

EXTERN_DLL_EXPORT int NewScope()
{
   if (engine) {
      return engine->newScope();
   }
   else return -1;
}

EXTERN_DLL_EXPORT void* InterpretScopeFile(int scope_id, const char* pathStr, int encoding, bool autoDetect)
{
   PathString path(pathStr);

   return engine->translate(0, *path, (FileEncoding)encoding, autoDetect);
}

EXTERN_DLL_EXPORT void Release(void* tape)
{
   if (tape)
      engine->free(tape);
}

EXTERN_DLL_EXPORT size_t GetStatus(char* buffer, size_t maxLength)
{
   if (engine) {
      ustr_t error = engine->getLastError();
      size_t length = getlength(error);
      if (buffer != nullptr) {
         if (length > maxLength)
            length = maxLength;

         size_t retVal = maxLength;
         StrConvertor::copy(buffer, error, length, retVal);
         length= retVal;
      }

      return length;
   }
   else return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
   DWORD  ul_reason_for_call,
   LPVOID lpReserved
   )
{
   switch (ul_reason_for_call)
   {
      case DLL_PROCESS_ATTACH:
         init(hModule);
         return TRUE;
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
         return TRUE;
      case DLL_PROCESS_DETACH:
         break;
   }
   return TRUE;
}
