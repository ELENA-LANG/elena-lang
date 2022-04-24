//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This header contains the declaration of ELENA Engine Data Section
//		classes.
//                                                  (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef SECTION_H
#define SECTION_H

namespace elena_lang
{

   // --- Section mapping types ---
   class Section;
   typedef Map<ref_t, Section*, nullptr, nullptr, freeobj> SectionMap;

   typedef FixedMemoryMap<ref_t, pos_t>                    RelocationMap;

   // --- Section ---
#pragma pack(push, 1)
   struct RelocationEntry
   {
      ref_t reference;
      pos_t offset;
   };
#pragma pack(pop)

   class Section : public MemoryDump
   {
      RelocationMap _references;

   public:
      static Section* readSection(StreamReader& reader);
      static void writeSection(Section* section, StreamWriter& writer);

      void insert(pos_t position, const void* s, pos_t length) override;

      pos_t sizeInMemory()
      {
         return length() + _references.sizeInMemory() + (pos_t)sizeof(pos_t);
      }

      bool addReference(ref_t reference, pos_t position) override
      {
         _references.add(reference, position);

         return true;
      }

      void* getReferences() const override
      {
         return _references.Address();
      }

      template<class ArgT> void fixupReferences(ArgT arg, 
         void(relocate)(pos_t, ref_t, ref_t, void*, ArgT arg))
      {
         for(auto it = _references.start(); !it.eof(); ++it) {
            pos_t pos = *it;
            ref_t ref = it.key();

            relocate(pos, ref & mskAnyRef, ref & ~mskAnyRef, (char*)_buffer + pos, arg);
         }
      }

      Section()
         : MemoryDump(), _references(0)
      {         
      }
   };
}

#endif // SECTION_H