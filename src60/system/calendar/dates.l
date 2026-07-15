namespace calendar
{
   // --- TimeSpan --
   public sealed struct Timespan : BaseValue, info("A time interval")
   {
      SystemTime _value;

      static Timespan Default : info("Returns a zero interval")
         = new Timespan();

      static Timespan MinValue  : info("Returns a zero interval")
         = new SystemTime(0l);

      constructor() : info("Creates a zero interval")
      {
         _value := 0l
      }
      
      constructor(SystemTime val) : info("Converts a system time to an interval")
      {
         _value := val
      }

      constructor new(int hours, int minutes, int seconds) : info("Initializes an interval")
      {
         _value := SystemTime.loadAsTimespan(0, hours, minutes, seconds);
      }

      constructor new(int days, int hours, int minutes, int seconds) : info("Initializes an interval")
      {
         _value := SystemTime.loadAsTimespan(days, hours, minutes, seconds);
      }

      SystemTime SystemTime : info("Returns the interval as a system time")
      {
         get()
         {
            ^ _value
         }
      }

      bool isEmpty() : info("Returns true if the interval is zero")
      {
         long val := _value.Value;

         ^ val == 0;
      }

      get int Days() : info("Gets the days component of the time interval represented by the interval")
      {
         long retVal := _value.Value;
            
         retVal := retVal / 864000000000l;
            
         ^ IntConvertor.convert(retVal)
      }
    
      get int Hours() : info("Gets the hours component of the time interval represented by the interval")
      {
         long retVal := _value.Value;
            
         retVal := retVal / 36000000000l;
            
         ^ IntConvertor.convert(retVal)
      }
                            
      get int Minutes() : info("Gets the minute component of the time interval represented by the interval")
      {
         long retVal := _value.Value;
            
         retVal := retVal / 600000000l;
            
         ^ IntConvertor.convert(retVal)
      }
    
      get int Seconds() : info("Gets the seconds component of the time interval represented by the interval")
      {
         long retVal := _value.Value;
            
         retVal := retVal / 10000000l;
            
         ^ IntConvertor.convert(retVal)
      }
    
      get long Milliseconds() : info("Gets the milliseconds component of the time interval represented by the interval")
      {
         long retVal := _value.Value;
            
         retVal := retVal / 10000l;
            
         ^ new LongNumber(retVal)
      }

      bool equal(Timespan span) : info("Returns the true if the interval equals to <b>span</b>")
      {
         SystemTime value := span.SystemTime;
    
         ^ value.equal(_value)
      }

      bool less(Timespan span) : info("Returns the true if the interval is less than <b>span</b>")
      {
         SystemTime value := span.SystemTime;
    
         ^ _value.less(value)
      }
    
      Date add(Date date) : info("Adds the interval to the provided<b>date</b> and returns a new date")
         = date.add(self);

      Timespan add(Timespan span) : info("Returns the sum of the interval and <b>span</b>")
      {
         SystemTime value := span.SystemTime;
    
         SystemTime retVal := _value + value;
    
         ^ new Timespan(retVal)
      }
    
      Timespan subtract(Timespan span) : info("Returns the difference betwen the interval and <b>span</b>")
      {
         SystemTime value := span.SystemTime;
    
         SystemTime retVal := _value - value;
            
         ^ new Timespan(retVal)
      }
    
      string toPrintable() : info("Returns a string representation of the interval")
      {
         TimeStruct value := _value;
    
         string s := value.toShortTime();
    
         ^ self.Days.toPrintable() + "." + s
      }
    
      Timespan clone() : info("Returns the copy of the interval")
         = new Timespan(_value);      
   }

   // --- Date ---
   public sealed struct Date : BaseValue, info("A date / time structure")
   {
      SystemTime _value;

      constructor() : info("Creates the minimal date/time value")
      {
         _value := 0l
      }
        
      static Date Default : info("Returns the minimal date/time value") = new Date();
    
      static Date MinValue : info("Returns the minimal date/time value") = new Date();

      get constructor Now() : info("Returns the current local date / time")
      {
         SystemTime nowValue := systemTimeFactory.getNow();

         _value := nowValue
      }
          
      get constructor UtcNow() : info("Returns the current UTC date / time")
      {
           SystemTime v := systemTimeFactory.getUtcNow(); 
           _value := v
      }

      constructor(SystemTime val) : info("Converts a system time to the date / time structure")
      {
         _value := val
      }

      constructor new(int year, int month, int day, int hour, int minute, int second) : info("Creates a date / time with the provided values")
      {
         TimeStruct systemTime := new TimeStruct();
         systemTime.Year        := year;
         systemTime.Month       := month;
         systemTime.Day         := day;
         systemTime.Hour        := hour;
         systemTime.Minute      := minute;
         systemTime.Second      := second;
         systemTime.Millisecond := 0;
    
         systemTime.readTo(out SystemTime value);
         _value := value
      }      
        
      constructor new(int year, int month, int day) : info("Creates a date / time with the provided values")
         <= new(year, month, day, 0, 0, 0);
      
      SystemTime cast() = new SystemTime(_value);

      TimeStruct cast()
         = new TimeStruct(_value);
    
      SystemTime SystemTime : info("Returns the date / time as a system time")
      {
         get()
         {
            ^ _value
         }
      }

      bool equal(Date date) : info("Returns the true if the date / time equals to <b>date</b>")
      {
         SystemTime value := date.SystemTime;
            
         ^ value.equal(_value)
      }

      bool less(Date date) : info("Returns the true if the date / time is earlier than <b>date</b>")
      {
         SystemTime value := date.SystemTime;
            
         ^ _value.less(value)
      }
      
      Timespan subtract(Date dt) : info("Returns the difference of the date / time and <b>dt</b> as an interval")
      {
         SystemTime val := dt.SystemTime;
    
         ^ new Timespan(_value.subtract(val))
      }
      
      Date subtract(Timespan timeSpan) : info("Returns the difference of the date / time and <b>timeSpan</b> as a date")
      {
         SystemTime value := timeSpan.SystemTime;
            
         ^ new Date(_value.subtract(value))
      }
      
      Date add(Timespan timeSpan) : info("Returns the sum of the current date / time and <b>timeSpan</b> as a new date")
      {
         SystemTime value := timeSpan.SystemTime;
            
         ^ new Date(_value.add(value))
      }
    
      int Year : info("Gets the year component of the date / time")
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Year;
                
            ^ retVal
         }
      }
    
      int Month : info("Gets the month component of the date / time")
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Month;
                
            ^ retVal
         }
      }
    
      int Day : info("Gets the day component of the date / time")
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Day;
                
            ^ retVal
         }
      }
    
      int DayOfWeek : info("Gets the day of the week component of the date / time")
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.DayOfWeek;
                
            ^ retVal
         }
      }
    
      int Hour : info("Gets the hour component of the date / time")
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Hour;
                
            ^ retVal
         }
      }
    
      int Minute : info("Gets the minute component of the date / time")
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Minute;
                
            ^ retVal
         }
      }
    
      int Second : info("Gets the second component of the date / time")
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Second;
                
            ^ retVal
         }
      }
    
      int Millisecond : info("Gets the millisecond component of the date / time")
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Millisecond;
                
            ^ retVal  
         }
      }
    
      Date addYears(int val) : info("Adds the number of years to the current date and returns a new date")
      {
         TimeStruct st := _value;
    
         int years := st.Year;
         years := years + val;
            
         st.Year := years;
    
         st.readTo(out SystemTime retVal);        
         ^ new Date(retVal)
      }
    
      Date addMonths(int val) : info("Adds the number of months to the current date and returns a new date")
      {
         TimeStruct st := _value;
            
         int months := val;        
         int years := months / 12;        
         months := months - months / 12 * 12;
    
         int n := st.Month;
         months := months + n;
         if (months > 12)
            { months := months - 12; years := years + 1 };
                    
         st.Month := months;
                    
         n := st.Year;
         n += years;
         st.Year := n;        
            
         st.readTo(out SystemTime retVal);        
         ^ new Date(retVal)
      }
    
      addDays(int val) : info("Adds the number of days to the current date and returns a new date")
         = self.add(Timespan.new(val, 0, 0, 0));
    
      addHours(int val) : info("Adds the number of hours to the current date and returns a new date")
         = self.add(Timespan.new(0, val, 0, 0));
    
      addMinutes(int val) : info("Adds the number of minutes to the current date and returns a new date")
         = self.add(Timespan.new(0, 0, val, 0));
    
      addSeconds(int val) : info("Adds the number of seconds to the current date and returns a new date")
         = self.add(Timespan.new(0, 0, 0, val));
    
      string toPrintable() : info("Returns the string representation of the date as a short date and time")
      {
         TimeStruct struct := _value;
    
         string timeValue := struct.toShortTime();
            
         string dateValue := struct.toShortDate();
    
         ^ dateValue + " " + timeValue
      }
    
      Date clone() : info("Returns a copy")
         = new Date(_value);
   }

   // --- obsolete ---    
   public Date now = Date.Now;    
   // --- obsolete ---    
   public Date utcNow = Date.UtcNow;

   // --- now ---    
   public Date Now : info("A current local date") = Date.Now;
   public Date UtcNow : info("A current UTC date") = Date.UtcNow;
}