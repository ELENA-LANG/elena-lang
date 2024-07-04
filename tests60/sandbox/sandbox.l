//Color : enum(Red = 1, Green = 2, Blue = 3);

//class Color
//{
//   int _value;
//
//   private constructor(int v) { _value := v }
//
//   get static Color Red()
//      = new Color(1);
//
//   get static Color Green()
//      = new Color(2);
//
//   get static Color Blue()
//      = new Color(3);
//}

/*
   get static T $name member()
      = member;
*/

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

   string toPrintable()
   {
      string[] names := __enumNames;
/*      T[] values := __enumValues;

      int len := values.Length;
      for (int i := 0; i < len; i++) {
         if (values[i] == _value) {
            ^ names[i]
         }            
      };

      InvalidOperationException.raise();*/

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

public const struct Color : enum<int>(Red = 1,Green = 2,Blue = 3);

public program()
{
   Color red := Color.Red;
//   Color green := Color.Green;
//
//   if (red == Color.Red)
//   {
//      console.writeLine("Comparson works!")
//   };
//   if (red == Color.Green)
//   {
//      console.writeLine("Comparson doesn't work!")
//   };

   var s1 := red.toPrintable();
//   var s2 := red.toPrintable();
}
