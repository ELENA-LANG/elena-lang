namespace math
{
   public singleton modulo
   {
      function(loperand, roperand)
         = cast int(loperand).mod(roperand);
        
      int function(int loperand, int roperand)
         = loperand.mod(roperand);
            
      byte function(byte loperand, byte roperand)
         = loperand.mod(roperand);
            
      short function(short loperand, short roperand)
         = loperand.mod(roperand);
            
      uint function(uint loperand, uint roperand)
         = cast int(loperand).mod(roperand);
            
      long function(long loperand, long roperand)
         = loperand.mod(roperand);
   }   
}