namespace calendar
{
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
         extern KERNEL32.FileTimeToSystemTime(fileTime, self)
      }

      embeddable constructor(SystemTime time)
      {
         self.init(time)
      }

      embeddable constructor()
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
   }
}