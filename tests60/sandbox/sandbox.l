import extensions;

//public interface Enumerator<T>
//{
//   Enumerator<T> enumerator() = self;
//}

public class XTest<T>
{
   T arg;

   constructor(T arg) 
   {
      this arg := arg;
   }

   T get()
      = arg;
}

A
{
//   XTest<int> test;

//   test(XTest<int> arg)
//   {
//   }

   test()
   {
   }
}

public program()
{
   XTest<int> o := new XTest<int>(2);

//   var a := new Enumerator<object>();

   console.writeLine("Hello");
}
