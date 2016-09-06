//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the class implementing ELENA Engine Module class
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "module.h"
//#include "errors.h"

using namespace _ELENA_;

// --- BaseModule ---

_BaseModule  :: _BaseModule()
   : _references(0), _subjects(0), _constants(0)
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

ident_t _BaseModule :: resolveSubject(ref_t reference)
{
   ident_t key = _resolvedSubjects.get(reference);
   if (!key) {
      key = retrieveKey(_subjects.start(), reference, DEFAULT_STR);

      _resolvedSubjects.add(reference, key);
   }
   return key;
}

ident_t _BaseModule :: resolveConstant(ref_t reference)
{
   return retrieveKey(_constants.start(), reference, DEFAULT_STR);
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

ref_t Module :: mapReference(ident_t reference)
{
   size_t nextId = _references.Count() + 1;

   // generate an exception if reference id is out of range
   if (nextId > ~mskAnyRef)
      throw InternalError("Reference overflow");

   ref_t refId = mapKey(_references, reference, nextId);

   // if we added new reference, clear resolved reference cache (due to possible string relocation)
   if (refId == nextId)
      _resolvedReferences.clear();

   return refId;
}

ref_t Module :: mapSubject(ident_t subject, bool existing)
{
   if (existing)
      return _subjects.get(subject);
   else {
      size_t nextId = _subjects.Count() + 1;

      ref_t refId = mapKey(_subjects, subject, nextId);

      // if we added new message, clear resolved message cache (due to possible string relocation)
      if (refId == nextId)
         _resolvedSubjects.clear();

      return refId;
   }
}

ref_t Module :: mapConstant(ident_t constant)
{
   size_t nextId = _constants.Count() + 1;

   return mapKey(_constants, constant, nextId);
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
   int totalSize = 0;

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
   reader.read(signature, strlen(MODULE_SIGNATURE));
   if (strncmp(signature, ELENA_SIGNITURE, strlen(ELENA_SIGNITURE)) != 0) {
      return (strncmp(signature, ELENA_SIGNITURE, 6) == 0) ? lrWrongVersion : lrWrongStructure;
   }

   // load name...
   reader.readString(_name);

   // load references...
   _references.read(&reader);

   // load subjects...
   _subjects.read(&reader);

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
   writer.write(MODULE_SIGNATURE, strlen(MODULE_SIGNATURE));

   // save name...
   writer.writeLiteral(_name, getlength(_name) + 1);

   // save references...
   _references.write(&writer);

   // save subjects...
   _subjects.write(&writer);

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
   reader.read(signature, strlen(MODULE_SIGNATURE));
   if (strncmp(signature, ELENA_SIGNITURE, strlen(ELENA_SIGNITURE)) != 0) {
      result = (strncmp(signature, ELENA_SIGNITURE, 6) == 0) ? lrWrongVersion : lrWrongStructure;
      return;
   }

   // load name...
   reader.readString(_name);

   // load references...
   _references.read(&reader);

   // load subjects...
   _subjects.read(&reader);

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

ref_t ROModule :: mapSubject(ident_t reference, bool existing)
{
   if (!existing) {
      throw InternalError("Read-only Module");
   }
   else return _subjects.get(reference);
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
