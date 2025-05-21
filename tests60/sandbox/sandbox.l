import extensions;

template record(params member)::
{
#for(member) 

   member;

   member
     = member;

#endfor

   constructor(
#for(member) 
      member
#endfor
   ) {
#for(member) 
      $value this member := $value member;
#endfor
   }

   constructor()
   {
#for(member) 
      $value this member := default;
#endfor
   }

   bool equal(__typeof o)
   {
#for(member) 
      if($value member != $value get mssg member(o)) {
         ^ false
      };
#endfor

      ^ true
   }
}   

MyRecord : record(string FirstName,string LastName,int Age);

public program()
{
   MyRecord r := new MyRecord("Ivan", "Ivanov", 22);
   MyRecord r2 := new MyRecord("Ivan", "Ivanov", 22);
   MyRecord r3 := new MyRecord("Petr", "Ivanov", 22);

   Console.writeLine(r.FirstName);
   Console.writeLine(r2.FirstName);
   Console.writeLine(r3.FirstName);

   Assert.ifTrue(r == r2);
   Assert.ifFalse(r == r3);
}
