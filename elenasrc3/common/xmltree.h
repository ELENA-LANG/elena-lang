//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains XML Tree class header
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef XMLTREE_H
#define XMLTREE_H

#include "common.h"

namespace elena_lang
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

   // --- XmlNode ---
   class XmlNode
   {
   public:
      typedef List<XmlNode>                                    NodeList;
      typedef String<char, 50>                                 NodeTag;
      typedef String<char, 50>                                 Attribute;
      typedef String<char, 250>                                XPath;
      typedef Map<ustr_t, char*, allocUStr, freeUStr, freestr> AttributeList;
      typedef List<size_t>                                     PositionList;

   protected:
      XmlNode* _parent;
      size_t   _position;

      virtual ustr_t getContent()
      {
         return _parent->getContent();
      }

      size_t loadTag(ustr_t content, size_t start, NodeTag& tag, AttributeList* list);

      size_t parse(ustr_t content, size_t position, size_t end, PositionList* list);

   public:
      static XmlNode Default()
      {
         XmlNode node;

         return node;
      }

      bool isNotFound()
      {
         return _position == NOTFOUND_POS;
      }

      size_t position() const { return _position; }

      bool compareTag(ustr_t tag);

      XmlNode findChild(ustr_t childTag);
      bool findChildren(ustr_t childTag, NodeList& list);

      bool readAttribute(ustr_t name, Attribute& value);
      bool readContent(DynamicString<char>& s);

      XmlNode();
      XmlNode(XmlNode* parent, size_t position);
   };

   typedef XmlNode::NodeList XmlNodeList;

   // --- XmlTree ---
   class XmlTree : public  XmlNode
   {
      DynamicString<char, 512> _content;

      ustr_t getContent() override
      {
         return _content.str();
      }

      size_t resolve(ustr_t xpath);
      size_t resolve(XmlNode& node, ustr_t xpath);

      size_t resolveSubPath(ustr_t xpath, size_t& end);

   public:
      bool empty() { return _content.empty(); }

      bool selectNodes(ustr_t xpath, XmlNodeList& nodes);
      bool selectNodes(XmlNode& node, ustr_t xpath, XmlNodeList& nodes);

      XmlNode selectNode(ustr_t xpath);
      XmlNode selectNode(XmlNode& node, ustr_t xpath);

      bool load(path_t path, FileEncoding encoding);

      XmlTree();
   };
}

#endif
