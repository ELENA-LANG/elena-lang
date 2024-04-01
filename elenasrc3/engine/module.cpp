//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the class implementing ELENA Engine Module class
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "module.h"
#include "langcommon.h"

using namespace elena_lang;

// --- AbstractModule ---

ustr_t AbstractModule :: resolveReference(ref_t reference)
{
   ustr_t key = _resolvedReferences.get(reference);
   if (key.empty()) {
      key = _references.retrieve<ref_t>(nullptr, reference, [](ref_t reference, ustr_t key, ref_t current)
         {
            return current == reference;
         });

      _resolvedReferences.add(reference, key);
   }
   return key;
}

ref_t AbstractModule :: retrieveSignature(ref_t* references, size_t length, bool existing)
{
   String<char, 256> signatureStr;
   String<char, 9> tmp;
   for(size_t i = 0; i < length; i++) {
      tmp.clear();
      tmp.appendUInt(references[i], 16);

      size_t filling = 8 - tmp.length();
      for (size_t j = 0; j < filling; j++)
         signatureStr.append('0');

      signatureStr.append(tmp.str());
   }
   if (existing) {
      return _actionNames.get(signatureStr.str());
   }
   else {
      ref_t nextNameId = _actionNames.count() + 1;
      ref_t nameId = mapKey(_actionNames, signatureStr.str(), nextNameId);

      // if we added new message, clear resolved message cache (due to possible string relocation)
      if (nameId == nextNameId)
         _resolvedActionNames.clear();

      return nameId;
   }
}

ustr_t AbstractModule :: resolveConstant(ref_t reference)
{
   return _constants.retrieve<ref_t>(nullptr, reference, [](ref_t reference, ustr_t key, ref_t current)
      {
         return current == reference;
      });
}

ustr_t AbstractModule :: resolveAction(ref_t reference, ref_t& signature)
{
   ref64_t actionRef = _resolvedActions.get(reference);
   if (!actionRef) {
      actionRef = _actions.retrieve<ref_t>(0ll, reference, [](ref_t reference, ref64_t key, ref_t current)
         {
            return current == reference;
         });

      _resolvedActions.add(reference, actionRef);
   }

   ref_t nameRef = 0;
   decodeAction64(actionRef, nameRef, signature);

   ustr_t key = _resolvedActionNames.get(nameRef);
   if (!key) {
      key = _actionNames.retrieve<ref_t>(DEFAULT_STR, nameRef, [](ref_t reference, ustr_t key, ref_t current)
         {
            return current == reference;
         });

      _resolvedActions.add(reference, actionRef);
   }

   return key;
}

size_t AbstractModule :: resolveSignature(ref_t signature, ref_t* references)
{
   ustr_t key = _resolvedActionNames.get(signature);
   if (!key) {
      key = _actionNames.retrieve<ref_t>(DEFAULT_STR, signature, [](ref_t reference, ustr_t key, ref_t current)
         {
            return current == reference;
         });

      _resolvedActionNames.add(signature, key);
   }
   if (!key.empty()) {
      size_t len = key.length() >> 3;
      String<char, 9> tmp;
      for (size_t i = 0; i < len; i++) {
         tmp.copy(key.str() + (i << 3), 8);

         references[i] = StrConvertor::toUInt(tmp.str(), 16);
      }

      return len;
   }
   else return 0;
}

void AbstractModule :: forEachReference(void* arg, void(*lambda)(ModuleBase*, ref_t, void*))
{
   for (auto it = _references.start(); !it.eof(); ++it) {
      lambda(this, *it, arg);
   }
}

// --- Module ---

Module :: Module(ustr_t name)
   : _sections(nullptr)
{
   _name.copy(name);
}

ref_t Module :: mapReference(ustr_t referenceName)
{
   ref_t nextId = _references.count() + 1;

#ifdef _DEBUG
   // generate an exception if reference id is out of range
   if (nextId > ~mskAnyRef)
      throw InternalError(errReferenceOverflow);
#endif

   ref_t reference = mapKey(_references, referenceName, nextId);

   // if we added new reference, clear resolved reference cache (due to possible string relocation)
   if (reference == nextId)
      _resolvedReferences.clear();

   return reference;
}

ref_t Module :: mapReference(ustr_t referenceName, bool existing)
{
   if (existing) {
      return _references.get(referenceName);
   }
   else return mapReference(referenceName);
}

void Module :: mapPredefinedReference(ustr_t referenceName, ref_t reference)
{
   _resolvedReferences.clear();

   _references.add(referenceName, reference);
}

ref_t Module::mapSignature(ref_t* references, size_t length, bool existing)
{
   return retrieveSignature(references, length, existing);
}

ref_t Module :: mapAction(ustr_t actionName, ref_t signature, bool existing)
{
   if (existing) {
      ref_t actionNameId = _actionNames.get(actionName);

      return _actions.get(encodeAction64(actionNameId, signature));
   }
   else {
      ref_t nextNameId = _actionNames.count() + 1;
      ref_t nameId = mapKey(_actionNames, actionName, nextNameId);

      // if we added new message, clear resolved message cache (due to possible string relocation)
      if (nameId == nextNameId)
         _resolvedActionNames.clear();

      ref_t nextId = _actions.count() + 1;
      ref_t refId = mapKey(_actions, encodeAction64(nameId, signature), nextId);

      return refId;
   }
}

ref_t Module :: mapConstant(ustr_t constant)
{
   ref_t nextId = _constants.count() + 1;

   if (!constant) {
      return mapKey(_constants, "", nextId);
   }
   else return mapKey(_constants, constant, nextId);
}

MemoryBase* Module :: mapSection(ref_t reference, bool existing)
{
   Section* section = _sections.get(reference);
   if (!existing && section == nullptr) {
      section = new Section();

      _sections.add(reference, section);
   }

   return section;
}

void Module :: loadSections(StreamReader& reader)
{
   pos_t totalSize = reader.getPos();

   while (totalSize != 0) {
      ref_t key = reader.getRef();

      Section* section = Section::readSection(reader);

      _sections.add(key, section);

      totalSize -= section->sizeInMemory() + sizeof(ref_t);
   }
}

void Module :: saveSections(StreamWriter& writer)
{
   pos_t totalSize = _sections.sum<pos_t>(0,
      [](Section* section) { return section->sizeInMemory() + (pos_t)sizeof(ref_t); });

   // save total size
   writer.writePos(totalSize);

   _sections.forEach<StreamWriter*>(&writer, [](StreamWriter* output, ref_t reference, Section* section)
   {
      output->writeRef(reference);

      Section::writeSection(section, *output);
   });
}

LoadResult Module::load(StreamReader& reader)
{
   if (reader.eof())
      return LoadResult::NotFound;

   // load signature...
   char signature[12];
   reader.read(signature, getlength_pos(MODULE_SIGNATURE));
   signature[getlength(MODULE_SIGNATURE)] = 0;
   if (strncmp(signature, MODULE_SIGNATURE, strlen(MODULE_SIGNATURE)) != 0) {
      return (strncmp(signature, ELENA_SIGNITURE, 6) == 0) ? LoadResult::WrongVersion : LoadResult::WrongStructure;
   }

   // load name...
   reader.readString(_name);

   // load references
   MapHelper::readStringMap<ReferenceMap, ref_t>(&reader, _references);

   // load action names
   MapHelper::readStringMap<ReferenceMap, ref_t>(&reader, _actionNames);

   // load actions...
   MapHelper::readReferenceMap64<ActionMap, ref_t>(&reader, _actions);

   // load constants
   MapHelper::readStringMap<ReferenceMap, ref_t>(&reader, _constants);

   // load sections..
   loadSections(reader);

   return LoadResult::Successful;
}

bool Module :: save(StreamWriter& writer)
{
   if (!writer.isOpen())
      return false;

   // save signature...
   writer.write(MODULE_SIGNATURE, getlength_pos(MODULE_SIGNATURE));

   // save name...
   writer.writeString(*_name, _name.length_pos() + 1);

   // save references...
   MapHelper::writeRelocationMap<ReferenceMap, ref_t>(&writer, _references);

   // save action names...
   MapHelper::writeRelocationMap<ReferenceMap, ref_t>(&writer, _actionNames);

   // save actions...
   MapHelper::writeReferenceMap64<ActionMap, ref_t>(&writer, _actions);

   // save constants...
   MapHelper::writeRelocationMap<ReferenceMap, ref_t>(&writer, _constants);

   // save sections..
   saveSections(writer);

   return true;
}

// --- ROModule::ROSection ---

bool ROModule::ROSection :: write(pos_t position, const void* s, pos_t length)
{
   // should never be called
   throw InternalError(errReadOnlyModule);
}

void ROModule::ROSection :: trim(pos_t)
{
   // should never be called
   throw InternalError(errReadOnlyModule);
}

bool ROModule::ROSection :: insert(pos_t position, const void* s, pos_t length)
{
   // should never be called
   throw InternalError(errReadOnlyModule);
}

void* ROModule::ROSection :: getReferences() const
{
   pos_t position = *(pos_t*)_buffer;

   return (void*)((char*)_buffer + position + sizeof(pos_t));
}

// --- ROModule ---

ROModule::ROModule(StreamReader& reader, LoadResult& result)
   : _sections(ROSection())
{
   if (reader.eof()) {
      result = LoadResult::NotFound;
      return;
   }

   // load signature...
   char signature[12];
   reader.read(signature, getlength_pos(MODULE_SIGNATURE));
   signature[getlength(MODULE_SIGNATURE)] = 0;
   if (strncmp(signature, ELENA_SIGNITURE, strlen(ELENA_SIGNITURE)) != 0) {
      result = (strncmp(signature, ELENA_SIGNITURE, 6) == 0) ? LoadResult::WrongVersion : LoadResult::WrongStructure;
      return;
   }

   // load name...
   reader.readString(_name);

   // load references
   MapHelper::readStringMap<ReferenceMap, ref_t>(&reader, _references);

   // load action names
   MapHelper::readStringMap<ReferenceMap, ref_t>(&reader, _actionNames);

   // load actions...
   MapHelper::readReferenceMap64<ActionMap, ref_t>(&reader, _actions);

   // load constants
   MapHelper::readStringMap<ReferenceMap, ref_t>(&reader, _constants);

   // load sections..
   loadSections(reader);

   result = LoadResult::Successful;
}

void ROModule :: loadSections(StreamReader& reader)
{
   pos_t totalSize = reader.getPos();

   // load section bodies
   _sectionDump.load(reader, totalSize);

   // create section map
   int position = 0;
   int length = _sectionDump.length();
   while (position < length) {
      // add section object
      _sections.add(_sectionDump.getRef(position), ROSection(_sectionDump.get(position + sizeof(pos_t))));

      // skip key
      position += sizeof(ref_t);
      // skip section + section size field
      position += _sectionDump.getPos(position) + sizeof(pos_t);
      // skip section relocation map + relocation map count field
      position += (_sectionDump.getPos(position) * sizeof(RelocationEntry)) + sizeof(pos_t);
   }
}

ref_t ROModule :: mapReference(ustr_t referenceName)
{
   return _references.get(referenceName);
}

ref_t ROModule :: mapReference(ustr_t referenceName, bool existing)
{
   if (!existing) {
      throw InternalError(errReadOnlyModule);
   }
   else return _references.get(referenceName);
}

void ROModule :: mapPredefinedReference(ustr_t referenceName, ref_t reference)
{
   throw InternalError(errReadOnlyModule);
}

ref_t ROModule :: mapAction(ustr_t actionName, ref_t signature, bool existing)
{
   if (existing) {
      ref_t actionNameId = _actionNames.get(actionName);

      return _actions.get(encodeAction64(actionNameId, signature));
   }
   else throw InternalError(errReadOnlyModule);
}

ref_t ROModule :: mapSignature(ref_t* references, size_t length, bool existing)
{
   if (existing) {
      return retrieveSignature(references, length, existing);
   }
   else throw InternalError(errReadOnlyModule);
}

ref_t ROModule :: mapConstant(ustr_t reference)
{
   return _constants.get(reference);
}

MemoryBase* ROModule :: mapSection(ref_t reference, bool existing)
{
   if (!existing)
      throw InternalError(errReadOnlyModule);

   return _sections.getPtr(reference);
}

