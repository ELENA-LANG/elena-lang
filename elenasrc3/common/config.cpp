//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains Config File class implementation
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "config.h"

using namespace elena_lang;

// --- ConfigFile ---

bool ConfigFile :: select(ustr_t xpath, Collection& collection)
{
   Node rootNode = selectRootNode();

   size_t index = xpath.find('/');

   return select(rootNode, xpath + index + 1, collection);
}

bool ConfigFile :: select(Node& node, ustr_t xpath, Collection& result)
{
   if (node.isNotFound())
      return false;

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

ConfigFile::Node ConfigFile :: selectRootNode()
{
   return Node(_tree);
}

ConfigFile::Node ConfigFile :: selectNode(ustr_t xpath)
{
   return Node(_tree.selectNode(xpath));
}

ConfigFile::Node ConfigFile :: selectNode(Node& node, ustr_t xpath)
{
   return Node(_tree.selectNode(node.xmlNode, xpath));
}

void ConfigFile :: appendSetting(ustr_t xpath, ustr_t value)
{
   XmlNode node = _tree.insertNode(xpath);
   if (node.isNotFound()) {
      assert(false);
   }
   else node.setContent(value);
}

void ConfigFile :: create()
{
   _tree.loadXml("<configuration></configuration>");
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

bool ConfigFile :: save(path_t path, FileEncoding encoding)
{
   return _tree.save(path, encoding, false, true);
}
