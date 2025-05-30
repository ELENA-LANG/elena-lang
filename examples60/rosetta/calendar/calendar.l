import system'text;
import system'routines;
import system'calendar;
import extensions;
import extensions'routines;

// --- calendar ---

const MonthNames = new string[]{"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"};
const DayNames = new string[]{"MO", "TU", "WE", "TH", "FR", "SA", "SU"};

class CalendarMonthPrinter
{
   Date           _date;
   TextBuilder    _line;
   int            _month;
   int            _year;
   Reference<int> _row;
    
   constructor(year, month)
   {
      _month := month;
      _year := year;
      _line := new TextBuilder();
      _row := 0;
   }

   writeTitle()
   {
      _row.Value := 0;
      _date := Date.new(_year, _month, 1);

      DayNames.forEach::(name)
         { _line.print(" ",name) }
   }
    
   writeLine()
   {
      _line.clear();

      if (_date.Month == _month)
      {
         var dw := _date.DayOfWeek;

         _line.writeCopies("   ",_date.DayOfWeek == 0 ? 6 : (_date.DayOfWeek - 1));

         do
         {
            _line.writePaddingLeft(_date.Day.toPrintable(), $32, 3);

            _date := _date.addDays(1)
         }
         until(_date.Month != _month || _date.DayOfWeek == 1)
      };

      int length := _line.Length;
      if (length < 21)
         { _line.writeCopies(" ", 21 - length) };

      _row.append(1)
   }
    
   indexer() = new Indexer
   {
      bool Available = _row < 7;

      int Index
      {
         get() = _row.Value;
            
         set(int index)
         {
            if (index <= _row)
               { self.writeTitle() };
                    
            while (index > _row)
               { self.writeLine() }
         }
      }

      appendIndex(int index)
      {
         this self.Index := _row.Value + index
      }
        
      get int Length() { ^ 7 }

      get Value() = self;
        
      set Value(o) { NotSupportedException.raise() }
   };
    
   printTitleTo(output)
   {
      output.writePadding(MonthNames[_month - 1], $32, 21)
   }
   
   printTo(output)
   {
      output.write(_line.Value)
   }
}

class Calendar
{
   int _year;
   int _rowLength;
    
   constructor new(int year)
   {
      _year := year;
      _rowLength := 3
   }
    
   printTo(output)
   {
      output.writePadding("[SNOOPY]", $32, _rowLength * 25); 
      output.writeLine();
      output.writePadding(_year.toPrintable(), $32, _rowLength * 25); 
      output.writeLine().writeLine();
        
      var rowCount := 12 / _rowLength;
      var months := Array.allocate(rowCount).populate::(i =>
            Array.allocate(_rowLength) 
                  .populate::(j =>
                     new CalendarMonthPrinter(_year, i * _rowLength + j + 1)));
        
      months.forEach::(row)
      {
         var r := row;
            
         row.forEach::(month)
         {
            month.printTitleTo(output);
                    
            output.write("   ")
         };
            
         output.writeLine();
            
         ParallelEnumerator.new(row).forEach::(line)
         {
            line.forEach::(printer)
            {
               printer.printTo(output);

               output.write("   ")
            };

            output.writeLine()
         }
      }        
   }
}

// --- program ---

public program()
{
   var calender := Calendar.new(Console.write("ENTER THE YEAR:").readLine().toInt());
    
   calender.printTo(Console);

   Console.readChar()
}