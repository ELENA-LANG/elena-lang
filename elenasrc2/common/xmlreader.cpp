//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains XML Reader / Writer File class implementation
//                                              (C)2005-2018, by Alexei Rakov
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
   _position = (size_t)-1;
   _parent = NULL;
//   _noChildren = true;
}

XMLNode :: XMLNode(size_t position, XMLNode* parent)
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

size_t XMLNode :: loadTag(ident_t content, size_t start, XMLNodeTag & tag, AttributeList* list)
{
   size_t position = start;

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

bool XMLNode :: isClosingTag(ident_t content, size_t start, XMLNodeTag& tag)
{
   if (content[start] == '<' && content[start + 1] == '/') {
      if (content.compare((const char*)tag, start + 2, tag.Length()) && content[start + 2 + tag.Length()] == '>')
         return true;
   }

   return false;
}

size_t XMLNode :: parse(ident_t content, size_t position, size_t end, PositionList* list)
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
         position = content.findSubStr(position, '<', end, end);
         if (isClosingTag(content, position, tag)) {
            return position + 3 + tag.Length();
         }
         else throw XMLException(position);
      }
      else if (!isClosingTag(content, position, tag)) {
         if (list)
            list->add(position);

         position = parse(content, position, end, NULL);
      }
      else return position + 3 + tag.Length();
   }

   throw XMLException(position);
}

void XMLNode :: readContent(DynamicString<char>& s)
{
   ident_t content = getContent();

   XMLNodeTag tag;
   size_t start = loadTag(content, _position, tag, NULL);
   size_t end = parse(content, _position, getlength(content), NULL);

   s.copy((const char*)content + start, end - start - tag.Length() - 3);
}

void XMLNode :: writeContent(ident_t s)
{
   setContent(_position, s);
}

XMLNode XMLNode :: appendNode(ident_t name)
{
   ident_t content = getContent();

   XMLNodeTag tag;
   loadTag(content, _position, tag, NULL);
   size_t end = parse(content, _position, getlength(content), NULL) - tag.Length() - 3;

   return XMLNode(insertNode(end, name), this);
}

bool XMLNode :: readAttribute(ident_t name, DynamicString<char>& s)
{
   ident_t content = getContent();

   XMLNodeTag tag;
   AttributeList list;
   loadTag(content, _position, tag, &list);

   ident_t value = list.get(name);
   if (!emptystr(value)) {
      s.copy(value);

      return true;
   }
   else return false;
}

void XMLNode :: writeAttribute(ident_t name, ident_t s)
{
   setAttribute(_position, name, s);
}

void XMLNode :: readTag(XMLNodeTag& tag)
{
   ident_t content = getContent();

   loadTag(content, _position, tag, NULL);
}

XMLNode XMLNode :: findNode(ident_t tag)
{
   ident_t content = getContent();
   if (!emptystr(content)) {
      PositionList positions;
      parse(content, _position, getlength(content), &positions);

      for (PositionList::Iterator it = positions.start(); !it.Eof(); it++) {
         XMLNode node(*it, this);

         if (node.compareTag(tag)) {
            return node;
         }
      }
   }

   return XMLNode();
}

bool XMLNode :: getNodeList(NodeList& list)
{
   bool found = false;

   ident_t content = getContent();

   PositionList positions;
   parse(content, _position, getlength(content), &positions);

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

   //read the file, NOTE : buffer should be bigger to contain the tailing zero
   char buffer[132];
   reader.readAll(_content, buffer, 128);

   return true;
}

bool XMLTree :: loadXml(ident_t content)
{
   _content.copy(content);

   return true;
}

bool XMLTree :: save(path_t path, int encoding, bool withBOM, bool formatted)
{
   TextFileWriter  writer(path, encoding, withBOM);

   if (!writer.isOpened())
      return false;

   if (formatted) {
      int indent = -4;
      bool inlineMode = false;
      bool openning = true;
      bool ignoreWhitespace = true;
      for (size_t i = 0; i < getlength(_content); i++) {
         if (_content[i] == '<') {
            if (_content[i + 1] != '/') { 
               openning = true;
               inlineMode = false;
               indent += 4;
               if (indent > 0) {
                  writer.writeNewLine();
                  writer.fillText(" ", 1, indent);
               }
            }
            else {
               openning = false;
               if (!inlineMode) {
                  writer.writeNewLine();
                  writer.fillText(" ", 1, indent);
               }
               inlineMode = false;

               indent -= 4;
            }
            ignoreWhitespace = false;
         }
         else if (_content[i] == '>') {
            if (openning)
               inlineMode = true;

            ignoreWhitespace = true;
         }
         else if (isWhitespace(_content[i])) {
            // ignore existing line breaks
            if (ignoreWhitespace)
               continue;
         }
         else if (inlineMode) {
            ignoreWhitespace = false;
         }            

         writer.writeChar(_content[i]);
      }
   }
   else writer.writeLiteral(_content);

   return true;
}

void XMLTree :: removeNode(size_t position)
{
   XMLNodeTag tag;
   size_t start = loadTag(_content.str(), position, tag, NULL);
   size_t end = parse(_content.str(), position, _content.Length(), NULL) - tag.Length() - 3;

   if (isClosingTag(_content.str(), end, tag)) {
      end = end + 3 + tag.Length();
   }

   _content.cut(position, end - start);
}

void XMLTree :: setContent(size_t position, ident_t value)
{
   XMLNodeTag tag;
   size_t start = loadTag(_content.str(), position, tag, NULL);
   size_t end = parse(_content.str(), position, _content.Length(), NULL) - tag.Length() - 3;

   _content.cut(start, end - start);
   _content.insert(value.c_str(), start);
}

void XMLTree :: setAttribute(size_t start, ident_t name, ident_t s)
{
   ident_t content = getContent();
   size_t position = start;

   // read tag name
   while (!isWhitespace(content[position]) && content[position] != '>') {
      if (content[position] == 0)
         throw XMLException(position);

      position++;
   }

   // read attributes
   size_t length = 0;
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

         if (name.compare(attr.c_str())) {
            length = position - start;
         }

         position++;
      }
   }
   if (length != 0) {
      _content.cut(start, length);
      _content.insert("\"\"", start);
      _content.insert(s, start + 1);
   }
   else {
      _content.insert("\"\"", position);
      _content.insert(s, position + 1);
      _content.insert(name, position);
      _content.insert("=", position + getlength(name));
      _content.insert(" ", position);
   }
}

size_t XMLTree :: insertNode(size_t  position, ident_t name)
{
   _content.insert(">", position);
   _content.insert(name, position);
   _content.insert("</", position);
   _content.insert(">", position);
   _content.insert(name, position);
   _content.insert("<", position);

   return position;
}
