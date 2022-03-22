//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains XML Reader / Writer File class implementation
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "xmltree.h"

using namespace elena_lang;

// --- XmlNode ---

inline bool isWhitespace(char ch)
{
   return (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t');
}

inline void seekEnd(ustr_t content, size_t& position, char endChar)
{
   while (!isWhitespace(content[position]) && content[position] != endChar) {
      if (content[position] == 0)
         throw XMLException(position);

      position++;
   }
}

inline void seekQuoteEnd(ustr_t content, size_t& position, char endChar)
{
   while (content[position] != endChar) {
      if (content[position] == 0)
         throw XMLException(position);

      position++;
   }
}

inline void skipWhitespace(ustr_t content, size_t& position)
{
   while (isWhitespace(content[position]))
      position++;
}

inline bool isClosingTag(ustr_t content, size_t start, ustr_t tag)
{
   if (content[start] == '<' && content[start + 1] == '/') {
      if (content.compareSub(tag, start + 2, tag.length()) && content[start + 2 + tag.length()] == '>') {
         return true;
      }
   }
   return false;
}

XmlNode::XmlNode()
{
   _position = NOTFOUND_POS;
   _parent = nullptr;
}

XmlNode::XmlNode(XmlNode* parent, size_t position)
{
   _parent = parent;
   _position = position;
}

size_t XmlNode :: loadTag(ustr_t content, size_t start, NodeTag& tag, AttributeList* list)
{
   size_t position = start;

   // validate starting angle
   if (content[start] != '<' || content[start + 1] == '/')
      throw XMLException(start);

   // read tag name
   seekEnd(content, position, '>');

   tag.copy(content + start + 1, position - start - 1);

   // read attributes
   while (content[position] != '>') {
      position++;
      start = position;

      seekEnd(content, position, '=');

      NodeTag attr;
      attr.copy(content + start, position - start);

      if (content[position] == '=') {
         position++;
         if (content[position] != '\"')
            throw XMLException(position);

         position++;
         start = position;
         seekQuoteEnd(content, position, '\"');

         if (list) {
            char* s = content.clone(start, position - start);

            list->add(attr.str(), s);
         }

         position++;
      }
      else throw XMLException(start);
   }

   return position + 1;
}

size_t XmlNode :: parse(ustr_t content, size_t position, size_t end, PositionList* list)
{
   // skip the ending whitespaces
   skipWhitespace(content, position);

   NodeTag tag;
   position = loadTag(content, position, tag, nullptr);

   while (position < end) {
      // skip the ending whitespaces
      skipWhitespace(content, position);

      if (content[position] != '<') {
         position = content.findSub(position, '<', end, end);
         if (isClosingTag(content, position, tag.str())) {
            return position + 3 + tag.length();
         }
         else throw XMLException(position);
      }
      else if (!isClosingTag(content, position, tag.str())) {
         if (list)
            list->add(position);

         position = parse(content, position, end, nullptr);
      }
      else return position + 3 + tag.length();
   }

   throw XMLException(position);
}

bool XmlNode :: compareTag(ustr_t tag)
{
   NodeTag current;
   loadTag(getContent(), _position, current, nullptr);

   return tag.compare(current.str());
}

XmlNode XmlNode :: findChild(ustr_t childTag)
{
   ustr_t content = getContent();
   if (!content.empty()) {
      PositionList positions(NOTFOUND_POS);
      parse(content, _position, content.length(), &positions);

      for (auto it = positions.start(); !it.eof(); ++it) {
         XmlNode node(_parent, *it);

         if (node.compareTag(childTag)) {
            return node;
         }
      }
   }

   return XmlNode();
}

bool XmlNode :: findChildren(ustr_t childTag, NodeList& list)
{
   bool found = false;
   ustr_t content = getContent();
   if (!content.empty()) {
      PositionList positions(NOTFOUND_POS);
      parse(content, _position, content.length(), &positions);

      for (auto it = positions.start(); !it.eof(); ++it) {
         XmlNode node(_parent, *it);

         if (childTag.compare("*") || node.compareTag(childTag)) {
            list.add(node);
            found = true;
         }
      }
   }

   return found;
}

bool XmlNode :: readAttribute(ustr_t name, Attribute& attrValue)
{
   ustr_t content = getContent();

   NodeTag       tag;
   AttributeList list(0);
   loadTag(content, _position, tag, &list);

   ustr_t value = list.get(name);
   if (!value.empty()) {
      attrValue.copy(value);

      return true;
   }
   else return false;
}

bool XmlNode :: readContent(DynamicString<char>& s)
{
   NodeTag tag;
   ustr_t content = getContent();
   size_t start = loadTag(content, _position, tag, nullptr);
   size_t end = parse(content, _position, content.length(), nullptr);

   s.copy(content + start, end - start - tag.length() - 3);

   return true;
}

// --- XmlTree ---

XmlTree :: XmlTree()
{
   _position = 0;
   _parent = this; // NOTE : due to current implementation - parent refers to the tree
}

bool XmlTree :: load(path_t path, FileEncoding encoding)
{
   TextFileReader reader(path, encoding, true);
   if (!reader.isOpen())
      return false;

   //read the file, NOTE : buffer should be bigger to contain the tailing zero
   char buffer[132];
   reader.readAll(_content, buffer, 128);

   return true;
}

size_t XmlTree :: resolve(ustr_t xpath)
{
   size_t length = xpath.length();
   size_t end = xpath.find('/', length);

   NodeTag tag(xpath.str(), end);
   if (compareTag(tag.str())) {
      if (end < length) {
         return resolve(*this, xpath + end + 1);
      }
      else return position();
   }
   else return NOTFOUND_POS;
}

size_t XmlTree :: resolve(XmlNode& node, ustr_t xpath)
{
   if (!xpath.empty()) {
      size_t length = xpath.length();
      size_t end = xpath.find('/', length);

      NodeTag tag(xpath.str(), end);
      XmlNode child = node.findChild(tag.str());
      if (child.position() != NOTFOUND_POS) {
         if (end < length) {
            return resolve(child, xpath + end + 1);
         }
         else return child.position();
      }
      return NOTFOUND_POS;
   }
   else return node.position();
}

size_t XmlTree :: resolveSubPath(ustr_t xpath, size_t& end)
{
   size_t length = xpath.length();
   end = xpath.findLast('/', length);

   size_t position = NOTFOUND_POS;
   if (length > end) {
      XPath subXPath(xpath.str(), end);

      position = resolve(subXPath.str());
   }

   return position;
}


bool XmlTree :: selectNodes(ustr_t xpath, XmlNodeList& nodes)
{
   size_t end = 0;
   size_t position = resolveSubPath(xpath, end);
   if (position == NOTFOUND_POS)
      return false;

   NodeTag childTag(xpath.str() + end + 1);

   XmlNode lastNode(this, position);
   return lastNode.findChildren(childTag.str(), nodes);
}

bool XmlTree :: selectNodes(XmlNode& node, ustr_t xpath, XmlNodeList& nodes)
{
   if (xpath.compare("*")) {
      return node.findChildren(xpath, nodes);
   }
   else {
      size_t length = xpath.length();
      size_t end = xpath.findLast('/', length);

      size_t position = 0;
      if (length > end) {
         XPath subXPath(xpath.str(), end);

         position = resolve(node, subXPath.str());
      }
      if (position == NOTFOUND_POS)
         return false;

      NodeTag childTag(xpath.str() + end + 1);

      XmlNode lastNode(this, position);
      return lastNode.findChildren(childTag.str(), nodes);
   }
}

XmlNode XmlTree :: selectNode(ustr_t xpath)
{
   size_t end = 0;
   size_t position = resolveSubPath(xpath, end);
   if (position == NOTFOUND_POS)
      return XmlNode();

   NodeTag childTag(xpath.str() + end + 1);

   XmlNode lastNode(this, position);
   return lastNode.findChild(childTag.str());
}

XmlNode XmlTree :: selectNode(XmlNode& node, ustr_t xpath)
{
   size_t length = xpath.length();
   size_t end = xpath.findLast('/', length);

   size_t position = 0;
   if (length > end) {
      XPath subXPath(xpath.str(), end);

      position = resolve(node, subXPath.str());
   }

   NodeTag childTag(xpath.str() + end + 1);

   XmlNode lastNode(this, position);
   if (lastNode.isNotFound())
      return lastNode;

   return lastNode.findChild(childTag.str());
 }
