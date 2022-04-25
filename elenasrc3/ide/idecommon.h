//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE common classes header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef IDECOMMON_H
#define IDECOMMON_H

#include "guicommon.h"
#include "eng/messages.h"

#define IDE_REVISION_NUMBER                           0x000E

namespace elena_lang
{
   constexpr auto ERROR_RUN_NEED_TARGET               = 0x0001;
   constexpr auto ERROR_DEBUG_FILE_NOT_FOUND_COMPILE  = 0x0002;
   constexpr auto ERROR_RUN_NEED_RECOMPILE            = 0x0003;

   // --- Notification codes ---
   constexpr auto NOTIFY_SOURCEMODEL                  = 1;
   constexpr auto NOTIFY_CURRENTVIEW_CHANGED          = 2;
   constexpr auto NOTIFY_CURRENTVIEW_SHOW             = 3;
   constexpr auto NOTIFY_CURRENTVIEW_HIDE             = 4;

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
      virtual GUIControlBase* createMainWindow(NotifierBase* notifier) = 0;
   };
}

#endif
