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
        
      T1 Item1 : rprop;

      at(int index)
      {
         index =>
            0 { ^ Item1 }
            ! { OutOfRangeException.raise() }
      }

      setAt(int index, object val)
      {
         index =>
            0 { Item1 := val }
            ! { OutOfRangeException.raise() }
      }
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
        
      T1 Item1 : rprop;
      T2 Item2 : rprop;

      at(int index)
      {
         index =>
            0 { ^ Item1 }
            1 { ^ Item2 }
            ! { OutOfRangeException.raise() }
      }      

      setAt(int index, object val)
      {
         index =>
            0 { Item1 := val }
            1 { Item2 := val }
            ! { OutOfRangeException.raise() }
      }

      Tuple<object,object> cast()
         = new Tuple<object,object>(Item1, Item2);
//         = ();        
   }

   public sealed VarTuple<T1, T2>
   {
      constructor()
      {
      }    
        
      constructor(T1 o1, T2 o2)
      {
         Item1 := o1;
         Item2 := o2
      }
        
      T1 Item1 : rprop;
      T2 Item2 : prop;

      at(int index)
      {
         index =>
            0 { ^ Item1 }
            1 { ^ Item2 }
            ! { OutOfRangeException.raise() }
      }      

      setAt(int index, object val)
      {
         index =>
            0 { Item1 := val }
            1 { Item2 := val }
            ! { OutOfRangeException.raise() }
      }
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
        
      T1 Item1 : rprop;
      T2 Item2 : rprop;
      T3 Item3 : rprop;

      at(int index)
      {
         index =>
            0 { ^ Item1 }
            1 { ^ Item2 }
            2 { ^ Item3 }
            ! { OutOfRangeException.raise() }
      }      

      setAt(int index, object val)
      {
         index =>
            0 { Item1 := val }
            1 { Item2 := val }
            2 { Item3 := val }
            ! { OutOfRangeException.raise() }
      }
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

      T1 Item1 : rprop;
      T2 Item2 : rprop;
      T3 Item3 : rprop;
      T4 Item4 : rprop;

      at(int index)
      {
         index =>
            0 { ^ Item1 }
            1 { ^ Item2 }
            2 { ^ Item3 }
            3 { ^ Item4 }
            ! { OutOfRangeException.raise() }
      }      

      setAt(int index, object val)
      {
         index =>
            0 { Item1 := val }
            1 { Item2 := val }
            2 { Item3 := val }
            3 { Item4 := val }
            ! { OutOfRangeException.raise() }
      }
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
        
      T1 Item1 : rprop;
      T2 Item2 : rprop;
      T3 Item3 : rprop;
      T4 Item4 : rprop;
      T5 Item5 : rprop;

      at(int index)
      {
         index =>
            0 { ^ Item1 }
            1 { ^ Item2 }
            2 { ^ Item3 }
            3 { ^ Item4 }
            4 { ^ Item5 }
            ! { OutOfRangeException.raise() }
      }      

      setAt(int index, object val)
      {
         index =>
            0 { Item1 := val }
            1 { Item2 := val }
            2 { Item3 := val }
            3 { Item4 := val }
            4 { Item5 := val }
            ! { OutOfRangeException.raise() }
      }
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
        
      T1 Item1 : rprop;
      T2 Item2 : rprop;
      T3 Item3 : rprop;
      T4 Item4 : rprop;
      T5 Item5 : rprop;
      T6 Item6 : rprop;

      at(int index)
      {
         index =>
            0 { ^ Item1 }
            1 { ^ Item2 }
            2 { ^ Item3 }
            3 { ^ Item4 }
            4 { ^ Item5 }
            5 { ^ Item6 }
            ! { OutOfRangeException.raise() }
      }      

      setAt(int index, object val)
      {
         index =>
            0 { Item1 := val }
            1 { Item2 := val }
            2 { Item3 := val }
            3 { Item4 := val }
            4 { Item5 := val }
            5 { Item6 := val }
            ! { OutOfRangeException.raise() }
      }
   }
}