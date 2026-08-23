import extensions;

public const struct Color : enum<int>(Red = 1,Green = 2,Blue = 3);

public Program()
{
    Color color := Color.Red; 
    var b := color == Color.Red;
//    if (color == Color.Red) {
//       Console.writeLine("red")
//    }
}
