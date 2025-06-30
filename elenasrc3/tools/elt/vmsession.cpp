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

inline void trimLine(IdentifierString& line)
{
   while (!line.empty() && line[line.length() - 1] == '\r' || line[line.length() - 1] == '\n')
      line[line.length() - 1] = 0;

   while (!line.empty() && line[line.length() - 1] == ' ')
      line[line.length() - 1] = 0;
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

inline void copyPrefixPostfix(ustr_t s, size_t start, size_t end, TemplateInfo& info)
{
   size_t pos = s.findSubStr(start, "$1", end - start);
   if (pos != NOTFOUND_POS) {
      info.prefix.copy(s + start, pos - start);
      info.postfix.copy(s + pos + 2, end - pos - 2);
   }
   else info.postfix.copy(s + start, end - start);

   trimLine(info.prefix);
   trimLine(info.postfix);
}

inline size_t findTerminator(ustr_t text, size_t index)
{
   bool quoteMode = false;
   size_t i = index;
   while (text[i]) {
      if (text[i]=='"') {
         if (quoteMode) {
            if (text[i + 1] != '"') {
               quoteMode = false;
            }
            else i++;
         }
         else quoteMode = true;
      }
      if (!quoteMode && text[i] == ';')
         break;
      i++;
   }

   return i;
}

inline bool insertVariablesAssignment(DynamicString<char>& text, size_t index, ustr_t prefix, ustr_t postfix)
{
   size_t i = index;
   while (i < text.length()) {
      size_t pos = ustr_t(text.str()).findSub(i, '$');
      if (pos == NOTFOUND_POS)
         break;

      if (!isAssignment(text.str() + pos)) {
         i = pos + 1;

         continue;
      }

      IdentifierString varName;
      size_t j = pos + 1;
      while (isLetterOrDigit(text[j])) {
         varName.append(text[j]);
         j++;
      }

      size_t assignPos = ustr_t(text.str()).findSubStr(pos, ":=", text.length() - pos);
      size_t endPos = findTerminator(text.str(), assignPos);
      if (endPos == NOTFOUND_POS) {
         return false;
      }
      IdentifierString expr(text.str() + assignPos + 2, endPos - assignPos - 2);

      text.cut(pos, endPos - pos + 1);

      text.insert(postfix, pos);
      text.insert(*expr, pos);
      text.insert("\" ,", pos);
      text.insert(*varName, pos);
      text.insert("\"", pos);
      text.insert(prefix, pos);

      i = pos;
   }

   return true;
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
      text.insert("\"", pos);
      text.insert(*varName, pos);
      text.insert("\"", pos);
      text.insert(prefix, pos);

      i = pos;
   }
}

// --- VMSession ---

VMSession :: VMSession(path_t appPath, PresenterBase* presenter)
   : _appPath(appPath), _env({}), _imports(nullptr)
{
   _started = false;

   _encoding = FileEncoding::UTF8;

   _presenter = presenter;
}

bool VMSession :: loadTemplate(TemplateType type, ustr_t name)
{
   _presenter->print(ELT_LOADING_TEMPLATE, name);

   if (name.find(PATH_SEPARATOR) != NOTFOUND_POS)
      return false;

   char buffer[1024];

   PathString path(*_appPath);
   path.combine("scripts");
   path.combine(name);
   path.changeExtension("elt");

   TextFileReader reader(*path, FileEncoding::UTF8, false);

   DynamicString<char> helpLine;
   DynamicString<char> content;
   if (!reader.isOpen())
      return false;

   while (reader.read(buffer, 1024)) {
      if (ustr_t(buffer).startsWith("///")) {
         helpLine.append(buffer + 3);
      }
      else content.append(buffer);
   }

   if (!helpLine.empty())
      _presenter->print(helpLine.str());

   switch (type) {
      case TemplateType::REPL:
         _repl.clear();
         copyPrefixPostfix(content.str(), 0, content.length(), _repl);
         break;
      case TemplateType::Multiline:
         _multiline.clear();
         copyPrefixPostfix(content.str(), 0, content.length(), _multiline);
         break;
      case TemplateType::GetVar:
         _get_var.clear();
         copyPrefixPostfix(content.str(), 0, content.length(), _get_var);
         break;
      case TemplateType::SetVar:
         _set_var.clear();
         copyPrefixPostfix(content.str(), 0, content.length(), _set_var);
         break;
      default:
         return false;
   }

   return true;
}

void VMSession::printHelp()
{
   _presenter->print("@help                      - help\n");
   _presenter->print("@quit                      - quit\n");
   _presenter->print("@multiline                 - switching to a multi-line mode\n");

   _presenter->print("<expr>                     - evaluate the expression and print the result\n");
   _presenter->print("$<var> := <expr>;          - assign a global variable\n");
   _presenter->print(".. $<var>  ..              - get a global variable value\n");

   _presenter->print("@base <path>               - set the base path for scripts\n");
   _presenter->print("@load <path>               - execute a script from file\n");
   _presenter->print("@import <path>             - load the script into multi-line script\n");

   _presenter->print("@use <template>            - use the template for multiline script\n");

   _presenter->print("@eval                      - executing the multi-line code and switch back to REPL mode\n");
   _presenter->print("@clear                     - clear the multi-line code and switch back to REPL mode\n");
   _presenter->print("@print                     - print the multi-line code\n");
   _presenter->print("@add import <reference>    - importing a module into the session\n");
   _presenter->print("@remove import <reference> - removing a module from the session\n");
}

void VMSession::setBasePath(ustr_t baseStr)
{
   _basePath.copy(baseStr);
}

bool VMSession::loadScript(ustr_t pathStr)
{
   void* tape = nullptr;

   pathStr = trim(pathStr);

   if (pathStr.find(PATH_SEPARATOR) == NOTFOUND_POS) {
      PathString totalPath(_basePath);
      totalPath.combine(pathStr);
      totalPath.changeExtension("es");

      IdentifierString totalPathStr(*totalPath);
      tape = InterpretFileSMLA(totalPathStr.str(), (int)_encoding, false);
   }
   else tape = InterpretFileSMLA(pathStr, (int)_encoding, false);

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

bool VMSession::importScript(ustr_t scriptName)
{
   PathString totalPath(_basePath);
   totalPath.combine(scriptName);
   totalPath.changeExtension("es");

   TextFileReader reader(*totalPath, _encoding, false);
   if (!reader.isOpen())
      return false;

   char buffer[1024];
   while (reader.read(buffer, 1024)) {
      _body.append(buffer);
   }
}

void VMSession :: executeCommandLine(bool preview, TemplateType type, ustr_t script)
{
   DynamicString<char> command;

   for (auto it = _imports.start(); !it.eof(); ++it) {
      command.append("import ");
      command.append(*it);
      command.append(";\n");
   }

   switch (type) {
      case TemplateType::REPL:
         command.append(*_repl.prefix);
         command.append(script);
         command.append(*_repl.postfix);
         break;
      case TemplateType::Multiline:
         command.append(*_multiline.prefix);
         command.append(script);
         command.append(*_multiline.postfix);
         break;
      default:
         break;
   }

   insertVariablesAssignment(command, 0, *_set_var.prefix, *_set_var.postfix);
   insertVariables(command, 0, *_get_var.prefix, *_get_var.postfix);

   if (preview) {
      _presenter->printLine(command.str());
   }
   else if (!executeScript(command.str())) {
      _presenter->print(ELT_CODE_FAILED);
   }
}

bool VMSession::executeTape(void* tape)
{
   bool retVal = false;
   if (!_started) {
      retVal = connect(tape);
   }
   else retVal = execute(tape);

   ReleaseSMLA(0);

   return retVal;
}

bool VMSession::executeScript(const char* script)
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

void VMSession::start()
{
   executeScript("[[ #start; ]]");

   _started = true;
}

bool VMSession::connect(void* tape)
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

bool VMSession::execute(void* tape)
{
   if (EvaluateVMLA(tape) != 0) {
      return false;
   }

   return true;
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
   else if (ustr_t(line).compare("@multiline")) {
      _multiLine = true;
   }
   else if (ustr_t(line).startsWith("@base ")) {
      IdentifierString basePath(line + 6);

      setBasePath(*basePath);
   }
   else if (line[1] == 'l') {
      if (line[2] == ' ') {
         loadScript(line + 2);
      }
      else if (ustr_t(line).startsWith("@load ")) {
         loadScript(line + 6);
      }
   }
   else if (ustr_t(line).startsWith("@use ")) {
      IdentifierString pluginName(line + 5);

      if(!loadTemplate(TemplateType::Multiline, *pluginName))
         _presenter->printLine(ELT_CANNOT_LOAD_TEMPLATE, *pluginName);
   }
   else if (ustr_t(line).startsWith("@import ")) {
      IdentifierString scriptPath(line + 8);

      importScript(*scriptPath);
   }
   else if (ustr_t(line).compare("@eval")) {
      executeCommandLine(false, TemplateType::Multiline, _body.str());

      _multiLine = false;
      _body.clear();
   }
   else if (ustr_t(line).compare("@print")) {
      executeCommandLine(true, TemplateType::Multiline, _body.str());
   }
   else if (ustr_t(line).compare("@clear")) {
      _multiLine = false;
      _body.clear();
   }
   else if (line[1] == 'a' && ustr_t(line).startsWith("@add import ")) {
      IdentifierString module(line + 12);

      _imports.add((*module).clone());
   }
   else if (line[1] == 'r' && ustr_t(line).startsWith("@remove import ")) {
      IdentifierString module(line + 15);

      _imports.cut(*module);
   }
   else return false;

   return true;
}

void VMSession :: run()
{
   char          buffer[MAX_LINE];
   bool          running = true;

   do {
      try {
         if (!_multiLine)
            _presenter->print("\n>");

         _presenter->readLine(buffer, MAX_LINE);

         IdentifierString line(buffer, getlength(buffer));
         trimLine(line);

         while (!line.empty() && line[line.length() - 1] == '\r' || line[line.length() - 1] == '\n')
            line[line.length() - 1] = 0;

         while (!line.empty() && line[line.length() - 1] == ' ')
            line[line.length() - 1] = 0;

         if (line[0] == '@') {
            if (!executeCommand(*line, running))
               _presenter->print("Invalid command, use -h to get the list of the commands\n");
         }
         else if (_multiLine) {
            _body.append(*line);
            _body.append("\n");
         }
         else if (isAssignment(*line)) {
            executeCommandLine(false, TemplateType::Multiline, *line);
         }
         else executeCommandLine(false, TemplateType::REPL, *line);
      }
      catch (...) {
         _presenter->print("Invalid operation");
      }
   } while (running);
}
