import system;

namespace math
{
   // --- mathController ---    
   singleton mathController
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
         = n.mod:m;
            
      mod(short n, object m)
         = n.mod:m;
            
      mod(int n, object m)
         = n.mod:m;      

      mod(BaseVariable v, object m)
         = (*v).mod(m);

      real pi()
         = Pi_value;

      real e()
         = 1.0.exp();
                
      Radian
         = realConvertor.convert(self).Radian;
                
      Degree
         = realConvertor.convert(self).Degree;
   }

   public extension mathOp
   {            
      Rounded
         = cast real(self).Rounded;
      
      RoundedInt
         = intConvertor.convert(cast real(self).Rounded);
      
      Integer
         = cast real(self).Integer;
      
      IntegerInt
         = intConvertor.convert(cast real(self).Integer);

      mod(n)
         = mathController.mod(self, n);
    
      frac()
         = cast real(self).frac();
    
      ln()
         = cast real(self).ln();
    
      sin()
         = cast real(self).sin();
    
      cos()
         = cast real(self).cos();
    
      tan()
         = cast real(self).tan();
    
      arctan()
         = cast real(self).arctan();
                
      arcsin()
         = cast real(self).arcsin();
                        
      arccos()
         = cast real(self).arccos();
   }      
}