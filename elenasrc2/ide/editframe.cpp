//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     EditFrame class Implementation File
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

//#include "guicommon.h"
//---------------------------------------------------------------------------
#include "editframe.h"
#include "idecommon.h"
#include "sourcedoc.h"

using namespace _GUI_;

// --- EditFrame ---

//void _EditFrame :: selectDocument(const tchar_t* path)
//{
//   selectDocumentTab(_mappings.get(path));
//}
//
//void _EditFrame :: selectDocument(int index)
//{
//   selectDocumentTab(index);
//}
//
//bool _EditFrame :: isDocumentIncluded(int index)
//{
//   Document* doc = getDocument(index);
//
//   return doc ? doc->status.included : false;
//}
//
//bool _EditFrame :: isDocumentModified(int index)
//{
//   Document* doc = getDocument(index);
//
//   return doc ? doc->status.modifiedMode : false;
//}
//
//bool _EditFrame :: isDocumentUnnamed(int index)
//{
//   Document* doc = getDocument(index);
//
//   return doc ? doc->status.unnamed : false;
//}
//
//void _EditFrame :: saveDocument(const tchar_t* path, int index)
//{
//   Document* doc = getDocument(index);
//   if (doc) {
//      doc->save(path);
//   }
//}
//
//void _EditFrame :: markDocumentAsIncluded(int index)
//{
//   Document* doc = getDocument(index);
//   if (doc) {
//      doc->status.included = true;
//   }
//}
//
//void _EditFrame :: markDocumentAsExcluded(int index)
//{
//   Document* doc = getDocument(index);
//   if (doc) {
//      doc->status.included = false;
//   }
//}
//
////bool Editor :: isDocumentUnnamed(int index)
////{
////   if (index >= 0 && index < (int)_documents.Count()) {
////      return (*_documents.get(index))->status.unnamed;
////   }
////   else return false;
////}
//
//void _EditFrame :: removeDocumentMarker(int index, int row, int bandStyle)
//{
//   Document* doc = getDocument(index);
//
//   doc->removeMarker(row, bandStyle);
//
//   refreshDocument();
//}

