/// a routine to perform a bitwise AND, OR, and XOR on two integers, a bitwise NOT on the first integer, a left shift, right

import extensions;

extension testOp
{
   bitwiseTest(y)
   {
      Console.printLine(self," and ",y," = ",self.band(y));
      Console.printLine(self," or ",y," = ",self.bor(y));
      Console.printLine(self," xor ",y," = ",self.bxor(y));
      Console.printLine("not ",self," = ",self.BInverted);
      Console.printLine(self," shr ",y," = ",self.shiftRight(y));
      Console.printLine(self," shl ",y," = ",self.shiftLeft(y));
   }
}

public program()
{
   Console.loadLineTo(new Integer()).bitwiseTest(Console.loadLineTo(new Integer()))
}