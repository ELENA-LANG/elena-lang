import system;

namespace math
{
   // --- mathController ---    
   singleton MathController
   {
      absolute(byte n)
         = n.Absolute;
            
      absolute(short n)
         = n.Absolute;
            
      absolute(int n)
         = n.Absolute;
            
      absolute(long n)
         = n.Absolute;
            
      absolute(real n)
         = n.Absolute;
            
      mod(byte n, object m)
         = n.mod(m);
            
      mod(short n, object m)
         = n.mod(m);
            
      mod(int n, object m)
         = n.mod(m);      

      mod(BaseVariable v, object m)
         = (*v).mod(m);

      real pi()
         = Pi_value;

      real e()
         = 1.0.exp();
                
      Radian
         = RealConvertor.convert(self).Radian;
                
      Degree
         = RealConvertor.convert(self).Degree;
   }

   public extension mathOp
   {            
      real Rounded : info("Returns a number rounded to the closest integer number as a real number")
         = cast real(self).Rounded;
      
      int RoundedInt : info("Returns a number rounded to the closest integer number as an integer")
         = IntConvertor.convert(cast real(self).Rounded);
      
      real Integer : info("Returns an integer part of the number as a real number")
         = cast real(self).Integer;
      
      int IntegerInt : info("Returns an integer part of the number as an integer")
         = IntConvertor.convert(cast real(self).Integer);

      mod(n) : info("Returns the remainder of an integer division")
         = MathController.mod(self, n);
    
      real frac() : info("Returns the fractional part of the real number")
         = cast real(self).frac();
    
      real exp() : info("Returns the result of e raised to the power of the number")
         = cast real(self).exp();
    
      real ln() : info("Returns the natural logarithm of the number")
         = cast real(self).ln();
    
      real log10() : info("Returns the base 10 logarithm of the real number")
      {
         real x := mathOp.ln(self).ln();
         real y := mathOp.ln(10);
            
         ^ x / y
      }

      real sin() : info("Returns the trigonometric sines of the number")
         = cast real(self).sin();
    
      real cos() : info("Returns the trigonometric cosines of the number")
         = cast real(self).cos();
    
      real tan() : info("Returns the trigonometric tangents of the number")
         = cast real(self).tan();
    
      real arctan() : info("Returns the trigonometric inverse tangents (arctan) of the number")
         = cast real(self).arctan();
                
      real arcsin() : info("Returns the trigonometric inverse sines (arcsin) of the number")
         = cast real(self).arcsin();
                        
      real arccos() : info("Returns the trigonometric inverse cosines (arccos) of the number")
         = cast real(self).arccos();
   }      

   public real Pi : info("A Pi number")
      = MathController.pi();

   public real E : info("Euler's number")
      = MathController.e();
}