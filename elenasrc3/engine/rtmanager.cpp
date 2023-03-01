//---------------------------------------------------------------------------
//      This header contains the implementation of the class
//      ELENA RT manager.
//                                              (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "rtmanager.h"
#include "streams.h"

using namespace elena_lang;

// --- RTManager ---

RTManager :: RTManager(MemoryBase* msection, MemoryBase* dbgsection)
   : msection(msection), dbgsection(dbgsection)
{

}

bool RTManager :: readAddressInfo(addr_t retAddress, LibraryLoaderBase& provider, ustr_t& symbol, ustr_t& method, ustr_t& path, int& row)
{
   MemoryReader reader(dbgsection);

   // skip a debugger entry pointer
   addr_t tempAddr = 0;
   reader.read(&tempAddr, sizeof(tempAddr));

   ustr_t ns = reader.getString(DEFAULT_STR);

   // search through debug section until the ret point is inside two consecutive steps within the same object
   int index = 0;
   bool found = false;
   while (!reader.eof() && !found) {
      // read reference
      ustr_t current_symbol = reader.getString(DEFAULT_STR);
      if (current_symbol.empty())
         break;

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
      bool isClass = true;
      // if symbol
      if (symbol[0] == '#') {
         symbol += 1;

         isClass = false;
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
                  stringReader.seek(info.addresses.source.nameRef);
                  path = stringReader.getString(DEFAULT_STR);
                  method = nullptr;
                  break;
               case DebugSymbol::MessageInfo:
                  stringReader.seek(info.addresses.source.nameRef);
                  method = stringReader.getString(DEFAULT_STR);
                  break;
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

size_t RTManager :: retriveAddressInfo(LibraryLoaderBase& provider, addr_t retAddress, char* buffer, size_t maxLength)
{
   ustr_t symbol  = nullptr;
   ustr_t method  = nullptr;
   ustr_t path    = nullptr;
   int row        = -1;

   if (readAddressInfo(retAddress, provider, symbol, method, path, row)) {
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
         copy(buffer, row + 1, copied);
         buffer[copied++] = ')';
      }
      buffer[copied] = 0;

      return copied;
   }

   return 0;
}

constexpr pos_t MessageEntryLen = sizeof(uintptr_t) * 2;

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
   for (ref_t subjectRef = 1; true; subjectRef++) {
      if (MemoryBase::getDWord(msection, mtableOffset + subjectRef * MessageEntryLen) == 0) {
         pos_t namePtr = MemoryBase::getDWord(msection, mtableOffset + subjectRef * MessageEntryLen + sizeof(uintptr_t));
         if (!namePtr)
            break;

         reader.seek(mtableOffset + namePtr);
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
