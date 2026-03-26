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

static inline void replaceAll(DynamicString<char>& content, ustr_t oriValue, ustr_t newValue)
{
   size_t pos = ustr_t(content.str()).findStr(oriValue);
   while (pos != NOTFOUND_POS) {
      content.cut(pos, oriValue.length());
      content.insert(newValue.str(), pos);

      pos = ustr_t(content.str()).findSubStr(pos + newValue.length(), oriValue, content.length());
   }
}

static inline void replaceAll(DynamicString<char>& content, char oriValue, char newValue)
{
   for (size_t i = 0; i < content.length(); i++) {
      if (content[i] == oriValue)
         content[i] = newValue;
   }
}

//static inline bool isLetterOrDigit(char ch)
//{
//   return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= '0' && ch <= '9');
//}

//static inline void trimLine(IdentifierString& line)
//{
//   while (!line.empty() && line[line.length() - 1] == '\r' || line[line.length() - 1] == '\n')
//      line[line.length() - 1] = 0;
//
//   while (!line.empty() && line[line.length() - 1] == ' ')
//      line[line.length() - 1] = 0;
//}

static inline void trimLine(char* line)
{
   while (line[0] != 0 && (line[getlength(line) - 1] == '\r' || line[getlength(line) - 1] == '\n'))
      line[getlength(line) - 1] = 0;

   while (line[0] != 0 && line[getlength(line) - 1] == ' ')
      line[getlength(line) - 1] = 0;
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
   _directives.add("file-eval", [](VMSession* session, Context* context)
      {
         return session->evalScriptPath(context);
      });
   _directives.add("set", [](VMSession* session, Context* context)
      {
         return session->assignVariable(context);
      });
   _directives.add("copy", [](VMSession* session, Context* context)
      {
         return session->copyVariable(context);
      });
   _directives.add("ask", [](VMSession* session, Context* context)
      {
         return session->inputVariable(context);
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
      ConfigFile::Collection paths;
      if (config.select(ELT_PATH_XPATH, paths)) {
         for (auto it = paths.start(); !it.eof(); ++it) {
            ConfigFile::Node pathNode = *it;

            if (!pathNode.readAttribute("name", name)) {
               name.clear();
            }

            if (ustr_t(name.str()).compare("lscripts")) {
               pathNode.readContent(script);

               replaceAll(script, '/', PATH_SEPARATOR);

               _scriptsPath.copy(*_appPath);
               _scriptsPath.combine(script.str());
            }
         }
      }

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

      bool replacing = true;
      // NOTE : check the script several times to support nested variables
      while (replacing) {
         replacing = false;
         size_t start = 0;
         size_t pos = ustr_t(content.str()).find('@');
         while (pos != NOTFOUND_POS) {
            if (content[pos + 1] == '@') {
               start = pos + 2;
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

               replacing = true;
            }

            pos = ustr_t(content.str()).findSub(start, '@');
         }
      }
   
      replaceAll(content, "@@", "@");

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

bool VMSession :: evalScriptPath(Context* context)
{
   if (context->directiveArg1.empty() || !context->isDirectiveArg1Variable)
      return false;

   ustr_t path = _variables.get(*context->directiveArg1);

   bool retVal = false;
   if (path.startsWith("~/") || path.startsWith("~\\")) {
      IdentifierString fullPath(*_scriptsPath);

      fullPath.append(path.str() + 1);

      retVal = executeScriptFile(*fullPath);
   }
   else retVal = executeScriptFile(path);

   if (!retVal)
      _presenter->print(ELT_CODE_FAILED);

   return true;
}

bool VMSession :: quit(Context* context)
{
   context->running = false;

   return true;
}

bool VMSession :: assignVariable(Context* context)
{
   if (context->directiveArg1.empty() || !context->isDirectiveArg1Variable)
      return false;

   if (context->directiveArg2.empty() || !context->isDirectiveArg2Variable)
      return false;

   ustr_t var_name = _variables.get(*context->directiveArg1);
   ustr_t value = _variables.get(*context->directiveArg2);

   setVariable(var_name, value);

   return true;
}

bool VMSession :: copyVariable(Context* context)
{
   if (context->directiveArg1.empty() || !context->isDirectiveArg1Variable)
      return false;

   if (context->directiveArg2.empty() || !context->isDirectiveArg2Variable)
      return false;

   ustr_t var_name = *context->directiveArg1;
   ustr_t value = _variables.get(*context->directiveArg2);

   setVariable(var_name, value);

   return true;
}

bool VMSession :: inputVariable(Context* context)
{
   if (context->directiveArg1.empty() || !context->isDirectiveArg1Variable)
      return false;

   IdentifierString caption("Please enter ");
   if (!context->directiveArg2.empty()) {
      caption.append(*context->directiveArg2);
   }
   else caption.append(*context->directiveArg1);

   ustr_t var_name = *context->directiveArg1;

   ustr_t prev_value = _variables.get(*context->directiveArg1);
   if (!prev_value.empty()) {
      caption.append("[");
      if (prev_value.length() > 20) {
         caption.append(prev_value, 20);
         caption.append("..");
      }
      else caption.append(prev_value);

      caption.append("]:");
   }
   else caption.append(":");

   _presenter->print(*caption);

   char buffer[1024];
   _presenter->readLine(buffer, 1024);
   trimLine(buffer);

   if (ustr_t(buffer).startsWith("~/") || ustr_t(buffer).startsWith("~\\")) {
      IdentifierString arg(*_scriptsPath);

      arg.append(buffer + 1);

      setVariable(var_name, *arg);
   }
   else if (getlength(buffer) != 0)
      setVariable(var_name, buffer);

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

void VMSession :: executeCommandLine(ustr_t script, Context& context)
{
   IdentifierString commandName;

   context.variableArg.clear();
   context.commandLineArgument = readCommand(script, commandName);

   Command* command = _commands.get(*commandName);
   if (command == nullptr) {
      IdentifierString secondName;
      context.commandLineArgument = readCommand(context.commandLineArgument, secondName);

      commandName.append('-');
      commandName.append(*secondName);

      command = _commands.get(*commandName);
   }

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

bool VMSession :: executeScriptFile(const char* path)
{
   void* tape = InterpretFileSMLA(path, (int)_encoding, false);
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

void VMSession :: run()
{
   char          buffer[MAX_LINE];

   Context       context = { true };

   do {
      try {
//         if (!_multiLineFlag)
            _presenter->print("\n>");

         _presenter->readLine(buffer, MAX_LINE);

         trimLine(buffer);

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
         /*else */executeCommandLine(/*false, TemplateType::REPL, */buffer, context);
      }
      catch (...) {
         _presenter->print("Invalid operation");
      }
   } while (context.running);
}
