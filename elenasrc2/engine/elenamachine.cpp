//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common implementation
//
//                                              (C)2018-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"

using namespace _ELENA_;

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

   int retVal = entry.evaluate2((pos_t)&frameHeader, address);

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

