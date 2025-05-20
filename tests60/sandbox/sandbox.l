//import extensions;
//public enumeration enum<member, T>

public xenum<T>(params member)::
{

#for(member)

   static member
     = nil; 

#endfor

}

public class MyColor : xenum<int>(Red, Blue, Green);

//template class Record(member)::
//template class Record<>(..member)
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

public program()
{
//   MyRecord r := new MyRecord();
//   MyRecord r := new MyRecord("Ivan", "Ivanov", 22);
}
