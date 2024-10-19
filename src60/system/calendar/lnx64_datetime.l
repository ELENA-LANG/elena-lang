namespace calendar
{
   const int TMSize = 56;
   
   // --- SystemTime ---
   public sealed const struct SystemTime : BaseValue
   {
      long tv_sec;
      long tv_usec;

      static SystemTime Default
         = 0l;

      constructor(long nanosec)
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

      constructor(SystemTime value)
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

      bool equal(SystemTime operand)
      {
         operand.read(ref long sec, ref long usec);
            
         if(sec == tv_sec)
            { ^ tv_usec == usec };
    
         ^ false            
      }
    
      bool less(SystemTime operand)
      {
         operand.read(ref long sec, ref long usec);
            
         if(sec == tv_sec)
            { ^ tv_usec < usec };
    
         ^ tv_sec < sec  
      }
    
      long cast() = self.Value;

      get long Value()
      {
         long l := tv_sec;
                
         l := l * 10000000l;

         long l2 := tv_usec;
         l2 := l2 * 10l;
                
         l := l + l2;
                
         ^ l
      }
    
      SystemTime subtract(SystemTime operand)
      {
         operand.read(ref long sec, ref long usec);
            
         long ms := tv_usec;
         if(usec > tv_usec)
            { sec := sec + 1; ms := ms + 1000000; };
    
         ms := ms - usec;
    
         ^ SystemTime.new(tv_sec - sec, ms);
      }
    
      SystemTime add(SystemTime operand)
      {
         operand.read(ref long sec, ref long usec);
            
         long ms := tv_usec + usec;
            
         if(ms > 1000000)
            {  sec := sec + 1; ms := ms - 1000000 };
            
         ^ SystemTime.new(tv_sec + sec, ms)
      }      
   }

   // --- TimeStruct ---
   public sealed struct TimeStruct
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

      constructor(SystemTime time)
      {
         self.write(time)
      }

      constructor()
      {
         tm_isdst := -1
      }

      int Year
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
    
      int Month
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
    
      int DayOfWeek
      {
         get()
         {
            int val := tm_wday;
                
            ^ val
         }
      }
    
      int Day
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
    
      int Hour
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
    
      int Minute
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
    
      int Second
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
    
      int Millisecond
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

      readTo(ref SystemTime retVal)
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

      string toShortDate()
      {
         int year := self.Year;
         int month := self.Month;
            
         ^ tm_mday.toPrintable().padLeft($48, 2) 
               + "." + month.toPrintable().padLeft($48, 2)
               + "." + year.toPrintable().padLeft($48, 2)
      }
    
      string toShortTime()
      {
         ^ tm_hour.toPrintable().padLeft($48, 2)
               + ":" + tm_min.toPrintable().padLeft($48, 2)
               + ":" + tm_sec.toPrintable().padLeft($48, 2)
      }
   }

   public singleton systemTimeFactory
   {
      SystemTime getUtcNow()
      {
         SystemTime time := 0l;        
         extern libc.gettimeofday(time, 0);
            
         TimeStruct tm;
         tm.writeUTC(time);
            
         tm.readTo(ref time);
            
         ^ time
      }
        
      SystemTime getNow()
      {
         SystemTime time := 0l;
         extern libc.gettimeofday(time, 0);
            
         ^ time
      }
   }
   
}