//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains Config File class header
//                                            (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"
#include "xmltree.h"

namespace elena_lang
{
   // --- ConfigFile ---
   class ConfigFile
   {
      XmlTree _tree;

   public:
      class Node
      {
         friend class ConfigFile;

         XmlNode xmlNode;

      public:
         bool compareAttribute(ustr_t key, ustr_t value)
         {
            XmlTree::Attribute attribute;
            xmlNode.readAttribute(key, attribute);

            return value.compare(attribute.str());
         }

         bool readAttribute(ustr_t key, DynamicString<char>& s)
         {
            XmlNode::Attribute attr;
            if (xmlNode.readAttribute(key, attr)) {
               s.copy(attr.str());

               return true;
            }
            else return false;
         }

         void readContent(DynamicString<char>& s)
         {
            xmlNode.readContent(s);
         }

         bool isNotFound()
         {
            return xmlNode.isNotFound();
         }

         Node(XmlNode node)
         {
            this->xmlNode = node;
         }
         Node()
         {
            this->xmlNode = XmlNode();
         }
      };

      typedef List<Node> NodeList;

      class Collection
      {
         friend class ConfigFile;

         NodeList nodes;

      public:
         NodeList::Iterator start()
         {
            return nodes.start();
         }

         Collection()
            :nodes(Node())
         {

         }
      };

      bool select(Node& node, ustr_t xpath, Collection& collection);
      bool select(ustr_t xpath, Collection& collection);

      Node selectNode(ustr_t xpath);
      Node selectNode(Node& node, ustr_t xpath);

      Node selectRootNode();

      template<class ArgT> Node selectNode(ustr_t xpath, ArgT arg, bool(*filter)(ArgT arg, Node& node))
      {
         XmlNodeList list(XmlNode::Default());
         _tree.selectNodes(xpath, list);
         for(auto it = list.start(); !it.eof(); ++it) {
            Node node(*it);
            if (filter(arg, node))
               return node;
         }

         return Node();
      }

      void appendSetting(ustr_t xpath, ustr_t value);

      bool load(path_t path, FileEncoding encoding);
      bool save(path_t path, FileEncoding encoding);

      ConfigFile() = default;
      ConfigFile(ustr_t rootTag)
         : _tree(rootTag)
      {
      }
   };
}

#endif
