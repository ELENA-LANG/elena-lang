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
#define ELT_BUILD_NUMBER   16
 
// global variables
int   _encoding = feAnsi;

const wchar16_t* trim(const wchar16_t* s)
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

void executeScript(const wchar16_t* script)
{
   int retVal = InterpretScript(script);
   if (retVal == 0) {
      wchar16_t error[0x200];
      int length = GetStatus(error, 0x200);
      error[length] = 0;
      if (!emptystr(error)) {
         wprintf(_T("\nFailed:%s"), error);
      }
      return;
   }
}

void loadScript(const wchar16_t* path)
{
   path = trim(path);

   int retVal = InterpretFile(path, _encoding, false);
   if (retVal == 0) {
      wchar16_t error[0x200];
      int length = GetStatus(error, 0x200);
      error[length] = 0;
      if (!emptystr(error)) {
         wprintf(_T("\nFailed:%s"), error);
      }
      return;
   }

}

bool executeCommand(const wchar16_t* line, bool& running)
{
   if (emptystr(line))
      return false;

   // check commands
   if(line[0] == 'q') {
      running = false;
   }
   else if(line[0] == 'h') {
      printHelp();
   }
   else if(line[0] == 'l') {
      loadScript(line + 1);
   }
   else return false;

   return true;
}

bool executeCommand(const wchar16_t* line)
{
   bool dummy;
   return executeCommand(line, dummy);
}

void runSession()
{
   char                        buffer[MAX_LINE];
   String<wchar_t, MAX_SCRIPT> line;
   bool running = true;

   do {
      try {
         printf("\n>");

         // !! fgets is used instead of fgetws, because there is strange bug in fgetws implementation
         fgets(buffer, MAX_LINE, stdin);
         line.copy(buffer, strlen(buffer));

         while (!emptystr(line) && line[getlength(line) - 1]=='\r' || line[getlength(line) - 1]=='\n')
            line[getlength(line) - 1] = 0;

         while (!emptystr(line) && line[getlength(line) - 1]==' ')
            line[getlength(line) - 1] = 0;

         if (line[0]=='-') {
            if(!executeCommand(line + 1, running))
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
   printf("ELENA command line VM terminal %d.%d.%d (C)2011-2014 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELT_BUILD_NUMBER);

   loadScript(ConstantIdentifier("scripts\\elt.es"));

   // load script passed via command line arguments
   if (argc > 1) {
      for (int i = 1 ; i < argc ; i++) {
         if (argv[i][0] == '-') {
            // check exit command
            if (argv[i][1] == 'q')
               return 0;

            String<wchar_t, 260> param;
            param.copy(argv[i]);
            // if the parameter is followed by argument
            if (i + 1 < argc && argv[i+1][0] != '-') {
               param.append(argv[i + 1]);
            }

            executeCommand(param + 1);
         }
      }
   }

   runSession();
}
