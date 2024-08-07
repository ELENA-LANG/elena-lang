// declare system categories
#new string descriptions;
#new __string preloadedSymbols;

// --- base inline templates ---

public inline using(blockName)
{
   #include textblock blockName;
}

public inline external(routine)
{
   #include intern routine;
}

// --- class meta info ---

public inline info(descr)
{
   #share super string descriptions;

   #let super descriptions[$name __decl] := descr;
}

// --- preloaded ---

public inline preloaded()
{
   #share __string preloadedSymbols;

   #let preloadedSymbols += __decl;
}

// --- prop ---

public field prop<name, T>
{
   get T name()
   {
      ^ name
   }

   set name(T value)
   {
      name := value
   }
}

public field prop<name>
{
   get name()
   {
      ^ name
   }

   set name(value)
   {
      name := value
   }
}

// --- rprop ---

public field rprop<name, T>
{
   get T name()
   {
      ^ name
   }
}

public field rprop<name>
{
   get name()
   {
      ^ name
   }
}

// --- dto_prop ---

public field dto_prop<name, T>
{
   get T name()
   {
      ^ name
   }

   set name(T value)
   {
      name := value
   }

   const subject[] __dto_properties;

   this __dto_properties += mssg name;

   static MessageName[] __dto_properties()
   {
      ^ __dto_properties
   }
}

// --- interface<T> ---

public weak interface<T>
{
   T cast() = new T
   {
      embeddable dispatch() => self;
   };
}

// --- enum ---

public enumeration enum<member, T>
{
   T _value;

   const T[] __enumValues;
   const string[] __enumNames;

   this __enumValues += $value member;
   this __enumNames += $name member;

   private constructor() {} 

   private constructor load(T v) { _value := v }

   get static __typeof member()
      = self.load($value member);

   T cast()
      = _value;

   private static __typeof getEnumValue(int index)
   {
      T value := __enumValues[index];

      ^ __typeof self.load(value)
   }

   private static int getEnumCount()
   {
      int len := __enumValues.Length;

      ^ len
   }

   static Enumerator enumerator() = new BaseEnumerator
   {
      this index := -1;
         
      this length := getEnumCount();
         
      get Value() = getEnumValue(this index.Value);
         
      enumerable() = self;
         
      reset()
      {
         this index.Value := -1
      }
   };

   string toPrintable()
   {
      string[] names := __enumNames;
      T[] values := __enumValues;

      int len := values.Length;
      for (int i := 0; i < len; i++) {
         if (values[i] == _value) {
            ^ names[i]
         }            
      };

      InvalidOperationException.raise();

      ^ string.MinValue
   }

   bool equal(o)
   {
      if (o; is T n)
      {
         ^ _value == n
      };

      ^ false
   }
}

//// --- yield state machine ---
//
//public sealed template YieldStateEnumerator : Enumerator
//{
//   private object __current; 
//
//   //private __raw rawData;
//   //private __array stackLocals;
//
////   private bool gotoNextStep()
////   {
////      <jumper>(rawData)
////
////      for (int stackLocals.i = 0; i < stackLocals.number; stackLocals.i++)
////      {
////         __current := stackLocals.start + 2 * stackLocals.i;
////         
////         <capture>(rawData);
////
////         ^ true;
////      };
////
////      __current := nil;
////      <reset>(rawData);
////
////      ^ false;      
////   }
//
//   Value()
//      = _current;
//
//   bool next()
//      <= gotoNextStep();
//
//   reset()
//   {
//      NotSupportedException.raise()
//   }
//
//   Enumerable enumerable()
//      = __typeof self;
//
//   static Enumerable cast()
//      = {
//         Enumerator enumerator()
//            = new __typeof __decl();
//      };
//}
