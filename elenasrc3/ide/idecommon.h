//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE common classes header File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef IDECOMMON_H
#define IDECOMMON_H

#include "guicommon.h"

#define IDE_REVISION_NUMBER                           0x003F

namespace elena_lang
{
   constexpr auto PLATFORM_CATEGORY                   = "configuration/platform";
   constexpr auto NAMESPACE_CATEGORY                  = "configuration/project/namespace";
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
   constexpr auto DEBUGGER_STOPPED                    = 0x0004;

   // --- Notification codes ---
   //constexpr auto NOTIFY_SOURCEMODEL                  = 1;
   constexpr auto NOTIFY_CURRENTVIEW_CHANGED             = 2;
   //constexpr auto NOTIFY_CURRENTVIEW_HIDE             = 4;
   //constexpr auto NOTIFY_LAYOUT_CHANGED               = 5;
   //constexpr auto NOTIFY_ACTIVATE_EDITFRAME           = 9;
   //constexpr auto NOTIFY_START_COMPILATION            = 10;
   //constexpr auto NOTIFY_PROJECTMODEL                 = 11;
   constexpr int NOTIFY_DEBUGGER_RESULT                  = 6;
   constexpr int NOTIFY_ERROR_SEL                        = 7;
   constexpr int NOTIFY_COMPILATION_RESULT               = 8;
   constexpr int NOTIFY_SHOW_RESULT                      = 9;
   constexpr int NOTIFY_TEXTFRAME_SEL                    = 10;
   constexpr int NOTIFY_PROJECTVIEW_SEL                  = 11;
   constexpr int NOTIFY_DEBUG_CHANGE                     = 12;
   constexpr int NOTIFY_IDE_CHANGE                       = 13;
   constexpr int NOTIFY_ONSTART                          = 14;
   constexpr int NOTIFY_REFRESH                          = 15;

   // --- Notification statuses ---
   constexpr NotificationStatus IDE_ONSTART              = -1;
   constexpr NotificationStatus NONE_CHANGED             = 0x00000;

   constexpr NotificationStatus IDE_LAYOUT_CHANGED       = 0x00001;
   constexpr NotificationStatus IDE_STATUS_CHANGED       = 0x00002;
   constexpr NotificationStatus FRAME_CHANGED            = 0x00004;
   constexpr NotificationStatus PROJECT_CHANGED          = 0x00008;
   constexpr NotificationStatus FRAME_VISIBILITY_CHANGED = 0x00010;
   constexpr NotificationStatus DEBUGWATCH_CHANGED       = 0x00020;
   constexpr NotificationStatus IDE_COMPILATION_STARTED  = 0x00040;
   constexpr NotificationStatus OUTPUT_SHOWN             = 0x00080;
   constexpr NotificationStatus FRAME_ACTIVATE           = 0x00100;

   // --- PathSettings ---
   struct PathSettings
   {
      PathString appPath;
   };

   // --- IDEStatus ---
   enum class IDEStatus
   {
      Empty                = 0,
      Ready                = 1,
      Busy                 = 2,
      AutoRecompiling      = 3,
      Compiling            = 4,
      CompiledSuccessfully = 5,
      CompiledWithWarnings = 6,
      CompiledWithErrors   = 7,
      Broken               = 8,
      DebuggerStopped      = 9,
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

   // --- ContextBrowserBase ---

   struct WatchContext
   {
      void*   root;
      addr_t  address;
   };

   typedef CachedList<void*, 20> WatchItems;

   class ContextBrowserBase
   {
   protected:
      virtual void* findWatchNodeStartingWith(WatchContext* root, ustr_t name) = 0;
      virtual void editWatchNode(void* item, ustr_t name, ustr_t className, addr_t address) = 0;
      virtual void* addWatchNode(void* parentItem, ustr_t name, ustr_t className, addr_t address) = 0;

      virtual void clearNode(void* item) = 0;
      virtual void populateNode(void* item, ustr_t value) = 0;

   public:
      virtual void expandRootNode() = 0;

      virtual void* addOrUpdate(WatchContext* root, ustr_t name, ustr_t className);
      virtual void* addOrUpdateBYTE(WatchContext* root, ustr_t name, int value);
      virtual void* addOrUpdateWORD(WatchContext* root, ustr_t name, short value);
      virtual void* addOrUpdateDWORD(WatchContext* root, ustr_t name, int value);
      virtual void* addOrUpdateQWORD(WatchContext* root, ustr_t name, long long value);
      virtual void* addOrUpdateFLOAT64(WatchContext* root, ustr_t name, double value);

      virtual void removeUnused(WatchItems& refreshedItems) = 0;

      virtual void populateWORD(WatchContext* root, unsigned short value);
      virtual void populateDWORD(WatchContext* root, unsigned int value);
      virtual void populateQWORD(WatchContext* root, unsigned long long value);
      virtual void populateFLOAT64(WatchContext* root, double value);
      virtual void populateString(WatchContext* root, const char* value);
      virtual void populateWideString(WatchContext* root, const wide_c* value);
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

      virtual int getDataOffset() = 0;

      virtual addr_t getBaseAddress() = 0;
      virtual void* getState() = 0;

      virtual addr_t getMemoryPtr(addr_t address) = 0;

      virtual addr_t getStackItem(int index, disp_t offset = 0) = 0;
      virtual addr_t getStackItemAddress(disp_t disp) = 0;

      virtual addr_t getField(addr_t address, int index) = 0;
      virtual addr_t getFieldAddress(addr_t address, disp_t disp) = 0;

      virtual addr_t getClassVMT(addr_t address) = 0;
      virtual ref_t getClassFlags(addr_t vmtAddress) = 0;

      virtual size_t getArrayLength(addr_t address) = 0;

      virtual char getBYTE(addr_t address) = 0;
      virtual unsigned short getWORD(addr_t address) = 0;
      virtual unsigned int getDWORD(addr_t address) = 0;
      virtual unsigned long long getQWORD(addr_t address) = 0;
      virtual double getFLOAT64(addr_t address) = 0;

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
