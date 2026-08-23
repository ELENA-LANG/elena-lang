import extensions;

const struct XY
{
   int x;
   int y;
   
   const static XY ZeroXY
      = XY { this x := 0; this y := 1; };
}

public interface I
{
   private constructor() {}
   
   abstract string whoAmI();
   
   const static I A
      = I { string whoAmI() = "A"; };
      
   const static I B
      = I { string whoAmI() = "B"; };
}

//public const struct Color : enum<int>(Red = 1,Green = 2,Blue = 3);

public Program()
{
   XY z := XY.ZeroXY;
   
//    Color color := Color.Red; 
//    if (color == Color.Red) {
//       Console.writeLine("red")
//    }
}
