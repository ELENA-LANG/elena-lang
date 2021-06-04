//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common types
//
//                                              (C)2018-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenamachineH
#define elenamachineH 1

namespace _ELENA_
{

#pragma pack(push, 1)
struct ExceptionStruct
{
   uintptr_t core_catch_addr;
   uintptr_t core_catch_level;
   uintptr_t core_catch_frame;
};

constexpr int SizeOfExceptionStruct64 = 0x10;
constexpr int SizeOfExceptionStruct32 = 0x0C;

// --- _Entry ---

struct _Entry
{
   union {
      void* address;
      int  (*entry)(void);
      int  (*evaluate)(void*);
      int  (*evaluate2)(uintptr_t, void*);
      int  (*evaluate3)(uintptr_t, void*, void*);
   };

   _Entry()
   {
      address = nullptr;
   }
};

// --- GCTable ---

struct GCTable
{
   uintptr_t gc_header;
   uintptr_t gc_start;
   uintptr_t gc_yg_start;
   uintptr_t gc_yg_current;
   uintptr_t gc_yg_end;
   uintptr_t gc_shadow;
   uintptr_t gc_shadow_end;
   uintptr_t gc_mg_start;
   uintptr_t gc_mg_current;
   uintptr_t gc_end;
   uintptr_t gc_mg_wbar;
   ExceptionStruct* gc_et_current;     // !! is not used for MTA
   uintptr_t gc_stack_frame;               // !! is not used for MTA
   pos_t gc_lock;                      // !! is not used for STA
   pos_t gc_signal;                    // !! is not used for STA
   pos_t tt_ptr;                       // !! is not used for STA
   pos_t tt_lock;                      // !! is not used for STA
   uintptr_t dbg_ptr;                      // NOTE : used only for VM Client
   uintptr_t gc_roots;
   uintptr_t gc_rootcount;
   uintptr_t gc_perm_start;
   uintptr_t gc_perm_end;
   uintptr_t gc_perm_current;
};

// --- TLSEntry ---

struct TLSEntry
{
   ExceptionStruct* tls_et_current;
   uintptr_t        tls_stack_frame;           // !! is not used for MTA
   void*            tls_sync_event;
   pos_t        tls_flags;
   pos_t        tls_threadindex;
};

// --- SystemEnv ---

struct SystemEnv
{
   pos_t             StatLength;   // NOTE : it is an initial value, should be copied to GCTable
   void*             StatRoots;    // NOTE : it is an initial value, should be copied to GCTable
   GCTable*          Table;
   pos_t*            TLSIndex;
   pos_t*            ThreadTable;
   void*             Invoker;
   pos_t             GCMGSize;
   pos_t             GCYGSize;
   pos_t             GCPERMSize;
   pos_t             MaxThread;
};

// --- ProgramHeader ---

struct ProgramHeader
{
   ExceptionStruct root_exception_struct;
};

// --- FrameHeader ---

struct FrameHeader
{
   uintptr_t reserved; // should be zero
   uintptr_t previousFrame;
};

struct GCRoot
{
   size_t size;
   union
   {
      void*  stackPtr;
      size_t stackPtrAddr;
   };   
};
#pragma pack(pop)

// --- SystemRoutineProvider ---

static class SystemRoutineProvider
{
public:
   static uintptr_t GetParent(void* classPtr);
   static size_t GetLength(void* classPtr);
   static size_t GetFlags(void* classPtr);

   static uintptr_t GetSignatureMember(void* messageTable, mssg_t message, int index);

   static uintptr_t NewHeap(int totalSize, int committedSize);
   static uintptr_t ExpandHeap(void* allocPtr, int newSize);
   static uintptr_t ExpandPerm(void* allocPtr, size_t newSize);
   static void CloseThreadHandle(TLSEntry* entry, bool withExit, pos_t exitCode);
   static TLSEntry* GetTLSEntry(pos_t tlsIndex);

   static void OpenSTAFrame(SystemEnv* env, FrameHeader* frameHeader);
   static void CloseSTAFrame(SystemEnv* env, FrameHeader* frameHeader);

   static void InitCriticalStruct(uintptr_t criticalHandler);
   static void InitTLSEntry(pos_t threadIndex, pos_t index, ProgramHeader* frameHeader, pos_t* threadTable);

   static void Init(SystemEnv* env);

   static void InitSTA(SystemEnv* env, ProgramHeader* frameHeader);
   static void InitMTA(SystemEnv* env, ProgramHeader* frameHeader);

   static int ExecuteInFrame(SystemEnv* env, _Entry& entry, void* address);

   static bool NewThread(SystemEnv* env, ProgramHeader* frameHeader);

   static void Exit(pos_t exitCode);
   static void ExitThread(SystemEnv* env, pos_t exitCode, bool withExit);

   static void OpenFrame(SystemEnv* env, FrameHeader* frameHeader);

   static void CloseFrame(SystemEnv* env, FrameHeader* frameHeader);

   static void* GCRoutine(GCTable* table, GCRoot* roots, size_t size);
   static void* GCRoutinePerm(GCTable* table, size_t size, size_t permSize);

   static void GCSignalStop(int handle);
   static void GCSignalClear(int handle);
   static void GCWaitForSignal(int handle);
   static void GCWaitForSignals(int count, int* handles);

   static void RaiseError(int code);
   static intptr_t AlignHeapSize(intptr_t size);

   static bool parseMessageLiteral(ident_t message, IdentifierString& messageName, pos_t& paramCount, ref_t& flags);

} __routineProvider;

} // _ELENA_

#endif // elenamachineH
