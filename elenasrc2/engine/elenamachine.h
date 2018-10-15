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
      int  (*entry)(void);
      int  (*evaluate)(void*);
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
   pos_t gc_mg_wbar;
   ExceptionStruct* gc_et_current;     // !! is not used for MTA
   pos_t gc_stack_frame;               // !! is not used for MTA
   pos_t gc_lock;                      // !! is not used for STA
   pos_t gc_signal;                    // !! is not used for STA
   pos_t tt_ptr;                       // !! is not used for STA
   pos_t tt_lock;                      // !! is not used for STA
   pos_t dbg_ptr;                      // NOTE : used only for VM Client
};

// --- TLSEntry ---

struct TLSEntry
{
   ExceptionStruct* tls_et_current;
   pos_t            tls_stack_frame;           // !! is not used for MTA
   void*            tls_sync_event;
   pos_t            tls_flags;
   pos_t            tls_threadindex;
};

// --- SystemEnv ---

struct SystemEnv
{
   pos_t             StatLength;
   void*             StatRoots;
   GCTable*          Table;
   pos_t*            TLSIndex;
   pos_t*            ThreadTable;
   pos_t             GCMGSize;
   pos_t             GCYGSize;
   pos_t             MaxThread;
};

// --- ProgramHeader ---

struct ProgramHeader
{
   ExceptionStruct root_exception_struct;
   CriticalStruct  root_critical_struct;
};

// --- FrameHeader ---

struct FrameHeader
{
   pos_t reserved; // should be zero
   pos_t previousFrame;
   pos_t currentFrame;
};

// --- SystemRoutineProvider ---

static class SystemRoutineProvider
{
public:
   static void InitCriticalStruct(CriticalStruct* header, pos_t criticalHandler);
   static void InitTLSEntry(pos_t threadIndex, pos_t index, ProgramHeader* frameHeader, pos_t* threadTable);

   static void Prepare();

   static void InitSTA(SystemEnv* env, ProgramHeader* frameHeader);
   static void InitMTA(SystemEnv* env, ProgramHeader* frameHeader);

   static int ExecuteInFrame(SystemEnv* env, _Entry& entry);

   static bool NewThread(SystemEnv* env, ProgramHeader* frameHeader);

   static void Exit(pos_t exitCode);
   static void ExitThread(SystemEnv* env, pos_t exitCode, bool withExit);

} __routineProvider;

} // _ELENA_

#endif // elenamachineH
