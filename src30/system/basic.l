
#template varof : template_subj
{
    #field(template_subj)$value.

    #method set : val
    [
        $value := val template_subj.
    ]
    
    #method(stacksafe) set &template_subj:val
    [
        $value := val.
    ]

    #method(w3) get = $value.
    
    #method(w3) cast : verb &to:target => $value.

    /// Dispatches the verb to the target object with generic subject
    #method(w3) cast : aVerb &to:aTarget &with:aParam => $value.
    
    #method(w3) equal : anObject
        => $value.

    #method(embeddable,w3) => $value.
}

#template nvarof : template_subj
{
    #field(template_subj)$value.
    
    #method set : val
    [
        $value := val template_subj.
    ]
    
    #method(stacksafe) set &template_subj:val
    [
        $value := val.
    ]
    
    #method append : val
    [
        $value := $value + val template_subj.
    ]
    
    #method reduce : val
    [
        $value := $value - val template_subj.
    ]

    #method multiplyBy : val
    [
        $value := $value * val template_subj.
    ]
    
    /// Divides into the value
    #method divideInto : val
    [
        $value := $value / val template_subj.
    ]
    
    #method(stacksafe) append &template_subj:val
    [
        $value := $value + val.
    ]
    
    #method(stacksafe) reduce &template_subj:val
    [
        $value := $value - val.
    ]

    #method(stacksafe) multiplyBy &template_subj:val
    [
        $value := $value * val.
    ]
    
    /// Divides into the value
    #method(stacksafe) divideInto &template_subj:val
    [
        $value := $value / val.
    ]

    #method(w3) get = $value.
    
    #method(w3) cast : verb &to:target => $value.

    /// Dispatches the verb to the target object with generic subject
    #method(w3) cast : aVerb &to:aTarget &with:aParam => $value.
    
    #method(w3) equal : anObject
        => $value.
        
    #method(embeddable,w3) => $value.
}

// === Boolean values ===

// --- true ---

/// A true value symbol
#symbol true = BaseBoolValue
{
    /// Executes the action
    #method(-4097) if : aTrueAction = aTrueAction eval.

    /// Executes then-action
    #method if : aTrueAction : aFalseAction = aTrueAction eval.

    /// Always returns nil.
    #method(-4098) ifnot : aTrueAction = $nil.

//    #method iif:aTrueValue:aFalseValue = aTrueValue.
    
    /// Executes logical OR operation
    ///
    /// Always return true
    #method or : anExpr = $self.

    /// Executes logical AND operation
    #method and : anExpr = anExpr bool.

    /// Executes logical XOR operation
    #method xor : aBool
    [
        aBool ? [ ^ false. ].
        
        ^ $self.
    ]

    /// Determines whether the specified object is equal to the current object boolean value.
    #method equal : anExpr = anExpr bool.

    /// Determines whether the specified object is equal to the current object boolean value.
    #method equal &bool:aBool = aBool.

    /// Always returns false.
    #method inverted = false.

    /// Returns literal representation.
    #method literal = "true".

    /// Returns literal representation.
    #method wide = "true"w.

    /// Returns itself.
    #method clone = $self.
}.

// --- false ---

/// A false value symbol
#symbol false = BaseBoolValue
{
    /// Always returns nil
    #method(-4098) if : anAction = $nil.
    
    /// Executes else-action
    #method if : aTrueAction : aFalseAction = aFalseAction eval.

    /// Executes the action
    #method(-4097) ifnot : aFalseAction = aFalseAction eval.

//    #method iif:aTrueValue:aFalseValue = aFalseValue.

    /// Executes logical OR operation
    #method or : anExpr = anExpr bool.

    /// Executes logical AND operation
    ///
    /// Always return false
    #method and : anExpr = $self.

    /// Executes logical XOR operation
    #method xor : anExpr 
    [
        anExpr ? [ ^ true. ].
        
        ^ $self.
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    #method equal : anExpr
    [
        anExpr bool ? [ ^ $self. ].
        
        ^ true.
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    #method equal &bool:anExpr
    [
        ^ anExpr not.
    ]

    /// Always returns true
    #method inverted = true.

    /// Returns literal representation
    #method literal = "false".
    
    /// Returns literal representation
    #method wide = "false"w.

    /// Returns itself
    #method clone = $self.
}.

// === Numeric Variables ===

#subject vbyte  = nvarof:byte.
#subject vshort = nvarof:short.
#subject vint   = nvarof:int.
#subject vlong  = nvarof:long.
#subject vreal  = nvarof:real.
#subject vchar  = varof:char.

// === basic data classes ===

// --- BaseValue ---

// A base value
#class BaseValue
{
    /// Compares the values
    #method equal : anObject = anObject cast:%equal &to:$self bool.

    /// Compares the values
    #method(bool) less : anObject = anObject cast:%less &to:$self bool.

    /// Compares the values
    #method(bool) greater : anObject = anObject less:$self bool.
}

// --- BaseNumber ---

/// A base numeric value
#class BaseNumber :: BaseValue
{
    /// Adds the value
    #method add : anObject = anObject cast:%add &to:$self.

    /// Subtracts the value
    #method subtract : anObject = anObject cast:%subtract &to:$self.

    /// Multiply by the value
    #method multiply : anObject = anObject cast:%multiply &to:$self.

    /// Divides into the value
    #method divide : anObject = anObject cast:%divide &to:$self.
    
//    #method negative
//        = 0 - self.
}

// --- IntBaseNumber ---

/// A base integer value
#class IntBaseNumber :: BaseNumber
{
//    /// Execute bitwise AND operation
//    #method and : anObject = anObject cast:%and &to:$self.
//    
//    /// Execute bitwise OR operation
//    #method or : anObject = anObject cast:%or &to:$self.
//    
//    /// Execute bitwise XOR operation
//    #method xor : anObject = anObject cast:%xor &to:$self.
//    
//    /// Returns true if all the mask bits are set
//    #method allMask : anOperand
//        = $self and:anOperand == anOperand.
//        
//    /// Returns true if any of the mask bits are set    
//    #method anyMask : anOperand
//        = 0 != $self and:anOperand.
//
//    #method shift &int:v [ ]
//    
//    /// Bitwise shift operation
//    #method shift &index:anOrder
//        <= shift &int:(anOrder int).
//
//    #method shift : anOrder
//        <= shift &int:(anOrder int).
}

// --- ByteNumber ---

/// An unsigned 8 bit integer
#class(embeddable,sealed,dispatchable:byte) ByteNumber :: IntBaseNumber
{
    #field(1,-11)theValue.

//    #constructor(stacksafe) &int:n
//    [
//        theValue := n.
//    ]
            
//    /// Returns the minimal value (0)
//    #constructor min
//        = 0.
//
//    /// Returns the maxial value (255)
//    #constructor max
//        = 255.
    
    /// Creates the object with a specified value
    #constructor(stacksafe,w3) new &byte:value
        = value.

//    /// Creates the object with a specified value
//    #constructor new : aValue
//        <= new &byte:(aValue byte).
//
//    /// Creates the object with a default value (0)
//    #constructor new
//        = 0.
//
//    /// Returns the value as a short integer
//    #method short = ShortNumber new &byte:$self.
//
//    /// Returns the value as an integer
//    #method int = IntNumber new &byte:$self.
//
//    /// Returns the value as a long integer
//    #method long = LongNumber new &byte:$self.
//
//    /// Returns the value as a real number
//    #method real = RealNumber new &byte:$self.

    #method(bool,stacksafe) equal &byte:anOperand
        = theValue == anOperand.

//    #method(bool,stacksafe) equal &short:anOperand
//        = ShortNumber new &byte:$self equal &short:anOperand.
//
//    #method(bool,stacksafe) less &byte:anOperand
//        = theValue < anOperand.
//
//    #method(bool,stacksafe) less &short:anOperand
//        = ShortNumber new &byte:$self less &short:anOperand.
//
//    /// Compares the value
//    #method(bool,stacksafe) equal &int:anOperand
//        = IntNumber new &byte:$self less &int:anOperand.
//        
//    /// Compares the value
//    #method(bool,stacksafe) less &int:anOperand
//        = IntNumber new &byte:$self less &int:anOperand.
//
//    /// Compares the value
//    #method(bool,stacksafe) equal &long:anOperand
//        = LongNumber new &byte:$self equal &long:anOperand.
//        
//    /// Compares the value
//    #method(bool,stacksafe) less &long:anOperand
//        = LongNumber new &byte:$self less &long:anOperand.
//
//    /// Compares the value
//    #method(bool,stacksafe) equal &real:anOperand
//        = RealNumber new &byte:$self equal &real:anOperand.
//
//    /// Compares the value
//    #method(bool,stacksafe) less &real:anOperand
//        = RealNumber new &byte:$self less &real:anOperand.

    /// Returns the sum
    #method(byte,stacksafe,w3) add &byte:anOperand
        = theValue + anOperand.

    /// Returns the sum
    #method(byte,stacksafe,w3) subtract &byte:anOperand
        = theValue - anOperand.

    /// Returns the product
    #method(byte,stacksafe,w3) multiply &byte:anOperand
        = theValue * anOperand.

    /// Returns the result of integer division
    #method(byte,stacksafe,w3) divide &byte:anOperand
        = theValue / anOperand.

//    /// Returns the sum
//    #method(short,stacksafe) add &short:anOperand
//        = ShortNumber new &byte:$self add &short:anOperand.
//
//    /// Returns the difference
//    #method(short,stacksafe) subtract &short:anOperand
//        = ShortNumber new &byte:$self subtract &short:anOperand.
//
//    /// Returns the product
//    #method(short,stacksafe) multiply &short:anOperand
//        = ShortNumber new &byte:$self multiply &short:anOperand.
//
//    /// Returns the result of integer division
//    #method(short,stacksafe) divide &short:anOperand
//        = ShortNumber new &byte:$self divide &short:anOperand.
//
//    /// Returns the sum
//    #method(int,stacksafe) add &int:anOperand
//        = IntNumber new &byte:$self add &int:anOperand.
//
//    /// Returns the difference
//    #method(int,stacksafe) subtract &int:anOperand
//        = IntNumber new &byte:$self subtract &int:anOperand.
//
//    /// Returns the product
//    #method(int,stacksafe) multiply &int:anOperand
//        = IntNumber new &byte:$self multiply &int:anOperand.
//
//    /// Returns the result of integer division
//    #method(int,stacksafe) divide &int:anOperand
//        = IntNumber new &byte:$self divide &int:anOperand.
//
//    /// Returns the sum
//    #method(long,stacksafe) add &long:anOperand
//        = LongNumber new &byte:$self add &long:anOperand.
//
//    /// Returns the difference
//    #method(long,stacksafe) subtract &long:anOperand
//        = LongNumber new &byte:$self subtract &long:anOperand.
//
//    /// Returns the product
//    #method(long,stacksafe) multiply &long:anOperand
//        = LongNumber new &byte:$self multiply &long:anOperand.
//
//    /// Returns the result of integer division
//    #method(long,stacksafe) divide &long:anOperand
//        = LongNumber new &byte:$self divide &long:anOperand.
//
//    /// Returns the sum
//    #method(real,stacksafe) add &real:anOperand
//        = RealNumber new &byte:$self add &real:anOperand.
//
//    /// Returns the difference
//    #method(real,stacksafe) subtract &real:anOperand
//        = RealNumber new &byte:$self subtract &real:anOperand.
//
//    /// Returns the product
//    #method(real,stacksafe) multiply &real:anOperand
//        = RealNumber new &byte:$self multiply &real:anOperand.
//
//    /// Returns the result of integer division
//    #method(real,stacksafe) divide &real:anOperand
//        = RealNumber new &byte:$self divide &real:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    #method(byte,stacksafe,w3) and &byte:anOperand
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(byte,stacksafe,w3) or &byte:anOperand
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(byte,stacksafe,w3) xor &byte:anOperand
//        = theValue ^^ anOperand.
//
//    /// Returns the result of bitwise AND operation
//    #method(short,stacksafe) and &short:anOperand
//        = ShortNumber new &byte:$self and &short:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(short,stacksafe) or &short:anOperand
//        = ShortNumber new &byte:$self or &short:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(short,stacksafe) xor &short:anOperand
//        = ShortNumber new &byte:$self xor &short:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    #method(int,stacksafe) and &int:anOperand
//        = IntNumber new &byte:$self and &int:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(int,stacksafe) or &int:anOperand
//        = IntNumber new &byte:$self or &int:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(int,stacksafe) xor &int:anOperand
//        = IntNumber new &byte:$self xor &int:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    #method(long,stacksafe) and &long:anOperand
//        = LongNumber new &byte:$self and &long:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(long,stacksafe) or &long:anOperand
//        = LongNumber new &byte:$self or &long:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(long,stacksafe) xor &long:anOperand
//        = LongNumber new &byte:$self xor &long:anOperand.
//
//    #method(byte,stacksafe,w3) shift &int:val
//    [
//        (val > 0)
//            ? [ ^ theValue >> val. ].
//            
//        #var(int)neg_val := 0 - val - 1.
//            
//        ^ theValue << neg_val.
//    ]
//    
//    /// Bitwise inversion
//    #method(byte,stacksafe) inverted
//        => system'core_routines'__byte.  
        
    /// Returns the literal presentation
    #method(w2) literal = system'literalConvertor convert &byte:$self.

    /// Returns the literal presentation
    #method(w2) wide = system'wideConvertor convert &byte:$self.

//    /// Clones the value
//    #method clone = ByteNumber new &byte:$self.
//
//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode 
//        => system'core_routines'__int.
//    
//    #method(embeddable,int,w3) hash_code
//    [
//        #var(int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]        
}

// --- ShortNumber ---

/// A signed 16 bit integer
#class(embeddable,sealed,dispatchable:short) ShortNumber :: IntBaseNumber
{
    #field(2,-11)theValue.    
    
//    #constructor(stacksafe)&int:n
//    [
//        theValue := n.
//    ]
//    
//    /// Returns the minimal value (-32768)
//    #constructor min
//        = -32768.
//
//    /// Returns the maxial value (32767)
//    #constructor max
//        = 32767.
//
//    /// Creates the object with specified value
//    #constructor(stacksafe) new &byte:value
//        = shortConvertor convert &byte:value.
        
    /// Creates the object with specified value
    #constructor(stacksafe,w3) new &short:value
        = value.

//    /// Creates the object with a default value (0)
//    #constructor new
//        = 0.
//
//    /// Creates the object with specified value
//    #constructor new : anObject
//        <= new &short:(anObject short).
//
//    /// Returns the byte value        
//    #method byte = byteConvertor convert &short:$self.
//    
//    /// Returns the integer value
//    #method int = IntNumber new &short:$self.
//    
//    /// Returns the long integer value
//    #method long = LongNumber new &short:$self.
//    
//    /// Returns the value as a real number
//    #method real = RealNumber new &short:$self.

    /// Compares the value
    #method(bool,stacksafe) equal &short:anOperand 
        = theValue == anOperand.
        
    /// Compares the value
    #method(bool,stacksafe) less &short:anOperand
        = theValue < anOperand.
        
//    /// Compares the value
//    #method(bool,stacksafe) equal &int:anOperand 
//        = IntNumber new &short:$self equal &int:anOperand.
//        
//    /// Compares the value
//    #method(bool,stacksafe) less &int:anOperand
//        = IntNumber new &short:$self less &int:anOperand.
//    
//    /// Compares the value
//    #method(bool,stacksafe) equal &byte:anOperand 
//        = $self equal &short:(ShortNumber new &byte:anOperand).
//        
//    /// Compares the value
//    #method(bool,stacksafe) less &byte:anOperand
//        = $self less &short:(ShortNumber new &byte:anOperand).
//    
//    /// Compares the value
//    #method(bool,stacksafe) equal &long:anOperand 
//        = LongNumber new &short:$self equal &long:anOperand.
//        
//    /// Compares the value
//    #method(bool,stacksafe) less &long:anOperand
//        = LongNumber new &short:$self less &long:anOperand.
//    
//    /// Compares the value
//    #method(bool,stacksafe) equal &real:anOperand 
//        = RealNumber new &short:$self equal &real:anOperand.
//        
//    /// Compares the value
//    #method(bool,stacksafe) less &real:anOperand
//        = RealNumber new &short:$self less &real:anOperand.

    /// Returns the sum
    #method(short,stacksafe,w3) add &short:anOperand
        = theValue + anOperand.

    /// Returns the sum
    #method(short,stacksafe,w3) subtract &short:anOperand
        = theValue - anOperand.

    /// Returns the product
    #method(short,stacksafe,w3) multiply &short:anOperand
        = theValue * anOperand.

    /// Returns the result of integer division
    #method(short,stacksafe,w3) divide &short:anOperand
        = theValue / anOperand.

//    /// Returns the sum
//    #method(stacksafe) add &byte:anOperand
//        = $self add &short:(ShortNumber new &byte:anOperand).
//
//    /// Returns the difference
//    #method(stacksafe) subtract &byte:anOperand
//        = $self subtract &short:(ShortNumber new &byte:anOperand).
//
//    /// Returns the product
//    #method(stacksafe) multiply &byte:anOperand
//        = $self multiply &short:(ShortNumber new &byte:anOperand).
//
//    /// Returns the result of integer division
//    #method(stacksafe) divide &byte:anOperand
//        = $self divide &short:(ShortNumber new &byte:anOperand).
//
//    /// Returns the sum
//    #method(stacksafe) add &int:anOperand
//        = IntNumber new &short:$self add &int:anOperand.
//
//    /// Returns the difference
//    #method(stacksafe) subtract &int:anOperand
//        = IntNumber new &short:$self subtract &int:anOperand.
//
//    /// Returns the product
//    #method(stacksafe) multiply &int:anOperand
//        = IntNumber new &short:$self multiply &int:anOperand.
//        
//    /// Returns the result of integer division
//    #method(stacksafe) divide &int:anOperand
//        = IntNumber new &short:$self divide &int:anOperand.
//
//    /// Returns the sum
//    #method(stacksafe) add &long:anOperand
//        = LongNumber new &short:$self add &long:anOperand.
//
//    /// Returns the difference
//    #method(stacksafe) subtract &long:anOperand
//        = LongNumber new &short:$self subtract &long:anOperand.
//
//    /// Returns the product
//    #method(stacksafe) multiply &long:anOperand
//        = LongNumber new &short:$self multiply &long:anOperand.
//        
//    /// Returns the result of integer division
//    #method(stacksafe) divide &long:anOperand
//        = LongNumber new &short:$self divide &long:anOperand.
//
//    /// Returns the sum
//    #method(stacksafe) add &real:anOperand
//        = RealNumber new &short:$self add &real:anOperand.
//
//    /// Returns the difference
//    #method(stacksafe) subtract &real:anOperand
//        = RealNumber new &short:$self subtract &real:anOperand.
//
//    /// Returns the product
//    #method(stacksafe) multiply &real:anOperand
//        = RealNumber new &short:$self multiply &real:anOperand.
//        
//    /// Returns the result of integer division
//    #method(stacksafe) divide &real:anOperand
//        = RealNumber new &short:$self divide &real:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    #method(short,stacksafe,w3) and &short:anOperand
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(short,stacksafe,w3) or &short:anOperand
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(short,stacksafe,w3) xor &short:anOperand
//        = theValue ^^ anOperand.
//        
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &byte:anOperand
//        = $self and &short:(ShortNumber new &byte:anOperand). 
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &byte:anOperand
//        = $self or &short:(ShortNumber new &byte:anOperand). 
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &byte:anOperand
//        = $self xor &short:(ShortNumber new &byte:anOperand). 
//        
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &int:anOperand
//        = IntNumber new &short:$self and &int:anOperand.        
//        
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &int:anOperand
//        = IntNumber new &short:$self or &int:anOperand.        
//        
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &int:anOperand
//        = IntNumber new &short:$self xor &int:anOperand.        
//        
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &long:anOperand
//        = LongNumber new &short:$self and &long:anOperand.        
//        
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &long:anOperand
//        = LongNumber new &short:$self or &long:anOperand.        
//        
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &long:anOperand
//        = LongNumber new &short:$self xor &long:anOperand.        
//
//    #method(short,stacksafe,w3) shift &int:val
//    [
//        (val > 0)
//            ? [ ^ theValue >> val. ].
//            
//        #var(int)neg_val := 0 - val - 1.
//            
//        ^ theValue << neg_val.
//    ]
//    
//    /// Bitwise inversion
//    #method(short,stacksafe) inverted
//        => system'core_routines'__short.  

    /// Returns the literal presentation
    #method(w2) literal = system'literalConvertor convert &short:$self.

    /// Returns the literal presentation
    #method(w2) wide = system'wideConvertor convert &short:$self.

//    /// Clones the value
//    #method clone = ShortNumber new &short:$self.    
//
//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode 
//        => system'core_routines'__int.
//    
//    #method(embeddable,int,w3) hash_code
//    [
//        #var(int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]        
}

// --- IntNumber ---

/// A signed 32 bit integer
#class(embeddable,sealed,dispatchable:int) IntNumber :: IntBaseNumber
{
    #field(4,-11)theValue.
    
//    #constructor(stacksafe)&byte:n
//    [
//        theValue := n.
//    ]
//    
//    #constructor(stacksafe)&short:n
//    [
//        theValue := n.
//    ]
//    
//    /// Creates the object with the minimal value
//    #constructor min
//        = -2147483648.
//
//    /// Creates the object with the maximal value
//    #constructor max
//        = 2147483647.
    
    /// Creates the object with specified value
    #constructor(stacksafe,w3) new &int:value
        = value.

//    /// Creates the object with specified value
//    #constructor(stacksafe) new &short:value
//        = intConvertor convert &short:value.
//
//    /// Creates the object with specified value
//    #constructor(stacksafe) new &byte:value
//        = intConvertor convert &byte:value.
//
//    /// Creates the object with a default value (0)
//    #constructor new
//        <= new &int:0.
//
//    /// Creates the object with specified value
//    #constructor new : anObject
//        <= new &int:(anObject int).
//
//    /// Returns the value as a short integer    
//    #method(stacksafe) short = shortConvertor convert &int:$self.
//
//    /// Returns the byte value    
//    #method byte = byteConvertor convert &int:$self.
//
//    /// Returns the long integer
//    #method long = LongNumber new &int:$self.
//
//    /// Returns the real number
//    #method real = RealNumber new &int:$self.
    
    /// Compares the value
    #method(bool,stacksafe) equal &int:anOperand 
        = theValue == anOperand.
        
    /// Compares the value
    #method(bool,stacksafe) less &int:anOperand
        = theValue < anOperand.
        
    /// Returns the sum
    #method(int,stacksafe,w3) add &int:anOperand
        = theValue + anOperand.

    /// Returns the sum
    #method(int,stacksafe,w3) subtract &int:anOperand
        = theValue - anOperand.

    /// Returns the product
    #method(int,stacksafe,w3) multiply &int:anOperand
        = theValue * anOperand.

    /// Returns the result of integer division
    #method(int,stacksafe,w3) divide &int:anOperand
        = theValue / anOperand.

//    /// Returns the result of bitwise AND operation
//    #method(int,stacksafe,w3) and &int:anOperand
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(int,stacksafe,w3) or &int:anOperand
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(int,stacksafe,w3) xor &int:anOperand
//        = theValue ^^ anOperand.    
//
//    /// Compares the integer value
//    #method(bool,stacksafe) equal &short:value 
//        = $self equal &int:(IntNumber new &short:value).
//
//    /// Compares the integer value
//    #method(bool,stacksafe) equal &byte:value
//        = $self equal &int:(IntNumber new &byte:value).
//        
//    /// Compares the integer value
//    #method(bool,stacksafe) equal &long:anOperand 
//        = LongNumber new &int:$self equal &long:anOperand.
//
//    /// Compares the integer value
//    #method(bool,stacksafe) equal &real:anOperand 
//        = RealNumber new &int:$self equal &real:anOperand.
//
//    /// Compares the integer value
//    #method(bool,stacksafe) less &short:value
//        = $self equal &int:(IntNumber new &short:value).
//
//    /// Compares the integer value
//    #method(bool,stacksafe) less &byte:value
//        = $self less &int:(IntNumber new &byte:value).
//        
//    /// Compares the integer value
//    #method(bool,stacksafe) less &long:anOperand 
//        = LongNumber new &int:$self less &long:anOperand.
//
//    /// Compares the integer value
//    #method(bool,stacksafe) less &real:anOperand 
//        = RealNumber new &int:$self less &real:anOperand.
//
//    /// Returns the sum of integers
//    #method(stacksafe) add &short:value
//        = $self add &int:(IntNumber new &short:value).
//
//    /// Returns the sum of integers
//    #method(stacksafe) add &byte:value
//        = $self add &int:(IntNumber new &byte:value).
//
//    /// Returns the sum of integers
//    #method(stacksafe) add &long:anOperand
//        = LongNumber new &int:$self add &long:anOperand.
//
//    /// Returns the sum of integers
//    #method(stacksafe) add &real:anOperand
//        = RealNumber new &int:$self add &real:anOperand.
//
//    /// Returns the difference of integers
//    #method(stacksafe) subtract &short:value
//        = $self subtract &int:(IntNumber new &short:value).
//
//    /// Returns the difference of integers
//    #method(stacksafe) subtract &byte:value
//        = $self subtract &int:(IntNumber new &byte:value).
//
//    /// Returns the difference of integers
//    #method(stacksafe) subtract &long:anOperand
//        = LongNumber new &int:$self subtract &long:anOperand.
//
//    /// Returns the difference of integers
//    #method(stacksafe) subtract &real:anOperand
//        = RealNumber new &int:$self subtract &real:anOperand.
//
//    /// Returns the product of integers
//    #method(stacksafe) multiply &short:value
//        = $self multiply &int:(IntNumber new &short:value).        
//
//    /// Returns the product of integers
//    #method(stacksafe) multiply &byte:value
//        = $self multiply &int:(IntNumber new &byte:value).        
//
//    /// Returns the product of integers
//    #method(stacksafe) multiply &long:anOperand
//        = LongNumber new &int:$self multiply &long:anOperand.
//
//    /// Returns the product of integers
//    #method(stacksafe) multiply &real:anOperand
//        = RealNumber new &int:$self multiply &real:anOperand.
//
//    /// Returns the result of integer division
//    #method(stacksafe) divide &short:value
//        = $self divide &int:(IntNumber new &short:value).        
//
//    /// Returns the result of integer division
//    #method(stacksafe) divide &byte:value
//        = $self divide &int:(IntNumber new &byte:value).        
//
//    /// Returns the result of integer division
//    #method(stacksafe) divide &long:anOperand
//        = LongNumber new &int:$self divide &long:anOperand.
//
//    /// Returns the result of integer division
//    #method(stacksafe) divide &real:anOperand
//        = RealNumber new &int:$self divide &real:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &short:anOperand
//        = $self and &int:(IntNumber new &short:anOperand).
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &byte:anOperand
//        = $self and &int:(IntNumber new &byte:anOperand).
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &long:anOperand
//        = LongNumber new &int:$self and &long:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &short:anOperand
//        = $self or &int:(IntNumber new &short:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &byte:anOperand
//        = $self or &int:(IntNumber new &byte:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &long:anOperand
//        = LongNumber new &int:$self or &long:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &short:anOperand
//        = $self xor &int:(IntNumber new &short:anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &byte:anOperand
//        = $self xor &int:(IntNumber new &byte:anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &long:anOperand
//        = LongNumber new &int:$self xor &long:anOperand.

    /// Returns the literal presentation
    #method(w2) literal = system'literalConvertor convert &int:$self.

    /// Returns the literal presentation
    #method(w2) wide = system'wideConvertor convert &int:$self.

//    /// Clones the value
//    #method clone = IntNumber new &int:$self.
//
//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode 
//        => system'core_routines'__int.
//    
//    #method(embeddable,int,w3) hash_code
//    [
//        #var(int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]        
//
//    #method(int,stacksafe,w3) shift &int:val
//    [
//        (val > 0)
//            ? [ ^ theValue >> val. ].
//            
//        #var(int)neg_val := 0 - val - 1.
//            
//        ^ theValue << neg_val.
//    ]
//    
//    /// Bitwise inversion
//    #method(int,stacksafe) inverted
//        => system'core_routines'__int.  
//        
//    /// Negates the value
//    #method(stacksafe/*,suppress:w3*/) negative
//        = 0 - theValue.
}

// --- LongNumber ---

/// A signed 64 bit integer value
#class(embeddable,sealed,dispatchable:long) LongNumber :: IntBaseNumber
{
    #field(8,-13)theValue.
    
//    /// Returns the minimal value
//    #constructor min
//        = -9223372036854775808l.
//
//    /// Returns the maximal value
//    #constructor max
//        = 9223372036854775807l.
//
//    /// Creates the object with a default value (0)
//    #constructor new
//        <= new &long:0l.
//
//    /// Creates the object with specified value
//    #constructor(stacksafe,w3) new &long:value
//        = value.
//
//    /// Creates the object with specified value
//    #constructor(stacksafe) new &int:value
//        = longConvertor convert &int:value.
//
//    /// Creates the object with specified value
//    #constructor(stacksafe) new &short:value 
//        = longConvertor convert &short:value.
//
//    /// Creates the object with specified value
//    #constructor(stacksafe) new &byte:value 
//        = longConvertor convert &byte:value.
//
//    /// Creates the object with specified value
//    #constructor new : anObject
//        <= new &long:(anObject long).
//
//    /// Returns an integer value
//    #method int = intConvertor convert &long:$self.
//
//    /// Returns the value as a short integer    
//    #method short = shortConvertor convert &long:$self.
//    
//    /// Returns the byte value
//    #method byte = byteConvertor convert &long:$self.
//
//    /// Returns a real value
//    #method real = RealNumber new &long:$self.
    
    /// Compares the value
    #method(bool,stacksafe) equal &long:anOperand 
        = theValue == anOperand.
        
    /// Compares the value
    #method(bool,stacksafe) less &long:anOperand
        = theValue < anOperand.
        
    /// Returns the sum
    #method(long,stacksafe,w3) add &long:anOperand
        = theValue + anOperand.

    /// Returns the sum
    #method(long,stacksafe,w3) subtract &long:anOperand
        = theValue - anOperand.

    /// Returns the product
    #method(long,stacksafe,w3) multiply &long:anOperand
        = theValue * anOperand.

    /// Returns the result of integer division
    #method(long,stacksafe,w3) divide &long:anOperand
        = theValue / anOperand.

//    /// Returns the result of bitwise AND operation
//    #method(long,stacksafe,w3) and &long:anOperand
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(long,stacksafe,w3) or &long:anOperand
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(long,stacksafe,w3) xor &long:anOperand
//        = theValue ^^ anOperand.    
//
//    /// Compares the integer value
//    #method(bool,stacksafe) equal &int:anOperand 
//        = $self equal &long:(LongNumber new &int:anOperand).
//
//    /// Compares the integer value
//    #method(bool,stacksafe) equal &short:anOperand 
//        = $self equal &long:(LongNumber new &short:anOperand).
//
//    /// Compares the integer value
//    #method(bool,stacksafe) equal &byte:anOperand 
//        = $self equal &long:(LongNumber new &byte:anOperand).
//
//    /// Compares the integer value
//    #method(stacksafe) equal &real:anOperand 
//        = RealNumber new &long:$self equal &real:anOperand.
//
//    /// Compares the integer value
//    #method(bool,stacksafe) less &int:anOperand 
//        = $self less &long:(LongNumber new &int:anOperand).
//
//    /// Compares the integer value
//    #method(bool,stacksafe) less &short:anOperand 
//        = $self less &long:(LongNumber new &short:anOperand).
//
//    /// Compares the integer value
//    #method(bool,stacksafe) less &byte:anOperand 
//        = $self less &long:(LongNumber new &byte:anOperand).
//
//    /// Compares the integer value
//    #method(stacksafe) less &real:anOperand 
//        = RealNumber new &long:$self less &real:anOperand.
//
//    /// Returns the sum of integers
//    #method(stacksafe) add &int:anOperand
//        = $self add &long:(LongNumber new &int:anOperand).
//
//    /// Returns the sum of integers
//    #method(stacksafe) add &short:anOperand
//        = $self add &long:(LongNumber new &short:anOperand).
//
//    /// Returns the sum of integers
//    #method(stacksafe) add &byte:anOperand
//        = $self add &long:(LongNumber new &byte:anOperand).
//
//    /// Returns the sum of integers
//    #method(stacksafe) add &real:anOperand
//        = RealNumber new &long:$self add &real:anOperand.
//
//    /// Returns the difference of integers
//    #method(stacksafe) subtract &int:anOperand
//        = $self subtract &long:(LongNumber new &int:anOperand).
//
//    /// Returns the difference of integers
//    #method(stacksafe) subtract &short:anOperand
//        = $self subtract &long:(LongNumber new &short:anOperand).
//
//    /// Returns the difference of integers
//    #method(stacksafe) subtract &byte:anOperand
//        = $self subtract &long:(LongNumber new &byte:anOperand).
//
//    /// Returns the difference of integers
//    #method(stacksafe) subtract &real:anOperand
//        = RealNumber new &long:$self subtract &real:anOperand.
//
//    /// Returns the product of integers
//    #method(stacksafe) multiply &real:anOperand
//        = RealNumber new &long:$self multiply &real:anOperand.
//
//    /// Returns the product of integers
//    #method(stacksafe) multiply &int:anOperand
//        = $self multiply &long:(LongNumber new &int:anOperand).
//
//    /// Returns the product of integers
//    #method(stacksafe) multiply &byte:anOperand
//        = $self multiply &long:(LongNumber new &byte:anOperand).
//
//    /// Returns the product of integers
//    #method(stacksafe) multiply &short:anOperand
//        = $self multiply &long:(LongNumber new &short:anOperand).
//
//    /// Returns the result of integer division
//    #method(stacksafe) divide &int:anOperand
//        = $self divide &long:(LongNumber new &int:anOperand).
//
//    /// Returns the result of integer division
//    #method(stacksafe) divide &short:anOperand
//        = $self divide &long:(LongNumber new &short:anOperand).
//
//    /// Returns the result of integer division
//    #method(stacksafe) divide &byte:anOperand
//        = $self divide &long:(LongNumber new &byte:anOperand).
//
//    /// Returns the result of integer division
//    #method(stacksafe) divide &real:anOperand
//        = RealNumber new &long:$self divide &real:anOperand.
//        
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &int:anOperand
//        = $self and &long:(LongNumber new &int:anOperand).
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &short:anOperand
//        = $self and &long:(LongNumber new &short:anOperand).
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &byte:anOperand
//        = $self and &long:(LongNumber new &byte:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &int:anOperand
//        = $self or &long:(LongNumber new &int:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &short:anOperand
//        = $self or &long:(LongNumber new &short:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &byte:anOperand
//        = $self or &long:(LongNumber new &byte:anOperand).        
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &int:anOperand
//        = $self xor &long:(LongNumber new &int:anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &short:anOperand
//        = $self xor &long:(LongNumber new &short:anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &byte:anOperand
//        = $self xor &long:(LongNumber new &byte:anOperand).

    /// Returns the literal presentation
    #method(w2) literal = system'literalConvertor convert &long:$self.

    /// Returns the literal presentation
    #method(w2) wide = system'wideConvertor convert &long:$self.

//    /// Clones the value
//    #method clone = LongNumber new &long:$self.
//    
//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode
//        => system'core_routines'__int.
//    
//    #method(int,embeddable,w3) hash_code
//    [
//        #var(int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]
//
//    #method(long,stacksafe,w3) shift &int:val
//    [
//        (val > 0)
//            ? [ ^ theValue >> val. ].
//            
//        #var(int)neg_val := 0 - val - 1.
//            
//        ^ theValue << neg_val.
//    ]
//    
//    /// Bitwise inversion
//    #method(long,stacksafe) inverted
//        => system'core_routines'__long.  
//        
//    /// Negates the value
//    #method(stacksafe/*,suppress:w3*/) negative
//        = 0l - theValue.
}

// --- realNumber ---

/// A 64bit float numeric value
#class(embeddable,sealed,dispatchable:real) RealNumber :: BaseNumber
{
    #field(8,-14)theValue.

//    #constructor(stacksafe)&int:n
//    [
//        realConvertor convert &int:n &vreal:$self.
//    ]
//            
//    /// Returns the minimal value
//    #constructor min
//        = -1.7976931348623157e+308r.
//    
//    /// Returns the maximal value
//    #constructor max
//        = 1.7976931348623157e+308r.
//
//    /// Creates the object with a default value (0)
//    #constructor new
//        <= new &real:0.0r.
//
//    /// Creates the object with specified value
//    #constructor(stacksafe,w3) new &real:value
//        = value.
//  
//    /// Creates the object with specified value
//    #constructor(stacksafe) new &int:value 
//        = realConvertor convert &int:value.
//
//    /// Creates the object with specified value
//    #constructor(stacksafe) new &short:value 
//        = realConvertor convert &short:value.
//
//    /// Creates the object with specified value
//    #constructor(stacksafe) new &byte:value 
//        = realConvertor convert &byte:value.
//
//    /// Creates the object with specified value
//    #constructor(stacksafe) new &long:value
//        = realConvertor convert &long:value.
//        
//    /// Creates the object with specified value
//    #constructor new : anObject
//        <= new &real:(anObject real).
        
    /// Compares the value
    #method(bool,stacksafe) equal &real:anOperand 
        = theValue == anOperand.
        
    /// Compares the value
    #method(bool,stacksafe) less &real:anOperand
        = theValue < anOperand.
        
    /// Returns the sum
    #method(real,stacksafe,w3) add &real:anOperand
        = theValue + anOperand.

    /// Returns the sum
    #method(real,stacksafe,w3) subtract &real:anOperand
        = theValue - anOperand.

    /// Returns the product
    #method(real,stacksafe,w3) multiply &real:anOperand
        = theValue * anOperand.

    /// Returns the result of integer division
    #method(real,stacksafe,w3) divide &real:anOperand
        = theValue / anOperand.
        
//    /// Compares the numeric values
//    #method(bool,stacksafe) equal &int:anOperand
//        = $self equal &real:(RealNumber new &int:anOperand).
//
//    /// Compares the numeric values
//    #method(bool,stacksafe) equal &byte:anOperand
//        = $self equal &real:(RealNumber new &byte:anOperand).
//
//    /// Compares the numeric values
//    #method(bool,stacksafe) equal &short:anOperand
//        = $self equal &real:(RealNumber new &short:anOperand).
//
//    /// Compares the numeric values
//    #method(bool,stacksafe) equal &long:anOperand
//        = $self equal &real:(RealNumber new &long:anOperand).
//
//    /// Compares the numeric values
//    #method(bool,stacksafe) less &int:anOperand
//        = $self less &real:(RealNumber new &int:anOperand).
//
//    /// Compares the numeric values
//    #method(bool,stacksafe) less &byte:anOperand
//        = $self less &real:(RealNumber new &byte:anOperand).
//
//    /// Compares the numeric values
//    #method(bool,stacksafe) less &short:anOperand
//        = $self less &real:(RealNumber new &short:anOperand).
//
//    /// Compares the numeric values
//    #method(bool,stacksafe) less &long:anOperand
//        = $self less &real:(RealNumber new &long:anOperand).
//
//    /// Returns the sum of numbers
//    #method(stacksafe) add &int:anOperand
//        = $self add &real:(RealNumber new &int:anOperand).
//
//    /// Returns the sum of numbers
//    #method(stacksafe) add &byte:anOperand
//        = $self add &real:(RealNumber new &byte:anOperand).
//
//    /// Returns the sum of numbers
//    #method(stacksafe) add &short:anOperand
//        = $self add &real:(RealNumber new &short:anOperand).
//
//    /// Returns the sum of numbers
//    #method(stacksafe) add &long:anOperand
//        = $self add &real:(RealNumber new &long:anOperand).
//
//    /// Returns the difference of numbers
//    #method(stacksafe) subtract &int:anOperand
//        = $self subtract &real:(RealNumber new &int:anOperand).
//
//    /// Returns the difference of numbers
//    #method(stacksafe) subtract &byte:anOperand
//        = $self subtract &real:(RealNumber new &byte:anOperand).
//
//    /// Returns the difference of numbers
//    #method(stacksafe) subtract &short:anOperand
//        = $self subtract &real:(RealNumber new &short:anOperand).
//
//    /// Returns the difference of numbers
//    #method(stacksafe) subtract &long:anOperand
//        = $self subtract &real:(RealNumber new &long:anOperand).
//
//    /// Returns the product of numbers
//    #method(stacksafe,real) multiply &int:anOperand
//    [
//        #var(real)n := anOperand.
//        
//        ^ $self multiply &real:n.
//    ]
//
//    /// Returns the product of numbers
//    #method(stacksafe) multiply &byte:anOperand
//        = $self multiply &real:(RealNumber new &byte:anOperand).
//
//    /// Returns the product of numbers
//    #method(stacksafe) multiply &short:anOperand
//        = $self multiply &real:(RealNumber new &short:anOperand).
//
//    /// Returns the product of numbers
//    #method(stacksafe) multiply &long:anOperand
//        = $self multiply &real:(RealNumber new &long:anOperand).
//
//    /// Returns the result of the division
//    #method(stacksafe) divide &int:anOperand
//    [
//        #var(real)n := anOperand.
//        
//        ^ $self divide &real:n.
//    ]
//
//    /// Returns the result of the division
//    #method(stacksafe) divide &byte:anOperand
//        = $self divide &real:(RealNumber new &byte:anOperand).
//        
//    /// Returns the result of the division
//    #method(stacksafe) divide &short:anOperand
//        = $self divide &real:(RealNumber new &short:anOperand).
//
//    /// Returns the result of the division
//    #method(stacksafe) divide &long:anOperand
//        = $self divide &real:(RealNumber new &long:anOperand).

    /// Returns the literal presentation
    #method(w2) literal = system'literalConvertor convert &real:$self.
    
    /// Returns the literal presentation
    #method(w2) wide = system'wideConvertor convert &real:$self.

//    /// Clones the value    
//    #method clone = RealNumber new &real:$self.
//    
//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode
//        => system'core_routines'__int.
//    
//    #method(int,embeddable,w3) hash_code
//    [
//        #var(int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]
//        
//    /// Negates the value
//    #method(stacksafe/*,suppress:w3*/) negative
//        = 0.0r - theValue.
}

// --- CharValue ---

/// An UTF-32 character symbol
#class(embeddable,sealed,dispatchable:char,const) CharValue :: BaseValue
{
    #field(int)theValue.

    /// Creates the object with a default value (0)
    #constructor new
        <= new &char:#0.

    /// Creates the object with specified value
    #constructor(stacksafe,w3) new &char:value
        = value.

//    /// Creates the object with specified value
//    #constructor new : anOperand
//        <= new &char:(anOperand cast:%convert &to:charConvertor char).
//
//    /// Creates the object with specified numeric value
//    #constructor(stacksafe) new &int:anOperand
//        <= new &char:(charConvertor convert &int:anOperand).

    /// Returns the minimal possible character value (0)
    #constructor min
        = #0.

    /// Returns the maximal possible character value (010FFFFh)
    #constructor max
        = #010FFFFh.

    #method(stacksafe,bool) equal &int:val
        = theValue == val.

//    /// Clones the value
//    #method clone = CharValue new &char:$self.

    /// Returns the literal value
    #method(w2) literal = system'literalConvertor convert &char:$self.

    /// Returns the wide literal value
    #method(w2) wide = system'wideConvertor convert &char:$self.
    
    /// Reads the number of bytes to represent the symbol
    #method(stacksafe) readLength &vint:aLength
        => system'core_routines'__char.
        
//    /// Reads the number of bytes to represent the symbol
//    #method(stacksafe) readWideLength &vint:aRetVal
//    [
//        #var(int)aLength.
//        $self readLength &vint:aLength.
//        
//        aLength := aLength + 1.
//        aLength := aLength / 2.
//
//        aRetVal := aLength.
//    ]
//        
//    /// Retuns the number of bytes to represent the symbol        
//    #method(stacksafe,int,embeddable,w3) length
//    [
//        #var(int)aLength.
//        $self readLength &vint:aLength.
//        
//        ^ aLength.
//    ]
//    
//    /// Retuns the number of short integers to represent the symbol
//    #method(stacksafe,int,embeddable,w3) wideLength
//    [
//        #var(int)aLength.
//        $self readWideLength &vint:aLength.
//        
//        ^ aLength.
//    ]
//
//    /// Compares values
//    #method(bool,stacksafe) equal &char:anOperand 
//        = theValue == anOperand.
//
//    /// Compares values
//    #method(bool,stacksafe) less &char:anOperand 
//        = theValue < anOperand.
//        
//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode 
//        => system'core_routines'__int.
//    
//    #method(embeddable,int,w3) hash_code
//    [
//        #var(int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]
}

//// === Basic Factory symbols ==
//
//#class BaseExtender
//{
//    #field theObject.
//    
//    /// Returns true if the specified object is equal to the current object; otherwise, false.
//    #method(bool) equal : anObject
//        => theObject.
//      
//    /// Dispatches the verb to the target object
//    #method cast : aVerb &to:aTarget => theObject.
//    
//    /// Dispatches the verb to the target object
//    #method cast : aVerb &to:aTarget &with:aParam => theObject.
//    
//    #method => theObject.
//}
//
///// A generic variable. 
/////
///// Extends an assigned value
//#class(sealed,nvarof:object) Variable
//{
//    /// Creates the empty variable
//    #constructor new
//    [
//    ]        
//        
//    /// Creates the variable
//    #constructor new : value
//    [
//        $value := value.
//    ]
//    
//    #method set : value
//    [
//        $value := value.
//    ]
//    
//    #method append : val
//    [
//        ($nil == $value)
//            ? [ $value := val. ]
//            ! [ $value := $value + val. ].
//    ]
//    
//    #method reduce : val
//    [
//        ($nil == $value)
//            ? [ #throw system'InvalidOperationException new. ]
//            ! [ $value := $value - val. ].
//    ]
//
//    #method multiplyBy : val
//    [
//        ($nil == $value)
//            ? [ #throw system'InvalidOperationException new. ]
//            ! [ $value := $value * val. ].
//    ]
//    
//    /// Divides into the value
//    #method divideInto : val
//    [
//        ($nil == $value)
//            ? [ #throw system'InvalidOperationException new. ]
//            ! [ $value := $value / val. ].
//    ]    
//}
//
//// --- KeyValue ---
//
///// A key - object pair
/////
///// Extends the object with a key property
//#class(sealed) KeyValue
//{
//    #field theKey.
//    #field theObject.
//    
//    /// Creates a new objext
//    #constructor new &key:aKey &object:anObject
//    [
//        theKey := aKey.
//        theObject := anObject.
//    ]
//    
//    /// Returns the key
//    #method key = theKey.
//    
//    /// Returns the key
//    #method object = theObject.
//
//    #method set &object:anObject
//    [
//        theObject := anObject.
//    ]
//    
//    /// Dispatches the verb to the target object
//    #method cast : aVerb &to:aTarget => theObject.
//    
//    /// Dispatches the verb to the target object
//    #method cast : aVerb &to:aTarget &with:aParam => theObject.
//            
//    /// Redirects the incoming messages to the object
//    #method => theObject.
//}
//
//// --- DTO ---
//
//#class(dispatchable:dto) DTO
//{
//}
//
//// === Basic Convertors ===
//
//// --- convertor helpers ---
//
///// A byte convertor
//#symbol byteConvertor =
//{
//    /// Converts a character into a byte value and saves the result into an output parameter
//    #method(stacksafe) convert &char:aValue &vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a short integer into a byte value and saves the result into an output parameter
//    #method(byte,stacksafe) convert &short:aValue &vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts an integer into a byte value and saves the result into an output parameter
//    #method(byte,stacksafe) convert &int:aValue &vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//
//    /// Converts a long integer into a byte value and saves the result into an output parameter
//    #method(stacksafe) convert &long:aValue &vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a real number into a byte value and saves the result into an output parameter
//    #method(stacksafe) convert &real:aValue &vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a literal value into a byte value and saves the result into an output parameter
//    #method(stacksafe) convert &literal:aValue &vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//
//    /// Converts a wide literal value into a byte value and saves the result into an output parameter
//    #method(stacksafe) convert &wide:aValue &vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Returns the parameter        
//    #method(byte,stacksafe) convert &byte:anInt
//        = ByteNumber new &byte:(anInt).
//        
//    /// Converts a character into a byte value
//    #method(byte,stacksafe) convert &char:aValue
//    [
//        #var(byte)aRetVal.
//        $self convert &char:aValue &vbyte:aRetVal.
//        
//        ^ ByteNumber new &byte:aRetVal.
//    ]
//        
//    /// Converts a short integer into a byte value
//    #method(byte,stacksafe) convert &short:aValue
//    [
//        #var(byte)aRetVal.
//        $self convert &short:aValue &vbyte:aRetVal.
//        
//        ^ ByteNumber new &byte:aRetVal.
//    ]
//        
//    /// Converts an integer into a byte value
//    #method(byte,stacksafe) convert &int:aValue
//    [
//        #var(byte)aRetVal.
//        $self convert &int:aValue &vbyte:aRetVal.
//        
//        ^ ByteNumber new &byte:aRetVal.
//    ]
//
//    /// Converts a long integer into a byte value
//    #method(byte,stacksafe) convert &long:aValue
//    [
//        #var(byte)aRetVal.
//        $self convert &long:aValue &vbyte:aRetVal.
//        
//        ^ ByteNumber new &byte:aRetVal.
//    ]
//
//    /// Converts a real number into a byte value
//    #method(byte,stacksafe) convert &real:aValue
//    [
//        #var(byte)aRetVal.
//        $self convert &real:aValue &vbyte:aRetVal.
//        
//        ^ ByteNumber new &byte:aRetVal.
//    ]
//                
//    /// Converts a literal value into a byte value
//    #method(byte,stacksafe) convert &literal:aLiteral
//    [
//        #var(byte)aValue.
//        $self convert &literal:aLiteral &vbyte:aValue.
//        
//        ^ ByteNumber new &byte:aValue.
//    ]
//                
//    /// Converts a wide literal value into a byte value
//    #method(byte,stacksafe) convert &wide:aLiteral
//    [
//        #var(byte)aValue. 
//        $self convert &wide:aLiteral &vbyte:aValue.
//        
//        ^ ByteNumber new &byte:aValue.
//    ]
//        
//    #method(byte) convert : anObject
//        = anObject cast:%convert &to:$self byte.
//}.
//
///// A 16bit integer convertor
//#symbol shortConvertor =
//{
//    /// Converts a character into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &char:aValue &vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a byte value into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &byte:aValue &vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts an integer into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &int:aValue &vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a long integer into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &long:aValue &vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a real number into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &real:aValue &vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//
//    /// Converts a literal string into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &literal:aValue &vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a wide literal string into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &wide:aValue &vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Returns a short integer value        
//    #method(short,stacksafe) convert &short:anInt
//        = ShortNumber new &short:anInt.
//        
//    /// Converts a byte value into a short integer value
//    #method(short,stacksafe) convert &byte:aValue
//    [
//        #var(short)aRetVal.
//        $self convert &byte:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
//
//    /// Converts an integer into a short integer value
//    #method(short,stacksafe) convert &int:aValue
//    [
//        #var(short)aRetVal.
//        $self convert &int:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
//
//    /// Converts a long integer into a short integer value
//    #method(short,stacksafe) convert &long:aValue
//    [
//        #var(short)aRetVal.
//        $self convert &long:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
//        
//    /// Converts a real number into a short integer value
//    #method(short,stacksafe) convert &real:aValue
//    [
//        #var(short)aRetVal.
//        $self convert &real:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
//        
//    /// Converts a character into a short integer value
//    #method(short,stacksafe) convert &char:aValue
//    [
//        #var(short)aRetVal.
//        $self convert &char:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
//        
//    /// Converts a literal string into a short integer value
//    #method(short,stacksafe) convert &literal:aValue
//    [
//        #var(short)aRetVal.
//        $self convert &literal:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
//        
//    /// Converts a wide literal string into a short integer value
//    #method(short,stacksafe) convert &wide:aValue
//    [
//        #var(short)aRetVal.
//        $self convert &wide:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
//        
//    #method(short) convert : anObject
//        = anObject cast:%convert &to:$self short.
//}.
//
///// An Unicode character convertor
//#symbol charConvertor =
//{
//    /// Converts a byte value into a character and saves the result into an output parameter
//    #method(stacksafe) convert &byte:aValue &vchar:aRetVal
//        => system'core_routines'__charconvertor.
//
//    /// Converts a short integer into a character and saves the result into an output parameter
//    #method(stacksafe) convert &short:aValue &vchar:aRetVal
//        => system'core_routines'__charconvertor.
//
//    /// Converts an integer into a character and saves the result into an output parameter
//    #method(stacksafe) convert &int:aValue &vchar:aRetVal
//        => system'core_routines'__charconvertor.
//        
//    /// Returns a parameter        
//    #method(char,stacksafe)convert &char:aChar
//        = CharValue new &char:aChar.
//  
//    #method(stacksafe) $convert &int:anIndex &literal:aLiteral &vchar:char
//        => system'core_routines'__charconvertor.
//  
//    #method(stacksafe) $convert &int:anIndex &bytearray:aLiteral &vchar:char
//        => system'core_routines'__charconvertor.
//  
//    #method(stacksafe) $convert &int:anIndex &wide:aLiteral &vchar:char
//        => system'core_routines'__charconvertor.
//
//    #method(stacksafe) $convert &int:anIndex &shortarray:aLiteral &vchar:char
//        => system'core_routines'__charconvertor.
//
//    #method(stacksafe) $save &char:aChar &int:anIndex &shortarray:aCharArray &vint:aLength
//        => system'core_routines'__charconvertor.
//    
//    #method(stacksafe) $save &char:aChar &int:anIndex &bytearray:aCharArray &vint:aLength
//        => system'core_routines'__charconvertor.
//
//    /// Converts a literal string into a character                
//    #method(char,stacksafe) convert &literal:aValue
//    [
//        #var(char)aRetVal.
//        $self $convert &int:0 &literal:aValue &vchar:aRetVal.
//        
//        #var(int)l1.
//        #var(int)l2 := aValue length int.
//        aRetVal readLength &vint:l1.
//        
//        (l1 != l2)
//            ? [ #throw system'InvalidArgumentException new. ].
//        
//        ^ CharValue new &char:aRetVal.
//    ]        
//        
//    /// Converts a wide literal string into a character                
//    #method(char,stacksafe) convert &wide:aValue
//    [
//        #var(char)aRetVal.
//        $self $convert &int:0 &wide:aValue &vchar:aRetVal.
//
//        #var(int)l1.
//        #var(int)l2 := aValue length int.
//        aRetVal readWideLength &vint:l1.
//                
//        (l1 != l2)
//            ? [ #throw system'InvalidArgumentException new. ].
//        
//        ^ CharValue new &char:aRetVal.
//    ]        
//        
//    /// Converts a byte value into a character
//    #method(char,stacksafe) convert &byte:aValue
//    [
//        #var(char)aRetVal.
//        $self convert &byte:aValue &vchar:aRetVal.
//        
//        ^ CharValue new &char:aRetVal.
//    ]        
//        
//    /// Converts a short integer into a character
//    #method(char,stacksafe) convert &short:aValue
//    [
//        #var(char)aRetVal.
//        $self convert &short:aValue &vchar:aRetVal.
//        
//        ^ CharValue new &char:aRetVal.
//    ]        
//        
//    /// Converts an integer into a character
//    #method(char,stacksafe) convert &int:aValue
//    [
//        #var(char)aRetVal.
//        $self convert &int:aValue &vchar:aRetVal.
//        
//        ^ CharValue new &char:aRetVal.
//    ]
//        
//    #method(char) convert : anObject
//        = anObject cast:%convert &to:$self char.
//}.
//
///// An integer convertor
//#symbol intConvertor =
//{
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &literal:aValue &vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &literal:value &int:base &vint:retVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a wide literal string into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &wide:aValue &vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a wide literal string into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &wide:value &int:base &vint:retVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a character value into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &char:aValue &vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a short integer value into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &short:aValue &vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a byte value into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &byte:aValue &vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a long integer into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &long:aValue &vint:aRetVal
//        => system'core_routines'__intconvertor.
//        
//    /// Converts a real number into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &real:aReal &vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a literal string into an integer
//    #method(int,stacksafe) convert &literal:aLiteral
//    [
//        #var(int)aRetVal.
//        $self convert &literal:aLiteral &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//        
//    /// Converts a literal string into an integer
//    #method(int,stacksafe) convert &literal:literal &base:base
//    [
//        #var(int)retVal.
//        $self convert &literal:literal &int:(base int) &vint:retVal.
//        
//        ^ IntNumber new &int:retVal.
//    ]
//        
//    /// Converts a wide literal string into an integer
//    #method(int,stacksafe) convert &wide:aLiteral
//    [
//        #var(int)aRetVal.
//        $self convert &wide:aLiteral &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//
//    /// Converts a wide literal string into an integer
//    #method(int,stacksafe) convert &wide:literal &base:base
//    [
//        #var(int)retVal.
//        $self convert &wide:literal &int:(base int) &vint:retVal.
//        
//        ^ IntNumber new &int:retVal.
//    ]
//        
//    /// Converts a character value into an integer
//    #method(int,stacksafe) convert &char:aValue
//    [
//        #var(int)aRetVal.
//        $self convert &char:aValue &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//        
//    /// Converts a byte value into an integer
//    #method(int,stacksafe) convert &byte:aValue
//    [
//        #var(int)aRetVal.
//        $self convert &byte:aValue &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//        
//    /// Converts a short integer value into an integer
//    #method(int,stacksafe) convert &short:aValue
//    [
//        #var(int)aRetVal.
//        $self convert &short:aValue &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//        
//    /// Converts a long integer value into an integer and saves the result into an output parameter
//    #method(int,stacksafe) convert &long:aValue
//    [
//        #var(int)aRetVal.
//        $self convert &long:aValue &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//
//    /// Converts a long integer into an integer
//    #method(int,stacksafe) convert &real:aReal
//    [
//        #var(int)aRetVal.
//        $self convert &real:aReal &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//
//    /// Returns a parameter                                    
//    #method(int,stacksafe) convert &int:anInt
//        = IntNumber new &int:anInt.
//        
//    #method(int) convert : anObject
//        = anObject cast:%convert &to:$self int.
//}.
//
///// A long integer convertor
//#symbol longConvertor =
//{
//    /// Converts a literal string into a long integer and saves the result into an output parameter
//    #method(stacksafe)convert &literal:aValue &vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a literal string into a long integer and saves the result into an output parameter
//    #method(stacksafe)convert &literal:value &int:base &vlong:retVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a wide literal string into a long integer and saves the result into an output parameter
//    #method(stacksafe)convert &wide:aValue &vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a wide literal string into a long integer and saves the result into an output parameter
//    #method(stacksafe)convert &wide:value &int:base &vlong:retVal
//        => system'core_routines'__longconvertor.
//
//    #method(stacksafe)convert &int:aValue &vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    #method(stacksafe)convert &short:aValue &vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    #method(stacksafe)convert &byte:aValue &vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a real number into a long integer and saves the result into an output parameter
//    #method(stacksafe)convert &real:aValue &vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts an integer value into a long integer
//    #method(long,stacksafe)convert &int:aValue
//    [
//        #var(long)n.
//        $self convert &int:aValue &vlong:n.
//        
//        ^ LongNumber new &long:n.
//    ]
//
//    /// Converts an integer value into a long integer
//    #method(long,stacksafe)convert &byte:aValue
//    [
//        #var(long)n.
//        $self convert &byte:aValue &vlong:n.
//        
//        ^ LongNumber new &long:n.
//    ]
//
//    /// Converts an integer value into a long integer
//    #method(long,stacksafe)convert &short:aValue
//    [
//        #var(long)n.
//        $self convert &short:aValue &vlong:n.
//        
//        ^ LongNumber new &long:n.
//    ]
//
//    /// Converts a real number into a long integer
//    #method(stacksafe)convert &real:aValue
//    [
//        #var(long)aRetVal.
//        $self convert &real:aValue &vlong:aRetVal.
//        
//        ^ LongNumber new &long:aRetVal.
//    ]
//
//    /// Converts a character into a long integer
//    #method(stacksafe)convert &char:aValue
//    [
//        #var(int)aRetVal.
//        intConvertor convert &char:aValue &vint:aRetVal.
//        
//        ^ LongNumber new &int:aRetVal.
//    ]
//
//    /// Converts a literal string into a long integer
//    #method(long,stacksafe) convert &literal:aLiteral
//    [
//        #var(long)aRetVal.
//        $self convert &literal:aLiteral &vlong:aRetVal.
//        
//        ^ LongNumber new &long:aRetVal.
//    ]
//
//    /// Converts a literal string into a long integer
//    #method(long,stacksafe) convert &literal:literal &base:base
//    [
//        #var(long)retVal.
//        $self convert &literal:literal &int:(base int) &vlong:retVal.
//        
//        ^ LongNumber new &long:retVal.
//    ]
//
//    /// Converts a wide literal string into a long integer
//    #method(long,stacksafe) convert &wide:aLiteral
//    [
//        #var(long)aRetVal.
//        $self convert &wide:aLiteral &vlong:aRetVal.
//        
//        ^ LongNumber new &long:aRetVal.
//    ]
//
//    /// Converts a wide literal string into a long integer
//    #method(long,stacksafe) convert &wide:literal &base:base
//    [
//        #var(long)retVal.
//        $self convert &wide:literal &int:(base int) &vlong:retVal.
//        
//        ^ LongNumber new &long:retVal.
//    ]
//    
//    /// Returns a parameter
//    #method(long,stacksafe) convert &long:aLong
//        = LongNumber new &long:aLong.
//        
//    #method(long) convert : anObject
//        = anObject cast:%convert &to:$self long.
//}.
//
///// A real number convertor
//#symbol realConvertor =
//{
//    /// Converts an integer into a real number and saves the result into an output parameter
//    #method(stacksafe)convert &int:aValue &vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    #method(stacksafe)convert &long:aValue &vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    #method(stacksafe)convert &byte:aValue &vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    #method(stacksafe)convert &short:aValue &vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts a literal string into a real number and saves the result into an output parameter
//    #method(stacksafe)convert &literal:aValue &vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts a wide literal string into a real number and saves the result into an output parameter
//    #method(stacksafe)convert &wide:aValue &vreal:aRetVal
//        => system'core_routines'__realconvertor.
//        
//    /// Converts a literal string into a real number
//    #method(real,stacksafe)convert &literal:aLiteral
//    [
//        #var(real)aRetVal.
//        $self convert &literal:aLiteral &vreal:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]
//        
//    /// Converts a wide literal string into a real number
//    #method(real,stacksafe)convert &wide:aLiteral
//    [
//        #var(real)aRetVal.
//        $self convert &wide:aLiteral &vreal:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]
//
//    /// Returns a parameter
//    #method(real,stacksafe)convert &real:aReal
//        = RealNumber new &real:aReal.
//        
//    /// Converts an integer into a real number
//    #method(real,stacksafe)convert &int:anInt
//    [
//        #var(real)aRetVal.
//        $self convert &int:anInt &vreal:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]
//
//    /// Converts a long integer into a real number
//    #method(real,stacksafe)convert &long:aLong
//    [
//        #var(real)aRetVal.
//        $self convert &long:aLong &vreal:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]
//
//    /// Converts a byte into a real number
//    #method(real,stacksafe)convert &byte:aValue
//    [
//        #var(real)aRetVal.
//        $self convert &byte:aValue &vreal:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]
//
//    /// Converts a short integer into a real number
//    #method(real,stacksafe)convert &short:aValue
//    [
//        #var(real)aRetVal.
//        $self convert &short:aValue &vreal:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]
//        
//    #method(real) convert : anObject
//        = anObject cast:%convert &to:$self real.
//}.
//
//
//// --- realOpController ---
//
//#symbol realOpController =
//{
//    #method(stacksafe) exp &real:aReal &vreal:aRetVal
//        => system'core_routines'__real.
//        
//    #method(stacksafe) abs &real:aReal &vreal:aRetVal
//        => system'core_routines'__real.
//        
//    #method(stacksafe) ln &real:aReal &vreal:aRetVal
//        => system'core_routines'__real.
//        
//    #method(stacksafe) round &real:aReal &vreal:aRetVal
//        => system'core_routines'__real.
//        
//    #method(stacksafe) truncate &real:aReal &vreal:aRetVal
//        => system'core_routines'__real.
//        
//    #method(stacksafe) sin &real:aReal &vreal:aRetVal
//        => system'core_routines'__real.
//        
//    #method(stacksafe) cos &real:aReal &vreal:aRetVal
//        => system'core_routines'__real.
//        
//    #method(stacksafe) arctan &real:aReal &vreal:aRetVal
//        => system'core_routines'__real.
//                
//    #method(stacksafe) sqrt &real:aReal &vreal:aRetVal
//        => system'core_routines'__real.
//        
//    #method(stacksafe) pi &vreal:aRetVal
//        => system'core_routines'__real.
//        
//    #method(real,embeddable,w3,stacksafe) pi
//    [
//        #var(real)aRetVal.
//        $self pi &vreal:aRetVal.
//        
//        ^ aRetVal.
//    ]        
//    
//    #method(real,w3,stacksafe) e
//    [
//        #var(real)aRetVal.
//        $self exp &real:1.0r &vreal:aRetVal.
//        
//        ^ aRetVal.
//    ]        
//}.
//
//#symbol(real)pi_value = realOpController pi.
//
//#symbol(real)e_value = realOpController e.
//
//// --- obsolete variables ---
//
//#symbol Integer =
//{
//    #method(vint,w3) new
//        = 0.
//        
//    #method(vint,w3) new &int:n
//        = n.
//        
//    #method(vint,w3) new:n
//        = n int.
//}.
//
//#symbol Real =
//{
//    #method(vreal,w3) new
//        = 0.0r.
//        
//    #method(vreal,w3) new : n
//        = n real.
//        
//    #method(vreal,w3) new &int:n
//        = n real.
//        
//    #method(vreal,w3) new &real:n
//        = n.
//}.