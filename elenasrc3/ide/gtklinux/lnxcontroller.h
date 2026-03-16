//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains Linux process controller header
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LNXCONTROLLER_H
#define LNXCONTROLLER_H

#include "idecommon.h"
#include <thread>

namespace elena_lang
{
   // -- LinuxProcess ---
   class LinuxProcess : public ProcessBase
   {
      std::thread*         _outputThread;

      int      _extraArg;
      bool     _stopped;
      int      _exitCode;

      char     _buffer[512];
      int      _buf_len;

      void freeOutputThread();

      void writeStdOut();
      void writeStdError(const char* error);
      void afterExecution();

      void run(path_t path, path_t cmdLine);

   public:
      static char** generateArguments(path_t cmdLine);

      bool start(path_t path, path_t commandLine, path_t curDir, bool readOnly, int extraArg) override;

      void stop(int exitCode = 0) override;

      bool write(char ch) override;
      bool write(const char* line, size_t length) override;

      LinuxProcess();
   };
}

#endif // WIN32CONTROLLER_H
