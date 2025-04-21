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

#if (__project["_Win32"])

class Win32A
{
}

#endif

#if (__project["_Win64"])

class Win64A
{
}

#endif

public program()
{
//   auto stack := new Stack<string?>();

//   stack.push(nil)
}