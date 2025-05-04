#include "elena.h"
// --------------------------------------------------------------------------------
#include "langcommon.h"
#include "windows/presenter.h"
#include "windows/elenawinvmachine.h"

#include <windows.h>

using namespace elena_lang;

constexpr auto CONFIG_FILE = "elenavm60.cfg";

constexpr auto VA_ALIGNMENT = 0x08;
constexpr unsigned int DEFAULT_MGSIZE = 344064;
constexpr unsigned int DEFAULT_YGSIZE = 86016;
constexpr unsigned int DEFAULT_STACKRESERVED = 0x200000;

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
      VA_ALIGNMENT, { DEFAULT_MGSIZE, DEFAULT_YGSIZE, DEFAULT_STACKRESERVED }, createJITCompiler);
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
      case errFailedMemoryAllocation:
         printf("ELENAVM: cannot reserve the memory");
         break;
      case errCommandSetAbsent:
         printf("ELENAVM: cannot initialize core");
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

EXTERN_DLL_EXPORT int InitializeVMSTLA(SystemEnv* env, void* tape, const char* criricalHandlerReference)
{
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

   if (machine->isStandAlone())
      machine->Exit(retVal);

   return retVal;
}

EXTERN_DLL_EXPORT int EvaluateVMLA(void* tape)
{
#ifdef DEBUG_OUTPUT
   printf("EvaluateVMSTLA.6 %x,%x\n", (int)env, (int)criricalHandler);

   fflush(stdout);
#endif

   int retVal = 0;
   try
   {
      machine->evaluate(tape);
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

   return retVal;
}

EXTERN_DLL_EXPORT int PrepareVMLA(const char* configName, const char* ns, const char* path, const char* exceptionHandler)
{
#ifdef DEBUG_OUTPUT
   printf("PrepareVMLA\n");
#endif

   IdentifierString package(ns, "=", path);

   auto env = new SystemEnv();
   env->gc_yg_size = 0x15000;
   env->gc_mg_size = 0x54000;
   env->th_single_content = new ThreadContent();

   MemoryDump tape;
   MemoryWriter tapeWriter(&tape);

   tapeWriter.writeDWord(VM_TERMINAL_CMD);

   tapeWriter.writeDWord(VM_CONFIG_CMD);
   tapeWriter.writeString(configName); // vm_client

   tapeWriter.writeDWord(VM_PACKAGE_CMD);
   tapeWriter.writeString(*package); // "embedded1=."

   tapeWriter.writeDWord(VM_INIT_CMD);
   tapeWriter.writeDWord(VM_ENDOFTAPE_CMD);

   return InitializeVMSTLA(env, tape.get(0), exceptionHandler);
}

EXTERN_DLL_EXPORT int ExecuteVMLA(const char* target, const char* arg, char* output, size_t maxLength)
{
#ifdef DEBUG_OUTPUT
   printf("ExecuteVMLA.6\n");
#endif

   MemoryDump tape;
   MemoryWriter tapeWriter(&tape);

   tapeWriter.writeDWord(VM_ALLOC_CMD);
   tapeWriter.writeDWord(2);

   tapeWriter.writeDWord(VM_STRING_CMD);
   tapeWriter.writeString(arg);

   tapeWriter.writeDWord(VM_SET_ARG_CMD);
   tapeWriter.writeDWord(0);

   tapeWriter.writeDWord(VM_CALLSYMBOL_CMD);
   tapeWriter.writeString(target);

   tapeWriter.writeDWord(VM_SEND_MESSAGE_CMD);
   tapeWriter.writeString("function:#invoke[1]");

   tapeWriter.writeDWord(VM_ENDOFTAPE_CMD);

   int retVal = -1;
   try
   {
      if (output) {
         size_t copied = 0;
         if (machine->evaluateAndReturn(tape.get(0), output, maxLength, copied))
            retVal = (int)copied;
      }
      else {
         machine->evaluate(tape.get(0));
         retVal = 0;
      }
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

   return retVal;
}

EXTERN_DLL_EXPORT int FreeVMLA()
{
   return -1;
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
   return __routineProvider.GCRoutine(machine->getSystemEnv()->gc_table, (GCRoot*)roots, size, false);
}

EXTERN_DLL_EXPORT void* CollectPermGCLA(size_t size)
{
   return __routineProvider.GCRoutinePerm(machine->getSystemEnv()->gc_table, size);
}

EXTERN_DLL_EXPORT void* ForcedCollectGCLA(void* roots, int fullMode)
{
   return __routineProvider.GCRoutine(machine->getSystemEnv()->gc_table, (GCRoot*)roots, INVALID_SIZE, fullMode != 0);
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

EXTERN_DLL_EXPORT mssg_t LoadActionLA(const char* actionName)
{
   return machine->loadAction(actionName);
}

EXTERN_DLL_EXPORT size_t LoadActionNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadActionName((mssg_t)message, buffer, length);
}

/// <summary>
/// Fills the passed dispatch list with references to extension message overload list
/// </summary>
/// <param name="moduleList">List of imported modules separated by semicolon</param>
/// <param name="message">Extension message</param>
/// <param name="output">Dispatch list</param>
/// <returns></returns>
EXTERN_DLL_EXPORT int LoadExtensionDispatcherLA(const char* moduleList, mssg_t message, void* output)
{
   return machine->loadExtensionDispatcher(moduleList, message, output);
}

EXTERN_DLL_EXPORT size_t LoadClassMessagesLA(void* classPtr, mssg_t* output, size_t skip, size_t maxLength)
{
   return machine->loadClassMessages(classPtr, output, skip, maxLength);
}

EXTERN_DLL_EXPORT bool CheckClassMessageLA(void* classPtr, mssg_t message)
{
   return machine->checkClassMessage(classPtr, message);
}

EXTERN_DLL_EXPORT void GetRandomSeedLA(SeedStruct& seed)
{
   machine->initRandomSeed(seed);
}

EXTERN_DLL_EXPORT unsigned int GetRandomIntLA(SeedStruct& seed)
{
   return machine->getRandomNumber(seed);
}

/// <summary>
/// Returns the signature list
/// </summary>
/// <param name="message">A strong-typed message</param>
/// <param name="output">Signature tyoe</param>
/// <returns>the total length</returns>
EXTERN_DLL_EXPORT int LoadSignatureLA(mssg_t message, addr_t* output, int maximalLength)
{
   return machine->loadSignature(message, output, maximalLength);
}

/// <summary>
/// Inject an interface VMT into a dynamic proxy class
/// </summary>
/// <returns>a reference to dynamically created VMT</returns>
EXTERN_DLL_EXPORT void* InjectProxyTypeLA(void* target, void* type, int staticLength, int nameIndex)
{
   return (void*)machine->injectType(target, type, staticLength, nameIndex);
}

EXTERN_DLL_EXPORT void GetGCStatisticsLA(GCStatistics* statistics)
{
   machine->getGCStatistics(statistics);
}

EXTERN_DLL_EXPORT void ResetGCStatisticsLA(GCStatistics* statistics)
{
   SystemRoutineProvider::ResetGCStatistics();
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
