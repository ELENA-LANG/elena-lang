//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line DSA script terminal main file
//                                              (C)2011-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include <stdarg.h>
#include "elenasm.h"
#include "elenavm.h"

using namespace _ELENA_;

#define MAX_LINE           256
#define MAX_SCRIPT         4096
#define ELT_BUILD_NUMBER   4
 
// global variables
int   _encoding = feAnsi;
bool _loaded = false;

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
   int retVal = InterpretTape(tape);
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
      _loaded = true;
      loadScript(line + 2);
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
   IdentifierString command(line);
   for(int i = 0; i < getlength(command) ; i++) {
      if (command[i] == '^')
         command[i] = '\"';
   }

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
         else if (!emptystr(line) && line[getlength(line) - 1]!=','){
            executeScript(line);
         }
      }
      catch(...) {
         print("Invalid operation");
      }
   }
   while(running);
}

int main(int argc, char* argv[])
{
   print("ELENA command line VM terminal %d.%d.%d (C)2011-2017 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELT_BUILD_NUMBER);
   
   executeScript("[[ #config vm_console #start; ]]");

   // load script passed via command line arguments
   if (argc > 1) {
      for (int i = 1 ; i < argc ; i++) {
         if (argv[i][0] == '-') {
            // check exit command
            if (argv[i][1] == 'q')
               return 0;

            executeCommand(argv[i]);
         }
         else executeCommandLine(argv[i]);
      }
   }

   if (!_loaded)
      loadScript("scripts\\elena.es");

   runSession();
}
