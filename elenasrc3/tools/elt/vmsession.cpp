//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM session code
//
//                                             (C)2023-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
#include "vmsession.h"
#include "elenasm.h"
#include "elenavm.h"
#include "eltconst.h"

using namespace elena_lang;

#define MAX_LINE           256

const char* trim(const char* s)
{
   while (s[0] == 0x20)s++;

   return s;
}

// --- VMSession ---

VMSession :: VMSession(PresenterBase* presenter)
   : _env({})
{
   _started = false;

   _encoding = FileEncoding::UTF8;

   _presenter = presenter;

   _prefixBookmark = 0;
}

bool VMSession :: loadTemplate(path_t path)
{
   char buff[512];

   TextFileReader reader(path, FileEncoding::UTF8, false);

   if (reader.isOpen()) {
      IdentifierString content;
      reader.readAll(content, buff);

      size_t index = (*content).findStr("$1");
      if (index != NOTFOUND_POS) {
         _prefix.copy(*content, index);
         _postfix.copy(*content + index + 2);
      }
      else _prefix.copy(*content);

      _prefixBookmark = 0;

      return true;
   }
   else return false;
}

void VMSession :: executeCommandLine(const char* line)
{
   DynamicString<char> command;

   command.append(*_prefix);
   command.append(_body.str());
   command.append('\n');
   command.append(line);
   command.append(*_postfix);

   if (!executeScript(command.str())) {
      _presenter->print(ELT_CODE_FAILED);
   }

   _body.clear();
}

bool VMSession :: executeTape(void* tape)
{
   bool retVal = false;
   if (!_started) {
      retVal = connect(tape);
   }
   else retVal = execute(tape);

   ReleaseSMLA(tape);

   return retVal;
}

bool VMSession :: loadScript(ustr_t pathStr)
{
   pathStr = trim(pathStr);

   void* tape = InterpretFileSMLA(pathStr, (int)_encoding, false);
   if (tape == nullptr) {
      char error[0x200];
      size_t length = GetStatusSMLA(error, 0x200);
      error[length] = 0;
      if (!emptystr(error)) {
         _presenter->print(ELT_SCRIPT_FAILED, error);
         return false;
      }
      return true;
   }
   return executeTape(tape);
}

bool VMSession :: executeScript(const char* script)
{
   void* tape = InterpretScriptSMLA(script);
   if (tape == nullptr) {
      char error[0x200];
      size_t length = GetStatusSMLA(error, 0x200);
      error[length] = 0;
      if (!emptystr(error)) {
         _presenter->printLine(ELT_SCRIPT_FAILED, error);
         return false;
      }
      return true;
   }
   return executeTape(tape);
}

void VMSession :: start()
{
   executeScript("[[ #start; ]]");

   _started = true;
}

bool VMSession :: connect(void* tape)
{
   _env.gc_yg_size = 0x15000;
   _env.gc_mg_size = 0x54000;

   int retVal = InitializeVMSTLA(&_env, tape, ELT_EXCEPTION_HANDLER);
   if (retVal != 0) {
      _presenter->printLine(ELT_STARTUP_FAILED);

      return false;
   }

   return true;
}

bool VMSession :: execute(void* tape)
{
   if (EvaluateVMLA(tape) != 0) {
      return false;
   }

   return true;
}

void VMSession::printHelp()
{
   _presenter->print("-q                         - quit\n");
   _presenter->print("-c                         - clear\n");
   _presenter->print("-h                         - help\n");
   _presenter->print("-l <path>                  - execute a script from file\n");
   _presenter->print("-p<script>;                - prepend the prefix code\n");
   _presenter->print("{ <script>; }*\n <script>                  - execute script\n");
}

bool VMSession :: executeCommand(const char* line, bool& running)
{
   if (getlength(line) < 2)
      return false;

   // check commands
   if (line[1] == 'q') {
      running = false;
   }
   else if (line[1] == 'h') {
      printHelp();
   }
   else if (line[1] == 'l') {
      loadScript(line + 2);
   }
   else if (line[1] == 'c') {
      _body.clear();
      _prefix.cut(0, _prefixBookmark);
      _prefixBookmark = 0;
   }
   else if (line[1] == 'p') {
      ustr_t snipet = line + 2;

      _prefix.insert(snipet, _prefixBookmark);
      _prefixBookmark += getlength(snipet);
   }
   else return false;

   return true;
}

void VMSession :: run()
{
   char          buffer[MAX_LINE];
   //IdentifierString line;
   bool          running = true;

   do {
      try {
         _presenter->print("\n>");

         _presenter->readLine(buffer, MAX_LINE);

         IdentifierString line(buffer, getlength(buffer));

         while (!line.empty() && line[line.length() - 1] == '\r' || line[line.length() - 1] == '\n')
            line[line.length() - 1] = 0;

         while (!line.empty() && line[line.length() - 1] == ' ')
            line[line.length() - 1] = 0;

         if (line[0] == '-') {
            if (!executeCommand(*line, running))
               _presenter->print("Invalid command, use -h to get the list of the commands\n");
         }
         else if (!line.empty() && line[line.length() - 1] == ';') {
            line[line.length() - 1] = 0;

            _body.append(*line);
         }
         else executeCommandLine(*line);
      }
      catch (...) {
         _presenter->print("Invalid operation");
      }
   } while (running);
}
