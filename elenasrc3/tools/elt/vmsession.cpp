//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM session code
//
//                                             (C)2023-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
#include "vmsession.h"
#include "elenasm.h"
#include "elenavm.h"
#include "eltconst.h"

#include "config.h"
#include "scriptreader.h"

using namespace elena_lang;

constexpr auto MAX_LINE = 256;

struct CommandError : ExceptionBase
{

};

static inline const char* trim(const char* s)
{
   while (s[0] == 0x20)s++;

   return s;
}

//static inline bool isLetterOrDigit(char ch)
//{
//   return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= '0' && ch <= '9');
//}

static inline void trimLine(IdentifierString& line)
{
   while (!line.empty() && line[line.length() - 1] == '\r' || line[line.length() - 1] == '\n')
      line[line.length() - 1] = 0;

   while (!line.empty() && line[line.length() - 1] == ' ')
      line[line.length() - 1] = 0;
}

//static inline bool isAssignment(ustr_t line)
//{
//   if (line[0] != '$')
//      return false;
//
//   size_t len = line.length();
//   size_t i = 1;
//   while (i < len && (isLetterOrDigit(line[i])))
//      i++;
//
//   while (line[i] == ' ')
//      i++;
//
//   return (i < len - 1) && (line[i] == ':' && line[i + 1] == '=');
//}
//
//static inline void copyPrefixPostfix(ustr_t s, size_t start, size_t end, TemplateInfo& info)
//{
//   size_t pos = s.findSubStr(start, "$1", end - start);
//   if (pos != NOTFOUND_POS) {
//      info.prefix.copy(s + start, pos - start);
//      info.postfix.copy(s + pos + 2, end - pos - 2);
//   }
//   else info.postfix.copy(s + start, end - start);
//
//   trimLine(info.prefix);
//   trimLine(info.postfix);
//}
//
//static inline size_t findTerminator(ustr_t text, size_t index)
//{
//   bool quoteMode = false;
//   size_t i = index;
//   while (text[i]) {
//      if (text[i]=='"') {
//         if (quoteMode) {
//            if (text[i + 1] != '"') {
//               quoteMode = false;
//            }
//            else i++;
//         }
//         else quoteMode = true;
//      }
//      if (!quoteMode && text[i] == ';')
//         break;
//      i++;
//   }
//
//   return i;
//}
//
//static inline bool insertVariablesAssignment(DynamicString<char>& text, size_t index, ustr_t prefix, ustr_t postfix)
//{
//   size_t i = index;
//   while (i < text.length()) {
//      size_t pos = ustr_t(text.str()).findSub(i, '$');
//      if (pos == NOTFOUND_POS)
//         break;
//
//      if (!isAssignment(text.str() + pos)) {
//         i = pos + 1;
//
//         continue;
//      }
//
//      IdentifierString varName;
//      size_t j = pos + 1;
//      while (isLetterOrDigit(text[j])) {
//         varName.append(text[j]);
//         j++;
//      }
//
//      size_t assignPos = ustr_t(text.str()).findSubStr(pos, ":=", text.length() - pos);
//      size_t endPos = findTerminator(text.str(), assignPos);
//      if (endPos == NOTFOUND_POS) {
//         return false;
//      }
//      IdentifierString expr(text.str() + assignPos + 2, endPos - assignPos - 2);
//
//      text.cut(pos, endPos - pos + 1);
//
//      text.insert(postfix, pos);
//      text.insert(*expr, pos);
//      text.insert("\" ,", pos);
//      text.insert(*varName, pos);
//      text.insert("\"", pos);
//      text.insert(prefix, pos);
//
//      i = pos;
//   }
//
//   return true;
//}
//
//static inline void insertVariables(DynamicString<char>& text, size_t index, ustr_t prefix, ustr_t postfix)
//{
//   size_t i = index;
//   while (i < text.length()) {
//      size_t pos = ustr_t(text.str()).findSub(i, '$');
//      if (pos == NOTFOUND_POS)
//         break;
//
//      IdentifierString varName;
//      size_t j = pos + 1;
//      while (isLetterOrDigit(text[j])) {
//         varName.append(text[j]);
//         j++;
//      }
//
//      text.cut(pos, j - pos);
//
//      text.insert(postfix, pos);
//      text.insert("\"", pos);
//      text.insert(*varName, pos);
//      text.insert("\"", pos);
//      text.insert(prefix, pos);
//
//      i = pos;
//   }
//}

// --- VMSession ---

VMSession :: VMSession(path_t appPath, PresenterBase* presenter)
   : _started(false), _appPath(appPath), _encoding(FileEncoding::UTF8), _env({}), /*_imports(nullptr), */_presenter(presenter), _commands(nullptr), _variables(nullptr), _directives(nullptr)
{
//   _multiLineFlag = false;

   _directives.add("quit", [](VMSession* session, Context* context)
      {
         return session->quit(context);
      });
   _directives.add("list-commands", [](VMSession* session, Context* context)
      {
         if (!context->isEmptyDirective())
            return false;

         session->list(GroupType::Commands);

         return true;
      });
   _directives.add("load", [](VMSession* session, Context* context)
      {
         return session->readScriptTemplate(context);
      });
   _directives.add("eval", [](VMSession* session, Context* context)
      {
         return session->evalScript(context);
      });
}

void VMSession :: setBasePath(path_t baseStr)
{
   _basePath.copy(baseStr);
}

bool VMSession :: loadConfig(path_t configPath)
{
   DynamicString<char> name;
   DynamicString<char> arg;
   DynamicString<char> variable;
   DynamicString<char> script;

   ConfigFile config;
   if (config.load(configPath, FileEncoding::UTF8)) {
      ConfigFile::Collection commands;
      if (config.select(ELT_COMMAND_XPATH, commands)) {
         for (auto it = commands.start(); !it.eof(); ++it) {
            ConfigFile::Node commandNode = *it;

            if (!commandNode.readAttribute("name", name)) {
               name.clear();
            }
            if (!commandNode.readAttribute("arg", arg)) {
               arg.clear();
            }
            if (!commandNode.readAttribute("variable", variable)) {
               variable.clear();
            }

            commandNode.readContent(script);

            if (name.empty()) {
               _presenter->print(ELT_INVALID);

               return true;
            }

            auto command = new Command();
            command->scriptCommand.copy(script.str());
            command->argument.copy(arg.str());
            command->variable.copy(variable.str());

            _commands.add(name.str(), command);
         }
      }

      return true;
   }

   return false;
}

//bool VMSession :: loadTemplate(TemplateType type, ustr_t name)
//{
//   _presenter->print(ELT_LOADING_TEMPLATE, name);
//
//   if (name.find(PATH_SEPARATOR) != NOTFOUND_POS)
//      return false;
//
//   char buffer[1024];
//
//   PathString path(*_appPath);
//   path.combine("scripts");
//   path.combine(name);
//   path.changeExtension("elt");
//
//   TextFileReader reader(*path, FileEncoding::UTF8, false);
//
//   DynamicString<char> helpLine;
//   DynamicString<char> content;
//   if (!reader.isOpen())
//      return false;
//
//   while (reader.read(buffer, 1024)) {
//      if (ustr_t(buffer).startsWith("///")) {
//         helpLine.append(buffer + 3);
//      }
//      else content.append(buffer);
//   }
//
//   if (!helpLine.empty())
//      _presenter->print(helpLine.str());
//
//   switch (type) {
//      case TemplateType::REPL:
//         _repl.clear();
//         copyPrefixPostfix(content.str(), 0, content.length(), _repl);
//         break;
//      case TemplateType::Multiline:
//         _multiline.clear();
//         copyPrefixPostfix(content.str(), 0, content.length(), _multiline);
//         break;
//      case TemplateType::GetVar:
//         _get_var.clear();
//         copyPrefixPostfix(content.str(), 0, content.length(), _get_var);
//         break;
//      case TemplateType::SetVar:
//         _set_var.clear();
//         copyPrefixPostfix(content.str(), 0, content.length(), _set_var);
//         break;
//      default:
//         return false;
//   }
//
//   return true;
//}
//
//void VMSession::printHelp()
//{
//   _presenter->print("@help                      - help\n");
//   _presenter->print("@quit                      - quit\n");
//   _presenter->print("@multiline                 - switching to a multi-line mode\n");
//
//   _presenter->print("<expr>                     - evaluate the expression and print the result\n");
//   _presenter->print("$<var> := <expr>;          - assign a global variable\n");
//   _presenter->print(".. $<var>  ..              - get a global variable value\n");
//
//   _presenter->print("@base <path>               - set the base path for scripts\n");
//   _presenter->print("@load <path>               - execute a script from file\n");
//   _presenter->print("@import <path>             - load the script into multi-line script\n");
//
//   _presenter->print("@use <template>            - use the template for multiline script\n");
//
//   _presenter->print("@eval                      - executing the multi-line code and switch back to REPL mode\n");
//   _presenter->print("@clear                     - clear the multi-line code and switch back to REPL mode\n");
//   _presenter->print("@print                     - print the multi-line code\n");
//   _presenter->print("@add import <reference>    - importing a module into the session\n");
//   _presenter->print("@remove import <reference> - removing a module from the session\n");
//}

//bool VMSession::importScript(ustr_t scriptName)
//{
//   PathString totalPath(_basePath);
//   totalPath.combine(scriptName);
//   totalPath.changeExtension("es");
//
//   TextFileReader reader(*totalPath, _encoding, false);
//   if (!reader.isOpen())
//      return false;
//
//   char buffer[1024];
//   while (reader.read(buffer, 1024)) {
//      _body.append(buffer);
//   }
//
//   return true;
//}

static inline ustr_t readCommand(ustr_t script, IdentifierString& command)
{
   size_t pos = script.find(' ');
   if (pos != NOTFOUND_POS) {
      command.copy(script, pos);

      while (command[pos + 1] == ' ')
         pos++;

      return script + pos + 1;
   }
   else {
      command.copy(script);

      return nullptr;
   }
}

inline static void raiseSyntaxError(PresenterBase* presenter, ustr_t commandStr)
{
   presenter->print(ELT_INVALID, commandStr);

   throw CommandError();
}

inline static void raiseCommandError(PresenterBase* presenter, ustr_t commandStr)
{
   presenter->print(ELT_UNKNOWNCOMMAND, commandStr);

   throw CommandError();
}

inline static void raiseUnknownError(PresenterBase* presenter, path_t path)
{
   presenter->printPath(ELT_CANNOT_LOAD_TEMPLATE, path);

   throw CommandError();
}

inline static void readToken(PresenterBase* presenter, ScriptReader& scriptReader, ScriptToken& token, ustr_t expected, bool skipRead = false)
{
   if (!skipRead)
      scriptReader.read(token);

   if (!token.compare(expected)) {
      raiseSyntaxError(presenter, token.token.str());
   }

   if (skipRead)
      scriptReader.read(token);
}

void VMSession :: setVariable(ustr_t name, ustr_t value)
{
   _variables.erase(name);
   _variables.add(name, value.clone());
}

void VMSession :: listCommands()
{
   for (auto it = _commands.start(); !it.eof(); ++it) {
      _presenter->printLine(it.key());
   }
}

void VMSession :: list(GroupType type)
{
   switch (type) {
      case GroupType::Commands:
         listCommands();
         break;
      default:
         break;
   }
}

bool VMSession :: loadScript(ustr_t pathStr)
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

size_t seekEnd(const char* str, size_t index)
{
   while (isValidLetter(str[index]) || isValidDigit(str[index]))
      index++;

   return index;
}

bool VMSession :: readScriptTemplate(path_t pathStr, ustr_t targetVariable)
{
   if (pathStr.find(PATH_SEPARATOR) == NOTFOUND_POS) {
      PathString fullPath(_basePath);
      fullPath.combine(pathStr);
      fullPath.changeExtension("elt");

      TextFileReader reader(*fullPath, FileEncoding::UTF8, false);
      char buffer[1024];
      DynamicString<char> content;
      if (!reader.isOpen())
         return false;
      
      while (reader.read(buffer, 1024)) {
         content.append(buffer);
      }

      size_t start = 0;
      size_t pos = ustr_t(content.str()).find('@');
      while (pos != NOTFOUND_POS) {
         if (content[pos + 1] == '@') {
            content.cut(pos, 1);

            start = pos + 1;
         }
         else {
            size_t pos_end = seekEnd(content.str(), pos + 1);

            IdentifierString variableName(content.str() + pos + 1, pos_end - pos - 1);
            content.cut(pos, pos_end - pos);

            ustr_t varValue = _variables.get(*variableName);
            if (!varValue.empty()) {
               content.insert(varValue.str(), pos);

               start = pos + varValue.length();
            }
            else start = pos;
         }

         pos = ustr_t(content.str()).findSub(start, '@');
      }
   
      if (!targetVariable.empty())
         setVariable(targetVariable, content.str());

      return true;
   }

   return false;
}

bool VMSession :: readScriptTemplate(Context* context)
{
   if (context->directiveArg1.empty() || context->isDirectiveArg1Variable)
      return false;

   PathString scriptPath(*context->directiveArg1);

   if (!context->directiveArg2.empty() && !context->isDirectiveArg2Variable)
      return false;

   return readScriptTemplate(*scriptPath, *context->directiveArg2);
}

bool VMSession :: evalScript(Context* context)
{
   if (context->directiveArg1.empty() || !context->isDirectiveArg1Variable)
      return false;

   DynamicString<char> content;
   content.append(_variables.get(*context->directiveArg1));

   if (!context->directiveArg2.empty()) {
      if (!context->isDirectiveArg2Variable)
         return false;

      content.append("\n");
      content.append(_variables.get(*context->directiveArg2));
   }
    
   if(!executeScript(content.str()))
      _presenter->print(ELT_CODE_FAILED);

   return true;
}

bool VMSession :: quit(Context* context)
{
   context->running = false;

   return true;
}

void VMSession :: executeCommand(Command* command, Context& context)
{
   IdentifierTextReader reader(*command->scriptCommand);
   ScriptReader scriptReader(3, &reader);

   ScriptToken token;
   while (scriptReader.read(token)) {
      context.clearDirectiveArgs();

      readToken(_presenter, scriptReader, token, "^", true);

      IdentifierString commandName;
      commandName.copy(token.token.str());

      CommandInvoker invoker = _directives.get(*commandName);

      if (invoker == nullptr) {
         if (scriptReader.read(token) && !token.compare(";")) {
            commandName.append("-");
            commandName.append(token.token.str());

            invoker = _directives.get(*commandName);
         }
      }

      if (invoker == nullptr)
         raiseCommandError(_presenter, *commandName);

      bool invalid = true;
      while (scriptReader.read(token)) {
         if (token.compare(";")) {
            invalid = false;

            break;
         }

         bool variable = false;
         if (token.compare("@")) {
            variable = true;
            if (!scriptReader.read(token))
               break;
         }

         if (context.directiveArg1.empty()) {
            context.isDirectiveArg1Variable = variable;
            context.directiveArg1.copy(token.token.str());
         }
         else if (context.directiveArg2.empty()) {
            context.isDirectiveArg2Variable = variable;
            context.directiveArg2.copy(token.token.str());
         }
         else break;
      }

      if (invalid || invoker == nullptr)
         raiseSyntaxError(_presenter, *command->scriptCommand);

      if (!invoker(this, &context))
         raiseSyntaxError(_presenter, *command->scriptCommand);
   }
}

void VMSession :: executeCommandLine(/*bool preview, TemplateType type, */ustr_t script, Context& context)
{
   IdentifierString commandName;

   context.variableArg.clear();
   context.commandLineArgument = readCommand(script, commandName);

   Command* command = _commands.get(*commandName);
   if (command != nullptr) {
      if (!command->variable.empty()) {
         if (context.commandLineArgument[0] != '@') {
            _presenter->print(ELT_INVALID, *commandName);

            return;
         }
         context.commandLineArgument = readCommand(context.commandLineArgument + 1, context.variableArg);
         if (!context.commandLineArgument.startsWith(":=")) {
            _presenter->print(ELT_INVALID, *commandName);

            return;
         }

         setVariable(command->variable.str(), *context.variableArg);

         context.commandLineArgument = trim(context.commandLineArgument + 2);
      }

      if (!command->argument.empty()) {
         setVariable(command->argument.str(), context.commandLineArgument);
      }

      executeCommand(command, context);
   }
   else _presenter->print(ELT_UNKNOWNCOMMAND, *commandName);

//   DynamicString<char> command;
//
//   for (auto it = _imports.start(); !it.eof(); ++it) {
//      command.append("import ");
//      command.append(*it);
//      command.append(";\n");
//   }
//
//   switch (type) {
//      case TemplateType::REPL:
//         command.append(*_repl.prefix);
//         command.append(script);
//         command.append(*_repl.postfix);
//         break;
//      case TemplateType::Multiline:
//         command.append(*_multiline.prefix);
//         command.append(script);
//         command.append(*_multiline.postfix);
//         break;
//      default:
//         break;
//   }
//
//   insertVariablesAssignment(command, 0, *_set_var.prefix, *_set_var.postfix);
//   insertVariables(command, 0, *_get_var.prefix, *_get_var.postfix);
//
//   if (preview) {
//      _presenter->printLine(command.str());
//   }
//   else if (!executeScript(command.str())) {
//      _presenter->print(ELT_CODE_FAILED);
//   }
}

bool VMSession :: executeTape(void* tape)
{
   bool retVal = false;
   if (!_started) {
      retVal = connect(tape);
   }
   else retVal = execute(tape);

   ReleaseSMLA(0);

   return retVal;
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

//bool VMSession :: executeCommand(const char* line, bool& running)
//{
//   size_t len = getlength(line);
//   if (len < 2)
//      return false;
//
//   // check commands
//   if (line[1] == 'q' || (len > 2 && ustr_t(line).compare("@quit"))) {
//      running = false;
//   }
//   else if (line[1] == 'h' || (len > 2 && ustr_t(line).compare("@help"))) {
//      printHelp();
//   }
//   else if (ustr_t(line).compare("@multiline")) {
//      _multiLineFlag = true;
//   }
//   else if (ustr_t(line).startsWith("@base ")) {
//      IdentifierString basePath(line + 6);
//
//      setBasePath(*basePath);
//   }
//   else if (line[1] == 'l') {
//      if (line[2] == ' ') {
//         loadScript(line + 2);
//      }
//      else if (ustr_t(line).startsWith("@load ")) {
//         loadScript(line + 6);
//      }
//   }
//   else if (ustr_t(line).startsWith("@use ")) {
//      IdentifierString pluginName(line + 5);
//
//      if(!loadTemplate(TemplateType::Multiline, *pluginName))
//         _presenter->printLine(ELT_CANNOT_LOAD_TEMPLATE, *pluginName);
//   }
//   else if (ustr_t(line).startsWith("@import ")) {
//      IdentifierString scriptPath(line + 8);
//
//      importScript(*scriptPath);
//   }
//   else if (ustr_t(line).compare("@eval")) {
//      executeCommandLine(false, TemplateType::Multiline, _body.str());
//
//      _multiLineFlag = false;
//      _body.clear();
//   }
//   else if (ustr_t(line).compare("@print")) {
//      executeCommandLine(true, TemplateType::Multiline, _body.str());
//   }
//   else if (ustr_t(line).compare("@clear")) {
//      _multiLineFlag = false;
//      _body.clear();
//   }
//   else if (line[1] == 'a' && ustr_t(line).startsWith("@add import ")) {
//      IdentifierString module(line + 12);
//
//      _imports.add((*module).clone());
//   }
//   else if (line[1] == 'r' && ustr_t(line).startsWith("@remove import ")) {
//      IdentifierString module(line + 15);
//
//      _imports.cut(*module);
//   }
//   else return false;
//
//   return true;
//}

void VMSession :: run()
{
   char          buffer[MAX_LINE];

   Context       context = { true };

   do {
      try {
//         if (!_multiLineFlag)
            _presenter->print("\n>");

         _presenter->readLine(buffer, MAX_LINE);

         IdentifierString line(buffer, getlength(buffer));
         trimLine(line);

//         if (line[0] == '@') {
//            if (!executeCommand(*line, running))
//               _presenter->print("Invalid command, use -h to get the list of the commands\n");
//         }
//         else if (_multiLineFlag) {
//            _body.append(*line);
//            _body.append("\n");
//         }
//         else if (isAssignment(*line)) {
//            executeCommandLine(false, TemplateType::Multiline, *line);
//         }
         /*else */executeCommandLine(/*false, TemplateType::REPL, */*line, context);
      }
      catch (...) {
         _presenter->print("Invalid operation");
      }
   } while (context.running);
}
