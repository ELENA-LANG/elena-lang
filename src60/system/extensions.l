// --- objectOp ---

public extension objectOp
{
   string __getClassName()   
      = __className;

   __getClass() : external(system'core_routines'__object_class),
                  info("Returns the object class");

   bool safeEqual(o)
      = self.equal:o \ back:false;

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

// --- charOp ---

public extension charOp : CharValue
{
   bool isWhitespace()
   {
      self =>
         $9  { ^ true }
         $32 { ^ true }
         $10 { ^ true }
         $13 { ^ true }
         $12 { ^ true };
            
      ^ false
   }

   bool isDigit()
   {
      int ch := intConvertor.convert(self);
        
      ^ (ch >= 30h) && (ch <= 39h)
   }

    
   bool isLToken()
   {
      int ch := intConvertor.convert(self);
        
      if (ch >= 41h && ch <= 5Ah)
         { ^ true };
        
      if (ch >= 61h && ch <= 7Ah)
         { ^ true };
        
      if (ch >= 3ACh && ch <= 3CEh)
         { ^ true };
            
      if (ch >= 400h && ch <= 42Fh)
         { ^ true };
            
      ^ ch == 39
   }

}