//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Win32
//                                              (C)2005-2011, by Alexei Rakov
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
   int code, rdata, bss, stat, tls, import;

   RelocationFixMap importMapping;

   ImageBaseMap()
      : importMapping((size_t)-1)
   {
      base = code = rdata = bss = stat = tls = import = 0;
   }
};

// --- Linker ---

class Linker
{
   typedef Map<const wchar16_t*, ReferenceMap*>  ImportTable;

   Project*         _project;

   // Import table
   ImportTable      _importTable;   

   // image base addresses
   ImageBaseMap     _map;

   // Linker target image properties
   int _headerSize, _imageSize;
   int _entryPoint; 

   bool _withDebugInfo;

   int countSections(Image& image);

   int fillImportTable(Image& image);
   void createImportTable(Image& image);

   void mapImage(Image& image);
   void fixImage(Image& image);

   void writeDOSStub(FileWriter* file);
   void writeHeader(FileWriter* file, short characteristics, int sectionCount);
   void writeNTHeader(Image& image, FileWriter* file, ref_t tls_directory);

   void writeSectionHeader(FileWriter* file, const char* name, Section* section, int& tblOffset, 
                           int alignment, int sectionAlignment, int vaddress, int characteristics);
   void writeBSSSectionHeader(FileWriter* file, const char* name, size_t size, 
                              int sectionAlignment,  int vaddress, int characteristics);
   void writeSection(FileWriter* file, Section* section, int alignment);
   void writeSections(Image& image, FileWriter* file);

   bool createExecutable(Image& image, const _path_t* exePath, ref_t tls_directory);
   bool createDebugFile(Image& image, const _path_t* debugFilePath);

public:
   void run(Image& image, ref_t tls_directory);

   Linker(Project* project)
      : _importTable(NULL, freeobj)
   {
      _project = project;

      _entryPoint = 0;
      _headerSize = _imageSize = 0;
      _withDebugInfo = project->BoolSetting(opDebugMode);
   }
};

} // _ELENA_

#endif // linkerH
