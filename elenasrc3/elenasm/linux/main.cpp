#include "elena.h"
// --------------------------------------------------------------------------
#include "scriptmachine.h"
#include "elenasm.h"

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

int NewScopeSMLA()
{
   if (engine) {
      return engine->newScope();
   }
   else return -1;
}

void* InterpretFileSMLA(const char* pathStr, int encoding, bool autoDetect)
{
   PathString path(pathStr);

   return engine->translate(0, *path, (FileEncoding)encoding, autoDetect);
}

void* InterpretScopeFileSMLA(int scope_id, const char* pathStr, int encoding, bool autoDetect)
{
   PathString path(pathStr);

   return engine->translate(scope_id, *path, (FileEncoding)encoding, autoDetect);
}

void* InterpretScopeScriptSMLA(int scope_id, const char* script)
{
   return engine->translate(scope_id, script);
}

void* InterpretScriptSMLA(const char* script)
{
   return engine->translate(0, script);
}

int GetLengthSMLA(void* tape)
{
   if (tape) {
      return engine->getLength(tape);
   }
   else return 0;
}

void ReleaseSMLA(void* tape)
{
   if (tape)
      engine->free(tape);
}

size_t GetStatusSMLA(char* buffer, size_t maxLength)
{
   if (engine) {
      ustr_t error = engine->getLastError();
      size_t length = getlength(error);
      if (buffer != nullptr) {
         if (length > maxLength)
            length = maxLength;

         size_t retVal = maxLength;
         StrConvertor::copy(buffer, error, length, retVal);
         length = retVal;
      }

      return length;
   }
   else return 0;
}
