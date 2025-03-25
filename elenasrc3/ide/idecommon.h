//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE common classes header File
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef IDECOMMON_H
#define IDECOMMON_H

#include "elena.h"
#include "guieditor.h"

namespace elena_lang
{
   constexpr auto PLATFORM_CATEGORY                   = "configuration/platform";
   constexpr auto NAMESPACE_CATEGORY                  = "configuration/project/namespace";
   constexpr auto TEMPLATE_CATEGORY                   = "configuration/project/template";
   constexpr auto OPTIONS_CATEGORY                    = "configuration/project/options";
   constexpr auto TARGET_CATEGORY                     = "configuration/project/executable";
   constexpr auto FILE_CATEGORY                       = "configuration/files/module/file";

   constexpr auto TARGET_SUB_CATEGORY                 = "project/executable";
   constexpr auto TEMPLATE_SUB_CATEGORY               = "project/template";
   constexpr auto NAMESPACE_SUB_CATEGORY              = "project/namespace";
   constexpr auto OPTIONS_SUB_CATEGORY                = "project/options";
   constexpr auto OUTPUT_SUB_CATEGORY                 = "project/output";
   constexpr auto STRICT_TYPE_SETTING                 = "project/stricttype";

   constexpr auto PROFILE_CATEGORY                    = "/profile";

   constexpr auto MODULE_CATEGORY                     = "files/*";
   constexpr auto REFERENCE_CATEGORY                  = "references/*";

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

   // --- Event IDs ---
   // NOTE : the generic notifications must be less than 0x100
   constexpr int EVENT_TEXTVIEW_MODEL_CHANGED         = 0x0001;
   constexpr int EVENT_TEXTFRAME_SELECTION_CHANGED    = 0x0002;
   constexpr int EVENT_STARTUP                        = 0x0003;
   constexpr int EVENT_LAYOUT                         = 0x0004;
   constexpr int EVENT_PROJECTVIEW_SELECTION_CHANGED  = 0x0005;
   constexpr int EVENT_COMPILATION_END                = 0x0006;
   constexpr int EVENT_ERRORLIST_SELECTION            = 0x0007;
   constexpr int EVENT_TEXT_CONTEXTMENU               = 0x0008;
   constexpr int EVENT_BROWSE_CONTEXT                 = 0x0009;
   constexpr int EVENT_TEXT_MARGINLICK                = 0x000A;

   // --- Event Statuses ---
   constexpr int STATUS_NONE                          = 0x0000;
   constexpr int STATUS_DOC_READY                     = 0x0001;
   constexpr int STATUS_STATUS_CHANGED                = 0x0002;
   constexpr int STATUS_PROJECT_CHANGED               = 0x0004;
   constexpr int STATUS_FRAME_VISIBILITY_CHANGED      = 0x0108;
   constexpr int STATUS_FRAME_CHANGED                 = 0x0010;
   constexpr int STATUS_DEBUGGER_NOSOURCE             = 0x0020;
   constexpr int STATUS_DEBUGGER_STOPPED              = 0x0040;
   constexpr int STATUS_DEBUGGER_STEP                 = 0x0080;
   constexpr int STATUS_LAYOUT_CHANGED                = 0x0100;
   constexpr int STATUS_COMPILING                     = 0x0200;
   constexpr int STATUS_PROJECT_REFRESH               = 0x0400;
   constexpr int STATUS_WITHERRORS                    = 0x0800;
   constexpr int STATUS_DEBUGGER_RUNNING              = 0x1000;
   constexpr int STATUS_DEBUGGER_FINISHED             = 0x2000;
   constexpr int STATUS_FRAME_ACTIVATE                = 0x4000;
   constexpr int STATUS_COLORSCHEME_CHANGED           = 0x8000;

   constexpr int NOTIFY_DEBUGGER_RESULT                  = 6;
   constexpr int NOTIFY_ERROR_SEL                        = 7;
   constexpr int NOTIFY_COMPILATION_RESULT               = 8;
   constexpr int NOTIFY_SHOW_RESULT                      = 9;
   constexpr int NOTIFY_DEBUG_CHANGE                     = 12;
   constexpr int NOTIFY_IDE_CHANGE                       = 13;
   constexpr int NOTIFY_ONSTART                          = 14;
   constexpr int NOTIFY_REFRESH                          = 15;
   constexpr int NOTIFY_DEBUG_START                      = 16;
   constexpr int NOTIFY_DEBUG_CONTEXT_EXPANDED           = 17;
   constexpr int NOTIFY_DEBUG_LOAD                       = 18;
   constexpr int NOTIFY_DEBUG_NOSOURCE                   = 19;
   constexpr int NOTIFY_DEBUG_RUNNING                    = 20;

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
      AutoRecompiling      = 3,
      Compiling            = 4,
      CompiledSuccessfully = 5,
      CompiledWithWarnings = 6,
      CompiledWithErrors   = 7,
      Broken               = 8,
      DebuggerStopped      = 9,
      Running              = 10,
      Stopped              = 11,
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
      virtual void afterExecution(int exitCode, int extraArg) = 0;
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

      virtual bool start(path_t path, path_t commandLine, path_t curDir, bool readOnly, int extraArg) = 0;
      virtual void stop(int exitCode) = 0;

      virtual bool write(const char* line, size_t length) = 0;
      virtual bool write(wchar_t ch) = 0;

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

   // --- ContextBrowserModel ---
   struct ContextBrowserModel
   {
      bool hexadecimalMode;

      ContextBrowserModel()
      {
         hexadecimalMode = false;
      }
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
      ContextBrowserModel* _model;

      virtual void* findWatchNodeStartingWith(WatchContext* root, ustr_t name) = 0;
      virtual void editWatchNode(void* item, ustr_t name, ustr_t className, addr_t address) = 0;
      virtual void* addWatchNode(void* parentItem, ustr_t name, ustr_t className, addr_t address) = 0;

      virtual void clearNode(void* item) = 0;
      virtual void populateNode(void* item, ustr_t value) = 0;

   public:
      virtual void clearRootNode() = 0;
      virtual void expandRootNode() = 0;

      virtual void expandNode(size_t param) = 0;

      virtual void refreshCurrentNode() = 0;

      virtual void* addOrUpdate(WatchContext* root, ustr_t name, ustr_t className);
      virtual void* addOrUpdateBYTE(WatchContext* root, ustr_t name, int value);
      virtual void* addOrUpdateWORD(WatchContext* root, ustr_t name, short value);
      virtual void* addOrUpdateDWORD(WatchContext* root, ustr_t name, int value);
      virtual void* addOrUpdateUINT(WatchContext* root, ustr_t name, int value);
      virtual void* addOrUpdateQWORD(WatchContext* root, ustr_t name, long long value);
      virtual void* addOrUpdateFLOAT64(WatchContext* root, ustr_t name, double value);

      virtual void removeUnused(WatchItems& refreshedItems) = 0;

      virtual void populateWORD(WatchContext* root, unsigned short value);
      virtual void populateDWORD(WatchContext* root, unsigned int value);
      virtual void populateUINT(WatchContext* root, unsigned int value);
      virtual void populateQWORD(WatchContext* root, long long value);
      virtual void populateFLOAT64(WatchContext* root, double value);
      virtual void populateString(WatchContext* root, const char* value);
      virtual void populateWideString(WatchContext* root, const wide_c* value);

      //virtual void browse() = 0;
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

   struct StartUpSettings
   {
      bool withExplicitConsole;
      bool includeAppPath2Paths;  // applicable only for Windows
   };

   // --- DebugProcessBase ---
   class DebugProcessBase
   {
   public:
      virtual bool startThread(DebugControllerBase* controller) = 0;
      virtual bool startProgram(path_t exePath, path_t cmdLine, path_t appPath, StartUpSettings& startUpSettings) = 0;

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
      virtual void addBreakpoint(addr_t address) = 0;
      virtual void removeBreakpoint(addr_t address) = 0;

      virtual void addStep(addr_t address, void* current) = 0;

      virtual bool readDump(addr_t address, char* s, pos_t length) = 0;

      virtual addr_t findEntryPoint(path_t programPath) = 0;
      virtual bool findSignature(StreamReader& reader, char* signature, pos_t length) = 0;

      virtual ~DebugProcessBase() = default;
   };

   class PathHelperBase
   {
   public:
      virtual void makePathRelative(PathString& path, path_t rootPath) = 0;
   };

   struct Breakpoint
   {
      int              row;
      IdentifierString source;
      IdentifierString module;
      void*            param;

      Breakpoint(int row, ustr_t source, ustr_t module)
         : row(row), source(source), module(module), param(nullptr)
      {
      }
   };

   struct GUISettinngs
   {
      bool withTabAboverscore;
   };

   // --- GUIFactory ---
   class GUIFactoryBase
   {
   public:
      virtual void reloadStyles(TextViewModelBase* model) = 0;

      virtual GUIApp* createApp() = 0;
      virtual GUIControlBase* createMainWindow(NotifierBase* notifier, ProcessBase* outputProcess,
         ProcessBase* vmConsoleProcess) = 0;
   };

   // --- SelectionEvent ---
   class SelectionEvent : public EventBase
   {
      int _eventId;
      int _index;

   public:
      int eventId() override;

      int Index() { return _index; }

      SelectionEvent(int id, int index);
   };

   // --- CompletionEvent ---
   class CompletionEvent : public EventBase
   {
      int _eventId;
      int _exitCode;
      int _postpinedAction;

   public:
      int eventId() override;

      int ExitCode() { return _exitCode; }

      int PostpinedAction() { return _postpinedAction; }

      CompletionEvent(int id, int exitCode, int postpinedAction);
   };

   // --- ParamSelectionEvent ---
   class ParamSelectionEvent : public EventBase
   {
      int    _eventId;
      size_t _param;

   public:
      int eventId() override;

      size_t Param() { return _param; }

      ParamSelectionEvent(int id, size_t param);
   };

   // --- BrowseEvent ---
   class BrowseEvent : public EventBase
   {
      int    _eventId;
      size_t _item;
      size_t _param;

   public:
      int eventId() override { return _eventId; }

      size_t Item() { return _item; }

      size_t Param() { return _param; }

      BrowseEvent(int id, size_t item, size_t param);
   };

   // --- LayoutEvent ---
   class LayoutEvent : public EventBase
   {
   public:
      int eventId() override;

      LayoutEvent(int status);
   };

   // --- ContextMenuEvent ---
   class ContextMenuEvent : public EventBase
   {
      int _eventId;
      int  _x, _y;
      bool _hasSelection;

   public:
      int eventId() override;

      int X() { return _x; }
      int Y() { return _y; }
      bool HasSelection() { return _hasSelection; }

      ContextMenuEvent(int id, int x, int y, bool hasSelection);
   };

   // --- StartUpEvent ---
   class StartUpEvent : public EventBase
   {
   public:
      int eventId() override;

      StartUpEvent(int status);
   };

   // --- SimpleEvent ---
   class SimpleEvent : public EventBase
   {
      int _eventId;

   public:
      int eventId() override;

      SimpleEvent(int eventId);
   };

   // --- TextViewModelEvent ---
   struct TextViewModelEvent : public EventBase
   {
   public:
      DocumentChangeStatus changeStatus;

      int eventId() override;

      TextViewModelEvent(int status, DocumentChangeStatus changeStatus)
         : EventBase(status), changeStatus(changeStatus)
      {
      }
   };

}

#endif
