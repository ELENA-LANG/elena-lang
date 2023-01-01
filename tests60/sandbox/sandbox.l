import extensions;

public sealed XArray<T> //: Indexable<T>
{
   embeddable __string T[] array;

   constructor allocate(int length)
      = new T[](length);   
}

public program()
{
   XArray<int> r;

   console.writeLine("Hello");
}
