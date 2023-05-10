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
   }

   public extension mathOp
   {
      mod(n)
         = mathController.mod(self, n);
    
      frac()
         = cast real(self).frac();
    
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