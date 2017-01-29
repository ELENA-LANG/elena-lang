//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains XML Reader / Writer class header
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef xmlreaderH
#define xmlreaderH

#include "common.h"

namespace _ELENA_
{

// the internal exception if xml structure is not valid
struct XMLException
{
   size_t index;

   XMLException(size_t i)
   {
      index = i;
   }
};

typedef String<char, 255> XMLNodeTag;
typedef Map<ident_t, char*> AttributeList;

// node structure, used to keep the information about the node
class XMLNode
{
public:
   typedef List<size_t>  PositionList;
   typedef List<XMLNode> NodeList;

protected:
   // node position (in the xml text)
   size_t      _position;
   XMLNode*    _parent;

   virtual ident_t getContent()
   {
      return _parent->getContent();
   }

   virtual void setContent(size_t position, ident_t value)
   {
      _parent->setContent(position, value);
   }

   virtual void insertNode(size_t position, ident_t name)
   {
      _parent->insertNode(position, name);
   }

   // recognize opening tag and parse it starting from start, return value - the node content position
   size_t loadTag(ident_t content, size_t start, XMLNodeTag& tag, AttributeList* list);
   // find and return the closing node position starting from start
   bool isClosingTag(ident_t content, size_t start, XMLNodeTag& tag);
   // parse xml
   size_t parse(ident_t content, size_t start, size_t end, PositionList* list);

//   //// add new sub node to the node content and return the node id
//   //virtual int appendContent(const char* s);

public:
   size_t Position() { return _position; }

   bool compareTag(ident_t tag);

   // returns the sub node content
   virtual void readContent(DynamicString<char>& s);
   virtual bool readAttribute(ident_t name, DynamicString<char>& s);
   virtual void readTag(XMLNodeTag& s);

   virtual void writeContent(ident_t s);

   virtual XMLNode appendNode(ident_t name);

//   //// remove the node from the parent
//   //void remove();

   // search for the nodes with provided tag and return their indexes
   bool getNodeList(NodeList& list);

   XMLNode findNode(ident_t tag);

   // load an existing sub node
   XMLNode(size_t position, XMLNode* parent);
   XMLNode();
};

typedef XMLNode::NodeList NodeList;

class XMLTree : public XMLNode
{
   DynamicString<char> _content;

   virtual ident_t getContent()
   {
      return (const char*)_content;
   }

   virtual void setContent(int position, ident_t value);
   virtual void insertNode(int position, ident_t name);

public:
   void clear()
   {
      _content.clear();
   }

   //void assign(const char* s);
   bool load(path_t path, int encoding);
   //   //void XMLS_saveFile(const char* path);

   //   virtual int getContent(int id, DynamicString<char>& s);
   //   //virtual int setContent(int id, const char* s);
   //
   //   XMLTree(ident_t tag);
   XMLTree();
};

} // _ELENA_

#endif // xmlreaderH
