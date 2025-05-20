//import extensions;
//public enumeration enum<member, T>

template record(params member)::
{
#for(member) 

   member;

   member
     = member;

#endfor

   constructor()
   {
#for(member) 
      $name this member := default;
#endfor
   }
}   

MyRecord : record(string FirstName,string LastName,int Age);

public program()
{
//   MyRecord r := new MyRecord();
//   MyRecord r := new MyRecord("Ivan", "Ivanov", 22);
}
