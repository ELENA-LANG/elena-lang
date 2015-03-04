//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Image class declarations
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef imageH
#define imageH 1

#include "loader.h"
#include "project.h"

namespace _ELENA_
{
// --- ExecutableImage ---

class ExecutableImage : public Image, public _ImageLoader
{
public:
   // --- Helper ---
   class _Helper
   {
   public:
      virtual void beforeLoad(_JITCompiler* compiler, ExecutableImage& image) = 0;

      virtual void afterLoad(ExecutableImage& image) = 0;
   };

private:
   Project* _project;
   void*    _entryPoint;

   ConstantIdentifier _literal;
   ConstantIdentifier _character;
   ConstantIdentifier _int;
   ConstantIdentifier _long;
   ConstantIdentifier _real;
   ConstantIdentifier _message;
   ConstantIdentifier _signature;
   ConstantIdentifier _verb;

public:
   virtual ref_t getEntryPoint()
   {
      return (ref_t)_entryPoint & ~mskAnyRef;
   }

   virtual ref_t getDebugEntryPoint();

   virtual ReferenceMap::Iterator getExternalIt()
   {
      return _exportReferences.start();
   }

   virtual _Memory* getTargetSection(size_t mask);

   virtual SectionInfo getSectionInfo(const wchar16_t* reference, size_t mask);
   virtual ClassSectionInfo getClassSectionInfo(const wchar16_t* reference, size_t codeMask, size_t vmtMask, bool silentMode);
   virtual SectionInfo getCoreSectionInfo(ref_t reference, size_t mask);

   virtual _Memory* getTargetDebugSection()
   {
      return &_debug;
   }

   virtual size_t getLinkerConstant(int id);

   virtual const wchar16_t* getLiteralClass();
   virtual const wchar16_t* getCharacterClass();
   virtual const wchar16_t* getIntegerClass();
   virtual const wchar16_t* getRealClass();
   virtual const wchar16_t* getLongClass();
   virtual const wchar16_t* getMessageClass();
   virtual const wchar16_t* getSignatureClass();
   virtual const wchar16_t* getVerbClass();
   virtual const wchar16_t* getNamespace();

   virtual const wchar16_t* retrieveReference(_Module* module, ref_t reference, ref_t mask);

   Project* getProject() const { return _project; }

   ExecutableImage(Project* project, _JITCompiler* compiler, _Helper& helper);
};

// --- VirtualMachineClientImage ---

class VirtualMachineClientImage : public Image
{
   ReferenceMap   _exportReferences;
//   Project*       _project;

   class VMClientHelper : public _BinaryHelper
   {
      VirtualMachineClientImage* _owner;
      ReferenceMap*              _references;
      MemoryWriter*              _dataWriter;
      _Module*                   _module;

   public:
      virtual void writeReference(MemoryWriter& writer, const wchar16_t* reference, int mask);

      VMClientHelper(VirtualMachineClientImage* owner, ReferenceMap* references, MemoryWriter* writer, _Module* module)
      {
         _owner = owner;
         _references = references;
         _dataWriter = writer;
         _module = module;
      }
   };

   friend class VMClientHelper;

   ref_t resolveExternal(const wchar16_t* function)
   {
      return mapKey(_exportReferences, function, mskImportRef | (_exportReferences.Count() + 1));
   }

   ref_t createTape(MemoryWriter& data, Project* project);

public:
   virtual ReferenceMap::Iterator getExternalIt()
   {
      return _exportReferences.start();
   }

   virtual ref_t getEntryPoint()
   {
      return 0; // !! temporal
   }

   virtual ref_t getDebugEntryPoint()
   {
      return 0; // !! temporal
   }

   VirtualMachineClientImage(Project* project, _JITCompiler* compiler, const tchar_t* appPath);
};

} // _ELENA_

#endif // imageH
