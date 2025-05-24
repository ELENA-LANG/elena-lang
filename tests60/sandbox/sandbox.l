import extensions;

MyRecord : record(string FirstName,string LastName,int Age);

public program()
{
   MyRecord r := new MyRecord("Ivan", "Ivanov", 22);
   MyRecord r2 := new MyRecord("Ivan", "Ivanov", 22);
   MyRecord r3 := new MyRecord("Petr", "Ivanov", 22);

   Assert.ifTrue(r == r2);
   Assert.ifFalse(r == r3);
}
