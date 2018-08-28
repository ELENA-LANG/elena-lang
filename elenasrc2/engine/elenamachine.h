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

// --- SystemEnv ---

struct SystemEnv
{
   size_t StatLength;
   void*  StatRoots;
   size_t GCMGSize;
   size_t GCYGSize;
   size_t MaxThread;
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
