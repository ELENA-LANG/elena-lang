//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This header contains the declaration of the class implementing
//      ELENA Engine Module class
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef moduleH
#define moduleH 1

namespace _ELENA_
{

// --- BaseModule class ---

class _BaseModule  : public _Module
{
protected:
   typedef Cache<ref_t, ident_t, 20> ResolvedMap;
   typedef Cache<ref_t, ref64_t, 20> ResolvedAMap;

   ReferenceMap  _references;
   ReferenceMap  _actionNames;
   ActionMap     _actions;

   ReferenceMap  _constants;

   ResolvedMap   _resolvedReferences;
   ResolvedMap   _resolvedActionNames;
   ResolvedAMap  _resolvedActions;

   ref_t retrieveSignature(ref_t* references, size_t length, bool existing);

public:
   virtual ident_t resolveReference(ref_t reference);
   virtual ident_t resolveAction(ref_t reference, ref_t& signature);
   virtual size_t resolveSignature(ref_t signature, ref_t* references);
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
   virtual void mapPredefinedAction(ident_t name, ref_t reference, ref_t signature);

   virtual ref_t mapReference(ident_t reference);
   virtual ref_t mapReference(ident_t reference, bool existing);

   virtual ref_t mapAction(ident_t actionName, ref_t signature, bool existing);
   virtual ref_t mapSignature(ref_t* references, size_t length, bool existing);
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
      virtual pos_t Length() const { return *(pos_t*)_buffer; }

      virtual void* get(pos_t position) const { return (void*)(_buffer + position + 4); }

      //virtual void* getLong(pos64_t position) const 
      //{ 
      //   if (position < INT_MAX) {
      //      return get((pos_t)position);
      //   }
      //   else return NULL;
      //}

      virtual bool read(pos_t position, void* s, pos_t length)
      {
         memcpy(s, _buffer + position + 4, length);

         return true;
      }

      virtual bool readLong(pos64_t position, void* s, pos64_t length)
      {
         if (position < INT_MAX && length < INT_MAX) {
            return read((pos_t)position, s, (pos_t)length);
         }
         else return false;
      }

      virtual bool write(pos_t, const void*, pos_t)
      {
         // should never be called
         throw InternalError("Read-only Module");
      }

      virtual void insert(pos_t, const void*, pos_t)
      {
         // should never be called
         throw InternalError("Read-only Module");
      }

      virtual bool writeBytes(pos_t, char, pos_t)
      {
         // should never be called
         throw InternalError("Read-only Module");
      }

      virtual void* getReferences()
      {
         int position = *(int*)_buffer;

         return (void*)(_buffer + position + 4);
      }

      virtual void trim(pos_t)
      {
         // should never be called
         throw InternalError("Read-only Module");
      }
      virtual void trimLong(pos64_t)
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
         _buffer = nullptr;
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

   virtual ref_t mapAction(ident_t actionName, ref_t signature, bool existing);
   virtual ref_t mapSignature(ref_t* references, size_t length, bool existing);
   virtual ref_t mapConstant(ident_t reference);

   virtual void mapPredefinedReference(ident_t, ref_t)
   {
      throw InternalError("Read-only Module");
   }

   virtual void mapPredefinedAction(ident_t, ref_t, ref_t)
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
