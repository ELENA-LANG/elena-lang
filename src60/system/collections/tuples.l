namespace collections
{
   public sealed Tuple<T1>
   {
      constructor()
      {
      }    
        
      constructor(T1 o1)
      {
         Item1 := o1
      }
        
      T1 Item1 : prop;
   }

   public sealed Tuple<T1, T2>
   {
      constructor()
      {
      }    
        
      constructor(T1 o1, T2 o2)
      {
         Item1 := o1;
         Item2 := o2
      }
        
      constructor(T1 o1)
      {
         Item1 := o1
      }
        
      T1 Item1 : prop;
      T2 Item2 : prop;
   }

   public sealed Tuple<T1, T2, T3>
   {
      constructor()
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3)
      {
         Item1 := o1;
         Item2 := o2;
         Item3 := o3
      }
        
      T1 Item1 : prop;
      T2 Item2 : prop;
      T3 Item3 : prop;
   }

   public sealed Tuple<T1, T2, T3, T4>
   {
      constructor()
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3, T4 o4)
      {
         Item1 := o1;
         Item2 := o2;
         Item3 := o3;
         Item4 := o4
      }
        
      T1 Item1 : prop;
      T2 Item2 : prop;
      T3 Item3 : prop;
      T4 Item4 : prop;
   }

   public sealed Tuple<T1, T2, T3, T4, T5>
   {
      constructor()
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3, T4 o4, T5 o5)
      {
         Item1 := o1;
         Item2 := o2;
         Item3 := o3;
         Item4 := o4;
         Item5 := o5;
      }
        
      T1 Item1 : prop;
      T2 Item2 : prop;
      T3 Item3 : prop;
      T4 Item4 : prop;
      T5 Item5 : prop;
   }

   public sealed Tuple<T1, T2, T3, T4, T5, T6>
   {
      constructor()
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3, T4 o4, T5 o5, T6 o6)
      {
         Item1 := o1;
         Item2 := o2;
         Item3 := o3;
         Item4 := o4;
         Item5 := o5;
         Item6 := o6;
      }
        
      T1 Item1 : prop;
      T2 Item2 : prop;
      T3 Item3 : prop;
      T4 Item4 : prop;
      T5 Item5 : prop;
      T6 Item6 : prop;
   }

}