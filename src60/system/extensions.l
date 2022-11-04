// --- objectOp ---

public extension objectOp
{
   string __getClassName()   
      = __className;

   __getClass() : external<system'core_routines'__object_class>,
                  info<"Returns the object class">;
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