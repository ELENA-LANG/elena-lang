//import extensions;
//public enumeration enum<member, T>

template record(params member)::
{
#for(member) 

//   __privatename member;

   member
//     = __privatename member;
     = default; 

#endfor

//   constructor(..member)
//   {
//#for(member) 
//      this __privatename member := __argumentof member;
//#endfor
//   }
}   

MyRecord : record(string FirstName,string LastName,int Age);

public program()
{
//   MyRecord r := new MyRecord();
//   MyRecord r := new MyRecord("Ivan", "Ivanov", 22);
}
