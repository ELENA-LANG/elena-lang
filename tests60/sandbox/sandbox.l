//import extensions;
//public enumeration enum<member, T>

template record(params member)::
{
#for(member) 

   member;

   member
     = member;

#endfor

/*
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
*/
   equal(__typeof o)
   {
//#for(member) 
//      if($value member != $value mssg member(o)) {
//         ^ false
//      };
//#endfor
   }
}   

//MyRecord : record(string FirstName,string LastName,int Age);
//MyRecord2 : record(FirstName,LastName,Age);

singleton A { X = 2; }

public program()
{
    var o := mssg X(A);

//   MyRecord r := new MyRecord();
//   MyRecord r := new MyRecord("Ivan", "Ivanov", 22);
}
