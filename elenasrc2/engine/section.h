//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This header contains the declaration of ELENA Engine Data Section
//		classes.
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef SectionH
#define SectionH 1

namespace _ELENA_
{

// --- Section Fixup Hash function ---
ref_t indexReference(ref_t reference);

// --- Section Fixup map ---
typedef MemoryHashTable<ref_t, int, indexReference, cnHashSize> RelocationFixMap;

// --- Section mapping types ---
class Section;
typedef Map<ref_t, Section*> SectionMap;

// --- Section class ---
typedef IntFixedMap<ref_t> RelocationMap;

class Section : public MemoryDump
{
   RelocationMap _references;

public:
   RelocationMap::Iterator References() { return _references.start(); }

   virtual void* getReferences()
   {
      return _references.Array();
   }

   pos_t Size() const
   {
      return Length() + _references.Size();
   }

   virtual void insert(pos_t position, const void* s, pos_t length);

   virtual bool addReference(ref_t reference, pos_t position)
   {
      _references.add(reference, position);

      return true;
   }

   // realloc parameters: position, key, disp, param
   void fixupReferences(void* param, ref_t(realloc)(ref_t, ref_t, ref_t, void* param));

   friend Section* _readSection(StreamReader* reader);

   friend void _readToMap(StreamReader* reader, SectionMap* map, pos_t counter, ref_t& key, Section* section);

   friend void _writeIterator(StreamWriter* writer, int key, Section* section)
   {
      writer->writeDWord(key);
      writer->writeDWord(section->Length());

      MemoryReader reader(section);
      writer->read(&reader, section->Length());

      section->_references.write(writer);
   }

   virtual void clear()
   {
      MemoryDump::clear();
      _references.clear();
   }

   Section()
   {
   }

   Section(int capacity)
      : MemoryDump(capacity)
   {
   }
};

// --- section friend functions ---
Section* _readSection(StreamReader* reader);

} // _ELENA_

#endif	// SectionH
