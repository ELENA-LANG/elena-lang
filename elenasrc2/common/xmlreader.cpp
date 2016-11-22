//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains XML Reader / Writer File class implementation
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "xmlreader.h"

using namespace _ELENA_;

// --- XMLNode ---

inline bool isWhitespace(char ch)
{
   return (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t');
}

XMLNode :: XMLNode()
//   : _attributes(NULL, freestr)
{
   _position = -1;
   _parent = NULL;
//   _noChildren = true;
}

XMLNode :: XMLNode(int position, XMLNode* parent)
//   : _attributes(NULL, freestr)
{
   _parent = parent;
   _position = position;
}

bool XMLNode :: compareTag(ident_t tag)
{
   XMLNodeTag current;
   loadTag(getContent(), _position, current, NULL);

   return tag.compare(current);
}

int XMLNode :: loadTag(ident_t content, int start, XMLNodeTag & tag, AttributeList* list)
{
   int position = start;

   // read tag and attributes
   if (content[start] != '<' || content[start + 1] == '/')
      throw XMLException(start);

   // read tag name
   while (!isWhitespace(content[position]) && content[position] != '>') {
      if (content[position] == 0)
         throw XMLException(position);

      position++;
   }

   tag.copy(content + start + 1, position - start - 1);

   // read attributes
   while (content[position] != '>') {
      position++;
      start = position;

      while (!isWhitespace(content[position]) && content[position] != '=') {
         if (content[position] == 0)
            throw XMLException(position);

         position++;
      }

      XMLNodeTag attr;
      attr.copy(content + start, position - start);

      if (content[position] == '=') {
         position++;
         if (content[position] != '\"')
            throw XMLException(position);

         position++;
         start = position;
         while (content[position] != '\"') {
            if (content[position] == 0)
               throw XMLException(position);

            position++;
         }

         if (list) {
            char* s = content.clone(start, position - start);

            list->add((const char*)attr, s);
         }

         position++;
      }
   }

   return position + 1;
}

bool XMLNode :: isClosingTag(ident_t content, int start, XMLNodeTag& tag)
{
   if (content[start] == '<' && content[start + 1] == '/') {
      if (content.compare((const char*)tag, start + 2, tag.Length()) && content[start + 2 + tag.Length()] == '>')
         return true;
   }

   return false;
}

int XMLNode :: parse(ident_t content, int position, int end, PositionList* list)
{
   // skip the ending whitespaces
   while (isWhitespace(content[position]))
      position++;

   XMLNodeTag tag;
   position = loadTag(content, position, tag, NULL);

   while (position < end) {
      // skip the ending whitespaces
      while (isWhitespace(content[position]))
         position++;

      if (content[position] != '<') {
         position = content.findSubStr(position, '<', end - position, end);
         if (content[position + 1] != '/') {
            throw XMLException(position);
         }
      }
      else if (!isClosingTag(content, position, tag)) {
         if (list)
            list->add(position);

         position = parse(content, position, end, NULL);
      }
      else return position + 3 + tag.Length();
   }

   throw XMLException(position);

   return -1;
}

void XMLNode :: readContent(DynamicString<char>& s)
{
   ident_t content = getContent();

   XMLNodeTag tag;
   int start = loadTag(content, _position, tag, NULL);
   int end = parse(content, _position, getlength(content), NULL);

   s.copy((const char*)content + start, end - start - tag.Length() - 3);
}

bool XMLNode :: readAttribute(ident_t name, DynamicString<char>& s)
{
   ident_t content = getContent();

   XMLNodeTag tag;
   AttributeList list;
   int start = loadTag(content, _position, tag, &list);

   ident_t value = list.get(name);
   if (!emptystr(value)) {
      s.copy(value);

      return true;
   }
   else return false;
}

void XMLNode :: readTag(XMLNodeTag& tag)
{
   ident_t content = getContent();

   loadTag(content, _position, tag, NULL);
}

XMLNode XMLNode :: findNode(ident_t tag)
{
   bool found = false;

   ident_t content = getContent();

   PositionList positions;
   int end = parse(content, _position, getlength(content), &positions);

   for (PositionList::Iterator it = positions.start(); !it.Eof(); it++) {
      XMLNode node(*it, this);

      if (node.compareTag(tag)) {
         return node;
      }
   }

   return XMLNode();
}

bool XMLNode :: getNodeList(List& list)
{
   bool found = false;

   ident_t content = getContent();

   PositionList positions;
   int end = parse(content, _position, getlength(content), &positions);

   for (PositionList::Iterator it = positions.start(); !it.Eof(); it++) {
      XMLNode node(*it, this);

      found = true;

      list.add(node);
   }

   return found;
}

// --- XMLTree ---

//XMLTree :: XMLTree(ident_t tag)
//{
//   _parent = NULL;
//   _id = -1;
//
//   _content.append("<");
//   _content.append(tag);
//   _content.append("></");
//   _content.append(tag);
//   _content.append(">");
//
//   parse((const char*)_content);
//}

XMLTree :: XMLTree()
{
   _parent = this; // !! HOTFIX
   _position = 0;
}

bool XMLTree :: load(path_t path, int encoding)
{
   TextFileReader reader(path, encoding, true);
   if (!reader.isOpened())
      return false;

   //read the file
   char buffer[128];
   reader.readAll(_content, buffer, 128);

   return true;
}
