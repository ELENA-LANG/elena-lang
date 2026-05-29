import system'collections;
import system'text;
import system'io;
import extensions;

public interface IXmlFormatter
{
   abstract openTag(TextWriter output, int level, XmlNode node);
   
   abstract writeElement(TextWriter output, int level, XmlNode node);
   
   abstract closeTag(TextWriter output, int level, XmlNode node);
}

public interface IXmlReader
{
   abstract read();
   
   get abstract bool isAvailable();
   
   get abstract bool isOpenTag();
   
   get abstract bool isCloseTag();
   
   get abstract bool isContent();

   get abstract isAttribute();
   
   get abstract isAttributeValue();
   
   get abstract string Name();
   
   get abstract string Content();
}

public class XmlDocument
{
   XmlNode _root;
   
   constructor loadFromFile(string path)
      <= loadFromFile(path, UTF8Encoder);
   
   constructor loadFromFile(string path, Encoder encoder)
   {
      using(auto reader := TextStreamReader.new(path, encoder)) {
         load(XmlReader.open(reader));
      };      
   }
   
   constructor loadFromString(string xml)
   {
      auto stream := MemoryStream.load(xml.toByteArray());      
      using(auto reader := TextStreamReader.new(stream, UTF8Encoder)) {
         load(XmlReader.open(reader));
      };      
   }
   
   constructor load(TextReader reader)
   {
      load(XmlReader.open(reader));
   }
   
   constructor new(XmlNode root)
   {
      _root := root;
   }

   constructor new()
   {
   }

   XmlNode Root
      = _root;

   load(IXmlReader reader)
   {
      Stack<XmlNode> nodes := new Stack<XmlNode>();
      
      reader.read();
      while(reader.isAvailable) {
         if (reader.isOpenTag) {
            XmlNode node := XmlNode.newNode(reader.Name);
            
            nodes.push(node);
         }
         else if(reader.isCloseTag) {
            XmlNode node := nodes.pop();
            if (nodes.Length > 0) {
               nodes.peek().appendChild(node)
            }
            else if:not:nil(_root) { 
               InvalidOperationException.raise("The XML can have only one root element");
            }            
            else _root := node;
         }
         else if (reader.isContent) {
            nodes.peek().setContent(reader.Content);
         };
         
         reader.read();
      }
   }

   save(IXmlFormatter formatter, TextWriter output)
   {
      var Stack<XmlNode?> stack := new Stack<XmlNode?>();
      
      int level := 0;
      stack.push(_root);
      while (stack.Length > 0) {
         XmlNode? node := stack.pop();
         if:not:nil(node) {
            if (node.ChildCounter > 0) {               
               level++;
               formatter.openTag(output, level, node);
                  
               stack.push(node);
               stack.push(nil);
               
               XmlNode? child := node.firstChild();
               if:not:nil(child)
                  stack.push(child);
            }
            else {
               formatter.writeElement(output, level + 1, node);
               
               XmlNode? next := node.nextSibling();
               if:not:nil(next)
                  stack.push(next);               
            }
         }
         else {
            node := stack.pop();
            
            formatter.closeTag(output, level, node!);
            level--;
               
            XmlNode? next := node?.nextSibling();
            if:not:nil(next)
               stack.push(next);
         }
      };
   }

   XmlNode? findNode(string xpath)
   {
      if (xpath[0]==$47) {
         (string current, string subpath) := XmlNode.parseXPath(1, xpath);
         
         if (_root.Name == current) {
            ^ _root.findChild(subpath)
         };
      };
      
      ^ nil;
   }
}