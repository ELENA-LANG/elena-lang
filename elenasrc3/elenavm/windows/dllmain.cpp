#include "elena.h"
// --------------------------------------------------------------------------------
#include "langcommon.h"
#include "windows/presenter.h"
#include "windows/elenawinvmachine.h"

#include <windows.h>

using namespace elena_lang;

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

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

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

void init()
{
   machine = new ELENAWinVMMachine(&Presenter::getInstance(), CURRENT_PLATFORM, createJITCompiler);
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
         printf("ELENAVM: Unknown error %x\n", errCode);
         break;
   }
}

// --- API export ---

EXTERN_DLL_EXPORT void InitializeVMSTLA(SystemEnv* env, void* tape, void* criricalHandler)
{
#ifdef DEBUG_OUTPUT
   printf("InitializeVMSTLA.6 %x,%x\n", (int)env, (int)criricalHandler);

   fflush(stdout);
#endif

   int retVal = 0;
   try
   {
      machine->startSTA(env, tape, criricalHandler);
   }
   catch (InternalError err)
   {
      printError(err.messageCode);
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
}

EXTERN_DLL_EXPORT void* CollectGCLA(void* roots, size_t size)
{
   // !! temporal
   return nullptr;
}

EXTERN_DLL_EXPORT size_t LoadMessageNameLA(size_t message, char* buffer, size_t length)
{
   // !! temporal
   return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
       case DLL_PROCESS_ATTACH:
          init();
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
