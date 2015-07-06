//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the base class implementing ELENA RTManager.
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------
#include "rtman.h"

using namespace _ELENA_;

// --- RTManager ---

void RTManager :: readCallStack(StreamReader& stack, size_t startPosition, size_t currentAddress, StreamWriter& output)
{
   size_t position = startPosition;
   size_t ret = 0;

   output.writeDWord(currentAddress);

   do {
      stack.seek(position);
      position = stack.getDWord();
      ret = stack.getDWord();
      if (position == 0 && ret != 0) {
         position = ret;
      }
      else if (ret != 0) {
         output.writeDWord(ret);
      }
   } while (position != 0);
}

size_t RTManager :: readCallStack(StreamReader& reader, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
{
   MemoryDump retPoints;
   MemoryWriter writer(&retPoints);

   readCallStack(reader, framePosition, currentAddress, writer);

   size_t index = 0;
   size_t current = 0;
   while (current < retPoints.Length()) {
      if (startLevel == 0) {
         if (index == maxLength - 2) {
            buffer[index] = 0;
            index++;
            buffer[index] = retPoints[retPoints.Length() - 4];
         }
         else {
            buffer[index] = retPoints[current];

            index++;
         }
      }
      else startLevel--;

      current += 4;
   }

   return index + 1;
}

size_t RTManager::readClassName(StreamReader& reader, size_t classVAddress, ident_c* buffer, size_t maxLength)
{
   ident_t symbol;

   // search through debug section until the ret point is inside two consecutive steps within the same object
   while (!reader.Eof()/* && !found*/) {
      // read reference
      symbol = reader.getLiteral(DEFAULT_STR);

      // define the next record position
      size_t size = reader.getDWord() - 4;
      int nextPosition = reader.Position() + size;

      // check the class
      if (symbol[0] != '#') {
         int vmtAddress = reader.getDWord();
         if (vmtAddress == classVAddress) {
            size_t len = getlength(symbol);
            if (len < maxLength) {
               StringHelper::copy(buffer, symbol, len, len);

               return len;
            }
         }
      }

      reader.seek(nextPosition);
   }

   return 0;
}

void* RTManager :: loadSymbol(StreamReader& reader, ident_t name)
{
   ident_t symbol;

   // search through debug section until the ret point is inside two consecutive steps within the same object
   while (!reader.Eof()/* && !found*/) {
      // read reference
      symbol = reader.getLiteral(DEFAULT_STR);

      // define the next record position
      size_t size = reader.getDWord() - 4;
      int nextPosition = reader.Position() + size;

      // check the class
      if (symbol[0] == '#') {
         int address = reader.getDWord();

         if (StringHelper::compare(name, symbol + 1)) {
            return (void*)address;
         }
      }

      reader.seek(nextPosition);
   }

   return NULL;
}

bool RTManager :: readAddressInfo(StreamReader& reader, size_t retAddress, _LibraryManager* manager,
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
         symbol++;

         isClass = false;
      }
      LoadResult result;
      size_t position = 0;
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

void copy(ident_c* buffer, ident_t word, int& copied, size_t maxLength)
{
   size_t length = getlength(word);

   if (maxLength - copied < length)
      length = maxLength - copied;

   if (length > 0)
      StringHelper::copy(buffer + copied, word, length, length);

   copied += length;
}

void copy(ident_c* buffer, int value, int& copied)
{
   ident_c tmp[10];
   StringHelper::intToStr(value, tmp, 10);

   size_t length = getlength(tmp);
   StringHelper::copy(buffer + copied, tmp, length, length);

   copied += length;
}

size_t RTManager :: readAddressInfo(StreamReader& debug, size_t retAddress, _LibraryManager* manager, ident_c* buffer, size_t maxLength)
{
   ident_t symbol = NULL;
   ident_t method = NULL;
   ident_t path = NULL;
   int row = -1;

   if (readAddressInfo(debug, retAddress, manager, symbol, method, path, row)) {
      int copied = 0;
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

      return copied;
   }
   else return 0;
}

size_t RTManager :: readSubjectName(StreamReader& reader, size_t subjectRef, ident_c* buffer, size_t maxLength)
{
   ReferenceMap subjects(0);
   subjects.read(&reader);

   ident_t name = retrieveKey(subjects.start(), subjectRef, DEFAULT_STR);
   if (!emptystr(name)) {
      size_t len = getlength(name);
      if (len < maxLength) {
         StringHelper::copy(buffer, name, len, len);

         return len;
      }
   }
   return 0;
}

void* RTManager :: loadSubject(StreamReader& reader, ident_t name)
{
   ReferenceMap subjects(0);
   subjects.read(&reader);

   return (void*)(encodeMessage(subjects.get(name), 0, 0) | MESSAGE_MASK);
}
