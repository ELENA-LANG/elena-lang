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
   _events.init(-1);
}

DPASessionWrapper :: ~DPASessionWrapper()
{
   freeobj(_session);
}

void DPASessionWrapper :: prepare()
{
   _session = new dpa::Session();

   // The ConfigurationDone request is made by the client once all configuration
   // requests have been made.
    
   //session->registerHandler([&](const dap::ConfigurationDoneRequest&) {
   //   configured.fire();
   //   return dap::ConfigurationDoneResponse();
   //   });

}

void DPASessionWrapper :: bind()
{
   _session->connect();
}

void DPASessionWrapper :: run()
{
   _session->start();

   // Wait for the ConfigurationDone request to be made.
   _events.waitForEvent(LDBF_CONFIGURED);
}
