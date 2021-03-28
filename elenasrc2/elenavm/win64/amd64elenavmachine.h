//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef amd64elenavmachineH
#define amd64elenavmachineH 1

#include "elena.h"
#include "elenavmachine.h"

#include "amd64process.h"

namespace _ELENA_
{

// --- AMD64Instance ---

class AMD64Instance : public Instance
{
   path_t _rootPath;

   AMD64Process     _codeProcess, _dataProcess, _bssProcess, _statProcess, _debugProcess, _messageProcess, _mattributeProcess;

   virtual lvaddr_t resolveExternal(ident_t reference);

protected:
   virtual _Memory* getTargetSection(pos_t mask);
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

   AMD64Instance(ELENAVMMachine* machine);
};

// --- AMD64ELENAVMMachine ---

class AMD64ELENAVMMachine : public ELENAVMMachine
{
public:
   Instance* getInstance()
   {
      return _instance;
   }

   AMD64ELENAVMMachine(path_t rootPath);
};

} // _ELENA_

#endif // amd64elenavmachineH
