// --- flowOp ---

public extension flowOp
{
   writeTo(target)
   {
      target.write(self);
   }
}

// --- loopOp --

public extension loopOp
{
   doWhile()
   {
      while (cast bool(self))
      {
      }
   }
}