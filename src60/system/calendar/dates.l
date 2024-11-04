namespace calendar
{
   // --- TimeSpan --
   public sealed struct Timespan : BaseValue
   {
      SystemTime _value;

      static Timespan Default
         = new Timespan();

      static Timespan MinValue 
         = new SystemTime(0l);

      constructor()
      {
         _value := 0l
      }
      
      constructor(SystemTime val)
      {
         _value := val
      }

      constructor new(int hours, int minutes, int seconds)
      {
         _value := SystemTime.loadAsTimespan(0, hours, minutes, seconds);
      }

      constructor new(int days, int hours, int minutes, int seconds)
      {
         _value := SystemTime.loadAsTimespan(days, hours, minutes, seconds);
      }

      SystemTime SystemTime
      {
         get()
         {
            ^ _value
         }
      }

      bool isEmpty()
      {
         long val := _value.Value;

         ^ val == 0;
      }

      get int Days()
      {
         long retVal := _value.Value;
            
         retVal := retVal / 864000000000l;
            
         ^ IntConvertor.convert(retVal)
      }
    
      get int Hours()
      {
         long retVal := _value.Value;
            
         retVal := retVal / 36000000000l;
            
         ^ IntConvertor.convert(retVal)
      }
                            
      get int Minutes()
      {
         long retVal := _value.Value;
            
         retVal := retVal / 600000000l;
            
         ^ IntConvertor.convert(retVal)
      }
    
      get int Seconds()
      {
         long retVal := _value.Value;
            
         retVal := retVal / 10000000l;
            
         ^ IntConvertor.convert(retVal)
      }
    
      get long Milliseconds()
      {
         long retVal := _value.Value;
            
         retVal := retVal / 10000l;
            
         ^ new LongNumber(retVal)
      }

      bool equal(Timespan span)
      {
         SystemTime value := span.SystemTime;
    
         ^ value.equal(_value)
      }

      bool less(Timespan span)
      {
         SystemTime value := span.SystemTime;
    
         ^ _value.less(value)
      }
    
      add(Date date)
         = date.add(self);

      add(Timespan span)
      {
         SystemTime value := span.SystemTime;
    
         SystemTime retVal := _value + value;
    
         ^ new Timespan(retVal)
      }
    
      subtract(Timespan span)
      {
         SystemTime value := span.SystemTime;
    
         SystemTime retVal := _value - value;
            
         ^ new Timespan(retVal)
      }
    
      string toPrintable()
      {
         TimeStruct value := _value;
    
         string s := value.toShortTime();
    
         ^ self.Days.toPrintable() + "." + s
      }
    
      Timespan clone()
         = new Timespan(_value);      
   }

   // --- Date ---
   public sealed struct Date : BaseValue
   {
      SystemTime _value;

      constructor()
      {
         _value := 0l
      }
        
      static Date Default = new Date();
    
      static Date MinValue = new Date();

      get constructor Now()
      {
         SystemTime nowValue := systemTimeFactory.getNow();

         _value := nowValue
      }
          
      get constructor UtcNow()
      {
           SystemTime v := systemTimeFactory.getUtcNow(); 
           _value := v
      }

      constructor(SystemTime val)
      {
         _value := val
      }

      constructor new(int year, int month, int day, int hour, int minute, int second)
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
        
      constructor new(int year, int month, int day)
         <= new(year, month, day, 0, 0, 0);
      
      SystemTime cast() = new SystemTime(_value);

      TimeStruct cast()
         = new TimeStruct(_value);
    
      SystemTime SystemTime
      {
         get()
         {
            ^ _value
         }
      }

      bool equal(Date date)
      {
         SystemTime value := date.SystemTime;
            
         ^ value.equal(_value)
      }

      bool less(Date date)
      {
         SystemTime value := date.SystemTime;
            
         ^ _value.less(value)
      }
      
      subtract(Date dt)
      {
         SystemTime val := dt.SystemTime;
    
         ^ new Timespan(_value.subtract(val))
      }
      
      subtract(Timespan timeSpan)
      {
         SystemTime value := timeSpan.SystemTime;
            
         ^ new Date(_value.subtract(value))
      }
      
      add(Timespan timeSpan)
      {
         SystemTime value := timeSpan.SystemTime;
            
         ^ new Date(_value.add(value))
      }
    
      int Year
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Year;
                
            ^ retVal
         }
      }
    
      int Month
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Month;
                
            ^ retVal
         }
      }
    
      int Day
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Day;
                
            ^ retVal
         }
      }
    
      int DayOfWeek
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.DayOfWeek;
                
            ^ retVal
         }
      }
    
      int Hour
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Hour;
                
            ^ retVal
         }
      }
    
      int Minute
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Minute;
                
            ^ retVal
         }
      }
    
      int Second
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Second;
                
            ^ retVal
         }
      }
    
      int Millisecond
      {
         get()
         {
            TimeStruct systemTime := _value;
                
            int retVal := systemTime.Millisecond;
                
            ^ retVal  
         }
      }
    
      addYears(int val)
      {
         TimeStruct st := _value;
    
         int years := st.Year;
         years := years + val;
            
         st.Year := years;
    
         st.readTo(out SystemTime retVal);        
         ^ new Date(retVal)
      }
    
      addMonths(int val)
      {
         TimeStruct st := _value;
            
         int months := val;        
         int years := months / 12;        
         months := months -  months / 12 * 12;
    
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
    
      addDays(int val)
         = self.add(Timespan.new(val, 0, 0, 0));
    
      addHours(int val)
         = self.add(Timespan.new(0, val, 0, 0));
    
      addMinutes(int val)
         = self.add(Timespan.new(0, 0, val, 0));
    
      addSeconds(int val)
         = self.add(Timespan.new(0, 0, 0, val));
    
      string toPrintable()
      {
         TimeStruct struct := _value;
    
         string timeValue := struct.toShortTime();
            
         string dateValue := struct.toShortDate();
    
         ^ dateValue + " " + timeValue
      }
    
      Date clone()
         = new Date(_value);
   }

   // --- obsolete ---    
   public Date now = Date.Now;    
   // --- obsolete ---    
   public Date utcNow = Date.UtcNow;

   // --- now ---    
   public Date Now = Date.Now;
   public Date UtcNow = Date.UtcNow;
}