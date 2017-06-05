
template varof :: template_subj
{
    template_subj $value.

    set : val
    [
        $value := val template_subj.
    ]
    
    stacksafe set template_subj:val
    [
        $value := val.
    ]

    get = $value.
    
    cast : verb to:target => $value.

    /// Dispatches the verb to the target object with generic subject
    cast : aVerb to:aTarget with:aParam => $value.
    
    equal : anObject
        => $value.

    embeddable dispatch => $value.
}

template nvarof :: template_subj
{
    template_subj $value.
    
    stacksafe implicit template_subj : val
    [
        $value := val.
    ]
    
    set : val
    [
        $value := val template_subj.
    ]
    
    stacksafe set template_subj:val
    [
        $value := val.
    ]
    
    append : val
    [
        $value := $value + val template_subj.
    ]
    
    reduce : val
    [
        $value := $value - val template_subj.
    ]

    multiplyBy : val
    [
        $value := $value * val template_subj.
    ]
    
    /// Divides into the value
    divideInto : val
    [
        $value := $value / val template_subj.
    ]
    
    stacksafe append template_subj:val
    [
        $value := $value + val.
    ]
    
    stacksafe reduce template_subj:val
    [
        $value := $value - val.
    ]

    stacksafe multiplyBy template_subj:val
    [
        $value := $value * val.
    ]
    
    /// Divides into the value
    stacksafe divideInto template_subj:val
    [
        $value := $value / val.
    ]

    get = $value.
    
    cast : verb to:target => $value.

    /// Dispatches the verb to the target object with generic subject
    cast : aVerb to:aTarget with:aParam => $value.
    
    equal : anObject
        => $value.

    embeddable dispatch => $value.
}

// === Boolean values ===

// --- true ---

/// A true value symbol
class true = BaseBoolValue::
{
    /// Executes the action
    __ifoperator if : aTrueAction = aTrueAction eval.

    /// Executes then-action
    if : aTrueAction : aFalseAction = aTrueAction eval.

    /// Always returns nil.
    __ifnotoperator ifnot : aTrueAction = $nil.

    iif:aTrueValue:aFalseValue = aTrueValue.
    
    /// Executes logical OR operation
    ///
    /// Always return true
    or : anExpr = $self.

    /// Executes logical AND operation
    and : anExpr = anExpr bool.

    /// Executes logical XOR operation
    xor : aBool
    [
        aBool bool ? [ ^ false ].
        
        ^ $self
    ]

    /// Determines whether the specified object is equal to the current object boolean value.
    equal : anExpr = anExpr bool.

    /// Determines whether the specified object is equal to the current object boolean value.
    equal bool:aBool = aBool.

    /// Always returns false.
    inverted = false.

    /// Returns literal representation.
    literal = "true".

    /// Returns itself.
    clone = $self.
}.

// --- false ---

/// A false value symbol
class false = BaseBoolValue::
{
    /// Always returns nil
    __ifnotoperator if : anAction = $nil.
    
    /// Executes else-action
    if : aTrueAction : aFalseAction = aFalseAction eval.

    /// Executes the action
    __ifoperator ifnot : aFalseAction = aFalseAction eval.

    iif:aTrueValue:aFalseValue = aFalseValue.

    /// Executes logical OR operation
    or : anExpr = anExpr bool.

    /// Executes logical AND operation
    ///
    /// Always return false
    and : anExpr = $self.

    /// Executes logical XOR operation
    xor : anExpr 
    [
        anExpr bool ? [ ^ true ].
        
        ^ $self
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    equal : anExpr
    [
        anExpr bool ? [ ^ $self ].
        
        ^ true
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    equal bool:anExpr
    [
        ^ anExpr inverted
    ]

    /// Always returns true
    inverted = true.

    /// Returns literal representation
    literal = "false".
    
    /// Returns itself
    clone = $self.
}.

// === basic data classes ===

// --- BaseValue ---

// A base value
class BaseValue
{
    /// Compares the values
    equal : anObject = anObject cast:%equal to:$self.

    /// Compares the values
    bool less : anObject = anObject cast:%less to:$self.

    /// Compares the values
    bool greater : anObject = anObject less:$self.

    bool notless : anObject = $self less:anObject; inverted.

    bool notgreater : anObject = $self greater:anObject; inverted.
}

// --- BaseNumber ---

/// A base numeric value
class BaseNumber :: BaseValue
{
    /// Adds the value
    add : anObject = anObject cast:%add to:$self.

    /// Subtracts the value
    subtract : anObject = anObject cast:%subtract to:$self.

    /// Multiply by the value
    multiply : anObject = anObject cast:%multiply to:$self.

    /// Divides into the value
    divide : anObject = anObject cast:%divide to:$self.
    
    negative
        = 0 - self.
}

// --- IntBaseNumber ---

/// A base integer value
class IntBaseNumber :: BaseNumber
{
    /// Execute bitwise AND operation
    and : anObject = anObject cast:%and to:$self.

    /// Execute bitwise OR operation
    or : anObject = anObject cast:%or to:$self.
    
    /// Execute bitwise XOR operation
    xor : anObject = anObject cast:%xor to:$self.
    
    /// Returns true if all the mask bits are set
    bool allMask : anOperand
        = $self and:anOperand == anOperand.
        
    /// Returns true if any of the mask bits are set    
    bool anyMask : anOperand
        = 0 != $self and:anOperand.
}

// --- ByteNumber ---

/// An unsigned 8 bit integer
sealed embeddable struct ByteNumber :: IntBaseNumber, dispatchable(byte)
{
    __int8 theValue.

    stacksafe implicit int:n
    [
        theValue := n && 255.
    ]

    /// Creates the object with a default value (0)
    constructor new
        = 0.

    /// Returns the minimal value (0)
    constructor min
        = 0.

    /// Returns the maxial value (255)
    constructor max
        = 255.
    
    /// Creates the object with a specified value
    stacksafe constructor new byte:value
        = value.

    /// Creates the object with a specified value
    constructor new : aValue
        <= new byte:aValue.

    /// Returns the value as a short integer
    short = ShortNumber new byte:$self.

    /// Returns the value as an integer
    int = IntNumber new byte:$self.

    /// Returns the value as a long integer
    long = LongNumber new byte:$self.

    /// Returns the value as a real number
    real = RealNumber new byte:$self.

    stacksafe bool equal byte:anOperand
        = theValue == anOperand.

    stacksafe bool equal short:anOperand
        = ShortNumber new byte:$self; equal short:anOperand.

    stacksafe bool less byte:anOperand
        = theValue < anOperand.

    stacksafe bool less short:anOperand
        = ShortNumber new byte:$self; less short:anOperand.

    /// Compares the value
    stacksafe bool equal int:anOperand
        = IntNumber new byte:$self; less int:anOperand.
        
    /// Compares the value
    stacksafe bool less int:anOperand
        = IntNumber new byte:$self; less int:anOperand.

    /// Compares the value
    stacksafe bool equal long:anOperand
        = LongNumber new byte:$self; equal long:anOperand.
        
    /// Compares the value
    stacksafe bool less long:anOperand
        = LongNumber new byte:$self; less long:anOperand.

    /// Compares the value
    stacksafe bool equal real:anOperand
        = RealNumber new byte:$self; equal real:anOperand.

    /// Compares the value
    stacksafe bool less real:anOperand
        = RealNumber new byte:$self; less real:anOperand.

    /// Returns the sum
    stacksafe byte add byte:anOperand
        = theValue + anOperand.

    /// Returns the sum
    stacksafe byte subtract byte:anOperand
        = theValue - anOperand.

    /// Returns the product
    stacksafe byte multiply byte:anOperand
        = theValue * anOperand.

    /// Returns the result of integer division
    stacksafe byte divide byte:anOperand
        = theValue / anOperand.

    /// Returns the sum
    stacksafe short add short:anOperand
        = ShortNumber new byte:$self; add short:anOperand.

    /// Returns the difference
    stacksafe short subtract short:anOperand
        = ShortNumber new byte:$self; subtract short:anOperand.

    /// Returns the product
    stacksafe short multiply short:anOperand
        = ShortNumber new byte:$self; multiply short:anOperand.

    /// Returns the result of integer division
    stacksafe short divide short:anOperand
        = ShortNumber new byte:$self; divide short:anOperand.

    /// Returns the sum
    stacksafe int add int:anOperand
        = IntNumber new byte:$self; add int:anOperand.

    /// Returns the difference
    stacksafe int subtract int:anOperand
        = IntNumber new byte:$self; subtract int:anOperand.

    /// Returns the product
    stacksafe int multiply int:anOperand
        = IntNumber new byte:$self; multiply int:anOperand.

    /// Returns the result of integer division
    stacksafe int divide int:anOperand
        = IntNumber new byte:$self; divide int:anOperand.

    /// Returns the sum
    stacksafe long add long:anOperand
        = LongNumber new byte:$self; add long:anOperand.

    /// Returns the difference
    stacksafe long subtract long:anOperand
        = LongNumber new byte:$self; subtract long:anOperand.

    /// Returns the product
    stacksafe long multiply long:anOperand
        = LongNumber new byte:$self; multiply long:anOperand.

    /// Returns the result of integer division
    stacksafe long divide long:anOperand
        = LongNumber new byte:$self; divide long:anOperand.

    /// Returns the sum
    stacksafe real add real:anOperand
        = RealNumber new byte:$self; add real:anOperand.

    /// Returns the difference
    stacksafe real subtract real:anOperand
        = RealNumber new byte:$self; subtract real:anOperand.

    /// Returns the product
    stacksafe real multiply real:anOperand
        = RealNumber new byte:$self; multiply real:anOperand.

    /// Returns the result of integer division
    stacksafe real divide real:anOperand
        = RealNumber new byte:$self; divide real:anOperand.

    /// Returns the result of bitwise AND operation
    stacksafe byte and byte:anOperand
        = theValue && anOperand.

    /// Returns the result of bitwise OR operation
    stacksafe byte or byte:anOperand
        = theValue || anOperand.

    /// Returns the result of bitwise XOR operation
    stacksafe byte xor byte:anOperand
        = theValue ^^ anOperand.

    /// Returns the result of bitwise AND operation
    stacksafe short and short:anOperand
        = ShortNumber new byte:$self; and short:anOperand.

    /// Returns the result of bitwise OR operation
    stacksafe short or short:anOperand
        = ShortNumber new byte:$self; or short:anOperand.

    /// Returns the result of bitwise XOR operation
    stacksafe short xor short:anOperand
        = ShortNumber new byte:$self; xor short:anOperand.

    /// Returns the result of bitwise AND operation
    stacksafe int and int:anOperand
        = IntNumber new byte:$self; and int:anOperand.

    /// Returns the result of bitwise OR operation
    stacksafe int or int:anOperand
        = IntNumber new byte:$self; or int:anOperand.

    /// Returns the result of bitwise XOR operation
    stacksafe int xor int:anOperand
        = IntNumber new byte:$self; xor int:anOperand.

    /// Returns the result of bitwise AND operation
    stacksafe long and long:anOperand
        = LongNumber new byte:$self; and long:anOperand.

    /// Returns the result of bitwise OR operation
    stacksafe long or long:anOperand
        = LongNumber new byte:$self; or long:anOperand.

    /// Returns the result of bitwise XOR operation
    stacksafe long xor long:anOperand
        = LongNumber new byte:$self; xor long:anOperand.

    stacksafe byte shiftLeft int:val
    [
        ^ theValue << val
    ]

    stacksafe byte shiftRight int:val
    [
        ^ theValue >> val
    ]
    
    /// Bitwise inversion
    stacksafe byte inverted
        => system'core_routines'__byte.
        
    /// Returns the literal presentation
    literal = system'literalConvertor convert byte:$self.

    /// Clones the value
    clone = ByteNumber new byte:$self.

    // Reads the object hash code into the output parameter
    stacksafe readHashCodeTo vint:aCode 
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        $self readHashCodeTo vint:aCode.
        
        ^ aCode
    ]
}

// --- ShortNumber ---

/// A signed 16 bit integer
sealed embeddable struct ShortNumber :: IntBaseNumber, dispatchable(short)
{
    __int16 theValue.

    stacksafe implicit int:n
    [
        (n < 0)
            ? [ 
                theValue := n && 0FFFFh. 
                
                theValue := theValue || 0FFFF0000h
            ]
            ! [ theValue := n && 0FFFFh ]
    ]
    
    /// Returns the minimal value (-32768)
    constructor min
        = -32768.

    /// Returns the maxial value (32767)
    constructor max
        = 32767.

    /// Creates the object with specified value
    stacksafe constructor new byte:value
        = shortConvertor convert byte:value.
        
    /// Creates the object with specified value
    stacksafe constructor new short:value
        = value.

    /// Creates the object with a default value (0)
    constructor new
        = 0.

    /// Creates the object with specified value
    constructor new : anObject
        <= new short:anObject.

    /// Returns the byte value        
    byte = byteConvertor convert short:$self.
    
    /// Returns the integer value
    int = IntNumber new short:$self.
    
    /// Returns the long integer value
    long = LongNumber new short:$self.
    
    /// Returns the value as a real number
    real = RealNumber new short:$self.

    /// Compares the value
    stacksafe bool equal short:anOperand 
        = theValue == anOperand.
        
    /// Compares the value
    stacksafe bool less short:anOperand
        = theValue < anOperand.
        
    /// Compares the value
    stacksafe bool equal int:anOperand 
        = IntNumber new short:$self; equal int:anOperand.
        
    /// Compares the value
    stacksafe bool less int:anOperand
        = IntNumber new short:$self; less int:anOperand.
    
    /// Compares the value
    stacksafe bool equal byte:anOperand 
        = $self equal short:(ShortNumber new byte:anOperand).
        
    /// Compares the value
    stacksafe bool less byte:anOperand
        = $self less short:(ShortNumber new byte:anOperand).
    
    /// Compares the value
    stacksafe bool equal long:anOperand 
        = LongNumber new short:$self; equal long:anOperand.
        
    /// Compares the value
    stacksafe bool less long:anOperand
        = LongNumber new short:$self; less long:anOperand.
    
    /// Compares the value
    stacksafe bool equal real:anOperand 
        = RealNumber new short:$self; equal real:anOperand.
        
    /// Compares the value
    stacksafe bool less real:anOperand
        = RealNumber new short:$self; less real:anOperand.

    /// Returns the sum
    stacksafe short add short:anOperand
        = theValue + anOperand.

    /// Returns the sum
    stacksafe short subtract short:anOperand
        = theValue - anOperand.

    /// Returns the product
    stacksafe short multiply short:anOperand
        = theValue * anOperand.

    /// Returns the result of integer division
    stacksafe short divide short:anOperand
        = theValue / anOperand.

    /// Returns the sum
    stacksafe add byte:anOperand
        = $self add short:(ShortNumber new byte:anOperand).

    /// Returns the difference
    stacksafe subtract byte:anOperand
        = $self subtract short:(ShortNumber new byte:anOperand).

    /// Returns the product
    stacksafe multiply byte:anOperand
        = $self multiply short:(ShortNumber new byte:anOperand).

    /// Returns the result of integer division
    stacksafe divide byte:anOperand
        = $self divide short:(ShortNumber new byte:anOperand).

    /// Returns the sum
    stacksafe add int:anOperand
        = IntNumber new short:$self; add int:anOperand.

    /// Returns the difference
    stacksafe subtract int:anOperand
        = IntNumber new short:$self; subtract int:anOperand.

    /// Returns the product
    stacksafe multiply int:anOperand
        = IntNumber new short:$self; multiply int:anOperand.
        
    /// Returns the result of integer division
    stacksafe divide int:anOperand
        = IntNumber new short:$self; divide int:anOperand.

    /// Returns the sum
    stacksafe add long:anOperand
        = LongNumber new short:$self; add long:anOperand.

    /// Returns the difference
    stacksafe subtract long:anOperand
        = LongNumber new short:$self; subtract long:anOperand.

    /// Returns the product
    stacksafe multiply long:anOperand
        = LongNumber new short:$self; multiply long:anOperand.
        
    /// Returns the result of integer division
    stacksafe divide long:anOperand
        = LongNumber new short:$self; divide long:anOperand.

    /// Returns the sum
    stacksafe add real:anOperand
        = RealNumber new short:$self; add real:anOperand.

    /// Returns the difference
    stacksafe subtract real:anOperand
        = RealNumber new short:$self; subtract real:anOperand.

    /// Returns the product
    stacksafe multiply real:anOperand
        = RealNumber new short:$self; multiply real:anOperand.
        
    /// Returns the result of integer division
    stacksafe divide real:anOperand
        = RealNumber new short:$self; divide real:anOperand.

    /// Returns the result of bitwise AND operation
    stacksafe short and short:anOperand
        = theValue && anOperand.

    /// Returns the result of bitwise OR operation
    stacksafe short or short:anOperand
        = theValue || anOperand.

    /// Returns the result of bitwise XOR operation
    stacksafe short xor short:anOperand
        = theValue ^^ anOperand.
        
    /// Returns the result of bitwise AND operation
    stacksafe and byte:anOperand
        = $self and short:(ShortNumber new byte:anOperand). 

    /// Returns the result of bitwise OR operation
    stacksafe or byte:anOperand
        = $self or short:(ShortNumber new byte:anOperand). 

    /// Returns the result of bitwise XOR operation
    stacksafe xor byte:anOperand
        = $self xor short:(ShortNumber new byte:anOperand). 
        
    /// Returns the result of bitwise AND operation
    stacksafe and int:anOperand
        = IntNumber new short:$self; and int:anOperand.        
        
    /// Returns the result of bitwise OR operation
    stacksafe or int:anOperand
        = IntNumber new short:$self; or int:anOperand.        
        
    /// Returns the result of bitwise XOR operation
    stacksafe xor int:anOperand
        = IntNumber new short:$self; xor int:anOperand.        
        
    /// Returns the result of bitwise AND operation
    stacksafe and long:anOperand
        = LongNumber new short:$self; and long:anOperand.        
        
    /// Returns the result of bitwise OR operation
    stacksafe or long:anOperand
        = LongNumber new short:$self; or long:anOperand.        
        
    /// Returns the result of bitwise XOR operation
    stacksafe xor long:anOperand
        = LongNumber new short:$self; xor long:anOperand.        

    stacksafe short shiftLeft int:val
    [
        ^ theValue << val
    ]

    stacksafe short shiftRight int:val
    [
        ^ theValue >> val
    ]
    
    /// Bitwise inversion
    stacksafe short inverted
        => system'core_routines'__short.  

    /// Returns the literal presentation
    literal = system'literalConvertor convert short:$self.

    /// Clones the value
    clone = ShortNumber new short:$self.    

    // Reads the object hash code into the output parameter
    stacksafe readHashCodeTo vint:aCode 
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        $self readHashCodeTo vint:aCode.
        
        ^ aCode.
    ]        
}

// --- IntNumber ---

/// A signed 32 bit integer
sealed embeddable struct IntNumber :: IntBaseNumber, dispatchable(int,long,real,char,literal,wide)
{
    __int32 theValue.

    stacksafe implicit byte:n
    [
        theValue := n.
    ]
    
    stacksafe implicit short:n
    [
        theValue := n.
    ]
    
    /// Creates the object with the minimal value
    constructor min
        = -2147483648.

    /// Creates the object with the maximal value
    constructor max
        = 2147483647.
    
    /// Creates the object with specified value
    stacksafe constructor new int:value
        = value.

    /// Creates the object with specified value
    stacksafe constructor new short:value
        = intConvertor convert short:value.

    /// Creates the object with specified value
    stacksafe constructor new byte:value
        = intConvertor convert byte:value.

    /// Creates the object with a default value (0)
    constructor new
        <= new int:0.

    /// Creates the object with specified value
    constructor new : anObject
        <= new int:anObject.

    /// Returns the value as a short integer    
    stacksafe short = shortConvertor convert int:$self.

    /// Returns the byte value    
    byte = byteConvertor convert int:$self.

    /// Returns the long integer
    long = LongNumber new int:$self.

    /// Returns the real number
    real = RealNumber new int:$self.
    
    /// Compares the value
    stacksafe bool equal int:anOperand 
        = theValue == anOperand.
        
    /// Compares the value
    stacksafe bool less int:anOperand
        = theValue < anOperand.
        
    /// Returns the sum
    stacksafe int add int:anOperand
        = theValue + anOperand.

    /// Returns the sum
    stacksafe int subtract int:anOperand
        = theValue - anOperand.

    /// Returns the product
    stacksafe int multiply int:anOperand
        = theValue * anOperand.

    /// Returns the result of integer division
    stacksafe int divide int:anOperand
        = theValue / anOperand.

    /// Returns the result of bitwise AND operation
    stacksafe int and int:anOperand
        = theValue && anOperand.

    /// Returns the result of bitwise OR operation
    stacksafe int or int:anOperand
        = theValue || anOperand.

    /// Returns the result of bitwise XOR operation
    stacksafe int xor int:anOperand
        = theValue ^^ anOperand.    

    /// Compares the integer value
    stacksafe bool equal short:value 
        = $self equal int:(IntNumber new short:value).

    /// Compares the integer value
    stacksafe bool equal byte:value
        = $self equal int:(IntNumber new byte:value).
        
    /// Compares the integer value
    stacksafe bool equal long:anOperand 
        = LongNumber new int:$self; equal long:anOperand.

    /// Compares the integer value
    stacksafe bool equal real:anOperand 
        = RealNumber new int:$self; equal real:anOperand.

    /// Compares the integer value
    stacksafe bool less short:value
        = $self equal int:(IntNumber new short:value).

    /// Compares the integer value
    stacksafe bool less byte:value
        = $self less int:(IntNumber new byte:value).
        
    /// Compares the integer value
    stacksafe bool less long:anOperand 
        = LongNumber new int:$self; less long:anOperand.

    /// Compares the integer value
    stacksafe bool less real:anOperand 
        = RealNumber new int:$self; less real:anOperand.

    /// Returns the sum of integers
    stacksafe add short:value
        = $self add int:(IntNumber new short:value).

    /// Returns the sum of integers
    stacksafe add byte:value
        = $self add int:(IntNumber new byte:value).

    /// Returns the sum of integers
    stacksafe add long:anOperand
        = LongNumber new int:$self; add long:anOperand.

    /// Returns the sum of integers
    stacksafe add real:anOperand
        = RealNumber new int:$self; add real:anOperand.

    /// Returns the difference of integers
    stacksafe subtract short:value
        = $self subtract int:(IntNumber new short:value).

    /// Returns the difference of integers
    stacksafe subtract byte:value
        = $self subtract int:(IntNumber new byte:value).

    /// Returns the difference of integers
    stacksafe subtract long:anOperand
        = LongNumber new int:$self; subtract long:anOperand.

    /// Returns the difference of integers
    stacksafe subtract real:anOperand
        = RealNumber new int:$self; subtract real:anOperand.

    /// Returns the product of integers
    stacksafe multiply short:value
        = $self multiply int:(IntNumber new short:value).        

    /// Returns the product of integers
    stacksafe multiply byte:value
        = $self multiply int:(IntNumber new byte:value).        

    /// Returns the product of integers
    stacksafe multiply long:anOperand
        = LongNumber new int:$self; multiply long:anOperand.

    /// Returns the product of integers
    stacksafe multiply real:anOperand
        = RealNumber new int:$self; multiply real:anOperand.

    /// Returns the result of integer division
    stacksafe divide short:value
        = $self divide int:(IntNumber new short:value).        

    /// Returns the result of integer division
    stacksafe divide byte:value
        = $self divide int:(IntNumber new byte:value).        

    /// Returns the result of integer division
    stacksafe divide long:anOperand
        = LongNumber new int:$self; divide long:anOperand.

    /// Returns the result of integer division
    stacksafe divide real:anOperand
        = RealNumber new int:$self; divide real:anOperand.

    /// Returns the result of bitwise AND operation
    stacksafe and short:anOperand
        = $self and int:(IntNumber new short:anOperand).

    /// Returns the result of bitwise AND operation
    stacksafe and byte:anOperand
        = $self and int:(IntNumber new byte:anOperand).

    /// Returns the result of bitwise AND operation
    stacksafe and long:anOperand
        = LongNumber new int:$self; and long:anOperand.

    /// Returns the result of bitwise OR operation
    stacksafe or short:anOperand
        = $self or int:(IntNumber new short:anOperand).

    /// Returns the result of bitwise OR operation
    stacksafe or byte:anOperand
        = $self or int:(IntNumber new byte:anOperand).

    /// Returns the result of bitwise OR operation
    stacksafe or long:anOperand
        = LongNumber new int:$self; or long:anOperand.

    /// Returns the result of bitwise XOR operation
    stacksafe xor short:anOperand
        = $self xor int:(IntNumber new short:anOperand).

    /// Returns the result of bitwise XOR operation
    stacksafe xor byte:anOperand
        = $self xor int:(IntNumber new byte:anOperand).

    /// Returns the result of bitwise XOR operation
    stacksafe xor long:anOperand
        = LongNumber new int:$self; xor long:anOperand.

    /// Returns the literal presentation
    literal = system'literalConvertor convert int:$self.

    /// Clones the value
    clone = IntNumber new int:$self.

    // Reads the object hash code into the output parameter
    stacksafe readHashCodeTo vint:aCode 
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        $self readHashCodeTo vint:aCode.
        
        ^ aCode.
    ]        

    stacksafe int shiftLeft int:val
    [
        ^ theValue << val
    ]

    stacksafe int shiftRight int:val
    [
        ^ theValue >> val
    ]
    
    /// Bitwise inversion
    stacksafe int inverted
        => system'core_routines'__int.  
        
    /// Negates the value
    stacksafe negative
        = 0 - theValue.
}

// --- LongNumber ---

/// A signed 64 bit integer value
sealed embeddable struct LongNumber :: IntBaseNumber, dispatchable(long,int,real,literal,wide)
{
    __int64 theValue.
    
    /// Returns the minimal value
    constructor min
        = -9223372036854775808l.

    /// Returns the maximal value
    constructor max
        = 9223372036854775807l.

    /// Creates the object with a default value (0)
    constructor new
        <= new long:0l.

    /// Creates the object with specified value
    stacksafe constructor new long:value
        = value.

    /// Creates the object with specified value
    stacksafe constructor new int:value
        = longConvertor convert int:value.

    /// Creates the object with specified value
    stacksafe constructor new short:value 
        = longConvertor convert short:value.

    /// Creates the object with specified value
    stacksafe constructor new byte:value 
        = longConvertor convert byte:value.

    /// Creates the object with specified value
    constructor new : anObject
        <= new long:anObject.

    /// Returns an integer value
    int = intConvertor convert long:$self.

    /// Returns the value as a short integer    
    short = shortConvertor convert long:$self.
    
    /// Returns the byte value
    byte = byteConvertor convert long:$self.

    /// Returns a real value
    real = RealNumber new long:$self.
    
    /// Compares the value
    stacksafe bool equal long:anOperand 
        = theValue == anOperand.
        
    /// Compares the value
    stacksafe bool less long:anOperand
        = theValue < anOperand.
        
    /// Returns the sum
    stacksafe long add long:anOperand
        = theValue + anOperand.

    /// Returns the sum
    stacksafe long subtract long:anOperand
        = theValue - anOperand.

    /// Returns the product
    stacksafe long multiply long:anOperand
        = theValue * anOperand.

    /// Returns the result of integer division
    stacksafe long divide long:anOperand
        = theValue / anOperand.

    /// Returns the result of bitwise AND operation
    stacksafe long and long:anOperand
        = theValue && anOperand.

    /// Returns the result of bitwise OR operation
    stacksafe long or long:anOperand
        = theValue || anOperand.

    /// Returns the result of bitwise XOR operation
    stacksafe long xor long:anOperand
        = theValue ^^ anOperand.    

    /// Compares the integer value
    stacksafe bool equal int:anOperand 
        = $self equal long:(LongNumber new int:anOperand).

    /// Compares the integer value
    stacksafe bool equal short:anOperand 
        = $self equal long:(LongNumber new short:anOperand).

    /// Compares the integer value
    stacksafe bool equal byte:anOperand 
        = $self equal long:(LongNumber new byte:anOperand).

    /// Compares the integer value
    stacksafe equal real:anOperand 
        = RealNumber new long:$self; equal real:anOperand.

    /// Compares the integer value
    stacksafe bool less int:anOperand 
        = $self less long:(LongNumber new int:anOperand).

    /// Compares the integer value
    stacksafe bool less short:anOperand 
        = $self less long:(LongNumber new short:anOperand).

    /// Compares the integer value
    stacksafe bool less byte:anOperand 
        = $self less long:(LongNumber new byte:anOperand).

    /// Compares the integer value
    stacksafe less real:anOperand 
        = RealNumber new long:$self; less real:anOperand.

    /// Returns the sum of integers
    stacksafe add int:anOperand
        = $self add long:(LongNumber new int:anOperand).

    /// Returns the sum of integers
    stacksafe add short:anOperand
        = $self add long:(LongNumber new short:anOperand).

    /// Returns the sum of integers
    stacksafe add byte:anOperand
        = $self add long:(LongNumber new byte:anOperand).

    /// Returns the sum of integers
    stacksafe add real:anOperand
        = RealNumber new long:$self; add real:anOperand.

    /// Returns the difference of integers
    stacksafe subtract int:anOperand
        = $self subtract long:(LongNumber new int:anOperand).

    /// Returns the difference of integers
    stacksafe subtract short:anOperand
        = $self subtract long:(LongNumber new short:anOperand).

    /// Returns the difference of integers
    stacksafe subtract byte:anOperand
        = $self subtract long:(LongNumber new byte:anOperand).

    /// Returns the difference of integers
    stacksafe subtract real:anOperand
        = RealNumber new long:$self; subtract real:anOperand.

    /// Returns the product of integers
    stacksafe multiply real:anOperand
        = RealNumber new long:$self; multiply real:anOperand.

    /// Returns the product of integers
    stacksafe multiply int:anOperand
        = $self multiply long:(LongNumber new int:anOperand).

    /// Returns the product of integers
    stacksafe multiply byte:anOperand
        = $self multiply long:(LongNumber new byte:anOperand).

    /// Returns the product of integers
    stacksafe multiply short:anOperand
        = $self multiply long:(LongNumber new short:anOperand).

    /// Returns the result of integer division
    stacksafe divide int:anOperand
        = $self divide long:(LongNumber new int:anOperand).

    /// Returns the result of integer division
    stacksafe divide short:anOperand
        = $self divide long:(LongNumber new short:anOperand).

    /// Returns the result of integer division
    stacksafe divide byte:anOperand
        = $self divide long:(LongNumber new byte:anOperand).

    /// Returns the result of integer division
    stacksafe divide real:anOperand
        = RealNumber new long:$self; divide real:anOperand.
        
    /// Returns the result of bitwise AND operation
    stacksafe and int:anOperand
        = $self and long:(LongNumber new int:anOperand).

    /// Returns the result of bitwise AND operation
    stacksafe and short:anOperand
        = $self and long:(LongNumber new short:anOperand).

    /// Returns the result of bitwise AND operation
    stacksafe and byte:anOperand
        = $self and long:(LongNumber new byte:anOperand).

    /// Returns the result of bitwise OR operation
    stacksafe or int:anOperand
        = $self or long:(LongNumber new int:anOperand).

    /// Returns the result of bitwise OR operation
    stacksafe or short:anOperand
        = $self or long:(LongNumber new short:anOperand).

    /// Returns the result of bitwise OR operation
    stacksafe or byte:anOperand
        = $self or long:(LongNumber new byte:anOperand).        

    /// Returns the result of bitwise XOR operation
    stacksafe xor int:anOperand
        = $self xor long:(LongNumber new int:anOperand).

    /// Returns the result of bitwise XOR operation
    stacksafe xor short:anOperand
        = $self xor long:(LongNumber new short:anOperand).

    /// Returns the result of bitwise XOR operation
    stacksafe xor byte:anOperand
        = $self xor long:(LongNumber new byte:anOperand).

    /// Returns the literal presentation
    literal = system'literalConvertor convert long:$self.

    /// Clones the value
    clone = LongNumber new long:$self.
    
    // Reads the object hash code into the output parameter
    stacksafe readHashCodeTo vint:aCode
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        $self readHashCodeTo vint:aCode.
        
        ^ aCode.
    ]

    stacksafe long shiftLeft int:val
    [
        ^ theValue << val
    ]

    stacksafe long shiftRight int:val
    [
        ^ theValue >> val
    ]
    
    /// Bitwise inversion
    stacksafe long inverted
        => system'core_routines'__long.  
        
    /// Negates the value
    stacksafe negative
        = 0l - theValue.
}

// --- realNumber ---

/// A 64bit float numeric value
sealed embeddable struct RealNumber :: BaseNumber, dispatchable(real,int,long,literal,wide)
{
    __real64 theValue.

    stacksafe implicit int:n
    [
        realConvertor convert int:n vreal:$self.
    ]
            
    /// Returns the minimal value
    constructor min
        = -1.7976931348623157e+308r.
    
    /// Returns the maximal value
    constructor max
        = 1.7976931348623157e+308r.

    /// Creates the object with a default value (0)
    constructor new
        <= new real:0.0r.

    /// Creates the object with specified value
    stacksafe constructor new real:value
        = value.
  
    /// Creates the object with specified value
    stacksafe constructor new int:value 
        = realConvertor convert int:value.

    /// Creates the object with specified value
    stacksafe constructor new short:value 
        = realConvertor convert short:value.

    /// Creates the object with specified value
    stacksafe constructor new byte:value 
        = realConvertor convert byte:value.

    /// Creates the object with specified value
    stacksafe constructor new long:value
        = realConvertor convert long:value.
        
    /// Creates the object with specified value
    constructor new : anObject
        <= new real:anObject.
        
    /// Compares the value
    stacksafe bool equal real:anOperand 
        = theValue == anOperand.
        
    /// Compares the value
    stacksafe bool less real:anOperand
        = theValue < anOperand.
        
    /// Returns the sum
    stacksafe real add real:anOperand
        = theValue + anOperand.

    /// Returns the sum
    stacksafe real subtract real:anOperand
        = theValue - anOperand.

    /// Returns the product
    stacksafe real multiply real:anOperand
        = theValue * anOperand.

    /// Returns the result of integer division
    stacksafe real divide real:anOperand
        = theValue / anOperand.
        
    /// Compares the numeric values
    stacksafe bool equal int:anOperand
        = $self equal real:(RealNumber new int:anOperand).

    /// Compares the numeric values
    stacksafe bool equal byte:anOperand
        = $self equal real:(RealNumber new byte:anOperand).

    /// Compares the numeric values
    stacksafe bool equal short:anOperand
        = $self equal real:(RealNumber new short:anOperand).

    /// Compares the numeric values
    stacksafe bool equal long:anOperand
        = $self equal real:(RealNumber new long:anOperand).

    /// Compares the numeric values
    stacksafe bool less int:anOperand
        = $self less real:(RealNumber new int:anOperand).

    /// Compares the numeric values
    stacksafe bool less byte:anOperand
        = $self less real:(RealNumber new byte:anOperand).

    /// Compares the numeric values
    stacksafe bool less short:anOperand
        = $self less real:(RealNumber new short:anOperand).

    /// Compares the numeric values
    stacksafe bool less long:anOperand
        = $self less real:(RealNumber new long:anOperand).

    /// Returns the sum of numbers
    stacksafe add int:anOperand
        = $self add real:(RealNumber new int:anOperand).

    /// Returns the sum of numbers
    stacksafe add byte:anOperand
        = $self add real:(RealNumber new byte:anOperand).

    /// Returns the sum of numbers
    stacksafe add short:anOperand
        = $self add real:(RealNumber new short:anOperand).

    /// Returns the sum of numbers
    stacksafe add long:anOperand
        = $self add real:(RealNumber new long:anOperand).

    /// Returns the difference of numbers
    stacksafe subtract int:anOperand
        = $self subtract real:(RealNumber new int:anOperand).

    /// Returns the difference of numbers
    stacksafe subtract byte:anOperand
        = $self subtract real:(RealNumber new byte:anOperand).

    /// Returns the difference of numbers
    stacksafe subtract short:anOperand
        = $self subtract real:(RealNumber new short:anOperand).

    /// Returns the difference of numbers
    stacksafe subtract long:anOperand
        = $self subtract real:(RealNumber new long:anOperand).

    /// Returns the product of numbers
    stacksafe real multiply int:anOperand
    [
        real n := anOperand.
        
        ^ $self multiply real:n.
    ]

    /// Returns the product of numbers
    stacksafe multiply byte:anOperand
        = $self multiply real:(RealNumber new byte:anOperand).

    /// Returns the product of numbers
    stacksafe multiply short:anOperand
        = $self multiply real:(RealNumber new short:anOperand).

    /// Returns the product of numbers
    stacksafe multiply long:anOperand
        = $self multiply real:(RealNumber new long:anOperand).

    /// Returns the result of the division
    stacksafe divide int:anOperand
    [
        real n := anOperand.
        
        ^ $self divide real:n
    ]

    /// Returns the result of the division
    stacksafe divide byte:anOperand
        = $self divide real:(RealNumber new byte:anOperand).
        
    /// Returns the result of the division
    stacksafe divide short:anOperand
        = $self divide real:(RealNumber new short:anOperand).

    /// Returns the result of the division
    stacksafe divide long:anOperand
        = $self divide real:(RealNumber new long:anOperand).

    /// Returns the literal presentation
    literal = system'literalConvertor convert real:$self.

    /// Clones the value    
    clone = RealNumber new real:$self.
    
    // Reads the object hash code into the output parameter
    stacksafe readHashCodeTo vint:aCode
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        $self readHashCodeTo vint:aCode.
        
        ^ aCode
    ]
        
//    /// Negates the value
//    stacksafe negative
//        = 0.0r - theValue.
}

// --- CharValue ---

/// An UTF-32 character symbol
sealed const embeddable struct CharValue :: BaseValue, dispatchable(char,int,literal,wide)
{
    __dword theValue.

    /// Creates the object with a default value (0)
    constructor new
        <= new char:$0.

    /// Creates the object with specified value
    stacksafe constructor new char:value
        = value.

    /// Creates the object with specified value
    constructor new : anOperand
        <= new char:(anOperand cast:%convert to:3).

    /// Creates the object with specified numeric value
    stacksafe constructor new int:anOperand
        <= new char:(charConvertor convert int:anOperand).

    /// Returns the minimal possible character value (0)
    constructor min
        = $0.

    /// Returns the maximal possible character value (010FFFFh)
    constructor max
        = $010FFFFh.

    stacksafe bool equal int:val
        = theValue == val.

    /// Clones the value
    clone = CharValue new char:$self.

    /// Returns the literal value
    stacksafe literal = system'literalConvertor convert char:$self.

    /// Returns the wide literal value
    wide = system'wideConvertor convert char:$self.
    
    /// Reads the number of bytes to represent the symbol
    stacksafe readLengthTo vint:aLength
        => system'core_routines'__char.

    /// Reads the number of bytes to represent the symbol
    stacksafe readWideLength vint:aRetVal
    [
        int aLength := 0.
        $self readLengthTo vint:aLength.
        
        aLength := aLength + 1.
        aLength := aLength / 2.

        aRetVal int := aLength
    ]
        
    /// Retuns the number of bytes to represent the symbol        
    stacksafe embeddable int length
    [
        int aLength := 0.
        $self readLengthTo vint:aLength.
        
        ^ aLength
    ]
    
    /// Retuns the number of short integers to represent the symbol
    stacksafe embeddable int wideLength
    [
        int aLength := 0.
        $self readWideLengthTo vint:aLength.
        
        ^ aLength.
    ]

    /// Compares values
    stacksafe bool equal char:anOperand 
        = theValue == anOperand.

    /// Compares values
    stacksafe bool less char:anOperand 
        = theValue < anOperand.
        
    // Reads the object hash code into the output parameter
    stacksafe readHashCodeTo vint:aCode 
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        $self readHashCodeTo vint:aCode.
        
        ^ aCode.
    ]
}

// --- nil constant ---

/// A nil value
singleton nil :: dispatchable(void)
{
    /// Returns true if the specified object is nil.
    equal : anObject 
    [
        ($nil == anObject)
            ? [ ^ true. ].
        
        ($super equal:anObject)
            ? [ ^ true. ].
        
        ^ false.
    ]
    
    /// Returns nil
    ///
    /// Any typecasting message will be handled and nil value returned
    generic get = $nil.
}

// === Numeric Variables ===

type vbyte  :: nvarof(byte).
type vshort :: nvarof(short).
type vint   :: nvarof(int).
type vlong  :: nvarof(long).
type vreal  :: nvarof(real).
type vchar  :: varof(char).

// === Basic Factory symbols ==

class BaseExtender
{
    object theObject.
    
    /// Returns true if the specified object is equal to the current object; otherwise, false.
    equal : anObject
        => theObject.
      
    /// Dispatches the verb to the target object
    cast : aVerb to:aTarget => theObject.
    
    /// Dispatches the verb to the target object
    cast : aVerb to:aTarget with:aParam => theObject.
    
    dispatch => theObject.
}

/// A generic variable. 
///
/// Extends an assigned value
sealed class Variable :: nvarof(object)
{
    /// Creates the empty variable
    constructor new
    [
    ]        
        
    /// Creates the variable
    constructor new : value
    [
        $value := value.
    ]
    
    set : value
    [
        $value := value.
    ]
    
    append : val
    [
        ($nil == $value)
            ? [ $value := val ]
            ! [ $value := $value + val ]
    ]
    
    reduce : val
    [
        ($nil == $value)
            ? [ system'InvalidOperationException new; raise ]
            ! [ $value := $value - val ]
    ]

    multiplyBy : val
    [
        ($nil == $value)
            ? [ system'InvalidOperationException new; raise ]
            ! [ $value := $value * val ]
    ]
    
    /// Divides into the value
    divideInto : val
    [
        ($nil == $value)
            ? [ system'InvalidOperationException new; raise ]
            ! [ $value := $value / val ]
    ]    
}

// --- KeyValue ---

/// A key - object pair
///
/// Extends the object with a key property
sealed class KeyValue
{
    object theKey.
    object theObject.
    
    /// Creates a new objext
    constructor new key:aKey value:anObject
    [
        theKey := aKey.
        theObject := anObject.
    ]
    
    /// Returns the key
    key = theKey.
    
    /// Returns the key
    value = theObject.

    set value:anObject
    [
        theObject := anObject.
    ]
    
    /// Dispatches the verb to the target object
    cast : aVerb to:aTarget => theObject.
    
    /// Dispatches the verb to the target object
    cast : aVerb to:aTarget with:aParam => theObject.
            
    /// Redirects the incoming messages to the object
    dispatch => theObject.
}

// --- DTO ---

class DTO :: dispatchable(dto)
{
}

// === Basic Convertors ===

// --- convertor helpers ---

/// A byte convertor
class byteConvertor =
{
    /// Converts a character into a byte value and saves the result into an output parameter
    stacksafe convert char:aValue vbyte:aRetVal
        => system'core_routines'__byteconvertor.
        
    /// Converts a short integer into a byte value and saves the result into an output parameter
    stacksafe byte convert short:aValue vbyte:aRetVal
        => system'core_routines'__byteconvertor.
        
    /// Converts an integer into a byte value and saves the result into an output parameter
    stacksafe byte convert int:aValue vbyte:aRetVal
        => system'core_routines'__byteconvertor.

    /// Converts a long integer into a byte value and saves the result into an output parameter
    stacksafe convert long:aValue vbyte:aRetVal
        => system'core_routines'__byteconvertor.
        
    /// Converts a real number into a byte value and saves the result into an output parameter
    stacksafe convert real:aValue vbyte:aRetVal
        => system'core_routines'__byteconvertor.
        
    /// Converts a literal value into a byte value and saves the result into an output parameter
    stacksafe convert literal:aValue vbyte:aRetVal
        => system'core_routines'__byteconvertor.

    /// Converts a wide literal value into a byte value and saves the result into an output parameter
    stacksafe convert wide:aValue vbyte:aRetVal
        => system'core_routines'__byteconvertor.
        
    /// Returns the parameter        
    stacksafe byte convert byte:anInt
        = ByteNumber new byte:anInt.
        
    /// Converts a character into a byte value
    stacksafe byte convert char:aValue
    [
        byte aRetVal := 0.
        $self convert char:aValue vbyte:aRetVal.
        
        ^ ByteNumber new byte:aRetVal
    ]

    /// Converts a short integer into a byte value
    stacksafe byte convert short:aValue
    [
        byte aRetVal := 0.
        $self convert short:aValue vbyte:aRetVal.
        
        ^ ByteNumber new byte:aRetVal
    ]
        
    /// Converts an integer into a byte value
    stacksafe byte convert int:aValue
    [
        byte aRetVal := 0.
        $self convert int:aValue vbyte:aRetVal.
        
        ^ ByteNumber new byte:aRetVal
    ]

    /// Converts a long integer into a byte value
    stacksafe byte convert long:aValue
    [
        byte aRetVal := 0.
        $self convert long:aValue vbyte:aRetVal.
        
        ^ ByteNumber new byte:aRetVal
    ]

    /// Converts a real number into a byte value
    stacksafe byte convert real:aValue
    [
        byte aRetVal := 0.
        $self convert real:aValue vbyte:aRetVal.
        
        ^ ByteNumber new byte:aRetVal
    ]

    /// Converts a literal value into a byte value
    stacksafe byte convert literal:aLiteral
    [
        byte aValue := 0.
        $self convert literal:aLiteral vbyte:aValue.
        
        ^ ByteNumber new byte:aValue
    ]
                
    /// Converts a wide literal value into a byte value
    stacksafe byte convert wide:aLiteral
    [
        byte aValue := 0. 
        $self convert wide:aLiteral vbyte:aValue.
        
        ^ ByteNumber new byte:aValue
    ]
        
    byte convert : anObject
        = anObject cast:%convert to:$self.
}.

/// A 16bit integer convertor
class shortConvertor =
{
    /// Converts a character into a short integer value and saves the result into an output parameter
    stacksafe convert char:aValue vshort:aRetVal
        => system'core_routines'__shortconvertor.
        
    /// Converts a byte value into a short integer value and saves the result into an output parameter
    stacksafe convert byte:aValue vshort:aRetVal
        => system'core_routines'__shortconvertor.
        
    /// Converts an integer into a short integer value and saves the result into an output parameter
    stacksafe convert int:aValue vshort:aRetVal
        => system'core_routines'__shortconvertor.
        
    /// Converts a long integer into a short integer value and saves the result into an output parameter
    stacksafe convert long:aValue vshort:aRetVal
        => system'core_routines'__shortconvertor.
        
    /// Converts a real number into a short integer value and saves the result into an output parameter
    stacksafe convert real:aValue vshort:aRetVal
        => system'core_routines'__shortconvertor.

    /// Converts a literal string into a short integer value and saves the result into an output parameter
    stacksafe convert literal:aValue vshort:aRetVal
        => system'core_routines'__shortconvertor.
        
    /// Converts a wide literal string into a short integer value and saves the result into an output parameter
    stacksafe convert wide:aValue vshort:aRetVal
        => system'core_routines'__shortconvertor.
        
    /// Returns a short integer value        
    stacksafe short convert short:anInt
        = ShortNumber new short:anInt.
        
    /// Converts a byte value into a short integer value
    stacksafe short convert byte:aValue
    [
        short aRetVal := 0.
        $self convert byte:aValue vshort:aRetVal.
        
        ^ ShortNumber new short:aRetVal
    ]

    /// Converts an integer into a short integer value
    stacksafe short convert int:aValue
    [
        short aRetVal := 0.
        $self convert int:aValue vshort:aRetVal.
        
        ^ ShortNumber new short:aRetVal
    ]

    /// Converts a long integer into a short integer value
    stacksafe short convert long:aValue
    [
        short aRetVal := 0.
        $self convert long:aValue vshort:aRetVal.
        
        ^ ShortNumber new short:aRetVal
    ]
        
    /// Converts a real number into a short integer value
    stacksafe short convert real:aValue
    [
        short aRetVal := 0.
        $self convert real:aValue vshort:aRetVal.
        
        ^ ShortNumber new short:aRetVal
    ]
        
    /// Converts a character into a short integer value
    stacksafe short convert char:aValue
    [
        short aRetVal := 0.
        $self convert char:aValue vshort:aRetVal.
        
        ^ ShortNumber new short:aRetVal
    ]
        
    /// Converts a literal string into a short integer value
    stacksafe short convert literal:aValue
    [
        short aRetVal := 0.
        $self convert literal:aValue vshort:aRetVal.
        
        ^ ShortNumber new short:aRetVal
    ]
        
    /// Converts a wide literal string into a short integer value
    stacksafe short convert wide:aValue
    [
        short aRetVal := 0.
        $self convert wide:aValue vshort:aRetVal.
        
        ^ ShortNumber new short:aRetVal
    ]
        
    short convert : anObject
        = anObject cast:%convert to:$self.
}.

/// An Unicode character convertor
class charConvertor =
{
    /// Converts a byte value into a character and saves the result into an output parameter
    stacksafe convert byte:aValue vchar:aRetVal
        => system'core_routines'__charconvertor.

    /// Converts a short integer into a character and saves the result into an output parameter
    stacksafe convert short:aValue vchar:aRetVal
        => system'core_routines'__charconvertor.

    /// Converts an integer into a character and saves the result into an output parameter
    stacksafe convert int:aValue vchar:aRetVal
        => system'core_routines'__charconvertor.
        
    /// Returns a parameter        
    stacksafe char convert char:aChar
        = CharValue new char:aChar.
  
    stacksafe $convert int:anIndex literal:aLiteral vchar:char
        => system'core_routines'__charconvertor.
        
    stacksafe $convert int:anIndex bytearray:aLiteral vchar:char
        => system'core_routines'__charconvertor.
  
    stacksafe $convert int:anIndex wide:aLiteral vchar:char
        => system'core_routines'__charconvertor.

    stacksafe $convert int:anIndex shortarray:aLiteral vchar:char
        => system'core_routines'__charconvertor.

    stacksafe $save char:aChar int:anIndex shortarray:aCharArray vint:aLength
        => system'core_routines'__charconvertor.
    
    stacksafe $save char:aChar int:anIndex bytearray:aCharArray vint:aLength
        => system'core_routines'__charconvertor.

    /// Converts a literal string into a character                
    stacksafe char convert literal:aValue
    [
        char aRetVal := $0.
        $self $convert int:0 literal:aValue vchar:aRetVal.
        
        int l1 := 0.
        int l2 := aValue length.
        aRetVal readLengthTo vint:l1.
        
        (l1 != l2)
            ? [ system'InvalidArgumentException new; raise ].
        
        ^ CharValue new char:aRetVal
    ]        
        
    /// Converts a wide literal string into a character                
    stacksafe char convert wide:aValue
    [
        char aRetVal := $0.
        $self $convert int:0 wide:aValue vchar:aRetVal.

        int l1 := 0.
        int l2 := aValue length.
        aRetVal readWideLengthTo vint:l1.
                
        (l1 != l2)
            ? [ system'InvalidArgumentException new; raise ].
        
        ^ CharValue new char:aRetVal
    ]        
        
    /// Converts a byte value into a character
    stacksafe char convert byte:aValue
    [
        char aRetVal := $0.
        $self convert byte:aValue vchar:aRetVal.
        
        ^ CharValue new char:aRetVal
    ]        
        
    /// Converts a short integer into a character
    stacksafe char convert short:aValue
    [
        char aRetVal := $0.
        $self convert short:aValue vchar:aRetVal.
        
        ^ CharValue new char:aRetVal
    ]        
        
    /// Converts an integer into a character
    stacksafe char convert int:aValue
    [
        char aRetVal := $0.
        $self convert int:aValue vchar:aRetVal.
        
        ^ CharValue new char:aRetVal
    ]
        
    char convert : anObject
        = anObject cast:%convert to:$self.
}.

/// An integer convertor
class intConvertor =
{
    /// Converts a literal string into an integer and saves the result into an output parameter
    stacksafe convert literal:aValue vint:aRetVal
        => system'core_routines'__intconvertor.

    /// Converts a literal string into an integer and saves the result into an output parameter
    stacksafe convert literal:value int:base vint:retVal
        => system'core_routines'__intconvertor.

    /// Converts a wide literal string into an integer and saves the result into an output parameter
    stacksafe convert wide:aValue vint:aRetVal
        => system'core_routines'__intconvertor.

    /// Converts a wide literal string into an integer and saves the result into an output parameter
    stacksafe convert wide:value int:base vint:retVal
        => system'core_routines'__intconvertor.

    /// Converts a character value into an integer and saves the result into an output parameter
    stacksafe convert char:aValue vint:aRetVal
        => system'core_routines'__intconvertor.

    /// Converts a short integer value into an integer and saves the result into an output parameter
    stacksafe convert short:aValue vint:aRetVal
        => system'core_routines'__intconvertor.

    /// Converts a byte value into an integer and saves the result into an output parameter
    stacksafe convert byte:aValue vint:aRetVal
        => system'core_routines'__intconvertor.

    /// Converts a long integer into an integer and saves the result into an output parameter
    stacksafe convert long:aValue vint:aRetVal
        => system'core_routines'__intconvertor.
        
    /// Converts a real number into an integer and saves the result into an output parameter
    stacksafe convert real:aReal vint:aRetVal
        => system'core_routines'__intconvertor.

    /// Converts a literal string into an integer
    stacksafe int convert literal:aLiteral
    [
        int aRetVal := 0.
        $self convert literal:aLiteral vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]
        
    /// Converts a literal string into an integer
    stacksafe int convert literal:literal base:base
    [
        int retVal := 0.
        $self convert literal:literal int:base vint:retVal.
        
        ^ IntNumber new int:retVal
    ]
        
    /// Converts a wide literal string into an integer
    stacksafe int convert wide:aLiteral
    [
        int aRetVal := 0.
        $self convert wide:aLiteral vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]

    /// Converts a wide literal string into an integer
    stacksafe int convert wide:literal base:base
    [
        int retVal := 0.
        $self convert wide:literal int:base vint:retVal.
        
        ^ IntNumber new int:retVal
    ]
        
    /// Converts a character value into an integer
    stacksafe int convert char:aValue
    [
        int aRetVal := 0.
        $self convert char:aValue vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]
        
    /// Converts a byte value into an integer
    stacksafe int convert byte:aValue
    [
        int aRetVal := 0.
        $self convert byte:aValue vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]
        
    /// Converts a short integer value into an integer
    stacksafe int convert short:aValue
    [
        int aRetVal := 0.
        $self convert short:aValue vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]
        
    /// Converts a long integer value into an integer and saves the result into an output parameter
    stacksafe int convert long:aValue
    [
        int aRetVal := 0.
        $self convert long:aValue vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]

    /// Converts a long integer into an integer
    stacksafe int convert real:aReal
    [
        int aRetVal := 0.
        $self convert real:aReal vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]

    /// Returns a parameter                                    
    stacksafe int convert int:anInt
        = IntNumber new int:anInt.
        
    int convert : anObject
        = anObject cast:%convert to:$self.
}.

/// A long integer convertor
class longConvertor =
{
    /// Converts a literal string into a long integer and saves the result into an output parameter
    stacksafe convert literal:aValue vlong:aRetVal
        => system'core_routines'__longconvertor.

    /// Converts a literal string into a long integer and saves the result into an output parameter
    stacksafe convert literal:value int:base vlong:retVal
        => system'core_routines'__longconvertor.

    /// Converts a wide literal string into a long integer and saves the result into an output parameter
    stacksafe convert wide:aValue vlong:aRetVal
        => system'core_routines'__longconvertor.

    /// Converts a wide literal string into a long integer and saves the result into an output parameter
    stacksafe convert wide:value int:base vlong:retVal
        => system'core_routines'__longconvertor.

    stacksafe convert int:aValue vlong:aRetVal
        => system'core_routines'__longconvertor.

    stacksafe convert short:aValue vlong:aRetVal
        => system'core_routines'__longconvertor.

    stacksafe convert byte:aValue vlong:aRetVal
        => system'core_routines'__longconvertor.

    /// Converts a real number into a long integer and saves the result into an output parameter
    stacksafe convert real:aValue vlong:aRetVal
        => system'core_routines'__longconvertor.

    /// Converts an integer value into a long integer
    stacksafe long convert int:aValue
    [
        long n := 0l.
        $self convert int:aValue vlong:n.
        
        ^ LongNumber new long:n
    ]

    /// Converts an integer value into a long integer
    stacksafe long convert byte:aValue
    [
        long n := 0l.
        $self convert byte:aValue vlong:n.
        
        ^ LongNumber new long:n
    ]

    /// Converts an integer value into a long integer
    stacksafe long convert short:aValue
    [
        long n := 0l.
        $self convert short:aValue vlong:n.
        
        ^ LongNumber new long:n
    ]

    /// Converts a real number into a long integer
    stacksafe convert real:aValue
    [
        long aRetVal := 0l.
        $self convert real:aValue vlong:aRetVal.
        
        ^ LongNumber new long:aRetVal
    ]

    /// Converts a character into a long integer
    stacksafe convert char:aValue
    [
        int aRetVal := 0.
        intConvertor convert char:aValue vint:aRetVal.
        
        ^ LongNumber new int:aRetVal
    ]

    /// Converts a literal string into a long integer
    stacksafe long convert literal:aLiteral
    [
        long aRetVal := 0l.
        $self convert literal:aLiteral vlong:aRetVal.
        
        ^ LongNumber new long:aRetVal
    ]

    /// Converts a literal string into a long integer
    stacksafe long convert literal:literal base:base
    [
        long retVal := 0l.
        $self convert literal:literal int:base vlong:retVal.
        
        ^ LongNumber new long:retVal
    ]

    /// Converts a wide literal string into a long integer
    stacksafe long convert wide:aLiteral
    [
        long aRetVal := 0l.
        $self convert wide:aLiteral vlong:aRetVal.
        
        ^ LongNumber new long:aRetVal
    ]

    /// Converts a wide literal string into a long integer
    stacksafe long convert wide:literal base:base
    [
        long retVal := 0l.
        $self convert wide:literal int:base vlong:retVal.
        
        ^ LongNumber new long:retVal
    ]
    
    /// Returns a parameter
    stacksafe long convert long:aLong
        = LongNumber new long:aLong.
        
    long convert : anObject
        = anObject cast:%convert to:$self.
}.

/// A real number convertor
class realConvertor =
{
    /// Converts an integer into a real number and saves the result into an output parameter
    stacksafe convert int:aValue vreal:aRetVal
        => system'core_routines'__realconvertor.

    /// Converts an integer into a real number and saves the result into an output parameter
    stacksafe convert long:aValue vreal:aRetVal
        => system'core_routines'__realconvertor.

    /// Converts an integer into a real number and saves the result into an output parameter
    stacksafe convert byte:aValue vreal:aRetVal
        => system'core_routines'__realconvertor.

    /// Converts an integer into a real number and saves the result into an output parameter
    stacksafe convert short:aValue vreal:aRetVal
        => system'core_routines'__realconvertor.

    /// Converts a literal string into a real number and saves the result into an output parameter
    stacksafe convert literal:aValue vreal:aRetVal
        => system'core_routines'__realconvertor.

    /// Converts a wide literal string into a real number and saves the result into an output parameter
    stacksafe convert wide:aValue vreal:aRetVal
        => system'core_routines'__realconvertor.
        
    /// Converts a literal string into a real number
    stacksafe real convert literal:aLiteral
    [
        real aRetVal := 0.0r.
        $self convert literal:aLiteral vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]
        
    /// Converts a wide literal string into a real number
    stacksafe real convert wide:aLiteral
    [
        real aRetVal := 0.0r.
        $self convert wide:aLiteral vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]

    /// Returns a parameter
    stacksafe real convert real:aReal
        = RealNumber new real:aReal.
        
    /// Converts an integer into a real number
    stacksafe real convert int:anInt
    [
        real aRetVal := 0.0r.
        $self convert int:anInt vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]

    /// Converts a long integer into a real number
    stacksafe real convert long:aLong
    [
        real aRetVal := 0.0r.
        $self convert long:aLong vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]

    /// Converts a byte into a real number
    stacksafe real convert byte:aValue
    [
        real aRetVal := 0.0r.
        $self convert byte:aValue vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]

    /// Converts a short integer into a real number
    stacksafe real convert short:aValue
    [
        real aRetVal := 0.0r.
        $self convert short:aValue vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]
        
    real convert : anObject
        = anObject cast:%convert to:$self.
}.

// --- realOpController ---

class realOpController =
{
    stacksafe exp real:aReal vreal:aRetVal
        => system'core_routines'__real.
        
    stacksafe abs real:aReal vreal:aRetVal
        => system'core_routines'__real.
        
    stacksafe ln real:aReal vreal:aRetVal
        => system'core_routines'__real.
        
    stacksafe round real:aReal vreal:aRetVal
        => system'core_routines'__real.
        
    stacksafe truncate real:aReal vreal:aRetVal
        => system'core_routines'__real.
        
    stacksafe sin real:aReal vreal:aRetVal
        => system'core_routines'__real.
        
    stacksafe cos real:aReal vreal:aRetVal
        => system'core_routines'__real.
        
    stacksafe arctan real:aReal vreal:aRetVal
        => system'core_routines'__real.
                
    stacksafe sqrt real:aReal vreal:aRetVal
        => system'core_routines'__real.
        
    stacksafe pi vreal:aRetVal
        => system'core_routines'__real.
        
    stacksafe embeddable real pi
    [
        real aRetVal := 0.0r.
        $self pi vreal:aRetVal.
        
        ^ aRetVal
    ]        
    
    stacksafe embeddable real e
    [
        real aRetVal := 0.0r.
        $self exp real:1.0r vreal:aRetVal.
        
        ^ aRetVal
    ]        
}.

real pi_value = realOpController pi.

real e_value = realOpController e.

// --- obsolete variables ---

class Integer =
{
    vint new
        = 0.
        
    vint new int:n
        = n.
        
    vint new:n
        = n int.
}.

class Real =
{
    vreal new
        = 0.0r.
        
    vreal new : n
        = n real.
        
    vreal new int:n
        = n real.
        
    vreal new real:n
        = n.
}.