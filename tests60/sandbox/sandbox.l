import extensions;

   public sealed struct TimeStruct
   {
      short _year;
      short _month;

      int Month
      {
         get()
         {
            int val := _month;
                
            ^ val
         }
            
//         set(value)
//         {
//            short n := value;
//                    
//            _month := n
//         }
      }
}

public program()
{
   
}
