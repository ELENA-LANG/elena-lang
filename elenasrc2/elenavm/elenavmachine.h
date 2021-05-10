//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenavmachineH
#define elenavmachineH 1

#include "loader.h"
#include "config.h"
#include "libman.h"
#include "elenamachine.h"

constexpr auto ELENAVM_REVISION = 0x0014;

// --- ELENAVM common constants ---
#ifdef _WIN32
constexpr auto ELENAVM_GREETING = L"ELENA VM %d.%d.%d (C)2005-2021 by Alex Rakov";
constexpr auto ELENAVM_INITIALIZING = L"Initializing...";
constexpr auto ELENAVM_DEBUGINFO = L"Debug mode...";
constexpr auto ELENAVM_DONEINFO = L"Done...";
#elif _LINUX
constexpr auto ELENAVM_GREETING = "ELENA VM %d.%d.%d (C)2005-2021 by Alex Rakov";
constexpr auto ELENAVM_INITIALIZING = "Initializing...";
constexpr auto ELENAVM_DEBUGINFO = "Debug mode...";
constexpr auto ELENAVM_DONEINFO = "Done...";
#endif

namespace _ELENA_
{

typedef Map<ident_t, char*> Templates;
typedef Map<ident_t, char*> Primitives;
typedef Map<ident_t, char*> ForwardMap;

// --- InstanceConfig ---

struct InstanceConfig
{
   int platform;

   // options
//   int maxThread;
   int mgSize;
   int ygSize;
   int permSize;

   // paths
   Path libPath;

   // mappings
   Primitives primitives;
   ForwardMap forwards;
   ForwardMap moduleForwards;

   void loadForwardList(XmlConfigFile& config);
   void loadList(XmlConfigFile& config, const char* category, path_t path, Map<ident_t, char*>* list);
   void init(path_t configPath, XmlConfigFile& config);

   bool load(path_t path, Templates* templates);

   InstanceConfig()
      : primitives(NULL, freestr), forwards(NULL, freestr)
   {
      //maxThread = 1;
      mgSize = 20000;
      ygSize = 4000;
      permSize = 1000;
      platform = 0;
   }
   InstanceConfig(InstanceConfig& parent)
      : primitives(NULL, freestr), forwards(NULL, freestr), moduleForwards(NULL, freestr)
   {
      //maxThread = parent.maxThread;
      mgSize = parent.mgSize;
      ygSize = parent.ygSize;
      permSize = parent.permSize;

      // copy paths
      libPath.copy(parent.libPath.c_str());

      // copy forwards
      ForwardMap::Iterator it = parent.forwards.start();
      while (!it.Eof()) {
         forwards.add(it.key(), StrFactory::clone(*it));

         it++;
      }

      // copy module forwards
      it = parent.moduleForwards.start();
      while (!it.Eof()) {
         moduleForwards.add(it.key(), StrFactory::clone(*it));

         it++;
      }
   }
};

class Instance;

class ELENAVMMachine
{
protected:
   Instance* _instance;

   struct Config : InstanceConfig
   {
      bool load(path_t path, Templates* templates);
   };

   Path _rootPath;

public:
   // machine config
   Templates templates;
   Config    config;

   void startSTA(ProgramHeader* frameHeader, SystemEnv* env, void* sehTable, void* tape);

   void Exit(int exitCode);

   path_t getRootPath() { return _rootPath.c_str(); }
//   void setLibPath(const TCHAR* path);    // !! temporal

   void deleteInstance()
   {
      freeobj(_instance);
      _instance = nullptr;
   }

   ELENAVMMachine(path_t rootPath);
   virtual ~ELENAVMMachine()
   {
      deleteInstance();
   }
};

// --- Instance ---

class Instance : public _ImageLoader
{
protected:
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

      virtual lvaddr_t getVAddress(MemoryWriter& writer, ref_t mask) { return NULL; }

      virtual mssg_t resolveMessage(mssg_t reference, _Module* module) { return reference; }

      virtual void writeVAddress(MemoryWriter& writer, lvaddr_t vaddress, pos_t disp);
      virtual void writeRelVAddress(MemoryWriter& writer, lvaddr_t vaddress, ref_t mask, pos_t disp);

      virtual void writeReference(MemoryWriter& writer, ref_t reference, pos_t disp, _Module* module);
      virtual void writeMTReference(MemoryWriter& writer);

      //virtual void writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp, _Module* module)
      //{
      //   throw InternalError("Currently not supported");
      //}

      virtual void addBreakpoint(pos_t position);

      void writeTape(MemoryWriter& tape, lvaddr_t vaddress, int mask);

      ImageReferenceHelper(Instance* instance)
      {
         _instance = instance;

         _Memory* image = instance->getTargetSection(mskCodeRef);
         _Memory* data = instance->getTargetSection(mskRDataRef);

         _codeBase = (size_t)image->get(0);
         _statBase = (size_t)data->get(0);
      }
   };

   bool            _withExtDispatchers;
   bool            _debugMode;
//   int             _platform;
//
   bool            _initialized;
//   bool       _traceMode;
   _JITCompiler*   _compiler;
   JITLinker*      _linker;

   _Memory*       _messageTable;
   _Memory*       _messageBodyTable;
   _Memory*       _mattributeTable;
   size_t         _ConvertedMTSize; // used to trace the message table change
   size_t         _ConvertedMATSize; // used to trace the meta attribute table change

   IdentifierString _superClass;
   IdentifierString _literalClass;
   IdentifierString _wideLiteralClass;
   IdentifierString _characterClass;
   IdentifierString _intClass;
   IdentifierString _longClass;
   IdentifierString _realClass;
   IdentifierString _msgClass;
   IdentifierString _extMsgClass;
   IdentifierString _subjClass;

   LibraryManager  _loader;
   ELENAVMMachine* _machine;
   InstanceConfig  _config;

   MessageMap      _verbs;

   // status
   IdentifierString _status;

   void clearMessageTable();
   void clearMetaAttributeTable();

   virtual ident_t resolveForward(ident_t forward);

   ident_t resolveTemplateWeakReference(ident_t referenceName);
   virtual ReferenceInfo retrieveReference(_Module* module, ref_t reference, ref_t mask);
   virtual ident_t retrieveReference(void* address, ref_t mask);

   _Module* resolveModule(ident_t referenceName, LoadResult& result, ref_t& reference);
   _Module* resolveWeakModule(ident_t weakReferenceName, ref_t& reference);

   virtual SectionInfo getSectionInfo(ReferenceInfo referenceInfo, ref_t mask, bool silentMode);
   virtual ClassSectionInfo getClassSectionInfo(ReferenceInfo referenceInfo, ref_t codeMask, ref_t vmtMask, bool silentMode);
   virtual SectionInfo getCoreSectionInfo(ref_t reference, ref_t mask);

   virtual _Memory* getMessageSection() = 0;
   virtual _Memory* getMetaAttributeSection() = 0;

   bool initLoader(InstanceConfig& config);

   void setPackagePath(ident_t package, path_t path);
   void setPackagePath(ident_t line);

   void addPackagePath(ident_t package, ident_t path);
   void addPackagePath(ident_t line);

   bool loadTemplate(ident_t name);

   virtual bool restart(SystemEnv* env, void* sehTable, bool debugMode, bool withExtDispatchers);

   void translate(MemoryReader& reader, ImageReferenceHelper& helper, MemoryDump& dump, int terminator);
   void configurate(SystemEnv* env, void* sehTable, MemoryReader& reader, int terminator);
   void resolveMessageTable();
   void resolveMetaAttributeTable();

   void onNewCode(SystemEnv* env);

   //void* findDebugEntryPoint(ByteArray& tape);

   virtual void resumeVM() = 0;
   virtual void stopVM() = 0;

   void* parseMessage(SystemEnv* systemEnv, ident_t message);

//   void saveActionNames(MemoryWriter* writer)
//   {
////      _actions.write(writer);
//   }

   lvaddr_t loadReference(SystemEnv* systemEnv, ident_t referenceName, int mask, bool silentMode)
   {
      lvaddr_t ref = 0;
      if (_debugMode) {
         //// remove subject list from the debug section
         _Memory* debugSection = getTargetDebugSection();
         //if ((*debugSection)[0] > 0)
         //   debugSection->trim((*debugSection)[0]);

         ref = loadSymbol(referenceName, mask, silentMode);

         (*debugSection)[0] = debugSection->Length();

         //// add subject list to the debug section
         //_ELENA_::MemoryWriter debugWriter(debugSection);
         //saveActionNames(&debugWriter);

         raiseBreakpoint();
      }
      else ref = loadSymbol(referenceName, mask, silentMode);

      /*if (_linker->withNewInitializers())
         onNewInitializers(systemEnv);
      else*/onNewCode(systemEnv);

      return ref;
   }

   void* loadMetaAttribute(ident_t name, int category);
   ref_t loadDispatcherOverloadlist(ident_t referenceName);

public:
   ident_t getStatus() { return emptystr(_status) ? NULL : (const char*)_status; }

#ifdef _WIN32
   void printInfo(const wchar_t* s, ...);
   void printInfo(const char* s)
   {
      WideString str(s);

      printInfo(str.c_str());
   }
#elif _LINUX
   void printInfo(const char* s, ...);
#endif

   void setStatus(ident_t s)
   {
      _status.copy(s);
   }

   void setStatus(ident_t s1, ident_t s2)
   {
      _status.copy(s1);
      _status.append(emptystr(s2) ? "(null)" : s2);
   }

   void setStatus(ident_t s1, ReferenceInfo info)
   {
      _status.copy(s1);
      if (info.module != NULL) {
         if (isWeakReference(info.referenceName)) {
            _status.append(info.module->Name());
         }
      }
      _status.append(emptystr(info.referenceName) ? "(null)" : info.referenceName);
   }

   virtual void createConsole() = 0;

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

   virtual ident_t getWideLiteralClass()
   {
      return _wideLiteralClass;
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

   virtual ident_t getMessageNameClass()
   {
      return _subjClass;
   }

   virtual ident_t getMessageClass()
   {
      return _msgClass;
   }

   virtual ident_t getExtMessageClass()
   {
      return _extMsgClass;
   }

   virtual ident_t getNamespace()
   {
      return _loader.getNamespace();
   }

   virtual ident_t getClassName(void* vmtAddress)
   {
      return retrieveReference(vmtAddress, mskVMTRef);
   }

   virtual ident_t getSubject(ref_t subjectRef);

   virtual lvaddr_t getClassVMTRef(SystemEnv* systemEnv, ident_t referenceName, bool silentMode)
   {
      return loadReference(systemEnv, referenceName, mskVMTRef, silentMode);
   }

   virtual lvaddr_t getSymbolRef(SystemEnv* systemEnv, ident_t referenceName, bool silentMode)
   {
      return loadReference(systemEnv, referenceName, mskSymbolRef, silentMode);
   }

   int loadMessageName(mssg_t messageRef, char* buffer, size_t length);

   virtual ref_t getSubjectRef(SystemEnv* systemEnv, ident_t subjectName)
   {
      if (subjectName.find('$') != -1) {
         setStatus("Invalid subject");

         return 0;
      }

      ref_t subjRef = _linker->parseAction(subjectName);

      if (_messageTable->Length() > _ConvertedMTSize) {
         stopVM();

         onNewCode(systemEnv);

         resumeVM();
      }

      return subjRef;
   }

   virtual mssg_t getMessageRef(SystemEnv* systemEnv, ident_t messageName)
   {
      if (messageName.find('$') != -1) {
         setStatus("Invalid subject");

         return 0;
      }

      return (mssg_t)parseMessage(systemEnv, messageName);
   }

   virtual bool initSymbolReference(void* object, ident_t referenceName)
   {
      lvaddr_t symbolAddress = loadSymbol(referenceName, mskSymbolRef, true);
      if (symbolAddress != LOADER_NOTLOADED) {
         *(int*)object = (int)symbolAddress;

         return true;
      }
      else return false;
   }

   virtual pos_t getLinkerConstant(int id);

   virtual void* loadDebugSection() = 0;

//   bool init();

   lvaddr_t loadSymbol(ident_t reference, int mask, bool silentMode = false);

   int interprete(SystemEnv* env, void* sehTable, void* tape, bool standAlone);

   void onNewInitializers(SystemEnv* env);

   bool loadAddressInfo(void* address, char* buffer, size_t& maxLength);

   int loadExtensionDispatcher(SystemEnv* env, const char* moduleList, mssg_t message, void* output);

   virtual void addListener(_JITLoaderListener* listener)
   {
      _loader.addListener(listener);
   }

   Instance(ELENAVMMachine* machine);
   virtual ~Instance();
};

} // _ELENA_

#endif // elenavmachineH
