//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line DSA script terminal main file
//                                              (C)2011-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
////#include "config.h"
#include "elenasm.h"

using namespace _ELENA_;

#define MAX_LINE           256
#define MAX_SCRIPT         4096
#define ELT_BUILD_NUMBER   1
 
// global variables
int   _encoding = feAnsi;
bool _loaded = false;

const char* trim(const char* s)
{
   while(s[0]==0x20)s++;

   return s;
}

void printHelp()
{
   printf("-q                   - quit\n");
   printf("-h                   - help\n");
   printf("-l <path>            - execute a script from file\n");
   printf("<script>             - execute script\n");
}

void executeScript(const char* script)
{
   int retVal = InterpretScript(script);
   if (retVal == 0) {
      char error[0x200];
      int length = GetStatus(error, 0x200);
      error[length] = 0;
      if (!emptystr(error)) {
         _ELENA_::WideString message(error);

         wprintf(L"\nFailed:%s", (const wchar_t*)message);
      }
      return;
   }
}

void loadScript(const char* path)
{
   path = trim(path);

   int retVal = InterpretFile(path, _encoding, false);
   if (retVal == 0) {
      char error[0x200];
      int length = GetStatus(error, 0x200);
      error[length] = 0;
      if (!emptystr(error)) {
         _ELENA_::WideString message(error);

         wprintf(L"\nFailed:%s", (const wchar_t*)message);
      }
      return;
   }

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

void runSession()
{
   wchar_t          buffer[MAX_LINE];
   IdentifierString line;
   bool             running = true;

   do {
      try {
         printf("\n>");

         fgetws(buffer, MAX_LINE, stdin);
         IdentifierString line(buffer, getlength(buffer));

         while (!emptystr(line) && line[getlength(line) - 1]=='\r' || line[getlength(line) - 1]=='\n')
            line[getlength(line) - 1] = 0;

         while (!emptystr(line) && line[getlength(line) - 1]==' ')
            line[getlength(line) - 1] = 0;

         if (line[0]=='-') {
            if(!executeCommand(line, running))
               printf("Invalid command, use -h to get the list of the commands\n");
         }
         else if (!emptystr(line) && line[getlength(line) - 1]!=','){
            executeScript(line);
         }
      }
      catch(...) {
         printf("Invalid operation");
      }
   }
   while(running);
}

int main(int argc, char* argv[])
{
   printf("ELENA command line VM terminal %d.%d.%d (C)2011-2015 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELT_BUILD_NUMBER);

   
   // load script passed via command line arguments
   if (argc > 1) {
      for (int i = 1 ; i < argc ; i++) {
         if (argv[i][0] == '-') {
            // check exit command
            if (argv[i][1] == 'q')
               return 0;

            executeCommand(argv[i]);
         }
      }
   }

   if (!_loaded)
      loadScript("scripts\\elena.es");
      
   runSession();
}
