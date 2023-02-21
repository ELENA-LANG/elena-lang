// --- objectOp ---

public extension objectOp
{
   string __getClassName()   
      = __className;

   __getClass() : external(system'core_routines'__object_class),
                  info("Returns the object class");

   bool safeEqual(o)
      = self.equal:o \ back:false;
}

// --- intOp ---

public extension intOp : IntNumber
{
   int Hashcode
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int retVal := PrimitiveIntOperations.getHashcode(self);

         ^ retVal
      }
   }

   int mod(int operand)
   {
      int retVal := self / operand * operand;

      retVal := self - retVal;
        
      ^ retVal
   }

   int mod(o)
   {
      int retVal := self.mod(cast int(o));
        
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
   int Hashcode
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int retVal := PrimitiveRealOperations.getHashcode(self);

         ^ retVal
      }
   }

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

// --- stringOp ---

public extension stringOp : String
{
   string Substring(int index)
   {
      int len := self.Length;
      if (len > index)
      {
         len := len - index;
            
         ^ self.Substring(index, len)
      }
      else
      {
         ^ emptyString
      }        
   }

   int Hashcode
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int retVal := PrimitiveStringOperations.getHashcode(self);

         ^ retVal
      }
   }

   bool isEmpty()
   {
      int len := self.Length;
        
      ^ len == 0;
   }
        
   bool isNonempty()
   {
      int len := self.Length;
        
      ^ len != 0;
   }
}