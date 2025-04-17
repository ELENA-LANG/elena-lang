import system'collections;

internal class XmlNodeBase
{
   List<Tuple<string, string>>   _attributes;   
   List<XmlNode>                 _children;
   
   string Name : rprop;
   
   string Value : prop;
}

internal class RootXmlNode : XmlNodeBase
{
   XmlDocument _document;
   
   constructor(XmlDocument document)
   {
      _document := document
   }
}


public class XmlNode : XmlNodeBase
{
   XmlNodeBase _parent;
   
   internal constructor create(XmlNodeBase parent, string name)
   {
      _parent := parent;
      
      Name := name;
   }
}