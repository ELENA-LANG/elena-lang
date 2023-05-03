#include "elena.h"
// --------------------------------------------------------------------------------
#include "langcommon.h"
#include "windows/presenter.h"
#include "windows/elenawinvmachine.h"

#include <windows.h>

using namespace elena_lang;

constexpr auto CONFIG_FILE = "elenavm60.cfg";

constexpr auto VA_ALIGNMENT = 0x08;
constexpr int  DEFAULT_MGSIZE = 344064;
constexpr int  DEFAULT_YGSIZE = 86016;

#ifdef _M_IX86

#include "x86compiler.h"

constexpr auto CURRENT_PLATFORM = PlatformType::Win_x86;

JITCompilerBase* createJITCompiler(LibraryLoaderBase* loader, PlatformType platform)
{
   switch (platform) {
      case PlatformType::Win_x86:
         return new X86JITCompiler();
      default:
         return nullptr;
   }
}

#elif _M_X64

#include "x86_64compiler.h"

constexpr auto CURRENT_PLATFORM = PlatformType::Win_x86_64;

JITCompilerBase* createJITCompiler(LibraryLoaderBase* loader, PlatformType platform)
{
   switch (platform) {
      case PlatformType::Win_x86_64:
         return new X86_64JITCompiler();
      default:
         return nullptr;
   }
}

#endif

static ELENAVMMachine* machine = nullptr;
static SystemEnv* systemEnv = nullptr;

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

void loadDLLPath(PathString& rootPath, HMODULE hModule)
{
   TCHAR path[MAX_PATH + 1];

   ::GetModuleFileName(hModule, path, MAX_PATH);

   rootPath.copySubPath(path, true);
   rootPath.lower();
}

// --- Presenter ---

class Presenter : public WinConsolePresenter
{
private:
   Presenter() = default;

public:
   ustr_t getMessage(int code) override
   {
      // !!temporal : not used
      return nullptr;
   }

   static Presenter& getInstance()
   {
      static Presenter instance;

      return instance;
   }

   Presenter(Presenter const&) = delete;
   void operator=(Presenter const&) = delete;

   ~Presenter() override = default;
};

void init(HMODULE hModule)
{
   PathString rootPath;
   loadDLLPath(rootPath, hModule);
   rootPath.combine(CONFIG_FILE);

   machine = new ELENAWinVMMachine(*rootPath, &Presenter::getInstance(), CURRENT_PLATFORM,
      VA_ALIGNMENT, { DEFAULT_MGSIZE, DEFAULT_YGSIZE }, createJITCompiler);
}

void printError(int errCode)
{
   switch (errCode) {
      case errVMNotExecuted:
         printf("ELENAVM: no code was executed");
         break;
      case errVMBroken:
         printf("ELENAVM: execution was broken");
         break;
      case errVMNotInitialized:
         printf("ELENAVM: not initialized");
         break;
      default:
         printf("ELENAVM: Unknown error %d\n", errCode);
         break;
   }
}

void printError(int errCode, ustr_t arg)
{
   switch (errCode) {
      case errVMReferenceNotFound:
         printf("ELENAVM: Cannot load reference %s\n", arg.str());
         break;
      default:
         printf("ELENAVM: Unknown error %d\n", errCode);
         break;
   }
}

// --- API export ---

EXTERN_DLL_EXPORT void InitializeVMSTLA(SystemEnv* env, void* tape, const char* criricalHandlerReference)
{
   systemEnv = env;

#ifdef DEBUG_OUTPUT
   printf("InitializeVMSTLA.6 %x,%x\n", (int)env, (int)criricalHandler);

   fflush(stdout);
#endif

   int retVal = 0;
   try
   {
      machine->startSTA(env, tape, criricalHandlerReference);
   }
   catch (InternalError err)
   {
      printError(err.messageCode);
      retVal = -1;
   }
   catch (JITUnresolvedException& e)
   {
      printError(errVMReferenceNotFound, e.referenceInfo.referenceName);

      retVal = -1;
   }
   catch (...)
   {
      printError(errVMBroken);
      retVal = -1;
   }

   machine->Exit(retVal);
}

EXTERN_DLL_EXPORT void ExitLA(int retVal)
{
   if (retVal) {
      printf("Aborted:%x\n", retVal);
      fflush(stdout);
   }

   machine->Exit(retVal);
}

EXTERN_DLL_EXPORT void* CollectGCLA(void* roots, size_t size)
{
   return __routineProvider.GCRoutine(systemEnv->gc_table, (GCRoot*)roots, size, false);
}

EXTERN_DLL_EXPORT void* CollectPermGCLA(size_t size)
{
   return __routineProvider.GCRoutinePerm(systemEnv->gc_table, size);
}

EXTERN_DLL_EXPORT void* ForcedCollectGCLA(void* roots, int fullMode)
{
   return __routineProvider.GCRoutine(systemEnv->gc_table, (GCRoot*)roots, INVALID_SIZE, fullMode != 0);
}

EXTERN_DLL_EXPORT size_t LoadMessageNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadMessageName((mssg_t)message, buffer, length);
}

EXTERN_DLL_EXPORT size_t LoadCallStackLA(uintptr_t framePtr, uintptr_t* list, size_t length)
{
   return __routineProvider.LoadCallStack(framePtr, list, length);
}

EXTERN_DLL_EXPORT size_t LoadAddressInfoLM(size_t retPoint, char* lineInfo, size_t length)
{
   return machine->loadAddressInfo(retPoint, lineInfo, length);
}

EXTERN_DLL_EXPORT addr_t LoadSymbolByStringLA(const char* symbolName)
{
   return machine->loadSymbol(symbolName);
}

EXTERN_DLL_EXPORT addr_t LoadClassByStringLA(const char* symbolName)
{
   return machine->loadClassReference(symbolName);
}

EXTERN_DLL_EXPORT addr_t LoadClassByBufferLA(void* referenceName, size_t index, size_t length)
{
   if (length < 0x100) {
      IdentifierString str((const char*)referenceName + index, length);

      return LoadClassByStringLA(*str);
   }
   else {
      DynamicString<char> str((const char*)referenceName, index, length);

      return LoadClassByStringLA(str.str());
   }
}

EXTERN_DLL_EXPORT addr_t LoadSymbolByString2LA(const char* ns, const char* symbolName)
{
   ReferenceName fullName(ns, symbolName);

   return machine->loadSymbol(*fullName);
}

EXTERN_DLL_EXPORT mssg_t LoadMessageLA(const char* messageName)
{
   return machine->loadMessage(messageName);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
       case DLL_PROCESS_ATTACH:
          init(hModule);
          break;
       case DLL_THREAD_ATTACH:
       case DLL_THREAD_DETACH:
       case DLL_PROCESS_DETACH:
           break;
       default:
          // to make compiler happy
          break;
    }
    return TRUE;
}
