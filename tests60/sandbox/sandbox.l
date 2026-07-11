import extensions;
import xml;
import system'io;

printNode(int indent, XmlNode root)
{   
   Console.fillString(" ", indent);
   Console.print("<", root.Name);
   
   if (root.HasAttributes) {
      foreach(var t; in root.Attributes) {
         Console.print(" ", t.Item1,"=""", t.Item2, """")
      }
   };
   
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
      Console.write(root.Content ?? String.MinValue);
   };
   Console.printLine("</", root.Name, ">");
}

public Program()
{
   short l := 3;
   short m := 3;
   short r := l | m;

   auto xmlDoc := XmlDocument.loadFromFile("db.xml");

   printNode(0, xmlDoc.Root);
}
