namespace calendar
{
   const IntNumber DATE_SHORTDATE  = 00000001h;
   const IntNumber DATE_LONGDATE   = 00000002h;
    
   // --- SystemTime ---
   public sealed const struct SystemTime : BaseValue
   {
      embeddable __word _value[8];

      private assign(long value) : external(system'core_routines'__st_assign);

      static SystemTime Default
         = 0l;

      constructor(long value)
      {
         self.assign(value);
      }

      constructor(SystemTime value)
         = value;

      internal constructor loadAsTimespan(int days, int hours, int minutes, int seconds)
      {
         long value := 0l;        
         long n := hours;
            
         n := n * 36000000000l;
         value := n;
            
         n := minutes;
         n := n * 600000000l;            
         value := value + n;
            
         n := seconds;
         n := n * 10000000l;
         value := value + n;
    
         n := days;
         n := n * 864000000000l;
         value := value + n;
            
         self.assign(value);         
      }

      get long Value()
      {
         ^ _value
      }

      bool equal(SystemTime operand)
         = _value == operand;
    
      bool less(SystemTime operand)
      {
         long n1 := self.Value;
         long n2 := operand.Value;

         ^ n1 < n2;
      }
    
      SystemTime subtract(SystemTime operand)
      {
         long n1 := self.Value;
         long n2 := operand.Value;

         n1 -= n2;

         ^ new SystemTime(n1)
      }
    
      SystemTime add(SystemTime operand)
      {
         long n1 := self.Value;
         long n2 := operand.Value;

         n1 += n2;

         ^ new SystemTime(n1)
      }

      
   }

   // --- TimeStruct ---
   public sealed struct TimeStruct
   {
      short _year;
      short _month;
      short _dayOfWeek;
      short _day;
      short _hour;
      short _minute;
      short _second;
      short _millisecond;

      private init(SystemTime fileTime)
      {
         extern KERNEL32.FileTimeToSystemTime(ref fileTime, self)
      }

      constructor(SystemTime time)
      {
         self.init(time)
      }

      constructor()
      {
         _year := 0;
         _month := 0;
         _dayOfWeek := 0;
         _day := 0;
         _hour := 0;
         _minute := 0;
         _second := 0;
         _millisecond := 0;
      }

      int Year
      {
         get()
         {
            int val := _year;
                
            ^ val
         }
        
         set(value)
         {
            short n := value;
        
            _year := n
         }
      }
    
      int Month
      {
         get()
         {
            int val := _month;
                
            ^ val
         }
            
         set(value)
         {
            short n := value;
                    
            _month := n
         }
      }
    
      int DayOfWeek
      {
         get()
         {
            int val := _dayOfWeek;
                
            ^ val
         }
      }
    
      int Day
      {
         get()
         {
            int val := _day;
                
            ^ val
         }
        
         set(value)
         {
            short n := value;
                    
            _day := n
         }
      }
    
      int Hour
      {
         get()
         {
            int val := _hour;
                
            ^ val
         }
         set(value)
         { 
            short n := value;
                    
            _hour := n
         }
      }
    
      int Minute
      {
         get()
         {
            int val := _minute;
                
            ^ val
         }
        
         set(value)
         {
            short n := value;
                    
            _minute := n
         }
      }
    
      int Second
      {
         get()
         {
            int val := _second;
                
            ^ val
         }
        
         set(value)
         { 
            short n := value;
                    
            _second := n
         }
      }
    
      int Millisecond
      {
         get()
         {
            int val := _millisecond;
                
            ^ val
         }
        
         set(value)
         {
            short n := value;
                    
            _millisecond := n
         }
      }

      readTo(ref SystemTime retVal)
      {
         SystemTime value := 0l;
         extern KERNEL32.SystemTimeToFileTime(self, ref value);
            
         retVal := value
      }
          
      private string dateToString(int flags)
      {
         byte buffer[127];
    
         int length := extern KERNEL32.GetDateFormatA(
            0, flags,self,0,buffer,255);
                
         if (length == 0)
            { FormatException.new:"Invalid date".raise() };
    
         ^ String.fromByteArray(0, length - 1, buffer)
      }
    
      private string timeToString(int flags)
      {
         byte buffer[127];
    
         int length := extern KERNEL32.GetTimeFormatA(
            0,flags,self,0,buffer,255);
                
         if (length == 0)
            { FormatException.new:"Invalid time".raise() };
    
         ^ String.fromByteArray(0, length - 1, buffer)
      }
          
      private wide dateToWide(int flags)
      {
         short buffer[127];
    
         int length := extern KERNEL32.GetDateFormatW(
            0, flags,self,0,buffer,255);
                
         if (length == 0)
            { FormatException.new:"Invalid date".raise() };
    
         ^ WideString.fromShortArray(0, length - 1, buffer)
      }
    
      private wide timeToWide(int flags)
      {
         short buffer[127];
    
         int length := extern KERNEL32.GetTimeFormatW(
            0,flags,self,0,buffer,255);
                
         if (length == 0)
            { FormatException.new:"Invalid time".raise() };
    
         ^ WideString.fromShortArray(0, length - 1, buffer)
      }

      wide toShortDate()
         = self.dateToWide(DATE_SHORTDATE);

      wide toShortTime()
         = self.timeToWide(0);

      retoverload string toShortDate()
         = self.dateToString(DATE_SHORTDATE);

      retoverload string toShortTime()
         = self.timeToString(0);

   }

   public singleton systemTimeFactory
   {
      SystemTime getUtcNow()
      {
         SystemTime time := 0l;        
         extern KERNEL32.GetSystemTimeAsFileTime(ref time);
            
         ^ time
      }
        
      SystemTime getNow()
      {
         SystemTime time := 0l;
         SystemTime localTime := 0l;
            
         extern KERNEL32.GetSystemTimeAsFileTime(ref time);
    
         extern KERNEL32.FileTimeToLocalFileTime(ref time, ref localTime);
            
         ^ localTime
      }
   }
   
}