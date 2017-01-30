//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the implementation of ELENA Engine Data Section
//		classes.
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "section.h"

using namespace _ELENA_;

// --- Section ---

Section* _ELENA_::_readSection(StreamReader* reader)
{
   Section* section = new Section();

   pos_t length = reader->getDWord();
   section->load(reader, length);

   section->_references.read(reader);

   return section;
}

void _ELENA_::_readToMap(StreamReader* reader, SectionMap* map, pos_t counter, ref_t& key, Section* section)
{
   while (counter > 0) {
      reader->readDWord(key);

      section = _readSection(reader);

      map->add(key, section);
      counter--;
   }
}

void Section :: insert(pos_t position, const void* s, pos_t length)
{
   MemoryDump::insert(position, s, length);

   ::shift(_references.start(), position, length);
}

//void Section :: fixupReferences(RelocationFixMap& fixupTable, int base, bool relative)
//{
//   RelocationMap::Iterator it = _references.start();
//   while (!it.Eof()) {
//      if (fixupTable.exist(it.key())) {
//         if (relative && test(it.key(), mskRelativeRef)) {
//            *((int*)(_buffer + *it)) = fixupTable.get(it.key()) - (base + *it + 4);
//         }
//         if (!relative && !test(it.key(), mskRelativeRef)) {
//            *((int*)(_buffer + *it)) += (base + fixupTable.get(it.key()));
//         }
//      }
//      it++;
//   }
//}

void Section :: fixupReferences(void* param, ref_t(realloc)(ref_t, ref_t, ref_t, void* param))
{
   RelocationMap::Iterator it = _references.start();
   while (!it.Eof()) {
      pos_t pos = *it;

      *((int*)(_buffer + pos)) = realloc(pos, it.key(), *((int*)(_buffer + pos)), param);
//      if ((key & mskSectionMask) == mask) {
//         ref_t address = realloc(key);
//         if (test(key, mskRelativeRef)) {
//            *((int*)(_buffer + pos)) = address - pos - 4;
//         }
//         else *((int*)(_buffer + pos)) += (base + address);
//      }

      it++;
   }
}

// --- Section Fixup Hash function ---

ref_t _ELENA_::indexReference(ref_t reference)
{
   return ((reference && ~mskAnyRef) >> 2);
}
