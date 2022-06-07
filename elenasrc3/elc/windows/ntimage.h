//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker base class declaration
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef NTIMAGE_H
#define NTIMAGE_H

#include "clicommon.h"

namespace elena_lang
{
   typedef Map<ustr_t, ReferenceMap*, allocUStr, freeUStr>  ImportTable;

   // --- WinNtImageFormatter --

   class WinNtImageFormatter : public ImageFormatter
   {
   protected:
      ForwardResolverBase* _resolver;

      WinNtImageFormatter(ForwardResolverBase* resolver)
      {
         _resolver = resolver;
      }

      virtual pos_t fillImportTable(ImportTable& importTable, AddressMap::Iterator it);
      virtual void createImportSection(ImageProviderBase& provider, RelocationMap& importMapping) = 0;

      void mapImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections,
         pos_t sectionAlignment, pos_t fileAlignment);

      virtual void fixSection(Section* section, AddressSpace& map) = 0;
      virtual void fixImportSection(Section* section, AddressSpace& map) = 0;

      void fixImage(ImageProviderBase& provider, AddressSpace& map, bool withDebugInfo);

   public:
      void prepareImage(ImageProviderBase& code, AddressSpace& map, ImageSections& sections,
         pos_t sectionAlignment, pos_t fileAlignment, bool withDebugInfo) override;
   };

   // --- Win32NtImageFormatter ---
   class Win32NtImageFormatter : public WinNtImageFormatter
   {
      Win32NtImageFormatter(ForwardResolverBase* resolver)
         : WinNtImageFormatter(resolver)
      {         
      }

      void createImportSection(ImageProviderBase& provider, RelocationMap& importMapping) override;

      void fixSection(Section* section, AddressSpace& map) override;
      void fixImportSection(Section* section, AddressSpace& map) override;

   public:
      static Win32NtImageFormatter& getInstance(ForwardResolverBase* resolver)
      {
         static Win32NtImageFormatter instance(resolver);

         return instance;
      }
   };

   // --- Win64NtImageFormatter ---
   class Win64NtImageFormatter : public WinNtImageFormatter
   {
      Win64NtImageFormatter(ForwardResolverBase* resolver)
         : WinNtImageFormatter(resolver)
      {
         
      }

      void createImportSection(ImageProviderBase& provider, RelocationMap& importMapping) override;

      void fixSection(Section* section, AddressSpace& map) override;
      void fixImportSection(Section* section, AddressSpace& map) override;

   public:
      static Win64NtImageFormatter& getInstance(ForwardResolverBase* resolver)
      {
         static Win64NtImageFormatter instance(resolver);

         return instance;
      }
   };

}

#endif // NTIMAGE_H