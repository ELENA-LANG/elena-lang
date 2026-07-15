namespace calendar
{

#if (__project["_Win"]) // Windows

   const IntNumber DATE_SHORTDATE  = 00000001h;
   const IntNumber DATE_LONGDATE   = 00000002h;
    
   // --- SystemTime ---
   public sealed const struct SystemTime : BaseValue, info("An system time")
   {
      embeddable __word _value[8];

      private assign(long value) : external(system'core_routines'__st_assign);

      static SystemTime Default : info("Returns a default system time")
         = 0l;

      constructor(long value) : info("Initializes a system time with the number of 100-nanosecond intervals since January 1, 1601 (UTC)")
      {
         self.assign(value);
      }

      constructor(SystemTime value) : info("Creates a copy of the system time")
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

      get long Value() : info("Returns the system time as a number of 100-nanosecond intervals since January 1, 1601")
      {
         ^ _value
      }

      bool equal(SystemTime operand) : info("Returns true if the system time equals to <b>operand</b>")
         = _value == operand;
    
      bool less(SystemTime operand) : info("Returns true if the system time less than <b>operand</b>")
      {
         long n1 := self.Value;
         long n2 := operand.Value;

         ^ n1 < n2;
      }
    
      SystemTime subtract(SystemTime operand) : info("Returns a difference between the system time and <b>operand</b>")
      {
         long n1 := self.Value;
         long n2 := operand.Value;

         n1 -= n2;

         ^ new SystemTime(n1)
      }
    
      SystemTime add(SystemTime operand) : info("Returns a sum of the system time and <b>operand</b>")
      {
         long n1 := self.Value;
         long n2 := operand.Value;

         n1 += n2;

         ^ new SystemTime(n1)
      }      
   }

   // --- TimeStruct ---
   public sealed struct TimeStruct : info("A date / time structure")
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

      constructor(SystemTime time) : info("Initializes the date/time structure by a system time")
      {
         self.init(time)
      }

      constructor() : info("Creates an empty structure")
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

      int Year : info("A year component of the date/time structure")
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
    
      int Month : info("A month component of the date/time structure")
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
    
      int DayOfWeek : info("A day of week component of the date/time structure")
      {
         get()
         {
            int val := _dayOfWeek;
                
            ^ val
         }
      }
    
      int Day : info("A day component of the date/time structure")
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
    
      int Hour : info("An hour component of the date/time structure")
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
    
      int Minute : info("An minute component of the date/time structure")
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
    
      int Second : info("A second component of the date/time structure")
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
    
      int Millisecond : info("A millisecond component of the date/time structure")
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

      readTo(out SystemTime retVal) : info("Converts the date / time structure into a system time")
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
            { FormatException.new("Invalid date").raise() };
    
         ^ String.fromByteArray(0, length - 1, buffer)
      }
    
      private string timeToString(int flags)
      {
         byte buffer[127];
    
         int length := extern KERNEL32.GetTimeFormatA(
            0,flags,self,0,buffer,255);
                
         if (length == 0)
            { FormatException.new("Invalid time").raise() };
    
         ^ String.fromByteArray(0, length - 1, buffer)
      }
          
      private wide dateToWide(int flags)
      {
         short buffer[127];
    
         int length := extern KERNEL32.GetDateFormatW(
            0, flags,self,0,buffer,255);
                
         if (length == 0)
            { FormatException.new("Invalid date").raise() };
    
         ^ WideString.fromShortArray(0, length - 1, buffer)
      }
    
      private wide timeToWide(int flags)
      {
         short buffer[127];
    
         int length := extern KERNEL32.GetTimeFormatW(
            0,flags,self,0,buffer,255);
                
         if (length == 0)
            { FormatException.new("Invalid time").raise() };
    
         ^ WideString.fromShortArray(0, length - 1, buffer)
      }

      wide toShortDate() : info("Returns a short date representation of the date / time structure")
         = self.dateToWide(DATE_SHORTDATE);

      wide toShortTime() : info("Returns a short time representation of the date / time structure")
         = self.timeToWide(0);

      retoverload string toShortDate() : info("Returns a short date representation of the date / time structure")
         = self.dateToString(DATE_SHORTDATE);

      retoverload string toShortTime() : info("Returns a short time representation of the date / time structure")
         = self.timeToString(0);

   }

#elif (__project["_Linux32"])

   const int TMSize = 44;
   
   // --- SystemTime ---
   public sealed const struct SystemTime : BaseValue, info("A system time")
   {
      int tv_sec;
      int tv_usec;

      static SystemTime Default : info("Returns a default system time")
         = 0l;

      constructor(long nanosec) : info("Initializes a system time with the number of nanosecond intervals since January 1, 1970 (UTC)")
      {
         long secs := nanosec / 1000000000l;
            
         long rem := nanosec - secs * 1000000000l;
         rem := rem / 1000l;
                            
         int tmp := IntConvertor.convert(secs);
         tv_sec := tmp;
            
         tmp := IntConvertor.convert(rem);
         tv_usec := tmp;
      }

      internal constructor loadAsTimespan(int days, int hours, int minutes, int seconds)
      {
         int secs := 0;
         int n := hours;
            
         n := n * 3600;
         secs := n;
            
         n := minutes;
         n := n * 60;            
         secs := secs + n;
            
         secs := secs + seconds;
    
         n := days;
         n := n * 86400;
         secs := secs + n;
                            
         tv_sec := secs;            
         tv_usec := 0;
      }

      constructor(SystemTime value) : info("Creates a copy of the system time")
         = value;

      internal constructor new(int seconds, int microSeconds)
      {
         tv_sec := seconds;
         tv_usec := microSeconds
      }

      internal read(out int seconds, out int microSeconds)
      {
         seconds := tv_sec;
         microSeconds := tv_usec
      }

      internal readMSec(out int microSeconds)
      {
         microSeconds := tv_usec;
      }

      bool equal(SystemTime operand) : info("Returns true if the system time equals to <b>operand</b>")
      {
         operand.read(out int sec, out int usec);
            
         if(sec == tv_sec)
            { ^ tv_usec == usec };
    
         ^ false            
      }
    
      bool less(SystemTime operand) : info("Returns true if the system time less than <b>operand</b>")
      {
         operand.read(out int sec, out int usec);
            
         if(sec == tv_sec)
            { ^ tv_usec < usec };
    
         ^ tv_sec < sec  
      }
    
      long cast() = self.Value;

      get long Value() : info("Returns the system time as a number of nanosecond intervals since January 1, 1970")
      {
         long l := LongConvertor.convert(tv_sec);
                
         l := l * 10000000l;

         long l2 := LongConvertor.convert(tv_usec);
         l2 := l2 * 10l;
                
         l := l + l2;
                
         ^ l
      }
    
      SystemTime subtract(SystemTime operand) : info("Returns a difference between the system time and <b>operand</b>")
      {
         operand.read(out int sec, out int usec);
            
         int ms := tv_usec;
         if(usec > tv_usec)
            { sec := sec + 1; ms := ms + 1000000; };
    
         ms := ms - usec;
    
         ^ SystemTime.new(tv_sec - sec, ms);
      }
    
      SystemTime add(SystemTime operand) : info("Returns a sum of the system time and <b>operand</b>")
      {
         operand.read(out int sec, out int usec);
            
         int ms := tv_usec + usec;
            
         if(ms > 1000000)
            {  sec := sec + 1; ms := ms - 1000000 };
            
         ^ SystemTime.new(tv_sec + sec, ms)
      }      
   }

   // --- TimeStruct ---
   public sealed struct TimeStruct : info("A date / time structure")
   {
      int tm_sec;         /* seconds */
      int tm_min;         /* minutes */
      int tm_hour;        /* hours */
      int tm_mday;        /* day of the month */
      int tm_mon;         /* month */
      int tm_year;        /* year */
      int tm_wday;        /* day of the week */
      int tm_yday;        /* day in the year */
      int tm_isdst;       /* daylight saving time */
    
      int tmp1;
      int tmp2;
        
      int tm_usec;        

      constructor(SystemTime time) : info("Initializes the date/time structure by a system time")
      {
         self.write(time)
      }

      constructor() : info("Creates an empty structure")
      {
         tm_isdst := -1
      }

      int Year : info("A year component of the date/time structure")
      {
         get()
         {
            int val := tm_year;
            val += 1900;
                
            ^ val
         }
        
         set(value)
         {
            int n := value - 1900;
        
            tm_year := n
         }
      }
    
      int Month : info("A month component of the date/time structure")
      {
         get()
         {
            int val := tm_mon;
            val += 1;
                
            ^ val
         }
            
         set(value)
         {
            int n := value - 1;
                    
            tm_mon := n
         }
      }
    
      int DayOfWeek : info("A day of week component of the date/time structure")
      {
         get()
         {
            int val := tm_wday;
                
            ^ val
         }
      }
    
      int Day : info("A day component of the date/time structure")
      {
         get()
         {
            int val := tm_mday;
                
            ^ val
         }
        
         set(value)
         {
            int n := value;
                    
            tm_mday := n
         }
      }
    
      int Hour : info("An hour component of the date/time structure")
      {
         get()
         {
            int val := tm_hour;
                
            ^ val
         }
         set(value)
         { 
            int n := value;
                    
            tm_hour := n
         }
      }
    
      int Minute : info("An minute component of the date/time structure")
      {
         get()
         {
            int val := tm_min;
                
            ^ val
         }
        
         set(value)
         {
            int n := value;
                    
            tm_min := n
         }
      }
    
      int Second : info("A second component of the date/time structure")
      {
         get()
         {
            int val := tm_sec;
                
            ^ val
         }
        
         set(value)
         { 
            int n := value;
                    
            tm_sec := n
         }
      }
    
      int Millisecond : info("A millisecond component of the date/time structure")
      {
         get()
         {
            int val := tm_usec;
            val /= 1000;
                
            ^ val
         }
        
         set(value)
         {
            int n := value * 1000;
                    
            tm_usec := n
         }
      }

      readTo(out SystemTime retVal) : info("Converts the date / time structure into a system time")
      {
         // HOTFIX
         tm_isdst := -1;
            
         int tm_sec := extern libc.mktime(self);
         SystemTime tm := SystemTime.new(tm_sec, tm_usec);
            
         retVal := tm 
      }
          
      private write(SystemTime time)
      {
         pointer ptr := extern libc.localtime(time);
            
         extern libc.memcpy(self, ptr, TMSize);
            
         time.readMSec(ref int tmp);
         tm_usec := tmp
      }

      internal writeUTC(SystemTime time)
      {
         pointer ptr := extern libc.gmtime(time);
            
         extern libc.memcpy(self, ptr, TMSize);
            
         time.readMSec(ref int tmp);
         tm_usec := tmp
      }

      string toShortDate() : info("Returns a short date representation of the date / time structure")
      {
         int year := self.Year;
         int month := self.Month;
            
         ^ tm_mday.toPrintable().padLeft($48, 2) 
               + "." + month.toPrintable().padLeft($48, 2)
               + "." + year.toPrintable().padLeft($48, 2)
      }
    
      string toShortTime() : info("Returns a short time representation of the date / time structure")
      {
         ^ tm_hour.toPrintable().padLeft($48, 2)
               + ":" + tm_min.toPrintable().padLeft($48, 2)
               + ":" + tm_sec.toPrintable().padLeft($48, 2)
      }
   }

#elif (__project["_Linux64"] || __project["_FreeBSD64"] || __project["_LinuxPPC64le"] || __project["_LinuxARM64"] || __project["_MacOSARM64"])

   const int TMSize = 56;
   
   // --- SystemTime ---
   public sealed const struct SystemTime : BaseValue, info("A system time")
   {
      long tv_sec;
      long tv_usec;

      static SystemTime Default : info("Returns a default system time")
         = 0l;

      constructor(long nanosec) : info("Initializes a system time with the number of nanosecond intervals since January 1, 1970 (UTC)")
      {
         long secs := nanosec / 1000000000l;
            
         long rem := nanosec - secs * 1000000000l;
         rem := rem / 100l;
                            
         tv_sec := secs;
         tv_usec := rem;
      }

      internal constructor loadAsTimespan(int days, int hours, int minutes, int seconds)
      {
         int secs := 0;
         int n := hours;
            
         n := n * 3600;
         secs := n;
            
         n := minutes;
         n := n * 60;            
         secs := secs + n;
            
         secs := secs + seconds;
    
         n := days;
         n := n * 86400;
         secs := secs + n;
                            
         tv_sec := secs;            
         tv_usec := 0;
      }

      constructor(SystemTime value) : info("Creates a copy of the system time")
         = value;

      internal constructor new(long seconds, long microSeconds)
      {
         tv_sec := seconds;
         tv_usec := microSeconds
      }

      internal read(ref long seconds, ref long microSeconds)
      {
         seconds := tv_sec;
         microSeconds := tv_usec
      }

      internal readMSec(ref long microSeconds)
      {
         microSeconds := tv_usec;
      }

      bool equal(SystemTime operand) : info("Returns true if the system time equals to <b>operand</b>")
      {
         operand.read(ref long sec, ref long usec);
            
         if(sec == tv_sec)
            { ^ tv_usec == usec };
    
         ^ false            
      }
    
      bool less(SystemTime operand) : info("Returns true if the system time less than <b>operand</b>")
      {
         operand.read(ref long sec, ref long usec);
            
         if(sec == tv_sec)
            { ^ tv_usec < usec };
    
         ^ tv_sec < sec  
      }
    
      long cast() = self.Value;

      get long Value() : info("Returns the system time as a number of nanosecond intervals since January 1, 1970")
      {
         long l := tv_sec;
                
         l := l * 10000000l;

         long l2 := tv_usec;
         l2 := l2 * 10l;
                
         l := l + l2;
                
         ^ l
      }
    
      SystemTime subtract(SystemTime operand) : info("Returns a difference between the system time and <b>operand</b>")
      {
         operand.read(ref long sec, ref long usec);
            
         long ms := tv_usec;
         if(usec > tv_usec)
            { sec := sec + 1; ms := ms + 1000000; };
    
         ms := ms - usec;
    
         ^ SystemTime.new(tv_sec - sec, ms);
      }
    
      SystemTime add(SystemTime operand) : info("Returns a sum of the system time and <b>operand</b>")
      {
         operand.read(ref long sec, ref long usec);
            
         long ms := tv_usec + usec;
            
         if(ms > 1000000)
            {  sec := sec + 1; ms := ms - 1000000 };
            
         ^ SystemTime.new(tv_sec + sec, ms)
      }      
   }


   // --- TimeStruct ---
   public sealed struct TimeStruct : info("A date / time structure")
   {
      int tm_sec;         /* seconds */
      int tm_min;         /* minutes */
      int tm_hour;        /* hours */
      int tm_mday;        /* day of the month */
      int tm_mon;         /* month */
      int tm_year;        /* year */
      int tm_wday;        /* day of the week */
      int tm_yday;        /* day in the year */
      int tm_isdst;       /* daylight saving time */
    
      long tmp1;
      long tmp2;
        
      long tm_usec;        

      constructor(SystemTime time) : info("Initializes the date/time structure by a system time")
      {
         self.write(time)
      }

      constructor() : info("Creates an empty structure")
      {
         tm_isdst := -1
      }

      int Year : info("A year component of the date/time structure")
      {
         get()
         {
            int val := tm_year;
            val += 1900;
                
            ^ val
         }
        
         set(value)
         {
            int n := value - 1900;
        
            tm_year := n
         }
      }
    
      int Month : info("A month component of the date/time structure")
      {
         get()
         {
            int val := tm_mon;
            val += 1;
                
            ^ val
         }
            
         set(value)
         {
            int n := value - 1;
                    
            tm_mon := n
         }
      }
    
      int DayOfWeek : info("A day of week component of the date/time structure")
      {
         get()
         {
            int val := tm_wday;
                
            ^ val
         }
      }
    
      int Day : info("A day component of the date/time structure")
      {
         get()
         {
            int val := tm_mday;
                
            ^ val
         }
        
         set(value)
         {
            int n := value;
                    
            tm_mday := n
         }
      }
    
      int Hour : info("An hour component of the date/time structure")
      {
         get()
         {
            int val := tm_hour;
                
            ^ val
         }
         set(value)
         { 
            int n := value;
                    
            tm_hour := n
         }
      }
    
      int Minute : info("An minute component of the date/time structure")
      {
         get()
         {
            int val := tm_min;
                
            ^ val
         }
        
         set(value)
         {
            int n := value;
                    
            tm_min := n
         }
      }
    
      int Second : info("A second component of the date/time structure")
      {
         get()
         {
            int val := tm_sec;
                
            ^ val
         }
        
         set(value)
         { 
            int n := value;
                    
            tm_sec := n
         }
      }
    
      int Millisecond : info("A millisecond component of the date/time structure")
      {
         get()
         {
            long val := tm_usec;
            val /= 1000;
                
            int nval := IntConvertor.convert(val);

            ^ nval
         }
        
         set(value)
         {
            long n := value;
            n := n * 1000;
                    
            tm_usec := n
         }
      }

      readTo(ref SystemTime retVal) : info("Converts the date / time structure into a system time")
      {
         // HOTFIX
         tm_isdst := -1;
            
         long tm_sec := extern libc.mktime(self);
         SystemTime tm := SystemTime.new(tm_sec, tm_usec);
            
         retVal := tm 
      }
          
      private write(SystemTime time)
      {
         pointer ptr := extern libc.localtime(time);
            
         extern libc.memcpy(self, ptr, TMSize);
            
         time.readMSec(ref long tmp);
         tm_usec := tmp
      }

      internal writeUTC(SystemTime time)
      {
         pointer ptr := extern libc.gmtime(time);
            
         extern libc.memcpy(self, ptr, TMSize);
            
         time.readMSec(ref long tmp);
         tm_usec := tmp
      }

      string toShortDate() : info("Returns a short date representation of the date / time structure")
      {
         int year := self.Year;
         int month := self.Month;
            
         ^ tm_mday.toPrintable().padLeft($48, 2) 
               + "." + month.toPrintable().padLeft($48, 2)
               + "." + year.toPrintable().padLeft($48, 2)
      }
    
      string toShortTime() : info("Returns a short time representation of the date / time structure")
      {
         ^ tm_hour.toPrintable().padLeft($48, 2)
               + ":" + tm_min.toPrintable().padLeft($48, 2)
               + ":" + tm_sec.toPrintable().padLeft($48, 2)
      }
   }

#endif

#if (__project["_Win"]) // Windows

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

#elif (__project["_Linux"] || __project["_FreeBSD"] || __project["_MacOS"])

   public singleton systemTimeFactory : info("A current device system time helper")
   {
      SystemTime getUtcNow() : info("Returns the current system time")
      {
         SystemTime time := 0l;        
         extern libc.gettimeofday(time, 0);
            
         TimeStruct tm;
         tm.writeUTC(time);
            
         tm.readTo(ref time);
            
         ^ time
      }
        
      SystemTime getNow() : info("Returns the current local time")
      {
         SystemTime time := 0l;
         extern libc.gettimeofday(time, 0);
            
         ^ time
      }
   }

#endif

}
