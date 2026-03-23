//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM session declaration
//
//                                             (C)2023-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELTVMSESSION_H
#define ELTVMSESSION_H

#include "core.h"

namespace elena_lang
{
//   enum class TemplateType
//   {
//      REPL,
//      Multiline,
//      GetVar,
//      SetVar
//   };
//
//   struct TemplateInfo
//   {
//      IdentifierString     prefix;
//      IdentifierString     postfix;
//
//      void clear()
//      {
//         prefix.clear();
//         postfix.clear();
//      }
//   };

   class VMSession
   {
      enum GroupType
      {
         Node = 0,
         Commands
      };

      struct Command
      {
         IdentifierString scriptCommand;
         IdentifierString variable;
         IdentifierString argument;
      };

      struct Context
      {
         bool              running;
         ustr_t            commandLineArgument;

         bool              isDirectiveArg1Variable;
         bool              isDirectiveArg2Variable;

         IdentifierString  variableArg;
         IdentifierString  directiveArg1;
         IdentifierString  directiveArg2;

         void clearDirectiveArgs()
         {
            directiveArg1.clear();
            directiveArg2.clear();

            isDirectiveArg1Variable = false;
            isDirectiveArg2Variable = false;
         }

         bool isEmptyDirective()
         {
            return directiveArg1.empty() && directiveArg2.empty();
         }
      };

      typedef bool(*CommandInvoker)(VMSession*, Context*);

      typedef MemoryMap<ustr_t, Command*, Map_StoreUStrAligned4, Map_GetUStr, freeobj>    LocalMap;
      typedef Map<ustr_t, ustr_t, allocUStr, freeUStr, freeUStr>                          VariableMap;
      typedef Map<ustr_t, CommandInvoker, allocUStr, freeUStr>                            DirectiveMap;

      bool                 _started;
//      bool                 _multiLineFlag;

      PathString           _appPath;
      PathString           _basePath;

      FileEncoding         _encoding;

      PresenterBase*       _presenter;

      DirectiveMap         _directives;
      LocalMap             _commands;
      VariableMap          _variables;

//      TemplateInfo         _repl;
//      TemplateInfo         _multiline;
//      TemplateInfo         _get_var;
//      TemplateInfo         _set_var;
//
//      DynamicString<char>  _body;
//
//      IdentifierList       _imports;

      SystemEnv            _env;

      bool connect(void* tape);
      bool execute(void* tape);

      void setVariable(ustr_t name, ustr_t value);

      bool readScriptTemplate(path_t scriptPath, ustr_t targetVariable);

      void listCommands();
      void list(GroupType type);

      void executeCommand(Command* command, Context& context);
      void executeCommandLine(/*bool preview, TemplateType type, */ustr_t script, Context& context);

      bool executeTape(void* tape);

//      bool importScript(ustr_t name);

   public:
      // directives
      bool quit(Context* context);
      bool readScriptTemplate(Context* context);
      bool evalScript(Context* context);

//      void printHelp();
//
//      bool loadTemplate(TemplateType type, ustr_t name);

      bool executeScript(const char* line);
//      bool executeCommand(const char* line, bool& running);

      bool loadScript(ustr_t pathStr);

      void start();

      void setBasePath(path_t basePath);

      bool loadConfig(path_t path);

      void run();

      VMSession(path_t appPath, PresenterBase* presenter);
   };
}

#endif
