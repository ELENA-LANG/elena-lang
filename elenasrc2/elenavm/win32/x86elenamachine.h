//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2015, by Alexei Rakov
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
   path_t _rootPath;

   x86Process     _codeProcess, _dataProcess, _bssProcess, _statProcess, _debugProcess;

   virtual ref_t resolveExternal(ident_t reference);

protected:
   virtual _Memory* getTargetSection(size_t mask);
   virtual _Memory* getTargetDebugSection();

   virtual void mapReference(ident_t reference, void* vaddress, size_t mask);

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
   x86Instance* _instance;

public:
   Instance* getInstance()
   {
      return _instance;
   }

   void newInstance(x86Instance* instance)
   {
      _instance = instance;
   }

   void deleteInstance()
   {
      freeobj(_instance);
      _instance = NULL;
   }

   x86ELENAMachine(path_t rootPath);
};

} // _ELENA_

#endif // x86elenamachineH
