//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Win32
//                                              (C)2005-2021, by Alexei Rakov
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
   lvaddr_t base;
   lvaddr_t code, adata, mdata, rdata, bss, stat, tls, import;

   RelocationFixMap importMapping;

   ImageBaseMap()
      : importMapping(INVALID_REF)
   {
      base = code = adata = mdata = rdata = bss = stat = tls = import = 0;
   }
};

// --- Linker ---

class Linker
{
   typedef Map<ident_t, ReferenceMap*>  ImportTable;

   struct ImageInfo
   {
      Project* project;
      Image* image;
      bool         withDebugInfo;

      // Import table
      ImportTable  importTable;

      // image base addresses
      ImageBaseMap map;

      // Linker target image properties
      int  headerSize, imageSize;
      lvaddr_t entryPoint;

      ImageInfo(Project* project, Image* image)
         : importTable(NULL, freeobj)
      {
         this->project = project;
         this->image = image;
         this->entryPoint = 0;
         this->headerSize = imageSize = 0;
         this->withDebugInfo = project->BoolSetting(opDebugMode);
      }
   };

   bool _mode64bit;

   int countSections(Image* image);

   int fillImportTable(ImageInfo& info);
   void createImportTable(ImageInfo& info);
   void createImportTable64(ImageInfo& info);

   void mapImage(ImageInfo& info);
   void fixImage(ImageInfo& info);

   void writeDOSStub(Project* project, FileWriter* file);
   void writeHeader(FileWriter* file, short characteristics, int sectionCount);
   void writeNTHeader(ImageInfo& info, FileWriter* file, ref_t tls_directory);
   void writeNTHeader64(ImageInfo& info, FileWriter* file, ref_t tls_directory);

   void writeSectionHeader(FileWriter* file, const char* name, Section* section, int& tblOffset,
      int alignment, int sectionAlignment, int vaddress, int characteristics);
   void writeBSSSectionHeader(FileWriter* file, const char* name, size_t size,
      int sectionAlignment, int vaddress, int characteristics);
   void writeSection(FileWriter* file, Section* section, int alignment);
   void writeSections(ImageInfo& info, FileWriter* file);

   bool createExecutable(ImageInfo& info, path_t exePath, ref_t tls_directory);
   bool createDebugFile(ImageInfo& image, path_t debugFilePath);

public:
   void prepareTLS(Image& image, ref_t tls_variable, ref_t& tls_directory);

   void run(Project& project, Image& image, ref_t tls_directory);

   Linker()
   {
      _mode64bit = false;
   }
   Linker(bool mode64bit)
   {
      _mode64bit = mode64bit;
   }
};

} // _ELENA_

#endif // linkerH
