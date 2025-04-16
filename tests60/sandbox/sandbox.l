import algorithms;
import extensions;

public singleton IfOrdered<T> : Func<T,T,bool>
{
   bool function(T f, T l)
      = f < l;
}

public program()
{
   var list := new int[]{3, 9, 4, 6, 8, 1, 7, 2, 5};

   Console.printLine("after :", list.insertSort().asEnumerable());
}
