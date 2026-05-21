//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive Linker class implementation
//		Supported platforms: MacOS
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "macholinker.h"

#include <Availability.h>
#include <mach-o/fixup-chains.h>
#include <mach-o/nlist.h>

#if defined(__unix__) || defined(__MACH__)
#include <sys/stat.h>
#endif

using namespace elena_lang;

constexpr auto DYLINKER_PATH = "/usr/lib/dyld";
constexpr auto LIBSYSTEM_PATH = "/usr/lib/libSystem.B.dylib";
constexpr uint32_t LIBSYSTEM_CURRENT_VERSION = 0x054C0000;
constexpr uint32_t LIBSYSTEM_COMPATIBILITY_VERSION = 0x00010000;
constexpr uint32_t APPLE_LD_VERSION = 0x04F20800;
constexpr pos_t DYLINKER_NAME_OFFSET = sizeof(dylinker_command);
constexpr pos_t DYLIB_NAME_OFFSET = sizeof(dylib_command);
constexpr pos_t RPATH_PATH_OFFSET = sizeof(rpath_command);
constexpr pos_t CODE_SIGNATURE_PAGE_SIZE = 0x1000;
constexpr pos_t CODE_SIGNATURE_PAGE_SHIFT = 12;
constexpr pos_t CODE_SIGNATURE_SUPERBLOB_HEADER_SIZE = 20;
constexpr pos_t CODE_DIRECTORY_HEADER_SIZE = 88;
constexpr pos_t SHA256_HASH_SIZE = 32;

constexpr uint32_t CSMAGIC_CODEDIRECTORY = 0xFADE0C02;
constexpr uint32_t CSMAGIC_EMBEDDED_SIGNATURE = 0xFADE0CC0;
constexpr uint32_t CSSLOT_CODEDIRECTORY = 0;
constexpr uint32_t CS_SUPPORTSEXECSEG = 0x20400;
constexpr uint32_t CS_ADHOC = 0x00000002;
constexpr uint32_t CS_LINKER_SIGNED = 0x00020000;
constexpr uint64_t CS_EXECSEG_MAIN_BINARY = 0x1;
constexpr uint8_t CS_HASHTYPE_SHA256 = 2;

constexpr auto MACOS_LOCAL_RPATH = "@executable_path/bin";

struct SHA256Context
{
   uint8_t data[64];
   uint32_t datalen;
   uint64_t bitlen;
   uint32_t state[8];
};

static uint32_t sha256RotateRight(uint32_t value, uint32_t count)
{
   return (value >> count) | (value << (32 - count));
}

static void sha256Transform(SHA256Context& context, const uint8_t data[])
{
   static constexpr uint32_t constants[64] = {
      0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
      0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
      0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
      0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
      0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
      0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
      0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
      0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
      0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
      0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
      0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
      0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
      0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
      0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
      0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
      0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
   };

   uint32_t dataWords[64];
   for (uint32_t i = 0, j = 0; i < 16; i++, j += 4) {
      dataWords[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | data[j + 3];
   }

   for (uint32_t i = 16; i < 64; i++) {
      uint32_t s0 = sha256RotateRight(dataWords[i - 15], 7) ^ sha256RotateRight(dataWords[i - 15], 18) ^ (dataWords[i - 15] >> 3);
      uint32_t s1 = sha256RotateRight(dataWords[i - 2], 17) ^ sha256RotateRight(dataWords[i - 2], 19) ^ (dataWords[i - 2] >> 10);

      dataWords[i] = dataWords[i - 16] + s0 + dataWords[i - 7] + s1;
   }

   uint32_t a = context.state[0];
   uint32_t b = context.state[1];
   uint32_t c = context.state[2];
   uint32_t d = context.state[3];
   uint32_t e = context.state[4];
   uint32_t f = context.state[5];
   uint32_t g = context.state[6];
   uint32_t h = context.state[7];

   for (uint32_t i = 0; i < 64; i++) {
      uint32_t s1 = sha256RotateRight(e, 6) ^ sha256RotateRight(e, 11) ^ sha256RotateRight(e, 25);
      uint32_t ch = (e & f) ^ ((~e) & g);
      uint32_t temp1 = h + s1 + ch + constants[i] + dataWords[i];
      uint32_t s0 = sha256RotateRight(a, 2) ^ sha256RotateRight(a, 13) ^ sha256RotateRight(a, 22);
      uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
      uint32_t temp2 = s0 + maj;

      h = g;
      g = f;
      f = e;
      e = d + temp1;
      d = c;
      c = b;
      b = a;
      a = temp1 + temp2;
   }

   context.state[0] += a;
   context.state[1] += b;
   context.state[2] += c;
   context.state[3] += d;
   context.state[4] += e;
   context.state[5] += f;
   context.state[6] += g;
   context.state[7] += h;
}

static void sha256Init(SHA256Context& context)
{
   context.datalen = 0;
   context.bitlen = 0;
   context.state[0] = 0x6a09e667;
   context.state[1] = 0xbb67ae85;
   context.state[2] = 0x3c6ef372;
   context.state[3] = 0xa54ff53a;
   context.state[4] = 0x510e527f;
   context.state[5] = 0x9b05688c;
   context.state[6] = 0x1f83d9ab;
   context.state[7] = 0x5be0cd19;
}

static void sha256Update(SHA256Context& context, const uint8_t* data, pos_t length)
{
   for (pos_t i = 0; i < length; i++) {
      context.data[context.datalen] = data[i];
      context.datalen++;
      if (context.datalen == 64) {
         sha256Transform(context, context.data);
         context.bitlen += 512;
         context.datalen = 0;
      }
   }
}

static void sha256Final(SHA256Context& context, uint8_t hash[SHA256_HASH_SIZE])
{
   uint32_t i = context.datalen;

   if (context.datalen < 56) {
      context.data[i++] = 0x80;
      while (i < 56)
         context.data[i++] = 0x00;
   }
   else {
      context.data[i++] = 0x80;
      while (i < 64)
         context.data[i++] = 0x00;
      sha256Transform(context, context.data);
      memset(context.data, 0, 56);
   }

   context.bitlen += context.datalen * 8;
   context.data[63] = context.bitlen;
   context.data[62] = context.bitlen >> 8;
   context.data[61] = context.bitlen >> 16;
   context.data[60] = context.bitlen >> 24;
   context.data[59] = context.bitlen >> 32;
   context.data[58] = context.bitlen >> 40;
   context.data[57] = context.bitlen >> 48;
   context.data[56] = context.bitlen >> 56;
   sha256Transform(context, context.data);

   for (i = 0; i < 4; i++) {
      hash[i] = (context.state[0] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 4] = (context.state[1] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 8] = (context.state[2] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 12] = (context.state[3] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 16] = (context.state[4] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 20] = (context.state[5] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 24] = (context.state[6] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 28] = (context.state[7] >> (24 - i * 8)) & 0x000000ff;
   }
}

static void sha256(const void* data, pos_t length, uint8_t hash[SHA256_HASH_SIZE])
{
   SHA256Context context;
   sha256Init(context);
   sha256Update(context, static_cast<const uint8_t*>(data), length);
   sha256Final(context, hash);
}

static void writeBE32(StreamWriter* writer, uint32_t value)
{
   uint8_t buffer[4] = {
      static_cast<uint8_t>(value >> 24),
      static_cast<uint8_t>(value >> 16),
      static_cast<uint8_t>(value >> 8),
      static_cast<uint8_t>(value)
   };

   writer->write(buffer, 4);
}

static void writeBE64(StreamWriter* writer, uint64_t value)
{
   uint8_t buffer[8] = {
      static_cast<uint8_t>(value >> 56),
      static_cast<uint8_t>(value >> 48),
      static_cast<uint8_t>(value >> 40),
      static_cast<uint8_t>(value >> 32),
      static_cast<uint8_t>(value >> 24),
      static_cast<uint8_t>(value >> 16),
      static_cast<uint8_t>(value >> 8),
      static_cast<uint8_t>(value)
   };

   writer->write(buffer, 8);
}

static pos_t uleb128Size(uint64_t value)
{
   pos_t size = 0;
   do {
      value >>= 7;
      size++;
   } while (value != 0);

   return size;
}

static void writeULEB128(StreamWriter* writer, uint64_t value)
{
   do {
      uint8_t current = value & 0x7F;
      value >>= 7;
      if (value != 0)
         current |= 0x80;

      writer->writeByte(current);
   } while (value != 0);
}

static pos_t getCodeSignatureSlotCount(pos_t codeLimit, pos_t pageSize)
{
   return align(codeLimit, pageSize) / pageSize;
}

static pos_t getCodeDirectoryHashOffset(ustr_t identifier)
{
   return align(CODE_DIRECTORY_HEADER_SIZE + identifier.length_pos() + 1, 4);
}

static pos_t getCodeDirectorySize(ustr_t identifier, pos_t codeLimit, pos_t pageSize)
{
   return getCodeDirectoryHashOffset(identifier) + getCodeSignatureSlotCount(codeLimit, pageSize) * SHA256_HASH_SIZE;
}

static pos_t getCodeSignatureSize(ustr_t identifier, pos_t codeLimit, pos_t pageSize)
{
   return align(CODE_SIGNATURE_SUPERBLOB_HEADER_SIZE + getCodeDirectorySize(identifier, codeLimit, pageSize), 16);
}

static uint32_t encodeMacOSVersion(uint32_t version)
{
   return (version / 10000) << 16 | ((version / 100) % 100) << 8 | (version % 100);
}

static uint32_t getMacOSSDKVersion()
{
#ifdef __MAC_OS_X_VERSION_MAX_ALLOWED
   return encodeMacOSVersion(__MAC_OS_X_VERSION_MAX_ALLOWED);
#else
   return MacOS_11_0_0;
#endif
}

static pos_t loadCommandStringSize(pos_t commandOffset, ustr_t value)
{
   return align(commandOffset + value.length_pos() + 1, 8);
}

static Command* createDylinkerCommand(ustr_t path)
{
   pos_t commandSize = loadCommandStringSize(DYLINKER_NAME_OFFSET, path);
   auto command = new Command(commandSize);
   auto dylinker = command->as<dylinker_command>();

   dylinker->cmd = LC_LOAD_DYLINKER;
   dylinker->cmdsize = commandSize;
   dylinker->name.offset = DYLINKER_NAME_OFFSET;
   strncpy(static_cast<char*>(command->image.get(DYLINKER_NAME_OFFSET)), path.str(), commandSize - DYLINKER_NAME_OFFSET - 1);

   return command;
}

static Command* createDylibCommand(ustr_t path, uint32_t currentVersion = 0, uint32_t compatibilityVersion = 0)
{
   pos_t commandSize = loadCommandStringSize(DYLIB_NAME_OFFSET, path);
   auto command = new Command(commandSize);
   auto dylib = command->as<dylib_command>();

   dylib->cmd = LC_LOAD_DYLIB;
   dylib->cmdsize = commandSize;
   dylib->dylib.name.offset = DYLIB_NAME_OFFSET;
   dylib->dylib.timestamp = 2;
   dylib->dylib.current_version = currentVersion;
   dylib->dylib.compatibility_version = compatibilityVersion;
   strncpy(static_cast<char*>(command->image.get(DYLIB_NAME_OFFSET)), path.str(), commandSize - DYLIB_NAME_OFFSET - 1);

   return command;
}

static Command* createRPathCommand(ustr_t path)
{
   pos_t commandSize = loadCommandStringSize(RPATH_PATH_OFFSET, path);
   auto command = new Command(commandSize);
   auto rpath = command->as<rpath_command>();

   rpath->cmd = LC_RPATH;
   rpath->cmdsize = commandSize;
   rpath->path.offset = RPATH_PATH_OFFSET;
   strncpy(static_cast<char*>(command->image.get(RPATH_PATH_OFFSET)), path.str(), commandSize - RPATH_PATH_OFFSET - 1);

   return command;
}

static bool isLibraryLoaded(MachOExecutableImage& image, ustr_t path)
{
   for (auto it = image.importLibraries.start(); !it.eof(); ++it) {
      if ((*it).compare(path))
         return true;
   }

   return false;
}

static int addImportLibrary(MachOExecutableImage& image, ustr_t path)
{
   int ordinal = 1;
   for (auto it = image.importLibraries.start(); !it.eof(); ++it, ordinal++) {
      if ((*it).compare(path))
         return ordinal;
   }

   image.importLibraries.add(path.clone());

   return image.importLibraries.count();
}

static bool hasRPathImport(MachOExecutableImage& image)
{
   for (auto it = image.importLibraries.start(); !it.eof(); ++it) {
      if ((*it).startsWith("@rpath/"))
         return true;
   }

   return false;
}

static void copyMachOSymbolName(IdentifierString& target, ustr_t symbol)
{
   target.clear();
   if (!symbol.startsWith("_"))
      target.append('_');
   target.append(symbol);
}

static void prepareImportSection(ForwardResolverBase* resolver, ImageProviderBase& provider, MachOExecutableImage& image)
{
   MemoryBase* import = provider.getImportSection();
   MemoryWriter importWriter(import);

   for (auto it = provider.externals(); !it.eof(); ++it) {
      ustr_t referenceName = it.key();
      size_t index = referenceName.findLast('.');
      if (index == NOTFOUND_POS)
         continue;

      IdentifierString library(referenceName, index);
      ustr_t symbol = referenceName + index + 1;
      if (symbol.startsWith("##"))
         symbol += 2;

      if ((*library).compare(RT_FORWARD)) {
         ustr_t resolvedName = resolver->resolveExternal(*library);
         if (!resolvedName.empty())
            library.copy(resolvedName);
      }

      IdentifierString machOSymbol;
      copyMachOSymbolName(machOSymbol, symbol);

      int ordinal = addImportLibrary(image, *library);
      pos_t slotOffset = importWriter.position();
      ref_t importRef = ((ref_t)*it & ~mskAnyRef) | mskImportRef64;

      image.addressMap.importMapping.add(importRef, slotOffset);
      image.imports.add(new MachOImportInfo(*library, *machOSymbol, slotOffset, ordinal, importRef));

      importWriter.writeQWord(0);
   }
}

static Command* createEntryPointCommand(pos_t entryPoint, pos_t stackReserved)
{
   auto command = new Command(sizeof(entry_point_command));
   auto entryPointCommand = command->as<entry_point_command>();

   entryPointCommand->cmd = LC_MAIN;
   entryPointCommand->cmdsize = sizeof(entry_point_command);
   entryPointCommand->entryoff = entryPoint;
   entryPointCommand->stacksize = stackReserved;

   return command;
}

static Command* createBuildVersionCommand()
{
   auto command = new Command(sizeof(build_version_command) + sizeof(build_tool_version));
   auto buildVersion = command->as<build_version_command>();
   auto buildTool = reinterpret_cast<build_tool_version*>(
      static_cast<char*>(command->bytes()) + sizeof(build_version_command));

   buildVersion->cmd = LC_BUILD_VERSION;
   buildVersion->cmdsize = sizeof(build_version_command) + sizeof(build_tool_version);
   buildVersion->platform = PLATFORM_MACOS;
   buildVersion->minos = MacOS_11_0_0;
   buildVersion->sdk = getMacOSSDKVersion();
   buildVersion->ntools = 1;

   buildTool->tool = TOOL_LD;
   buildTool->version = APPLE_LD_VERSION;

   return command;
}

static Command* createUUIDCommand(ustr_t identifier)
{
   auto command = new Command(sizeof(uuid_command));
   auto uuid = command->as<uuid_command>();

   uint8_t hash[SHA256_HASH_SIZE];
   sha256(identifier.str(), identifier.length_pos(), hash);

   uuid->cmd = LC_UUID;
   uuid->cmdsize = sizeof(uuid_command);
   memcpy(uuid->uuid, hash, sizeof(uuid->uuid));
   uuid->uuid[6] = (uuid->uuid[6] & 0x0F) | 0x40;
   uuid->uuid[8] = (uuid->uuid[8] & 0x3F) | 0x80;

   return command;
}

static Command* createCodeSignatureCommand(pos_t offset, pos_t size)
{
   auto command = new Command(sizeof(linkedit_data_command));
   auto linkEditData = command->as<linkedit_data_command>();

   linkEditData->cmd = LC_CODE_SIGNATURE;
   linkEditData->cmdsize = sizeof(linkedit_data_command);
   linkEditData->dataoff = offset;
   linkEditData->datasize = size;

   return command;
}

static Command* createLinkEditDataCommand(uint32_t commandType, pos_t offset, pos_t size)
{
   auto command = new Command(sizeof(linkedit_data_command));
   auto linkEditData = command->as<linkedit_data_command>();

   linkEditData->cmd = commandType;
   linkEditData->cmdsize = sizeof(linkedit_data_command);
   linkEditData->dataoff = offset;
   linkEditData->datasize = size;

   return command;
}

static Command* createDyldInfoCommand(MachOExecutableImage& image)
{
   auto command = new Command(sizeof(dyld_info_command));
   auto dyldInfo = command->as<dyld_info_command>();

   dyldInfo->cmd = LC_DYLD_INFO_ONLY;
   dyldInfo->cmdsize = sizeof(dyld_info_command);
   dyldInfo->rebase_off = image.rebaseInfoOffset;
   dyldInfo->rebase_size = image.rebaseInfoSize;
   dyldInfo->bind_off = image.bindInfoOffset;
   dyldInfo->bind_size = image.bindInfoSize;
   dyldInfo->export_off = image.exportsTrieOffset;
   dyldInfo->export_size = image.exportsTrieSize;

   return command;
}

static Command* createSymtabCommand(MachOExecutableImage& image)
{
   auto command = new Command(sizeof(symtab_command));
   auto symtab = command->as<symtab_command>();

   symtab->cmd = LC_SYMTAB;
   symtab->cmdsize = sizeof(symtab_command);
   symtab->symoff = image.symtabOffset;
   symtab->nsyms = image.symtabCount;
   symtab->stroff = image.stringTableOffset;
   symtab->strsize = image.stringTableSize;

   return command;
}

static Command* createDysymtabCommand(MachOExecutableImage& image)
{
   auto command = new Command(sizeof(dysymtab_command));
   auto dysymtab = command->as<dysymtab_command>();

   dysymtab->cmd = LC_DYSYMTAB;
   dysymtab->cmdsize = sizeof(dysymtab_command);
   dysymtab->iextdefsym = 0;
   dysymtab->nextdefsym = image.definedSymbolCount;
   dysymtab->iundefsym = image.definedSymbolCount;
   dysymtab->nundefsym = image.symtabCount - image.definedSymbolCount;
   dysymtab->indirectsymoff = image.indirectSymtabOffset;
   dysymtab->nindirectsyms = image.indirectSymtabCount;

   return command;
}

static Command* createSourceVersionCommand()
{
   auto command = new Command(sizeof(source_version_command));
   auto sourceVersion = command->as<source_version_command>();

   sourceVersion->cmd = LC_SOURCE_VERSION;
   sourceVersion->cmdsize = sizeof(source_version_command);
   sourceVersion->version = 0;

   return command;
}

static pos_t getExecutableSectionSize(MachOExecutableImage& image)
{
   int headerIndex = 1;
   for (auto it = image.imageSections.headers.start(); !it.eof(); ++it) {
      if ((*it).name.compare(__TEXT_SEGMENT)) {
         for (auto itemIt = image.imageSections.items.start(); !itemIt.eof(); ++itemIt) {
            if (itemIt.key() == headerIndex && (*itemIt).section != nullptr)
               return (*itemIt).section->length();
         }

         return (*it).fileSize;
      }

      headerIndex++;
   }

   return 0;
}

static pos_t getExportsTrieSize(pos_t entryPoint)
{
   constexpr auto mhExecuteHeaderSuffix = "_mh_execute_header";
   constexpr auto mainSuffix = "main";

   pos_t mhTerminalSize = uleb128Size(0) + uleb128Size(0);
   pos_t mainTerminalSize = uleb128Size(0) + uleb128Size(entryPoint);
   pos_t mhNodeSize = uleb128Size(mhTerminalSize) + mhTerminalSize + 1;
   pos_t mainNodeSize = uleb128Size(mainTerminalSize) + mainTerminalSize + 1;
   pos_t rootSize = 1 + 1 + 2 + 1;
   pos_t branchSize = 1 + 1
      + getlength(mhExecuteHeaderSuffix) + 1 + 1
      + getlength(mainSuffix) + 1 + 1;

   return align(rootSize + branchSize + mhNodeSize + mainNodeSize, 8);
}

static pos_t getFunctionStartsSize(pos_t entryPoint)
{
   return align(uleb128Size(entryPoint) + 1, 8);
}

static pos_t getSegmentIndex(MachOExecutableImage& image, ustr_t segmentName)
{
   pos_t index = 0;
   for (auto it = image.imageSections.headers.start(); !it.eof(); ++it, index++) {
      if ((*it).name.compare(segmentName))
         return index;
   }

   return 0;
}

static pos_t getSegmentRelativeOffset(MachOExecutableImage& image, ustr_t segmentName, pos_t offset)
{
   for (auto it = image.imageSections.headers.start(); !it.eof(); ++it) {
      if ((*it).name.compare(segmentName))
         return offset - (*it).vaddress;
   }

   return offset;
}

static pos_t getSegmentIndexForOffset(MachOExecutableImage& image, pos_t offset)
{
   pos_t index = 0;
   for (auto it = image.imageSections.headers.start(); !it.eof(); ++it, index++) {
      if (offset >= (*it).vaddress && offset < (*it).vaddress + (*it).memorySize)
         return index;
   }

   return 0;
}

static pos_t getSegmentRelativeOffset(MachOExecutableImage& image, pos_t offset)
{
   for (auto it = image.imageSections.headers.start(); !it.eof(); ++it) {
      if (offset >= (*it).vaddress && offset < (*it).vaddress + (*it).memorySize)
         return offset - (*it).vaddress;
   }

   return offset;
}

static bool isRebaseReference(ref_t mask)
{
   switch (mask) {
      case mskCodeRef64:
      case mskRDataRef64:
      case mskDataRef64:
      case mskImportRef64:
      case mskMBDataRef64:
      case mskMDataRef64:
      case mskStatDataRef64:
      case mskTLSRef64:
         return true;
      default:
         return false;
   }
}

static void addRebaseOffset(MachOExecutableImage& image, pos_t offset)
{
   for (auto it = image.rebaseOffsets.start(); !it.eof(); ++it) {
      if (*it == offset)
         return;
   }

   image.rebaseOffsets.add(offset);
}

static void collectRebaseOffsets(MachOExecutableImage& image, MemoryBase* section, pos_t sectionOffset)
{
   if (section == nullptr || section->getReferences() == nullptr)
      return;

   for (auto it = RelocationMap::Iterator(section->getReferences()); !it.eof(); ++it) {
      ref_t mask = it.key() & mskAnyRef;
      if (isRebaseReference(mask))
         addRebaseOffset(image, sectionOffset + *it);
   }
}

static void prepareRebaseInfo(ImageProviderBase& provider, MachOExecutableImage& image)
{
   collectRebaseOffsets(image, provider.getTextSection(), image.addressMap.code);
   collectRebaseOffsets(image, provider.getADataSection(), image.addressMap.adata);
   collectRebaseOffsets(image, provider.getMDataSection(), image.addressMap.mdata);
   collectRebaseOffsets(image, provider.getMBDataSection(), image.addressMap.mbdata);
   collectRebaseOffsets(image, provider.getRDataSection(), image.addressMap.rdata);
   collectRebaseOffsets(image, provider.getImportSection(), image.addressMap.import);
   collectRebaseOffsets(image, provider.getDataSection(), image.addressMap.data);
   collectRebaseOffsets(image, provider.getStatSection(), image.addressMap.stat);
}

static pos_t getRebaseInfoSize(MachOExecutableImage& image)
{
   if (image.rebaseOffsets.count() == 0)
      return 0;

   pos_t size = 1; // REBASE_OPCODE_SET_TYPE_IMM

   for (auto it = image.rebaseOffsets.start(); !it.eof(); ++it) {
      pos_t segmentIndex = getSegmentIndexForOffset(image, *it);
      pos_t segmentOffset = getSegmentRelativeOffset(image, *it);

      assert(segmentIndex < 16);
      size += 1 + uleb128Size(segmentOffset);
      size += 1; // REBASE_OPCODE_DO_REBASE_IMM_TIMES
   }

   return align(size + 1, 8); // REBASE_OPCODE_DONE
}

static pos_t getBindInfoSize(MachOExecutableImage& image)
{
   if (image.imports.count() == 0)
      return 0;

   pos_t dataSegmentIndex = getSegmentIndex(image, __DATA_SEGMENT);
   pos_t size = 1; // BIND_OPCODE_DONE

   for (auto it = image.imports.start(); !it.eof(); ++it) {
      MachOImportInfo* import = *it;
      pos_t slotOffset = getSegmentRelativeOffset(image, __DATA_SEGMENT, image.addressMap.import + import->slotOffset);

      size += import->libraryOrdinal < 16 ? 1 : 1 + uleb128Size(import->libraryOrdinal);
      size += 1 + import->symbol.length_pos() + 1;
      size += 1;
      size += 1 + uleb128Size(slotOffset);
      size += 1;

      assert(dataSegmentIndex < 16);
   }

   return size;
}

static pos_t getStringTableSize(MachOExecutableImage& image)
{
   pos_t size = 1 + getlength("__mh_execute_header") + 1 + getlength("_main") + 1;

   for (auto it = image.imports.start(); !it.eof(); ++it) {
      size += (*it)->symbol.length_pos() + 1;
   }

   return align(size, 8);
}

static void prepareLinkEditData(MachOExecutableImage& image)
{
   pos_t offset = image.addressMap.imageSize;

   image.linkEditOffset = offset;
   image.dyldFixupsOffset = 0;
   image.dyldFixupsSize = 0;

   image.rebaseInfoOffset = 0;
   image.rebaseInfoSize = getRebaseInfoSize(image);
   if (image.rebaseInfoSize != 0) {
      image.rebaseInfoOffset = offset;
      offset += image.rebaseInfoSize;
      offset = align(offset, 8);
   }

   image.bindInfoOffset = 0;
   image.bindInfoSize = getBindInfoSize(image);
   if (image.bindInfoSize != 0) {
      image.bindInfoOffset = offset;
      offset += image.bindInfoSize;
      offset = align(offset, 8);
   }

   image.exportsTrieOffset = offset;
   image.exportsTrieSize = getExportsTrieSize(image.addressMap.entryPoint);
   offset += image.exportsTrieSize;

   image.functionStartsOffset = offset;
   image.functionStartsSize = getFunctionStartsSize(image.addressMap.entryPoint);
   offset += image.functionStartsSize;

   image.dataInCodeOffset = offset;
   image.dataInCodeSize = 0;

   image.symtabOffset = offset;
   image.definedSymbolCount = 2;
   image.symtabCount = image.definedSymbolCount + image.imports.count();
   offset += image.symtabCount * sizeof(nlist_64);

   image.indirectSymtabOffset = 0;
   image.indirectSymtabCount = image.imports.count();
   if (image.indirectSymtabCount != 0) {
      image.indirectSymtabOffset = offset;
      offset += image.indirectSymtabCount * sizeof(uint32_t);
   }

   image.stringTableOffset = offset;
   image.stringTableSize = getStringTableSize(image);
   offset += image.stringTableSize;

   image.linkEditSize = offset - image.linkEditOffset;
}

static void prepareCodeSignature(MachOExecutableImage& image)
{
   image.codeSignaturePageSize = CODE_SIGNATURE_PAGE_SIZE;
   image.codeSignatureOffset = align(image.linkEditOffset + image.linkEditSize, image.fileAlignment);
   image.codeSignatureSize = getCodeSignatureSize(
      *image.codeSignatureIdentifier,
      image.codeSignatureOffset,
      image.codeSignaturePageSize);

   for (auto it = image.imageSections.headers.start(); !it.eof(); ++it) {
      if ((*it).name.compare(__LINKEDIT_SEGMENT)) {
         (*it).fileSize = image.codeSignatureOffset - image.linkEditOffset + image.codeSignatureSize;
         (*it).memorySize = align((*it).fileSize, image.sectionAlignment);
         break;
      }
   }
}

static void writeRebaseInfo(MachOExecutableImage& image, StreamWriter* file)
{
   if (image.rebaseInfoSize == 0)
      return;

   if (file->position() < image.rebaseInfoOffset)
      file->writeBytes(0, image.rebaseInfoOffset - file->position());

   pos_t start = file->position();
   file->writeByte(REBASE_OPCODE_SET_TYPE_IMM | REBASE_TYPE_POINTER);

   for (auto it = image.rebaseOffsets.start(); !it.eof(); ++it) {
      pos_t segmentIndex = getSegmentIndexForOffset(image, *it);
      pos_t segmentOffset = getSegmentRelativeOffset(image, *it);

      assert(segmentIndex < 16);
      file->writeByte(REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB | segmentIndex);
      writeULEB128(file, segmentOffset);
      file->writeByte(REBASE_OPCODE_DO_REBASE_IMM_TIMES | 1);
   }

   file->writeByte(REBASE_OPCODE_DONE);
   file->writeBytes(0, start + image.rebaseInfoSize - file->position());
}

static void writeBindLibraryOrdinal(StreamWriter* file, int ordinal)
{
   if (ordinal < 16) {
      file->writeByte(BIND_OPCODE_SET_DYLIB_ORDINAL_IMM | ordinal);
   }
   else {
      file->writeByte(BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB);
      writeULEB128(file, ordinal);
   }
}

static void writeBindInfo(MachOExecutableImage& image, StreamWriter* file)
{
   if (image.bindInfoSize == 0)
      return;

   if (file->position() < image.bindInfoOffset)
      file->writeBytes(0, image.bindInfoOffset - file->position());

   pos_t dataSegmentIndex = getSegmentIndex(image, __DATA_SEGMENT);
   assert(dataSegmentIndex < 16);

   pos_t start = file->position();
   for (auto it = image.imports.start(); !it.eof(); ++it) {
      MachOImportInfo* import = *it;
      pos_t slotOffset = getSegmentRelativeOffset(image, __DATA_SEGMENT, image.addressMap.import + import->slotOffset);

      writeBindLibraryOrdinal(file, import->libraryOrdinal);
      file->writeByte(BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM);
      file->writeString(*import->symbol);
      file->writeByte(BIND_OPCODE_SET_TYPE_IMM | BIND_TYPE_POINTER);
      file->writeByte(BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB | dataSegmentIndex);
      writeULEB128(file, slotOffset);
      file->writeByte(BIND_OPCODE_DO_BIND);
   }

   file->writeByte(BIND_OPCODE_DONE);
   assert(file->position() == start + image.bindInfoSize);
}

void MachOLinker :: writeSection(StreamWriter* file, MemoryBase* section)
{
   if (section != nullptr) {
      MemoryReader reader(section);
      file->copyFrom(&reader, section->length());
   }
}


void MachOLinker :: writeSegments(MachOExecutableImage& image, StreamWriter* file)
{
   int headerIndex = 0;
   pos_t itemIndex = 0;

   for (auto it = image.imageSections.items.start(); !it.eof(); ++it) {
      if (headerIndex != it.key()) {
         headerIndex = it.key();
         itemIndex = 0;
      }

      ImageSectionHeader header = image.imageSections.headers.get(headerIndex);
      MachOSectionInfo sectionInfo = getSectionInfo(header, itemIndex, image.addressMap);
      if (sectionInfo.fileOffset != 0 && file->position() < sectionInfo.fileOffset)
         file->writeBytes(0, sectionInfo.fileOffset - file->position());

      writeSection(file, (*it).section);
      if ((*it).isAligned)
         file->writeBytes(0, align(file->position(), image.fileAlignment) - file->position());

      itemIndex++;
   }
   if (file->position() < image.linkEditOffset)
      file->writeBytes(0, image.linkEditOffset - file->position());
   file->writeBytes(0, align(file->position(), image.fileAlignment) - file->position());
}

void MachOLinker :: writeLinkEditData(MachOExecutableImage& image, StreamWriter* file)
{
   if (image.dyldFixupsSize != 0) {
      if (file->position() < image.dyldFixupsOffset)
         file->writeBytes(0, image.dyldFixupsOffset - file->position());

      dyld_chained_fixups_header fixupsHeader = {};
      fixupsHeader.fixups_version = 0;
      fixupsHeader.starts_offset = 0x20;
      fixupsHeader.imports_offset = 0x30;
      fixupsHeader.symbols_offset = 0x30;
      fixupsHeader.imports_count = 0;
      fixupsHeader.imports_format = DYLD_CHAINED_IMPORT;
      fixupsHeader.symbols_format = 0;
      file->write(&fixupsHeader, sizeof(fixupsHeader));
      file->writeBytes(0, fixupsHeader.starts_offset - sizeof(fixupsHeader));

      uint32_t segmentCount = image.imageSections.headers.count();
      uint32_t emptySegmentInfoOffset = 0;
      file->write(&segmentCount, sizeof(segmentCount));
      for (uint32_t i = 0; i < segmentCount; i++) {
         file->write(&emptySegmentInfoOffset, sizeof(emptySegmentInfoOffset));
      }
      file->writeBytes(0, image.dyldFixupsOffset + image.dyldFixupsSize - file->position());
   }

   writeRebaseInfo(image, file);
   writeBindInfo(image, file);

   if (file->position() < image.exportsTrieOffset)
      file->writeBytes(0, image.exportsTrieOffset - file->position());

   constexpr auto mhExecuteHeaderSuffix = "_mh_execute_header";
   constexpr auto mainSuffix = "main";
   pos_t rootOffset = 0;
   pos_t branchOffset = 5;
   pos_t branchSize = 1 + 1
      + getlength(mhExecuteHeaderSuffix) + 1 + 1
      + getlength(mainSuffix) + 1 + 1;
   pos_t mhNodeOffset = branchOffset + branchSize;
   pos_t mhTerminalSize = uleb128Size(0) + uleb128Size(0);
   pos_t mhNodeSize = uleb128Size(mhTerminalSize) + mhTerminalSize + 1;
   pos_t mainNodeOffset = mhNodeOffset + mhNodeSize;
   pos_t mainTerminalSize = uleb128Size(0) + uleb128Size(image.addressMap.entryPoint);

   writeULEB128(file, 0);
   file->writeByte(1);
   file->writeString("_");
   writeULEB128(file, branchOffset);

   assert(file->position() == image.exportsTrieOffset + branchOffset);
   writeULEB128(file, 0);
   file->writeByte(2);
   file->writeString(mhExecuteHeaderSuffix);
   writeULEB128(file, mhNodeOffset);
   file->writeString(mainSuffix);
   writeULEB128(file, mainNodeOffset);

   assert(file->position() == image.exportsTrieOffset + mhNodeOffset);
   writeULEB128(file, mhTerminalSize);
   writeULEB128(file, 0);
   writeULEB128(file, rootOffset);
   file->writeByte(0);

   assert(file->position() == image.exportsTrieOffset + mainNodeOffset);
   writeULEB128(file, mainTerminalSize);
   writeULEB128(file, 0);
   writeULEB128(file, image.addressMap.entryPoint);
   file->writeByte(0);
   file->writeBytes(0, image.exportsTrieOffset + image.exportsTrieSize - file->position());

   if (file->position() < image.functionStartsOffset)
      file->writeBytes(0, image.functionStartsOffset - file->position());
   writeULEB128(file, image.addressMap.entryPoint);
   file->writeByte(0);
   file->writeBytes(0, image.functionStartsOffset + image.functionStartsSize - file->position());

   if (file->position() < image.symtabOffset)
      file->writeBytes(0, image.symtabOffset - file->position());

   nlist_64 symbol = {};
   symbol.n_un.n_strx = 1;
   symbol.n_type = N_SECT | N_EXT;
   symbol.n_sect = 1;
   symbol.n_desc = REFERENCED_DYNAMICALLY;
   symbol.n_value = image.addressMap.imageBase;
   file->write(&symbol, sizeof(symbol));

   symbol = {};
   symbol.n_un.n_strx = 1 + getlength("__mh_execute_header") + 1;
   symbol.n_type = N_SECT | N_EXT;
   symbol.n_sect = 1;
   symbol.n_value = image.addressMap.imageBase + image.addressMap.entryPoint;
   file->write(&symbol, sizeof(symbol));

   uint32_t stringIndex = 1 + getlength("__mh_execute_header") + 1 + getlength("_main") + 1;
   uint32_t symbolIndex = image.definedSymbolCount;
   for (auto it = image.imports.start(); !it.eof(); ++it, symbolIndex++) {
      MachOImportInfo* import = *it;

      symbol = {};
      symbol.n_un.n_strx = stringIndex;
      symbol.n_type = N_UNDF | N_EXT;
      symbol.n_sect = NO_SECT;
      symbol.n_desc = REFERENCE_FLAG_UNDEFINED_NON_LAZY;
      SET_LIBRARY_ORDINAL(symbol.n_desc, import->libraryOrdinal);
      file->write(&symbol, sizeof(symbol));

      stringIndex += import->symbol.length_pos() + 1;
   }

   if (file->position() < image.indirectSymtabOffset)
      file->writeBytes(0, image.indirectSymtabOffset - file->position());

   symbolIndex = image.definedSymbolCount;
   for (auto it = image.imports.start(); !it.eof(); ++it, symbolIndex++) {
      file->writeDWord(symbolIndex);
   }

   if (file->position() < image.stringTableOffset)
      file->writeBytes(0, image.stringTableOffset - file->position());
   file->writeByte(0);
   file->writeString("__mh_execute_header");
   file->writeString("_main");
   for (auto it = image.imports.start(); !it.eof(); ++it) {
      file->writeString(*(*it)->symbol);
   }
   file->writeBytes(0, image.stringTableOffset + image.stringTableSize - file->position());

   if (file->position() < image.codeSignatureOffset)
      file->writeBytes(0, image.codeSignatureOffset - file->position());
}

void MachOLinker :: writeCodeSignature(MachOExecutableImage& image, MemoryDump& executable, StreamWriter* file)
{
   pos_t codeLimit = image.codeSignatureOffset;
   pos_t codeSlots = getCodeSignatureSlotCount(codeLimit, image.codeSignaturePageSize);
   pos_t codeDirectoryHashOffset = getCodeDirectoryHashOffset(*image.codeSignatureIdentifier);
   pos_t codeDirectorySize = getCodeDirectorySize(
      *image.codeSignatureIdentifier,
      codeLimit,
      image.codeSignaturePageSize);

   MemoryDump signature(image.codeSignatureSize);
   MemoryWriter signatureWriter(&signature, 0);

   writeBE32(&signatureWriter, CSMAGIC_EMBEDDED_SIGNATURE);
   writeBE32(&signatureWriter, image.codeSignatureSize);
   writeBE32(&signatureWriter, 1);
   writeBE32(&signatureWriter, CSSLOT_CODEDIRECTORY);
   writeBE32(&signatureWriter, CODE_SIGNATURE_SUPERBLOB_HEADER_SIZE);

   writeBE32(&signatureWriter, CSMAGIC_CODEDIRECTORY);
   writeBE32(&signatureWriter, codeDirectorySize);
   writeBE32(&signatureWriter, CS_SUPPORTSEXECSEG);
   writeBE32(&signatureWriter, CS_ADHOC | CS_LINKER_SIGNED);
   writeBE32(&signatureWriter, codeDirectoryHashOffset);
   writeBE32(&signatureWriter, CODE_DIRECTORY_HEADER_SIZE);
   writeBE32(&signatureWriter, 0);
   writeBE32(&signatureWriter, codeSlots);
   writeBE32(&signatureWriter, codeLimit);
   signatureWriter.writeByte(SHA256_HASH_SIZE);
   signatureWriter.writeByte(CS_HASHTYPE_SHA256);
   signatureWriter.writeByte(0);
   signatureWriter.writeByte(CODE_SIGNATURE_PAGE_SHIFT);
   writeBE32(&signatureWriter, 0);
   writeBE32(&signatureWriter, 0);
   writeBE32(&signatureWriter, 0);
   writeBE32(&signatureWriter, 0);
   writeBE64(&signatureWriter, 0);
   writeBE64(&signatureWriter, 0);
   writeBE64(&signatureWriter, getExecutableSectionSize(image));
   writeBE64(&signatureWriter, CS_EXECSEG_MAIN_BINARY);

   signatureWriter.writeString(*image.codeSignatureIdentifier);
   signatureWriter.align(4, 0);

   uint8_t hash[SHA256_HASH_SIZE];
   for (pos_t slot = 0; slot < codeSlots; slot++) {
      pos_t offset = slot * image.codeSignaturePageSize;
      pos_t length = image.codeSignaturePageSize;
      if (offset + length > codeLimit)
         length = codeLimit - offset;

      sha256(executable.get(offset), length, hash);
      signatureWriter.write(hash, SHA256_HASH_SIZE);
   }

   signatureWriter.align(16, 0);
   if (signatureWriter.position() < image.codeSignatureSize) {
      signatureWriter.writeBytes(0, image.codeSignatureSize - signatureWriter.position());
   }

   MemoryReader reader(&signature);
   file->copyFrom(&reader, image.codeSignatureSize);
}

bool MachOLinker :: createExecutable(MachOExecutableImage& image, path_t exePath)
{
   if (exePath.empty())
      _errorProcessor->raiseInternalError(errEmptyTarget);

   if (!PathUtil::recreatePath(/*nullptr, */exePath))
      return false;

   MemoryDump executableImage;
   MemoryWriter executable(&executableImage, 0);

   writeMachOHeader(image, &executable/*, ph_length */);

   // write commands
   for (auto command_it = image.commands.start(); !command_it.eof(); ++command_it) {
      Command* command = *command_it;

      executable.write(command->bytes(), command->size());
   }

   // write sections, linkedit data and the embedded code signature
   writeSegments(image, &executable);
   writeLinkEditData(image, &executable);
   writeCodeSignature(image, executableImage, &executable);

   FileWriter file(exePath, FileEncoding::Raw, false);
   if (!file.isOpen())
      return false;

   MemoryReader reader(&executableImage);
   file.copyFrom(&reader, executableImage.length());

#if defined(__unix__) || defined(__MACH__)
   chmod(exePath.str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif

   return true;
}

void MachOLinker :: addCommand(MachOExecutableImage& image, Command* command)
{
   image.commands.add(command);
   image.totalCommandSize += command->size();
}

MachOSectionInfo MachOLinker :: getSectionInfo(ImageSectionHeader& header, pos_t itemIndex,
   AddressSpace& addressMap)
{
   if (header.name.compare(__TEXT_SEGMENT)) {
      switch (itemIndex) {
         case 0:
            return { "__text", addressMap.code };
         default:
            break;
      }
   }
   else if (header.name.compare(__DATA_CONST_SEGMENT)) {
      switch (itemIndex) {
         case 0:
            return { "__adata", addressMap.adata };
         case 1:
            return { "__mdata", addressMap.mdata };
         case 2:
            return { "__mbdata", addressMap.mbdata };
         case 3:
            return { "__const", addressMap.rdata };
         default:
            break;
      }
   }
   else if (header.name.compare(__DATA_SEGMENT)) {
      switch (itemIndex) {
         case 0:
            return { "__import", addressMap.import };
         case 1:
            return { "__data", addressMap.data };
         case 2:
            return { "__stat", addressMap.stat };
         default:
            break;
      }
   }

   return { nullptr, 0 };
}

void MachOLinker :: prepareCommands(MachOExecutableImage& image)
{
   int headerIndex = 1;
   for (auto it = image.imageSections.headers.start(); !it.eof(); ++it) {
      ImageSectionHeader header = *it;

      Command* command = createSegmentCommand(header, headerIndex, image.imageSections, image.addressMap);
      addCommand(image, command);
      headerIndex++;
   }

   addCommand(image, createDyldInfoCommand(image));
   addCommand(image, createSymtabCommand(image));
   addCommand(image, createDysymtabCommand(image));
   addCommand(image, createDylinkerCommand(DYLINKER_PATH));
   addCommand(image, createUUIDCommand(*image.codeSignatureIdentifier));
   addCommand(image, createBuildVersionCommand());
   addCommand(image, createSourceVersionCommand());
   addCommand(image, createEntryPointCommand(image.addressMap.entryPoint, image.stackReserved));
   for (auto it = image.importLibraries.start(); !it.eof(); ++it) {
      if ((*it).compare(LIBSYSTEM_PATH)) {
         addCommand(image, createDylibCommand(*it, LIBSYSTEM_CURRENT_VERSION, LIBSYSTEM_COMPATIBILITY_VERSION));
      }
      else addCommand(image, createDylibCommand(*it));
   }
   if (!isLibraryLoaded(image, LIBSYSTEM_PATH))
      addCommand(image, createDylibCommand(LIBSYSTEM_PATH, LIBSYSTEM_CURRENT_VERSION, LIBSYSTEM_COMPATIBILITY_VERSION));
   if (hasRPathImport(image))
      addCommand(image, createRPathCommand(MACOS_LOCAL_RPATH));
   addCommand(image, createLinkEditDataCommand(LC_FUNCTION_STARTS, image.functionStartsOffset, image.functionStartsSize));
   addCommand(image, createLinkEditDataCommand(LC_DATA_IN_CODE, image.dataInCodeOffset, image.dataInCodeSize));
   addCommand(image, createCodeSignatureCommand(image.codeSignatureOffset, image.codeSignatureSize));
}

void MachOLinker :: prepareMachOImage(ForwardResolverBase* resolver, ImageProviderBase& provider, MachOExecutableImage& image)
{
   image.flags |= MH_NOUNDEFS;
   image.flags |= MH_DYLDLINK;
   image.flags |= MH_TWOLEVEL;
   image.flags |= MH_PIE;
   image.stackReserved = provider.getStackReserved();

   if (!image.sectionAlignment)
      image.sectionAlignment = SECTION_ALIGNMENT;

   if (!image.fileAlignment)
      image.fileAlignment = FILE_ALIGNMENT;

   image.addressMap.imageBase = MACHO64_IMAGE_BASE;
   image.addressMap.headerSize = image.sectionAlignment;

   prepareImportSection(resolver, provider, image);

   _imageFormatter->prepareImage(provider, image.addressMap, image.imageSections,
      image.sectionAlignment,
      image.fileAlignment,
      image.withDebugInfo);

   prepareRebaseInfo(provider, image);
   prepareLinkEditData(image);
   prepareCodeSignature(image);
   prepareCommands(image);
}

LinkResult MachOLinker :: run(ProjectBase& project, ImageProviderBase& provider, PlatformType osType, PlatformType, path_t exeExtension)
{
   bool withDebugMode = project.BoolSetting(ProjectOption::DebugMode, true);
   MachOExecutableImage image(withDebugMode);

   PathString exePath(project.PathSetting(ProjectOption::TargetPath));
   if (!exeExtension.empty())
      exePath.changeExtension(exeExtension);

   FileNameString identifier(*exePath);
   image.codeSignatureIdentifier.copy(*identifier);

   image.addressMap.entryPoint = (pos_t)provider.getEntryPoint();
   prepareMachOImage(&project, provider, image/*, calcHeaderSize()*/);

   if (!createExecutable(image, *exePath)) {
      _errorProcessor->raisePathError(errCannotCreate, project.PathSetting(ProjectOption::TargetPath));
   }

   //if (withDebugMode) {
   //   PathString debugFilePath(*exePath);
   //   debugFilePath.changeExtension("dn");

   //   if (!createDebugFile(provider, image, *debugFilePath)) {
   //      _errorProcessor->raisePathError(errCannotCreate, *debugFilePath);
   //   }
   //}

   // !! temporal stub
   return {};
}
