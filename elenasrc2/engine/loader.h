//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Image Loader class declarations
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef loaderH
#define loaderH 1

#include "jitlinker.h"

namespace _ELENA_
{

// --- _ImageLoader ---

//typedef Map<ident_t, AddressMap*> ModuleAddressMap;

class _ImageLoader : public _JITLoader
{
protected:
   // addresses
   VAddressMap  _codeReferences, _dataReferences, _symbolReferences;
   VAddressMap  _statReferences;
   VAddressMap  _constReferences, _numberReferences, _literalReferences, _characterReferences, _wideReferences;
   VAddressMap  _mssgReferences, _subjReferences;
   VAddressMap  _bssReferences;
   VAddressMap  _exportReferences;

   void mapReference(ident_t reference, lvaddr_t vaddress, ref_t mask);
   lvaddr_t resolveReference(ident_t reference, ref_t mask);

public:
   virtual lvaddr_t resolveReference(ReferenceInfo referenceInfo, ref_t mask);

   virtual lvaddr_t resolveExternal(ident_t external);

   virtual void mapReference(ReferenceInfo referenceInfo, lvaddr_t vaddress, ref_t mask);
   //virtual void mapPredefinedAction(ident_t name, ref_t reference);

   void clearReferences()
   {
      _codeReferences.clear();
      _dataReferences.clear();
      _symbolReferences.clear();
      _statReferences.clear();
      _exportReferences.clear();
      _constReferences.clear();
      _numberReferences.clear();
      _literalReferences.clear();
      _characterReferences.clear();
      _wideReferences.clear();
      _bssReferences.clear();
      _mssgReferences.clear();
      _subjReferences.clear();
   }

   _ImageLoader()
      : _codeReferences(INVALID_VADDR), _dataReferences(INVALID_VADDR), _symbolReferences(INVALID_VADDR),
        _statReferences(INVALID_VADDR), _constReferences(INVALID_VADDR), _numberReferences(INVALID_VADDR), 
        _characterReferences(INVALID_VADDR), _literalReferences(INVALID_VADDR), _bssReferences(INVALID_VADDR), 
        _exportReferences(INVALID_VADDR), _wideReferences(INVALID_VADDR), _mssgReferences(INVALID_VADDR),
        _subjReferences(INVALID_VADDR)
   {
   }
};

// --- Image ---
class Image
{
protected:
   Section _text, _data, _bss, _stat, _import, _tls;
   Section _debug, _mdata, _adata;

public:
   virtual Section* getTextSection()   { return &_text; }
   virtual Section* getStatSection()   { return &_stat; }
   virtual Section* getRDataSection()  { return &_data; }
   virtual Section* getBSSSection()    { return &_bss; }
   virtual Section* getImportSection() { return &_import; }
   virtual Section* getDebugSection()  { return &_debug; }
   virtual Section* getTLSSection()    { return &_tls; }
   // returns the message table
   virtual Section* getMDataSection()  { return &_mdata; }
   virtual Section* getADataSection()  { return &_adata; }

   virtual VAddressMap::Iterator getExternalIt() = 0;

   virtual lvaddr_t getEntryPoint() = 0;
   virtual ref_t getDebugEntryPoint() = 0;

   Image(bool standALone);
   virtual ~Image() {}
};

} // _ELENA_

#endif // loaderH
