//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This header contains the declaration of the class implementing
//      ELENA Engine Module class
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MODULE_H
#define MODULE_H

#include "elena.h"

namespace elena_lang
{
   // --- AbstractModule ---
   class AbstractModule : public ModuleBase
   {
   protected:
      typedef Cache<ref_t, ustr_t, 20>  ResolvedMap;
      typedef Cache<ref_t, ref64_t, 20> ResolvedAMap;

      IdentifierString _name;

      ReferenceMap  _references;
      ReferenceMap  _actionNames;
      ActionMap     _actions;

      ReferenceMap  _constants;

      ResolvedMap   _resolvedReferences;
      ResolvedMap   _resolvedActionNames;
      ResolvedAMap  _resolvedActions;

      ref_t retrieveSignature(ref_t* references, size_t length, bool existing);

   public:
      ustr_t name() const override
      {
         return _name.str();
      }

      ustr_t resolveReference(ref_t reference) override;
      size_t resolveSignature(ref_t signature, ref_t* references) override;
      ustr_t resolveAction(ref_t reference, ref_t& signature) override;
      ustr_t resolveConstant(ref_t reference) override;

      void forEachReference(void* arg, void(*lambda)(ModuleBase*, ref_t, void*)) override;

      AbstractModule()
         : _references(0), _actionNames(0),
           _actions(0), _constants(0),
           _resolvedReferences(nullptr),
           _resolvedActionNames(nullptr),
           _resolvedActions(0ll)
      {

      }
   };

   // --- Module ---
   class Module : public AbstractModule
   {
      SectionMap       _sections;

      void saveSections(StreamWriter& writer);
      void loadSections(StreamReader& writer);

   public:
      ref_t mapReference(ustr_t referenceName) override;
      ref_t mapReference(ustr_t referenceName, bool existing) override;
      void mapPredefinedReference(ustr_t referenceName, ref_t reference) override;

      ref_t mapSignature(ref_t* references, size_t length, bool existing) override;
      ref_t mapAction(ustr_t actionName, ref_t signature, bool existing) override;
      ref_t mapConstant(ustr_t constant) override;

      MemoryBase* mapSection(ref_t reference, bool existing) override;

      bool save(StreamWriter& writer);
      LoadResult load(StreamReader& reader);

      Module()
         : _sections(nullptr)
      {
      }
      Module(ustr_t name);
   };

   // --- ROModule ---

   class ROModule : public AbstractModule
   {
   public:
      // ROSection
      class ROSection : public MemoryBase
      {
         friend class ROModule;

         const void* _buffer;

         ROSection(const void* buffer)
         {
            _buffer = buffer;
         }
      public:
         ROSection()
         {
            _buffer = nullptr;
         }

         pos_t length() const override
         {
            return *(const pos_t*)_buffer;
         }

         void* get(pos_t position) const override
         {
            return (char*)_buffer + position + sizeof(pos_t);
         }

         bool read(pos_t position, void* s, pos_t length) const override
         {
            memcpy(s, (char*)_buffer + position + sizeof(pos_t), length);

            return true;
         }

         bool insert(pos_t position, const void* s, pos_t length) override;

         bool write(pos_t position, const void* s, pos_t length) override;

         void trim(pos_t) override;

         void* getReferences() const override;
      }; 

      typedef FixedMemoryMap<ref_t, ROSection> ROSectionMap;

   private:
      MemoryDump       _sectionDump;
      ROSectionMap     _sections;

      void loadSections(StreamReader& reader);

   public:
      ref_t mapReference(ustr_t referenceName) override;
      ref_t mapReference(ustr_t referenceName, bool existing) override;
      void mapPredefinedReference(ustr_t referenceName, ref_t reference) override;

      ref_t mapSignature(ref_t* references, size_t length, bool existing) override;
      ref_t mapAction(ustr_t actionName, ref_t signature, bool existing) override;
      ref_t mapConstant(ustr_t reference) override;

      MemoryBase* mapSection(ref_t reference, bool existing) override;

      ROModule(StreamReader& reader, LoadResult& result);
   };
}

#endif
