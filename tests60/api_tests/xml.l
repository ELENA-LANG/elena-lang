import system'io;
import xml;
import extensions'text;

// == XML Library tests ==
// =======================

// --- LoadSave Tests ---

public xmlLoadSaveTest() : testCase()
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

   var expected := "<Employees>
	<Employee>
		<FullName>Lydia Thomason</FullName>
		<Salary>25.64</Salary>
		<DepartmentID>1</DepartmentID>
	</Employee>
	<Employee>
		<FullName>June Grath</FullName>
		<Salary>16.38</Salary>
		<DepartmentID>4</DepartmentID>
	</Employee>
</Employees>";

   var actual := String.MinValue;

   using(auto writer := new StringWriter()) {
     doc.save(TabXmlFormatter, writer);

     actual := writer.Value
   };   

   Assert.ifEqual(actual, expected);
}