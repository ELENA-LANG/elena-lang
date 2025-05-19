//import extensions;

//class Record<>(..member)
//{
//#for(member) 

//   __privatename member;

//   member
//     = __privatename member;
//     = nil; 

//#endfor

//   constructor(..member)
//   {
//#for(member) 
//      this __privatename member := __argumentof member;
//#endfor
//   }
//}   

//MyRecord : record(FirstName,LastName,Age);

class TemplateWithDefaults<T>(defValue)::
{
   T X := defValue;
}

MyClass : TemplateWithDefaults<int>(2)
{
}

public program()
{
//   TemplateWithDefaults<int,defValue=2> arg := new TemplateWithDefaults<int,defValue=2>();

//   MyRecord r := new MyRecord();
//   MyRecord r := new MyRecord("Ivan", "Ivanov", 22);
}
