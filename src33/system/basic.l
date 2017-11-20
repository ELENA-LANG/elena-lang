// base variable mixin
class BaseVariable
{
    // should be overridden
    value = $nil.
}

//template<BaseVariable> var :: template_subj
//{
//    template_subj $value.
//    
//    sealed embeddable template_subj = $value.
//    
//    value = $value.
//    
//    stacksafe explicit template_subj:val
//    [
//        $value := val.
//    ]
//    
//    stacksafe set template_subj:val
//    [
//        $value := val.
//    ]
//    
//    stacksafe append template_subj:val
//    [
//        $value := $value + val.
//    ]
//    
//    multi append:val
//        <= append template_subj:val.
//    
//    stacksafe reduce template_subj:val
//    [
//        $value := $value - val.
//    ]
//    
//    multi reduce:val
//        <= reduce template_subj:val.
//    
//    bool equal : anObject
//        => $value.
//
//    embeddable dispatch => $value.
//}
//
//template<BaseVariable> ref :: template_subj
//{
//    template_subj $value.
//    
//    stacksafe explicit template_subj:val
//    [
//        $value := val.
//    ]
//    
//    stacksafe set template_subj:val
//    [
//        $value := val.
//    ]
//    
//    value = $value.
//    
//    sealed embeddable template_subj = $value.
//    
//    bool equal : anObject
//        => $value.
//
//    embeddable dispatch => $value.
//}
//
//// === Numeric Variables ===
//
//type vbyte  = var<ByteNumber>.
//type vshort = var<ShortNumber>.
//type vint   = var<IntNumber>.
//type vlong  = var<LongNumber>.
//type vreal  = var<RealNumber>.
//type vchar  = var<CharValue>.

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
    bool or : anExpr = $self.

    /// Executes logical AND operation
    bool and : anExpr = anExpr bool.

    /// Executes logical XOR operation
    bool xor : aBool
    [
        aBool bool ? [ ^ false ].
        
        ^ $self
    ]

    /// Determines whether the specified object is equal to the current object boolean value.
    bool equal : anExpr = anExpr bool.

    /// Determines whether the specified object is equal to the current object boolean value.
    bool equal bool:aBool = aBool.

    /// Always returns false.
    bool inverted = false.

    /// Returns literal representation.
    literal = "true".

    /// Returns literal representation.
    wide = "true"w.

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
    bool or : anExpr = anExpr bool.

    /// Executes logical AND operation
    ///
    /// Always return false
    bool and : anExpr = $self.

    /// Executes logical XOR operation
    bool xor : anExpr 
    [
        anExpr bool ? [ ^ true ].
        
        ^ $self
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    bool equal : anExpr
    [
        anExpr bool ? [ ^ $self ].
        
        ^ true
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    bool equal bool:anExpr
    [
        ^ anExpr inverted
    ]

    /// Always returns true
    bool inverted = true.

    /// Returns literal representation
    literal = "false".
    
    /// Returns literal representation
    wide = "false"w.
    
    /// Returns itself
    clone = $self.
}.

// === basic data classes ===

// --- BaseValue ---

// A base value
class BaseValue
{
    bool equal(BaseVariable var)
        <= equal(var value).
    
    bool less(BaseVariable var)
        <= equal(var value).
    
    /// Compares the values
    bool multi equal : anObject
        = false.

    /// Compares the values
    bool multi less : anObject
    [
        system'NotSupportedException new; raise.
    ]

    /// Compares the values
    bool greater : anObject = anObject less:$self.

    bool notless : anObject = $self less:anObject; inverted.

    bool notgreater : anObject = $self greater:anObject; inverted.
}

// --- BaseNumber ---

/// A base numeric value
class BaseNumber :: BaseValue
{
    add(BaseVariable var)
        <= add(var value).

    subtract(BaseVariable var)
        <= subtract(var value).
    
    multiply(BaseVariable var)
        <= multiply(var value).

    divide(BaseVariable var)
        <= divide(var value).
    
    /// Adds the value
    multi add : anObject
    [
        system'NotSupportedException new; raise.
    ]

    /// Subtracts the value
    multi subtract : anObject
    [
        system'NotSupportedException new; raise.
    ]

    /// Multiply by the value
    multi multiply : anObject
    [
        system'NotSupportedException new; raise.
    ]

    /// Divides into the value
    multi divide : anObject
    [
        system'NotSupportedException new; raise.
    ]
    
//    negative
//        = 0 - self.
}

// --- IntBaseNumber ---

/// A base integer value
class IntBaseNumber :: BaseNumber
{
//    and(BaseVariable var)
//        <= and(var value).
//        
//    or(BaseVariable var)
//        <= or(var value).
//        
//    xor(BaseVariable var)
//        <= xor(var value).
//    
//    /// Execute bitwise AND operation
//    multi and : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//
//    /// Execute bitwise OR operation
//    multi or : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//    
//    /// Execute bitwise XOR operation
//    multi xor : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//    
//    /// Returns true if all the mask bits are set
//    bool allMask : anOperand
//        = $self and:anOperand == anOperand.
//        
//    /// Returns true if any of the mask bits are set    
//    bool anyMask : anOperand
//        = 0 != $self and:anOperand.
//        
//    stacksafe shiftLeft:n
//        <= shiftLeft int:n.        
//        
//    stacksafe shiftRight:n
//        <= shiftRight int:n.        
}

// --- ByteNumber ---

/// An unsigned 8 bit integer
sealed struct ByteNumber :: IntBaseNumber
{
    __int<1> theValue.

//    stacksafe explicit(IntNumber n)
//    [
//        theValue := n && 255.
//    ]
//
//    /// Creates the object with a default value (0)
//    constructor new
//        = 0.
//
//    /// Returns the minimal value (0)
//    constructor min
//        = 0.
//
//    /// Returns the maxial value (255)
//    constructor max
//        = 255.
//    
//    /// Creates the object with a specified value
//    stacksafe constructor new byte:value
//        = value.
//
//    /// Creates the object with a specified value
//    constructor new : aValue
//        <= new byte:aValue.
//
//    /// Returns the value as a short integer
//    short = ShortNumber new byte:$self.
//
//    /// Returns the value as an integer
//    int = IntNumber new byte:$self.
//
//    /// Returns the value as an integer
//    uint = UIntNumber new byte:$self.
//
//    /// Returns the value as a long integer
//    long = LongNumber new byte:$self.
//
//    /// Returns the value as a real number
//    real = RealNumber new byte:$self.
//
//    stacksafe bool equal(BaseVariable v)
//        <= equal(v value).
//
//    stacksafe bool equal byte:anOperand
//        = theValue == anOperand.
//
//    stacksafe bool equal short:anOperand
//        = ShortNumber new byte:$self; equal short:anOperand.
//        
//    /// Compares the value
//    stacksafe bool equal int:anOperand
//        = IntNumber new byte:$self; less int:anOperand.
//        
//    /// Compares the value
//    stacksafe bool equal real:anOperand
//        = RealNumber new byte:$self; equal real:anOperand.
//        
//    multi bool equal : o
//    [
//        var n := o byte \ back:$nil.
//        ($nil == n)
//            ? [ ^ false ]
//            ! [ ^ $self equal byte:n ]
//    ]
//
//    multi bool less : n
//        <= less byte:n.                
//
//    stacksafe bool less(BaseVariable v)
//        <= less(v value).
//
//    stacksafe bool less byte:anOperand
//        = theValue < anOperand.
//
//    stacksafe bool less short:anOperand
//        = ShortNumber new byte:$self; less short:anOperand.
//
//    /// Compares the value
//    stacksafe bool less int:anOperand
//        = IntNumber new byte:$self; less int:anOperand.
//        
//    /// Compares the value
//    stacksafe bool equal long:anOperand
//        = LongNumber new byte:$self; equal long:anOperand.
//        
//    /// Compares the value
//    stacksafe bool less long:anOperand
//        = LongNumber new byte:$self; less long:anOperand.
//
//    /// Compares the value
//    stacksafe bool less real:anOperand
//        = RealNumber new byte:$self; less real:anOperand.
//
//    multi add:n
//        <= add byte:n.
//
//    multi subtract:n
//        <= subtract byte:n.
//
//    multi multiply:n
//        <= multiply byte:n.
//
//    multi divide:n
//        <= divide byte:n.
//
//    stacksafe byte add(BaseVariable v)
//        <= add(v value).
//
//    stacksafe byte subtract(BaseVariable v)
//        <= subtract(v value).
//
//    stacksafe byte multiply(BaseVariable v)
//        <= multiply(v value).
//
//    stacksafe byte divide(BaseVariable v)
//        <= divide(v value).
//
//    /// Returns the sum
//    stacksafe byte add byte:anOperand
//        = theValue + anOperand.
//
//    /// Returns the sum
//    stacksafe byte subtract byte:anOperand
//        = theValue - anOperand.
//
//    /// Returns the product
//    stacksafe byte multiply byte:anOperand
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    stacksafe byte divide byte:anOperand
//        = theValue / anOperand.
//
//    /// Returns the sum
//    stacksafe short add short:anOperand
//        = ShortNumber new byte:$self; add short:anOperand.
//
//    /// Returns the difference
//    stacksafe short subtract short:anOperand
//        = ShortNumber new byte:$self; subtract short:anOperand.
//
//    /// Returns the product
//    stacksafe short multiply short:anOperand
//        = ShortNumber new byte:$self; multiply short:anOperand.
//
//    /// Returns the result of integer division
//    stacksafe short divide short:anOperand
//        = ShortNumber new byte:$self; divide short:anOperand.
//
//    /// Returns the sum
//    stacksafe int add int:anOperand
//        = IntNumber new byte:$self; add int:anOperand.
//
//    /// Returns the difference
//    stacksafe int subtract int:anOperand
//        = IntNumber new byte:$self; subtract int:anOperand.
//
//    /// Returns the product
//    stacksafe int multiply int:anOperand
//        = IntNumber new byte:$self; multiply int:anOperand.
//
//    /// Returns the result of integer division
//    stacksafe int divide int:anOperand
//        = IntNumber new byte:$self; divide int:anOperand.
//
//    /// Returns the sum
//    stacksafe long add long:anOperand
//        = LongNumber new byte:$self; add long:anOperand.
//
//    /// Returns the difference
//    stacksafe long subtract long:anOperand
//        = LongNumber new byte:$self; subtract long:anOperand.
//
//    /// Returns the product
//    stacksafe long multiply long:anOperand
//        = LongNumber new byte:$self; multiply long:anOperand.
//
//    /// Returns the result of integer division
//    stacksafe long divide long:anOperand
//        = LongNumber new byte:$self; divide long:anOperand.
//
//    /// Returns the sum
//    stacksafe real add real:anOperand
//        = RealNumber new byte:$self; add real:anOperand.
//
//    /// Returns the difference
//    stacksafe real subtract real:anOperand
//        = RealNumber new byte:$self; subtract real:anOperand.
//
//    /// Returns the product
//    stacksafe real multiply real:anOperand
//        = RealNumber new byte:$self; multiply real:anOperand.
//
//    /// Returns the result of integer division
//    stacksafe real divide real:anOperand
//        = RealNumber new byte:$self; divide real:anOperand.
//
//    multi and:n
//        <= and byte:n.
//        
//    multi or:n
//        <= or byte:n.
//        
//    multi xor:n
//        <= xor byte:n.
//        
//    stacksafe byte and(BaseVariable v)
//        <= and(v value).
//        
//    stacksafe byte or(BaseVariable v)
//        <= or(v value).
//        
//    stacksafe byte xor(BaseVariable v)
//        <= xor(v value).
//        
//    /// Returns the result of bitwise AND operation
//    stacksafe byte and byte:anOperand
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe byte or byte:anOperand
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe byte xor byte:anOperand
//        = theValue ^^ anOperand.
//
//    /// Returns the result of bitwise AND operation
//    stacksafe short and short:anOperand
//        = ShortNumber new byte:$self; and short:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe short or short:anOperand
//        = ShortNumber new byte:$self; or short:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe short xor short:anOperand
//        = ShortNumber new byte:$self; xor short:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    stacksafe int and int:anOperand
//        = IntNumber new byte:$self; and int:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe int or int:anOperand
//        = IntNumber new byte:$self; or int:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe int xor int:anOperand
//        = IntNumber new byte:$self; xor int:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    stacksafe long and long:anOperand
//        = LongNumber new byte:$self; and long:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe long or long:anOperand
//        = LongNumber new byte:$self; or long:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe long xor long:anOperand
//        = LongNumber new byte:$self; xor long:anOperand.
//
//    stacksafe byte shiftLeft int:val
//    [
//        ^ theValue << val
//    ]
//
//    stacksafe byte shiftRight int:val
//    [
//        ^ theValue >> val
//    ]
//    
//    /// Bitwise inversion
//    stacksafe byte inverted
//        => system'core_routines'__byte.
//        
//    /// Returns the literal presentation
//    literal = system'literalConvertor convert byte:$self.
//        
//    /// Returns the literal presentation
//    wide = system'wideConvertor convert byte:$self.
//
//    /// Clones the value
//    clone = ByteNumber new byte:$self.
//
//    // Reads the object hash code into the output parameter
//    stacksafe readHashCodeTo vint:aCode 
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        $self readHashCodeTo vint:aCode.
//        
//        ^ aCode
//    ]
}

//// --- ShortNumber ---
//
///// A signed 16 bit integer
//sealed struct ShortNumber :: IntBaseNumber
//{
//    __int& theValue(2).
//
//    stacksafe explicit(IntNumber n)
//    [
//        (n < 0)
//            ? [ 
//                theValue := n && 0FFFFh. 
//                
//                theValue := theValue || 0FFFF0000h
//            ]
//            ! [ theValue := n && 0FFFFh ]
//    ]
//    
//    /// Returns the minimal value (-32768)
//    constructor min
//        = -32768.
//
//    /// Returns the maxial value (32767)
//    constructor max
//        = 32767.
//
//    /// Creates the object with specified value
//    stacksafe constructor new byte:value
//        = shortConvertor convert byte:value.
//        
//    /// Creates the object with specified value
//    stacksafe constructor new short:value
//        = value.
//
//    /// Creates the object with a default value (0)
//    constructor new
//        = 0.
//
//    /// Creates the object with specified value
//    constructor new : anObject
//        <= new short:anObject.
//
//    /// Returns the byte value        
//    byte = byteConvertor convert short:$self.
//    
//    /// Returns the integer value
//    int = IntNumber new short:$self.
//    
//    /// Returns the integer value
//    uint = UIntNumber new short:$self.
//    
//    /// Returns the long integer value
//    long = LongNumber new short:$self.
//    
//    /// Returns the value as a real number
//    real = RealNumber new short:$self.
//
//    multi bool equal:o
//    [
//        var n := o short \ back:$nil.
//        ($nil == n)
//            ? [ ^ false ]
//            ! [ ^ $self equal short:n ]
//    ]
//
//    multi bool less:n
//        <= less short:n.
//
//    stacksafe bool equal(BaseVariable v)
//        <= equal(v value).
//                
//    stacksafe bool less(BaseVariable v)
//        <= equal(v value).
//
//    /// Compares the value
//    stacksafe bool equal short:anOperand 
//        = theValue == anOperand.
//        
//    /// Compares the value
//    stacksafe bool less short:anOperand
//        = theValue < anOperand.
//        
//    /// Compares the value
//    stacksafe bool equal int:anOperand 
//        = IntNumber new short:$self; equal int:anOperand.
//        
//    /// Compares the value
//    stacksafe bool less int:anOperand
//        = IntNumber new short:$self; less int:anOperand.
//    
//    /// Compares the value
//    stacksafe bool equal byte:anOperand 
//        = $self equal short:(ShortNumber new byte:anOperand).
//        
//    /// Compares the value
//    stacksafe bool less byte:anOperand
//        = $self less short:(ShortNumber new byte:anOperand).
//    
//    /// Compares the value
//    stacksafe bool equal long:anOperand 
//        = LongNumber new short:$self; equal long:anOperand.
//        
//    /// Compares the value
//    stacksafe bool less long:anOperand
//        = LongNumber new short:$self; less long:anOperand.
//    
//    /// Compares the value
//    stacksafe bool equal real:anOperand 
//        = RealNumber new short:$self; equal real:anOperand.
//        
//    /// Compares the value
//    stacksafe bool less real:anOperand
//        = RealNumber new short:$self; less real:anOperand.
//
//    multi add:n
//        <= add short:n.
//
//    multi subtract:n
//        <= subtract short:n.
//
//    multi multiply:n
//        <= multiply short:n.
//
//    multi divide:n
//        <= divide short:n.
//
//    stacksafe short add(BaseVariable v)
//        <= add(v value).
//
//    stacksafe short subtract(BaseVariable v)
//        <= subtract(v value).
//
//    stacksafe short multiply(BaseVariable v)
//        <= multiply(v value).
//
//    stacksafe short divide(BaseVariable v)
//        <= divide(v value).
//
//    /// Returns the sum
//    stacksafe short add short:anOperand
//        = theValue + anOperand.
//
//    /// Returns the sum
//    stacksafe short subtract short:anOperand
//        = theValue - anOperand.
//
//    /// Returns the product
//    stacksafe short multiply short:anOperand
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    stacksafe short divide short:anOperand
//        = theValue / anOperand.
//
//    /// Returns the sum
//    stacksafe add byte:anOperand
//        = $self add short:(ShortNumber new byte:anOperand).
//
//    /// Returns the difference
//    stacksafe subtract byte:anOperand
//        = $self subtract short:(ShortNumber new byte:anOperand).
//
//    /// Returns the product
//    stacksafe multiply byte:anOperand
//        = $self multiply short:(ShortNumber new byte:anOperand).
//
//    /// Returns the result of integer division
//    stacksafe divide byte:anOperand
//        = $self divide short:(ShortNumber new byte:anOperand).
//
//    /// Returns the sum
//    stacksafe add int:anOperand
//        = IntNumber new short:$self; add int:anOperand.
//
//    /// Returns the difference
//    stacksafe subtract int:anOperand
//        = IntNumber new short:$self; subtract int:anOperand.
//
//    /// Returns the product
//    stacksafe multiply int:anOperand
//        = IntNumber new short:$self; multiply int:anOperand.
//        
//    /// Returns the result of integer division
//    stacksafe divide int:anOperand
//        = IntNumber new short:$self; divide int:anOperand.
//
//    /// Returns the sum
//    stacksafe add long:anOperand
//        = LongNumber new short:$self; add long:anOperand.
//
//    /// Returns the difference
//    stacksafe subtract long:anOperand
//        = LongNumber new short:$self; subtract long:anOperand.
//
//    /// Returns the product
//    stacksafe multiply long:anOperand
//        = LongNumber new short:$self; multiply long:anOperand.
//        
//    /// Returns the result of integer division
//    stacksafe divide long:anOperand
//        = LongNumber new short:$self; divide long:anOperand.
//
//    /// Returns the sum
//    stacksafe add real:anOperand
//        = RealNumber new short:$self; add real:anOperand.
//
//    /// Returns the difference
//    stacksafe subtract real:anOperand
//        = RealNumber new short:$self; subtract real:anOperand.
//
//    /// Returns the product
//    stacksafe multiply real:anOperand
//        = RealNumber new short:$self; multiply real:anOperand.
//        
//    /// Returns the result of integer division
//    stacksafe divide real:anOperand
//        = RealNumber new short:$self; divide real:anOperand.
//
//    multi and:n
//        <= and short:n.
//        
//    multi or:n
//        <= divide short:n.
//        
//    multi xor:n
//        <= xor short:n.
//        
//    stacksafe short and(BaseVariable v)
//        <= and(v value).
//        
//    stacksafe short or(BaseVariable v)
//        <= divide(v value).
//        
//    stacksafe short xor(BaseVariable v)
//        <= xor(v value).
//        
//    /// Returns the result of bitwise AND operation
//    stacksafe short and short:anOperand
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe short or short:anOperand
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe short xor short:anOperand
//        = theValue ^^ anOperand.
//        
//    /// Returns the result of bitwise AND operation
//    stacksafe and byte:anOperand
//        = $self and short:(ShortNumber new byte:anOperand). 
//
//    /// Returns the result of bitwise OR operation
//    stacksafe or byte:anOperand
//        = $self or short:(ShortNumber new byte:anOperand). 
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor byte:anOperand
//        = $self xor short:(ShortNumber new byte:anOperand). 
//        
//    /// Returns the result of bitwise AND operation
//    stacksafe and int:anOperand
//        = IntNumber new short:$self; and int:anOperand.        
//        
//    /// Returns the result of bitwise OR operation
//    stacksafe or int:anOperand
//        = IntNumber new short:$self; or int:anOperand.        
//        
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor int:anOperand
//        = IntNumber new short:$self; xor int:anOperand.        
//        
//    /// Returns the result of bitwise AND operation
//    stacksafe and long:anOperand
//        = LongNumber new short:$self; and long:anOperand.        
//        
//    /// Returns the result of bitwise OR operation
//    stacksafe or long:anOperand
//        = LongNumber new short:$self; or long:anOperand.        
//        
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor long:anOperand
//        = LongNumber new short:$self; xor long:anOperand.        
//
//    stacksafe short shiftLeft int:val
//    [
//        ^ theValue << val
//    ]
//
//    stacksafe short shiftRight int:val
//    [
//        ^ theValue >> val
//    ]
//    
//    /// Bitwise inversion
//    stacksafe short inverted
//        => system'core_routines'__short.  
//
//    /// Returns the literal presentation
//    literal = system'literalConvertor convert short:$self.
//
//    /// Returns the literal presentation
//    wide = system'wideConvertor convert short:$self.
//
//    /// Clones the value
//    clone = ShortNumber new short:$self.    
//
//    // Reads the object hash code into the output parameter
//    stacksafe readHashCodeTo vint:aCode 
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        $self readHashCodeTo vint:aCode.
//        
//        ^ aCode.
//    ]        
//}

// --- IntNumber ---

/// A signed 32 bit integer
sealed struct IntNumber :: IntBaseNumber
{
    __int<4> theValue.

//    stacksafe explicit(ByteNumber n)
//    [
//        theValue := n.
//    ]
//    
//    stacksafe explicit(ShortNumber n)
//    [
//        theValue := n.
//    ]
//    
//    /// Creates the object with the minimal value
//    constructor min
//        = -2147483648.
//
//    /// Creates the object with the maximal value
//    constructor max
//        = 2147483647.
//    
//    /// Creates the object with specified value
//    stacksafe constructor new int:value
//        = value.
//
//    /// Creates the object with specified value
//    stacksafe constructor new short:value
//        = intConvertor convert short:value.
//
//    /// Creates the object with specified value
//    stacksafe constructor new byte:value
//        = intConvertor convert byte:value.
//
//    /// Creates the object with a default value (0)
//    constructor new
//        <= new int:0.
//
//    /// Creates the object with specified value
//    constructor new : anObject
//        <= new int:anObject.
//
//    /// Returns the value as a short integer    
//    stacksafe short = shortConvertor convert int:$self.
//
//    /// Returns the byte value    
//    byte = byteConvertor convert int:$self.
//
//    /// Returns the long integer
//    long = LongNumber new int:$self.
//
//    /// Returns the real number
//    real = RealNumber new int:$self.
//
//    /// Returns the real number
//    uint = UIntNumber new int:$self.
//    
//    multi bool equal:o
//    [
//        var n := o int \ back:$nil.
//        ($nil == n)
//            ? [ ^ false ]
//            ! [ ^ $self equal int:n ]
//    ]
//
//    multi bool less:n
//        <= less int:n.
//        
//    stacksafe bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    stacksafe bool less(BaseVariable v)
//        <= less(v value).
//        
//    /// Compares the value
//    stacksafe bool equal int:anOperand 
//        = theValue == anOperand.
//        
//    /// Compares the value
//    stacksafe bool less int:anOperand
//        = theValue < anOperand.
//        
//    multi add:n
//        <= add int:n.
//
//    multi subtract:n
//        <= subtract int:n.
//
//    multi multiply:n
//        <= multiply int:n.
//
//    multi divide:n
//        <= divide int:n.
//        
//    stacksafe int add(BaseVariable v)
//        <= add(v value).
//
//    stacksafe int subtract(BaseVariable v)
//        <= subtract(v value).
//
//    stacksafe int multiply(BaseVariable v)
//        <= multiply(v value).
//
//    stacksafe int divide(BaseVariable v)
//        <= divide(v value).
//        
//    /// Returns the sum
//    stacksafe int add int:anOperand
//        = theValue + anOperand.
//
//    /// Returns the sum
//    stacksafe int subtract int:anOperand
//        = theValue - anOperand.
//
//    /// Returns the product
//    stacksafe int multiply int:anOperand
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    stacksafe int divide int:anOperand
//        = theValue / anOperand.
//
//    multi and:n
//        <= and int:n.
//        
//    multi or:n
//        <= divide int:n.
//        
//    multi xor:n
//        <= xor int:n.
//        
//    stacksafe int and(BaseVariable v)
//        <= and(v value).
//        
//    stacksafe int or(BaseVariable v)
//        <= or(v value).
//        
//    stacksafe int xor(BaseVariable v)
//        <= xor(v value).
//        
//    /// Returns the result of bitwise AND operation
//    stacksafe int and int:anOperand
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe int or int:anOperand
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe int xor int:anOperand
//        = theValue ^^ anOperand.    
//
//    /// Compares the integer value
//    stacksafe bool equal short:value 
//        = $self equal int:(IntNumber new short:value).
//
//    /// Compares the integer value
//    stacksafe bool equal byte:value
//        = $self equal int:(IntNumber new byte:value).
//        
//    /// Compares the integer value
//    stacksafe bool equal long:anOperand 
//        = LongNumber new int:$self; equal long:anOperand.
//
//    /// Compares the integer value
//    stacksafe bool equal real:anOperand 
//        = RealNumber new int:$self; equal real:anOperand.
//
//    /// Compares the integer value
//    stacksafe bool less short:value
//        = $self equal int:(IntNumber new short:value).
//
//    /// Compares the integer value
//    stacksafe bool less byte:value
//        = $self less int:(IntNumber new byte:value).
//        
//    /// Compares the integer value
//    stacksafe bool less long:anOperand 
//        = LongNumber new int:$self; less long:anOperand.
//
//    /// Compares the integer value
//    stacksafe bool less real:anOperand 
//        = RealNumber new int:$self; less real:anOperand.
//
//    /// Returns the sum of integers
//    stacksafe add short:value
//        = $self add int:(IntNumber new short:value).
//
//    /// Returns the sum of integers
//    stacksafe add byte:value
//        = $self add int:(IntNumber new byte:value).
//
//    /// Returns the sum of integers
//    stacksafe long add long:anOperand
//        = LongNumber new int:$self; add long:anOperand.
//
//    /// Returns the sum of integers
//    stacksafe real add real:anOperand
//        = RealNumber new int:$self; add real:anOperand.
//
//    /// Returns the difference of integers
//    stacksafe subtract short:value
//        = $self subtract int:(IntNumber new short:value).
//
//    /// Returns the difference of integers
//    stacksafe subtract byte:value
//        = $self subtract int:(IntNumber new byte:value).
//
//    /// Returns the difference of integers
//    stacksafe long subtract long:anOperand
//        = LongNumber new int:$self; subtract long:anOperand.
//
//    /// Returns the difference of integers
//    stacksafe real subtract real:anOperand
//        = RealNumber new int:$self; subtract real:anOperand.
//
//    /// Returns the product of integers
//    stacksafe multiply short:value
//        = $self multiply int:(IntNumber new short:value).        
//
//    /// Returns the product of integers
//    stacksafe multiply byte:value
//        = $self multiply int:(IntNumber new byte:value).        
//
//    /// Returns the product of integers
//    stacksafe long multiply long:anOperand
//        = LongNumber new int:$self; multiply long:anOperand.
//
//    /// Returns the product of integers
//    stacksafe real multiply real:anOperand
//        = RealNumber new int:$self; multiply real:anOperand.
//
//    /// Returns the result of integer division
//    stacksafe divide short:value
//        = $self divide int:(IntNumber new short:value).        
//
//    /// Returns the result of integer division
//    stacksafe divide byte:value
//        = $self divide int:(IntNumber new byte:value).        
//
//    /// Returns the result of integer division
//    stacksafe long divide long:anOperand
//        = LongNumber new int:$self; divide long:anOperand.
//
//    /// Returns the result of integer division
//    stacksafe real divide real:anOperand
//        = RealNumber new int:$self; divide real:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    stacksafe and short:anOperand
//        = $self and int:(IntNumber new short:anOperand).
//
//    /// Returns the result of bitwise AND operation
//    stacksafe and byte:anOperand
//        = $self and int:(IntNumber new byte:anOperand).
//
//    /// Returns the result of bitwise AND operation
//    stacksafe and long:anOperand
//        = LongNumber new int:$self; and long:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe or short:anOperand
//        = $self or int:(IntNumber new short:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    stacksafe or byte:anOperand
//        = $self or int:(IntNumber new byte:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    stacksafe or long:anOperand
//        = LongNumber new int:$self; or long:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor short:anOperand
//        = $self xor int:(IntNumber new short:anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor byte:anOperand
//        = $self xor int:(IntNumber new byte:anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor long:anOperand
//        = LongNumber new int:$self; xor long:anOperand.
//
//    /// Returns the literal presentation
//    literal = system'literalConvertor convert int:$self.
//
//    /// Returns the literal presentation
//    wide = system'wideConvertor convert int:$self.
//
//    /// Clones the value
//    clone = IntNumber new int:$self.
//
//    // Reads the object hash code into the output parameter
//    stacksafe readHashCodeTo vint:aCode 
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        $self readHashCodeTo vint:aCode.
//        
//        ^ aCode.
//    ]        
//
//    stacksafe int shiftLeft int:val
//    [
//        ^ theValue << val
//    ]
//
//    stacksafe int shiftRight int:val
//    [
//        ^ theValue >> val
//    ]
//    
//    /// Bitwise inversion
//    stacksafe int inverted
//        => system'core_routines'__int.  
//        
//    /// Negates the value
//    stacksafe negative
//        = 0 - theValue.
}

//// --- UIntNumber ---
//
///// A unsigned 32 bit integer
//
//sealed struct UIntNumber :: IntBaseNumber
//{
//    __int& theValue(4).
//
//    stacksafe explicit(ByteNumber n)
//    [
//        theValue := n.
//    ]
//    
//    stacksafe explicit(ShortNumber n)
//    [
//        theValue := n.
//    ]
//    
//    stacksafe explicit(IntNumber n)
//    [
//        theValue := n.
//    ]
//    
//    explicit u literal:s
//    [
//        theValue := intConvertor convertUnsigned literal:s.
//    ]
//    
//    /// Creates the object with the minimal value
//    constructor min
//        = 0.
//
//    /// Creates the object with the maximal value
//    constructor max
//        = 0FFFFFFFFh.
//        
//    constructor new int:n
//    [
//        theValue := n.
//    ]        
//        
//    constructor new byte:n
//    [
//        theValue := n.
//    ]        
//        
//    constructor new short:n
//    [
//        theValue := n.
//    ]        
//        
//    constructor new uint:n
//    [
//        theValue := n.
//    ]        
//        
//    constructor new
//    [
//        theValue := 0.
//    ]        
//
//    /// Returns the literal presentation
//    literal = system'literalConvertor convert uint:$self.
//
//    /// Returns the literal presentation
//    wide = system'wideConvertor convert uint:$self.
//        
//    /// Returns the value as a short integer    
//    stacksafe short = shortConvertor convert uint:$self.
//
//    /// Returns the byte value    
//    byte = byteConvertor convert uint:$self.
//
//    int = $self.
//
//    /// Returns the long integer
//    long = LongNumber new uint:$self.
//
//    /// Returns the real number
//    real = RealNumber new uint:$self.
//
//    multi bool equal:o
//    [
//        var n := o int \ back:$nil.
//        ($nil == n)
//            ? [ ^ false ]
//            ! [ ^ $self equal int:n ]
//    ]
//
//    multi bool less:n
//        <= less int:n.
//        
//    stacksafe bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    stacksafe bool less(BaseVariable v)
//        <= less(v value).
//        
//    /// Compares the value
//    stacksafe bool equal int:anOperand 
//        = theValue == anOperand.
//        
//    /// Compares the value
//    stacksafe bool less int:anOperand
//        = theValue < anOperand.
//        
//    multi add:n
//        <= add int:n.
//
//    multi subtract:n
//        <= subtract int:n.
//
//    multi multiply:n
//        <= multiply int:n.
//
//    multi divide:n
//        <= divide int:n.
//        
//    stacksafe int add(BaseVariable v)
//        <= add(v value).
//
//    stacksafe int subtract(BaseVariable v)
//        <= subtract(v value).
//
//    stacksafe int multiply(BaseVariable v)
//        <= multiply(v value).
//
//    stacksafe int divide(BaseVariable v)
//        <= divide(v value).
//        
//    /// Returns the sum
//    stacksafe int add int:anOperand
//        = theValue + anOperand.
//
//    /// Returns the sum
//    stacksafe int subtract int:anOperand
//        = theValue - anOperand.
//
//    /// Returns the product
//    stacksafe int multiply int:anOperand
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    stacksafe int divide int:anOperand
//        = theValue / anOperand.
//
//    multi and:n
//        <= and int:n.
//        
//    multi or:n
//        <= divide int:n.
//        
//    multi xor:n
//        <= xor int:n.
//        
//    stacksafe int and(BaseVariable v)
//        <= and(v value).
//        
//    stacksafe int or(BaseVariable v)
//        <= or(v value).
//        
//    stacksafe int xor(BaseVariable v)
//        <= xor(v value).
//        
//    /// Returns the result of bitwise AND operation
//    stacksafe int and int:anOperand
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe int or int:anOperand
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe int xor int:anOperand
//        = theValue ^^ anOperand.
//
//    /// Compares the value
//    stacksafe bool equal uint:anOperand 
//        = theValue == anOperand.
//        
//    /// Compares the value
//    stacksafe bool less uint:anOperand
//        = theValue < anOperand.
//        
//    /// Returns the sum
//    stacksafe uint add uint:anOperand
//        = theValue + anOperand.
//
//    /// Returns the sum
//    stacksafe uint subtract uint:anOperand
//        = theValue - anOperand.
//
//    /// Returns the product
//    stacksafe uint multiply uint:anOperand
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    stacksafe uint divide uint:anOperand
//        = theValue / anOperand.
//
//    /// Returns the result of bitwise AND operation
//    stacksafe uint and uint:anOperand
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe uint or uint:anOperand
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe uint xor uint:anOperand
//        = theValue ^^ anOperand.
//
//    /// Compares the integer value
//    stacksafe bool equal short:value 
//        = $self equal uint:value.
//
//    /// Compares the integer value
//    stacksafe bool equal byte:value
//        = $self equal uint:value.
//        
//    /// Compares the integer value
//    stacksafe bool equal long:anOperand 
//        = LongNumber new uint:$self; equal long:anOperand.
//
//    /// Compares the integer value
//    stacksafe bool equal real:anOperand 
//        = RealNumber new uint:$self; equal real:anOperand.
//
//    /// Compares the integer value
//    stacksafe bool less short:value
//        = $self equal uint:value.
//
//    /// Compares the integer value
//    stacksafe bool less byte:value
//        = $self less uint:value.
//        
//    /// Compares the integer value
//    stacksafe bool less long:anOperand 
//        = LongNumber new uint:$self; less long:anOperand.
//
//    /// Compares the integer value
//    stacksafe bool less real:anOperand 
//        = RealNumber new uint:$self; less real:anOperand.
//
//    /// Returns the sum of integers
//    stacksafe add short:value
//        = $self add uint:value.
//
//    /// Returns the sum of integers
//    stacksafe add byte:value
//        = $self add uint:value.
//
//    /// Returns the sum of integers
//    stacksafe add long:anOperand
//        = LongNumber new uint:$self; add long:anOperand.
//
//    /// Returns the sum of integers
//    stacksafe add real:anOperand
//        = RealNumber new uint:$self; add real:anOperand.
//
//    /// Returns the difference of integers
//    stacksafe subtract short:value
//        = $self subtract uint:value.
//
//    /// Returns the difference of integers
//    stacksafe subtract byte:value
//        = $self subtract uint:value.
//
//    /// Returns the difference of integers
//    stacksafe subtract long:anOperand
//        = LongNumber new uint:$self; subtract long:anOperand.
//
//    /// Returns the difference of integers
//    stacksafe subtract real:anOperand
//        = RealNumber new uint:$self; subtract real:anOperand.
//
//    /// Returns the product of integers
//    stacksafe multiply short:value
//        = $self multiply uint:value.
//
//    /// Returns the product of integers
//    stacksafe multiply byte:value
//        = $self multiply uint:value.
//
//    /// Returns the product of integers
//    stacksafe multiply long:anOperand
//        = LongNumber new uint:$self; multiply long:anOperand.
//
//    /// Returns the product of integers
//    stacksafe multiply real:anOperand
//        = RealNumber new uint:$self; multiply real:anOperand.
//
//    /// Returns the result of integer division
//    stacksafe divide short:value
//        = $self divide uint:value.
//
//    /// Returns the result of integer division
//    stacksafe divide byte:value
//        = $self divide uint:value.
//
//    /// Returns the result of integer division
//    stacksafe divide long:anOperand
//        = LongNumber new uint:$self; divide long:anOperand.
//
//    /// Returns the result of integer division
//    stacksafe divide real:anOperand
//        = RealNumber new uint:$self; divide real:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    stacksafe and short:anOperand
//        = $self and uint:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    stacksafe and byte:anOperand
//        = $self and uint:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    stacksafe and long:anOperand
//        = LongNumber new uint:$self; and long:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe or short:anOperand
//        = $self or uint:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe or byte:anOperand
//        = $self or uint:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe or long:anOperand
//        = LongNumber new uint:$self; or long:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor short:anOperand
//        = $self xor uint:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor byte:anOperand
//        = $self xor uint:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor long:anOperand
//        = LongNumber new uint:$self; xor long:anOperand.
//
//    /// Clones the value
//    clone = UIntNumber new uint:$self.
//
//    // Reads the object hash code into the output parameter
//    stacksafe readHashCodeTo vint:aCode 
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        $self readHashCodeTo vint:aCode.
//        
//        ^ aCode.
//    ]        
//
//    stacksafe uint shiftLeft int:val
//    [
//        ^ theValue << val
//    ]
//
//    stacksafe uint shiftRight int:val
//    [
//        ^ theValue >> val
//    ]
//    
//    /// Bitwise inversion
//    stacksafe uint inverted
//        => system'core_routines'__int.  
//}
//
//// --- LongNumber ---
//
///// A signed 64 bit integer value
//sealed struct LongNumber :: IntBaseNumber
//{
//    __int& theValue(8).
//    
//    /// Returns the minimal value
//    constructor min
//        = -9223372036854775808l.
//
//    /// Returns the maximal value
//    constructor max
//        = 9223372036854775807l.
//
//    /// Creates the object with a default value (0)
//    constructor new
//        <= new long:0l.
//
//    /// Creates the object with specified value
//    stacksafe constructor new long:value
//        = value.
//
//    /// Creates the object with specified value
//    stacksafe constructor new int:value
//        = longConvertor convert int:value.
//
//    /// Creates the object with specified value
//    stacksafe constructor new uint:value
//        = longConvertor convert uint:value.
//
//    /// Creates the object with specified value
//    stacksafe constructor new short:value 
//        = longConvertor convert short:value.
//
//    /// Creates the object with specified value
//    stacksafe constructor new byte:value 
//        = longConvertor convert byte:value.
//
//    /// Creates the object with specified value
//    constructor new : anObject
//        <= new long:anObject.
//
//    /// Returns an integer value
//    int = intConvertor convert long:$self.
//
//    /// Returns an integer value
//    uint = uintConvertor convert long:$self.
//
//    /// Returns the value as a short integer    
//    short = shortConvertor convert long:$self.
//    
//    /// Returns the byte value
//    byte = byteConvertor convert long:$self.
//
//    /// Returns a real value
//    real = RealNumber new long:$self.
//    
//    multi bool equal:o
//    [
//        var n := o long \ back:$nil.
//        ($nil == n)
//            ? [ ^ false ]
//            ! [ ^ $self equal long:n ]
//    ]
//
//    multi bool less:n
//        <= less long:n.
//        
//    stacksafe bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    stacksafe bool less(BaseVariable v)
//        <= less(v value).
//        
//    /// Compares the value
//    stacksafe bool equal long:anOperand 
//        = theValue == anOperand.
//        
//    /// Compares the value
//    stacksafe bool less long:anOperand
//        = theValue < anOperand.
//        
//    multi add:n
//        <= add long:n.
//
//    multi subtract:n
//        <= subtract long:n.
//
//    multi multiply:n
//        <= multiply long:n.
//
//    multi divide:n
//        <= divide long:n.
//        
//    stacksafe long add(BaseVariable v)
//        <= add(v value).
//
//    stacksafe long subtract(BaseVariable v)
//        <= subtract(v value).
//
//    stacksafe long multiply(BaseVariable v)
//        <= multiply(v value).
//
//    stacksafe long divide(BaseVariable v)
//        <= divide(v value).
//        
//    /// Returns the sum
//    stacksafe long add long:anOperand
//        = theValue + anOperand.
//
//    /// Returns the sum
//    stacksafe long subtract long:anOperand
//        = theValue - anOperand.
//
//    /// Returns the product
//    stacksafe long multiply long:anOperand
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    stacksafe long divide long:anOperand
//        = theValue / anOperand.
//
//    multi and:n
//        <= and long:n.
//        
//    multi or:n
//        <= divide long:n.
//        
//    multi xor:n
//        <= xor long:n.
//        
//    stacksafe long and(BaseVariable v)
//        <= and(v value).
//        
//    stacksafe long or(BaseVariable v)
//        <= divide(v value).
//        
//    stacksafe long xor(BaseVariable v)
//        <= xor(v value).
//        
//    /// Returns the result of bitwise AND operation
//    stacksafe long and long:anOperand
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    stacksafe long or long:anOperand
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe long xor long:anOperand
//        = theValue ^^ anOperand.    
//
//    /// Compares the integer value
//    stacksafe bool equal int:anOperand 
//        = $self equal long:(LongNumber new int:anOperand).
//
//    /// Compares the integer value
//    stacksafe bool equal short:anOperand 
//        = $self equal long:(LongNumber new short:anOperand).
//
//    /// Compares the integer value
//    stacksafe bool equal byte:anOperand 
//        = $self equal long:(LongNumber new byte:anOperand).
//
//    /// Compares the integer value
//    stacksafe bool equal real:anOperand 
//        = RealNumber new long:$self; equal real:anOperand.
//
//    /// Compares the integer value
//    stacksafe bool less int:anOperand 
//        = $self less long:(LongNumber new int:anOperand).
//
//    /// Compares the integer value
//    stacksafe bool less short:anOperand 
//        = $self less long:(LongNumber new short:anOperand).
//
//    /// Compares the integer value
//    stacksafe bool less byte:anOperand 
//        = $self less long:(LongNumber new byte:anOperand).
//
//    /// Compares the integer value
//    stacksafe bool less real:anOperand 
//        = RealNumber new long:$self; less real:anOperand.
//
//    /// Returns the sum of integers
//    stacksafe add int:anOperand
//        = $self add long:(LongNumber new int:anOperand).
//
//    /// Returns the sum of integers
//    stacksafe add short:anOperand
//        = $self add long:(LongNumber new short:anOperand).
//
//    /// Returns the sum of integers
//    stacksafe add byte:anOperand
//        = $self add long:(LongNumber new byte:anOperand).
//
//    /// Returns the sum of integers
//    stacksafe add real:anOperand
//        = RealNumber new long:$self; add real:anOperand.
//
//    /// Returns the difference of integers
//    stacksafe subtract int:anOperand
//        = $self subtract long:(LongNumber new int:anOperand).
//
//    /// Returns the difference of integers
//    stacksafe subtract short:anOperand
//        = $self subtract long:(LongNumber new short:anOperand).
//
//    /// Returns the difference of integers
//    stacksafe subtract byte:anOperand
//        = $self subtract long:(LongNumber new byte:anOperand).
//
//    /// Returns the difference of integers
//    stacksafe subtract real:anOperand
//        = RealNumber new long:$self; subtract real:anOperand.
//
//    /// Returns the product of integers
//    stacksafe multiply real:anOperand
//        = RealNumber new long:$self; multiply real:anOperand.
//
//    /// Returns the product of integers
//    stacksafe multiply int:anOperand
//        = $self multiply long:(LongNumber new int:anOperand).
//
//    /// Returns the product of integers
//    stacksafe multiply byte:anOperand
//        = $self multiply long:(LongNumber new byte:anOperand).
//
//    /// Returns the product of integers
//    stacksafe multiply short:anOperand
//        = $self multiply long:(LongNumber new short:anOperand).
//
//    /// Returns the result of integer division
//    stacksafe divide int:anOperand
//        = $self divide long:(LongNumber new int:anOperand).
//
//    /// Returns the result of integer division
//    stacksafe divide short:anOperand
//        = $self divide long:(LongNumber new short:anOperand).
//
//    /// Returns the result of integer division
//    stacksafe divide byte:anOperand
//        = $self divide long:(LongNumber new byte:anOperand).
//
//    /// Returns the result of integer division
//    stacksafe divide real:anOperand
//        = RealNumber new long:$self; divide real:anOperand.
//        
//    /// Returns the result of bitwise AND operation
//    stacksafe and int:anOperand
//        = $self and long:(LongNumber new int:anOperand).
//
//    /// Returns the result of bitwise AND operation
//    stacksafe and short:anOperand
//        = $self and long:(LongNumber new short:anOperand).
//
//    /// Returns the result of bitwise AND operation
//    stacksafe and byte:anOperand
//        = $self and long:(LongNumber new byte:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    stacksafe or int:anOperand
//        = $self or long:(LongNumber new int:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    stacksafe or short:anOperand
//        = $self or long:(LongNumber new short:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    stacksafe or byte:anOperand
//        = $self or long:(LongNumber new byte:anOperand).        
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor int:anOperand
//        = $self xor long:(LongNumber new int:anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor short:anOperand
//        = $self xor long:(LongNumber new short:anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    stacksafe xor byte:anOperand
//        = $self xor long:(LongNumber new byte:anOperand).
//
//    /// Returns the literal presentation
//    literal = system'literalConvertor convert long:$self.
//
//    /// Returns the literal presentation
//    wide = system'wideConvertor convert long:$self.
//
//    /// Clones the value
//    clone = LongNumber new long:$self.
//    
//    // Reads the object hash code into the output parameter
//    stacksafe readHashCodeTo vint:aCode
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        $self readHashCodeTo vint:aCode.
//        
//        ^ aCode.
//    ]
//
//    stacksafe long shiftLeft int:val
//    [
//        ^ theValue << val
//    ]
//
//    stacksafe long shiftRight int:val
//    [
//        ^ theValue >> val
//    ]
//    
//    /// Bitwise inversion
//    stacksafe long inverted
//        => system'core_routines'__long.  
//        
//    /// Negates the value
//    stacksafe negative
//        = 0l - theValue.
//}
//
//// --- realNumber ---
//
///// A 64bit float numeric value
//sealed struct RealNumber :: BaseNumber
//{
//    __real& theValue(8).
//
//    stacksafe explicit(IntNumber n)
//    [
//        realConvertor convert int:n vreal:$self.
//    ]
//            
//    /// Returns the minimal value
//    constructor min
//        = -1.7976931348623157e+308r.
//    
//    /// Returns the maximal value
//    constructor max
//        = 1.7976931348623157e+308r.
//        
//    /// Returns the negative infinity
//    embeddable constructor negativeInfinity 
//        = -1.0r / 0.0r. 
//        
//    /// Returns the positive infinity 
//    embeddable constructor positiveInfinity 
//        = 1.0r / 0.0r.
//        
//    /// returns the NaN
//    embeddable constructor NaN
//        = 0.0r / 0.0r.
//
//    /// Creates the object with a default value (0)
//    constructor new
//        <= new real:0.0r.
//
//    /// Creates the object with specified value
//    stacksafe constructor new real:value
//        = value.
//  
//    /// Creates the object with specified value
//    stacksafe constructor new int:value 
//        = realConvertor convert int:value.
//  
//    /// Creates the object with specified value
//    stacksafe constructor new uint:value 
//        = realConvertor convert uint:value.
//
//    /// Creates the object with specified value
//    stacksafe constructor new short:value 
//        = realConvertor convert short:value.
//
//    /// Creates the object with specified value
//    stacksafe constructor new byte:value 
//        = realConvertor convert byte:value.
//
//    /// Creates the object with specified value
//    stacksafe constructor new long:value
//        = realConvertor convert long:value.
//        
//    /// Creates the object with specified value
//    constructor new : anObject
//        <= new real:anObject.
//        
//    multi bool equal:o
//    [
//        var n := o real \ back:$nil.
//        ($nil == n)
//            ? [ ^ false ]
//            ! [ ^ $self equal real:n ]
//    ]
//
//    multi bool less:n
//        <= less real:n.
//        
//    stacksafe bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    stacksafe bool less(BaseVariable v)
//        <= less(v value).
//        
//    /// Compares the value
//    stacksafe bool equal real:anOperand 
//        = theValue == anOperand.
//        
//    /// Compares the value
//    stacksafe bool less real:anOperand
//        = theValue < anOperand.
//        
//    multi add :n
//        <= add real:n.
//
//    multi subtract:n
//        <= subtract real:n.
//
//    multi multiply:n
//        <= multiply real:n.
//
//    multi divide:n
//        <= divide real:n.
//        
//    stacksafe real add(BaseVariable v)
//        <= add(v value).
//
//    stacksafe real subtract(BaseVariable v)
//        <= subtract(v value).
//
//    stacksafe real multiply(BaseVariable v)
//        <= multiply(v value).
//
//    stacksafe real divide(BaseVariable v)
//        <= divide(v value).
//        
//    /// Returns the sum
//    stacksafe real add real:anOperand
//        = theValue + anOperand.
//
//    /// Returns the sum
//    stacksafe real subtract real:anOperand
//        = theValue - anOperand.
//
//    /// Returns the product
//    stacksafe real multiply real:anOperand
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    stacksafe real divide real:anOperand
//        = theValue / anOperand.
//        
//    /// Compares the numeric values
//    stacksafe bool equal int:anOperand
//        = $self equal real:(RealNumber new int:anOperand).
//
//    /// Compares the numeric values
//    stacksafe bool equal byte:anOperand
//        = $self equal real:(RealNumber new byte:anOperand).
//
//    /// Compares the numeric values
//    stacksafe bool equal short:anOperand
//        = $self equal real:(RealNumber new short:anOperand).
//
//    /// Compares the numeric values
//    stacksafe bool equal long:anOperand
//        = $self equal real:(RealNumber new long:anOperand).
//
//    /// Compares the numeric values
//    stacksafe bool less int:anOperand
//        = $self less real:(RealNumber new int:anOperand).
//
//    /// Compares the numeric values
//    stacksafe bool less byte:anOperand
//        = $self less real:(RealNumber new byte:anOperand).
//
//    /// Compares the numeric values
//    stacksafe bool less short:anOperand
//        = $self less real:(RealNumber new short:anOperand).
//
//    /// Compares the numeric values
//    stacksafe bool less long:anOperand
//        = $self less real:(RealNumber new long:anOperand).
//
//    /// Returns the sum of numbers
//    stacksafe add int:anOperand
//        = $self add real:(RealNumber new int:anOperand).
//
//    /// Returns the sum of numbers
//    stacksafe add byte:anOperand
//        = $self add real:(RealNumber new byte:anOperand).
//
//    /// Returns the sum of numbers
//    stacksafe add short:anOperand
//        = $self add real:(RealNumber new short:anOperand).
//
//    /// Returns the sum of numbers
//    stacksafe add long:anOperand
//        = $self add real:(RealNumber new long:anOperand).
//
//    /// Returns the difference of numbers
//    stacksafe subtract int:anOperand
//        = $self subtract real:(RealNumber new int:anOperand).
//
//    /// Returns the difference of numbers
//    stacksafe subtract byte:anOperand
//        = $self subtract real:(RealNumber new byte:anOperand).
//
//    /// Returns the difference of numbers
//    stacksafe subtract short:anOperand
//        = $self subtract real:(RealNumber new short:anOperand).
//
//    /// Returns the difference of numbers
//    stacksafe subtract long:anOperand
//        = $self subtract real:(RealNumber new long:anOperand).
//
//    /// Returns the product of numbers
//    stacksafe real multiply int:anOperand
//    [
//        real n := anOperand.
//        
//        ^ $self multiply real:n.
//    ]
//
//    /// Returns the product of numbers
//    stacksafe multiply byte:anOperand
//        = $self multiply real:(RealNumber new byte:anOperand).
//
//    /// Returns the product of numbers
//    stacksafe multiply short:anOperand
//        = $self multiply real:(RealNumber new short:anOperand).
//
//    /// Returns the product of numbers
//    stacksafe multiply long:anOperand
//        = $self multiply real:(RealNumber new long:anOperand).
//
//    /// Returns the result of the division
//    stacksafe divide int:anOperand
//    [
//        real n := anOperand.
//        
//        ^ $self divide real:n
//    ]
//
//    /// Returns the result of the division
//    stacksafe divide byte:anOperand
//        = $self divide real:(RealNumber new byte:anOperand).
//        
//    /// Returns the result of the division
//    stacksafe divide short:anOperand
//        = $self divide real:(RealNumber new short:anOperand).
//
//    /// Returns the result of the division
//    stacksafe divide long:anOperand
//        = $self divide real:(RealNumber new long:anOperand).
//
//    /// Returns the literal presentation
//    literal = system'literalConvertor convert real:$self.
//
//    /// Returns the literal presentation
//    wide = system'wideConvertor convert real:$self.
//
//    /// Clones the value    
//    clone = RealNumber new real:$self.
//    
//    // Reads the object hash code into the output parameter
//    stacksafe readHashCodeTo vint:aCode
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        $self readHashCodeTo vint:aCode.
//        
//        ^ aCode
//    ]
//        
//    /// Negates the value
//    stacksafe negative
//        = 0.0r - theValue.
//}
//
//// --- CharValue ---
//
///// An UTF-32 character symbol
//sealed const struct CharValue :: BaseValue
//{
//    __dword& theValue(4).
//
//    /// Creates the object with a default value (0)
//    constructor new
//        <= new char:$0.
//
//    /// Creates the object with specified value
//    stacksafe constructor new char:value
//        = value.
//
//    /// Creates the object with specified value
//    constructor new : anOperand
//        <= new char:(anOperand cast:%convert to:3).
//
//    /// Creates the object with specified numeric value
//    stacksafe constructor new int:anOperand
//        <= new char:(charConvertor convert int:anOperand).
//
//    /// Returns the minimal possible character value (0)
//    constructor min
//        = $0.
//
//    /// Returns the maximal possible character value (010FFFFh)
//    constructor max
//        = $010FFFFh.
//
//    multi bool equal:o
//    [
//        var n := o char \ back:$nil.
//        ($nil == n)
//            ? [ ^ false ]
//            ! [ ^ $self equal char:n ]
//    ]
//
//    multi bool less:n
//        <= less char:n.
//        
//    stacksafe bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    stacksafe bool less(BaseVariable v)
//        <= less(v value).
//        
//    stacksafe bool equal int:val
//        = theValue == val.
//
//    stacksafe bool equal literal:val
//        = val == $self literal.
//
//    stacksafe bool equal wide:val
//        = val == $self wide.
//
//    stacksafe bool less literal:val
//        = val < $self literal.
//
//    stacksafe bool less wide:val
//        = val < $self wide.
//
//    /// Clones the value
//    clone = CharValue new char:$self.
//
//    /// Returns the literal value
//    stacksafe literal = system'literalConvertor convert char:$self.
//
//    /// Returns the wide literal value
//    wide = system'wideConvertor convert char:$self.
//    
//    /// Reads the number of bytes to represent the symbol
//    stacksafe readLengthTo vint:aLength
//        => system'core_routines'__char.
//
//    /// Reads the number of bytes to represent the symbol
//    stacksafe readWideLengthTo vint:aRetVal
//    [
//        int aLength := 0.
//        $self readLengthTo vint:aLength.
//        
//        aLength := aLength + 1.
//        aLength := aLength / 2.
//
//        aRetVal int := aLength
//    ]
//        
//    /// Retuns the number of bytes to represent the symbol        
//    stacksafe embeddable int length
//    [
//        int aLength := 0.
//        $self readLengthTo vint:aLength.
//        
//        ^ aLength
//    ]
//    
//    /// Retuns the number of short integers to represent the symbol
//    stacksafe embeddable int wideLength
//    [
//        int aLength := 0.
//        $self readWideLengthTo vint:aLength.
//        
//        ^ aLength.
//    ]
//
//    /// Compares values
//    stacksafe bool equal char:anOperand 
//        = theValue == anOperand.
//
//    /// Compares values
//    stacksafe bool less char:anOperand 
//        = theValue < anOperand.
//        
//    // Reads the object hash code into the output parameter
//    stacksafe readHashCodeTo vint:aCode 
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        $self readHashCodeTo vint:aCode.
//        
//        ^ aCode.
//    ]
//}
//
//// --- nil constant ---
//
///// A nil value
//singleton nil
//{
//    /// Returns true if the specified object is nil.
//    bool equal : anObject 
//    [
//        ($nil == anObject)
//            ? [ ^ true. ].
//        
//        ($super equal:anObject)
//            ? [ ^ true. ].
//        
//        ^ false.
//    ]
//    
//    /// Returns nil
//    ///
//    /// Any typecasting message will be handled and nil value returned
//    generic = $nil.
//}
//
//// === Basic Factory symbols ==
//
//class BaseExtender
//{
//    object theObject.
//    
//    /// Returns true if the specified object is equal to the current object; otherwise, false.
//    bool equal : anObject
//        => theObject.
//      
//    dispatch => theObject.
//}
//
///// A generic variable. 
/////
///// Extends an assigned value
//sealed class Variable :: var<object>
//{
//    /// Creates the empty variable
//    constructor new
//    [
//    ]        
//        
//    /// Creates the variable
//    constructor new : value
//    [
//        $value := value.
//    ]
//    
////    set : value
////    [
////        $value := value.
////    ]
//    
//    append : val
//    [
//        ($nil == $value)
//            ? [ $value := val ]
//            ! [ $value := $value + val ]
//    ]
//    
//    reduce : val
//    [
//        ($nil == $value)
//            ? [ system'InvalidOperationException new; raise ]
//            ! [ $value := $value - val ]
//    ]
//    
//    multiplyBy : val
//    [
//        ($nil == $value)
//            ? [ system'InvalidOperationException new; raise ]
//            ! [ $value := $value * val ]
//    ]
//    
//    /// Divides into the value
//    divideInto : val
//    [
//        ($nil == $value)
//            ? [ system'InvalidOperationException new; raise ]
//            ! [ $value := $value / val ]
//    ]    
//}
//
//// --- KeyValue ---
//
///// A key - object pair
/////
///// Extends the object with a key property
//sealed class KeyValue
//{
//    object theKey.
//    object theObject.
//    
//    /// Creates a new objext
//    constructor new key:aKey value:anObject
//    [
//        theKey := aKey.
//        theObject := anObject.
//    ]
//    
//    /// Returns the key
//    key = theKey.
//    
//    /// Returns the key
//    value = theObject.
//
//    set value:anObject
//    [
//        theObject := anObject.
//    ]
//           
//    bool equal : o
//        => theObject.
//                                                                                      
//    /// Redirects the incoming messages to the object
//    dispatch => theObject.
//}
//
////// --- DTO ---
////
////class DTO
////{
////}
//
//// === Basic Convertors ===
//
//// --- convertor helpers ---
//
///// A byte convertor
//class byteConvertor =
//{
//    /// Converts a character into a byte value and saves the result into an output parameter
//    stacksafe convert char:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a short integer into a byte value and saves the result into an output parameter
//    stacksafe byte convert short:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts an integer into a byte value and saves the result into an output parameter
//    stacksafe byte convert int:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//
//    /// Converts an integer into a byte value and saves the result into an output parameter
//    stacksafe byte convert uint:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//
//    /// Converts a long integer into a byte value and saves the result into an output parameter
//    stacksafe convert long:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a real number into a byte value and saves the result into an output parameter
//    stacksafe convert real:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a literal value into a byte value and saves the result into an output parameter
//    stacksafe convert literal:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//
//    /// Converts a wide literal value into a byte value and saves the result into an output parameter
//    stacksafe convert wide:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    byte convert(BaseVariable v)
//        <= convert(v value).
//                        
//    /// Returns the parameter        
//    stacksafe byte convert byte:anInt
//        = ByteNumber new byte:anInt.
//        
//    /// Converts a character into a byte value
//    stacksafe byte convert char:aValue
//    [
//        byte aRetVal := 0.
//        $self convert char:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//
//    /// Converts a short integer into a byte value
//    stacksafe byte convert short:aValue
//    [
//        byte aRetVal := 0.
//        $self convert short:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//        
//    /// Converts an integer into a byte value
//    stacksafe byte convert int:aValue
//    [
//        byte aRetVal := 0.
//        $self convert int:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//        
//    /// Converts an integer into a byte value
//    stacksafe byte convert uint:aValue
//    [
//        byte aRetVal := 0.
//        $self convert uint:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//
//    /// Converts a long integer into a byte value
//    stacksafe byte convert long:aValue
//    [
//        byte aRetVal := 0.
//        $self convert long:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//
//    /// Converts a real number into a byte value
//    stacksafe byte convert real:aValue
//    [
//        byte aRetVal := 0.
//        $self convert real:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//
//    /// Converts a literal value into a byte value
//    stacksafe byte convert literal:aLiteral
//    [
//        byte aValue := 0.
//        $self convert literal:aLiteral vbyte:aValue.
//        
//        ^ ByteNumber new byte:aValue
//    ]
//                
//    /// Converts a wide literal value into a byte value
//    stacksafe byte convert wide:aLiteral
//    [
//        byte aValue := 0. 
//        $self convert wide:aLiteral vbyte:aValue.
//        
//        ^ ByteNumber new byte:aValue
//    ]
//
//    byte multi convert : anObject
//        = anObject byte.
//}.
//
///// A 16bit integer convertor
//class shortConvertor =
//{
//    /// Converts a character into a short integer value and saves the result into an output parameter
//    stacksafe convert char:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a byte value into a short integer value and saves the result into an output parameter
//    stacksafe convert byte:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts an integer into a short integer value and saves the result into an output parameter
//    stacksafe convert int:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts an integer into a short integer value and saves the result into an output parameter
//    stacksafe convert uint:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a long integer into a short integer value and saves the result into an output parameter
//    stacksafe convert long:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a real number into a short integer value and saves the result into an output parameter
//    stacksafe convert real:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//
//    /// Converts a literal string into a short integer value and saves the result into an output parameter
//    stacksafe convert literal:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a wide literal string into a short integer value and saves the result into an output parameter
//    stacksafe convert wide:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    short convert(BaseVariable v)
//        <= convert(v value).
//                        
//    /// Returns a short integer value        
//    stacksafe short convert short:anInt
//        = ShortNumber new short:anInt.
//        
//    /// Converts a byte value into a short integer value
//    stacksafe short convert byte:aValue
//    [
//        short aRetVal := 0.
//        $self convert byte:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//
//    /// Converts an integer into a short integer value
//    stacksafe short convert int:aValue
//    [
//        short aRetVal := 0.
//        $self convert int:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//
//    /// Converts an integer into a short integer value
//    stacksafe short convert uint:aValue
//    [
//        short aRetVal := 0.
//        $self convert uint:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//
//    /// Converts a long integer into a short integer value
//    stacksafe short convert long:aValue
//    [
//        short aRetVal := 0.
//        $self convert long:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//        
//    /// Converts a real number into a short integer value
//    stacksafe short convert real:aValue
//    [
//        short aRetVal := 0.
//        $self convert real:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//        
//    /// Converts a character into a short integer value
//    stacksafe short convert char:aValue
//    [
//        short aRetVal := 0.
//        $self convert char:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//        
//    /// Converts a literal string into a short integer value
//    stacksafe short convert literal:aValue
//    [
//        short aRetVal := 0.
//        $self convert literal:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//        
//    /// Converts a wide literal string into a short integer value
//    stacksafe short convert wide:aValue
//    [
//        short aRetVal := 0.
//        $self convert wide:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//        
//    short multi convert : anObject
//        = anObject short.
//}.
//
///// An Unicode character convertor
//class charConvertor =
//{
//    /// Converts a byte value into a character and saves the result into an output parameter
//    stacksafe convert byte:aValue vchar:aRetVal
//        => system'core_routines'__charconvertor.
//
//    /// Converts a short integer into a character and saves the result into an output parameter
//    stacksafe convert short:aValue vchar:aRetVal
//        => system'core_routines'__charconvertor.
//
//    /// Converts an integer into a character and saves the result into an output parameter
//    stacksafe convert int:aValue vchar:aRetVal
//        => system'core_routines'__charconvertor.
//        
//    char convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Returns a parameter        
//    stacksafe char convert char:aChar
//        = CharValue new char:aChar.
//  
//    stacksafe $convert int:anIndex literal:aLiteral vchar:char
//        => system'core_routines'__charconvertor.
//        
//    stacksafe $convert int:anIndex bytearray:aLiteral vchar:char
//        => system'core_routines'__charconvertor.
//  
//    stacksafe $convert int:anIndex wide:aLiteral vchar:char
//        => system'core_routines'__charconvertor.
//
//    stacksafe $convert int:anIndex shortarray:aLiteral vchar:char
//        => system'core_routines'__charconvertor.
//
//    stacksafe $save char:aChar int:anIndex shortarray:aCharArray vint:aLength
//        => system'core_routines'__charconvertor.
//    
//    stacksafe $save char:aChar int:anIndex bytearray:aCharArray vint:aLength
//        => system'core_routines'__charconvertor.
//
//    /// Converts a literal string into a character                
//    stacksafe char convert literal:aValue
//    [
//        char aRetVal := $0.
//        $self $convert int:0 literal:aValue vchar:aRetVal.
//        
//        int l1 := 0.
//        int l2 := aValue length.
//        aRetVal readLengthTo vint:l1.
//        
//        (l1 != l2)
//            ? [ system'InvalidArgumentException new; raise ].
//        
//        ^ CharValue new char:aRetVal
//    ]        
//        
//    /// Converts a wide literal string into a character                
//    stacksafe char convert wide:aValue
//    [
//        char aRetVal := $0.
//        $self $convert int:0 wide:aValue vchar:aRetVal.
//
//        int l1 := 0.
//        int l2 := aValue length.
//        aRetVal readWideLengthTo vint:l1.
//                
//        (l1 != l2)
//            ? [ system'InvalidArgumentException new; raise ].
//        
//        ^ CharValue new char:aRetVal
//    ]        
//        
//    /// Converts a byte value into a character
//    stacksafe char convert byte:aValue
//    [
//        char aRetVal := $0.
//        $self convert byte:aValue vchar:aRetVal.
//        
//        ^ CharValue new char:aRetVal
//    ]        
//        
//    /// Converts a short integer into a character
//    stacksafe char convert short:aValue
//    [
//        char aRetVal := $0.
//        $self convert short:aValue vchar:aRetVal.
//        
//        ^ CharValue new char:aRetVal
//    ]        
//        
//    /// Converts an integer into a character
//    stacksafe char convert int:aValue
//    [
//        char aRetVal := $0.
//        $self convert int:aValue vchar:aRetVal.
//        
//        ^ CharValue new char:aRetVal
//    ]
//        
//    char multi convert : anObject
//        = anObject char.
//}.
//
///// An integer convertor
//class intConvertor =
//{
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    stacksafe convert literal:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    stacksafe convertUnsigned literal:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    stacksafe convert literal:value int:base vint:retVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a wide literal string into an integer and saves the result into an output parameter
//    stacksafe convert wide:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a wide literal string into an integer and saves the result into an output parameter
//    stacksafe convert wide:value int:base vint:retVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a character value into an integer and saves the result into an output parameter
//    stacksafe convert char:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a short integer value into an integer and saves the result into an output parameter
//    stacksafe convert short:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a byte value into an integer and saves the result into an output parameter
//    stacksafe convert byte:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a long integer into an integer and saves the result into an output parameter
//    stacksafe convert long:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//        
//    /// Converts a real number into an integer and saves the result into an output parameter
//    stacksafe convert real:aReal vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    int convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Converts a literal string into an integer
//    stacksafe int convert literal:aLiteral
//    [
//        int aRetVal := 0.
//        $self convert literal:aLiteral vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//        
//    /// Converts a literal string into an integer
//    stacksafe int convertUnsigned literal:aLiteral
//    [
//        int aRetVal := 0.
//        $self convertUnsigned literal:aLiteral vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//        
//    /// Converts a literal string into an integer
//    stacksafe int convert literal:literal int:base
//    [
//        int retVal := 0.
//        $self convert literal:literal int:base vint:retVal.
//        
//        ^ IntNumber new int:retVal
//    ]
//        
//    /// Converts a wide literal string into an integer
//    stacksafe int convert wide:aLiteral
//    [
//        int aRetVal := 0.
//        $self convert wide:aLiteral vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//
//    /// Converts a wide literal string into an integer
//    stacksafe int convert wide:literal int:base
//    [
//        int retVal := 0.
//        $self convert wide:literal int:base vint:retVal.
//        
//        ^ IntNumber new int:retVal
//    ]
//        
//    /// Converts a character value into an integer
//    stacksafe int convert char:aValue
//    [
//        int aRetVal := 0.
//        $self convert char:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//        
//    /// Converts a byte value into an integer
//    stacksafe int convert byte:aValue
//    [
//        int aRetVal := 0.
//        $self convert byte:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//        
//    /// Converts a short integer value into an integer
//    stacksafe int convert short:aValue
//    [
//        int aRetVal := 0.
//        $self convert short:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//        
//    /// Converts a long integer value into an integer and saves the result into an output parameter
//    stacksafe int convert long:aValue
//    [
//        int aRetVal := 0.
//        $self convert long:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//
//    /// Converts a long integer into an integer
//    stacksafe int convert real:aReal
//    [
//        int aRetVal := 0.
//        $self convert real:aReal vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//
//    /// Returns a parameter                                    
//    stacksafe int convert int:anInt
//        = IntNumber new int:anInt.
//
//    stacksafe int convert uint:anInt
//        = IntNumber new int:anInt.        
//                        
//    int multi convert : anObject
//        = anObject int.
//}.
//
///// A unsigned integer convertor
//class uintConvertor =
//{
//    uint convert int:val
//        = UIntNumber new int:val.
//        
//    /// Converts a long integer into an integer and saves the result into an output parameter
//    stacksafe convert long:aValue vint:aRetVal
//        => system'core_routines'__uintconvertor.
//        
//    /// Converts a long integer value into an integer and saves the result into an output parameter
//    stacksafe uint convert long:aValue
//    [
//        int aRetVal := 0.
//        $self convert long:aValue vint:aRetVal.
//        
//        ^ UIntNumber new int:aRetVal
//    ]        
//}.
//
///// A long integer convertor
//class longConvertor =
//{
//    /// Converts a literal string into a long integer and saves the result into an output parameter
//    stacksafe convert literal:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a literal string into a long integer and saves the result into an output parameter
//    stacksafe convert literal:value int:base vlong:retVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a wide literal string into a long integer and saves the result into an output parameter
//    stacksafe convert wide:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a wide literal string into a long integer and saves the result into an output parameter
//    stacksafe convert wide:value int:base vlong:retVal
//        => system'core_routines'__longconvertor.
//
//    stacksafe convert int:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//        
//    stacksafe convert uint:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//        
//    stacksafe convert short:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    stacksafe convert byte:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a real number into a long integer and saves the result into an output parameter
//    stacksafe convert real:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    long convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Converts an integer value into a long integer
//    stacksafe long convert int:aValue
//    [
//        long n := 0l.
//        $self convert int:aValue vlong:n.
//        
//        ^ LongNumber new long:n
//    ]
//
//    /// Converts an integer value into a long integer
//    stacksafe long convert uint:aValue
//    [
//        long n := 0l.
//        $self convert uint:aValue vlong:n.
//        
//        ^ LongNumber new long:n
//    ]
//
//    /// Converts an integer value into a long integer
//    stacksafe long convert byte:aValue
//    [
//        long n := 0l.
//        $self convert byte:aValue vlong:n.
//        
//        ^ LongNumber new long:n
//    ]
//
//    /// Converts an integer value into a long integer
//    stacksafe long convert short:aValue
//    [
//        long n := 0l.
//        $self convert short:aValue vlong:n.
//        
//        ^ LongNumber new long:n
//    ]
//
//    /// Converts a real number into a long integer
//    stacksafe long convert real:aValue
//    [
//        long aRetVal := 0l.
//        $self convert real:aValue vlong:aRetVal.
//        
//        ^ LongNumber new long:aRetVal
//    ]
//
//    /// Converts a character into a long integer
//    stacksafe long convert char:aValue
//    [
//        int aRetVal := 0.
//        intConvertor convert char:aValue vint:aRetVal.
//        
//        ^ LongNumber new int:aRetVal
//    ]
//
//    /// Converts a literal string into a long integer
//    stacksafe long convert literal:aLiteral
//    [
//        long aRetVal := 0l.
//        $self convert literal:aLiteral vlong:aRetVal.
//        
//        ^ LongNumber new long:aRetVal
//    ]
//
////    /// Converts a literal string into a long integer
////    stacksafe long convert literal:literal base:base
////    [
////        long retVal := 0l.
////        $self convert literal:literal int:base vlong:retVal.
////        
////        ^ LongNumber new long:retVal
////    ]
//
//    /// Converts a wide literal string into a long integer
//    stacksafe long convert wide:aLiteral
//    [
//        long aRetVal := 0l.
//        $self convert wide:aLiteral vlong:aRetVal.
//        
//        ^ LongNumber new long:aRetVal
//    ]
//
////    /// Converts a wide literal string into a long integer
////    stacksafe long convert wide:literal base:base
////    [
////        long retVal := 0l.
////        $self convert wide:literal int:base vlong:retVal.
////        
////        ^ LongNumber new long:retVal
////    ]
//    
//    /// Returns a parameter
//    stacksafe long convert long:aLong
//        = LongNumber new long:aLong.
//        
//    long multi convert : anObject
//        = anObject long.
//}.
//
///// A real number convertor
//class realConvertor =
//{
//    /// Converts an integer into a real number and saves the result into an output parameter
//    stacksafe convert int:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    stacksafe convert uint:aValue vreal:aRetVal
//    [
//        (aValue < 0)?
//        [
//            // If the number is bigger than 2^31
//            // than we are unlucky
//            int odd := aValue && 1.            
//            int halfed := aValue >> 1.
//            
//            real r := 0.
//            $self convert int:halfed vreal:r.
//            r := r * 2.0r.
//            r := r + odd.
//            
//            aRetVal real := r.
//        ]
//        ! [ $self convert int:aValue vreal:aRetVal. ]
//    ]    
//    
//    //    => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    stacksafe convert long:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    stacksafe convert byte:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    stacksafe convert short:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts a literal string into a real number and saves the result into an output parameter
//    stacksafe convert literal:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts a wide literal string into a real number and saves the result into an output parameter
//    stacksafe convert wide:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//        
//    real convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Converts a literal string into a real number
//    stacksafe real convert literal:aLiteral
//    [
//        real aRetVal := 0.0r.
//        $self convert literal:aLiteral vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//        
//    /// Converts a wide literal string into a real number
//    stacksafe real convert wide:aLiteral
//    [
//        real aRetVal := 0.0r.
//        $self convert wide:aLiteral vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//
//    /// Returns a parameter
//    stacksafe real convert real:aReal
//        = RealNumber new real:aReal.
//        
//    /// Converts an integer into a real number
//    stacksafe real convert int:anInt
//    [
//        real aRetVal := 0.0r.
//        $self convert int:anInt vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//        
//    /// Converts an integer into a real number
//    stacksafe real convert uint:anInt
//    [
//        real aRetVal := 0.0r.
//        $self convert uint:anInt vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//
//    /// Converts a long integer into a real number
//    stacksafe real convert long:aLong
//    [
//        real aRetVal := 0.0r.
//        $self convert long:aLong vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//
//    /// Converts a byte into a real number
//    stacksafe real convert byte:aValue
//    [
//        real aRetVal := 0.0r.
//        $self convert byte:aValue vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//
//    /// Converts a short integer into a real number
//    stacksafe real convert short:aValue
//    [
//        real aRetVal := 0.0r.
//        $self convert short:aValue vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//        
//    real multi convert : anObject
//        = anObject real.
//}.
//
//// --- realOpController ---
//
//class realOpController =
//{
//    stacksafe exp real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe abs real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe ln real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe round real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe truncate real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe sin real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe cos real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe arctan real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//                
//    stacksafe sqrt real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe pi vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe embeddable real pi
//    [
//        real aRetVal := 0.0r.
//        $self pi vreal:aRetVal.
//        
//        ^ aRetVal
//    ]        
//    
//    stacksafe embeddable real e
//    [
//        real aRetVal := 0.0r.
//        $self exp real:1.0r vreal:aRetVal.
//        
//        ^ aRetVal
//    ]        
//}.
//
//real pi_value = realOpController pi.
//
//real e_value = realOpController e.
//
//// --- obsolete variables ---
//
//class Integer =
//{
//    vint new
//        = 0.
//        
//    vint new int:n
//        = n.
//        
//    vint new:n
//        = n int.
//}.
//
//class LongInteger =
//{
//    vlong new
//        = 0l.
//        
//    vlong new long:n
//        = n.
//        
//    vlong new:n
//        = n long.
//}.
//
//class Real =
//{
//    vreal new
//        = 0.0r.
//        
//    vreal new : n
//        = n real.
//        
//    vreal new int:n
//        = n real.
//        
//    vreal new real:n
//        = n.
//}.