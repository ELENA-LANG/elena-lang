//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the class implementing ELENA Engine Module class
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "module.h"
//#include "errors.h"

using namespace _ELENA_;

inline ref64_t encodeActionX(ref_t actionNameRef, ref_t signatureRef)
{
   ref64_t r = signatureRef;

   r = (r << 32) + actionNameRef;

   return r;
}

inline void decodeActionX(ref64_t r, ref_t& actionName, ref_t& signatureRef)
{
   actionName = r & 0xFFFFFF;

   signatureRef = (r >> 32);
}

// --- BaseModule ---

_BaseModule  :: _BaseModule()
   : _references(0), _actionNames(0), _actions(0ll), _constants(0)
{
}

ident_t _BaseModule :: resolveReference(ref_t reference)
{
   ident_t key = _resolvedReferences.get(reference);
   if (!key) {
      ReferenceMap::Iterator it = _references.start();
      key = goToKey(it, reference, DEFAULT_STR);

      _resolvedReferences.add(reference, key);
   }
   return key;
}

ident_t _BaseModule :: resolveAction(ref_t reference, ref_t& signature)
{
   ref64_t actionRef = _resolvedActions.get(reference);
   if (!actionRef) {
      actionRef = retrieveKey(_actions.start(), reference, 0ll);

      _resolvedActions.add(reference, actionRef);
   }

   ref_t nameRef = 0;
   decodeActionX(actionRef, nameRef, signature);

   ident_t key = _resolvedActionNames.get(nameRef);
   if (!key) {
      key = retrieveKey(_actionNames.start(), nameRef, DEFAULT_STR);

      _resolvedActionNames.add(nameRef, key);
   }

   return key;
}

size_t _BaseModule :: resolveSignature(ref_t signature, ref_t* references)
{
   ident_t key = _resolvedActionNames.get(signature);
   if (!key) {
      key = retrieveKey(_actionNames.start(), signature, DEFAULT_STR);

      _resolvedActionNames.add(signature, key);
   }
   if (!emptystr(key)) {
      size_t len = getlength(key) >> 3;
      String<char, 9> tmp;
      for (size_t i = 0; i < len; i++) {
         tmp.copy(key.c_str() + (i << 3), 8);

         references[i] = ident_t(tmp).toULong(16);
      }

      return len;
   }
   else return 0;
}

ident_t _BaseModule :: resolveConstant(ref_t reference)
{
   return retrieveKey(_constants.start(), reference, DEFAULT_STR);
}

ref_t _BaseModule :: retrieveSignature(ref_t* references, size_t length, bool existing)
{
   String<char, 256> signatureStr;
   String<char, 9> tmp;
   for (size_t i = 0; i < length; i++) {
      tmp.copyHex(references[i]);

      size_t filling = 8 - getlength(tmp);
      for(size_t j = 0 ; j < filling ; j++)
         signatureStr.append('0');

      signatureStr.append(tmp);
   }
   if (existing) {
      return _actionNames.get(signatureStr.c_str());
   }
   else {
      ref_t nextNameId = _actionNames.Count() + 1;
      ref_t nameId = mapKey(_actionNames, signatureStr.c_str(), nextNameId);

      // if we added new message, clear resolved message cache (due to possible string relocation)
      if (nameId == nextNameId)
         _resolvedActionNames.clear();

      return nameId;
   }
}

// --- Module ---

Module :: Module()
   : _sections(NULL, freeobj)
{
}

Module :: Module(ident_t name)
   : _name(name), _sections(NULL, freeobj)
{
}

void Module :: mapPredefinedReference(ident_t name, ref_t reference)
{
   _resolvedReferences.clear();

   _references.add(name, reference);
}

void Module :: mapPredefinedAction(ident_t actionName, ref_t reference, ref_t signature)
{
   _resolvedActions.clear();

   ref_t nextNameId = _actionNames.Count() + 1;
   ref_t nameId = mapKey(_actionNames, actionName, nextNameId);

   // if we added new message, clear resolved message cache (due to possible string relocation)
   if (nameId == nextNameId)
      _resolvedActionNames.clear();

   _actions.add(encodeActionX(nameId, signature), reference);
}

ref_t Module :: mapReference(ident_t reference)
{
   ref_t nextId = _references.Count() + 1;

   // generate an exception if reference id is out of range
   if (nextId > ~mskAnyRef)
      throw InternalError("Reference overflow");

   ref_t refId = mapKey(_references, reference, nextId);

   // if we added new reference, clear resolved reference cache (due to possible string relocation)
   if (refId == nextId)
      _resolvedReferences.clear();

   return refId;
}

ref_t Module :: mapAction(ident_t actionName, ref_t signature, bool existing)
{
   if (existing) {
      ref_t actionNameId = _actionNames.get(actionName);

      return _actions.get(encodeActionX(actionNameId, signature));
   }
   else {
      ref_t nextNameId = _actionNames.Count() + 1;
      ref_t nameId = mapKey(_actionNames, actionName, nextNameId);

      // if we added new message, clear resolved message cache (due to possible string relocation)
      if (nameId == nextNameId)
         _resolvedActionNames.clear();

      ref_t nextId = _actions.Count() + 1;
      ref_t refId = mapKey(_actions, encodeActionX(nameId, signature), nextId);

      return refId;
   }
}

ref_t Module :: mapSignature(ref_t* references, size_t length, bool existing)
{
   return retrieveSignature(references, length, existing);
}

ref_t Module :: mapConstant(ident_t constant)
{
   ref_t nextId = _constants.Count() + 1;

   if (!constant) {
      return mapKey(_constants, "", nextId);
   }
   else return mapKey(_constants, constant, nextId);
}

ref_t Module :: mapReference(ident_t reference, bool existing)
{
   if (existing) {
      return _references.get(reference);
   }
   else return mapReference(reference);
}

Section* Module :: mapSection(ref_t reference, bool existing)
{
   Section* section = _sections.get(reference);
   if (!existing && section==NULL) {
      section = new Section();

      _sections.add(reference, section);
   }
   return section;
}

void Module :: loadSections(StreamReader& reader)
{
   int totalSize = reader.getDWord();

   while (totalSize > 0) {
      ref_t key = reader.getDWord();

      Section* section = _readSection(&reader);

      _sections.add(key, section);

      totalSize -= section->Size() + 8;
   }
}

void Module :: saveSections(StreamWriter& writer)
{
   pos_t totalSize = 0;

   // calculate the total section size
   SectionMap::Iterator it = _sections.start();
   while (!it.Eof()) {
      totalSize += (*it)->Size() + 8;

      it++;
   }

   // save total size
   writer.writeDWord(totalSize);

   // save sections
   it = _sections.start();
   while (!it.Eof()) {
      _writeIterator(&writer, it.key(), *it);

      it++;
   }
}

LoadResult Module :: load(StreamReader& reader)
{
   if (reader.Eof())
      return lrNotFound;

   // load signature...
   char signature[12];
   reader.read(signature, (pos_t)strlen(MODULE_SIGNATURE));
   if (strncmp(signature, MODULE_SIGNATURE, strlen(MODULE_SIGNATURE)) != 0) {
      return (strncmp(signature, ELENA_SIGNITURE, 6) == 0) ? lrWrongVersion : lrWrongStructure;
   }

   // load name...
   reader.readString(_name);

   // load references...
   _references.read(&reader);

   // load action names...
   _actionNames.read(&reader);

   // load actions...
   _actions.read(&reader);

   // load constants...
   _constants.read(&reader);

   // load sections..
   loadSections(reader);

   return lrSuccessful;
}

bool Module :: save(StreamWriter& writer)
{
   if (!writer.isOpened())
      return false;

   // save signature...
   writer.write(MODULE_SIGNATURE, (pos_t)strlen(MODULE_SIGNATURE));

   // save name...
   writer.writeLiteral(_name, getlength(_name) + 1);

   // save references...
   _references.write(&writer);

   // save action names...
   _actionNames.write(&writer);

   // save actions...
   _actions.write(&writer);

   // save constants...
   _constants.write(&writer);

   // save sections..
   saveSections(writer);

   return true;
}

// --- ROModule ---

ROModule :: ROModule(StreamReader& reader, LoadResult& result)
   : _sections(ROModule::ROSection())
{
   if (reader.Eof()) {
      result = lrNotFound;
      return;
   }

   // load signature...
   char signature[12];
   reader.read(signature, (pos_t)strlen(MODULE_SIGNATURE));
   if (strncmp(signature, ELENA_SIGNITURE, strlen(ELENA_SIGNITURE)) != 0) {
      result = (strncmp(signature, ELENA_SIGNITURE, 6) == 0) ? lrWrongVersion : lrWrongStructure;
      return;
   }

   // load name...
   reader.readString(_name);

   // load references...
   _references.read(&reader);

   // load action names...
   _actionNames.read(&reader);

   // load actions...
   _actions.read(&reader);

   // load constants...
   _constants.read(&reader);

   // load sections..
   loadSections(reader);

   result = lrSuccessful;
}

void ROModule :: loadSections(StreamReader& reader)
{
   int totalSize = reader.getDWord();

   // load section bodies
   _sectionDump.load(&reader, totalSize);

   // create section map
   int position = 0;
   int length = _sectionDump.Length();
   while (position < length) {
      // add section object
      _sections.add(_sectionDump[position], ROSection((char*)_sectionDump.get(position + 4)));

      // skip key
      position += 4;
      // skip section + section size field
      position += _sectionDump[position] + 4;
      // skip section relocation map + relocation map count field
      position += (_sectionDump[position] << 3) + 4;
   }
}

ref_t ROModule :: mapReference(ident_t reference)
{
   return _references.get(reference);
}

ref_t ROModule :: mapReference(ident_t reference, bool existing)
{
   if (!existing) {
      throw InternalError("Read-only Module");
   }
   else return _references.get(reference);
}

ref_t ROModule :: mapAction(ident_t actionName, ref_t signature, bool existing)
{
   if (existing) {
      ref_t actionNameId = _actionNames.get(actionName);

      return _actions.get(encodeActionX(actionNameId, signature));
   }
   else throw InternalError("Read-only Module");
}

ref_t ROModule :: mapSignature(ref_t* references, size_t length, bool existing)
{
   if (existing) {
      return retrieveSignature(references, length, existing);
   }
   else throw InternalError("Read-only Module");
}

ref_t ROModule :: mapConstant(ident_t reference)
{
   return _constants.get(reference);
}

_Memory* ROModule :: mapSection(ref_t reference, bool existing)
{
   if (!existing)
      throw InternalError("Read-only Module");

   return _sections.getPtr(reference);
}
