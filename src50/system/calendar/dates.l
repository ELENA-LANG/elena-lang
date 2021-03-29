namespace calendar
{

    // --- TimeSpan --
    
    public sealed struct Timespan : BaseValue
    {
        SystemTime theValue;
    
        static SystemTime Default
            = new Timespan();
    
        constructor()
        {
            theValue := 0l
        }
    
        static Timespan MinValue = new SystemTime(0l);
    
        constructor(SystemTime val)
        {
            theValue := val
        }
    
        constructor new(int hours, int minutes, int seconds)
        {
            long value := 0l;        
            long n := cast long(hours);
            
            n := n * 36000000000l;
            value := n;
            
            n := cast long(minutes);
            n := n * 600000000l;            
            value := value + n;
            
            n := cast long(seconds);
            n := n * 10000000l;
            value := value + n;
            
            theValue := value
        }
    
        constructor new(int days, int hours, int minutes, int seconds)
        {
            long value := 0l;
            long n := cast long(hours);
            
            n := n * 36000000000l;
            value := n;
            
            n := cast long(minutes);
            n := n * 600000000l;            
            value := value + n;
            
            n := cast long(seconds);
            n := n * 10000000l;
            value := value + n;
    
            n := cast long(days);
            n := n * 864000000000l;
            value := value + n;
    
            theValue := value
        }
    
        SystemTime SystemTime
        {
            get()
            {
                ^ theValue
            }
        }
    
        get int Days()
        {
            long retVal := theValue.Value;
            
            retVal := retVal / 864000000000l;
            
            ^ intConvertor.convert(retVal)
        }
    
        get int Hours()
        {
            long retVal := theValue.Value;
            
            retVal := retVal / 36000000000l;
            
            ^ intConvertor.convert(retVal)
        }
                            
        get int Minutes()
        {
            long retVal := theValue.Value;
            
            retVal := retVal / 600000000l;
            
            ^ intConvertor.convert(retVal)
        }
    
        get int Seconds()
        {
            long retVal := theValue.Value;
            
            retVal := retVal / 10000000l;
            
            ^ intConvertor.convert(retVal)
        }
    
        get long Milliseconds()
        {
            long retVal := theValue.Value;
            
            retVal := retVal / 10000l;
            
            ^ new LongNumber(retVal)
        }
    
        bool equal(Timespan span)
        {
            SystemTime value := span.SystemTime;
    
            ^ value.equal(theValue)
        }
    
        bool less(Timespan span)
        {
            SystemTime value := span.SystemTime;
    
            ^ theValue.less(value)
        }
    
        add(Date date)
            = date.add(self);
        
        add(Timespan span)
        {
            SystemTime value := span.SystemTime;
    
            SystemTime retVal := theValue + value;
    
            ^ new Timespan(retVal)
        }
    
        subtract(Timespan span)
        {
            SystemTime value := span.SystemTime;
    
            SystemTime retVal := theValue - value;
            
            ^ new Timespan(retVal)
        }
    
        string toPrintable()
        {
            TimeStruct value := theValue;
    
            string s := value.toShortTime();
    
            ^ self.Days.toPrintable() + "." + s
        }
    
        Timespan clone()
            = new Timespan(theValue);
    }
    
    // --- Date ---
    
    public sealed struct Date : BaseValue
    {
        SystemTime theValue;
    
        constructor()
        {
            theValue := 0l
        }
        
        static Date Default = new Date();
    
        static Date MinValue = new Date();
    
        get constructor Now()
        {
           systemTimeFactory.readNow(ref SystemTime v); 
           theValue := v
        }
    
        get constructor UtcNow()
        {
           systemTimeFactory.readUtcNow(ref SystemTime v); 
           theValue := v
        }
                
        constructor(SystemTime val)
        {
            theValue := val
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
    
            systemTime.readTo(ref SystemTime value);
            theValue := value
        }
        
        constructor new(int year, int month, int day)
            <= new(year, month, day, 0, 0, 0);
      
        SystemTime cast() = new SystemTime(theValue);
    
        TimeStruct cast()
            = new TimeStruct(theValue);
    
        SystemTime SystemTime
        {
            get()
            {
                ^ theValue
            }
        }
    
        bool equal(Date date)
        {
            SystemTime value := date.SystemTime;
            
            ^ value.equal(theValue)
        }
    
        bool less(Date date)
        {
            SystemTime value := date.SystemTime;
            
            ^ theValue.less(value)
        }
      
        subtract(Date dt)
        {
            SystemTime val := dt.SystemTime;
    
            ^ new Timespan(theValue.subtract(val))
         }
      
        subtract(Timespan timeSpan)
        {
            SystemTime value := timeSpan.SystemTime;
            
            ^ new Date(theValue.subtract(value))
        }
      
        add(Timespan timeSpan)
        {
            SystemTime value := timeSpan.SystemTime;
            
            ^ new Date(theValue.add(value))
        }
    
        int Year
        {
            get()
            {
                TimeStruct systemTime := theValue;
                
                int retVal := systemTime.Year;
                
                ^ retVal
            }
        }
    
        int Month
        {
            get()
            {
                TimeStruct systemTime := theValue;
                
                int retVal := systemTime.Month;
                
                ^ retVal
            }
        }
    
        int Day
        {
            get()
            {
                TimeStruct systemTime := theValue;
                
                int retVal := systemTime.Day;
                
                ^ retVal
            }
        }
    
        int DayOfWeek
        {
            get()
            {
                TimeStruct systemTime := theValue;
                
                int retVal := systemTime.DayOfWeek;
                
                ^ retVal
            }
        }
    
        int Hour
        {
            get()
            {
                TimeStruct systemTime := theValue;
                
                int retVal := systemTime.Hour;
                
                ^ retVal
            }
        }
    
        int Minute
        {
            get()
            {
                TimeStruct systemTime := theValue;
                
                int retVal := systemTime.Minute;
                
                ^ retVal
            }
        }
    
        int Second
        {
            get()
            {
                TimeStruct systemTime := theValue;
                
                int retVal := systemTime.Second;
                
                ^ retVal
            }
        }
    
        int Millisecond
        {
            get()
            {
                TimeStruct systemTime := theValue;
                
                int retVal := systemTime.Millisecond;
                
                ^ retVal  
            }
        }
    
        addYears(int val)
        {
            TimeStruct st := theValue;
    
            int years := st.Year;
            years := years + val;
            
            st.Year := years;
    
            st.readTo(ref SystemTime retVal);        
            ^ new Date(retVal)
        }
    
        addMonths(int val)
        {
            TimeStruct st := theValue;
            
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
            
            st.readTo(ref SystemTime retVal);        
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
            TimeStruct struct := theValue;
    
            string timeValue := struct.toShortTime();
            
            string dateValue := struct.toShortDate();
    
            ^ dateValue + " " + timeValue
        }
    
        Date clone()
            = new Date(theValue);
    }
    
    // --- now ---
    
    public Date now = Date.Now;
    
    public Date utcNow = Date.UtcNow;

}