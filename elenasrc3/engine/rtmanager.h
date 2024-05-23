//---------------------------------------------------------------------------
//      This header contains the declaration of a class
//      ELENA RT manager.
//                                             (C)2023-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef RTMANAGER_H
#define RTMANAGER_H

#include "elena.h"

namespace elena_lang
{
   // --- RTManager ---
   class RTManager
   {
      MemoryBase* msection;
      MemoryBase* dbgsection;

      bool readAddressInfo(addr_t retAddress, LibraryLoaderBase& provider, ustr_t& symbol, ustr_t& method, 
         ustr_t& path, int& row, bool vmMode);

   public:
      void loadRootPackage(LibraryProviderBase& provider, path_t rootPath);

      ref_t loadSubject(ustr_t actionName);

      mssg_t loadWeakMessage(mssg_t message, bool vmMode);

      void loadSubjectName(IdentifierString& actionName, ref_t subjectRef);

      size_t retriveAddressInfo(LibraryLoaderBase& provider, addr_t retAddress, char* buffer, 
         size_t length, bool vmMode);

      bool loadSignature(ref_t subjectRef, pos_t argCount, addr_t* addresses);

      addr_t retrieveGlobalAttribute(int attribute, ustr_t name);

      static size_t loadClassName(addr_t classAddress, char* buffer, size_t length);

      RTManager(MemoryBase* msection, MemoryBase* dbgsection);
   };

}

#endif