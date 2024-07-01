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
   get static T member()
      = $value member;
}
//
//
//{
//   get static T $name Red()
//      = nil;
//
//   get static T $name Green()
//      = nil;
//
//   get static T $name Blue()
//      = nil;
//}
//
//


public Color : enum<int>(Red = 1,Green = 2,Blue = 3);

public program()
{
//   Color red := Color.Red;
//   Color green := Color.Green;

//   var kv := x = 3;

//   var v := kv.x;
}
