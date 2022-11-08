//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE common classes header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef IDECOMMON_H
#define IDECOMMON_H

#include "guicommon.h"
#include "eng/messages.h"

#define IDE_REVISION_NUMBER                           0x0024

namespace elena_lang
{
   constexpr auto PLATFORM_CATEGORY                   = "configuration/platform";
   constexpr auto MODULE_CATEGORY                     = "files/*";

   constexpr auto WIN_X86_KEY                         = "Win_x86";
   constexpr auto WIN_X86_64_KEY                      = "Win_x64";
   constexpr auto LINUX_X86_KEY                       = "Linux_I386";
   constexpr auto LINUX_X86_64_KEY                    = "Linux_AMD64";
   constexpr auto LINUX_PPC64le_KEY                   = "Linux_PPC64le";
   constexpr auto LINUX_ARM64_KEY                     = "Linux_ARM64";

   constexpr auto ERROR_RUN_NEED_TARGET               = 0x0001;
   constexpr auto ERROR_DEBUG_FILE_NOT_FOUND_COMPILE  = 0x0002;
   constexpr auto ERROR_RUN_NEED_RECOMPILE            = 0x0003;

   // --- Notification codes ---
   constexpr auto NOTIFY_SOURCEMODEL                  = 1;
   constexpr auto NOTIFY_CURRENTVIEW_CHANGED          = 2;
   constexpr auto NOTIFY_CURRENTVIEW_SHOW             = 3;
   constexpr auto NOTIFY_CURRENTVIEW_HIDE             = 4;
   constexpr auto NOTIFY_LAYOUT_CHANGED               = 5;
   constexpr auto NOTIFY_SHOW_RESULT                  = 6;
   constexpr auto NOTIFY_COMPILATION_RESULT           = 7;
   constexpr auto NOTIFY_ERROR_HIGHLIGHT_ROW          = 8;
   constexpr auto NOTIFY_ACTIVATE_EDITFRAME           = 9;
   constexpr auto NOTIFY_START_COMPILATION            = 10;
   constexpr auto NOTIFY_PROJECTMODEL                 = 11;
   constexpr auto NOTIFY_PROJECTVIEW_SEL              = 12;

   // --- PathSettings ---
   struct PathSettings
   {
      PathString appPath;
   };

   // --- IDEStatus ---
   enum class IDEStatus
   {
      None            = 0,
      Ready           = 1,
      Busy            = 2,
      AutoRecompiling = 3,
   };

   inline bool testIDEStatus(IDEStatus value, IDEStatus mask)
   {
      return test((int)value, (int)mask);
   }

   // --- ProcessListenerBase --
   class ProcessListenerBase
   {
   public:
      virtual void onOutput(const char* s) = 0;
      virtual void onErrorOutput(const char* s) = 0;
      virtual void afterExecution(int exitCode) = 0;
   };

   typedef List<ProcessListenerBase*> ProcessListeners;

   // --- ProcessBase --
   class ProcessBase
   {
   protected:
      ProcessListeners _listeners;

   public:
      void attachListener(ProcessListenerBase* listener)
      {
         _listeners.add(listener);
      }

      virtual bool start(path_t path, path_t commandLine, path_t curDir, bool readOnly) = 0;
      virtual void stop(int exitCode) = 0;

      ProcessBase()
         : _listeners(nullptr)
      {

      }
   };

   struct MessageLogInfo
   {
      path_t path;
      int    row;
      int    column;
   };

   class ErrorLogBase
   {
   public:
      virtual void addMessage(text_str message, text_str file, text_str row, text_str col) = 0;

      virtual MessageLogInfo getMessage(int index) = 0;

      virtual void clearMessages() = 0;
   };

   // --- DebugControllerBase ---
   constexpr auto DEBUG_CLOSE = 0;
   constexpr auto DEBUG_SUSPEND = 1;
   constexpr auto DEBUG_RESUME = 2;
   constexpr auto DEBUG_ACTIVE = 3;
   constexpr auto MAX_DEBUG_EVENT = 4;

   class DebugControllerBase
   {
   public:
      virtual void debugThread() = 0;
   };

   // --- DebugProcessException ---
   struct DebugProcessException
   {
      int   code;
      addr_t address;
   };

   // --- DebugProcessBase ---
   class DebugProcessBase
   {
   public:
      virtual bool startThread(DebugControllerBase* controller) = 0;
      virtual bool startProgram(const wchar_t* exePath, const wchar_t* cmdLine) = 0;

      virtual void activate() = 0;
      virtual void run() = 0;
      virtual bool proceed(int timeout) = 0;
      virtual void stop() = 0;
      virtual void reset() = 0;

      virtual void setStepMode() = 0;

      virtual DebugProcessException* Exception() = 0;
      virtual void resetException() = 0;

      virtual void initEvents() = 0;
      virtual void setEvent(int event) = 0;
      virtual void resetEvent(int event) = 0;
      virtual bool waitForEvent(int event, int timeout) = 0;
      virtual int waitForAnyEvent() = 0;
      virtual void clearEvents() = 0;

      virtual bool isStarted() = 0;
      virtual bool isTrapped() = 0;
      virtual bool isInitBreakpoint() = 0;

      virtual void initHook() = 0;

      virtual addr_t getBaseAddress() = 0;
      virtual void* getState() = 0;

      virtual void setBreakpoint(addr_t address, bool withStackLevelControl) = 0;

      virtual void addStep(addr_t address, void* current) = 0;

      virtual bool readDump(addr_t address, char* s, pos_t length) = 0;

      virtual addr_t findEntryPoint(path_t programPath) = 0;
      virtual bool findSignature(StreamReader& reader, char* signature, pos_t length) = 0;

      virtual ~DebugProcessBase() = default;
   };

   struct GUISettinngs
   {
      bool withTabAboverscore;
   };

   // --- GUIFactory ---
   class GUIFactoryBase
   {
   public:
      virtual GUIApp* createApp() = 0;
      virtual GUIControlBase* createMainWindow(NotifierBase* notifier, ProcessBase* outputProcess) = 0;
   };
}

#endif
