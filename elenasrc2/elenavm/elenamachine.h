//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenamachineH
#define elenamachineH 1

#include "loader.h"
#include "config.h"
#include "libman.h"

#define VM_INIT           "$native'coreapi'vm_console_entry"

#define VM_INTERPRET      "$native'core_vm'eval"
#define VM_INTERPRET_EXT  "$native'core_vm'start_n_eval"

// --- ELENAVM common constants ---
#define ELENAVM_GREETING        L"ELENA VM %d.%d.%d (C)2005-2015 by Alex Rakov"

#define ELENAVM_REVISION_NUMBER    0x0001             // ELENAVM revision version

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
      address = NULL;
   }
};

typedef Map<ident_t, ident_c*> Templates;
typedef Map<ident_t, ident_c*> Primitives;
typedef Map<ident_t, ident_c*> ForwardMap;

// --- InstanceConfig ---

struct InstanceConfig
{
   // options
//   int maxThread;
   int mgSize;
   int ygSize;
   int objSize;

   // paths
   Path libPath;

   // mappings
   Primitives primitives;
   ForwardMap forwards;
   ForwardMap moduleForwards;

   void loadForwardList(IniConfigFile& config);
   void loadList(IniConfigFile& config, const char* category, path_t path, Map<ident_t, ident_c*>* list);
   void init(path_t configPath, IniConfigFile& config);

   bool load(path_t path, Templates* templates);

   InstanceConfig()
      : primitives(NULL, freestr), forwards(NULL, freestr)
   {
      //maxThread = 1;
      mgSize = 20000;
      ygSize = 4000;
      objSize = 0x0C;
   }
   InstanceConfig(InstanceConfig& parent)
      : primitives(NULL, freestr), forwards(NULL, freestr), moduleForwards(NULL, freestr)
   {
      //maxThread = parent.maxThread;
      mgSize = parent.mgSize;
      ygSize = parent.ygSize;
      objSize = parent.objSize;

      // copy paths
      libPath.copy(parent.libPath);

      // copy forwards
      ForwardMap::Iterator it = parent.forwards.start();
      while (!it.Eof()) {
         forwards.add(it.key(), StringHelper::clone(*it));

         it++;
      }

      // copy module forwards
      it = parent.moduleForwards.start();
      while (!it.Eof()) {
         moduleForwards.add(it.key(), StringHelper::clone(*it));

         it++;
      }
   }
};

class ELENAMachine
{
protected:
   struct Config : InstanceConfig
   {
      bool load(path_t path, Templates* templates);
   };

   Path _rootPath;

public:
   // machine config
   Templates templates;
   Config    config;

   path_t getRootPath() { return _rootPath; }
//   void setLibPath(const TCHAR* path);    // !! temporal

   ELENAMachine(path_t rootPath);
   virtual ~ELENAMachine()
   {
   }
};

// --- Instance ---

class Instance : public _ImageLoader
{
protected:
   typedef void*(*VMAPI)(Instance*, void*);
   typedef size_t(*VMAPI_NAME)(Instance*, void*,ident_c*,size_t);

   // --- ImageReferenceHelper ---
   // in most cases the references are already real ones
   // so only some of the methods are implemented
   class ImageReferenceHelper : public _ReferenceHelper
   {
      Instance* _instance;
      size_t    _codeBase, _statBase;

   public:
      virtual ref_t getLinkerConstant(ref_t constant)
      {
         return _instance->getLinkerConstant(constant);
      }

      virtual SectionInfo getCoreSection(ref_t reference) { return SectionInfo(); }
      virtual SectionInfo getSection(ref_t reference, _Module* module) { return SectionInfo(); }

      virtual void* getVAddress(MemoryWriter& writer, int mask) { return NULL; }

      virtual ref_t resolveMessage(ref_t reference, _Module* module) { return reference; }

      virtual void writeReference(MemoryWriter& writer, ref_t reference, size_t disp, _Module* module);
      virtual void writeReference(MemoryWriter& writer, void* vaddress, bool relative, size_t disp);

      virtual void addBreakpoint(size_t position);

      void writeTape(MemoryWriter& tape, void* vaddress, int mask);

      ImageReferenceHelper(Instance* instance)
      {
         _instance = instance;

         _Memory* image = instance->getTargetSection(mskCodeRef);
         _Memory* data = instance->getTargetSection(mskRDataRef);

         _codeBase = (size_t)image->get(0);
         _statBase = (size_t)data->get(0);
      }
   };

   bool            _debugMode;

   bool            _initialized;
//   bool       _traceMode;
   _JITCompiler*   _compiler;
   JITLinker*      _linker;

   IdentifierString _literalClass;
   IdentifierString _characterClass;
   IdentifierString _intClass;
   IdentifierString _longClass;
   IdentifierString _realClass;
   IdentifierString _msgClass;
   IdentifierString _signClass;
   IdentifierString _verbClass;

   LibraryManager  _loader;
   ELENAMachine*   _machine;
   InstanceConfig  _config;

   // vm interface
   VMAPI_NAME _loadClassName;
   VMAPI_NAME _loadSubjectName;
   VMAPI      _loadSymbolPtr;
   VMAPI      _loadSubject;
   VMAPI      _interprete;
   VMAPI      _getLastError;
   VMAPI_NAME _loadAddrInfo;

   // status
   IdentifierString _status;

   virtual ident_t resolveForward(ident_t forward);

   virtual ident_t retrieveReference(_Module* module, ref_t reference, ref_t mask);
   virtual ident_t retrieveReference(void* address, ref_t mask);

   _Module* resolveModule(ident_t referenceName, LoadResult& result, ref_t& reference);

   virtual SectionInfo getSectionInfo(ident_t reference, size_t mask);
   virtual ClassSectionInfo getClassSectionInfo(ident_t reference, size_t codeMask, size_t vmtMask, bool silentMode);
   virtual SectionInfo getCoreSectionInfo(ref_t reference, size_t mask);

   bool initLoader(InstanceConfig& config);

   void setPackagePath(ident_t package, path_t path);
   void setPackagePath(ident_t line);

   bool loadTemplate(ident_t name);

   virtual bool restart(bool debugMode);

   void translate(MemoryReader& reader, ImageReferenceHelper& helper, MemoryDump& dump, int terminator);
   void configurate(MemoryReader& reader, int terminator);

   //void* findDebugEntryPoint(ByteArray& tape);

   void printInfo(const wide_c* s, ...);

   virtual void resumeVM() = 0;
   virtual void stopVM() = 0;

public:
   ident_t getStatus() { return emptystr(_status) ? NULL : (ident_t)_status; }

   void setStatus(ident_t s)
   {
      _status.copy(s);
   }

   void setStatus(ident_t s1, ident_t s2)
   {
      _status.copy(s1);
      _status.append(emptystr(s2) ? "(null)" : s2);
   }

   virtual void raiseBreakpoint() = 0;

   void setDebugMode(bool isActive = true)
   {
      _debugMode = isActive;
   }

   bool isDebugMode() const { return _debugMode; }
//   bool isInitialized() const { return _initialized; }

   void addForward(ident_t forward, ident_t reference);
   void addForward(ident_t line);

   virtual ident_t getLiteralClass()
   {
      return _literalClass;
   }

   virtual ident_t getCharacterClass()
   {
      return _characterClass;
   }

   virtual ident_t getIntegerClass()
   {
      return _intClass;
   }

   virtual ident_t getLongClass()
   {
      return _longClass;
   }

   virtual ident_t getRealClass()
   {
      return _realClass;
   }

   virtual ident_t getSignatureClass()
   {
      return _signClass;
   }

   virtual ident_t getMessageClass()
   {
      return _msgClass;
   }

   virtual ident_t getVerbClass()
   {
      return _verbClass;
   }

   virtual ident_t getNamespace()
   {
      return _loader.getPackage();
   }

   virtual ident_t getClassName(void* vmtAddress)
   {
      return retrieveReference(vmtAddress, mskVMTRef);
   }

   virtual ident_t getSubject(ref_t subjectRef)
   {
      return retrieveReference((void*)subjectRef, 0);
   }

   //virtual void* getClassVMTRef(const wchar16_t* referenceName)
   //{
   //   return loadSymbol(referenceName, mskVMTRef);
   //}

   virtual void* getSymbolRef(ident_t referenceName)
   {
      return loadSymbol(referenceName, mskSymbolRef);
   }

   virtual ref_t getSubjectRef(ident_t subjectName)
   {
      return (ref_t)resolveReference(subjectName, 0);
   }

   virtual bool initSymbolReference(void* object, ident_t referenceName)
   {
      void* symbolAddress = loadSymbol(referenceName, mskSymbolRef);
      if (symbolAddress != LOADER_NOTLOADED) {
         *(int*)object = (int)symbolAddress;

         return true;
      }
      else return false;
   }

   virtual size_t getLinkerConstant(int id);

   virtual void* loadDebugSection() = 0;

//   bool init();

   void* loadSymbol(ident_t reference, int mask);
   //size_t loadMessage(ident_t reference);

   int interprete(void* tape, ident_t interpreter);

   bool loadAddressInfo(void* address, ident_c* buffer, size_t& maxLength);

   //bool loadSubjectInfo(size_t subjectId, ident_c* buffer, size_t& maxLength);
   //bool loadMessageInfo(size_t messageId, ident_c* buffer, size_t& maxLength);

   Instance(ELENAMachine* machine);
   virtual ~Instance();
};

} // _ELENA_

#endif // elenamachineH
