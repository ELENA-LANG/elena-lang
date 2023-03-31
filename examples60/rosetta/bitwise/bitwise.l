/// a routine to perform a bitwise AND, OR, and XOR on two integers, a bitwise NOT on the first integer, a left shift, right

import extensions;

extension testOp
{
   bitwiseTest(y)
   {
      console.printLine(self," and ",y," = ",self.band(y));
      console.printLine(self," or ",y," = ",self.bor(y));
      console.printLine(self," xor ",y," = ",self.bxor(y));
      console.printLine("not ",self," = ",self.BInverted);
      console.printLine(self," shr ",y," = ",self.shiftRight(y));
      console.printLine(self," shl ",y," = ",self.shiftLeft(y));
   }
}

public program()
{
   console.loadLineTo(new Integer()).bitwiseTest(console.loadLineTo(new Integer()))
}