//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains Config File class implementation
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "config.h"

using namespace elena_lang;

// --- ConfigFile ---

bool ConfigFile :: select(Node& node, ustr_t xpath, Collection& result)
{
   XmlNodeList xmlNodes(XmlNode::Default());
   if (_tree.selectNodes(node.xmlNode, xpath, xmlNodes)) {
      for (auto it = xmlNodes.start(); !it.eof(); ++it) {
         Node child(*it);

         result.nodes.add(child);
      }

      return true;
   }
   else return false;
}

ConfigFile::Node ConfigFile::selectRootNode()
{
   return Node(_tree);
}

ConfigFile::Node ConfigFile::selectNode(ustr_t xpath)
{
   return Node(_tree.selectNode(xpath));
}

ConfigFile::Node ConfigFile :: selectNode(Node& node, ustr_t xpath)
{
   return Node(_tree.selectNode(node.xmlNode, xpath));
}

bool ConfigFile :: load(path_t path, FileEncoding encoding)
{
   try
   {
      if (_tree.load(path, encoding)) {
         return !_tree.empty() && _tree.compareTag("configuration");
      }
   }
   catch (XMLException&)
   {
   }

   return false;
}