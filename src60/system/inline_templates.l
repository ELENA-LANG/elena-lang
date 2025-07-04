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

// --- importing ---

public inline importing(libraryName)
{
   #import __decl = libraryName;
}

// --- prop ---

public field prop<prop_name, T, field_name>
{
   T field_name;

   get T prop_name()
   {
      ^ field_name
   }

   set prop_name(T value)
   {
      field_name := value
   }
}

public field prop<name, T>
{
   T name;

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
   T name;

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

public field rprop<prop_name, T, field_name>
{
   T field_name;

   get T prop_name()
   {
      ^ field_name
   }
}

public field rprop<name, T>
{
   T name;

   get T name()
   {
      ^ name
   }
}

public field rprop<name>
{
   T name;

   get name()
   {
      ^ name
   }
}

// --- dto_prop ---

public field dto_prop<prop_name, T, field_name>
{
   T field_name;

   get T prop_name()
   {
      ^ field_name
   }

   set prop_name(T value)
   {
      field_name := value
   }

   const subject[] __dto_properties;

   this __dto_properties += mssg prop_name;

   static MessageName[] __dto_properties()
   {
      ^ __dto_properties
   }
}

public field dto_prop<name, T>
{
   T name;

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

public enum<T>(params member)::
{
   T _value;

   const T[] __enumValues;
   const string[] __enumNames;

   private constructor() {} 

   private constructor load(T v) { _value := v }

#for(member)

   this __enumValues += $value member;
   this __enumNames += $name member;

   get static __typeof member()
      = self.load($value member);

#endfor

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

// --- record ---

public template record(params member)::
{
#for(member) 

   member;

   member
     = $value member;

#endfor

   constructor(
#for(member) 
      member
#endfor
   ) {
#for(member) 
      $value this member := $value member;
#endfor
   }

   constructor()
   {
#for(member) 
      $value this member := default;
#endfor
   }

   bool equal(__typeof o)
   {
#for(member) 
      if($value member != $value get mssg member(o)) {
         ^ false
      };
#endfor

      ^ true
   }
}   

// --- yield state machine ---

public abstract class YieldStateEnumerator<T> : Enumerator<T>
{
   private __context;  // context must be the first one
   private T __current; 

   get T Value()
      = __current;

   reset()
   {
      NotSupportedException.raise()
   }

   Enumerator cast() = new Enumerator { embeddable dispatch() => self; };

   enumerable() = self;
}
