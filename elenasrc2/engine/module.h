//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This header contains the declaration of the class implementing
//      ELENA Engine Module class
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef moduleH
#define moduleH 1

namespace _ELENA_
{

// --- BaseModule class ---

class _BaseModule  : public _Module
{
protected:
   typedef Cache<ref_t, ident_t, 20> ResolveMap;

   ReferenceMap _references;
   ReferenceMap _subjects;
   ReferenceMap _constants;

   ResolveMap   _resolvedReferences;
   ResolveMap   _resolvedSubjects;

public:
   virtual ident_t resolveReference(ref_t reference);
   virtual ident_t resolveSubject(ref_t reference);
   virtual ident_t resolveConstant(ref_t reference);

   _BaseModule ();
};

// --- Module class ---

class Module : public _BaseModule
{
   IdentifierString _name;

   SectionMap   _sections;

   void loadSections(StreamReader& reader);
   void saveSections(StreamWriter& writer);

public:
   virtual ident_t Name() const { return _name; }

   ReferenceMap::Iterator References() { return _references.start(); }

   virtual void mapPredefinedReference(ident_t name, ref_t reference);

   virtual ref_t mapReference(ident_t reference);
   virtual ref_t mapReference(ident_t reference, bool existing);

   virtual ref_t mapSubject(ident_t message, bool existing);
   virtual ref_t mapConstant(ident_t constant);

   virtual Section* mapSection(ref_t reference, bool existing);

   virtual LoadResult load(StreamReader& reader);
   virtual bool save(StreamWriter& writer);

   Module();
   Module(ident_t name);
};

// --- ROModule ---

class ROModule : public _BaseModule
{
public:
   // ROSection
   class ROSection : public _Memory
   {
      friend class ROModule;

      const char* _buffer;

      ROSection(const char* buffer)
      {
         _buffer = buffer;
      }

   public:
      virtual size_t Length() const { return *(size_t*)_buffer; }

      virtual void* get(size_t position) const { return (void*)(_buffer + position + 4); }

      virtual bool read(size_t position, void* s, size_t length)
      {
         memcpy(s, _buffer + position + 4, length);

         return true;
      }

      virtual bool write(size_t, const void*, size_t)
      {
         // should never be called
         throw InternalError("Read-only Module");
      }

      virtual void insert(size_t, const void*, size_t)
      {
         // should never be called
         throw InternalError("Read-only Module");
      }

      virtual bool writeBytes(size_t, char, size_t)
      {
         // should never be called
         throw InternalError("Read-only Module");
      }

      virtual void* getReferences()
      {
         int position = *(int*)_buffer;

         return (void*)(_buffer + position + 4);
      }

      virtual void trim(size_t)
      {
         // should never be called
         throw InternalError("Read-only Module");
      }

      ROSection& operator =(const ROSection& section)
      {
         this->_buffer = section._buffer;

         return *this;
      }

      ROSection()
      {
         _buffer = NULL;
      }
   };

   typedef IntFixedMap<ROSection> ROSectionMap;

private:
   IdentifierString _name;

   MemoryDump   _sectionDump;
   ROSectionMap _sections;

   void loadSections(StreamReader& reader);

public:
   virtual ident_t Name() const { return _name; }

   virtual ref_t mapReference(ident_t reference);
   virtual ref_t mapReference(ident_t reference, bool existing);

   virtual ref_t mapSubject(ident_t reference, bool existing);
   virtual ref_t mapConstant(ident_t reference);

   virtual void mapPredefinedReference(ident_t, ref_t)
   {
      throw InternalError("Read-only Module");
   }

   virtual _Memory* mapSection(ref_t reference, bool existing);

   virtual bool save(StreamWriter&)
   {
      return false; // Read-Only module should not save anything
   }

   ROModule(StreamReader& reader, LoadResult& result);
};

} // _ELENA_

#endif // moduleH
