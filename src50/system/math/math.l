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
            
        absolute(n)
            <= absolute(n.Value);
            
        mod(byte n, object m)
            = n.mod:m;
            
        mod(short n, object m)
            = n.mod:m;
            
        mod(int n, object m)
            = n.mod:m;
            
        mod(long n, object m)
            = n.mod:m;
    
        mod(n, m)
            <= mod(n.Value, m);
                    
        bool isOdd(byte n)
            = n.isOdd();
            
        bool isOdd(short n)
            = n.isOdd();
            
        bool isOdd(int n)
            = n.isOdd();
            
        bool isOdd(long n)
            = n.isOdd();
    
        bool isOdd(n)
            <= isOdd(n.Value);
                            
        bool isEven(byte n)
            = n.isEven();
            
        bool isEven(short n)
            = n.isEven();
            
        bool isEven(int n)
            = n.isEven();
            
        bool isEven(long n)
            = n.isEven();
    
        bool isEven(n)
            <= isEven(n.Value);
                
        bool isZero(byte n)
            = n.isZero();
            
        bool isZero(short n)
            = n.isZero();
            
        bool isZero(int n)
            = n.isZero();
            
        bool isZero(long n)
            = n.isZero();
            
        bool isZero(real n)
            = n.isZero();
            
        bool isZero(n)
            <= isZero(n.Value);
            
        bool isPositive(byte n)
            = n.isPositive();
            
        bool isPositive(short n)
            = n.isPositive();
            
        bool isPositive(int n)
            = n.isPositive();
            
        bool isPositive(long n)
            = n.isPositive();
            
        bool isPositive(real n)
            = n.isPositive();
            
        bool isPositive(n)
            <= isPositive(n.Value);
            
        bool isNegative(byte n)
            = n.isNegative();
            
        bool isNegative(short n)
            = n.isNegative();
            
        bool isNegative(int n)
            = n.isNegative();
            
        bool isNegative(long n)
            = n.isNegative();
            
        bool isNegative(real n)
            = n.isNegative();
    
        bool isNegative(n)
            <= isNegative(n.Value);
                    
        bool isNonnegative(byte n)
            = n.isNonnegative();
            
        bool isNonnegative(short n)
            = n.isNonnegative();
            
        bool isNonnegative(int n)
            = n.isNonnegative();
            
        bool isNonnegative(long n)
            = n.isNonnegative();
            
        bool isNonnegative(real n)
            = n.isNonnegative();
    
        bool isNonnegative(n)
            <= isNonnegative(n.Value);
                    
        real power(real n, int m)
            = n.power(m);
            
        real power(real n, real m)
            = n.power(m);        
            
        power(byte n, int m)
            = n.power(m);       
            
        power(short n, int m)
            = n.power(m);        
            
        power(int n, int m)
            = n.power(m);      
            
        power(long n, int m)
            = n.power(m);
            
        power(object n, object m)
            <= power(n.Value, m);
            
        sqr(byte n)
            = n.sqr();
            
        sqr(short n)
            = n.sqr();
            
        sqr(int n)
            = n.sqr();
            
        sqr(long n)
            = n.sqr();
            
        sqr(real n)
            = n.sqr();
            
        sqr(n)
            <= sqr(n.Value);
    }
    
    // --- mathOp ---
    
    public extension mathOp
    {
        Absolute
            = mathController.absolute:self;
    
        mod(n)
            = mathController.mod(self, n);
    
        bool isOdd()
            = mathController.isOdd(self);
    
        bool isEven()
            = mathController.isEven(self);
    
        bool isZero()
            = mathController.isZero(self);
    
        bool isPositive()
            = mathController.isPositive(self);
    
        bool isNegative()
            = mathController.isNegative(self);
    
        bool isNonnegative()
            = mathController.isNonnegative(self);

        real floor()
        {
            real x := cast real(self);
            
            if (x >= 0)
            {
                ^ x.Integer
            }
            else
            {
                real y := x.Integer;
                
                ^ (y == x).iif(y,y-1)
            }
        }
    
        real ceil()
        {
            real x := cast real(self);
            
            if (x < 0)
            {
                ^ x.Integer
            }
            else
            {
                real y := x.Integer;
                
                ^ (y == x).iif(y,y+1)
            }
        }
    
        real exp()
            = cast real(self).exp();
        
        real ln()
            = cast real(self).ln();
      
        real log2()
        {
            real x := mathOp.ln(self);
            real y := mathOp.ln(2);
            
            ^ x / y
        }
            
        real log10()
        {
            real x := mathOp.ln(self).ln();
            real y := mathOp.ln(10);
            
            ^ x / y
        }
        
        sqrt()
            = cast real(self).sqrt();
    
        power(p)
            = mathController.power(self, p);
    
        sqr()
            = mathController.sqr(self);
            
        Rounded
            = cast real(self).Rounded;
    
        RoundedInt
            = intConvertor.convert(cast real(self).Rounded);
    
        Integer
            = cast real(self).Integer;
    
        IntegerInt
            = intConvertor.convert(cast real(self).Integer);
    
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
                
        Radian
            = cast real(self).Radian;
                
        Degree
            = cast real(self).Degree;
                
        Reciprocal
            = cast real(self).Reciprocal;
    }

}