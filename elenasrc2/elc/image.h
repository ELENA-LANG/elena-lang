//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Image class declarations
//                                              (C)2005-2018, by Alexei Rakov
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
   protected:
      bool _vmMode;

      void createTape(_Memory& data, Project* project, bool withNewConsole);

   public:
      virtual void beforeLoad(_JITCompiler* compiler, ExecutableImage& image) = 0;

      virtual void afterLoad(ExecutableImage& image) = 0;

      _Helper()
      {
         _vmMode = false;
      }
   };

private:
   Project* _project;
   lvaddr_t  _entryPoint;
   pos_t    _objectHeaderSize;

   ident_t  _literal;
   ident_t  _wideLiteral;
   ident_t  _character;
   ident_t  _int;
   ident_t  _long;
   ident_t  _real;
   ident_t  _message;
   ident_t  _ext_message;
   ident_t  _messageName;

public:
   virtual lvaddr_t getEntryPoint()
   {
      return (ref_t)_entryPoint & ~mskAnyRef;
   }

   virtual ref_t getDebugEntryPoint();

   virtual VAddressMap::Iterator getExternalIt()
   {
      return _exportReferences.start();
   }

   virtual _Memory* getTargetSection(ref_t mask);

   virtual SectionInfo getSectionInfo(ReferenceInfo referenceInfo, ref_t mask, bool silentMode);
   virtual ClassSectionInfo getClassSectionInfo(ReferenceInfo referenceInfo, ref_t codeMask, ref_t vmtMask, bool silentMode);
   virtual SectionInfo getCoreSectionInfo(ref_t reference, ref_t mask);

   virtual _Memory* getTargetDebugSection()
   {
      return &_debug;
   }

   virtual pos_t getLinkerConstant(int id);

   virtual ident_t getLiteralClass();
   virtual ident_t getWideLiteralClass();
   virtual ident_t getCharacterClass();
   virtual ident_t getIntegerClass();
   virtual ident_t getRealClass();
   virtual ident_t getLongClass();
   virtual ident_t getMessageClass();
   virtual ident_t getExtMessageClass();
   virtual ident_t getMessageNameClass();
   virtual ident_t getNamespace();

   ident_t resolveTemplateWeakReference(ident_t referenceName);
   virtual ReferenceInfo retrieveReference(_Module* module, ref_t reference, ref_t mask);

   Project* getProject() const { return _project; }

   //void saveActionNames(MemoryWriter* writer)
   //{
   //   _actions.write(writer);
   //}

   virtual void addListener(_JITLoaderListener* listener)
   {
      _project->addLoaderListener(listener);
   }

   ExecutableImage(bool standAlone, Project* project, _JITCompiler* compiler, _Helper& helper);
};

//// --- VirtualMachineClientImage ---
//
//class VirtualMachineClientImage : public Image
//{
//   ReferenceMap   _exportReferences;
//   Project*       _project;
//
//   class VMClientHelper : public _BinaryHelper
//   {
//      VirtualMachineClientImage* _owner;
//      ReferenceMap*              _references;
//      MemoryWriter*              _dataWriter;
//      _Module*                   _module;
//
//   public:
//      virtual void writeReference(MemoryWriter& writer, ident_t reference, int mask);
//
//      VMClientHelper(VirtualMachineClientImage* owner, ReferenceMap* references, MemoryWriter* writer, _Module* module)
//      {
//         _owner = owner;
//         _references = references;
//         _dataWriter = writer;
//         _module = module;
//      }
//   };
//
//   friend class VMClientHelper;
//
//   ref_t resolveExternal(ident_t function)
//   {
//      return mapKey(_exportReferences, function, mskImportRef | (_exportReferences.Count() + 1));
//   }
//
//   ref_t createTape(MemoryWriter& data, Project* project, bool withNewConsole);
//
//public:
//   virtual ReferenceMap::Iterator getExternalIt()
//   {
//      return _exportReferences.start();
//   }
//
//   virtual ref_t getEntryPoint()
//   {
//      return 0; // !! temporal
//   }
//
//   virtual ref_t getDebugEntryPoint()
//   {
//      return 0; // !! temporal
//   }
//
//   VirtualMachineClientImage(Project* project, _JITCompiler* compiler, bool guiMode = false);
//};

} // _ELENA_

#endif // imageH
