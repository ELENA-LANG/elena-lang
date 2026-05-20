//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains relocation functions
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

using namespace elena_lang;

inline addr_t arm64resolvePageReference(ref_t mask, ref_t reference, AddressSpace* space)
{
   addr_t base = space->imageBase + reference;

   switch (mask) {
      case mskCodeRelRef32Hi4k:
      case mskCodeRef32Lo12:
      case mskCodeRef32Lo12_8:
         return base + space->code;
      case mskRDataRelRef32Hi4k:
      case mskRDataRef32Lo12:
      case mskRDataRef32Lo12_8:
         return base + space->rdata;
      case mskDataRelRef32Hi4k:
      case mskDataRef32Lo12:
      case mskDataRef32Lo12_8:
         return base + space->data;
      case mskMDataRelRef32Hi4k:
      case mskMDataRef32Lo12:
      case mskMDataRef32Lo12_8:
         return base + space->mdata;
      case mskStatDataRelRef32Hi4k:
      case mskStatDataRef32Lo12:
      case mskStatDataRef32Lo12_8:
         return base + space->stat;
      default:
         return base;
   }
}

inline void arm64relocatePage(pos_t pos, void* address, addr_t target, AddressSpace* space)
{
   addr_t codeAddress = space->imageBase + space->code + pos;
   long long disp = ((long long)(target >> 12) - (long long)(codeAddress >> 12));
   unsigned int imm = (unsigned int)disp & 0x1FFFFF;
   unsigned int opcode = *(unsigned int*)address;

   opcode |= (((imm >> 2) & 0x7FFFF) << 5);
   opcode |= ((imm & 0x3) << 29);

   *(unsigned int*)address = opcode;
}

inline void arm64relocatePageOffset(void* address, addr_t target, int shift)
{
   unsigned int opcode = *(unsigned int*)address;

   opcode += (unsigned int)(((target & 0xFFF) >> shift) << 10);

   *(unsigned int*)address = opcode;
}

inline void arm64relocate(pos_t pos, ref_t mask, ref_t reference, void* address, AddressSpace* space)
{
   addr_t base = space->imageBase + reference;
   switch (mask) {
      case mskCodeRef32:
         *(unsigned int*)address += (unsigned int)(base + space->code);
         break;
      case mskCodeRelRef32:
         *(unsigned int*)address += (unsigned int)(reference - pos - 4);
         break;
      case mskCodeRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->code);
         break;
      case mskCodeRelRef32Hi4k:
      case mskRDataRelRef32Hi4k:
      case mskDataRelRef32Hi4k:
      case mskMDataRelRef32Hi4k:
      case mskStatDataRelRef32Hi4k:
         arm64relocatePage(pos, address, arm64resolvePageReference(mask, reference, space), space);
         break;
      case mskCodeRef32Lo12:
      case mskRDataRef32Lo12:
      case mskDataRef32Lo12:
      case mskMDataRef32Lo12:
      case mskStatDataRef32Lo12:
         arm64relocatePageOffset(address, arm64resolvePageReference(mask, reference, space), 0);
         break;
      case mskCodeRef32Lo12_8:
      case mskRDataRef32Lo12_8:
      case mskDataRef32Lo12_8:
      case mskMDataRef32Lo12_8:
      case mskStatDataRef32Lo12_8:
         arm64relocatePageOffset(address, arm64resolvePageReference(mask, reference, space), 3);
         break;
      //case mskImportRelRef32:
      //{
      //   addr_t tableAddress = space->import + space->imageBase + space->importMapping.get(reference | mask);
      //   addr_t codeAddress = space->code + space->imageBase + pos + 4;

      //   *(unsigned int*)address += (unsigned int)(tableAddress - codeAddress);
      //   break;
      //}
      case mskImportRef32Hi:
      {
         unsigned int opcode = *(unsigned int*)address;

         addr_t addr = space->imageBase + space->importMapping.get(reference | mskImportRef64)
            + space->import;

         opcode |= (addr >> 16) << 5;

         *(unsigned int*)address = opcode;
         break;
      }
      case mskImportRef32Lo:
      {
         unsigned int opcode = *(unsigned int*)address;

         addr_t addr = space->imageBase + space->importMapping.get(reference | mskImportRef64)
            + space->import;

         opcode += (addr & 0xFFFF) << 5;

         *(unsigned int*)address = opcode;
         break;
      }
      case mskImportRelRef32Hi4k:
      {
         addr_t tableAddress = (space->import + space->imageBase + 
            space->importMapping.get(reference | mskImportRef64)) >> 12;
         addr_t codeAddress = (space->code + space->imageBase + pos + 4) >> 12;

         unsigned int disp = (unsigned int)(tableAddress - codeAddress);

         unsigned int opcode = *(unsigned int*)address;

         opcode |= (((disp >> 2) & 0x7FFFF) << 5);
         opcode |= ((disp & 0x3) << 29);

         *(unsigned int*)address = opcode;
         break;
      }
      case mskImportRef32Lo12:
      {
         unsigned int opcode = *(unsigned int*)address;

         addr_t addr = space->imageBase + space->importMapping.get(reference | mskImportRef64)
            + space->import;

         opcode += (addr & 0xFFF) << 10;

         *(unsigned int*)address = opcode;
         break;
      }
      case mskImportRef32Lo12_8:
      {
         unsigned int opcode = *(unsigned int*)address;

         addr_t addr = space->imageBase + space->importMapping.get(reference | mskImportRef64)
            + space->import;

         opcode += ((addr & 0xFFF) >> 3) << 10;

         *(unsigned int*)address = opcode;
         break;
      }
      case mskImportRef64:
      {
         addr_t base = space->imageBase + space->importMapping.get(reference | mask);
         *(unsigned long long*)address += base + space->import;

         break;
      }
      case mskRDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->rdata);
         break;
      case mskDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->data);
         break;
      case mskStatDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->stat);
         break;
      case mskStatDataRef32Hi:
      {
         unsigned int opcode = *(unsigned int*)address;
         addr_t addr = (base + space->stat) >> 16;

         opcode |= addr << 5;

         *(unsigned int*)address = opcode;
         break;
      }
      case mskStatDataRef32Lo:
      {
         unsigned int opcode = *(unsigned int*)address;
         addr_t addr = (base + space->stat) & 0xFFFF;

         opcode |= (addr << 5);

         *(unsigned int*)address = opcode;
         break;
      }
      case mskMDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->mdata);
         break;
      case mskMBDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->mbdata);
         break;
      case mskRDataRef32Hi:
      {
         unsigned int opcode = *(unsigned int*)address;
         addr_t addr = (base + space->rdata) >> 16;

         opcode |= addr << 5;

         *(unsigned int*)address = opcode;
         break;
      }
      case mskRDataRef32Lo:
      {
         unsigned int opcode = *(unsigned int*)address;
         addr_t addr = (base + space->rdata) & 0xFFFF;

         opcode |= (addr << 5);

         *(unsigned int*)address = opcode;
         break;
      }
      case mskMDataRef32Hi:
      {
         unsigned int opcode = *(unsigned int*)address;
         addr_t addr = (base + space->mdata) >> 16;

         opcode |= addr << 5;

         *(unsigned int*)address = opcode;
         break;
      }
      case mskMDataRef32Lo:
      {
         unsigned int opcode = *(unsigned int*)address;
         addr_t addr = (base + space->mdata) & 0xFFFF;

         opcode |= (addr << 5);

         *(unsigned int*)address = opcode;
         break;
      }
      case mskDataRef32Hi:
      {
         unsigned int opcode = *(unsigned int*)address;
         addr_t addr = (base + space->data) >> 16;

         opcode |= addr << 5;

         *(unsigned int*)address = opcode;
         break;
      }
      case mskDataRef32Lo:
      {
         unsigned int opcode = *(unsigned int*)address;
         addr_t addr = (base + space->data) & 0xFFFF;

         opcode |= (((unsigned int)addr) << 5);

         *(unsigned int*)address = opcode;
         break;
      }
      case mskCodeRef32Hi:
      {
         unsigned int opcode = *(unsigned int*)address;
         addr_t addr = ((base + space->code) >> 16) & 0xFFFF;

         opcode += ((unsigned int)addr) << 5;

         *(unsigned int*)address = opcode;
         break;
      }
      case mskCodeRef32Lo:
      {
         unsigned int opcode = *(unsigned int*)address;
         addr_t addr = (base + space->code) & 0xFFFF;

         opcode += (((unsigned int)addr) << 5);

         *(unsigned int*)address = opcode;
         break;
      }
   }
}

inline void arm64relocateElf64Import(pos_t pos, ref_t mask, ref_t reference, void* address, AddressSpace* space)
{
   switch (mask) {
      case mskImportRef64:
      {
         addr_t base = space->imageBase + space->importMapping.get(reference | mask);
         *(unsigned long long*)address += base + space->import;

         break;
      }
      case mskRDataRef64:
      {
         addr_t base = space->imageBase + reference;
         *(unsigned long long*)address += (unsigned long long)(base + space->rdata);
         break;
      }
      case mskDataRef64:
      {
         addr_t base = space->imageBase + reference;
         *(unsigned long long*)address += (unsigned long long)(base + space->data);
         break;
      }
      case mskCodeRef64:
      {
         addr_t base = space->imageBase + reference;
         *(unsigned long long*)address += (unsigned long long)(base + space->code);
         break;
      }
   }
}
