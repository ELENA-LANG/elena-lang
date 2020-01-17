//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common types
//
//                                              (C)2018-2020, by Alexei Rakov
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
   pos_t gc_roots;                     
   pos_t gc_rootcount;
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
   pos_t             StatLength;   // NOTE : it is an initial value, should be copied to GCTable
   void*             StatRoots;    // NOTE : it is an initial value, should be copied to GCTable
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
};

struct GCRoot
{
   void*  stackPtr;
   size_t size;
};

// --- SystemRoutineProvider ---

static class SystemRoutineProvider
{
public:
   static pos_t NewHeap(int totalSize, int committedSize);
   static void CloseThreadHandle(TLSEntry* entry, bool withExit, pos_t exitCode);
   static TLSEntry* GetTLSEntry(pos_t tlsIndex);

   static void OpenSTAFrame(SystemEnv* env, FrameHeader* frameHeader);
   static void CloseSTAFrame(SystemEnv* env, FrameHeader* frameHeader);

   static void InitCriticalStruct(CriticalStruct* header, pos_t criticalHandler);
   static void InitTLSEntry(pos_t threadIndex, pos_t index, ProgramHeader* frameHeader, pos_t* threadTable);

   static void Prepare();
   static void Init(SystemEnv* env);

   static void InitSTA(SystemEnv* env, ProgramHeader* frameHeader);
   static void InitMTA(SystemEnv* env, ProgramHeader* frameHeader);

   static int Execute(void* address, FrameHeader* framePtr);
   static int ExecuteInFrame(SystemEnv* env, _Entry& entry);
   static int ExecuteInNewFrame(SystemEnv* env, _Entry& entry);

   static bool NewThread(SystemEnv* env, ProgramHeader* frameHeader);

   static void Exit(pos_t exitCode);
   static void ExitThread(SystemEnv* env, pos_t exitCode, bool withExit);

   static void OpenFrame(SystemEnv* env, FrameHeader* frameHeader);

   static void CloseFrame(SystemEnv* env, FrameHeader* frameHeader);

   static void GCRoutine(GCTable* table, GCRoot* roots);

} __routineProvider;

} // _ELENA_

#endif // elenamachineH
