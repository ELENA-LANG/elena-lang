//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line DSA script terminal main file
//                                              (C)2011-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include <stdarg.h>
#include "elenamachine.h"
#include "elenasm.h"
#include "elenavm.h"

using namespace _ELENA_;

#define MAX_LINE           256
#define MAX_SCRIPT         4096
#define ELT_BUILD_NUMBER   15
 
// global variables
ProgramHeader  _header;
int            _encoding = feAnsi;
pos_t          _tlsIndex = 0;
GCTable        _table = { 0 };
SystemEnv      _env = { 0 };
int            _sehTable = 0;

IdentifierString prefix;
IdentifierString postfix;

DynamicString<char> body;

void loadTemplate(path_t path)
{
   char buff[512];

   TextFileReader reader(path, feUTF8, false);

   if (reader.isOpened()) {
      IdentifierString content;
      reader.readAll(content, buff);

      int index = content.ident().find("$1");
      if (index != NOTFOUND_POS) {
         prefix.copy(content.ident(), index);
         postfix.copy(content.c_str() + index + 2);
      }
      else prefix.copy(content.ident());
   }
}

void print(const char* str, ...)
{
   va_list argptr;
   va_start(argptr, str);

   vprintf(str, argptr);
   va_end(argptr);

   fflush(stdout);
}

const char* trim(const char* s)
{
   while(s[0]==0x20)s++;

   return s;
}

void printHelp()
{
   print("-q                   - quit\n");
   print("-h                   - help\n");
   print("-l <path>            - execute a script from file\n");
   print("<script>             - execute script\n");
}

void executeTape(void* tape)
{
   int retVal = EvaluateTape(&_env, &_sehTable, tape);
   Release(tape);

   // copy vm error if retVal is zero
   if (!retVal) {
      ident_t error = GetVMLastError();
      if (!emptystr(error)) {
         printf("\nFailed:%s", (const char*)error);
      }
   }
}

void executeScript(const char* script)
{
   void* tape = InterpretScript(script);
   if (tape == NULL) {
      char error[0x200];
      int length = GetStatus(error, 0x200);
      error[length] = 0;
      if (!emptystr(error)) {
         printf("\nFailed:%s", error);
      }
      return;
   }
   else executeTape(tape);
}

void loadScript(const char* path)
{
   path = trim(path);

   void* tape = InterpretFile(path, _encoding, false);
   if (tape == NULL) {
      char error[0x200];
      int length = GetStatus(error, 0x200);
      error[length] = 0;
      if (!emptystr(error)) {
         printf("\nFailed:%s", error);
      }
      return;
   }
   else executeTape(tape);

}

bool executeCommand(const char* line, bool& running)
{
   if (getlength(line)<2)
      return false;

   // check commands
   if(line[1] == 'q') {
      running = false;
   }
   else if(line[1] == 'h') {
      printHelp();
   }
   else if(line[1] == 'l') {
      loadScript(line + 2);
   }
   else if (line[1] == 'c') {
      body.clear();
   }
   else return false;

   return true;
}

bool executeCommand(const char* line)
{
   bool dummy;
   return executeCommand(line, dummy);
}

void executeCommandLine(const char* line)
{
   DynamicString<char> command;

   command.append(body.str());
   command.append('\n');
   command.append(prefix);
   command.append(line);
   command.append(postfix);

   executeScript(command);
}

void runSession()
{
   char          buffer[MAX_LINE];
   IdentifierString line;
   bool             running = true;

   do {
      try {
         print("\n>");

         fgets(buffer, MAX_LINE, stdin);
         IdentifierString line(buffer, getlength(buffer));

         while (!emptystr(line) && line[getlength(line) - 1]=='\r' || line[getlength(line) - 1]=='\n')
            line[getlength(line) - 1] = 0;

         while (!emptystr(line) && line[getlength(line) - 1]==' ')
            line[getlength(line) - 1] = 0;

         if (line[0]=='-') {
            if(!executeCommand(line, running))
               print("Invalid command, use -h to get the list of the commands\n");
         }
         else if (!emptystr(line) && line[getlength(line) - 1]==';') {
            line[getlength(line) - 1] = 0;

            body.append(line.c_str());
         }
         else executeCommandLine(line);
      }
      catch(...) {
         print("Invalid operation");
      }
   }
   while(running);
}

int main(int argc, char* argv[])
{
   print("ELENA command line VM terminal %d.%d.%d (C)2011-2020 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELT_BUILD_NUMBER);

   Path commandPath("command.es");
   loadTemplate(commandPath.c_str());

   _env.MaxThread = 1;
   _env.Table = &_table;
   _env.TLSIndex = &_tlsIndex;
   _env.GCMGSize = 0x54000;
   _env.GCYGSize = 0x15000;

   InitializeVMSTA(&_sehTable, &_env, nullptr, nullptr, nullptr, &_header);

   loadScript("~\\elt.es");
   loadScript("~\\scripts\\grammar.es");
   loadScript("~\\scripts\\tscript.es");

   // load script passed via command line arguments
   if (argc > 1) {
      for (int i = 1 ; i < argc ; i++) {
         if (argv[i][0] == '-') {
            // check exit command
            if (argv[i][1] == 'q')
               return 0;

            executeCommand(argv[i]);
         }
         else executeScript(argv[i]);
      }
   }

   runSession();
}
