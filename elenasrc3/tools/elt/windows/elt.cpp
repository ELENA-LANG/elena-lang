//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM terminal
//
//                                              (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
#include "eltconst.h"

using namespace elena_lang;

int main()
{
   printf(ELT_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELT_REVISION_NUMBER);

   //Path commandPath("command.es");
   //loadTemplate(commandPath.c_str());

   //_env.MaxThread = 1;
   //_env.Table = &_table;
   //_env.TLSIndex = &_tlsIndex;
   //_env.GCMGSize = 0x54000;
   //_env.GCYGSize = 0x15000;

   //InitializeVMSTA(&_sehTable, &_env, nullptr, nullptr, nullptr, &_header);

   //loadScript("~\\elt.es");
   //loadScript("~\\scripts\\grammar.es");
   //loadScript("~\\scripts\\tscript.es");

   //// load script passed via command line arguments
   //if (argc > 1) {
   //   for (int i = 1; i < argc; i++) {
   //      if (argv[i][0] == '-') {
   //         // check exit command
   //         if (argv[i][1] == 'q')
   //            return 0;

   //         executeCommand(argv[i]);
   //      }
   //      else executeScript(argv[i]);
   //   }
   //}

   //runSession();

   return 0;
}