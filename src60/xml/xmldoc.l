import system'text;

public class XmlDocument
{
   XmlNode _root;
   
   constructor new(XmlNode root)
   {
      _root := root;
   }

   XmlNode findNode(string xpath)
   {
      if (xpath[0]==$47) {
         string current, string subpath := XmlNode.parseXPath(1, xpath);
         
         if (_root.Name == current) {
            ^ _root.findChild(subpath)
         };
      };
      
      ^ nil;
   }
} 