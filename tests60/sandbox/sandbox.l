//import extensions;
//public enumeration enum<member, T>

public xenum<T>(params member)::
{
   T _value;

   const T[] __enumValues;
   const string[] __enumNames;

   private constructor() {} 

   private constructor load(T v) { _value := v }

#for(member)

   this __enumValues += $value member;
   this __enumNames += $name member;

   get static __typeof member()
      = self.load($value member);

#endfor

}

public class MyColor : xenum<int>(Red = 1, Blue = 1, Green = 1);

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
