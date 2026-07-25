namespace collections
{
   public sealed Tuple<T1> : info("An immutable fixed-size collection of one elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1) : info("Creates a fixed-size collection with specified values")
      {
         Item1 := o1
      }
        
      T1 Item1 : rprop;

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            ! : { OutOfRangeException.raise() }
      }

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 1;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            NotSupportedException.raise();
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 1;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed Tuple<T1, T2> : info("An immutable fixed-size collection of two elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1, T2 o2) : info("Creates a fixed-size collection with specified values")
      {
         Item1 := o1;
         Item2 := o2
      }
        
      T1 Item1 : rprop;
      T2 Item2 : rprop;

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            1 : { ^ Item2 }
            ! : { OutOfRangeException.raise() }
      }      

      Tuple<object,object> cast()
         = new Tuple<object,object>(Item1, Item2);

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 2;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            NotSupportedException.raise();
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 2;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed VarTuple<T1, T2> : info("A variable fixed-size collection of two elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1, T2 o2) : info("Creates a fixed-size collection with specified values")
      {
         Item1 := o1;
         Item2 := o2
      }
        
      T1 Item1 : rprop;
      T2 Item2 : prop;

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            1 : { ^ Item2 }
            ! : { OutOfRangeException.raise() }
      }      

      setAt(int index, object val) : info("Assigns <b>item</b> to the element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { Item1 := val }
            1 : { Item2 := val }
            ! : { OutOfRangeException.raise() }
      }

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 2;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            self.setAt(this index, o);
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 2;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed VarTuple<T1, T2, T3> : info("A variable fixed-size collection of three elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3) : info("Creates a fixed-size collection with specified values")
      {
         Item1 := o1;
         Item2 := o2;
         Item3 := o3
      }
        
      T1 Item1 : prop;
      T2 Item2 : prop;
      T3 Item3 : prop;

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            1 : { ^ Item2 }
            2 : { ^ Item3 }
            ! : { OutOfRangeException.raise() }
      }      

      setAt(int index, object val) : info("Assigns <b>item</b> to the element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { Item1 := val }
            1 : { Item2 := val }
            2 : { Item3 := val }
            ! : { OutOfRangeException.raise() }
      }

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 3;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            self.setAt(this index, o)
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 3;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed Tuple<T1, T2, T3> : info("An immutable fixed-size collection of three elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3) : info("Creates a fixed-size collection with specified values")
      {
         Item1 := o1;
         Item2 := o2;
         Item3 := o3
      }
        
      T1 Item1 : rprop;
      T2 Item2 : rprop;
      T3 Item3 : rprop;

      Tuple<object,object,object> cast()
         = new Tuple<object,object,object>(Item1, Item2, Item3);

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            1 : { ^ Item2 }
            2 : { ^ Item3 }
            ! : { OutOfRangeException.raise() }
      }      

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 3;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            NotSupportedException.raise();
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 3;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed VarTuple<T1, T2, T3, T4> : info("A variable fixed-size collection of four elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3, T4 o4) : info("Creates a fixed-size collection with specified values")
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

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            1 : { ^ Item2 }
            2 : { ^ Item3 }
            3 : { ^ Item4 }
            ! : { OutOfRangeException.raise() }
      }      

      setAt(int index, object val) : info("Assigns <b>item</b> to the element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { Item1 := val }
            1 : { Item2 := val }
            2 : { Item3 := val }
            3 : { Item4 := val }
            ! : { OutOfRangeException.raise() }
      }

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 4;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            self.setAt(this index, o)
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 4;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed Tuple<T1, T2, T3, T4> : info("An immutable fixed-size collection of four elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3, T4 o4) : info("Creates a fixed-size collection with specified values")
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

      Tuple<object,object,object,object> cast()
         = new Tuple<object,object,object,object>(Item1, Item2, Item3, Item4);

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            1 : { ^ Item2 }
            2 : { ^ Item3 }
            3 : { ^ Item4 }
            ! : { OutOfRangeException.raise() }
      }      

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 4;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            NotSupportedException.raise();
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 4;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed VarTuple<T1, T2, T3, T4, T5> : info("A variable fixed-size collection of five elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3, T4 o4, T5 o5) : info("Creates a fixed-size collection with specified values")
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

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            1 : { ^ Item2 }
            2 : { ^ Item3 }
            3 : { ^ Item4 }
            4 : { ^ Item5 }
            ! : { OutOfRangeException.raise() }
      }      

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 5;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            NotSupportedException.raise();
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 5;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed Tuple<T1, T2, T3, T4, T5> : info("An immutable fixed-size collection of five elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3, T4 o4, T5 o5) : info("Creates a fixed-size collection with specified values")
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

      Tuple<object,object,object,object,object> cast()
         = new Tuple<object,object,object,object,object>(Item1, Item2, Item3, Item4, Item5);

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            1 : { ^ Item2 }
            2 : { ^ Item3 }
            3 : { ^ Item4 }
            4 : { ^ Item5 }
            ! : { OutOfRangeException.raise() }
      }      

      setAt(int index, object val) : info("Assigns <b>item</b> to the element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { Item1 := val }
            1 : { Item2 := val }
            2 : { Item3 := val }
            3 : { Item4 := val }
            4 : { Item5 := val }
            ! : { OutOfRangeException.raise() }
      }

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 5;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            self.setAt(this index, o)
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 5;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed Tuple<T1, T2, T3, T4, T5, T6> : info("An immutable fixed-size collection of six elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3, T4 o4, T5 o5, T6 o6) : info("Creates a fixed-size collection with specified values")
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

      Tuple<object,object,object,object,object,object> cast()
         = new Tuple<object,object,object,object,object,object>(Item1, Item2, Item3, Item4, Item5, Item6);

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            1 : { ^ Item2 }
            2 : { ^ Item3 }
            3 : { ^ Item4 }
            4 : { ^ Item5 }
            5 : { ^ Item6 }
            ! : { OutOfRangeException.raise() }
      }      

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 6;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            NotSupportedException.raise();
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 6;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed VarTuple<T1, T2, T3, T4, T5, T6> : info("A variable fixed-size collection of six elemenet")
   {
      constructor() : info("Creates a fixed-size collection with default values")
      {
      }    
        
      constructor(T1 o1, T2 o2, T3 o3, T4 o4, T5 o5, T6 o6) : info("Creates a fixed-size collection with specified values")
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

      at(int index) : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { ^ Item1 }
            1 : { ^ Item2 }
            2 : { ^ Item3 }
            3 : { ^ Item4 }
            4 : { ^ Item5 }
            5 : { ^ Item6 }
            ! : { OutOfRangeException.raise() }
      }      

      setAt(int index, object val) : info("Assigns <b>item</b> to the element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         index =>
            0 : { Item1 := val }
            1 : { Item2 := val }
            2 : { Item3 := val }
            3 : { Item4 := val }
            4 : { Item5 := val }
            5 : { Item6 := val }
            ! : { OutOfRangeException.raise() }
      }

      Indexer indexer() 
         : info("Creates a fixed-sized collection indexer") 
      = new BaseIndexer
      {
         this index := 0;
           
         this length := 6;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            self.setAt(this index, o)
         }
      };

      Enumerator enumerator()
        : info("Creates a fixed-sized collection enumerator") 
      = new BaseEnumerator
      {
         this index := -1;
         
         this length := 6;
        
         get Value() = self.at(this index.Value);
        
         enumerable() = self;
        
         reset()
         {
            this index.Value := -1
         }
      };
   }

   public sealed class Union<T1, T2>
   {
      T1? _instance1;
      T2? _instance2;
      
      constructor(T1 instance)
      {
         _instance1 := instance;
      }
      
      constructor(T2 instance)
      {
         _instance2 := instance;
      }
   
      get retoverload T1? Value()
         = _instance1;
   
      get retoverload T2? Value()
         = _instance2;
         
      get Value()
         = _instance1 ?? _instance2;
   }
}