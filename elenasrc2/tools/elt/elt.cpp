//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line DSA script terminal main file
//                                              (C)2011-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
////#include "config.h"
#include "elenasm.h"

using namespace _ELENA_;

#define MAX_LINE           256
#define MAX_SCRIPT         4096
#define ELT_BUILD_NUMBER   5
 
// global variables
int   _encoding = feAnsi;

////// --- commands ---
////
////void printMessage(int messageId, bool withRole)
////{
////   const wchar16_t* message = retrieveKey(_verbs.start(), messageId, (const wchar_t*)NULL);
////
////   if (withRole) {
////      wprintf(_T("@send role.%s\n"), message);
////   }
////   else wprintf(_T("@send %s\n"), message);
////}
////
//////void printMerge(const wchar16_t* vmt)
//////{
//////   if (ConstIdentifier::compare(vmt, GROUP_CLASS)) {
//////      wprintf(_T("@group-merge\n"));
//////   }
//////   else {
//////      wprintf(_T("@merge\n"));
//////   }
//////}
//
////void printTape(void* tape)
////{
////   wprintf((const wchar16_t*)tape);
////}
//
////void split(const TCHAR* p1, const TCHAR* &p2, int& p1_len)
////{
////   p1_len = 0;
////
////   while(p1[p1_len] != _T(' ') && p1_len < getlength(p1))
////      p1_len++;
////
////   // argument
////   p2 = p1 + p1_len;
////
////   while (p2[0]==' ')
////      p2++;
////}
//
//const wchar16_t* trim(const wchar16_t* s)
//{
//   while(s[0]==0x20)s++;
//
//   return s;
//}

void printHelp()
{
   printf("-q                   - quit\n");
//   printf("-h                   - help\n");
////   printf("-ton                 - trace mode is on\n");
////   printf("-toff                - trace mode is off\n");
//   printf("-l [name=]<path>     - execute a script from file\n");
////   printf("-lt [name=]<path>      - disassemble a script from file\n");
////   printf("-lc [name=]<path>      - generate CF parser from file\n");
////   printf("-li <path>             - load an inline script from file\n");
//   printf("<script>             - execute script\n");
}

void executeScript(const wchar16_t* script)
{
   int retVal = TranslateLVMTape(script);
   if (retVal == 0) {
      const wchar16_t* error = GetLSMStatus();
      if (!emptystr(error)) {
         wprintf(_T("\nFailed:%s"), error);
      }
      return;
   }
}

void loadScript(const wchar16_t* path)
{
   int retVal = TranslateLVMFile(path, _encoding, false);
   if (retVal == 0) {
      const wchar16_t* error = GetLSMStatus();
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
//   else if(line[0] == 'h') {
//      printHelp();
//   }
//   else if(line[0] == 'l') {
//      loadScript(line + 1);
//   }
////   else if(ConstantIdentifier::compare(line, "ton")) {
////      _tracing = true;
////   }
////   else if(ConstantIdentifier::compare(line, "toff")) {
////      _tracing = false;
////   }
////   else if (line[0]=='n') {
////      _grammarName.copy(line + 1);
////   }
   else return false;

   return true;
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
               printHelp();
         }
         else if (!emptystr(line) && line[getlength(line) - 1]!=','){
            executeScript(line);
         }
//         else printHelp();
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

//   // load script passed via command line arguments
//   if (argc > 1) {
//      for (int i = 1 ; i < argc ; i++) {
//         if (argv[i][0] == '-') {
//            // check exit command
//            if (argv[i][1] == 'q')
//               return 0;
//
//            String<wchar_t, 260> param;
//            param.copy(argv[i]);
//            // if the parameter is followed by argument
//            if (i + 1 < argc && argv[i+1][0] != '-') {
//               param.append(argv[i + 1]);
//            }
//
//            executeCommand(param + 1);
//         }
//      }
//   }

   loadScript(ConstantIdentifier("scripts\\elt.es"));

   runSession();
}
