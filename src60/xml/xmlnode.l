import system'collections;

public class XmlNode
{
   XmlNode                       _parent;
   
   List<Tuple<string, string>>   _attributes;   
   List<XmlNode>                 _children;
   
   string Name  : rprop(_name);
   
   string Value : prop(_value);
   
   appendChild(XmlNode child)
   {
      _children.append(child);
      
      child.setParent(self);
   }
   
   internal setParent(XmlNode parent)
   {
      if:not:nil(_parent)
         InvalidOperationException.raise("The node has already a parent");
      
      _parent := parent
   }
   
   static XmlNode newNode(string name, string value, params XmlNode[] children)
   {
      auto node := XmlNode.new(nil, name, value);
      for (int i := 0; i < children.Length; i++) {
         node.appendChild(children[i]);         
      };
      
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
   
   internal static ::(string, string) parseXPath(int index, string xpath)
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
      
      string current, string subpath := XmlNode.parseXPath(0, xpath);
      
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
   
   internal constructor new(XmlNode? parent, string name)
   {
      _attributes := new List<Tuple<string, string>>();
      _children := new List<XmlNode>();
      _name := name;
      
      if:not:nil(parent)
         parent.appendChild(self)
   }
   
   internal constructor new(XmlNode? parent, string name, string value)
   {
      _attributes := new List<Tuple<string, string>>();
      _children := new List<XmlNode>();
      _name := name;
      _value := value;
      
      if:not:nil(parent)
         parent.appendChild(self)
   }
}
