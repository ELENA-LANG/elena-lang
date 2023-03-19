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

   get int Absolute()
   {
      int n := self;
    
      if (n < 0)
         { n := -n };
    
      ^ n
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
    
   string padLeft(int length)
      = self.padLeft($32, length);
    
   string padLeft(char ch, int length)
   {
      int currentLength := self.Length;
        
      if (currentLength < length)
         { ^ String.fill(length - currentLength, ch).add(self) };
            
      ^ self
   }
    
   string padRight(int length)
       = self.padRight($32, length);
    
   string padRight(char ch, int length)
   {
      int currentLength := self.Length;

      if (currentLength < length)
         { ^ self.add(String.fill(length - currentLength, ch)) };
            
      ^ self
   }

   bool startingWith(string s)
   {
      int n := self.indexOf(0, s);
        
      ^ 0 == n
   }
        
   bool endingWith(string s)
   {
      int l1 := self.Length;
      int l2 := s.Length;

      int n := self.indexOf(l1 - l2, s);
        
      ^ n != -1
   }
        
   bool containing(string s)
   {
      int l1 := self.Length;
      int l2 := s.Length;

      int n := self.indexOf(0, s);
        
      ^ n != -1
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

// --- arrayOp ---

public extension weakArrayOp
{
    Array Subarray(int index, int length)
    {
        auto it := self.indexer();
        it.Index := index;
        
        auto a := Array.allocate(length);
        for (int i := 0, i < length, i += 1)
        {
            a[i] := *it;
            
            it.appendIndex(1)
        };
        
        ^ a;
    }
}
//
//public extension arrayOp<T> : Array<T>
//{
//    T[] Subarray(int index, int length)
//    {
//        ^ new Array<T>(self, index, length)
//    }
//}

// --- stringWeakOp ---
public extension stringWeakOp
{
   fillString(s, int counter)
   {
      for (int i := 0, i < counter, i += 1)
      {
            self.write(s)
      };
            
      ^ self
   }

   writeCopies(line, int counter)
      = self.fillString(line, counter);

   writePadding(line, char ch, int width)
   {
      int length := line.Length;        
        
      if (length < width)
      {
         int len := width - length;
         int leftLen := len / 2;
            
         self.fillString(ch, leftLen);
         self.write(line);
         self.fillString(ch, len - leftLen)
      }
      else
      { 
         self.write(line)
      };
            
      ^ self
   }   


   writePaddingLeft(line, char ch, int width)
   {
      var paddedStr := line.padLeft(ch, width);

      self.write(paddedStr);
            
      ^ self
   }
    
   writePaddingRight(line, char ch, int width)
   {
      var paddedStr := line.padRight(ch, width);

      self.write(paddedStr);
            
      ^ self
   }
}