//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common implementation
//
//                                              (C)2018-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"

using namespace _ELENA_;

uintptr_t SystemRoutineProvider :: GetParent(void* classPtr)
{
   VMTHeader* header = (VMTHeader*)((uintptr_t)classPtr - elVMTClassOffset32);

   return header->parentRef;
}

size_t SystemRoutineProvider :: GetLength(void* classPtr)
{
   VMTHeader* header = (VMTHeader*)((uintptr_t)classPtr - elVMTClassOffset32);

   return header->count;
}

size_t SystemRoutineProvider::GetFlags(void* classPtr)
{
   VMTHeader* header = (VMTHeader*)((uintptr_t)classPtr - elVMTClassOffset32);

   return header->flags;
}

uintptr_t SystemRoutineProvider :: GetSignatureMember(void* messageTable, mssg_t message, int index)
{
   ref_t action = message >> ACTION_ORDER;

   uintptr_t ptr = (uintptr_t)messageTable + (action * sizeof(uintptr_t) * 2) + sizeof(uintptr_t);

   uintptr_t sign = (uintptr_t)messageTable + *(uintptr_t*)ptr;

   uintptr_t t = ((uintptr_t*)sign)[index];

   return t;
}

bool SystemRoutineProvider :: parseMessageLiteral(ident_t message, IdentifierString& messageName,
   pos_t& paramCount, ref_t& flags)
{
   paramCount = -1;

   size_t subject = 0;
   size_t param = 0;
   for (size_t i = 0; i < getlength(message); i++) {
      if (message[i] == '[') {
         if (message[getlength(message) - 1] == ']') {
            messageName.copy(message + i + 1, getlength(message) - i - 2);
            paramCount = messageName.ident().toInt();
            if (paramCount > ARG_COUNT)
               return false;
         }
         else return false;

         param = i;
      }
      else if (message[i] >= 65 || (message[i] >= 48 && message[i] <= 57)) {
      }
      else if (message[i] == ']' && i == (getlength(message) - 1)) {
      }
      else if (message[i] == '#') {
      }
      else return false;
   }

   if (param != 0) {
      messageName.copy(message + subject, param - subject);
   }
   else if (paramCount != -1) {
      // if it is a function invoker
      messageName.copy(INVOKE_MESSAGE);

      flags |= FUNCTION_MESSAGE;
   }
   else messageName.copy(message + subject);

   if (messageName.ident().startsWith("prop#")) {
      flags |= PROPERTY_MESSAGE;

      messageName.cut(0, getlength("prop#"));
   }

   return true;
}

void SystemRoutineProvider :: OpenSTAFrame(SystemEnv* env, FrameHeader* frameHeader)
{
   frameHeader->previousFrame = env->Table->gc_stack_frame;
   frameHeader->reserved = 0;
}

void SystemRoutineProvider :: CloseSTAFrame(SystemEnv* env, FrameHeader* frameHeader)
{
   env->Table->gc_stack_frame = frameHeader->previousFrame;
}

void SystemRoutineProvider :: InitSTA(SystemEnv* env, ProgramHeader* frameHeader)
{
   Init(env);

   // setting current exception handler (only for STA)
   env->Table->gc_et_current = &frameHeader->root_exception_struct;
   env->Table->gc_stack_frame = 0;
}

void SystemRoutineProvider :: InitMTA(SystemEnv* env, ProgramHeader* frameHeader)
{
   Init(env);
   InitTLSEntry(0, *env->TLSIndex, frameHeader, env->ThreadTable);

   // set the thread table size
   env->ThreadTable[-1] = env->MaxThread;
}

int SystemRoutineProvider :: ExecuteInFrame(SystemEnv* env, _Entry& entry, void* address)
{
   FrameHeader frameHeader;
   OpenFrame(env, &frameHeader);

   int retVal = entry.evaluate2((uintptr_t)&frameHeader, address);

   CloseFrame(env, &frameHeader);

   return retVal;
}

void SystemRoutineProvider :: OpenFrame(SystemEnv* env, FrameHeader* frameHeader)
{
   if (env->MaxThread <= 1) {
      OpenSTAFrame(env, frameHeader);
   }
}

void SystemRoutineProvider :: CloseFrame(SystemEnv* env, FrameHeader* frameHeader)
{
   if (env->MaxThread <= 1) {
      CloseSTAFrame(env, frameHeader);
   }
}

