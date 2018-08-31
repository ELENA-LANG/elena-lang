//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common types
//
//                                              (C)2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenamachineH
#define elenamachineH 1

namespace _ELENA_
{

struct ExceptionStruct
{
   pos_t core_catch_addr;
   pos_t core_catch_level;
   pos_t core_catch_frame;
};

struct CriticalStruct
{
   pos_t previousStruct;
   pos_t handler;
};

// --- _Entry ---

struct _Entry
{
   union {
      void* address;
      void(*entry)(void);
      int(*evaluate)(void*);
   };

   _Entry()
   {
      address = nullptr;
   }
};

// --- GCTable ---

struct GCTable
{
   pos_t gc_header;
   pos_t gc_start;
   pos_t gc_yg_start;
   pos_t gc_yg_current;
   pos_t gc_yg_end;
   pos_t gc_shadow;
   pos_t gc_shadow_end;
   pos_t gc_mg_start;
   pos_t gc_mg_current;
   pos_t gc_end;
   pos_t reserved;
   pos_t gc_ext_stack_frame;
   pos_t gc_mg_wbar;
   pos_t gc_stack_bottom;
};

// --- SystemEnv ---

struct SystemEnv
{
   pos_t             StatLength;
   void*             StatRoots;
   GCTable*          Table;
   ExceptionStruct*  ExTable;
   pos_t             GCMGSize;
   pos_t             GCYGSize;
   pos_t             MaxThread;
};

// --- ProgramHeader ---

struct FrameHeader
{
   ExceptionStruct root_exception_struct;
   CriticalStruct  root_critical_struct;
};

// --- SystemRoutineProvider ---

static class SystemRoutineProvider
{
public:
   static void InitCriticalStruct(CriticalStruct* header, pos_t criticalHandler);

   static void Prepare();

   static void InitSTA(SystemEnv* env);

   static void NewFrame(SystemEnv* env, ExceptionStruct* topHeader, pos_t exceptionHandler);

   static void Exit(pos_t exitCode);

} __routineProvider;

} // _ELENA_

#endif // elenamachineH
