import system'math;
////import system'dynamic.
//
//// --- Functions ---
//
//public singleton mathControl
//{ 
//    min(object l, object r)
//        = (l < r).iif(l,r);
//
//    max(object l, object r)
//        = (l > r).iif(l,r);
//
////    generic(n) = mixin __received(new Wrap{n, mathOp}).eval();
////    
////    generic (n, m) = mixin __received(new Wrap{n, mathOp}).eval(m).
//    
//    absolute(v)
//        = v.Absolute;
//
//    mod(object v1, object v2)
//        = v1.mod(v2);
//
//    bool isOdd(v)
//        = v.isOdd();
//
//    bool isEven(v)
//        = v.isEven();
//
//    bool isZero(v)
//        = v.isZero();
//
//    bool isPositive(v)
//        = v.isPositive();
//
//    bool isNegative(v)
//        = v.isNegative();
//
//    bool isNonnegative(v)
//        = v.isNonnegative();
//
//    real floor(real x)
//        = x.floor();
//
//    real ceil(real x)
//        = x.ceil();
//
//    real exp(real x)
//        = x.exp();
//    
//    real ln(real x)
//        = x.ln();
//  
//    real log2(real x)
//        = x.log2();
//            
//    real log10(real x)
//        = x.log10();
//    
//    sqrt(real x)
//        = x.sqrt();
//
//    power(v, p)
//        = v.power(p);
//
//    sqr(v)
//        = v.sqr();
//        
//    rounded(real x)
//        = x.Rounded;
//
//    roundedInt(real x)
//        = x.RoundedInt;
//
//    integer(real x)
//        = x.Integer;
//
//    integerInt(real x)
//        = x.IntegerInt;
//
//    frac(real x)
//        = x.frac();
//
//    sin(real x)
//        = x.sin();
//
//    cos(real x)
//        = x.cos();
//
//    tan(real x)
//        = x.tan();
//
//    arctan(real x)
//        = x.arctan();
//            
//    arcsin(real x)
//        = x.arcsin();
//            
//    arccos(real x)
//        = x.arccos();
//            
//    radian(real x)
//        = x.Radian;
//            
//    degree(real x)
//        = x.Degree;
//            
//    reciprocal(real x)
//        = x.Reciprocal;
//}

public singleton modulo
{
    closure(loperand, roperand)
        = cast int(loperand).mod(roperand);
    
    int closure(int loperand, int roperand)
        = loperand.mod(roperand);
        
    byte closure(byte loperand, byte roperand)
        = loperand.mod(roperand);
        
    short closure(short loperand, short roperand)
        = loperand.mod(roperand);
        
    uint closure(uint loperand, uint roperand)
        = cast int(loperand).mod(roperand);
        
    long closure(long loperand, long roperand)
        = loperand.mod(roperand);
}

public singleton power
{
    closure(loperand, roperand)
        = cast real(loperand).power(roperand);
    
    real closure(real loperand, int roperand)
        = loperand.power(roperand);
        
    real closure(real loperand, real roperand)
        = loperand.power(roperand);
        
    int closure(int loperand, int roperand)
        = loperand.power(roperand);
        
    byte closure(byte loperand, int roperand)
        = loperand.power(roperand);
        
    short closure(short loperand, int roperand)
        = loperand.power(roperand);
        
    uint closure(uint loperand, int roperand)
        = cast int(loperand).power(roperand);
        
    long closure(long loperand, long roperand)
        = loperand.power(roperand);
}

public singleton sqr
{
    closure(operand)
        = cast real(operand).sqr();
    
    real closure(real operand)
        = operand.sqr();
        
    int closure(int operand)
        = operand.sqr();
        
    byte closure(byte operand)
        = operand.sqr();
        
    short closure(short operand)
        = operand.sqr();
        
    uint closure(uint operand)
        = cast int(operand).sqr();
        
    long closure(long operand)
        = operand.sqr();
}

public singleton sqrt
{
    closure(operand)
        = cast real(operand).sqrt();
    
    real closure(real operand)
        = operand.sqrt();
        
    int closure(int operand)
        = operand.sqrt();
}

public singleton exp
{
    closure(operand)
        = cast real(operand).exp();
    
    real closure(real operand)
        = operand.exp();
}

public singleton ln
{
    closure(operand)
        = cast real(operand).ln();
    
    real closure(real operand)
        = operand.ln();
}

//truncate(operand,precision)
public singleton truncate
{
    closure(operand, precision)
        = cast real(operand).truncate(precision);
    
    real closure(real operand, int precision)
        = operand.truncate(precision);
}

//frac(operand)
public singleton frac
{
    closure(operand)
        = cast real(operand).frac();
    
    real closure(real operand)
        = operand.frac();
}

//intg(operand)
public singleton intg
{
    closure(operand)
        = cast real(operand).Integer;
    
    real closure(real operand)
        = operand.Integer;
}

//sin(operand)
public singleton sin
{
    closure(operand)
        = cast real(operand).sin();
    
    real closure(real operand)
        = operand.sin();
}

//cos(operand)
public singleton cos
{
    closure(operand)
        = cast real(operand).cos();
    
    real closure(real operand)
        = operand.cos();
}

//tan(operand)
public singleton tan
{
    closure(operand)
        = cast real(operand).tan();
    
    real closure(real operand)
        = operand.tan();
}

//arctan(operand)
public singleton arctan
{
    closure(operand)
        = cast real(operand).arctan();
    
    real closure(real operand)
        = operand.arctan();
}

//arcsin(operand)
public singleton arcsin
{
    closure(operand)
        = cast real(operand).arcsin();
    
    real closure(real operand)
        = operand.arcsin();
}

//arccos(operand)
public singleton arccos
{
    closure(operand)
        = cast real(operand).arccos();
    
    real closure(real operand)
        = operand.arccos();
}

//floor(operand)
public singleton floor
{
    closure(operand)
        = cast real(operand).floor();
    
    real closure(real operand)
        = operand.floor();
}

//ceil(operand)
public singleton ceil
{
    closure(operand)
        = cast real(operand).ceil();
    
    real closure(real operand)
        = operand.ceil();
}

//log2(operand)
public singleton log2
{
    closure(operand)
        = cast real(operand).log2();
    
    real closure(real operand)
        = operand.log2();
}

//log10(operand)
public singleton log10
{
    closure(operand)
        = cast real(operand).log10();
    
    real closure(real operand)
        = operand.log10();
}
