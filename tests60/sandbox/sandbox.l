//import system'calendar;
import extensions;

//const int MAX = 50000000;
//
//public program()
//{
//    real pi := 0.0;
//    for(int i := 0, i < MAX, i += 1)
//    {
//        pi += -1.0.power(i) / (i*2+1) * 4
//    };
//
//    console.printLine(pi);
//}

singleton LookaheadHelper
{
   int X()
   {
      int r := 2;

      ^ r   
   }   

   WeakX()
   {
      ^ 3;
   }   
}

public program()
{
   int r := LookaheadHelper.X() + 1;
   int r2 := LookaheadHelper.WeakX() + 1;

   Assert.ifTrue(r == 3);
   Assert.ifTrue(r2 == 4);
}