A;

public sealed  XRef<T>
{
   constructor(T val) 
   {
      T arg := val
   }
}

public program()
{
   //Reference<A> ref;
  XRef<XRef<A>> ref;
}
