import system'math;

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

   public singleton abs
   {
      real function(real loperand)
         = loperand.Absolute;

      int function(int loperand)
         = loperand.Absolute;
            
      byte function(byte loperand)
         = loperand.Absolute;
            
      short function(short loperand)
         = loperand.Absolute;
            
      long function(long loperand)
         = loperand.Absolute;      
   }

   public singleton truncate
   {
      real function(real operand, int precision)
         = operand.truncate(precision);
   }

   public singleton frac
   {
      real function(real operand)
         = operand.frac();
   }
   
   public singleton floor
   {
      real function(real operand)
         = operand.floor();
   }

   public singleton ceil
   {
      real function(real operand)
         = operand.ceil();
   }   

   public singleton power
   {
      function(loperand, roperand)
         = cast real(loperand).power(roperand);
        
      real function(real loperand, int roperand)
         = loperand.power(roperand);
            
      real function(real loperand, real roperand)
         = loperand.power(roperand);
            
      int function(int loperand, int roperand)
         = loperand.power(roperand);
            
      byte function(byte loperand, int roperand)
         = loperand.power(roperand);
            
      short function(short loperand, int roperand)
         = loperand.power(roperand);
            
      long function(long loperand, long roperand)
         = loperand.power(roperand);
   }

   public singleton sqr
   {
      real function(real operand)
         = operand.sqr();
   }   

   public singleton sqrt
   {
      real function(real operand)
         = operand.sqrt();
   }   

   public singleton exp
   {
      real function(real operand)
         = operand.exp();
   }   

   public singleton ln
   {
      real function(real operand)
         = operand.ln();
   }   

   public singleton sin
   {
      real function(real operand)
         = operand.sin();
   }   

   public singleton cos
   {
      real function(real operand)
         = operand.cos();
   }   

   public singleton tan
   {
      real function(real operand)
         = operand.tan();
   }   

   public singleton arctan
   {
      real function(real operand)
         = operand.arctan();
   }   

   public singleton arcsin
   {
      real function(real operand)
         = operand.arcsin();
   }   

   public singleton arccos
   {
      real function(real operand)
         = operand.arccos();
   }   

   public singleton log2
   {
      real function(real operand)
         = operand.log2();
   }   

   public singleton log10
   {
      real function(real operand)
         = operand.log10();
   }   
}