//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenamachineH
#define elenamachineH 1

#include "loader.h"
#include "config.h"
#include "libman.h"

#define VM_INIT           _T("$package'core'vm_console_entry")

#define VM_INTERPRET      _T("$package'core_vm'eval")
#define VM_INTERPRET_EXT  _T("$package'core_vm'start_n_eval")

// --- ELENAVM common constants ---
#define ELENAVM_GREETING        _T("ELENA VM %d.%d.%d (C)2005-2013 by Alex Rakov")

#define ELENAVM_BUILD_NUMBER    0x0001             // ELENA Enging version

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

typedef Map<const wchar16_t*, wchar16_t*> Templates;
typedef Map<const wchar16_t*, wchar16_t*> Primitives;
typedef Map<const wchar16_t*, wchar16_t*> ForwardMap;

// --- InstanceConfig ---

struct InstanceConfig
{
   // options
//   int maxThread;
   int pageSize;
   int ygRatio;
   int objSize;

   // paths
   Path libPath;

   // mappings
   Primitives primitives;
   ForwardMap forwards;
   ForwardMap moduleForwards;

   void loadForwardList(IniConfigFile& config);
   void loadList(IniConfigFile& config, const wchar16_t* category, const _path_t* path, Map<const wchar16_t*, wchar16_t*>* list);
   void init(const _path_t* configPath, IniConfigFile& config);

   bool load(const _path_t* path, Templates* templates);

   InstanceConfig()
      : primitives(NULL, freestr), forwards(NULL, freestr)
   {
      //maxThread = 1;
      pageSize = 0x10;
      ygRatio = 10;
      objSize = 0x0C;
   }
   InstanceConfig(InstanceConfig& parent)
      : primitives(NULL, freestr), forwards(NULL, freestr), moduleForwards(NULL, freestr)
   {
      //maxThread = parent.maxThread;
      pageSize = parent.pageSize;
      ygRatio = parent.ygRatio;
      objSize = parent.objSize;

      // copy paths
      libPath.copy(parent.libPath);

      // copy forwards
      ForwardMap::Iterator it = parent.forwards.start();
      while (!it.Eof()) {
         forwards.add(it.key(), wcsdup(*it));

         it++;
      }

      // copy module forwards
      it = parent.moduleForwards.start();
      while (!it.Eof()) {
         moduleForwards.add(it.key(), wcsdup(*it));

         it++;
      }
   }
};

class ELENAMachine
{
protected:
   struct Config : InstanceConfig
   {
      bool load(const _path_t* path, Templates* templates);
   };

   Path _rootPath;

public:
   // machine config
   Templates templates;
   Config    config;

   const _path_t* getRootPath() { return _rootPath; }
//   void setLibPath(const TCHAR* path);    // !! temporal

   ELENAMachine(const _path_t* rootPath);
   virtual ~ELENAMachine()
   {
   }
};

// --- Instance ---

class Instance : public _ImageLoader
{
protected:
   typedef void*(*VMAPI)(Instance*, void*);

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

      virtual SectionInfo getPredefinedSection(ref_t reference) { return SectionInfo(); }
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

   ConstantIdentifier _literalClass;
   ConstantIdentifier _intClass;
   ConstantIdentifier _longClass;
   ConstantIdentifier _realClass;

   LibraryManager  _loader;
   ELENAMachine*   _machine;
   InstanceConfig  _config;

   // vm interface
   VMAPI _loadClassName;
   VMAPI _loadSymbolPtr;
   VMAPI _interprete;
   VMAPI _getLastError;

   // status
   String<wchar16_t, 255> _status;

   virtual const wchar16_t* resolveForward(const wchar16_t* forward);

   virtual const wchar16_t* retrieveReference(_Module* module, ref_t reference, ref_t mask);
   virtual const wchar16_t* retrieveReference(void* address, ref_t mask);

   _Module* resolveModule(const wchar16_t* referenceName, LoadResult& result, ref_t& reference);

   virtual SectionInfo getSectionInfo(const wchar16_t* reference, size_t mask);
   virtual ClassSectionInfo getClassSectionInfo(const wchar16_t* reference, size_t codeMask, size_t vmtMask);

   bool initLoader(InstanceConfig& config);

   void setPackagePath(const wchar16_t* package, const _path_t* path);
   void setPackagePath(const wchar16_t* line);

   bool loadTemplate(const wchar16_t* name);

   virtual bool restart(bool debugMode);

   void translate(size_t base, MemoryReader& reader, ImageReferenceHelper& helper, MemoryDump& dump, int terminator);
   void configurate(size_t base, MemoryReader& reader, int terminator);

   //void* findDebugEntryPoint(ByteArray& tape);

   void printInfo(const _text_t* s, ...);

   virtual void resumeVM() = 0;
   virtual void stopVM() = 0;

public:
   const wchar16_t* getStatus() { return emptystr(_status) ? NULL : (const wchar16_t*)_status; }

   void setStatus(const wchar16_t* s)
   {
      _status.copy(s);
   }

   void setStatus(const wchar16_t* s1, const wchar16_t* s2)
   {
      _status.copy(s1);
      _status.append(emptystr(s2) ? _T("(null)") : s2);
   }

   virtual void raiseBreakpoint() = 0;

   bool isDebugMode() const { return _linker->getDebugMode(); }
//   bool isInitialized() const { return _initialized; }

   void addForward(const wchar16_t* forward, const wchar16_t* reference);
   void addForward(const wchar16_t* line);

   virtual const wchar16_t* getLiteralClass()
   {
      return _literalClass;
   }

   virtual const wchar16_t* getIntegerClass()
   {
      return _intClass;
   }

   virtual const wchar16_t* getLongClass()
   {
      return _longClass;
   }

   virtual const wchar16_t* getRealClass()
   {
      return _realClass;
   }

   virtual const wchar16_t* getClassName(void* vmtAddress)
   {
      return retrieveReference(vmtAddress, mskVMTRef);
   }

   //virtual void* getClassVMTRef(const wchar16_t* referenceName)
   //{
   //   return loadSymbol(referenceName, mskVMTRef);
   //}

   virtual void* getSymbolRef(const wchar16_t* referenceName)
   {
      return loadSymbol(referenceName, mskSymbolRef);
   }

   virtual bool initSymbolReference(void* object, const wchar16_t* referenceName)
   {
      void* symbolAddress = loadSymbol(referenceName, mskSymbolRef);
      if (symbolAddress != LOADER_NOTLOADED) {
         *(int*)object = (int)symbolAddress;
      }
      else return false;
   }

   virtual size_t getLinkerConstant(int id);

   virtual void* loadDebugSection() = 0;

//   bool init();

   void* loadSymbol(const wchar16_t* reference, int mask);

   int interprete(void* tape, const wchar16_t* interpreter);

   Instance(ELENAMachine* machine);
   virtual ~Instance();
};

} // _ELENA_

#endif // elenamachineH
