import extensions;
import system'routines'stex;
import system'collections;
 
const int N = 20000;
/* 
extension op : IntNumber
{
    Enumerator<int> ProperDivisors
        = new Range(1,self / 2).filterBy::(int n => self.mod(n) == 0);
 
    get AmicablePairs()
    {
        auto divsums := new List<int>(
            cast Enumerator<int>(
                new Range(0, self).selectBy::(int i => i.ProperDivisors.summarize(0))));
 
        ^ new Range(0, divsums.Length)
            .filterBy::(int i)
            {
                auto sum := divsums[i];
                ^ (i < sum) && (sum < divsums.Length) && (divsums[sum] == i)
            }
            .selectBy::(int i => new Tuple<int,int>(i,divsums[i]));
    }
}
*/ 

/*
public decoratorTest()
{
   var a := new DecoratorBaseA(2);
   var decA := new DecoratorA(a, 3);

   Assert.ifEqual((decA as:IDecoratorA).giveMe(), 5);

   Console.write(".");
}

interface IDecoratorA
{
   abstract giveMe();
}

class DecoratorBaseA : IDecoratorA
{
   field value;

   constructor(value)
   {
      this value := value
   }

   giveMe()
      = value;
}

class DecoratorA
{
   object target;
   object extra;

   constructor(target, extra)
   {
      this target := target;
      this extra := extra;
   }

   giveMe()
      = target.giveMe() + extra;

   generic cast()
   {
      var type := __received.__getFirstSignatureMember();

      var proxy := ::{ dispatch() => self; };
      proxy.__injectInferface(type);
   
      ^ __received(proxy);
   }
}
*/

/*extension testOp<T> : T
{
   testMe(Func<T,T> f)
      = f(self);
}*/

public program()
{
   var f := ([object]int x => x + 1);


//   var r := 3::testMe<int>((int x => x + 1));
   

/*
   auto source := "Standard comparison operations are defined for all the primitive numeric types.";
   auto expected := "U3RhbmRhcmQgY29tcGFyaXNvbiBvcGVyYXRpb25zIGFyZSBkZWZpbmVkIGZvciBhbGwgdGhlIHByaW1pdGl2ZSBudW1lcmljIHR5cGVzLg==";
   
   auto buffer := MemoryBuffer.allocate();
   UTF8Encoding.fromString(source, buffer);
   var dest := Base64Encoding.toString(0, buffer.Length, *buffer);
   
   Assert.ifEqual(dest, expected);
*/
   
//   buffer.clear();
//   Base64Encoding.fromString(dest, buffer);
   
//   var decoded := UTF8Encoding.toString(buffer);
//   Assert.ifEqual(decoded, source);
//   Console.write(".");

//   decoratorTest();


//   Enumerator<int> e := new Range(0, N).selectBy::(int i => /*i.ProperDivisors.summarize(0)*/i + 1);

/*    N.AmicablePairs.forEach::(var Tuple<int,int> pair)
    {
        Console.printLine(pair.Item1, " ", pair.Item2)
    }*/
}