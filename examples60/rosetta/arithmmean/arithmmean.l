///  a program to find the mean (arithmetic average) of a numeric vector. 
import extensions;

// --- Average ---

extension op
{
   average()
   {
      real sum := 0;
      int count := 0;
        
      var enumerator := self.enumerator();
        
      while (enumerator.next())
      {
         sum += *enumerator;
         count += 1;
      };
        
      ^ sum / count
   }
}

// --- Program ---

public program()
{
   var array := new int[]{1, 2, 3, 4, 5, 6, 7, 8};
   Console.printLine(
      "Arithmetic mean of {",array.asEnumerable(),"} is ",
      array.average()).readChar()
}