import system'collections;

public class XmlNode
{
   XmlNode                       _parent;
   
   List<Tuple<string, string>>   _attributes;   
   List<XmlNode>                 _children;
   
   string Name    : rprop(_name);
   
   string Content : rprop(_content);
   
   appendChild(XmlNode child)
   {
      if:not:nil(_content)
         InvalidOperationException.raise("The node has already content"); 
      
      _children.append(child);
      
      child.setParent(self);
   }
   
   internal setParent(XmlNode parent)
   {
      if:not:nil(_parent)
         InvalidOperationException.raise("The node has already a parent");
      
      _parent := parent
   }
   
   internal setContent(string content)
   {
      if(_children.Length > 0) {
         InvalidOperationException.raise("The node has children");
      };
      
      _content := content
   }
   
   static XmlNode newElement(string name, string value)
   {
      auto node := XmlNode.new(nil, name, value);
      
      ^ node
   }
   
   static XmlNode newNode(string name, params XmlNode[] children)
   {
      auto node := XmlNode.new(nil, name);
      for (int i := 0; i < children.Length; i++) {
         node.appendChild(children[i]);         
      };
      
      ^ node
   }
   
   internal int getChildIndex(XmlNode child)
   {
      for (int i := 0; i < _children.Length; i++) {
         if (child.equalReference(_children[i])) {
            ^ i;
         }
      };
      
      ^ -1
   }
   
   internal XmlNode getChild(int index)
      = _children[index];
   
   get int ChildCounter()
      = _children.Length;
   
   internal static /*::(string, string)*/Tuple<string, string> parseXPath(int index, string xpath)
   {
      int separator := xpath.indexOf(index, $47);
      if (separator != -1) {
         ^ (xpath.Substring(index, separator - index), xpath.Substring(separator + 1));
      };
      
      ^ (xpath.Substring(index), String.MinValue);
   }
   
   XmlNode findChild(string xpath)
   {
      if (xpath == String.MinValue)
      {
         ^ self
      };
      
      (string current, string subpath) := XmlNode.parseXPath(0, xpath);
      
      if (current != _name) {
         ^ nil
      };
      
      for (int i := 0; i < _children.Length; i++) {
         auto node := _children[i].findChild(subpath);
         if:not:nil(node) {
            ^ node
         }
      };
      
      ^ nil
   }
   
   XmlNode nextSibling()
   {
      if:nil(_parent)
      {
         ^ nil;
      };
      
      int index := _parent.getChildIndex(self);
      int length := _parent.ChildCounter;
      
      index += 1;
      if (index < length) {
         ^ _parent.getChild(index)
      };
      
      ^ nil
   }
   
   XmlNode previousSibling()
   {
      if:nil(_parent)
         { ^ nil };
      
      int index := _parent.getChildIndex(self);
      
      if (index > 0) {
         ^ _parent.getChild(index - 1)
      };
      
      ^ nil      
   }
   
   XmlNode firstChild()
   {
      if (_children.Length == 0) {
         ^ nil
      };
      
      ^ _children[0]
   }
   
   XmlNode lastChild()
   {
      if (_children.Length == 0) {
         ^ nil
      };
      
      ^ _children[_children.Length - 1]
   }   
   
   internal constructor new(XmlNode? parent, string name)
   {
      _attributes := new List<Tuple<string, string>>();
      _children := new List<XmlNode>();
      _name := name;
      
      if:not:nil(parent)
         parent.appendChild(self)
   }
   
   internal constructor new(XmlNode? parent, string name, string content)
   {
      _attributes := new List<Tuple<string, string>>();
      _children := new List<XmlNode>();
      _name := name;
      _content := content;
      
      if:not:nil(parent)
         parent.appendChild(self)
   }
}
