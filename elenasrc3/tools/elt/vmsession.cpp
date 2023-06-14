//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM session code
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
#include "vmsession.h"
#include "core.h"
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
{
   _started = false;

   _encoding = FileEncoding::UTF8;

   _presenter = presenter;
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

      return true;
   }
   else return false;
}

void VMSession :: executeCommandLine(const char* line)
{
   DynamicString<char> command;

   command.append(_body.str());
   command.append('\n');
   command.append(*_prefix);
   command.append(line);
   command.append(*_postfix);

   if (!executeScript(command.str()))
      _presenter->print(ELT_CODE_FAILED);

   _body.clear();
}

bool VMSession :: executeTape(void* tape)
{
   bool retVal = false;
   if (!_started) {
      retVal = connect(tape);
   }

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
      int length = GetStatusSMLA(error, 0x200);
      error[length] = 0;
      if (!emptystr(error)) {
         _presenter->print(ELT_SCRIPT_FAILED, error);
         return false;
      }
      return true;
   }
   return executeTape(tape);
}

bool VMSession :: connect(void* tape)
{
   SystemEnv env = { };

   env.gc_yg_size = 0x15000;
   env.gc_mg_size = 0x54000;

   int retVal = InitializeVMSTLA(&env, tape, nullptr);
   if (retVal != 0) {
      _presenter->print(ELT_STARTUP_FAILED);

      return false;
   }

   _started = true;

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

         //if (line[0] == '-') {
         //   if (!executeCommand(line, running))
         //      print("Invalid command, use -h to get the list of the commands\n");
         //}
         //else if (!emptystr(line) && line[getlength(line) - 1] == ';') {
         //   line[getlength(line) - 1] = 0;

         //   body.append(line.c_str());
         //}
         /*else */executeCommandLine(*line);
      }
      catch (...) {
         _presenter->print("Invalid operation");
      }
   } while (running);
}
