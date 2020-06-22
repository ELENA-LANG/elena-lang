namespace math
{
    import system'math;
    
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
           
        uint function(uint loperand)
            = loperand;
            
        long function(long loperand)
            = loperand.Absolute;
        
    }
    
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
            
        uint function(uint loperand, int roperand)
            = cast int(loperand).power(roperand);
            
        long function(long loperand, long roperand)
            = loperand.power(roperand);
    }
    
    public singleton sqr
    {
        function(operand)
            = cast real(operand).sqr();
        
        real function(real operand)
            = operand.sqr();
            
        int function(int operand)
            = operand.sqr();
            
        byte function(byte operand)
            = operand.sqr();
            
        short function(short operand)
            = operand.sqr();
            
        uint function(uint operand)
            = cast int(operand).sqr();
            
        long function(long operand)
            = operand.sqr();
    }
    
    public singleton sqrt
    {
        function(operand)
            = cast real(operand).sqrt();
        
        real function(real operand)
            = operand.sqrt();
            
        int function(int operand)
            = operand.sqrt();
    }
    
    public singleton exp
    {
        function(operand)
            = cast real(operand).exp();
        
        real function(real operand)
            = operand.exp();
    }
    
    public singleton ln
    {
        function(operand)
            = cast real(operand).ln();
        
        real function(real operand)
            = operand.ln();
    }
    
    //truncate(operand,precision)
    public singleton truncate
    {
        function(operand, precision)
            = cast real(operand).truncate(precision);
        
        real function(real operand, int precision)
            = operand.truncate(precision);
    }
    
    //frac(operand)
    public singleton frac
    {
        function(operand)
            = cast real(operand).frac();
        
        real function(real operand)
            = operand.frac();
    }
    
    //intg(operand)
    public singleton intg
    {
        function(operand)
            = cast real(operand).Integer;
        
        real function(real operand)
            = operand.Integer;
    }
    
    //sin(operand)
    public singleton sin
    {
        function(operand)
            = cast real(operand).sin();
        
        real function(real operand)
            = operand.sin();
    }
    
    //cos(operand)
    public singleton cos
    {
        function(operand)
            = cast real(operand).cos();
        
        real function(real operand)
            = operand.cos();
    }
    
    //tan(operand)
    public singleton tan
    {
        function(operand)
            = cast real(operand).tan();
        
        real function(real operand)
            = operand.tan();
    }
    
    //arctan(operand)
    public singleton arctan
    {
        function(operand)
            = cast real(operand).arctan();
        
        real function(real operand)
            = operand.arctan();
    }
    
    //arcsin(operand)
    public singleton arcsin
    {
        function(operand)
            = cast real(operand).arcsin();
        
        real function(real operand)
            = operand.arcsin();
    }
    
    //arccos(operand)
    public singleton arccos
    {
        function(operand)
            = cast real(operand).arccos();
        
        real function(real operand)
            = operand.arccos();
    }
    
    //floor(operand)
    public singleton floor
    {
        function(operand)
            = cast real(operand).floor();
        
        real function(real operand)
            = operand.floor();
    }
    
    //ceil(operand)
    public singleton ceil
    {
        function(operand)
            = cast real(operand).ceil();
        
        real function(real operand)
            = operand.ceil();
    }
    
    //log2(operand)
    public singleton log2
    {
        function(operand)
            = cast real(operand).log2();
        
        real function(real operand)
            = operand.log2();
    }
    
    //log10(operand)
    public singleton log10
    {
        function(operand)
            = cast real(operand).log10();
        
        real function(real operand)
            = operand.log10();
    }
    
    public min(l1,l2)
    {
        if (l1 < l2)
        {
            ^ l1
        }
        else
        {
            ^ l2
        }
    }
    
    public max(l1,l2)
    {
        if (l1 > l2)
        {
            ^ l1
        }
        else
        {
            ^ l2
        }
    }
}