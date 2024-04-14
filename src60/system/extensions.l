// --- objectOp ---

public extension objectOp
{
   string __getClassName()   
      = __className;

   __getClass() : external(system'core_routines'__object_class),
                  info("Returns the object class");

   __getParentClass() : external(system'core_routines'__object_parentclass),
                  info("Returns the parent class");

   bool safeEqual(o)
         : info("Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible")
      = self.equal(o) \ back(false);

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

   bool isNil()
         : info("Returns true if the object is nil")
      = nil == self;       

   doWith(action)
      = action(self);

   bool instanceOf(type) : external(system'core_routines'__object_instanceOf),
                  info("Checks if the object is an instance of type");   
}

// --- valueOp ---
public extension valueOp
{
   bool isLiteral()
      : info("Returns true if the object is literal or character")
   {
      if (self; is string s)
      {
         ^ true
      };

      if (self; is wide s)
      {
         ^ true
      };

      if (self; is char ch)
      {
         ^ true
      };


      ^ false
   }

   bool isInteger()
      : info("Returns true if the object is an integer number")
   {
      if (self; is int n)
      {
         ^ true
      };

      if (self; is uint n)
      {
         ^ true
      };

      if (self; is short n)
      {
         ^ true
      };

      if (self; is ushort n)
      {
         ^ true
      };

      if (self; is byte n)
      {
         ^ true
      };

      if (self; is sbyte n)
      {
         ^ true
      };

      if (self; is long n)
      {
         ^ true
      };

      ^ false
   }

   bool isNumber()
      : info("Returns true if the object is a number")
   {
      if (self; is real n)
      {
         ^ true
      };

      ^ self.isInteger()
   }
} 

// --- byteOp ---

public extension byteOp : ByteNumber
{
   int Hashcode
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int val := self;
         int retVal := PrimitiveIntOperations.getHashcode(val);

         ^ retVal
      }
   }

   get byte Absolute()
   {
      byte n := self;
    
      if (n < 0)
         { n := -n };
    
      ^ n
   }

   byte power(int y)
   {
      byte x := self;
      byte r := 1;
        
      for (int i := 0; i < y; i += 1)
      {
         r := r * x
      };
        
      ^ new ByteNumber(r)
   }

   byte sqr()
      = self * self;

   byte sqrt()
   {   
      byte num := self;
              
      if (num == 0)
         { ^ 0 };
                  
      byte n := num / 2 + 1;
      byte n1 := (n + (num / n)) / 2;
      while (n1 < n)
      {
         n := n1;
         n1 := (n + (num / n)) / 2
      };
              
      ^ n
   }

   byte mod(operand)
   {
      var retVal := self / operand * operand;
        
      ^ byteConvertor.convert(retVal)
   }

   bool isOdd()
      = self.mod(2) != 0;
        
   bool isEven()
      = self.mod(2) == 0;
        
   bool isZero()
      = self.equal(0);
        
   bool isPositive()
      = self > 0;
        
   bool isNegative()
      = self < 0;
        
   bool isNonnegative()
      = self <= 0;
}

// --- shortOp ---

public extension shortOp : ShortNumber
{
   int Hashcode
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int val := self;
         int retVal := PrimitiveIntOperations.getHashcode(val);

         ^ retVal
      }
   }

   get short Absolute()
   {
      short n := self;
    
      if (n < 0)
         { n := -n };
    
      ^ n
   }

   short power(int y)
   {
      short x := self;
      short r := 1;
        
      for (int i := 0; i < y; i += 1)
      {
         r := r * x
      };
        
      ^ new ShortNumber(r)
   }

   short sqr()
      = self * self;

   short sqrt()
   {   
      short num := self;
              
      if (num == 0)
         { ^ 0 };
                  
      short n := num / 2 + 1;
      short n1 := (n + (num / n)) / 2;
      while (n1 < n)
      {
         n := n1;
         n1 := (n + (num / n)) / 2
      };
              
      ^ n
   }

   short mod(operand)
   {
      var retVal := self / operand * operand;
        
      ^ shortConvertor.convert(retVal)
   }    

   bool isOdd()
      = self.mod(2) != 0;
        
   bool isEven()
      = self.mod(2) == 0;
        
   bool isZero()
      = self.equal(0);
        
   bool isPositive()
      = self > 0;
        
   bool isNegative()
      = self < 0;
        
   bool isNonnegative()
      = self <= 0;

   get byte High()
   {
      short w := self;
      w := w & 0FF00h;
      w := w.shiftRight(8);
        
      ^ byteConvertor.convert(w)
   }
        
   get byte Low()
   {
      short w := self;
      w := w & 0FFh;
        
      ^ byteConvertor.convert(w)
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

   int power(int y)
   {
      int x := self;
      int r := 1;
      if (x == 0)
         { r := 0 };
        
      for(int i := 0; i < y; i += 1)
      {
         r := r * x
      };
        
      ^ r
   }

   int sqr()
      = self * self;

   int sqrt()
   {   
      int num := self;
              
      if (num == 0)
         { ^ 0 };
                  
      int n := num / 2 + 1;
      int n1 := (n + (num / n)) / 2;
      while (n1 < n)
      {
         n := n1;
         n1 := (n + (num / n)) / 2
      };
              
      ^ n
   }

   real realDiv(n)
      = new RealNumber(self).divide(n);

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

   bool isOdd()
      = self.mod(2) != 0;
        
   bool isEven()
      = self.mod(2) == 0;
        
   bool isZero()
      = self.equal(0);
        
   bool isPositive()
      = self > 0;
        
   bool isNegative()
      = self < 0;
        
   bool isNonnegative()
      = self <= 0;

   get short High()
   {
      int w := self;
      w := w & 0FFFF0000h;
      w := w.shiftRight(8);
        
      ^ shortConvertor.convert(w)
   }
        
   get short Low()
   {
      int w := self;
      w := w & 0FFFFh;
        
      ^ shortConvertor.convert(w)
   }
}

// --- uintOp ---

public extension uintOp : UIntNumber
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

   uint sqr()
      = self * self;

   uint sqrt()
   {   
      uint num := self;
              
      if (num == 0u)
         { ^ 0u };
                  
      uint n := num / 2u + 1u;
      uint n1 := (n + (num / n)) / 2u;
      while (n1 < n)
      {
         n := n1;
         n1 := (n + (num / n)) / 2u
      };
              
      ^ n
   }

   uint mod(uint operand)
   {
      uint retVal := self / operand * operand;

      retVal := self - retVal;
        
      ^ retVal
   }

   uint mod(int operand)
   {
      uint uoperand := operand;
      uint retVal := self / uoperand * uoperand;

      retVal := self - retVal;
        
      ^ retVal
   }

   uint mod(operand)
      = self.mod(cast uint(operand));

   real realDiv(n)
      = new RealNumber(self).divide(n);

   bool isOdd()
      = self.mod(2) != 0;
        
   bool isEven()
      = self.mod(2) == 0;
        
   bool isZero()
      = self.equal(0);
        
   bool isPositive()
      = self > 0;
        
   bool isNegative()
      = self < 0;
        
   bool isNonnegative()
      = self <= 0;
}

// --- longOp ---

public extension longOp : LongNumber
{   
   int Hashcode
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int retVal := PrimitiveLongOperations.getHashcode(self);

         ^ retVal
      }
   }

   get long Absolute()
   {
      long n := self;

      if (n < 0l)
         { n := n * -1l };
                
      ^ n
   }

   long power(long y)
   {
      long x := self;
      long r := 1;
        
      for(long i := 0l; i < y; i += 1l)
      {
         r := r * x;
      };
        
      ^ new LongNumber(r)
   }
    
   long power(int y)
   {
      long x := self;
      long r := 1;
        
      int i := 0;
      while (i < y)
      {
         r := r * x;
            
         i += 1;
      };
        
      ^ new LongNumber(r)
   }

   long sqr()
      = self * self;

   long sqrt()
   {   
      long num := self;
              
      if (num == 0l)
         { ^ 0l };
                  
      long n := num / 2l + 1l;
      long n1 := (n + (num / n)) / 2l;
      while (n1 < n)
      {
         n := n1;
         n1 := (n + (num / n)) / 2l
      };
              
      ^ n
   }

   real realDiv(n)
      = new RealNumber(self).divide(n);

   long mod(long operand)
   {
      long retVal := self / operand * operand;

      retVal := self - retVal;
        
      ^ retVal
   }    

   long mod(n)
   {
      long x := self;
      long y := cast long(n);
        
      long z := x / y;
      z := z * y;
        
      ^ new LongNumber(x - z)
   }

   bool isOdd()
      = self.mod(2l) != 0l;
        
   bool isEven()
      = self.mod(2l) == 0l;
        
   bool isZero()
      = self.equal(0l);
        
   bool isPositive()
      = self > 0l;
        
   bool isNegative()
      = self < 0l;
        
   bool isNonnegative()
      = self <= 0l;

   get int High()
   {
      long w := self;
      w := w & -4294967296l;
      w := w.shiftRight(32);
        
      ^ intConvertor.convert(w)
   }

   get int Low()
   {
      long w := self;
      w := w & 4294967295l;
        
      ^ intConvertor.convert(w)
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

   real power(real y)
   {
      real x := self;
        
      if(x == 0.0r)
         { ^ 0.0r };
                        
      if(y == 0.0r)
        { ^ 1.0r };
            
      x := x.ln();
        
      x := x * y;
        
      x := x.exp();
            
      ^ x            
   }

   real power(int y)
   {
      real x := self;

      if(x == 0.0r)
         { ^ 0.0r };
                        
      if(y == 0)
         { ^ 1.0r };
                        
      if (x < 0.0r)
      {
         x := x * -1.0r;
            
         x := x.ln();
            
         x := x * y;
            
         x := x.exp();
            
         if (y.isOdd())
            { x := x * -1.0r }
      }
      else
      {
         x := x.ln();
            
         x := x * y;
            
         x := x.exp()
      };
        
      ^ x
   }

   real sqr()
      = self * self;

   real sqrt()
   {
      PrimitiveRealOperations.sqrt(self, ref real retVal);

      ^ retVal
   }

   real exp()
   {
      PrimitiveRealOperations.exp(self, ref real retVal);

      ^ retVal
   }

   real ln()
   {
      PrimitiveRealOperations.ln(self, ref real retVal);

      ^ retVal
   }

   real sin()
   {
      PrimitiveRealOperations.sin(self, ref real retVal);

      ^ retVal
   }

   real cos()
   {
      PrimitiveRealOperations.cos(self, ref real retVal);

      ^ retVal
   }

   real tan()
   {
      PrimitiveRealOperations.sin(self, ref real sinVal);
      PrimitiveRealOperations.cos(self, ref real cosVal);
        
      real n := sinVal / cosVal;
        
      ^ n
   }

   real arctan()
   {
      PrimitiveRealOperations.arctan(self, ref real retVal);

      ^ retVal
   }

   real arcsin()
   {
      real n := self;
        
      real retVal := n * n;    
      retVal := 1.0r - retVal;
      PrimitiveRealOperations.sqrt(retVal, ref real arg);
        
      arg := n / arg;
      PrimitiveRealOperations.arctan(arg, ref retVal);
        
      ^ retVal
   }
            
   real arccos()
   {
      real n := self;        
      real retVal := 0.0r;
        
      retVal := n * n;    
      retVal := 1.0r - retVal;        
      PrimitiveRealOperations.sqrt(retVal, ref real arg);
        
      arg := arg / n;
      PrimitiveRealOperations.arctan(arg, ref retVal);
        
      ^ retVal
   }

   real log2()
   {
      real x := self.ln();
      real y := 2.0.ln();
            
      ^ x / y
   }
            
   real log10()
   {
      real x := self.ln();
      real y := 10.0.ln();
            
      ^ x / y
   }      

   get real Integer()
   {
      PrimitiveRealOperations.trunc(self, ref real retVal);

      ^ retVal
   }

   get int IntegerInt()
      = intConvertor.convert(self.Integer);

   get real Rounded()
   {
      PrimitiveRealOperations.round(self, ref real retVal);

      ^ retVal
   }

   get int RoundedInt()
      = intConvertor.convert(self.Rounded);

   get real Absolute()
   {
      PrimitiveRealOperations.abs(self, ref real retVal);

      ^ retVal  
   }

   get real Reciprocal()
   {
      real value := self;
        
      value := 1.0r / value;
        
      ^ value
   }    

   real frac()
   {
      real retVal := self.Absolute;
      
      ^ retVal - retVal.Integer
   }

   real ceil()
   {
      if (self < 0)
      {
         ^ self.Integer
      }
      else
      {
         real y := self.Integer;
                
         ^ (y == self).iif(y,y+1)
      }
   }

   real floor()
   {
      if (self >= 0)
      {
         ^ self.Integer
      }
      else
      {
         real y := self.Integer;
                
         ^ (y == self).iif(y,y-1)
      }
   }

   real round(int precision)
   {
      real a := self;
      bool neg := false;
      if(self.isNegative())
      {
         a := a * -1.0r;
         neg := true;
      };
           
      int b := 10.power(precision);
      real r := a * b + 0.5r;
        
      int c := intConvertor.convert(r.Rounded);
    
      a := c.realDiv(b);
    
      ^ neg ? (a * -1.0r) : a
   }    

   real truncate(int precision)
   {
      real a := self;
      bool neg := false;
      if(self.isNegative())
      {
         a := a * -1.0r;
         neg := true
      };
       
      int b := 10.power(precision);
      real c := (a * b).Integer; // returns the integer part of the real number

      a := c / b;

      ^ neg ? (a * -1.0r) : a 
   }

            
   get real Radian()
   {
      real value := self;
        
      value := value * Pi_value;
      value := value / 180.0r;
        
      ^ value
   }
            
   get real Degree()
   {
      real value := self;
        
      value := value * 180.0r;
      value := value / Pi_value;
        
      ^ value
   }

   bool isZero()
      = self.equal(0.0);
        
   bool isPositive()
      = self > 0.0;
        
   bool isNegative()
      = self < 0.0;
        
   bool isNonnegative()
      = self <= 0.0;
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
      };

      ^ emptyString
   }

   int indexOf(string s)
      = self.indexOf(0, s);

   int indexOf(char ch)
      = self.indexOf(0, ch);

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
    
   string trimLeft(char ch)
   {
      int  index := 0;
      int  len := self.Length;
      char current := $0;
      int  chLen := ch.Length;
        
      while(index < len)
      {
         current := self[index];
            
         ifnot(current == ch)
         { 
            if (index == 0)
            { 
               ^ self 
            }
            else
            { 
               ^ self.delete(0, index) 
            }
         };
            
         index += chLen
      };
        
      ^ emptyString
   }
    
   string trimLeft()
      = self.trimLeft($32);
       
   string trimRight(char ch)
   {
      int  len := self.Length;
      int  index := len - 1;
      int  prev := len;
      char current := $0;
      int  chLen := ch.Length;
        
      while (index >= 0)
      {
         current := self[index];
            
         ifnot (current == ch)
            { ^ self.delete(prev,len - prev) };
            
         prev := index;
         index -= chLen
      };
        
      ^ emptyString
   }
    
   string trimRight()
      = self.trimRight($32);
        
   string trim(char ch)
      = self.trimLeft(ch).trimRight(ch);
    
   string trim()
      = self.trimLeft($32).trimRight($32);

   string replace(string replacee, string replacer)
   {
      string target := emptyString;
      int    start := 0;
      int    end := self.indexOf(start, replacee);
      int    replaceeLen := replacee.Length;
        
      while (end != -1)
      {
         target := target
                     .add(self.Substring(start, end - start))
                     .add(replacer);
            
         start := end + replaceeLen;
            
         end := self.indexOf(start, replacee)
      };
    
      if(start != 0)
      {
         ^ target.add(self.Substring(start))
      };

      ^ self
   }   

   string replaceFirst(string replacee, string replacer)
   {
      int replaceeLen := replacee.Length;
      int i := self.indexOf(0, replacee);
        
      if (i != -1)
      { 
         ^ self.delete(i, replaceeLen).insert(i, replacee)
      }
      else
      {
         ^ self
      }    
   }    
}

// --- charOp ---

public extension charOp : CharValue
{
   int Hashcode
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int value := self;
         int retVal := PrimitiveIntOperations.getHashcode(value);

         ^ retVal
      }
   }

   bool isNewLine()
   {
      ^ self == $10
   }

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

   bool isLetter()
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
            
      ^ false
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

// --- wideOp ---

public extension wideOp : WideString
{
   wide Substring(int index)
   {
      int len := self.Length;
      if (len > index)
      {
         len := len - index;
            
         ^ self.Substring(index, len)
      };

      ^ emptyWideString
   }

   int indexOf(wide s)
      = self.indexOf(0, s);

   int indexOf(char ch)
      = self.indexOf(0, ch);

   int Hashcode
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int retVal := PrimitiveWideStringOperations.getHashcode(self);

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
    
   wide padLeft(int length)
      = self.padLeft($32, length);
    
   wide padLeft(char ch, int length)
   {
      int currentLength := self.Length;
        
      if (currentLength < length)
         { ^ WideString.fill(length - currentLength, ch).add(self) };
            
      ^ self
   }
    
   wide padRight(int length)
       = self.padRight($32, length);
    
   wide padRight(char ch, int length)
   {
      int currentLength := self.Length;

      if (currentLength < length)
         { ^ self.add(WideString.fill(length - currentLength, ch)) };
            
      ^ self
   }

   bool startingWith(wide s)
   {
      int n := self.indexOf(0, s);
        
      ^ 0 == n
   }
        
   bool endingWith(wide s)
   {
      int l1 := self.Length;
      int l2 := s.Length;

      int n := self.indexOf(l1 - l2, s);
        
      ^ n != -1
   }
        
   bool containing(wide s)
   {
      int l1 := self.Length;
      int l2 := s.Length;

      int n := self.indexOf(0, s);
        
      ^ n != -1
   }
    
   wide trimLeft(char ch)
   {
      int  index := 0;
      int  len := self.Length;
      char current := $0;
      int  chLen := ch.Length;
        
      while(index < len)
      {
         current := self[index];
            
         ifnot(current == ch)
         { 
            if (index == 0)
            { 
               ^ self 
            }
            else
            { 
               ^ self.delete(0, index) 
            }
         };
            
         index += chLen
      };
        
      ^ emptyWideString
   }
    
   wide trimLeft()
      = self.trimLeft($32);
       
   wide trimRight(char ch)
   {
      int  len := self.Length;
      int  index := len - 1;
      int  prev := len;
      char current := $0;
      int  chLen := ch.Length;
        
      while (index >= 0)
      {
         current := self[index];
            
         ifnot (current == ch)
            { ^ self.delete(prev,len - prev) };
            
         prev := index;
         index -= chLen
      };
        
      ^ emptyWideString
   }
    
   wide trimRight()
      = self.trimRight($32);
        
   wide trim(char ch)
      = self.trimLeft(ch).trimRight(ch);
    
   wide trim()
      = self.trimLeft($32).trimRight($32);

   wide replace(wide replacee, wide replacer)
   {
      wide   target := emptyWideString;
      int    start := 0;
      int    end := self.indexOf(start, replacee);
      int    replaceeLen := replacee.Length;
        
      while (end != -1)
      {
         target := target
                     .add(self.Substring(start, end - start))
                     .add(replacer);
            
         start := end + replaceeLen;
            
         end := self.indexOf(start, replacee)
      };
    
      if(start != 0)
      {
         ^ target.add(self.Substring(start))
      };

      ^ self
   }   

   wide replaceFirst(wide replacee, wide replacer)
   {
      int replaceeLen := replacee.Length;
      int i := self.indexOf(0, replacee);
        
      if (i != -1)
      { 
         ^ self.delete(i, replaceeLen).insert(i, replacee)
      }
      else
      {
         ^ self
      }    
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
        for (int i := 0; i < length; i += 1)
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
//    T[] (int index, int length)
//    {
//        ^ new Array<T>(self, index, length)
//    }
//}

// --- stringWeakOp ---
public extension stringWeakOp
{
   fillString(s, int counter)
   {
      for (int i := 0; i < counter; i += 1)
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
    
   writePaddingLeft(line, int width)
   {
      var paddedStr := line.padLeft($32, width);

      self.write(paddedStr);
            
      ^ self
   }
    
   writePaddingRight(line, int width)
   {
      var paddedStr := line.padRight($32, width);

      self.write(paddedStr);
            
      ^ self
   }
}

public extension threadObjectOp
{
   enterCriticalSection() : external(system'core_routines'__enterCritical);

   leaveCriticalSection() : external(system'core_routines'__leaveCritical);
}

//// --- intArrayOp ---
//
//public extension intArrayOp : Array<int>
//{
//    exchange(int i1, int i2)
//    {
//        int n1 := self[i1];
//        int n2 := self[i2];
//
//        self[i1] := n2;
//        self[i2] := n1;
//    }
//
//    populate(Func<int,int> f)
//    {
//        int i := 0;
//        int l := self.Length;
//        
//        while (i < l)
//        {
//            self[i] := f(i);
//            
//            i := i + 1
//        };
//        
//        ^ self
//    }
//    
//    populate(Func1 f)
//        = self.populate:(int n){ ^cast int(f(n)) };
//}
//
//public extension arrayOp<T> : Array<T>
//{
//    T[] Subarray(int index, int length)
//    {
//        ^ new Array<T>(self, index, length)
//    }
//}
//
//// --- intmatrixOp ---
//
//public extension intMatrixOp : IntMatrix
//{
//    populate(Func<int,int,int> fn)
//    {
//        int n := 0;
//        int m := 0;
//        self.readLengthsTo(ref n, ref m);
//        
//        int i := 0;
//        int j := 0;
//        while (i < n)
//        {
//            j := 0;
//            while (j < m)
//            {
//                int v := fn(i, j);
//                
//                self.write(i, j, v);
//                
//                j := j + 1
//            };
//            
//            i := i + 1
//        };
//        
//        ^ self
//    }
//}
