//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------
#include <unistd.h>

#include "elenavm.h"
#include "langcommon.h"
#include "linux/lnxconsts.h"
#include "linux/presenter.h"
#include "elenavmmachine.h"

using namespace elena_lang;

static ELENAVMMachine* machine = nullptr;

#if defined(__FreeBSD__)

#define ROOT_PATH          "/usr/local/lib/elena"

#else

#define ROOT_PATH          "/usr/lib/elena"

#endif

#if defined(__x86_64__)

#include "x86_64compiler.h"

#if defined(__FreeBSD__)

constexpr auto CURRENT_PLATFORM = PlatformType::FreeBSD_x86_64;

#else

constexpr auto CURRENT_PLATFORM = PlatformType::Linux_x86_64;

#endif

#elif defined(__i386__)

#include "x86compiler.h"

constexpr auto CURRENT_PLATFORM = PlatformType::Linux_x86;

#elif defined(__PPC64__)

#include "ppc64compiler.h "

constexpr auto CURRENT_PLATFORM = PlatformType::Linux_PPC64le;

#elif defined(__aarch64__)

#include "arm64compiler.h"

constexpr auto CURRENT_PLATFORM = PlatformType::Linux_ARM64;

#endif // defined

JITCompilerBase* createJITCompiler(LibraryLoaderBase* loader, PlatformType platform)
{
   switch (platform) {
   #if defined(__i386__) || defined(__x86_64__)
      case PlatformType::Linux_x86:
         return new X86JITCompiler();
   #endif
   #if defined(__x86_64__)
      case PlatformType::Linux_x86_64:
      case PlatformType::FreeBSD_x86_64:
         return new X86_64JITCompiler();
   #endif
   #if defined(__PPC64__)
      case PlatformType::Linux_PPC64le:
         return new PPC64leJITCompiler();
   #endif
   #if defined(__aarch64__)
      case PlatformType::Linux_ARM64:
         return new ARM64JITCompiler();
   #endif
      default:
         return nullptr;
   }
}

// --- Presenter ---

class Presenter : public LinuxConsolePresenter
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
   machine = new ELENAWinVMMachine(ROOT_PATH, &Presenter::getInstance(), CURRENT_PLATFORM,
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

void InitializeVMSTLA(SystemEnv* env, void* tape, const char* criricalHandlerReference)
{
#ifdef DEBUG_OUTPUT
   printf("InitializeVMSTLA.6 %x,%x\n", (int)env, (int)criricalHandler);

   fflush(stdout);
#endif

   if (!machine)
      init();

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

int EvaluateVMLA(void* tape)
{
#ifdef DEBUG_OUTPUT
   printf("EvaluateVMSTLA.6 %x,%x\n", (int)env, (int)criricalHandler);

   fflush(stdout);
#endif

   if (!machine)
      init();

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

int PrepareVMLA(const char* configName, const char* ns, const char* path, const char* exceptionHandler)
{
#ifdef DEBUG_OUTPUT
   printf("PrepareVMLA\n");
#endif

   if (!machine)
      init();

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

int ExecuteVMLA(const char* target, const char* arg, char* output, size_t maxLength)
{
#ifdef DEBUG_OUTPUT
   printf("ExecuteVMLA.6\n");
#endif

   if (!machine)
      init();

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

void* CollectGCLA(void* roots, size_t size)
{
   return __routineProvider.GCRoutine(machine->getSystemEnv()->gc_table, (GCRoot*)roots, size, false);
}

void* CollectPermGCLA(size_t size)
{
   return __routineProvider.GCRoutinePerm(machine->getSystemEnv()->gc_table, size);
}

void* ForcedCollectGCLA(void* roots, int fullMode)
{
   return __routineProvider.GCRoutine(machine->getSystemEnv()->gc_table, (GCRoot*)roots, INVALID_SIZE, fullMode != 0);
}

size_t LoadMessageNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadMessageName((mssg_t)message, buffer, length);
}

size_t LoadCallStackLA(uintptr_t framePtr, uintptr_t* list, size_t length)
{
   return __routineProvider.LoadCallStack(framePtr, list, length);
}

size_t LoadAddressInfoLM(size_t retPoint, char* lineInfo, size_t length)
{
   return machine->loadAddressInfo(retPoint, lineInfo, length);
}

addr_t LoadSymbolByStringLA(const char* symbolName)
{
   return machine->loadSymbol(symbolName);
}

addr_t LoadClassByStringLA(const char* symbolName)
{
   return machine->loadClassReference(symbolName);
}

addr_t LoadClassByBufferLA(void* referenceName, size_t index, size_t length)
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

addr_t LoadSymbolByString2LA(const char* ns, const char* symbolName)
{
   ReferenceName fullName(ns, symbolName);

   return machine->loadSymbol(*fullName);
}

mssg_t LoadMessageLA(const char* messageName)
{
   return machine->loadMessage(messageName);
}

mssg_t LoadActionLA(const char* actionName)
{
   return machine->loadAction(actionName);
}

size_t LoadActionNameLA(size_t message, char* buffer, size_t length)
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
int LoadExtensionDispatcherLA(const char* moduleList, mssg_t message, void* output)
{
   return machine->loadExtensionDispatcher(moduleList, message, output);
}

size_t LoadClassMessagesLA(void* classPtr, mssg_t* output, size_t skip, size_t maxLength)
{
   return machine->loadClassMessages(classPtr, output, skip, maxLength);
}

bool CheckClassMessageLA(void* classPtr, mssg_t message)
{
   return machine->checkClassMessage(classPtr, message);
}

void GetRandomSeedLA(SeedStruct& seed)
{
   machine->initRandomSeed(seed);
}

unsigned int GetRandomIntLA(SeedStruct& seed)
{
   return machine->getRandomNumber(seed);
}

/// <summary>
/// Returns the signature list
/// </summary>
/// <param name="message">A strong-typed message</param>
/// <param name="output">Signature tyoe</param>
/// <returns>the total length</returns>
int LoadSignatureLA(mssg_t message, addr_t* output, int maximalLength)
{
   return machine->loadSignature(message, output, maximalLength);
}

/// <summary>
/// Inject an interface VMT into a dynamic proxy class
/// </summary>
/// <returns>a reference to dynamically created VMT</returns>
void* InjectProxyTypeLA(void* target, void* type, int staticLength, int nameIndex)
{
   return (void*)machine->injectType(target, type, staticLength, nameIndex);
}

void GetGCStatisticsLA(GCStatistics* statistics)
{
   machine->getGCStatistics(statistics);
}

void ResetGCStatisticsLA(GCStatistics* statistics)
{
   SystemRoutineProvider::ResetGCStatistics();
}

void ExitLA(int retVal)
{
   if (retVal) {
      printf("Aborted:%x\n", retVal);
      fflush(stdout);
   }

   machine->Exit(retVal);
}

int FreeVMLA()
{
   return -1;
}
