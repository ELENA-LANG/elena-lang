//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef x86elenavmachineH
#define x86elenavmachineH 1

#include "elena.h"
#include "elenavmachine.h"

#include "x86process.h"

namespace _ELENA_
{

// --- x86Instance ---

class x86Instance : public Instance
{
   path_t _rootPath;

   x86Process     _codeProcess, _dataProcess, _bssProcess, _statProcess, _debugProcess, _messageProcess, _mattributeProcess;

   virtual ref_t resolveExternal(ident_t reference);

protected:
   virtual _Memory* getTargetSection(size_t mask);
   virtual _Memory* getTargetDebugSection();
   virtual _Memory* getMessageSection();
   virtual _Memory* getMetaAttributeSection();

   virtual bool restart(SystemEnv* env, void* sehTable, bool debugMode, bool withExtDispatchers);

   virtual void resumeVM();
   virtual void stopVM();

public:
   virtual void createConsole();

   virtual void* loadDebugSection();

   virtual void raiseBreakpoint();

   x86Instance(ELENAVMMachine* machine);
};

// --- x86ELENAMachine ---

class x86ELENAVMMachine : public ELENAVMMachine
{
public:
   Instance* getInstance()
   {
      return _instance;
   }

   x86ELENAVMMachine(path_t rootPath);
};

} // _ELENA_

#endif // x86elenavmachineH
