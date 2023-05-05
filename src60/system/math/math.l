import system;

namespace math
{
   // --- mathController ---    
   singleton mathController
   {
      mod(int n, object m)
         = n.mod:m;      

      mod(BaseVariable v, object m)
         = (*v).mod(m);
   }

   public extension mathOp
   {
      mod(n)
         = mathController.mod(self, n);
   }      
}