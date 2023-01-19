// --- objectOp ---

public extension objectOp
{
   string __getClassName()   
      = __className;

   __getClass() : external(system'core_routines'__object_class),
                  info("Returns the object class");
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

// --- longOp ---

public extension longOp : LongNumber
{
   long mod(long operand)
   {
      long retVal := self / operand * operand;

      retVal := self - retVal;
        
      ^ retVal
   }    
}

// --- realOp ---

public extension realOp : RealNumber
{
   get real Integer()
   {
      PrimitiveRealOperations.trunc(self, ref real retVal);

      ^ retVal
   }
}

// --- intRefOp ---

public extension intRefOp : Reference<int>
{
    append(int n)
    {
        int v := *self;
        v := v + n;
        
        self.Value := v
    }
    
    reduce(int n)
    {
        int v := *self;
        v := v - n;
        
        self.Value := v
    }
}
