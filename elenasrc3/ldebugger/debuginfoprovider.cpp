//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the DebugInfoProviderBase class implementation
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "common.h"
// -------------------------------------------------------------------------
#include "engine/elenacommon.h"
#include "engine/module.h"
#include "debuginfoprovider.h"

#if (defined(_WIN32) || defined(__WIN32__))

#include <tchar.h>

#else

#define _T(x) x

#endif

using namespace elena_lang;

inline bool isSymbolReference(ustr_t name)
{
   return name.endsWith("#sym");
}

inline ref_t mapModuleReference(ModuleBase* module, ustr_t referenceName, bool existing)
{
   if (!isWeakReference(referenceName)) {
      return module->mapReference(referenceName + getlength(module->name()), existing);
   }
   else return module->mapReference(referenceName, existing);
}

// --- DebugInfoProviderBase ---

DebugInfoProviderBase::DebugInfoProviderBase()
   : _entryPoint(0), _modules(nullptr), _classes(INVALID_ADDR), _classNames(INVALID_ADDR)
{
}

bool DebugInfoProviderBase :: loadDebugInfo(path_t debuggee, DebugInfoProviderBase* provider, DebugProcessBase* process)
{
   PathString debugDataPath(debuggee);
   debugDataPath.changeExtension(_T("dn"));

   FileReader reader(*debugDataPath, FileRBMode, FileEncoding::Raw, false);
   if (!reader.isOpen())
      return false;

   char header[8];
   reader.read(header, 8);
   if (ustr_t(DEBUG_MODULE_SIGNATURE).compare(header, 5)) {
      provider->setDebugInfo(reader.getDWord(), INVALID_ADDR);

      if (!reader.eof()) {
         provider->load(reader, true, process);

         provider->setDebugInfoSize(reader.position());

         //loadSubjectInfo(reader);

         return true;
      }
   }
   else provider->setDebugInfoSize(4);

   return false;
}

bool DebugInfoProviderBase :: load(StreamReader& reader, bool setEntryAddress, DebugProcessBase* process)
{
   IdentifierString reference;
   if (setEntryAddress) {
      // read entry point
      reader.read(&_entryPoint, sizeof(_entryPoint));

      if (_entryPoint != 0)
         setEntryAddress = false;

      // skip project namespace
      reader.readString(reference);
   }

   while (!reader.eof()) {
      // read reference
      reader.readString(reference);

      // define the next record position
      pos_t size = reader.getPos() - 4;
      pos_t nextPosition = reader.position() + size;

      if (setEntryAddress) {
         pos_t pos = reader.position();

         // if entry address was not defined take the first one
         reader.read(&_entryPoint, sizeof(_entryPoint));

         reader.seek(pos);
         setEntryAddress = false;
      }

      //// if it is a VM temporal symbol - skip it
      //if (reference.compare(TAPE_SYMBOL)/* && _debugTape*/) {
      ////      loadTapeDebugInfo(reader, size);
      //}
      // otherwise load standard debug info
      /*else */loadSymbol(*reference, reader, process);

      reader.seek(nextPosition);
   }

   return true;
}


bool DebugInfoProviderBase :: loadSymbol(ustr_t reference, StreamReader& addressReader, DebugProcessBase* process)
{
   bool isClass = true;

   //bool isClass = true;
   ModuleBase* module = nullptr;
   // if symbol
   if (isSymbolReference(reference)) {
      module = loadDebugModule(reference);
      isClass = false;
   }
   else module = loadDebugModule(reference);

   pos_t position = (module != nullptr) ? mapModuleReference(module, reference, true) : 0;
   if (position != 0) {
      // place reader on the next after symbol record
      MemoryReader reader(module->mapSection(DEBUG_LINEINFO_ID | mskDataRef, true), position);
      MemoryReader stringReader(module->mapSection(DEBUG_STRINGS_ID | mskDataRef, true));

      // map vmt address for a class
      if (isClass) {
         addr_t vmtPtr = 0;
         addressReader.read(&vmtPtr, sizeof(addr_t));

         if (vmtPtr != 0) {
            _classes.add(vmtPtr, (addr_t)reader.address());
            _classNames.add(reference, vmtPtr);
            if (reference.find('@') != NOTFOUND_POS && reference.find('#', 0) > 0) {
               IdentifierString weakName("'$auto", reference + getlength(module->name()));

               _classNames.add(*weakName, vmtPtr);
            }
         }
      }
      // skip symbol entry address
      else {
         addr_t dummy = 0;
         addressReader.read(&dummy, sizeof(addr_t));
      }

      // start to read lineinfo until end symbol
      DebugLineInfo info;
      void* current = nullptr;
      int level = 1;
      while (level > 0 && !reader.eof()) {
         current = reader.address();

         reader.read(&info, sizeof(DebugLineInfo));
         switch (info.symbol) {
         case DebugSymbol::Procedure:
            level++;
            break;
         case DebugSymbol::End:
            level--;
            break;
         case DebugSymbol::Local:
         case DebugSymbol::LocalAddress:
         case DebugSymbol::IntLocalAddress:
         case DebugSymbol::UIntLocalAddress:
         case DebugSymbol::LongLocalAddress:
         case DebugSymbol::RealLocalAddress:
         case DebugSymbol::ByteArrayAddress:
         case DebugSymbol::ShortArrayAddress:
         case DebugSymbol::IntArrayAddress:
         case DebugSymbol::Parameter:
         case DebugSymbol::IntParameterAddress:
         case DebugSymbol::RealParameterAddress:
         case DebugSymbol::LongParameterAddress:
         case DebugSymbol::ParameterAddress:
         case DebugSymbol::ByteArrayParameter:
         case DebugSymbol::ShortArrayParameter:
         case DebugSymbol::IntArrayParameter:
         case DebugSymbol::RealArrayParameter:
            // replace field name reference with the name
            stringReader.seek((pos_t)info.addresses.local.nameRef);

            ((DebugLineInfo*)current)->addresses.local.nameRef = (addr_t)stringReader.address();
            break;
         case DebugSymbol::Field:
         case DebugSymbol::FieldAddress:
         case DebugSymbol::ParameterInfo:
         case DebugSymbol::FieldInfo:
         case DebugSymbol::LocalInfo:
            // replace field name reference with the name
            stringReader.seek((pos_t)info.addresses.source.nameRef);

            ((DebugLineInfo*)current)->addresses.source.nameRef = (addr_t)stringReader.address();
            break;
         case DebugSymbol::Breakpoint:
         case DebugSymbol::VirtualBreakpoint:
         {
            addr_t stepAddress = 0;
            addressReader.read(&stepAddress, sizeof(addr_t));

            ((DebugLineInfo*)current)->addresses.step.address = stepAddress;
            // virtual end of expression should be stepped over automatically by debugger
            if (info.symbol != DebugSymbol::VirtualBreakpoint)
               process->addStep(stepAddress, (void*)current);

            break;
         }
         default:
            break;
         }
      }
      return true;
   }
   else return false;
}

ModuleBase* DebugInfoProviderBase :: loadDebugModule(ustr_t reference)
{
   NamespaceString name(reference);
   PathString      path;

   // check if the module is already loaded
   auto it = _modules.start();
   while (!it.eof()) {
      if (NamespaceString::isIncluded((*it)->name(), reference)) {
         ustr_t properName = reference + (*it)->name().length();
         if ((*it)->mapReference(properName, true))
            return *it;
      }
      it++;
   }

   Module* module = nullptr;
   while (!module && !name.empty()) {
      retrievePath(*name, path, _T("dnl"));

      if (!_modules.exist(*name)) {
         module = new Module();

         FileReader reader(*path, FileRBMode, FileEncoding::Raw, false);
         LoadResult result = module->load(reader);
         if (result == LoadResult::Successful) {
            ustr_t relativeName = reference.str() + module->name().length();

            if (relativeName[0] == '#')
               relativeName = relativeName + 1;

            if (module->mapReference(relativeName, true) != 0) {
               _modules.add(*name, module);
            }
            else {
               delete module;
               module = nullptr;

               name.trimLastSubNs();
            }
         }
         else {
            delete module;
            module = nullptr;

            name.trimLastSubNs();
         }
      }
      else name.trimLastSubNs();

   }
   return module;
}

void DebugInfoProviderBase :: defineModulePath(ustr_t name, PathString& path, path_t projectPath, path_t outputPath, path_t extension)
{
   path.copy(projectPath);
   path.combine(outputPath);

   ReferenceName::nameToPath(path, name);
   path.appendExtension(extension);
}


DebugLineInfo* DebugInfoProviderBase :: seekDebugLineInfo(addr_t lineInfoAddress, IdentifierString& moduleName, ustr_t& sourcePath)
{
   ModuleBase* module = getDebugModule(lineInfoAddress);
   if (module) {
      moduleName.copy(module->name());

      DebugLineInfo* current = (DebugLineInfo*)lineInfoAddress;
      while (current->symbol != DebugSymbol::Procedure)
         current = &current[-1];

      if (current->addresses.source.nameRef != INVALID_POS) {
         MemoryBase* section = module->mapSection(DEBUG_STRINGS_ID, true);

         if (section != nullptr) {
            sourcePath = (const char*)section->get(current->addresses.source.nameRef);

            if (sourcePath.findLast('\'') != NOTFOUND_POS) {
               size_t index = sourcePath.findLast('\'');
               moduleName.copy(sourcePath, index);
               sourcePath = sourcePath + index + 1;
            }
         }
      }

      return (DebugLineInfo*)lineInfoAddress;
   }
   else return nullptr;
}

ModuleBase* DebugInfoProviderBase :: getDebugModule(addr_t address)
{
   ModuleMap::Iterator it = _modules.start();
   while (!it.eof()) {
      MemoryBase* section = (*it)->mapSection(DEBUG_LINEINFO_ID, true);
      if (section != nullptr) {
         addr_t starting = (addr_t)section->get(0);
         addr_t len = section->length();
         if (starting <= address && (address - starting) < len) {
            return *it;
         }
      }
      ++it;
   }
   return nullptr;
}
