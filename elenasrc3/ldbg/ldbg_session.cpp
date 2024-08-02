//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains DPA Session wrapper implementation
//
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ldbg_session.h"
#include "common.h"

using namespace elena_lang;

// --- DPASessionWrapper ---

DPASessionWrapper :: DPASessionWrapper()
{

}

DPASessionWrapper :: ~DPASessionWrapper()
{
   freeobj(_session);
}

void DPASessionWrapper :: prepare()
{
   _session = new dpa::Session();
}

void DPASessionWrapper :: run()
{
   // Wait for the ConfigurationDone request to be made.

}
