//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains relocation functions
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

using namespace elena_lang;

inline void relocate(pos_t pos, ref_t mask, ref_t reference, void* address, AddressSpace* space)
{
   addr_t base = space->imageBase + reference;
   switch (mask) {
      case mskCodeRef32:
         *(unsigned int*)address += (unsigned int)(base + space->code);
         break;
      case mskCodeRelRef32:
         *(unsigned int*)address += (unsigned int)(reference - pos - 4);
         break;
      case mskRDataRef32:
         *(unsigned int*)address += (unsigned int)(base + space->rdata);
         break;
      case mskDataRef32:
         *(unsigned int*)address += (unsigned int)(base + space->data);
         break;
      case mskTLSRef32:
         *(unsigned int*)address += (unsigned int)(base + space->tls);
         break;
      case mskStatDataRef32:
         *(unsigned int*)address += (unsigned int)(base + space->stat);
         break;
      case mskMDataRef32:
         *(unsigned int*)address += (unsigned int)(base + space->mdata);
         break;
      case mskMBDataRef32:
         *(unsigned int*)address += (unsigned int)(base + space->mbdata);
         break;
      case mskImportRef32:
         base = space->imageBase + space->importMapping.get(reference | mask);
         *(unsigned int*)address += (unsigned int)(base + space->import);
         break;
      default:
         // to make compiler happy
         break;
   }
}

inline void relocate64(pos_t pos, ref_t mask, ref_t reference, void* address, AddressSpace* space)
{
   addr_t base = space->imageBase + reference;
   switch (mask) {
      case mskCodeRef32:
         *(unsigned int*)address += (unsigned int)(base + space->code);
         break;
      case mskCodeRelRef32:
         *(unsigned int*)address += (unsigned int)(reference - pos - 4);
         break;
      case mskDataRelRef32:
      {
         addr_t tableAddress = space->data + base;
         addr_t codeAddress = space->code + space->imageBase + pos + 4;

         *(unsigned int*)address += (unsigned int)(tableAddress - codeAddress);
         break;
      }
      case mskCodeRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->code);
         break;
      case mskRDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->rdata);
         break;
      case mskDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->data);
         break;
      case mskTLSRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->tls);
         break;
      case mskStatDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->stat);
         break;
      case mskMDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->mdata);
         break;
      case mskMBDataRef64:
         *(unsigned long long*)address += (unsigned long long)(base + space->mbdata);
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
         base = space->imageBase + space->importMapping.get(reference | mask);
         *(unsigned long long*)address += base + space->import;

         break;
      }
      default:
         // to make compiler happy
         break;
   }
}

inline void relocateImport(pos_t, ref_t mask, ref_t reference, void* address, AddressSpace* space)
{
   switch (mask) {
      case mskImportRef32:
      {
         unsigned int base = space->importMapping.get(reference | mask);
         *(unsigned int*)address += (unsigned int)(base + space->import);

         break;
      }
      default:
         // to make compiler happy
         break;
   }
}

inline void relocateElfImport(pos_t, ref_t mask, ref_t reference, void* address, AddressSpace* space)
{
   switch (mask) {
      case mskImportRef32:
      {
         addr_t base = space->imageBase + space->importMapping.get(reference | mask);
         *(unsigned int*)address += (unsigned int)(base + space->import);

         break;
      }
      case mskRDataRef32:
      {
         addr_t base = space->imageBase + reference;
         *(unsigned int*)address += (unsigned int)(base + space->rdata);
         break;
      }
      case mskDataRef32:
      {
         addr_t base = space->imageBase + reference;
         *(unsigned int*)address += (unsigned int)(base + space->data);
         break;
      }
      case mskCodeRef32:
      {
         addr_t base = space->imageBase + reference;
         *(unsigned int*)address += (unsigned int)(base + space->code);
         break;
      }
      default:
         // to make compiler happy
         break;
   }
}

inline void relocateElf64Import(pos_t, ref_t mask, ref_t reference, void* address, AddressSpace* space)
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
