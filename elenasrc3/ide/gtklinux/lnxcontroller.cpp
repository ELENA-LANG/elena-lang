//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains Linux process controller implementation
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/lnxcontroller.h"

using namespace elena_lang;

LinuxProcess :: LinuxProcess()
{
   args = nullptr
}

void LinuxProcess :: clear()
{
   if (_args) {
      size_t index = 0;

      while (_args[index]) {
         freestr(_args[index]);

         index++;
      }

      delete[] _args;

      _args = nullptr;
   }
}

void LinuxProcess :: setArguments(path_t cmdLine)
{
   size_t length = 1;   
   size_t index = cmdLine.find(' ');
   while (index != NOTFOUND_POS) {
      length++;
      
      while (cmdLine[++index] == ' ');

      index = cmdLine.findSub(index, ' ');
   }

   _args = new char* [length + 1];

   size_t start = 0;
   for (size_t i = 0; i < length; i++) {
      index = cmdLine.find(' ');
      if (index != NOTFOUND_POS) {
         PathString tmp(cmdLine + start, index - start);
         args[i++] = tmp.clone();

      }
      else _args[i++] = path_t(cmdLine + index).clone();

      while (cmdLine[++index] == ' ');

      start = index;
      index = cmdLine.findSub(index, ' ');
   }

   _args[i] = nullptr;
}

void LinuxProcess :: run(path_t path)
{
   stopped = false;
   exitCode = 0;

   int  stdinPipe[2];
   int  stdoutPipe[2];

   const char** args = _args;

   if (pipe(stdinPipe) < 0) {
      //perror("allocating pipe for child input redirect");
      return;
   }
   if (pipe(stdoutPipe) < 0) {
      ::close(stdinPipe[PIPE_READ]);
      ::close(stdinPipe[PIPE_WRITE]);
      //perror("allocating pipe for child output redirect");
      return;
   }

   int child = fork();
   if (child == 0) {
      // child continues here

      // redirect stdin
      if (dup2(stdinPipe[PIPE_READ], STDIN_FILENO) == -1) {
         //perror("redirecting stdin");
         return;
      }

      // redirect stdout
      if (dup2(stdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
         //perror("redirecting stdout");
         return;
      }

      // redirect stderr
      if (dup2(stdoutPipe[PIPE_WRITE], STDERR_FILENO) == -1) {
         //perror("redirecting stderr");
         return;
      }

      // all these are for use by parent only
      ::close(stdinPipe[PIPE_READ]);
      ::close(stdinPipe[PIPE_WRITE]);
      ::close(stdoutPipe[PIPE_READ]);
      ::close(stdoutPipe[PIPE_WRITE]);

      int retVal = execv(path, args);

      // if we get here at all, an error occurred, but we are in the child
      // process, so just exit
      //perror("exec of the child process");
      ::exit(retVal);
   }
   else if (child > 0) {
      // parent continues here

      // close unused file descriptors, these are for child only
      ::close(stdinPipe[PIPE_READ]);
      ::close(stdoutPipe[PIPE_WRITE]);

      // Include error check here
      //if (NULL != szMessage) {
      //   write(stdinPipe[PIPE_WRITE], szMessage, strlen(szMessage));
      //}

      while (true) {
         buf_len = read(stdoutPipe[PIPE_READ], buffer, 512);
         if (buf_len == 0)
            break;

         //owner->notifyOutput();

         // wait until the buffer is read
         while (true) {
            Glib::usleep(100);
            {
               Glib::Threads::Mutex::Lock lock(_mutex);
               if (buf_len == 0)
                  break;
            }
         }
      }

      // done with these in this example program, you would normally keep these
      // open of course as long as you want to talk to the child
      ::close(stdinPipe[PIPE_WRITE]);
      ::close(stdoutPipe[PIPE_READ]);

      int status;
      waitpid(child, &status, 0);
      if (WIFEXITED(status)) {
         exitCode = WEXITSTATUS(status);
      }

      stopped = true;
      //owner->notifyCompletion(exitCode);
   }
   else {
      // failed to create child
      ::close(stdinPipe[PIPE_READ]);
      ::close(stdinPipe[PIPE_WRITE]);
      ::close(stdoutPipe[PIPE_READ]);
      ::close(stdoutPipe[PIPE_WRITE]);
   }
}

bool LinuxProcess :: start(path_t path, path_t cmdLine, path_t curDir, bool readOnly, int extraArg)
{
   setArguments(cmdLine);

   _outputThread = Glib::Threads::Thread::create(
      sigc::bind(sigc::mem_fun(_outputProcess, &OutputProcess::compile), path));

   return false; //!! temporal
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
