import extensions;

class B<T>
{
   T value;

   T get() = value;
}

class A<T> : B<T>
{
   set(T t)
   {
      value := t
   }
}

public program()
{
   A<int> a;

//   var s := new Reference<int>(2);

//   this s.Value := 2
}
