//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Image class declarations
//                                              (C)2005-2013, by Alexei Rakov
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
   Project* _project;
   void*    _entryPoint;

   ConstantIdentifier _literal;
   ConstantIdentifier _int;
   ConstantIdentifier _long;
   ConstantIdentifier _real;

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
   virtual ClassSectionInfo getClassSectionInfo(const wchar16_t* reference, size_t codeMask, size_t vmtMask);
   virtual SectionInfo getPredefinedSectionInfo(ref_t reference, size_t mask);

   virtual _Memory* getTargetDebugSection()
   {
      return &_debug;
   }

   virtual size_t getLinkerConstant(int id);

   virtual const wchar16_t* getLiteralClass();
   virtual const wchar16_t* getIntegerClass();
   virtual const wchar16_t* getRealClass();
   virtual const wchar16_t* getLongClass();

   virtual const wchar16_t* retrieveReference(_Module* module, ref_t reference, ref_t mask);

   ExecutableImage(Project* project, _JITCompiler* compiler);
};

// --- VirtualMachineClientImage ---

class VirtualMachineClientImage : public Image
{
   ReferenceMap   _exportReferences;
   Project*       _project;

   Path           _rootPath; 

   class VMClientHelper : public _BinaryHelper
   {
      VirtualMachineClientImage* _owner;
      ReferenceMap*              _references;

   public:
      virtual void writeReference(MemoryWriter& writer, const wchar16_t* reference, int mask);

      VMClientHelper(VirtualMachineClientImage* owner, ReferenceMap* references)
      {
         _owner = owner;
         _references = references;
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
