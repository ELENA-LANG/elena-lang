//
//   public sealed struct TimeStruct
//   {
//      short _year;
//      short _month;
//
//      private string timeToString(int flags)
//      {
//         ^ "abc"
//      }
//    
//      private wide timeToWide(int flags)
//      {
//         ^ "abc"w
//      }
//
//      wide toShortTime()
//         = self.timeToWide(0);
//
//      retoverload string toShortTime()
//         = self.timeToString(0);
//
//   }
// 
//public program()
//{
//   TimeStruct tm := default;
//
//   string s := tm.toShortTime()
//}

A;

public singleton Tester
{
   getValue()
      = self;
      
   retoverload A getValue()
      = new A();
}

public Program()
{
   A a := Tester.getValue();
}
