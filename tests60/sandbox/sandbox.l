import extensions;
import system'collections;

   public sealed XList<T>
   {
//      T[]            _array;
    
      /// Appends a new object to the end of the list
      append(T object)
      {
         console.writeLine(object)
      }
    
//      append(object)
//         <= append(cast T(object));
}

A
{
   int cast() = 3;
}

public program()
{
   auto l := new XList<int>();

   l.append(2);
   l.append(new A());
}