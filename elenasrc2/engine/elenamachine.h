//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common types
//
//                                              (C)2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenamachineH
#define elenamachineH 1

namespace _ELENA_
{

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
   pos_t    StatLength;
   void*    StatRoots;
   GCTable* Table;
   pos_t    GCMGSize;
   pos_t    GCYGSize;
   pos_t    MaxThread;
};

// --- SystemRoutineProvider ---

static class SystemRoutineProvider
{
public:
   static void Prepare();

   static void InitSTA(SystemEnv* env);

   static void NewFrame();

   static void Exit();

} __routineProvider;

} // _ELENA_

#endif // elenamachineH
