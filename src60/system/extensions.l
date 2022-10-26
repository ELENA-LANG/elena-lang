// --- objectOp ---

public extension objectOp
{
   string __getClassName()   
      = __className;
}

// --- intOp ---

public extension intOp : IntNumber
{
   int mod(int operand)
   {
      int retVal := self / operand * operand;

      retVal := self - retVal;
        
      ^ retVal
   }    
}