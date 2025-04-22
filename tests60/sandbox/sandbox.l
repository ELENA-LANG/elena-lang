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

sealed class Record<T>
{
   T arg;

   set(T arg)
   {
      this arg := arg
   }
   
   T get()
      = arg;
}

public program()
{
   auto r := new Record<int?>();
   
   r.set(nil);
   
   int n := r.get()
   

//   auto stack := new Stack<string?>();

//   stack.push(nil)
}
