//---------------------------------------------------------------------------
//      This header contains the implementation of the class
//      ELENA RT manager.
//                                             (C)2023-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "rtmanager.h"

#include "core.h"
#include "streams.h"

using namespace elena_lang;

#if _M_IX86 || __i386__

constexpr int elVMTClassOffset = elVMTClassOffset32;

#else

constexpr int elVMTClassOffset = elVMTClassOffset64;

#endif

// --- RTManager ---

RTManager :: RTManager(MemoryBase* msection, MemoryBase* dbgsection)
   : msection(msection), dbgsection(dbgsection)
{

}

void RTManager :: loadRootPackage(LibraryProviderBase& provider, path_t rootPath)
{
   MemoryReader reader(dbgsection);

   // skip a debugger entry pointer
   addr_t tempAddr = 0;
   reader.read(&tempAddr, sizeof(tempAddr));

   ustr_t ns = reader.getString(DEFAULT_STR);
   provider.addPackage(ns, rootPath);
}

bool RTManager :: readAddressInfo(addr_t retAddress, LibraryLoaderBase& provider, ustr_t& symbol, ustr_t& method, 
   ustr_t& path, int& row, bool vmMode)
{
   MemoryReader reader(dbgsection);

   // skip a debugger entry pointer for the stand-alone application
   addr_t tempAddr = 0;
   if (!vmMode) {
      reader.read(&tempAddr, sizeof(tempAddr));

      ustr_t ns = reader.getString(DEFAULT_STR);

      const char* s = ns.str();
      printf("readAddressInfo ns %s\n", s);

   }

   // search through debug section until the ret point is inside two consecutive steps within the same object
   int index = 0;
   bool found = false;
   while (!reader.eof() && !found) {
      // read reference
      ustr_t current_symbol = reader.getString(DEFAULT_STR);

      // define the next record position
      pos_t size = reader.getPos() - 4;
      index = 0;
      addr_t previous = 0;
      addr_t current = 0;
      while (size != 0) {
         if(!reader.read(&current, sizeof(current)))
            break;

         if (retAddress == current || (previous != 0 && previous < retAddress && current >= retAddress)) {
            found = true;
            symbol = current_symbol;

            break;
         }

         previous = current;
         index++;

         size -= (size < sizeof(current)) ? size : sizeof(current);
      }
   }

   if (found) {
      const char* s = symbol.str();
      printf("readAddressInfo %s\n", s);

      // if symbol
      if (symbol[0] == '#') {
         symbol += 1;
      }

      auto moduleInfo = provider.getDebugModule({ symbol }, true);
      if (moduleInfo.module != nullptr) {
         // load the object debug section
         MemoryReader lineReader(moduleInfo.module->mapSection(DEBUG_LINEINFO_ID | mskDataRef, true), moduleInfo.reference);
         MemoryReader stringReader(moduleInfo.module->mapSection(DEBUG_STRINGS_ID | mskDataRef, true));

         // skip vmt address for a class
         reader.read(&tempAddr, sizeof(tempAddr));

         // look through the records to find the entry
         DebugLineInfo info;
         bool found = false;
         while (!lineReader.eof() && !found) {
            lineReader.read(&info, sizeof(DebugLineInfo));
            switch (info.symbol) {
               case DebugSymbol::Procedure:
                  stringReader.seek(addrToUInt32(info.addresses.source.nameRef));
                  path = stringReader.getString(DEFAULT_STR);
                  method = nullptr;
                  break;
               case DebugSymbol::MessageInfo:
                  stringReader.seek(addrToUInt32(info.addresses.source.nameRef));
                  method = stringReader.getString(DEFAULT_STR);
                  break;
               case DebugSymbol::Class:               // NOTE : to take into account vmt address
               case DebugSymbol::Breakpoint:
               case DebugSymbol::VirtualBreakpoint:
                  index--;
                  if (index == 0) {
                     found = true;
                  }
                  break;
               default:
                  break;
            }

            if (info.row > 0)
               row = info.row;
         }
      }
      else {
         path = nullptr;
         method = nullptr;
      }
   }

   return found;
}

inline void copy(char* buffer, ustr_t word, size_t& copied, size_t maxLength)
{
   size_t length = getlength(word);

   if (maxLength - copied < length)
      length = maxLength - copied;

   if (length > 0)
      StrConvertor::copy(buffer + copied, word, length, length);

   copied += length;
}

inline void copy(char* buffer, int value, size_t& copied)
{
   String<char, 10> tmp;
   tmp.appendInt(value);

   size_t length = tmp.length();
   StrConvertor::copy(buffer + copied, tmp.str(), length, length);

   copied += length;
}

size_t RTManager :: retriveAddressInfo(LibraryLoaderBase& provider, addr_t retAddress, char* buffer, 
   size_t maxLength, bool vmMode)
{
   ustr_t symbol  = nullptr;
   ustr_t method  = nullptr;
   ustr_t path    = nullptr;
   int row        = -1;

   if (readAddressInfo(retAddress, provider, symbol, method, path, 
      row, vmMode)) 
   {
      size_t copied = 0;
      copy(buffer, symbol, copied, maxLength - 2);
      if (!emptystr(method)) {
         buffer[copied++] = '.';
         copy(buffer, method, copied, maxLength - 1);
      }
      if (!emptystr(path)) {
         buffer[copied++] = ':';
         copy(buffer, path, copied, maxLength - 6);
      }
      if (row != -1) {
         buffer[copied++] = '(';
         copy(buffer, row, copied);
         buffer[copied++] = ')';
      }
      buffer[copied] = 0;

      printf("retriveAddressInfo %s\n", buffer);

      return copied;
   }

   return 0;
}

constexpr pos_t MessageEntryLen = sizeof(uintptr_t) * 2;

bool RTManager :: loadSignature(ref_t subjectRef, pos_t argCount, addr_t* addresses)
{
   pos_t mtableOffset = MemoryBase::getDWord(msection, 0);
   ref_t actionPtr = MemoryBase::getDWord(msection, mtableOffset + subjectRef * MessageEntryLen);
   if (actionPtr != 0) {
      uintptr_t singPtr = 0;
      msection->read(mtableOffset + subjectRef * MessageEntryLen + sizeof(uintptr_t), &singPtr, sizeof(uintptr_t));

      for (pos_t i = 0; i < argCount; i++) {
         addresses[i] = ((addr_t*)singPtr)[i];
      }

      return true;
   }

   return false;
}

void RTManager :: loadSubjectName(IdentifierString& actionName, ref_t subjectRef)
{
   pos_t mtableOffset = MemoryBase::getDWord(msection, 0);

   ref_t actionPtr = MemoryBase::getDWord(msection, mtableOffset + subjectRef * MessageEntryLen);
   if (!actionPtr) {
      addr_t namePtr = 0;
      msection->read(mtableOffset + subjectRef * sizeof(uintptr_t) * 2 + sizeof(uintptr_t), &namePtr, sizeof(addr_t));

      MemoryReader reader(msection);
      reader.seek((pos_t)(namePtr - (addr_t)msection->get(0)));

      reader.readString(actionName);
   }
   else loadSubjectName(actionName, actionPtr);
}

ref_t RTManager :: loadSubject(ustr_t actionName)
{
   pos_t mtableOffset = MemoryBase::getDWord(msection, 0);

   MemoryReader reader(msection);
   IdentifierString messageName;
   addr_t startPtr = (addr_t)msection->get(0);
   for (ref_t subjectRef = 1; true; subjectRef++) {
      if (MemoryBase::getDWord(msection, mtableOffset + subjectRef * MessageEntryLen) == 0) {
         pos_t namePtr = MemoryBase::getDWord(msection, mtableOffset + subjectRef * MessageEntryLen + sizeof(uintptr_t));
         if (!namePtr)
            break;

         reader.seek(static_cast<pos_t>(namePtr - startPtr));
         reader.readString(messageName);
         if (messageName.compare(actionName)) {
            return subjectRef;
         }
      }
   }

   return 0;
}

addr_t RTManager :: retrieveGlobalAttribute(int attribute, ustr_t name)
{
   IdentifierString currentName;
   pos_t size = MemoryBase::getDWord(msection, 0);

   MemoryReader reader(msection, 4);
   pos_t pos = reader.position();
   while (reader.position() < size) {
      int current = reader.getDWord();
      pos_t offset = reader.getDWord() + sizeof(addr_t);
      if (current == attribute) {
         reader.readString(currentName);
         if (currentName.compare(name)) {
            addr_t address = 0;
            reader.read(&address, sizeof(address));

            return address;
         }
      }

      pos += offset;
      reader.seek(pos);
   }

   return 0;
}

size_t RTManager :: loadClassName(addr_t classAddress, char* buffer, size_t length)
{
   uintptr_t namePtr = *(uintptr_t*)(classAddress - sizeof(uintptr_t) * 1 - elVMTClassOffset);

   char* name = (char*)namePtr;

   if (!ustr_t(name).copyTo(buffer, length))
      return 0;

   return length;
}

mssg_t RTManager :: loadWeakMessage(mssg_t message, bool vmMode)
{
   ref_t actionRef = 0, flags = 0;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   pos_t mtableOffset = vmMode ? 0 : MemoryBase::getDWord(msection, 0);
   MemoryReader reader(msection, mtableOffset);

   reader.seek(reader.position() + actionRef * sizeof(uintptr_t) * 2);

   pos_t weakActionRef = reader.getPos();

   return weakActionRef ? encodeMessage(weakActionRef, argCount, flags) : message;
}
