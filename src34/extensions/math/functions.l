import system'math.
import system'dynamic.

// --- Functions ---

public singleton mathControl
{ 
    min(object l, object r)
        = (l < r) iif(l,r).

    max(object l, object r)
        = (l > r) iif(l,r).

    generic eval : n = Wrap::(n, mathOp)~received get.
    
    generic eval(object n, object m) = Wrap::(n, mathOp)~received eval(m).
    
    absolute : v
        = v absolute.

    mod(object v1, object v2)
        = v1 mod(v2).

    bool isOdd : v
        = v isOdd.

    bool isEven : v
        = v isEven.

    bool isZero : v
        = v isZero.

    bool isPositive : v
        = v isPositive.

    bool isNegative : v
        = v isNegative.

    bool isNonnegative : v
        = v isNonnegative.

    real floor(real x)
        = x floor.

    real ceil(real x)
        = x ceil.

    real exp(real x)
        = x exp.
    
    real ln(real x)
        = x ln.
  
    real log2(real x)
        = x log2.
            
    real log10(real x)
        = x log10.
    
    sqrt(real x)
        = x sqrt.

    power(object v, object p)
        = v power(p).

    sqr : v
        = v sqr.
        
    rounded(real x)
        = x rounded.

    roundedInt(real x)
        = x roundedInt.

    integer(real x)
        = x integer.

    integerInt(real x)
        = x integerInt.

    frac(real x)
        = x frac.

    sin(real x)
        = x sin.

    cos(real x)
        = x cos.

    tan(real x)
        = x tan.

    arctan(real x)
        = x arctan.
            
    arcsin(real x)
        = x arcsin.
            
    arccos(real x)
        = x arccos.
            
    radian(real x)
        = x radian.
            
    degree(real x)
        = x degree.
            
    reciprocal(real x)
        = x reciprocal.
}
