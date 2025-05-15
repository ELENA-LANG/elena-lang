import algorithms;
import extensions;

public program()
{
   auto list2 := new int[]{3, 9, 4, 6, 8, 1, 7, 2, 5};

   Console.printLine("after :", list2.insertSort().asEnumerable());
}