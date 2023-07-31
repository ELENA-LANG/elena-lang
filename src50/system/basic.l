// --- DTO --

public class DTO
{
    const MessageName[] __dto_properties;
    
    sealed static MessageName[] __dto_properties()
    {
        ^ __dto_properties
    }
}

// === Basic Convertors ===

// --- convertor helpers ---

/// A byte convertor
public singleton byteConvertor
{
    byte convert(BaseVariable v)
        <= convert(v.Value);
                        
    /// Returns the parameter        
    byte convert(byte n)
        = n;
        
    /// Converts a character into a byte value
    byte convert(char val)
    {
        intern system'core_routines'__byteconvertor.convert(val, ref byte retVal);
        
        ^ retVal
    }

    /// Converts a short integer into a byte value
    byte convert(short n)
    {
        intern system'core_routines'__byteconvertor.convert(n, ref byte retVal);
        
        ^ retVal
    }
        
    /// Converts an integer into a byte value
    byte convert(int val)
    {
        intern system'core_routines'__byteconvertor.convert(val, ref byte retVal);
        
        ^ retVal
    }
        
    /// Converts an integer into a byte value
    byte convert(uint val)
    {
        intern system'core_routines'__byteconvertor.convert(val, ref byte retVal);
        
        ^ retVal
    }

    /// Converts a long integer into a byte value
    byte convert(long val)
    {
        intern system'core_routines'__byteconvertor.convert(val, ref byte retVal);
        
        ^ retVal
    }

    /// Converts a real number into a byte value
    byte convert(real val)
    {
        intern system'core_routines'__byteconvertor.convert(val, ref byte retVal);
        
        ^ retVal
    }

    /// Converts a literal value into a byte value
    byte convert(string s)
    {
        intern system'core_routines'__byteconvertor.convert(s, 10, ref byte retVal);
        
        ^ retVal
    }
                
    /// Converts a wide literal value into a byte value
    byte convert(wide s)
    {
        intern system'core_routines'__byteconvertor.convert(s, 10, ref byte retVal);
        
        ^ retVal
    }

    byte convert(o)
        = cast byte(o);
}

/// A 16bit integer convertor
public singleton shortConvertor
{
    short convert(BaseVariable v)
        <= convert(v.Value);
                        
    /// Returns a short integer value        
    short convert(short n)
        = n;
        
    /// Converts a byte value into a short integer value
    short convert(byte n)
    {
        intern system'core_routines'__shortconvertor.convert(n, ref short retVal);
        
        ^ retVal
    }

    /// Converts an integer into a short integer value
    short convert(int n)
    {
        intern system'core_routines'__shortconvertor.convert(n, ref short retVal);
        
        ^ retVal
    }

    /// Converts an integer into a short integer value
    short convert(uint n)
    {
        intern system'core_routines'__shortconvertor.convert(n, ref short retVal);
        
        ^ retVal
    }

    /// Converts a long integer into a short integer value
    short convert(long n)
    {
        intern system'core_routines'__shortconvertor.convert(n, ref short retVal);
        
        ^ retVal
    }
        
    /// Converts a real number into a short integer value
    short convert(real n)
    {
        intern system'core_routines'__shortconvertor.convert(n, ref short retVal);
        
        ^ retVal
    }
        
    /// Converts a character into a short integer value
    short convert(char ch)
    {
        intern system'core_routines'__shortconvertor.convert(ch, ref short retVal);
        
        ^ retVal
    }
        
    /// Converts a literal string into a short integer value
    short convert(string s)
    {
        intern system'core_routines'__shortconvertor.convert(s, 10, ref short retVal);
        
        ^ retVal
    }
        
    /// Converts a wide literal string into a short integer value
    short convert(wide s)
    {
        intern system'core_routines'__shortconvertor.convert(s, 10, ref short retVal);
        
        ^ retVal
    }
        
    short convert(o)
        = cast short(o);
}

/// An integer convertor
public singleton intConvertor
{
    int convert(BaseVariable v)
        <= convert(v.Value);
        
    /// Converts a literal string into an integer
    int convert(string s)
    {
        intern system'core_routines'__intconvertor.convert(s, 10, ref int retVal);
        
        ^ retVal
    }

    /// Converts a literal string into an integer
    int convert(string s, int base)
    {
        intern system'core_routines'__intconvertor.convert(s, base, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a wide literal string into an integer
    int convert(wide s)
    {
        intern system'core_routines'__intconvertor.convert(s, 10, ref int retVal);
        
        ^ retVal
    }

    /// Converts a wide literal string into an integer
    int convert(wide s, int base)
    {
        intern system'core_routines'__intconvertor.convert(s, base, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a character value into an integer
    int convert(char ch)
    {
        intern system'core_routines'__intconvertor.convert(ch, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a byte value into an integer
    int convert(byte val)
    {
        intern system'core_routines'__intconvertor.convert(val, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a short integer value into an integer
    int convert(short val)
    {
        intern system'core_routines'__intconvertor.convert(val, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a long integer value into an integer and saves the result into an output parameter
    int convert(long val)
    {
        intern system'core_routines'__intconvertor.convert(val, ref int retVal);
        
        ^ retVal
    }

    /// Converts a long integer into an integer
    int convert(real n)
    {
        intern system'core_routines'__intconvertor.convert(n, ref int retVal);
        
        ^ retVal
    }

    /// Returns a parameter                                    
    int convert(int n)
        = new IntNumber(n);

    int convert(uint n)
        = new IntNumber(n);
                        
    int convert(o)
        = cast int(o);
}

/// A unsigned integer convertor
public singleton uintConvertor
{
    uint convert(object)
        = cast uint(object);
    
    uint convert(int val)
        = new UIntNumber(val);
        
    /// Converts a long integer value into an integer and saves the result into an output parameter
    uint convert(long n)
    {
        intern system'core_routines'__uintconvertor.convert(n, ref int retVal);
        
        ^ retVal
    }        
    
    /// Converts a literal string into an unsgined integer
    uint convert(string s)
    {
        intern system'core_routines'__intconvertor.convertUnsigned(s, 10, ref int retVal);
        
        ^ retVal
    }    
    
    /// Converts a literal string into an unsgined integer
    uint convert(string s, int radix)
    {
        intern system'core_routines'__intconvertor.convertUnsigned(s, radix, ref int retVal);
        
        ^ retVal
    }    
}

/// A long integer convertor
public singleton longConvertor
{
    long convert(BaseVariable v)
        <= convert(v.Value);
        
    /// Converts an integer value into a long integer
    long convert(int n)
    {
        intern system'core_routines'__longconvertor.convert(n, ref long retVal);
        
        ^ retVal
    }

    /// Converts an integer value into a long integer
    long convert(uint val)
    {
        intern system'core_routines'__longconvertor.convert(val, ref long retVal);
        
        ^ retVal
    }

    /// Converts an integer value into a long integer
    long convert(byte val)
    {
        intern system'core_routines'__longconvertor.convert(val, ref long retVal);
        
        ^ retVal
    }

    /// Converts an integer value into a long integer
    long convert(short val)
    {
        intern system'core_routines'__longconvertor.convert(val, ref long retVal);
        
        ^ retVal
    }

    /// Converts a real number into a long integer
    long convert(real val)
    {
        intern system'core_routines'__longconvertor.convert(val, ref long retVal);
        
        ^ retVal
    }

    /// Converts a character into a long integer
    long convert(char ch)
    {
        intern system'core_routines'__intconvertor.convert(ch, ref int retVal);
        
        ^ retVal
    }

    /// Converts a literal string into a long integer
    long convert(string s)
    {
        intern system'core_routines'__longconvertor.convert(s, 10, ref long retVal);
        
        ^ retVal
    }

    /// Converts a literal string into a long integer
    long convert(string s, int base)
    {
        intern system'core_routines'__longconvertor.convert(s, base, ref long retVal);
        
        ^ retVal
    }

    /// Converts a wide literal string into a long integer
    long convert(wide s)
    {
        intern system'core_routines'__longconvertor.convert(s, 10, ref long retVal);
        
        ^ retVal
    }

    /// Converts a wide literal string into a long integer
    long convert(wide s, int base)
    {
        intern system'core_routines'__longconvertor.convert(s, base, ref long retVal);
        
        ^ retVal
    }
    
    /// Returns a parameter
    long convert(long n)
        = new LongNumber(n);
        
    long convert(n)
        = cast long(n);
}

/// A real number convertor
public singleton realConvertor
{
    real convert(BaseVariable v)
        <= convert(v.Value);
        
    /// Converts a literal string into a real number
    real convert(string s)
    {
        intern system'core_routines'__realconvertor.convert(s, 19, ref real retVal);
        
        ^ retVal
    }
        
    /// Converts a wide literal string into a real number
    real convert(wide s)
    {
        intern system'core_routines'__realconvertor.convert(s, 19, ref real retVal);
        
        ^ retVal
    }

    /// Returns a parameter
    real convert(real r)
        = new RealNumber(r);
        
    /// Converts an integer into a real number
    real convert(int n)
    {
        intern system'core_routines'__realconvertor.convert(n, ref real retVal);
        
        ^ retVal
    }
        
    /// Converts an integer into a real number
    real convert(uint n)
    {
        real retVal;
        (n < 0)?
        {
            // If the number is bigger than 2^31
            // than we are unlucky
            int odd := n & 1;
            int halfed := n $shl 1;
            
            real r := 0;
            intern system'core_routines'__realconvertor.convert(halfed, ref r);
            r := r * 2.0r;
            r := r + odd;
            
            retVal := r
        }
        : { 
            int number := n;
            intern system'core_routines'__realconvertor.convert(number, ref retVal);
        };
        
        ^ retVal
    }

    /// Converts a long integer into a real number
    real convert(long n)
    {
        intern system'core_routines'__realconvertor.convert(n, ref real retVal);
        
        ^ retVal
    }

    /// Converts a byte into a real number
    real convert(byte n)
    {
        intern system'core_routines'__realconvertor.convert(n, ref real retVal);
        
        ^ retVal
    }

    /// Converts a short integer into a real number
    real convert(short n)
    {
        intern system'core_routines'__realconvertor.convert(n, ref real retVal);
        
        ^ retVal
    }
        
    real convert(o)
        = cast real(o);
}

// --- realOpController ---

public singleton realOpController
{
    real exp(real n)
    {
        intern system'core_routines'__real.exp(n, ref real retVal);
        
        ^ retVal
    }
        
    real abs(real r)
    {
        intern system'core_routines'__real.abs(r, ref real retVal);
        
        ^ retVal
    }
        
    real ln(real n)
    {
        intern system'core_routines'__real.ln(n, ref real retVal);
        
        ^ retVal
    }
        
    real round(real r)
    {
        intern system'core_routines'__real.round(r, ref real retVal);
        
        ^ retVal
    }
        
    real truncate(real r)
    {
        intern system'core_routines'__real.truncate(r, ref real retVal);
        
        ^ retVal
    }
        
    real sin(real r)
    {
        intern system'core_routines'__real.sin(r, ref real retVal);
        
        ^ retVal
    }
        
    real cos(real r)
    {
        intern system'core_routines'__real.cos(r, ref real retVal);
        
        ^ retVal
    }
        
    real arctan(real r)
    {
        intern system'core_routines'__real.arctan(r, ref real retVal);
        
        ^ retVal
    }
                
    real sqrt(real r)
    {
        intern system'core_routines'__real.sqrt(r, ref real retVal);
        
        ^ retVal
    }
        
    real pi()
    {
        intern system'core_routines'__real.pi(ref real retVal);
        
        ^ retVal
    }
    
    real e()
    {
        real retVal := self.exp(1.0r);
        
        ^ retVal
    }        
}

public real Pi_value = realOpController.pi();

public real E_value = realOpController.e();

// === basic type extensions ===

public extension intRefOp : ref<int>
{
    append(int n)
    {
        int v := *self;
        v := v + n;
        
        self.Value := v
    }
    
    reduce(int n)
    {
        int v := *self;
        v := v - n;
        
        self.Value := v
    }
}

public extension realRefOp : ref<real>
{
    append(real n)
    {
        real v := *self;
        v := v + n;
        
        self.Value := v
    }
    
    reduce(real n)
    {
        real v := *self;
        v := v - n;
        
        self.Value := v
    }
}
