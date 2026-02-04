//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains Linux process controller header
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LNXCONTROLLER_H
#define LNXCONTROLLER_H

#include "idecommon.h"

namespace elena_lang
{
   // -- LinuxProcess ---
   class LinuxProcess : public ProcessBase
   {
      Glib::Threads::Thread* _outputThread;
      const char** _args;

      void setArguments(path_t cmdLine);

      void run(path_t path);

      void clear();

   public:
      bool start(path_t path, path_t commandLine, path_t curDir, bool readOnly, int extraArg) override;

      void stop(int exitCode = 0) override;

      bool write(char ch) override;
      bool write(const char* line, size_t length) override;

      LinuxProcess();
   };
}

#endif // WIN32CONTROLLER_H
