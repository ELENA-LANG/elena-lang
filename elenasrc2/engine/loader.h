//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Image Loader class declarations
//                                              (C)2005-2018, by Alexei Rakov
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
   ReferenceMap  _codeReferences, _dataReferences, _symbolReferences;
   ReferenceMap  _statReferences, _exportReferences;
//   ReferenceMap _constReferences, _numberReferences, _literalReferences, _characterReferences, _wideReferences;
   ReferenceMap  _bssReferences;

   // actions
   ReferenceMap  _actions;         // actions

   void mapReference(ident_t reference, void* vaddress, size_t mask);
   void* resolveReference(ident_t reference, size_t mask);

public:
   virtual void* resolveReference(ReferenceInfo referenceInfo, size_t mask);

   virtual ref_t resolveExternal(ident_t external);

   virtual void mapReference(ReferenceInfo referenceInfo, void* vaddress, size_t mask);
   virtual void mapPredefinedAction(ident_t name, ref_t reference);

   void clearReferences()
   {
      _codeReferences.clear();
      _dataReferences.clear();
      _symbolReferences.clear();
      _statReferences.clear();
      _exportReferences.clear();
      //_constReferences.clear();
      //_numberReferences.clear();
      //_literalReferences.clear();
      //_characterReferences.clear();
      //_wideReferences.clear();
      _bssReferences.clear();
      _actions.clear();
   }

   _ImageLoader()
      : _codeReferences(INVALID_REF), _dataReferences(INVALID_REF), _symbolReferences(INVALID_REF),
        _statReferences(INVALID_REF), //_constReferences((size_t)-1), _numberReferences((size_t)-1), _characterReferences((size_t)-1),
      /*  _literalReferences((size_t)-1), */_bssReferences(INVALID_REF), _exportReferences(INVALID_REF), //_wideReferences((size_t)-1),
        _actions(0)
   {
   }
};

// --- Image ---
class Image
{
protected:
   Section _text, _data, _bss, _stat, _import, _tls;
   Section _debug, _mdata;

public:
   virtual Section* getTextSection()   { return &_text; }
   virtual Section* getStatSection()   { return &_stat; }
   virtual Section* getRDataSection()  { return &_data; }
   virtual Section* getBSSSection()    { return &_bss; }
   virtual Section* getImportSection() { return &_import; }
   virtual Section* getDebugSection()  { return &_debug; }
   virtual Section* getTLSSection()    { return &_tls; }
   virtual Section* getMDataSection()  { return &_mdata; }

   virtual ReferenceMap::Iterator getExternalIt() = 0;

   virtual ref_t getEntryPoint() = 0;
   virtual ref_t getDebugEntryPoint() = 0;

   Image(bool standAlone);
   virtual ~Image() {}
};

} // _ELENA_

#endif // loaderH
