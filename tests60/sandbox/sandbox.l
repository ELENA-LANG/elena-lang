import extensions;
import xml;

printNode(int indent, XmlNode root)
{   
   Console.fillString(" ", indent);
   Console.print("<", root.Name);
   
   
   
   Console.print(">");
   
   if (root.ChildCounter > 0) {
      Console.writeLine();
      XmlNode? current := root.firstChild();
      while (current != nil) {
         printNode(indent + 3, current);
         
         current := current.nextSibling();
      };
      
      Console.fillString(" ", indent);
   }
   else {
      Console.write(root.Content);
   };
   Console.printLine("</", root.Name, ">");
}

public Program()
{
   auto xmlDoc := XmlDocument.loadFromFile("db.xml");

   printNode(0, xmlDoc.Root);

}
