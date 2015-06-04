//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Linux32
//                                              (C)2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef linkerH
#define linkerH 1

#include "project.h"
#include "loader.h"

namespace _ELENA_
{

// --- ImageBaseMap ---

struct ImageBaseMap
{
   int base;
   int code, rdata, bss, stat, /*tls,*/ debug, import;

   RelocationFixMap importMapping;

   ImageBaseMap()
      : importMapping((size_t)-1)
   {
      base = code = rdata = bss = stat /*= tls*/ = debug = import = 0;
   }
};

// --- Linker ---

class Linker32
{
   typedef Map<const char*, ref_t, true> ImportReferences;

   struct ImageInfo
   {
      Project*     project;
      Image*       image;
      bool         withDebugInfo;

      // Import tables
      ImportReferences functions;
      List<char*>      libraries;

      // image base addresses
      ImageBaseMap map;

      // Linker target image properties
      int  headerSize, textSize, rdataSize, importSize, bssSize, debugSize;
      int  ph_length;      // number of entries in the program header table
      int  interpreter, dynamic, entryPoint;

      ImageInfo(Project* project, Image* image)
         : libraries(NULL, freestr)
      {
         this->project = project;
         this->image = image;
         this->entryPoint = this->interpreter = this->dynamic = 0;
         this->ph_length = 0;
         this->headerSize = this->textSize = this->rdataSize = this->importSize = this->bssSize = this->debugSize = 0;
         this->withDebugInfo = project->BoolSetting(opDebugMode)/*false*/;
      }
   };

   int fillImportTable(ImageInfo& info);
   void createImportData(ImageInfo& info);

   void mapImage(ImageInfo& info);
   void fixImage(ImageInfo& info);

   void writeSection(FileWriter* file, Section* section, int alignment);

   void writeELFHeader(ImageInfo& info, FileWriter* file);
   void writePHTable(ImageInfo& info, FileWriter* file);
   void writeSegments(ImageInfo& info, FileWriter* file);

   bool createExecutable(ImageInfo& info, const char* exePath/*, ref_t tls_directory*/);

protected:
   virtual void writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference) = 0;
   virtual size_t writePLTEntry(MemoryWriter& codeWriter, int symbolIndex, ref_t gotReference, int gofOffset, int entryIndex) = 0;

public:
   void run(Project& project, Image& image/*, ref_t tls_directory*/);

   Linker32()
   {
   }
};

class I386Linker32 : public Linker32
{
protected:
   virtual void writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference);
   virtual size_t writePLTEntry(MemoryWriter& codeWriter, int symbolIndex, ref_t gotReference, int gofOffset, int entryIndex);

public:
   I386Linker32()
   {
   }
};

} // _ELENA_

#endif // linkerH
