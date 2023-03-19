import extensions;
import extensions'routines;
import system'routines;
import system'text;
import system'calendar;
import system'runtime;

//GCTestObject1;
//GCTestObject2;
//
//class GCTestContainer
//{
//   object field1 : prop;
//   object field2 : prop;
//
//   printMe()
//   {
//      console.writeLine(field1);
//      console.writeLine(field2);
//   }
//}
//
//gcTest_Create()
//{
//   ^ new GCTestContainer();
//}
//
//gcTest_Fill(c)
//{
//   c.field1 := new GCTestObject1();
//   c.field2 := new GCTestObject2();
//}
//
//gcTest_Test(c)
//{
//   c.printMe();
//}
//
//gcTest()
//{
//   var c := gcTest_Create();
//   var r := Array.allocate(20);
//
//   GCManager.collectFull();
//
//   gcTest_Fill(c);
//
//   GCManager.collectMinor();
//
//   gcTest_Test(c);
//}

   public sealed struct TimeStruct
   {
      short _year;
      short _month;
      short _dayOfWeek;
      short _day;

      constructor()
      {
         _year := 1;
         _month := 2;
         _dayOfWeek := 3;
         _day := 4;
      }

      int Day
      {
         get()
         {
            int val := _day;
                
            ^ val
         }
      }
      
   }

public program()
{
   var x := new TimeStruct();
   var d := x.Day;
   GCManager.collectFull();
   var d2 := x.Day;

//   var date := Date.new(2023,1,1);   
//   console.writeLine(date.Day.toPrintable());
//   GCManager.collectFull();
//   console.writeLine(date.Day.toPrintable());

//   var _line := new TextBuilder();
//   _line.clear();
//   _line.writeCopies("1111", 200);
//   _line.clear();
//   _line.writeCopies("1111", 400);

//   console.writeLine(_line);

//   var _date := Date.new(2023, 3, 6);
//
//   var _line := new TextBuilder();
//   var _month := 3;
//
//        _line.clear();
//        
//        if (_date.Month == _month)
//        {
//            var dw := _date.DayOfWeek;
//
//            _line.writeCopies("   ",_date.DayOfWeek == 0 ? 6 : (_date.DayOfWeek - 1));
//            
//            do
//            {
//                _line.writePaddingLeft(_date.Day.toPrintable(), $32, 3);
//                
//                _date := _date.addDays:1
//            }
//            until(_date.Month != _month || _date.DayOfWeek == 1)
//        };
//        
//        int length := _line.Length;
//        if (length < 21)
//            { _line.writeCopies(" ", 21 - length) };
//
//   console.writeLine(_line);
}