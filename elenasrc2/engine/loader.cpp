//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Image loader class implementations
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "loader.h"

using namespace _ELENA_;

// --- ImageLoader ---

void _ImageLoader :: mapReference(ident_t reference, void* vaddress, size_t mask)
{
   switch (mask) {
      case mskConstantRef:
         _constReferences.add(reference, (ref_t)vaddress);
         break;
      //case mskConstArray:
      //   _constReferences.add(reference, (ref_t)vaddress);
      //   break;
      //case mskInt64Ref:
      case mskInt32Ref:
      //case mskRealRef:
         _numberReferences.add(reference, (size_t)vaddress);
         break;
      //case mskCharRef:
      //   _characterReferences.add(reference, (size_t)vaddress);
      //   break;
      //case mskLiteralRef:
      //   _literalReferences.add(reference, (size_t)vaddress);
      //   break;
      //case mskWideLiteralRef:
      //   _wideReferences.add(reference, (size_t)vaddress);
      //   break;
      case mskSymbolRelRef:
      case mskSymbolRef:
         _symbolReferences.add(reference, (ref_t)vaddress);
         break;
      default:
      {
         size_t imageMask = mask & mskImageMask;
         switch (imageMask) {
            case mskCodeRef:
            case mskRelCodeRef:
               _codeReferences.add(reference, (size_t)vaddress);
               break;
            case mskRDataRef:
               _dataReferences.add(reference, (size_t)vaddress);
               break;
            case mskStatRef:
               _statReferences.add(reference, (size_t)vaddress);
               break;
            case mskDataRef:
               _bssReferences.add(reference, (size_t)vaddress);
               break;
         }
         break;
      }      
   }
}

void _ImageLoader :: mapReference(ReferenceInfo referenceInfo, void* vaddress, size_t mask)
{
   if (referenceInfo.isRelative()) {
      IdentifierString fullName(referenceInfo.module->Name(), referenceInfo.referenceName);

      mapReference(fullName.c_str(), vaddress, mask);
   }
   else mapReference(referenceInfo.referenceName, vaddress, mask);
}

ref_t _ImageLoader :: resolveExternal(ident_t external)
{
   ref_t reference = _exportReferences.get(external);
   if (reference == (size_t)-1) {
      reference = (_exportReferences.Count() + 1) | mskImportRef;

      _exportReferences.add(external, reference);
   }
   return reference;
}

void* _ImageLoader :: resolveReference(ident_t reference, size_t mask)
{
   if (mask != 0) {
      switch (mask) {
         case mskConstantRef:
         //case mskConstArray:
            return (void*)_constReferences.get(reference);
         //case mskInt64Ref:
         case mskInt32Ref:
         //case mskRealRef:
            return (void*)_numberReferences.get(reference);
         //case mskCharRef:
         //   return (void*)_characterReferences.get(reference);
         //case mskLiteralRef:
         //   return (void*)_literalReferences.get(reference);
         //case mskWideLiteralRef:
         //   return (void*)_wideReferences.get(reference);
         case mskSymbolRelRef:
         case mskSymbolRef:
           return (void*)_symbolReferences.get(reference);
         case mskImportRef:
            return (void*)resolveExternal(reference);
//         case mskRelImportRef:
//            //HOTFIX : relative import ref mask should not be lost
//            return (void*)(resolveExternal(reference) | mskRelImportRef);
//         case mskStatSymbolRef:
//            return (void*)_statReferences.get(reference);
         case mskMessageTableRef:
            return (void*)INVALID_REF; // !! HOTFIX : should be always resolved
         default:
         {
            size_t imageMask = mask & mskImageMask;
            switch (imageMask) {
               case mskCodeRef:
               case mskRelCodeRef:
                  return (void*)_codeReferences.get(reference);
               case mskRDataRef:
                  return (void*)_dataReferences.get(reference);
               case mskStatRef:
                  return (void*)_statReferences.get(reference);
               case mskDataRef:
                  return (void*)_bssReferences.get(reference);
               default:
                  return NULL;
            }
         }      
      }
   }
   //// !! make sure message id is smaller than 0x7FFFFF
   //else return (void*)mapKey(_actions, reference, (_actions.Count()));
   else throw InternalError("Unsupported");
}

void* _ImageLoader :: resolveReference(ReferenceInfo referenceInfo, size_t mask)
{
   if (referenceInfo.isRelative()) {
      IdentifierString fullName(referenceInfo.module->Name(), referenceInfo.referenceName);

      return resolveReference(fullName.c_str(), mask);
   }
   else return resolveReference(referenceInfo.referenceName, mask);
}

//void _ImageLoader :: mapPredefinedAction(ident_t name, ref_t reference)
//{
//   _actions.add(name, reference);
//}

// --- Image ---

Image :: Image(bool standAlone)
   : _tls(0), _debug(0)
{
   // put signature
   MemoryWriter writer(&_data);

   writer.writeDWord(0);  // put SYSTEM_ENV reference place holder

   if (standAlone) {
      writer.write(ELENA_SIGNITURE, strlen(ELENA_SIGNITURE));
   }
   else writer.write(ELENACLIENT_SIGNITURE, strlen(ELENACLIENT_SIGNITURE));

   String<char, 4> number;
   number.appendInt(ENGINE_MINOR_VERSION);
   writer.write(number, strlen(number));
   writer.writeChar('.');
   number.copyInt(ENGINE_RELEASE_VERSION);
   writer.write(number, strlen(number));               
                                                  
   writer.align(4, 0);

   // write debug length and entry point placeholders
   _debug.writeBytes(0, 0, 8);
}
