import extensions;

//Enumerable yield func(int start, int number)
//{
//   for (int i = 0; i < number; i++)
//   {
//       $yield start + 2 * i;
//   }
//}
//
//public program()
//{
//   foreach(var item :in func(2, 10))
//   {
//      console.writeLine(item)
//   }
//}



//singleton InfinitLoop
//{
//   static Enumerator _enum;
//
//   private Enumerable loop()
//   {
//      int i := 0;
//      while (true) {
//         $yield i;
//
//         i++;
//      }
//   }
//
//   int nextNumber()
//   {
//      var enum := _enum ?? _enum := loop().getEnumerator();
//
//      enum.next();
//
//      ^ *enum;
//   }
//}

//private sealed template YieldStateEnumerator : Enumerator
//{
//   private __raw rawData;
//   private __array stackLocals;
//
//   private object __current; 
//
//   private bool gotoNextStep()
//   {
//      <jumper>(rawData)
//
//      for (int stackLocals.i = 0; i < stackLocals.number; stackLocals.i++)
//      {
//         __current := stackLocals.start + 2 * stackLocals.i;
//         
//         <capture>(rawData);
//
//         ^ true;
//      };
//
//      __current := nil;
//      <reset>(rawData);
//
//      ^ false;      
//   }
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
//
//private sealed template YieldStateEnumerator<T> : Enumerator<T>
//{
//   private __raw rawData;
//   private __array stackLocals;
//
//   private T __current; 
//
//   private bool gotoNextStep()
//   {
//      <jumper>(rawData)
//
//      for (int stackLocals.i = 0; i < stackLocals.number; stackLocals.i++)
//      {
//         __current := stackLocals.start + 2 * stackLocals.i;
//         
//         <capture>(rawData);
//
//         ^ true;
//      };
//
//      __current := nil;
//      <reset>(rawData);
//
//      ^ false;      
//   }
//
//   T Value()
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
//   Enumerable<T> enumerable()
//      = __typeof self;
//
//   static Enumerable<T> cast()
//      = {
//         Enumerator<T> enumerator()
//            = new __typeof __decl();
//      };
//}

A
{
   get static Factory = ::{
      new()
         = new __typeof self();
   };
}


public program()
{
   var a := A.Factory.new();
}
