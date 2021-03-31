namespace calendar
{

    const int DATE_SHORTDATE  = 00000001h;
    const int DATE_LONGDATE   = 00000002h;
    
    // --- SystemTime ---
    
    public sealed const struct SystemTime : BaseValue
    {
        embeddable __int theValue[8];
    
        static SystemTime Default
            = 0l;
    
        constructor(long value)
        {
            theValue := value
        }
             
        constructor(SystemTime value)
            = value;
    
        bool equal(SystemTime operand)
            = theValue == operand;
    
        bool less(SystemTime operand)
            = theValue < operand;
    
        long cast() = theValue;
    
        long Value
        {
            get()
            {
                ^ theValue
            }        
        }
    
        SystemTime subtract(SystemTime operand)
        {
            SystemTime result := theValue - operand;
            
            ^ new SystemTime(result)
        }
    
        SystemTime add(SystemTime operand)
        {
            SystemTime result := theValue + operand;
            
            ^ new SystemTime(result)
        }
    }
    
    // --- TimeStruct ---
    
    public sealed struct TimeStruct
    {
        short theYear;
        short theMonth;
        short theDayOfWeek;
        short theDay;
        short theHour;
        short theMinute;
        short theSecond;
        short theMillisecond;
    
        embeddable constructor(SystemTime time)
        {
            self.init(time)
        }
    
        embeddable constructor()
        {
            intern system'core_routines'__timest.empty(self);
        }
    
        int Year
        {
            get()
            {
                int val := theYear;
                
                ^ val
            }
        
            set(value)
            {
                short n := value;
        
                theYear := n
            }
        }
    
        int Month
        {
            get()
            {
                int val := theMonth;
                
                ^ val
            }
            
            set(value)
            {
                short n := value;
                    
                theMonth := n
            }
        }
    
        int DayOfWeek
        {
            get()
            {
                int val := theDayOfWeek;
                
                ^ val
            }
        }
    
        int Day
        {
            get()
            {
                int val := theDay;
                
                ^ val
            }
        
            set(value)
            {
                short n := value;
                    
                theDay := n
            }
        }
    
        int Hour
        {
            get()
            {
                int val := theHour;
                
                ^ val
            }
            set(value)
            { 
                short n := value;
                    
                theHour := n
            }
        }
    
        int Minute
        {
            get()
            {
                int val := theMinute;
                
                ^ val
            }
        
            set(value)
            {
                short n := value;
                    
                theMinute := n
            }
        }
    
        int Second
        {
            get()
            {
                int val := theSecond;
                
                ^ val
            }
        
            set(value)
            { 
                short n := value;
                    
                theSecond := n
            }
        }
    
        int Millisecond
        {
            get()
            {
                int val := theMillisecond;
                
                ^ val
            }
        
            set(value)
            {
                short n := value;
                    
                theMillisecond := n
            }
        }
    
        readTo(ref SystemTime retVal)
        {
            SystemTime value := 0l;
            extern KERNEL32.SystemTimeToFileTime(self, value);
            
            retVal := value
        }
        
        private init(SystemTime fileTime)
        {
            extern KERNEL32.FileTimeToSystemTime(fileTime, self)
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
    }
    
    public singleton systemTimeFactory
    {
        readUtcNow(ref SystemTime retVal)
        {
            SystemTime time := 0l;        
            extern KERNEL32.GetSystemTimeAsFileTime(time);
            
            retVal := time
        }
        
        readNow(ref SystemTime retVal)
        {
            SystemTime time := 0l;
            SystemTime localTime := 0l;
            
            extern KERNEL32.GetSystemTimeAsFileTime(time);
    
            extern KERNEL32.FileTimeToLocalFileTime(time, localTime);
            
            retVal := localTime
        }
    }

}