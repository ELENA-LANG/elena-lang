//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM session code
//
//                                             (C)2023-2025, by Aleksey Rakov
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

inline bool isLetterOrDigit(char ch)
{
   return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= '0' && ch <= '9');
}


// --- VMSession ---

VMSession :: VMSession(PresenterBase* presenter)
   : _env({}), _imports(nullptr)
{
   _started = false;

   _encoding = FileEncoding::UTF8;

   _presenter = presenter;
}

inline void copyPrefixPostfix(ustr_t s, size_t start, size_t end, IdentifierString& prefix, IdentifierString& postfix)
{
   size_t pos = s.findSubStr(start, "$1", end-start);
   if (pos != NOTFOUND_POS) {
      prefix.copy(s + start, pos - start);
      postfix.copy(s + pos + 2, end - pos - 2);
   }
   else prefix.copy(s + start, end - start);
}

bool VMSession :: loadTemplate(path_t path)
{
   char buff[512];

   TextFileReader reader(path, FileEncoding::UTF8, false);

   if (reader.isOpen()) {
      IdentifierString content;
      reader.readAll(content, buff);

      size_t block2 = (*content).findStr("$$");
      size_t block3 = (*content).findSubStr(block2 + 2, "$$", content.length() - block2 - 2);
      size_t block4 = (*content).findSubStr(block3 + 2, "$$", content.length() - block3 - 2);

      copyPrefixPostfix(*content, 0, block2, _prefix1, _postfix1);
      copyPrefixPostfix(*content, block2 + 2, block3, _prefix2, _postfix2);
      copyPrefixPostfix(*content, block3 + 2, block4, _prefix3, _postfix3);
      copyPrefixPostfix(*content, block4 + 2, content.length(), _prefix4, _postfix4);

      return true;
   }
   else return false;
}

inline void insertVariables(DynamicString<char>& text, size_t index, ustr_t prefix, ustr_t postfix)
{
   size_t i = index;
   while (i < text.length()) {
      size_t pos = ustr_t(text.str()).findSub(i, '$');
      if (pos == NOTFOUND_POS)
         break;

      IdentifierString varName;
      size_t j = pos + 1;
      while (isLetterOrDigit(text[j])) {
         varName.append(text[j]);
         j++;
      }
         
      text.cut(pos, j - pos);

      text.insert(postfix, pos);
      text.insert(*varName, pos);
      text.insert(prefix, pos);

      i = pos;
   }
}

void VMSession :: executeCommandLine(const char* line, ustr_t prefix, ustr_t postfix)
{
   DynamicString<char> command;

   for (auto it = _imports.start(); !it.eof(); ++it) {
      command.append("import ");
      command.append(*it);
      command.append(";\n");
   }

   command.append(prefix);
   command.append(line);
   command.append(postfix);

   insertVariables(command, 0, *_prefix4, *_postfix4);

   if (!executeScript(command.str())) {
      _presenter->print(ELT_CODE_FAILED);
   }
}

bool VMSession :: executeAssigning(ustr_t line)
{
   size_t assingIndex = line.findStr(":=");

   IdentifierString varName(line + 1, assingIndex - 1);
   while (varName[varName.length() - 1] == ' ')
      varName.truncate(varName.length() - 1);

   if ((*varName).find(' '))
      return false;

   DynamicString<char> text;
   text.append('\"');
   text.append(*varName);
   text.append("\", ");
   text.append(line + assingIndex + 2);

   insertVariables(text, assingIndex + 2, *_prefix4, *_postfix4);

   executeCommandLine(text.str(), *_prefix3, *_postfix3);
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
   _presenter->print("<expr>                     - evaluate the expression and print the result\n");
   _presenter->print("$<var> := <expr>           - assign a global variable\n");
   _presenter->print("@multiline                 - switching to a multi-line mode\n");
   _presenter->print("@eval                      - executing the multi-line code\n");
   _presenter->print("@quit                      - quit\n");
   _presenter->print("@help                      - help\n");
   _presenter->print("@load <path>               - execute a script from file\n");
   _presenter->print("@add import <reference>    - importing a module into the session\n");
   _presenter->print("@remove import <reference> - removing a module from the session\n");
}

inline bool isAssignment(ustr_t line)
{
   if (line[0] != '$')
      return false;

   size_t len = line.length();
   size_t i = 1;
   while (i < len && (isLetterOrDigit(line[i])))
      i++;

   while (line[i] == ' ')
      i++;

   return (i < len - 1) && (line[i] == ':' && line[i + 1] == '=');
}

bool VMSession :: executeCommand(const char* line, bool& running)
{
   size_t len = getlength(line);
   if (len < 2)
      return false;

   // check commands
   if (line[1] == 'q' || (len > 2 && ustr_t(line).compare("@quit"))) {
      running = false;
   }
   else if (line[1] == 'h' || (len > 2 && ustr_t(line).compare("@help"))) {
      printHelp();
   }
   else if (line[1] == 'l') {
      if (line[2] == ' ') {
         loadScript(line + 2);
      }
      else if (ustr_t(line).startsWith("@load ")) {
         loadScript(line + 6);
      }
   }
   else if (line[1] == 'a' && ustr_t(line).startsWith("@add import ")) {
      IdentifierString module(line + 12);

      _imports.add((*module).clone());
   }
   else if (line[1] == 'r' && ustr_t(line).startsWith("@remove import ")) {
      IdentifierString module(line + 15);

      _imports.cut(*module);
   }
   else if (ustr_t(line).compare("@multiline")) {
      _multiLine = true;
   }
   else if (ustr_t(line).compare("@eval")) {
      if (isAssignment(_body.str())) {
         executeAssigning(_body.str());
      }
      else executeCommandLine(_body.str(), *_prefix2, *_postfix2);

      _multiLine = false;
      _body.clear();
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

         if (line[0] == '@') {
            if (!executeCommand(*line, running))
               _presenter->print("Invalid command, use -h to get the list of the commands\n");
         }
         else if (_multiLine) {
            line[line.length() - 1] = 0;

            _body.append(*line);
         }
         else if (isAssignment(*line)) {
            executeAssigning(*line);
         }
         else executeCommandLine(*line, *_prefix1, *_postfix1);
      }
      catch (...) {
         _presenter->print("Invalid operation");
      }
   } while (running);
}
