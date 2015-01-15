//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the base class implementing ELENA RTManager.
//
//                                              (C)2005-2014, by Alexei Rakov
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

bool RTManager :: readAddressInfo(StreamReader& reader, size_t retAddress, _LibraryManager* manager,
                                  const wchar16_t* &symbol, const wchar16_t* &method, const wchar16_t* &path, int& row)
{
   int index = 0;
   bool found = false;

   // search through debug section until the ret point is inside two consecutive steps within the same object
   while (!reader.Eof() && !found) {
      // read reference
      symbol = reader.getWideLiteral();

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
         if (isClass) {
            reader.getDWord();
         }

         // look through the records to find the entry
         DebugLineInfo info;
         void* current = NULL;
         while (!lineReader.Eof()) {
            lineReader.read(&info, sizeof(DebugLineInfo));
            if (info.symbol == dsProcedure) {
               stringReader.seek(info.addresses.source.nameRef);
               path = stringReader.getWideLiteral();
               method = NULL;
            }
            else if (info.symbol == dsMessage) {
               stringReader.seek(info.addresses.source.nameRef);
               method = stringReader.getWideLiteral();
            }
            else if ((info.symbol & dsDebugMask) == dsStep) {
               index--;
               if (index == 0) {
                  if (info.row != 0) {
                     row = info.row;
                  }
                  break;
               }
            }

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

void copy(wchar16_t* buffer, const wchar16_t* word, int& copied, int maxLength)
{
   int length = getlength(word);

   if (maxLength - copied < length)
      length = maxLength - copied;

   if (length > 0)
      StringHelper::copy(buffer + copied, word, length);

   copied += length;
}

void copy(wchar16_t* buffer, int value, int& copied)
{
   wchar16_t tmp[10];
   StringHelper::intToStr(value, tmp, 10);

   StringHelper::copy(buffer + copied, tmp, getlength(tmp));

   copied += getlength(tmp);
}

bool RTManager :: readAddressInfo(StreamReader& debug, size_t retAddress, _LibraryManager* manager, wchar16_t* buffer, size_t& length)
{
   const wchar16_t* symbol;
   const wchar16_t* method;
   const wchar16_t* path;
   int row;

   if (readAddressInfo(debug, retAddress, manager, symbol, method, path, row)) {
      int copied = 0;
      copy(buffer, symbol, copied, length - 2);
      buffer[copied++] = '.';
      copy(buffer, method, copied, length - 1);
      buffer[copied++] = ':';
      copy(buffer, path, copied, length - 6);
      buffer[copied++] = '(';
      copy(buffer, row, copied);
      buffer[copied++] = ')';

      length = copied;

      return true;
   }
   else {
      length = 0;

      return false;
   }
}
