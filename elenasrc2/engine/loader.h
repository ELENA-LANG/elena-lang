//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Image Loader class declarations
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef loaderH
#define loaderH 1

#include "jitlinker.h"

namespace _ELENA_
{

// --- _ImageLoader ---

class _ImageLoader : public _JITLoader
{
protected:
   // addresses
   ReferenceMap _codeReferences, _dataReferences, _symbolReferences, _loaderReferences;
   ReferenceMap _statReferences, _exportReferences;
   ReferenceMap _constReferences, _numberReferences, _literalReferences, _characterReferences;
   ReferenceMap _bssReferences;

   ReferenceMap _subjects;         // subjects

public:
   virtual void* resolveReference(ident_t reference, size_t mask);

   virtual ref_t resolveExternal(ident_t external);

   virtual void mapReference(ident_t reference, void* vaddress, size_t mask);

   void clearReferences()
   {
      _codeReferences.clear();
      _dataReferences.clear();
      _symbolReferences.clear();
      _loaderReferences.clear();
      _statReferences.clear();
      _exportReferences.clear();
      _constReferences.clear();
      _numberReferences.clear();
      _literalReferences.clear();
      _bssReferences.clear();
      _subjects.clear();
   }

   _ImageLoader()
      : _codeReferences((size_t)-1), _dataReferences((size_t)-1), _symbolReferences((size_t)-1), _loaderReferences((size_t)-1),
        _statReferences((size_t)-1), _constReferences((size_t)-1), _numberReferences((size_t)-1), _characterReferences((size_t)-1),
        _literalReferences((size_t)-1), _bssReferences((size_t)-1), _exportReferences((size_t)-1),
        _subjects(0)
   {
   }
};

// --- Image ---
class Image
{
protected:
   Section _text, _data, _bss, _stat, _import, _tls;
   Section _debug;

public:
   virtual Section* getTextSection()   { return &_text; }
   virtual Section* getStatSection()   { return &_stat; }
   virtual Section* getRDataSection()  { return &_data; }
   virtual Section* getBSSSection()    { return &_bss; }
   virtual Section* getImportSection() { return &_import; }
   virtual Section* getDebugSection()  { return &_debug; }
   virtual Section* getTLSSection()    { return &_tls; }

   virtual ReferenceMap::Iterator getExternalIt() = 0;

   virtual ref_t getEntryPoint() = 0;
   virtual ref_t getDebugEntryPoint() = 0;

   Image(bool standAlone);
   virtual ~Image() {}
};

} // _ELENA_

#endif // loaderH
