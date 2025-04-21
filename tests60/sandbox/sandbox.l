//import system'io;
//import xml;

/*
public program()
{
   XmlDocument doc := XmlDocument.new(
      XmlNode.newNode("Employees", 
         XmlNode.newNode("Employee", 
            XmlNode.newElement("FullName", "Lydia Thomason"),
            XmlNode.newElement("Salary", "25.64"),
            XmlNode.newElement("DepartmentID", "1")
         ),
         XmlNode.newNode("Employee", 
            XmlNode.newElement("FullName", "June Grath"),
            XmlNode.newElement("Salary", "16.38"),
            XmlNode.newElement("DepartmentID", "4")
         )
      )
   );
   
   using(auto writer := File.textwriter("db.xml")) {
      doc.save(TabXmlFormatter, writer)
   }
}
*/

class A
{
  test() 
  {
#if (__project["_Win32"])

    Console.writeLine("I'm 32 bit app");

#elif (__project["_Win64"])

    Console.writeLine("I'm 64 bit app");

#endif
  }
}

public program()
{
//   auto stack := new Stack<string?>();

//   stack.push(nil)
}