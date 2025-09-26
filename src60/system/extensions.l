// --- objectOp ---

public extension objectOp
{
   string __getClassName() : info("Returns the object class name")
      = __className;

   __getClass() : external(system'core_routines'__object_class),
                  info("Returns the object class");

   __getParentClass() : external(system'core_routines'__object_parentclass),
                  info("Returns the parent class");

   __getParent() : external(system'core_routines'__object_parent),
                  info("Returns the parent of the specified class");

   bool safeEqual(o)
         : info("Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible")
      = self.equal(o) \ back(false);

   bool isEmpty() : info("Returns true if the object length is 0")
   {
      int len := self.Length;
        
      ^ len == 0;
   }
        
   bool isNonempty() : info("Returns true if the object length is not 0")
   {
      int len := self.Length;
        
      ^ len != 0;
   }

   bool isNil()
         : info("Returns true if the object is nil")
      = nil == self;       

   doWith(action) : info("Invokes <b>action</b> function passing the object as an argument")
      = action(self);

   bool instanceOf(type) : external(system'core_routines'__object_instanceOf),
                  info("Checks if the object is an instance of type");

   bool subsetOf(type) : info("Returns true if the object is an instance of a child of <b>type</b>")
   {
      var class := self.__getClass();
      while (class != nil) {
         if (class == type) {
            ^ true
         };
       
         class := class.__getParent()  
      };

      ^ false
   }
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
   int Hashcode : info("Returns the hash code of the byte")
   {
      get()
      {
         int val := self;
         int retVal := PrimitiveIntOperations.getHashcode(val);

         ^ retVal
      }
   }

   get byte Absolute() : info("Returns the absolute value of the byte")
   {
      byte n := self;
    
      if (n < 0)
         { n := -n };
    
      ^ n
   }

   byte power(int y) : info("returns the power of the byte raised to <b>y</b>")
   {
      byte x := self;
      byte r := 1;
        
      for (int i := 0; i < y; i += 1)
      {
         r := r * x
      };
        
      ^ new ByteNumber(r)
   }

   byte sqr() : info("returns the power of the byte raised to 2")
      = self * self;

   byte sqrt() : info("returns the square root of the byte")
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

   byte mod(operand) : info("returns the remainder of an integer division")
   {
      var retVal := self / operand * operand;
        
      ^ ByteConvertor.convert(retVal)
   }

   bool isOdd() : info("Returns true if the byte is odd")
      = self.mod(2) != 0;
        
   bool isEven() : info("Returns true if the byte is even")
      = self.mod(2) == 0;
        
   bool isZero() : info("Returns true if the byte is zero")
      = self.equal(0);
        
   bool isPositive() : info("Returns true if the byte is positive")
      = self > 0;
        
   bool isNegative() : info("Returns true if the byte is negative")
      = self < 0;
        
   bool isNonnegative() : info("Returns true if the byte is not negative")
      = self >= 0;
}

// --- shortOp ---

public extension shortOp : ShortNumber
{
   int Hashcode : info("Returns the hash code of the short integer")
   {
      get()
      {
         int val := self;
         int retVal := PrimitiveIntOperations.getHashcode(val);

         ^ retVal
      }
   }

   get short Absolute() : info("Returns the absolute value of the short integer")
   {
      short n := self;
    
      if (n < 0)
         { n := -n };
    
      ^ n
   }

   short power(int y) : info("returns the power of the short integer raised to <b>y</b>")
   {
      short x := self;
      short r := 1;
        
      for (int i := 0; i < y; i += 1)
      {
         r := r * x
      };
        
      ^ new ShortNumber(r)
   }

   short sqr() : info("returns the power of the short integer raised to 2")
      = self * self;

   short sqrt() : info("returns the square root of the short integer")
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

   short mod(operand) : info("returns the remainder of an integer division")
   {
      var retVal := self / operand * operand;
        
      ^ ShortConvertor.convert(retVal)
   }    

   bool isOdd() : info("Returns true if the short integer is odd")
      = self.mod(2) != 0;
        
   bool isEven() : info("Returns true if the short integer is even")
      = self.mod(2) == 0;
        
   bool isZero() : info("Returns true if the short integer is zero")
      = self.equal(0);
        
   bool isPositive() : info("Returns true if the short integer is positive")
      = self > 0;
        
   bool isNegative() : info("Returns true if the short integer is negative")
      = self < 0;
        
   bool isNonnegative() : info("Returns true if the short integer is not negative")
      = self >= 0;

   get byte High() : info("Returns a high byte of the short integer")
   {
      short w := self;
      w := w & 0FF00h;
      w := w.shiftRight(8);
        
      ^ ByteConvertor.convert(w)
   }
        
   get byte Low() : info("Returns a low byte of the short integer")
   {
      short w := self;
      w := w & 0FFh;
        
      ^ ByteConvertor.convert(w)
   }
}

// --- intOp ---

public extension intOp : IntNumber
{
   int Hashcode : info("Returns the hash code of the integer")
   {
      get()
      {
         int retVal := PrimitiveIntOperations.getHashcode(self);

         ^ retVal
      }
   }

   get int Absolute() : info("Returns the absolute value of the integer")
   {
      int n := self;
    
      if (n < 0)
         { n := -n };
    
      ^ n
   }

   int power(int y) : info("returns the power of the integer raised to <b>y</b>")
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

   int sqr() : info("returns the power of the integer raised to 2")
      = self * self;

   int sqrt() : info("returns the square root of the integer")
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

   real realDiv(n) : info("returns the result of a real division")
      = new RealNumber(self).divide(n);

   int mod(int operand) : info("returns the remainder of an integer division")
   {
      int retVal := self / operand * operand;

      retVal := self - retVal;
        
      ^ retVal
   }

   int mod(o) : info("returns the remainder of an integer division")
   {
      int retVal := self.mod(cast int(o));
        
      ^ retVal
   }

   bool isOdd() : info("Returns true if the integer is odd")
      = self.mod(2) != 0;
        
   bool isEven() : info("Returns true if the integer is even")
      = self.mod(2) == 0;
        
   bool isZero() : info("Returns true if the integer is zero")
      = self.equal(0);
        
   bool isPositive() : info("Returns true if the integer is positive")
      = self > 0;
        
   bool isNegative() : info("Returns true if the integer is negative")
      = self < 0;
        
   bool isNonnegative() : info("Returns true if the integer is not negative")
      = self >= 0;

   get short High() : info("Returns a high word of the integer")
   {
      int w := self;
      w := w & 0FFFF0000h;
      w := w.shiftRight(8);
        
      ^ ShortConvertor.convert(w)
   }
        
   get short Low() : info("Returns a low word of the integer")
   {
      int w := self;
      w := w & 0FFFFh;
        
      ^ ShortConvertor.convert(w)
   }
}

// --- uintOp ---

public extension uintOp : UIntNumber
{
   int Hashcode : info("Returns the hash code of the unsigned integer")
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int retVal := PrimitiveIntOperations.getHashcode(self);

         ^ retVal
      }
   }

   uint sqr() : info("returns the power of the unsigned integer raised to 2")
      = self * self;

   uint sqrt() : info("returns the square root of the unsigned integer")
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

   uint mod(uint operand) : info("returns the remainder of an integer division")
   {
      uint retVal := self / operand * operand;

      retVal := self - retVal;
        
      ^ retVal
   }

   uint mod(int operand) : info("returns the remainder of an integer division")
   {
      uint uoperand := operand;
      uint retVal := self / uoperand * uoperand;

      retVal := self - retVal;
        
      ^ retVal
   }

   uint mod(operand) : info("returns the remainder of an integer division")
      = self.mod(cast uint(operand));

   real realDiv(n) : info("returns the result of a real division")
      = new RealNumber(self).divide(n);

   bool isOdd() : info("Returns true if the integer is odd")
      = self.mod(2) != 0;
        
   bool isEven() : info("Returns true if the integer is even")
      = self.mod(2) == 0;
        
   bool isZero() : info("Returns true if the integer is zero")
      = self.equal(0);
}

// --- longOp ---

public extension longOp : LongNumber
{   
   int Hashcode : info("Returns the hash code of the long integer")
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int retVal := PrimitiveLongOperations.getHashcode(self);

         ^ retVal
      }
   }

   get long Absolute() : info("Returns the absolute value of the long integer")
   {
      long n := self;

      if (n < 0l)
         { n := n * -1l };
                
      ^ n
   }

   long power(long y) : info("returns the power of the long integer raised to <b>y</b>")
   {
      long x := self;
      long r := 1;
        
      for(long i := 0l; i < y; i += 1l)
      {
         r := r * x;
      };
        
      ^ new LongNumber(r)
   }
    
   long power(int y) : info("returns the power of the long integer raised to <b>y</b>")
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

   long sqr() : info("returns the power of the long integer raised to 2")
      = self * self;

   long sqrt() : info("returns the square root of the long integer")
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

   real realDiv(n) : info("returns the result of a real division")
      = new RealNumber(self).divide(n);

   long mod(long operand) : info("returns the remainder of an long integer division")
   {
      long retVal := self / operand * operand;

      retVal := self - retVal;
        
      ^ retVal
   }    

   long mod(n) : info("returns the remainder of an long integer division")
   {
      long x := self;
      long y := cast long(n);
        
      long z := x / y;
      z := z * y;
        
      ^ new LongNumber(x - z)
   }

   bool isOdd() : info("Returns true if the long integer is odd")
      = self.mod(2l) != 0l;
        
   bool isEven() : info("Returns true if the long integer is even")
      = self.mod(2l) == 0l;
        
   bool isZero() : info("Returns true if the long integer is zero")
      = self.equal(0l);
        
   bool isPositive() : info("Returns true if the long integer is positive")
      = self > 0l;
        
   bool isNegative() : info("Returns true if the long integer is negative")
      = self < 0l;
        
   bool isNonnegative() : info("Returns true if the long integer is not negative")
      = self <= 0l;

   get int High() : info("Returns a high integer of the long integer")
   {
      long w := self;
      w := w & -4294967296l;
      w := w.shiftRight(32);
        
      ^ IntConvertor.convert(w)
   }

   get int Low() : info("Returns a low integer of the long integer")
   {
      long w := self;
      w := w & 4294967295l;
        
      ^ IntConvertor.convert(w)
   }
}

// --- realOp ---

public extension realOp : RealNumber
{
   int Hashcode : info("Returns the hash code of the real number")
   {
      // Reads the object hash code into the output parameter
      get()
      {
         int retVal := PrimitiveRealOperations.getHashcode(self);

         ^ retVal
      }
   }

   real power(real y) : info("returns the power of the real number raised to <b>y</b>")
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

   real power(int y) : info("returns the power of the real number raised to <b>y</b>")
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
            
         x *= y;
            
         x := x.exp();
            
         if (y.isOdd())
            { x := x * -1.0r }
      }
      else
      {
         x := x.ln();
            
         x *= y;
            
         x := x.exp()
      };
        
      ^ x
   }

   real sqr() : info("returns the power of the real number raised to 2")
      = self * self;

   real sqrt() : info("returns the square root of the real number")
   {
      PrimitiveRealOperations.sqrt(self, out real retVal);

      ^ retVal
   }

   real exp() : info("Returns the result of e raised to the power of the real number")
   {
      PrimitiveRealOperations.exp(self, out real retVal);

      ^ retVal
   }

   real ln() : info("returns the natural logarithm of the real number")
   {
      PrimitiveRealOperations.ln(self, out real retVal);

      ^ retVal
   }

   real sin() : info("returns the trigonometric sines of the real number")
   {
      PrimitiveRealOperations.sin(self, out real retVal);

      ^ retVal
   }

   real cos() : info("returns the trigonometric cosines of the real number")
   {
      PrimitiveRealOperations.cos(self, out real retVal);

      ^ retVal
   }

   real tan() : info("returns the trigonometric tangents of the real number")
   {
      PrimitiveRealOperations.sin(self, out real sinVal);
      PrimitiveRealOperations.cos(self, out real cosVal);
        
      real n := sinVal / cosVal;
        
      ^ n
   }

   real arctan() : info("returns the trigonometric inverse tangents (arctan) of the real number")
   {
      PrimitiveRealOperations.arctan(self, out real retVal);

      ^ retVal
   }

   real arcsin() : info("returns the trigonometric inverse sines (arcsin) of the real number")
   {
      real n := self;
        
      real retVal := n * n;    
      retVal := 1.0r - retVal;
      PrimitiveRealOperations.sqrt(retVal, out real arg);
        
      arg := n / arg;
      PrimitiveRealOperations.arctan(arg, out retVal);
        
      ^ retVal
   }
            
   real arccos() : info("returns the trigonometric inverse cosines (arccos) of the real number")
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

   real log2() : info("returns the base 2 logarithm of the real number")
   {
      real x := self.ln();
      real y := 2.0.ln();
            
      ^ x / y
   }
            
   real log10() : info("returns the base 10 logarithm of the real number")
   {
      real x := self.ln();
      real y := 10.0.ln();
            
      ^ x / y
   }      

   get real Integer() : info("Returns the integer part of the real number")
   {
      PrimitiveRealOperations.trunc(self, out real retVal);

      ^ retVal
   }

   get int IntegerInt() : info("Returns the integer part as an integer of the real number")
      = IntConvertor.convert(self.Integer);

   get real Rounded() : info("Rounds real number")
   {
      PrimitiveRealOperations.round(self, out real retVal);

      ^ retVal
   }

   get int RoundedInt() : info("Rounds real number as an integer")
      = IntConvertor.convert(self.Rounded);

   get real Absolute() : info("Returns the absolute value of the real number")
   {
      PrimitiveRealOperations.abs(self, out real retVal);

      ^ retVal  
   }

   get real Reciprocal() : info("Returns the reciprocal (1/x) of the real number")
   {
      real value := self;
        
      value := 1.0r / value;
        
      ^ value
   }    

   real frac() : info("Returns the fractional part of the real number")
   {
      real retVal := self.Absolute;
      
      ^ retVal - retVal.Integer
   }

   real ceil() : info("Returns the smallest integer value not less than the real number")
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

   real floor() : info("Round numbers down to the nearest integer")
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

   real round(int precision) : info("Returns the real number rounded to <b>precision</b> digits after the dot")
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
        
      int c := IntConvertor.convert(r.Rounded);
    
      a := c.realDiv(b);
    
      ^ neg ? (a * -1.0r) : a
   }    

   real truncate(int precision) : info("Returns the real number truncated to <b>precision</b> digits after the dot")
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

            
   get real Radian() : info("Returns the real number as a radian")
   {
      real value := self;
        
      value := value * Pi_value;
      value := value / 180.0r;
        
      ^ value
   }
            
   get real Degree() : info("Returns the real number as a degree")
   {
      real value := self;
        
      value := value * 180.0r;
      value := value / Pi_value;
        
      ^ value
   }

   bool isZero() : info("Returns true if the real number is zero")
      = self.equal(0.0);
        
   bool isPositive() : info("Returns true if the real number is positive")
      = self > 0.0;
        
   bool isNegative() : info("Returns true if the real number is negative")
      = self < 0.0;
        
   bool isNonnegative() : info("Returns true if the real number is not negative")
      = self >= 0.0;
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

      ^ EmptyString
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
            
         if:not(current == ch)
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
        
      ^ EmptyString
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
            
         if:not (current == ch)
            { ^ self.delete(prev,len - prev) };
            
         prev := index;
         index -= chLen
      };
        
      ^ EmptyString
   }
    
   string trimRight()
      = self.trimRight($32);
        
   string trim(char ch)
      = self.trimLeft(ch).trimRight(ch);
    
   string trim()
      = self.trimLeft($32).trimRight($32);

   string replace(string replacee, string replacer)
   {
      string target := EmptyString;
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
         $9  : { ^ true }
         $32 : { ^ true }
         $10 : { ^ true }
         $13 : { ^ true }
         $12 : { ^ true };
            
      ^ false
   }

   bool isDigit()
   {
      int ch := IntConvertor.convert(self);
        
      ^ (ch >= 30h) && (ch <= 39h)
   }

   bool isLetter()
   {
      int ch := IntConvertor.convert(self);
        
      if (ch >= 41h && ch <= 5Ah)
         { ^ true };
        
      if (ch >= 61h && ch <= 7Ah)
         { ^ true };
        
      if (ch >= 370h && ch <= 3FBh)
         { ^ true };
            
      if (ch >= 400h && ch <= 4FFh)
         { ^ true };
            
      ^ false
   }
    
   bool isLToken()
   {
      int ch := IntConvertor.convert(self);
        
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
            
         if:not(current == ch)
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
            
         if:not (current == ch)
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
