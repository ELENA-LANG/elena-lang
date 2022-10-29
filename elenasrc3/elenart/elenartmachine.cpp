//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Machine declaration
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenartmachine.h"
#include "bytecode.h"

using namespace elena_lang;

// --- ELENARTMachine ---

ELENARTMachine :: ELENARTMachine(void* mdata)
   : mdata(mdata)
{

}

void ELENARTMachine :: loadSubjectName(IdentifierString& actionName, ref_t subjectRef)
{
   ImageSection section(mdata, 0x1000000);
   ref_t actionPtr = MemoryBase::getDWord(&section, subjectRef * sizeof(uintptr_t) * 2);
   if (!actionPtr) {
      addr_t namePtr = 0;
      section.read(subjectRef * sizeof(uintptr_t) * 2 + sizeof(uintptr_t), &namePtr, sizeof(addr_t));

      MemoryReader reader(&section);
      reader.seek((pos_t)(namePtr - (addr_t)mdata));

      reader.readString(actionName);
   }
   else loadSubjectName(actionName, actionPtr);
}

size_t ELENARTMachine :: loadMessageName(mssg_t message, char* buffer, size_t length)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   IdentifierString actionName;
   loadSubjectName(actionName, actionRef);

   IdentifierString messageName;
   ByteCodeUtil::formatMessageName(messageName, nullptr, *actionName, nullptr, 0, argCount, flags);

   StrConvertor::copy(buffer, *messageName, messageName.length(), length);

   buffer[length] = 0;
   printf("loadMessageName %s\n", buffer);

   return length;
}

void ELENARTMachine :: Exit(int exitCode)
{
   __routineProvider.Exit(exitCode);
}

void ELENARTMachine :: startSTA(SystemEnv* env, SymbolList* entryList)
{
   // setting up system
   __routineProvider.InitSTA(env);

   Entry entry;
   entry.address = env->bc_invoker;

   // executing the program
   int retVal = 0;
   for (size_t i = 0; i < entryList->length; i += sizeof(intptr_t)) {
      try
      {
         retVal = entry.evaluate(entryList->entries[i].address, 0);
      }
      catch (InternalError&)
      {
         //_instance->printInfo("EAbortException");

         retVal = -1;
      }
   }

   // winding down system
   Exit(retVal);
}
