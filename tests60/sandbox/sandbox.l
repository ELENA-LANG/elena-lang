import extensions;
import xml;
import system'io;

singleton Tester
{
   read()
   {
      bool attributeMode := false;
      
//      bool available := true;
      auto tokenMode := 1;
   
      if:not(b) {
      }

      string currentLine := ">12n";
            
      int offs := 0;
      
      if (tokenMode == /*TokenType.AttributeValue*/1) {
//         tokenMode := TokenType.OpenTag;

         if (currentLine[offs] == $62) {
//            attributeMode := false;
            offs++
         }
         else attributeMode := true
      };

   }      
}

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
   Tester.read();
   
//   var reader := XmlReader.open();
//   reader.read();
   
   
   
   auto xmlDoc := XmlDocument.loadFromFile("db.xml");

   printNode(0, xmlDoc.Root);
}
