import system'collections;
import system'text;
import system'io;

public interface IXmlFormatter
{
   abstract openTag(TextWriter output, int level, XmlNode node);
   
   abstract writeElement(TextWriter output, int level, XmlNode node);
   
   abstract closeTag(TextWriter output, int level, XmlNode node);
}

public class XmlDocument
{
   XmlNode _root;
   
   constructor new(XmlNode root)
   {
      _root := root;
   }

   save(IXmlFormatter formatter, TextWriter output)
   {
      Stack<XmlNode?> stack := new Stack<XmlNode?>();
      
      int level := 0;
      stack.push(_root);
      while (stack.Length > 0) {
         XmlNode node := stack.pop();
         if:not:nil(node) {
            if (node.ChildCounter > 0) {               
               level++;
               formatter.openTag(output, level, node);
                  
               stack.push(node);
               stack.push(nil);
               
               XmlNode child := node.firstChild();
               if:not:nil(child)
                  stack.push(child);
            }
            else {
               formatter.writeElement(output, level + 1, node);
               
               XmlNode next := node.nextSibling();
               if:not:nil(next)
                  stack.push(next);               
            }
         }
         else {
            node := stack.pop();
            
            formatter.closeTag(output, level, node);
            level--;
               
            XmlNode next := node.nextSibling();
            if:not:nil(next)
               stack.push(next);
         }
      };
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