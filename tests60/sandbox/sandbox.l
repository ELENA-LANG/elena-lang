//import system'calendar;
//import extensions;
//
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

//extension op
//{
//   int get()
//   {
//      int n := 2;
//
//      ^ n
//   }
//}
//
//public program()
//{
//   int r := "dummy".get();
//}

extension op : real
{
   real xln()
   {
      real retVal := PrimitiveRealOperations.ln(self);

      ^ retVal
   }

}

public program()
{
   real n := 1.0;
   real r := n.xln();

   

//   string s := "Hello";
//   int len := s.Length;

//   console.writeLine("Hello");
}