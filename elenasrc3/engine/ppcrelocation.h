//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains relocation functions
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

using namespace elena_lang;

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
         addr_t addr = base + space->stat >> 16;

         *(unsigned short*)address += (unsigned short)(addr);
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
         addr_t addr = base + space->rdata >> 16;

         *(unsigned short*)address += (unsigned short)(addr);
         break;
      }
      case mskRDataRef32Lo:
      {
         addr_t addr = (base + space->rdata);

         *(unsigned short*)address += (unsigned short)(addr);
         break;
      }
      case mskStatDisp32Hi:
      {
         addr_t baseAddr = (base + space->rdata);
         addr_t addr = (base + space->stat);
         addr_t disp = addr - baseAddr;

         *(unsigned short*)address += (unsigned short)(disp >> 16);
         break;
      }
      case mskStatDisp32Lo:
      {
         addr_t baseAddr = (base + space->rdata);
         addr_t addr = (base + space->stat);
         addr_t disp = addr - baseAddr;

         *(unsigned short*)address += (unsigned short)(disp & 0xFFFF);
         break;
      }
      case mskRDataDisp32Lo:
      case mskCodeDisp32Lo:
      {
         unsigned short disp = (unsigned short)(reference & 0xFFFF);

         *(unsigned short*)address += disp;
         break;
      }
      case mskCodeDisp32Hi:
      case mskRDataDisp32Hi:
      {
         unsigned short disp = (unsigned short)(reference >> 16);

         *(unsigned short*)address += disp;
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
         addr_t disp = space->importMapping.get(reference | mskImportRef64) >> 16;

         *(unsigned short*)address += (unsigned short)(disp);
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
