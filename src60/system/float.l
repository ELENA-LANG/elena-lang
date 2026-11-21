// --- FloatNumber ---

public sealed const struct FloatNumber : BaseNumber,
      info("A 32bit float numeric value")
{
   embeddable __float _value[4];
   
   constructor(float r)
            : info("Creates the object with specified value")
      = r;
   
   bool equal(float n)
      = self == n;
   
   bool less(float n)
      = self < n;
   
   float add(float n)
      = self + n;
      
   float subtract(float n)
      = self - n;
      
   float multiply(float n)
      = self * n;
      
   float divide(float n)
      = self / n;
}

// --- RealNumber ---

public sealed const struct RealNumber : BaseNumber,
      info("A 64bit float numeric value")
{
   embeddable __float _value[8];

   static real MinValue
            : info("Returns the minimal value")
      = -1.7976931348623157e+308r;

   static real MaxValue
            : info("Returns the maximal value")
      = 1.7976931348623157e+308r;

   static real Default
            : info("Returns the default value (0)")
      = 0.0;

   static real NegativeInfinity 
            : info("Returns the negative infinity")
      = -1.0 / 0.0;   

   static real PositiveInfinity 
            : info("Returns the positive infinity")
      = 1.0 / 0.0;   

   static real NaN 
            : info("Returns the positive infinity")
      = 0.0 / 0.0;   

   static real Pi
            : info("Returns Pi value")
      = Pi_value;

   constructor(int n)
            : info("Converts the value")
      = RealConvertor.convert(n);
   
   constructor(byte n)
            : info("Converts the value")
      = RealConvertor.convert(n);
   
   constructor(short n)
            : info("Converts the value")
      = RealConvertor.convert(n);

   constructor()
            : info("Creates the object with a default value (0)")
      = 0.0;

   constructor(real r)
            : info("Creates the object with specified value")
      = r;
   
   constructor(uint n)
            : info("Creates the object with specified value")
      = RealConvertor.convert(n);

   constructor(long n)
            : info("Creates the object with specified value")
      = RealConvertor.convert(n);

   bool equal(o)
            : info("Compares the value;<br/>If no appropriate method overloading is found, an argument <b>o</b> is converted to the RealNumber type when it is possible and compared; otherwise returns false")
   {
      if (o; is real r)
      {
         ^ _value == r
      };

       ^ false
   }

   bool less(n)
            : info("Returns the true if the object is less than an argument;<br/>If no appropriate method overloading is found, an argument <b>o</b> is converted to the LongNumber type when it is possible and compared")
      <= less(cast real(n));

   bool equal(real n)
            : info("Compares the value")
      = _value == n;
   
   bool less(real n)
            : info("Compares the value")
      = _value < n;

   add(n)
            : info("Returns the sum;<br/>If no appropriate method overloading is found, an argument <b>o</b> is converted to the RealNumber type")
      <= add(cast real(n));

   subtract(n)
            : info("Returns the difference;<br/>If no appropriate method overloading is found, an argument <b>o</b> is converted to the RealNumber type")
      <= subtract(cast real(n));

   multiply(n)
            : info("Returns the product;<br/>If no appropriate method overloading is found, an argument <b>o</b> is converted to the RealNumber type")
      <= multiply(cast real(n));

   divide(n)
            : info("Returns the result of integer division;<br/>If no appropriate method overloading is found, an argument <b>o</b> is converted to the RealNumber type")
      <= divide(cast real(n));

   real add(real n)
            : info("Returns the sum")
      = _value + n;

   real subtract(real n)
            : info("Returns the difference")
      = _value - n;

   real multiply(real n)
            : info("Returns the product")
      = _value * n;

   real divide(real n)
            : info("Returns the result of integer division")
      = _value / n;

   bool equal(int n)
            : info("Returns the result of integer division")
      = self.equal(new RealNumber(n));

   bool equal(byte n)
            : info("Returns the result of integer division")
      = self.equal(new RealNumber(n));

   bool equal(short n)
            : info("Returns the result of integer division")
      = self.equal(new RealNumber(n));

   bool equal(long n)
            : info("Returns the result of integer division")
      = self.equal(new RealNumber(n));

   bool less(int n)
            : info("Returns the result of integer division")
      = self.less(new RealNumber(n));

   bool less(byte n)
            : info("Returns the result of integer division")
      = self.less(new RealNumber(n));

   bool less(short n)
            : info("Returns the result of integer division")
      = self.less(new RealNumber(n));

   bool less(long n)
            : info("Returns the result of integer division")
      = self.less(new RealNumber(n));

   real add(int n)
            : info("Returns the sum of numbers")
      = self.add(new RealNumber(n));

   real add(long n)
            : info("Returns the sum of numbers")
      = self.add(new RealNumber(n));

   real add(byte n)
            : info("Returns the sum of numbers")
      = self.add(new RealNumber(n));

   real add(short n)
            : info("Returns the sum of numbers")
      = self.add(new RealNumber(n));

   real subtract(int n)
            : info("Returns the difference of numbers")
      = self.subtract(new RealNumber(n));

   real subtract(long n)
            : info("Returns the difference of numbers")
      = self.subtract(new RealNumber(n));

   real subtract(byte n)
            : info("Returns the difference of numbers")
      = self.subtract(new RealNumber(n));

   real subtract(short n)
            : info("Returns the difference of numbers")
      = self.subtract(new RealNumber(n));

   real multiply(int n)
            : info("Returns the product of numbers")
      = self.multiply(new RealNumber(n));

   real multiply(long n)
            : info("Returns the product of numbers")
      = self.multiply(new RealNumber(n));

   real multiply(byte n)
            : info("Returns the product of numbers")
      = self.multiply(new RealNumber(n));

   real multiply(short n)
            : info("Returns the product of numbers")
      = self.multiply(new RealNumber(n));

   real divide(int n)
            : info("Returns the result of the division")
      = self.divide(new RealNumber(n));

   real divide(long n)
            : info("Returns the result of the division")
      = self.divide(new RealNumber(n));

   real divide(byte n)
            : info("Returns the result of the division")
      = self.divide(new RealNumber(n));

   real divide(short n)
            : info("Returns the result of the division")
      = self.divide(new RealNumber(n));

   string toPrintable()
            : info("Returns the literal presentation")
      = StringConvertor.convert(self);

   real clone()
            : info("Clones the value")
      = new RealNumber(self);

   get real Negative()
            : info("Negates the value")
      = 0.0 - _value;
}

public real Pi_value : info("Pi constant") = PrimitiveRealOperations.getPi();

public real E_value : info("E constant") = PrimitiveRealOperations.getE();
