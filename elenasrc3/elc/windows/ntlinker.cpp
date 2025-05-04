//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker base class body
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "clicommon.h"
// --------------------------------------------------------------------------
#include "ntlinker.h"
#include "langcommon.h"
#include "windows/winconsts.h"

#include <windows.h>
#include <time.h>

#pragma warning(disable:4996)

using namespace elena_lang;

constexpr auto WINSTUB_PATH      = L"winstub.ex_";
constexpr auto FILE_ALIGNMENT    = 0x200;
constexpr auto SECTION_ALIGNMENT = 0x1000;

// --- WinNtLinker ---

void WinNtLinker :: writeDOSStub(path_t appPath, FileWriter& executable)
{
   PathString stubPath(appPath, WINSTUB_PATH);
   FileReader stub(*stubPath, FileRBMode, FileEncoding::Raw, false);

   if (stub.isOpen()) {
      executable.copyFrom(&stub, stub.length());
   }
   else _errorProcessor->raisePathError(errInvalidFile, *stubPath);
}

void WinNtLinker :: writeExecutableHeader(WinNtExecutableImage& image, FileWriter& executable)
{
   executable.writeInt(IMAGE_NT_SIGNATURE);

   IMAGE_FILE_HEADER header = {};

   header.Characteristics = image.characteristics;
   header.NumberOfSections = image.imageSections.headers.count_short();
   header.TimeDateStamp = (int)time(nullptr);
   header.PointerToSymbolTable = 0;
   header.NumberOfSymbols = 0;
   header.Machine = image.machine;
   header.SizeOfOptionalHeader = (WORD)image.optionalHeaderSize;
   executable.write(&header, IMAGE_SIZEOF_FILE_HEADER);
}

void WinNtLinker :: writeSections(WinNtExecutableImage& image, FileWriter& file)
{
   unsigned int tblOffset = align(image.headerSize, image.fileAlignment);

   // write headers
   for(auto it = image.imageSections.headers.start(); !it.eof(); ++it) {
      IMAGE_SECTION_HEADER header = {};
      ImageSectionHeader headerInfo = *it;

      strncpy((char*)header.Name, headerInfo.name.str(), 8);
      header.PointerToRelocations = 0;
      header.PointerToLinenumbers = 0;
      header.NumberOfRelocations = 0;
      header.NumberOfLinenumbers = 0;
      header.VirtualAddress = headerInfo.vaddress;
      header.Misc.VirtualSize = headerInfo.memorySize;
      if (headerInfo.fileSize != 0) {
         header.PointerToRawData = tblOffset;
         header.SizeOfRawData = headerInfo.fileSize;
      }
      else {
         header.PointerToRawData = 0;
         header.SizeOfRawData = 0;
      }

      switch (headerInfo.type) {
         case ImageSectionHeader::SectionType::Text:
            header.Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;
            break;
         case ImageSectionHeader::SectionType::Data:
            header.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ;
            break;
         case ImageSectionHeader::SectionType::RData:
            header.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;
            break;
         default:
            // to make compiler happy
            break;
      }

      file.write(&header, IMAGE_SIZEOF_SECTION_HEADER);

      tblOffset += header.SizeOfRawData;
   }

   file.align(image.fileAlignment);

   // write data
   for (auto it = image.imageSections.items.start(); !it.eof(); ++it) {
      MemoryBase* section = (*it).section;

      MemoryReader reader(section);
      file.copyFrom(&reader, section->length());
      if ((*it).isAligned)
         file.align(image.fileAlignment);
   }
}

bool WinNtLinker :: createExecutable(WinNtExecutableImage& image, path_t exePath, path_t basePath)
{
   if (exePath.empty())
      _errorProcessor->raiseInternalError(errEmptyTarget);

   if (!PathUtil::recreatePath(/*nullptr, */exePath))
      return false;

   FileWriter executable(exePath, FileEncoding::Raw, false);
   if (!executable.isOpen())
      return false;

   writeDOSStub(basePath, executable);

   // NOTE : take into account the size of stub
   image.headerSize += executable.length();

   writeExecutableHeader(image, executable);
   writeNtHeader(image, executable);
   writeSections(image, executable);

   return true;
}

bool WinNtLinker :: createDebugFile(ImageProviderBase& provider, WinNtExecutableImage& image, path_t debugFilePath)
{
   FileWriter debugWriter(debugFilePath, FileEncoding::Raw, false);
   if (!debugWriter.isOpen())
      return false;

   MemoryBase* debug = provider.getTargetDebugSection();

   // signature - first 8 bytes
   debugWriter.write(DEBUG_MODULE_SIGNATURE, getlength_pos(DEBUG_MODULE_SIGNATURE));
   debugWriter.align(8);

   // save entry point
   addr_t entryPoint = image.addressSpace.code + image.addressSpace.imageBase + provider.getDebugEntryPoint();

   debugWriter.writePos(debug->length());
   debugWriter.write(&entryPoint, sizeof(addr_t));

   // save breakpoints
   MemoryReader reader(debug);
   debugWriter.copyFrom(&reader, debug->length());

   return true;
}

void WinNtLinker :: prepareNtImage(ImageProviderBase& provider, WinNtExecutableImage& image, PlatformType uiType)
{
   // !! temporal
   image.fileAlignment = FILE_ALIGNMENT;
   image.sectionAlignment = SECTION_ALIGNMENT;
   image.addressSpace.imageBase = IMAGE_BASE;

   _imageFormatter->prepareImage(provider, image.addressSpace, image.imageSections, 
      image.sectionAlignment, 
      image.fileAlignment,
      image.withDebugInfo);

   image.characteristics = IMAGE_FILE_EXECUTABLE_IMAGE;
   image.characteristics |= IMAGE_FILE_LOCAL_SYMS_STRIPPED;
   image.characteristics |= IMAGE_FILE_LINE_NUMS_STRIPPED;

   switch (uiType) {
      case PlatformType::GUI:
         image.subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;
         break;
      case PlatformType::CUI:
      default:
         image.subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
         break;
   }

   image.headerSize = IMAGE_SIZEOF_FILE_HEADER;
   image.headerSize += IMAGE_SIZEOF_SECTION_HEADER * image.imageSections.headers.count();
}

LinkResult WinNtLinker :: run(ProjectBase& project, ImageProviderBase& provider, PlatformType, PlatformType uiType, path_t exeExtension)
{
   bool withDebugMode = project.BoolSetting(ProjectOption::DebugMode, true); // !! temporally by default the debug mode is on

   WinNtExecutableImage image(withDebugMode);

   image.addressSpace.entryPoint = (pos_t)provider.getEntryPoint();
   prepareNtImage(provider, image, uiType);

   PathString exePath(project.PathSetting(ProjectOption::TargetPath));
   exePath.changeExtension(exeExtension);

   if (!createExecutable(image, *exePath, project.PathSetting(ProjectOption::BasePath))) {
      _errorProcessor->raisePathError(errCannotCreate, project.PathSetting(ProjectOption::TargetPath));
   }

   if (withDebugMode) {
      PathString debugFilePath(*exePath);
      debugFilePath.changeExtension("dn");

      if (!createDebugFile(provider, image, *debugFilePath)) {
         _errorProcessor->raisePathError(errCannotCreate, *debugFilePath);
      }
   }

   return{
      image.addressSpace.imageBase + image.addressSpace.code,
      image.addressSpace.imageBase + image.addressSpace.rdata
   };
}
