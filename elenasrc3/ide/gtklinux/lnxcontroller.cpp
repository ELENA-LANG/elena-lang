//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains Linux process controller implementation
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/lnxcontroller.h"

#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

using namespace elena_lang;

// --- LinuxProcess ---

LinuxProcess :: LinuxProcess()
   : _outputThread(nullptr)
{
   _stopped = true;
   _exitCode = 0;
   _extraArg = 0;
   _buf_len = 0;
}

void LinuxProcess :: freeOutputThread()
{
   if  (_outputThread) {
      _outputThread->detach();

      delete _outputThread;

      _outputThread = nullptr;
   }
}

char** LinuxProcess :: generateArguments(path_t cmdLine)
{
   size_t length = 1;
   size_t index = cmdLine.find(' ');
   while (index != NOTFOUND_POS) {
      length++;

      while (cmdLine[++index] == ' ');

      index = cmdLine.findSub(index, ' ');
   }

   auto args = new char* [length + 1];

   size_t start = 0;
   for (size_t i = 0; i < length; i++) {
      index = cmdLine.findSub(start, ' ');
      if (index != NOTFOUND_POS) {
         PathString tmp(cmdLine + start, index - start);
         args[i] = (*tmp).clone();

      }
      else args[i] = path_t(cmdLine + start).clone();

      while (cmdLine[++index] == ' ');

      start = index;
   }

   args[length] = nullptr;

   return args;
}

void LinuxProcess :: run(path_t path, path_t cmdLine)
{
   _stopped = false;

   const char* path_str = path.str();
   const char* cmdLine_str = cmdLine.str();

//   int  stdinPipe[2] = {};
   int  stdoutPipe[2] = {};

//   if (pipe(stdinPipe) < 0) {
//      //perror("allocating pipe for child input redirect");
//      return;
//   }
   if (pipe(stdoutPipe) < 0) {
//      ::close(stdinPipe[PIPE_READ]);
//      ::close(stdinPipe[PIPE_WRITE]);
//      //perror("allocating pipe for child output redirect");
      return;
   }

   int child = fork();
   if (child == 0) {
      // child continues here

      // redirect stdin
//      if (dup2(stdinPipe[PIPE_READ], fileno(stdin)) == -1) {
//         //perror("redirecting stdin");
//         return;
//      }

      if (dup2(stdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
//         printf("redirecting stdout error");
         return;
      }

      // redirect stderr
//      if (dup2(stdoutPipe[PIPE_WRITE], fileno(stderr)) == -1) {
//         printf("redirecting stderr error");
//         return;
//      }

      // all these are for use by parent only
//      ::close(stdinPipe[PIPE_READ]);
//      ::close(stdinPipe[PIPE_WRITE]);
      ::close(stdoutPipe[PIPE_READ]);
      ::close(stdoutPipe[PIPE_WRITE]);

      char* const* args = (char* const*)generateArguments(cmdLine_str);
      int retVal = execv(path_str, args);

      // if we get here at all, an error occurred, but we are in the child
      // process, so just exit
      //perror("exec of the child process");
      ::exit(retVal);
   }
   else if (child > 0) {
      // parent continues here

      // close unused file descriptors, these are for child only
//      ::close(stdinPipe[PIPE_READ]);
      ::close(stdoutPipe[PIPE_WRITE]);

      // Include error check here
      //if (NULL != szMessage) {
      //   write(stdinPipe[PIPE_WRITE], szMessage, strlen(szMessage));
      //}

      while (true) {
         _buf_len = read(stdoutPipe[PIPE_READ], _buffer, 511);
         if (_buf_len == 0)
            break;

         _buffer[_buf_len] = 0;
         //printf(_buffer);

         writeStdOut();
      }

      // done with these in this example program, you would normally keep these
//      // open of course as long as you want to talk to the child
//      ::close(stdinPipe[PIPE_WRITE]);
      ::close(stdoutPipe[PIPE_READ]);

      int status;
      waitpid(child, &status, 0);
      if (WIFEXITED(status)) {
         _exitCode = WEXITSTATUS(status);
      }

      _stopped = true;
      afterExecution();
   }
   else {
      // failed to create child
      //::close(stdinPipe[PIPE_READ]);
      //::close(stdinPipe[PIPE_WRITE]);
      ::close(stdoutPipe[PIPE_READ]);
      ::close(stdoutPipe[PIPE_WRITE]);
   }
}

bool LinuxProcess :: start(path_t path, path_t cmdLine, path_t curDir, bool readOnly, int extraArg)
{
   if (!_stopped)
      return false;

   chdir(curDir.str());

   _extraArg = extraArg;

   freeOutputThread();
   _outputThread = new std::thread(
      [this,path,cmdLine]
      {
        this->run(path, cmdLine);
      });

   return true;
}

void LinuxProcess :: stop(int exitCode)
{
//   if (_hChildProcess) {
//      TerminateProcess(_hChildProcess, exitCode);

//      WaitForSingleObject(_hChildProcess, INFINITE);

//      _hChildProcess = nullptr;
//   }
}

bool LinuxProcess :: write(const char* line, size_t length)
{
//   if (!_hStdinWrite)
//      return false;

//   if (_offset + length < BUF_SIZE) {
//      memcpy(_buffer + _offset, line, length);
//      _offset += length;

//      if (line[length - 1] == '\n')
//         flush();

      //return true;
   //}
   /*else */return false;
}

bool LinuxProcess :: write(char ch)
{
//   if (ch == 8) {
//      if (_offset > 0) {
//         _offset--;

//         return true;
//      }
      /*else */return false;
   //}
   //else return write((const char*)&ch, 1);
}

void LinuxProcess :: writeStdOut()
{
   //std::lock_guard<std::mutex> lock(_mutex);

   _buffer[_buf_len] = 0;

   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onOutput(_buffer);
   }

   _buf_len = 0;
}

void LinuxProcess :: writeStdError(const char* error)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onErrorOutput(error);
   }
}

void LinuxProcess :: afterExecution()
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->afterExecution(_exitCode, _extraArg);
   }
}
