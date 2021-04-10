//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA SM Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------
#include "elenasm.h"
#include "session.h"
#include <unistd.h>

#define ROOT_PATH        "/etc/elena"

using namespace _ELENA_;

static Session* session = nullptr;

void newSession()
{
   session = new Session(ROOT_PATH);
}

// === dll entries ===

void* InterpretScript(ident_t script)
{
   if (!session) {
      newSession();
   }

   return session->translate(0, script);
}

void* InterpretScopeScript(int scope_id, ident_t script)
{
   if (!session) {
      newSession();
   }

   return session->translate(scope_id, script);
}

void* InterpretFile(const char* pathStr, int encoding, bool autoDetect)
{
   if (!session) {
      newSession();
   }

   return session->translate(0, pathStr, encoding, autoDetect);
}

void* InterpretScopeFile(int scope_id, const char* pathStr, int encoding, bool autoDetect)
{
   if (!session) {
      newSession();
   }

   return session->translate(scope_id, pathStr, encoding, autoDetect);
}

void Release(void* tape)
{
   if (!session) {
      newSession();
   }

   session->free(tape);
}

int GetStatus(char* buffer, int maxLength)
{
   if (session) {
      ident_t error = session->getLastError();
      size_t length = getlength(error);
      if (buffer != NULL) {
         if ((int)length > maxLength)
            length = maxLength;

         Convertor::copy(buffer, error, length, length);
      }

      return length;
   }
   else return 0;
}

int NewScope()
{
   if (session) {
      return session->newScope();
   }
   else return -1;
}
