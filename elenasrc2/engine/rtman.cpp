//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the base class implementing ELENA RTManager.
//
//                                              (C)2005-2019, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------
#include "rtman.h"

using namespace _ELENA_;

//inline void printInfo2(char ch)
//{
//   putchar(ch);
//   fflush(stdout);
//}

// --- RTManager ---

void RTManager :: readCallStack(StreamReader& stack, lvaddr_t startPosition, lvaddr_t currentAddress, StreamWriter& output)
{
   lvaddr_t position = startPosition;
   lvaddr_t ret = 0;

   output.writeVAddress(currentAddress);

   do {
      stack.seek(position);
      position = stack.getVAddress();
      ret = stack.getVAddress();
      if (position == 0 && ret != 0) {
         position = ret;
      }
      else if (ret != 0) {
         output.writeVAddress(ret);
      }
   } while (position != 0);
}

inline lvaddr_t readVAddr(MemoryDump& dump, pos_t position)
{
   lvaddr_t addr;
   dump.read(position, &addr, sizeof(lvaddr_t));

   return addr;
}

pos_t RTManager :: readCallStack(StreamReader& reader, lvaddr_t framePosition, lvaddr_t currentAddress, lvaddr_t startLevel, lvaddr_t* buffer, pos_t maxLength)
{
   MemoryDump retPoints;
   MemoryWriter writer(&retPoints);

   readCallStack(reader, framePosition, currentAddress, writer);

   pos_t index = 0;
   pos_t current = 0;
   while (current < retPoints.Length()) {
      if (startLevel == 0) {
         if (index == maxLength - 2) {
            buffer[index] = 0;
            index++;
            buffer[index] = readVAddr(retPoints, retPoints.Length() - sizeof(lvaddr_t));
         }
         else {
            buffer[index] = readVAddr(retPoints, current);

            index++;
         }
      }
      else startLevel--;

      current += sizeof(lvaddr_t);
   }

   return index + 1;
}

lvaddr_t RTManager::readClassName(StreamReader& reader, lvaddr_t classVAddress, char* buffer, size_t maxLength)
{
   ident_t symbol;

   // search through debug section until the ret point is inside two consecutive steps within the same object
   while (!reader.Eof()/* && !found*/) {
      // read reference
      symbol = reader.getLiteral(DEFAULT_STR);

      // define the next record position
      pos_t size = reader.getDWord() - 4;
      pos_t nextPosition = reader.Position() + size;

      // check the class
      size_t pos = symbol.findLast('\'');
      if (symbol[pos + 1] != '#') {
         lvaddr_t vmtAddress = reader.getVAddress();
         if (vmtAddress == classVAddress) {
            size_t len = maxLength;
            if (len > getlength(symbol)) {
               symbol.copyTo(buffer, len);

               return len;
            }
         }
      }

      reader.seek(nextPosition);
   }

   return 0;
}

lvaddr_t RTManager :: loadMetaAttribute(StreamReader& reader, ident_t name, int category, size_t len)
{
   pos_t pos = reader.Position();

   len += pos;
   while (pos < len) {
      int current = reader.getDWord();
      int offset = sizeof(lvaddr_t) + reader.getDWord();
      if (current == category) {
         ident_t currentName = reader.getLiteral(DEFAULT_STR);
         lvaddr_t ptr = 0;
         reader.read(&ptr, sizeof(lvaddr_t));
         if (name.compare(currentName))
            return ptr;
      }

      pos += offset;
      reader.seek(pos);
   }

   return 0;
}

bool RTManager :: readAddressInfo(StreamReader& reader, uintptr_t retAddress, _LibraryManager* manager,
   ident_t &symbol, ident_t &method, ident_t &path, int& row)
{
   int index = 0;
   bool found = false;
   row = 0;

   // search through debug section until the ret point is inside two consecutive steps within the same object
   while (!reader.Eof() && !found) {
      // read reference
      symbol = reader.getLiteral(DEFAULT_STR);

      // define the next record position
      size_t size = reader.getDWord() - 4;
      index = 0;
      size_t previous = 0;
      size_t current = 0;
      while (size > 0) {
         current = reader.getDWord();

         if (retAddress == current || (previous != 0 && previous < retAddress && current >= retAddress)) {
            found = true;

            break;
         }

         previous = current;
         index++;
         size -= 4;
      }
   }

   if (found) {
      bool isClass = true;
      _Module* module = NULL;
      // if symbol
      if (symbol[0]=='#') {
         symbol+=1;

         isClass = false;
      }
      LoadResult result;
      ref_t position = 0;
      // load the appropriate debug module
      module = manager->resolveDebugModule(symbol, result, position);
      if (result == lrSuccessful) {
         // load the object debug section
         MemoryReader lineReader(module->mapSection(DEBUG_LINEINFO_ID | mskDataRef, true), position);
         MemoryReader stringReader(module->mapSection(DEBUG_STRINGS_ID | mskDataRef, true));

         // skip vmt address for a class
         reader.getDWord();

         // look through the records to find the entry
         DebugLineInfo info;
         while (!lineReader.Eof()) {
            lineReader.read(&info, sizeof(DebugLineInfo));
            if (info.symbol == dsProcedure) {
               stringReader.seek(info.addresses.source.nameRef);
               path = stringReader.getLiteral(DEFAULT_STR);
               method = NULL;
            }
            else if (info.symbol == dsMessage) {
               stringReader.seek(info.addresses.source.nameRef);
               method = stringReader.getLiteral(DEFAULT_STR);
            }
            else if ((info.symbol & dsDebugMask) == dsStep) {
               index--;
               if (index == 0) {
                  if (info.row >= 0) {
                     row = info.row;
                  }
                  break;
               }
            }

            if (info.row > 0)
               row = info.row;
         }
      }
      else {
         path = NULL;
         method = NULL;
      }
   }

   return found;
}

void copy(char* buffer, ident_t word, size_t& copied, size_t maxLength)
{
   size_t length = getlength(word);

   if (maxLength - copied < length)
      length = maxLength - copied;

   if (length > 0)
      Convertor::copy(buffer + copied, word, length, length);

   copied += length;
}

void copy(char* buffer, int value, size_t& copied)
{
   String<char, 10> tmp;
   tmp.appendInt(value);

   size_t length = getlength(tmp);
   Convertor::copy(buffer + copied, tmp, length, length);

   copied += length;
}

lvaddr_t RTManager :: readAddressInfo(StreamReader& debug, uintptr_t retAddress, _LibraryManager* manager, char* buffer, size_t maxLength)
{
   ident_t symbol = NULL;
   ident_t method = NULL;
   ident_t path = NULL;
   int row = -1;

   if (readAddressInfo(debug, retAddress, manager, symbol, method, path, row)) {
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
   else return 0;
}

//size_t RTManager :: readSubjectName(StreamReader& reader, size_t subjectRef, char* buffer, size_t maxLength)
//{
//   ReferenceMap subjects(0);
//   subjects.read(&reader);
//
//   ident_t name = retrieveKey(subjects.start(), subjectRef, DEFAULT_STR);
//   if (!emptystr(name)) {
//      size_t len = getlength(name);
//      if (len < maxLength) {
//         Convertor::copy(buffer, name, len, len);
//
//         return len;
//      }
//   }
//   return 0;
//}

pos_t RTManager :: loadSubject(StreamReader& reader, ident_t name)
{
   ReferenceMap subjects(0);
   subjects.read(&reader);

   return subjects.get(name);
}
