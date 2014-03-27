//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2011, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef x86elenamachineH
#define x86elenamachineH 1

#include "elena.h"
#include "elenamachine.h"

#include "x86process.h"

namespace _ELENA_
{

// --- x86Instance ---

class x86Instance : public Instance
{
   const tchar_t* _rootPath;

   x86Process     _codeProcess, _dataProcess, _bssProcess, _statProcess, _debugProcess;

   virtual ref_t resolveExternal(const wchar16_t* reference);

protected:
   virtual _Memory* getTargetSection(size_t mask);
   virtual _Memory* getTargetDebugSection();

   virtual void mapReference(const wchar16_t* reference, void* vaddress, size_t mask);

   virtual bool restart(bool debugMode);

   virtual void resumeVM();
   virtual void stopVM();

public:
   virtual void* loadDebugSection();

   virtual void raiseBreakpoint();

   x86Instance(ELENAMachine* machine);
};

// --- x86ELENAMachine ---

class x86ELENAMachine : public ELENAMachine
{
   Map<int, x86Instance*> _instances;

public:
//   int getInstanceCount() const
//   {
//      return _instances.Count();
//   }

   Instance* getInstance(int processID)
   {
      return _instances.get(processID);
   }

   void newInstance(int processID, x86Instance* instance)
   {
      _instances.add(processID, instance);
   }

   void deleteInstance(int processID)
   {
      _instances.erase(processID);
   }

   x86ELENAMachine(const tchar_t* rootPath);
};

} // _ELENA_

#endif // x86elenamachineH
