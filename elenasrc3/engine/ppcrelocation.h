//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains relocation functions
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

using namespace elena_lang;

inline short getHiAdjusted(disp_t n)
{
   // HOTFIX : if the DWORD LO is over 0x7FFF - adjust DWORD HI (by adding 1) to be properly combined in the following code:
   //     addis   r16, r16, __xdisp32hi_1
   //     addi    r16, r16, __xdisp32lo_1
   short lo = n & 0xFFFF;
   if (lo < 0)
      n += 0x10000;

   return (short)(n >> 16);
}

inline void ppc64relocate(pos_t pos, ref_t mask, ref_t reference, void* address, AddressSpace* space)
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
      case mskImportRelRef32:
      {
         addr_t tableAddress = space->import + space->imageBase + space->importMapping.get(reference | mask);
         addr_t codeAddress = space->code + space->imageBase + pos + 4;

         *(unsigned int*)address += (unsigned int)(tableAddress - codeAddress);
         break;
      }
      case mskImportRef64:
      {
         addr_t base = space->imageBase + space->importMapping.get(reference | mask);
         *(unsigned long long*)address += base + space->import;

         break;
      }
      case mskMDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->mdata);
         break;
      case mskMBDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->mbdata);
         break;
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
         addr_t addr = base + space->stat;

         //*(unsigned short*)address += (unsigned short)(addr);
         *(short*)address += getHiAdjusted(addr);
         break;
      }
      case mskStatDataRef32Lo:
      {
         addr_t addr = (base + space->stat);

         *(unsigned short*)address += (unsigned short)(addr);
         break;
      }
      case mskRDataRef32Hi:
      {
         addr_t addr = base + space->rdata;

         //*(unsigned short*)address += (unsigned short)(addr);
         *(short*)address += getHiAdjusted(addr);

         break;
      }
      case mskRDataRef32Lo:
      {
         addr_t addr = (base + space->rdata);

         *(unsigned short*)address += (unsigned short)(addr);
         break;
      }
      case mskStatXDisp32Hi:
      {
         addr_t baseAddr = space->imageBase + space->code;
         addr_t addr = (base + space->stat);
         addr_t disp = addr - baseAddr;

         *(short*)address += getHiAdjusted(disp);
         break;
      }
      case mskStatXDisp32Lo:
      {
         addr_t baseAddr = space->imageBase + space->code;
         addr_t addr = (base + space->stat);
         addr_t disp = addr - baseAddr;

         *(unsigned short*)address += (unsigned short)(disp & 0xFFFF);
         break;
      }
      case mskDataXDisp32Hi:
      {
         addr_t baseAddr = space->imageBase + space->code;
         addr_t addr = (base + space->data);
         disp_t disp = addr - baseAddr;

         *(short*)address += getHiAdjusted(disp);
         break;
      }
      case mskDataXDisp32Lo:
      {
         addr_t baseAddr = space->imageBase + space->code;
         addr_t addr = (base + space->data);
         addr_t disp = addr - baseAddr;

         *(short*)address += (short)(disp & 0xFFFF);
         break;
      }
      case mskRDataXDisp32Hi:
      {
         addr_t baseAddr = space->imageBase + space->code;
         addr_t addr = (base + space->rdata);
         disp_t disp = addr - baseAddr;

         *(short*)address += getHiAdjusted(disp);
         break;
      }
      case mskRDataXDisp32Lo:
      {
         addr_t baseAddr = space->imageBase + space->code;
         addr_t addr = (base + space->rdata);
         addr_t disp = addr - baseAddr;

         *(unsigned short*)address += (unsigned short)(disp & 0xFFFF);
         break;
      }
      case mskCodeXDisp32Lo:
      case mskDataDisp32Lo:
      case mskRDataDisp32Lo:
      case mskCodeDisp32Lo:
      {
         unsigned short disp = (unsigned short)(reference & 0xFFFF);

         *(unsigned short*)address += disp;
         break;
      }
      case mskCodeXDisp32Hi:
      case mskDataDisp32Hi:
      case mskRDataDisp32Hi:
      case mskCodeDisp32Hi:
      {
         *(unsigned short*)address += getHiAdjusted(reference);
         break;
      }
      case mskImportDisp32Lo:
      {
         addr_t disp = space->importMapping.get(reference | mskImportRef64);

         *(unsigned short*)address += (unsigned short)(disp);
         break;
      }
      case mskImportDisp32Hi:
      {
         addr_t disp = space->importMapping.get(reference | mskImportRef64);

         //*(unsigned short*)address += (unsigned short)(disp >> 16);
         *(short*)address += getHiAdjusted(disp);
         break;
      }
      default:
         // to make compiler happy
         break;
   }
}

inline void ppc64relocateElf64Import(pos_t pos, ref_t mask, ref_t reference, void* address, AddressSpace* space)
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
   default:
      // to make compiler happy
      break;
   }
}