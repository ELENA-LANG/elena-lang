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

//public enumeration enum<member, T>
//{
//   T _value;
//
//   private constructor() {} 
//
//   private constructor load(T v) { _value := v }
//
//   get static __typeof member()
//      = self.load($value member);
//}
//
//public const struct Color : enum<int>(Red = 1,Green = 2,Blue = 3);
//
//public program()
//{
//   Color red := Color.Red;
////   Color green := Color.Green;
//
////   var kv := x = 3;
//
////   var v := kv.x;
//}

/*const struct */X
{
   int _value;

   private constructor() {}

   /*private */constructor load(int v) { _value := v }
}

public program()
{
   var x := X.load(2);
}