//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenamachineH
#define elenamachineH 1

#define ELENART_BUILD_NUMBER    0x0001             // ELENART build version

#include "libman.h"

namespace _ELENA_
{

// --- Instance ---

class Instance
{
public:
   class ImageSection : public _Memory
   {
      void*  _section;
      size_t _length;

   public:
      void init(void* section, size_t length)
      {
         _section = section;
         _length = length;
      }
      void init(void* section)
      {
         _section = section;
         _length = 4;

         MemoryReader reader(this);
         _length = reader.getDWord();
      }

      virtual size_t Length() const
      {
         return _length;
      }

      virtual void* get(size_t position) const;

      virtual bool read(size_t position, void* s, size_t length);

      virtual bool write(size_t position, const void* s, size_t length)
      {
         // write operations are not supported
         return false;
      }

      virtual void insert(size_t position, const void* s, size_t length)
      {
         // insert operations are not supported
      }

      virtual bool writeBytes(size_t position, char value, size_t length)
      {
         // write operations are not supported
         return false;
      }

      virtual void trim(size_t position)
      {
         // trim operations are not supported
      }

      ImageSection()
      {
         _section = NULL;
         _length = 0;
      }
   };

private:
   ImageSection   _debugSection;
   LibraryManager _loader;

   bool loadConfig();

public:
   int readCallStack(size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength);

   int loadAddressInfo(size_t retPoint, wchar16_t* lineInfo, size_t length);

   void init(void* debugSection, const wchar16_t* package);

   Instance();

   virtual ~Instance()
   {
   }
};

} // _ELENA_

#endif // elenamachineH
