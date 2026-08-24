//import extensions;
//import system'collections;
//import system'routines;
// 
//isHappy(int n)
//{
//    auto cache := new List<int>(5);
//    int sum := 0;
//    int num := n;
//    while (num != 1)
//    {
//        if (cache.indexOfElement(num) != -1)
//        {
//            ^ false
//        };
//        cache.append(num);
//        while (num != 0)
//        {
//            int digit := num.mod(10);
//            sum += (digit*digit);
//            num /= 10
//        };
//        num := sum;
//        sum := 0
//    };
// 
//    ^ true
//}
// 
//public Program()
//{
//    auto happynums  := new List<int>(8);
//    int num := 1;
//    while (happynums.Length < 8)
//    {
//        if (isHappy(num))
//        {
//            happynums.append(num)
//        };
// 
//        num += 1
//    };
//    console.printLine("First 8 happy numbers: ", happynums.asEnumerable())
//}

public interface I
{
   abstract doSomething(arg);
}

public interface I<T>
{
   abstract T doSomething(T arg);
}

public class A : interface<I<int>>, interface<I>
{
   int doSomething(int arg)
      = arg + 1;
/*      
   I cast()
      = I {
         doSomething(arg)
            => self;
      };            */
}

public Program()
{
   A a := new A();
   
   I<int> i1 := a;
   I i2 := a;
   
   int r := i1.doSomething(2);
   int r2 := i2.doSomething(2);
}
