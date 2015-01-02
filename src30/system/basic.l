// === basic data classes ===

#define(const)false = system'false.

// --- literalConvertor ---

#symbol literalConvertor =
{
    #method(type:literal) convert &int:aNumber
        => system'internal'ws_copyint.

    #method(type:literal) convert &int:aNumber &index:aRadix
        => system'internal'ws_copyint_radix.

//    #method convert &long:aNumber &base:aRadix
//        = literalPrimitiveOp toLiteral &long:aNumber &index::aRadix.

    #method(type:literal) convert &long:aNumber
        => system'internal'ws_copylong.

    #method(type:literal) convert &real:aNumber
        => system'internal'ws_copyreal.
        
    #method(type:literal) convert &object:anObject
        = anObject literal.
        
    #method(type:literal) convert &literal:aLiteral
        = aLiteral.
}.

// --- BaseValue ---

#class BaseValue
{
    #method equal : anObject = anObject cast:%equal &to:$self bool.

    #method(type:bool) less : anObject = anObject cast:%less &to:$self bool.

    #method(type:bool) greater : anObject = anObject less:$self bool.
}

// --- BaseNumber ---

#class BaseNumber :: BaseValue
{
    #method add : anObject = anObject cast:%add &to:$self.

    #method subtract : anObject = anObject cast:%subtract &to:$self.

    #method multiply : anObject = anObject cast:%multiply &to:$self.

    #method divide : anObject = anObject cast:%divide &to:$self.
}

// --- IntBaseNumber ---

#class IntBaseNumber :: BaseNumber
{
    #method and : anObject = anObject cast:%and &to:$self.
    
    #method or : anObject = anObject cast:%or &to:$self.
    
    #method xor : anObject = anObject cast:%xor &to:$self.
    
    #method allMask : anOperand
        = $self and:anOperand == anOperand.
        
    #method anyMask : anOperand
        = 0 != $self and:anOperand.
        
    #method negative = 0 subtract:$self.
}

// --- IntBaseVariable ---

#class IntBaseVariable :: IntBaseNumber
{
    #method write : anOperand
    [
        anOperand cast:%write &to:$self.
    ]
    
    #method append : anOperand
    [
        anOperand cast:%append &to:$self.
    ]
    
    #method reduce : anOperand
    [
        anOperand cast:%reduce &to:$self.
    ]
    
    #method multiplyBy : anOperand
    [
        anOperand cast:%multiplyBy &to:$self.
    ]
    
    #method divideInto : anOperand
    [
        anOperand cast:%divideInto &to:$self.
    ]
}

// --- BaseNumber ---

#class BaseVariable :: BaseNumber
{
    #method write : anOperand
    [
        anOperand cast:%write &to:$self.
    ]
    
    #method append : anOperand
    [
        anOperand cast:%append &to:$self.
    ]
    
    #method reduce : anOperand
    [
        anOperand cast:%reduce &to:$self.
    ]
    
    #method multiplyBy : anOperand
    [
        anOperand cast:%multiplyBy &to:$self.
    ]
    
    #method divideInto : anOperand
    [
        anOperand cast:%divideInto &to:$self.
    ]
}

// --- ByteNumber ---

#class(sealed,integer:1) ByteNumber :: IntBaseNumber
{
    #method cast : aVerb &to:aTarget = aTarget::aVerb byte:$self.
}

// --- ShortNumber ---

#symbol(const)shortMaximalValue = 65535.

#class(sealed,integer:2) ShortNumber :: IntBaseNumber
{
    #constructor new &int:anOperand
        => system'internal'int_copyshort.
        
    #constructor new &short:anOperand
        => system'internal'int_copy.
        
    #method short = $self.
    
    #method int = IntNumber new &short:$self int.
    
    #method equal &int:anOperand 
        => system'internal'int_equal.
        
    #method less &int:anOperand
        => system'internal'int_less.
    
    #method equal &short:anOperand 
        => system'internal'int_equal.
        
    #method less &short:anOperand
        => system'internal'int_less.
        
    #method cast : aVerb &to:aTarget = aTarget::aVerb short:$self.
}

// --- IntNumber ---

#class(sealed,integer:4) IntNumber :: IntBaseNumber
{
    #constructor new &int:anOperand
        => system'internal'int_copy.

    #constructor new &short:anOperand
        => system'internal'int_copyshort.

    #constructor new
        => system'internal'int_init.

    #method int = $self.

    #method short = ShortNumber new &int:$self.

    #method(type:bool) equal &int:anOperand 
        => system'internal'int_equal.
        
    #method(type:bool) less &int:anOperand
        => system'internal'int_less.

    #method(type:int) add &int:anOperand
        => system'internal'int_add.

    #method(type:int) subtract &int:anOperand
        => system'internal'int_sub.

    #method(type:int) multiply &int:anOperand
        => system'internal'int_mul.

    #method(type:int) divide &int:anOperand
        => system'internal'int_div.

    #method(type:int) and &int:anOperand
        => system'internal'int_and.

    #method(type:int) or &int:anOperand
        => system'internal'int_or.

    #method(type:int) xor &int:anOperand
        => system'internal'int_xor.

    #method equal &long:anOperand 
        = LongNumber new &int:$self equal &long:anOperand.

    #method less &long:anOperand 
        = LongNumber new &int:$self less &long:anOperand.

    #method add &long:anOperand
        = LongNumber new &int:$self add &long:anOperand.

    #method subtract &long:anOperand
        = LongNumber new &int:$self subtract &long:anOperand.

    #method multiply &long:anOperand
        = LongNumber new &int:$self multiply &long:anOperand.

    #method divide &long:anOperand
        = LongNumber new &int:$self divide &long:anOperand.

    #method add &real:anOperand
        = RealNumber new &int:$self add &real:anOperand.

    #method subtract &real:anOperand
        = RealNumber new &int:$self subtract &real:anOperand.

    #method multiply &real:anOperand
        = RealNumber new &int:$self multiply &real:anOperand.

    #method divide &real:anOperand
        = RealNumber new &int:$self divide &real:anOperand.

    #method(type:int) shift &index:anOrder
        => system'internal'int_shift.

    #method read &out'short:aLength
        => system'internal'int_saveshort.

    #method literal = literalConvertor convert &int:$self.

    #method(type:int) not
        => system'internal'int_not.

    #method clone = IntNumber new &int:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb int:$self.
}

// --- longnumber ---

#class(sealed,integer:8) LongNumber :: IntBaseNumber
{
    #constructor new &long:anOperand 
        => system'internal'long_copy.

    #constructor new
        <= %new &long:0l.

    #constructor new &int:anOperand 
        => system'internal'long_copyint.

    #method long = $self.

    #method(type:bool) equal &long:anOperand
        => system'internal'long_equal.

    #method(type:bool) less &long:anOperand
        => system'internal'long_less.

    #method(type:long) add &long:anOperand
        => system'internal'long_add.

    #method(type:long) subtract &long:anOperand
        => system'internal'long_sub.

    #method(type:long) multiply &long:anOperand
        => system'internal'long_mul.

    #method(type:long) divide &long:anOperand
        => system'internal'long_div.

    #method(type:long) and &long:anOperand
        => system'internal'long_and.

    #method(type:long) or &long:anOperand
        => system'internal'long_or.

    #method(type:long) xor &long:anOperand
        => system'internal'long_xor.

    #method add &int:anOperand
        = $self add &long:(system'LongNumber new &int:anOperand).

    #method subtract &int:anOperand
        = $self subtract &long:(system'LongNumber new &int:anOperand).

    #method multiply &int:anOperand
        = $self multiply &long:(system'LongNumber new &int:anOperand).

    #method divide &int:anOperand
        = $self divide &long:(system'LongNumber new &int:anOperand).

    #method equal &int:anOperand 
        = $self equal &long:(system'LongNumber new &int:anOperand).

    #method less &int:anOperand 
        = $self less &long:(system'LongNumber new &int:anOperand).

    #method add &real:anOperand
        = RealNumber new &long:$self add &real:anOperand.

    #method subtract &real:anOperand
        = RealNumber new &long:$self subtract &real:anOperand.

    #method multiply &real:anOperand
        = RealNumber new &long:$self multiply &real:anOperand.

    #method divide &real:anOperand
        = RealNumber new &long:$self divide &real:anOperand.

    #method shift &index:anOrder
        => system'internal'long_shift.

    #method literal = literalConvertor convert &long:$self.

    #method negative = 0l - $self.

    #method not
        => system'internal'long_not.

    #method clone = LongNumber new &long:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb long:$self.
}

// --- realNumber ---

#class(sealed,floating:8) RealNumber :: BaseNumber
{
    #constructor new
        <= %new &real:0.0r.

    #constructor new &real:anOperand 
        => system'internal'long_copy.
  
    #constructor new &int:anOperand 
        => system'internal'real_copyint.

    #constructor new &long:anOperand 
        => system'internal'real_copylong.
        
    #method real = $self.

    #method(type:bool) equal &real:anOperand
        => system'internal'real_equal.

    #method(type:bool) less &real:anOperand
        => system'internal'real_less.

    #method(type:bool) equal &int:anOperand
        = $self equal &real:(system'RealNumber new &int:anOperand).

    #method(type:bool) less &int:anOperand
        = $self less &real:(system'RealNumber new &int:anOperand).

    #method(type:real) add &real:anOperand
        => system'internal'real_add.

    #method(type:real) subtract &real:anOperand
        => system'internal'real_sub.

    #method(type:real) multiply &real:anOperand
        => system'internal'real_mul.

    #method(type:real) divide &real:anOperand
        => system'internal'real_div.

    #method(type:real) add &int:anOperand
        = $self add &real:(system'RealNumber new &int:anOperand).

    #method(type:real) subtract &int:anOperand
        = $self subtract &real:(system'RealNumber new &int:anOperand).

    #method(type:real) multiply &int:anOperand
        = $self multiply &real:(system'RealNumber new &int:anOperand).

    #method(type:real) divide &int:anOperand
        = $self divide &real:(system'RealNumber new &int:anOperand).

    #method(type:real) add &long:anOperand
        = $self add &real:(system'RealNumber new &long:anOperand).

    #method(type:real) subtract &long:anOperand
        = $self subtract &real:(system'RealNumber new &long:anOperand).

    #method(type:real) multiply &long:anOperand
        = $self multiply &real:(system'RealNumber new &long:anOperand).

    #method(type:real) divide &long:anOperand
        = $self divide &real:(system'RealNumber new &long:anOperand).

    #method literal = literalConvertor convert &real:$self.
    
    #method clone = RealNumber new &real:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb real:$self.
}

// --- Integer ---

#class(sealed) Integer :: IntBaseVariable
{
    #field(type:int)theValue.
    
    #constructor new
    [
        theValue := 0.
    ]
    
    #constructor new : aValue
    [
        theValue := aValue int.
    ]
    
    #constructor new &int:aValue
    [
        theValue := aValue.
    ]
    
    #method intvar = $self.
    
    #method int = IntNumber new &int:theValue.
    
    #method real = RealNumber new &int:theValue.
    
    #method long = LongNumber new &int:theValue.
    
    #method read &out'int:aRetVal
    [
        aRetVal := theValue.
    ]
    
    #method write &int:anOperand
    [
        theValue := anOperand.
    ]
    
    #method(type:bool) equal &int:anOperand 
        = theValue == anOperand.

    #method(type:bool) less &int:anOperand
        = theValue < anOperand.
        
    #method add &int:anOperand
        = theValue + anOperand.
    
    #method subtract &int:anOperand
        = theValue - anOperand.
    
    #method multiply &int:anOperand
        = theValue * anOperand.
    
    #method divide &int:anOperand
        = theValue + anOperand.
        
    #method and &int:anOperand
        = theValue && anOperand.
        
    #method or &int:anOperand
        => system'internal'int_or.
        
    #method xor &int:anOperand
        = theValue ^^ anOperand.
        
    #method equal &long:anOperand 
        = LongNumber new &int:theValue equal &long:anOperand.

    #method less &long:anOperand
        = LongNumber new &int:theValue less &long:anOperand.
        
    #method add &long:anOperand
        = LongNumber new &int:theValue add &long:anOperand.
    
    #method subtract &long:anOperand
        = LongNumber new &int:theValue subtract &long:anOperand.
    
    #method multiply &long:anOperand
        = LongNumber new &int:theValue multiply &long:anOperand.
    
    #method divide &long:anOperand
        = LongNumber new &int:theValue divide &long:anOperand.
                
    #method equal &real:anOperand 
        = RealNumber new &int:theValue equal &real:anOperand.

    #method less &real:anOperand
        = RealNumber new &int:theValue less &real:anOperand.
        
    #method add &real:anOperand
        = RealNumber new &int:theValue add &real:anOperand.
    
    #method subtract &real:anOperand
        = RealNumber new &int:theValue subtract &real:anOperand.
    
    #method multiply &real:anOperand
        = RealNumber new &int:theValue multiply &real:anOperand.
    
    #method divide &real:anOperand
        = RealNumber new &int:theValue divide &real:anOperand.

    #method append &int:anOperand
    [
        theValue := theValue + anOperand.
    ]
        
    #method reduce &int:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    #method multiplyBy &int:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    #method divideInto &int:anOperand
    [
        theValue := theValue / anOperand.
    ]
    
    #method cast:aVerb &to:aTarget = aTarget::aVerb int:theValue.

    #method value <= %int.

    #method literal = literalConvertor convert &int:theValue.

    #method clone = Integer new &int:theValue.
    
    #method not
        => system'internal'int_not.
        
    #method shift &index:anOrder
        => system'internal'int_shift.
}

// --- Long ---

#class(sealed) Long :: IntBaseVariable
{
    #field(type:long)theValue.
    
    #constructor new
    [
        theValue := 0l.
    ]
    
    #constructor new : aValue
    [
        theValue := aValue long.
    ]
    
    #constructor new &long:aValue
    [
        theValue := aValue.
    ]
    
    #constructor new &int:aValue
        => system'internal'long_copyint.

    #method longvar = $self.
            
    #method long = LongNumber new &long:theValue.
    
    #method write &long:anOperand
    [
        theValue := anOperand.
    ]
    
//    #method write &int:anOperand
//    [
//        longConvertor convert &int:anOperand &out'long:theValue.
//    ]
//    
//    #method read &out'long:aRetVal
//    [
//        aRetVal := theValue.
//    ]
    
    #method(type:bool) equal &long:anOperand 
        = theValue == anOperand.

    #method(type:bool) less &long:anOperand
        = theValue < anOperand.
        
    #method add &long:anOperand
        = theValue + anOperand.
    
    #method subtract &long:anOperand
        = theValue - anOperand.
    
    #method multiply &long:anOperand
        = theValue * anOperand.
    
    #method divide &long:anOperand
        = theValue + anOperand.
        
    #method and &long:anOperand
        = theValue && anOperand.
        
    #method or &long:anOperand
        => system'internal'long_or.
        
    #method xor &long:anOperand
        = theValue ^^ anOperand.

    #method equal &int:anOperand 
        = $self equal &long:(LongNumber new &int:anOperand).

    #method less &int:anOperand
        = $self less &long:(LongNumber new &int:anOperand).
        
    #method add &int:anOperand
        = $self add &long:(LongNumber new &int:anOperand).
    
    #method subtract &int:anOperand
        = $self subtract &long:(LongNumber new &int:anOperand).
    
    #method multiply &int:anOperand
        = $self multiply &long:(LongNumber new &int:anOperand).
    
    #method divide &int:anOperand
        = $self divide &long:(LongNumber new &int:anOperand).
        
    #method and &int:anOperand
        = $self and &long:(LongNumber new &int:anOperand).
        
    #method or &int:anOperand
        = $self or &long:(LongNumber new &int:anOperand).
        
    #method xor &int:anOperand
        = $self xor &long:(LongNumber new &int:anOperand).
        
    #method equal &real:anOperand 
        = RealNumber new &long:theValue equal &real:anOperand.

    #method less &real:anOperand
        = RealNumber new &long:theValue less &real:anOperand.
        
    #method add &real:anOperand
        = RealNumber new &long:theValue add &real:anOperand.
    
    #method subtract &real:anOperand
        = RealNumber new &long:theValue subtract &real:anOperand.
    
    #method multiply &real:anOperand
        = RealNumber new &long:theValue multiply &real:anOperand.
    
    #method divide &real:anOperand
        = RealNumber new &long:theValue divide &real:anOperand.
                        
    #method append &long:anOperand    
    [
        theValue := theValue + anOperand.
    ]
        
    #method reduce &long:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    #method multiplyBy &long:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    #method divideInto &long:anOperand
    [
        theValue := theValue / anOperand.
    ]
                        
    #method append &int:anOperand    
    [
        theValue := theValue + LongNumber new &int:anOperand.
    ]
        
    #method reduce &int:anOperand
    [
        theValue := theValue - LongNumber new &int:anOperand.
    ]
        
    #method multiplyBy &int:anOperand
    [
        theValue := theValue * LongNumber new &int:anOperand.
    ]        
        
    #method divideInto &int:anOperand
    [
        theValue := theValue / LongNumber new &int:anOperand.
    ]
    
    #method cast : aTypecaster = aTypecaster eval &long:theValue.

    #method value <= %long.

    #method literal = literalConvertor convert &long:theValue.

    #method clone = Long new &long:theValue.
    
    #method not
        => system'internal'long_not.
        
    #method shift &index:anOrder
        => system'internal'long_shift.
}

// --- Real ---

#class(sealed) Real :: BaseVariable
{
    #field(type:real)theValue.
    
    #constructor new
    [
        theValue := 0.0r.
    ]

    #constructor new : aValue
    [
        theValue := aValue real.
    ]
    
    #constructor new &real:aValue
    [
        theValue := aValue.
    ]
    
//    #constructor new &long:aValue
//    [
//        realConvertor convert &long:aValue &out'real:theValue.
//    ]
    
    #constructor new &int:aValue
        => system'internal'real_copyint.
    
    #method realvar = $self.
    
    #method real = RealNumber new &real:theValue.
    
    #method write &real:anOperand
    [
        theValue := anOperand.
    ]
    
    #method write &int:anOperand
    [
        theValue := RealNumber new &int:anOperand.
    ]
//    
//    #method read &out'real:aRetVal
//    [
//        aRetVal := theValue.
//    ]
    
    #method(type:bool) equal &real:anOperand 
        = theValue == anOperand.

    #method(type:bool) less &real:anOperand
        = theValue < anOperand.
        
    #method add &real:anOperand
        = theValue + anOperand.
    
    #method subtract &real:anOperand
        = theValue - anOperand.
    
    #method multiply &real:anOperand
        = theValue * anOperand.
    
    #method divide &real:anOperand
        = theValue / anOperand.
    
    #method equal &int:anOperand 
        = $self equal &real:(RealNumber new &int:anOperand).

    #method less &int:anOperand
        = $self less &real:(RealNumber new &int:anOperand).
        
    #method add &int:anOperand
        = $self add &real:(RealNumber new &int:anOperand).
    
    #method subtract &int:anOperand
        = $self subtract &real:(RealNumber new &int:anOperand).
    
    #method multiply &int:anOperand
        = $self multiply &real:(RealNumber new &int:anOperand).
    
    #method divide &int:anOperand
        = $self divide &real:(RealNumber new &int:anOperand).
    
    #method equal &long:anOperand 
        = $self equal &real:(RealNumber new &long:anOperand).

    #method less &long:anOperand
        = $self less &real:(RealNumber new &long:anOperand).
        
    #method add &long:anOperand
        = $self add &real:(RealNumber new &long:anOperand).
    
    #method subtract &long:anOperand
        = $self subtract &real:(RealNumber new &long:anOperand).
    
    #method multiply &long:anOperand
        = $self multiply &real:(RealNumber new &long:anOperand).
    
    #method divide &long:anOperand
        = $self divide &real:(RealNumber new &long:anOperand).
    
    #method append &real:anOperand    
    [
        theValue := theValue + anOperand.
    ]
        
    #method reduce &real:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    #method multiplyBy &real:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    #method divideInto &real:anOperand
    [
        theValue := theValue / anOperand.
    ]
    
    #method append &int:anOperand    
    [
        theValue := theValue add &int:anOperand.
    ]
        
    #method reduce &int:anOperand
    [
        theValue := theValue subtract &int:anOperand.
    ]
        
    #method multiplyBy &int:anOperand
    [
        theValue := theValue multiply &int:anOperand.
    ]        
        
    #method divideInto &int:anOperand
    [
        theValue := theValue divide &int:anOperand.
    ]
    
    #method append &long:anOperand    
    [
        theValue := theValue add &long:anOperand.
    ]
        
    #method reduce &long:anOperand
    [
        theValue := theValue subtract &long:anOperand.
    ]
        
    #method multiplyBy &long:anOperand
    [
        theValue := theValue multiply &long:anOperand.
    ]        
        
    #method divideInto &long:anOperand
    [
        theValue := theValue divide &long:anOperand.
    ]
    
    #method cast:aVerb &to:aTarget = aTarget::aVerb real:theValue.

    #method value <= %real.

    #method literal = literalConvertor convert &real:theValue.

    #method clone = Real new &real:theValue.
}

// --- CharValue ---

#class(sealed,struct) CharValue :: BaseValue
{
    #field(type:short)theValue.
    
    #constructor new
    [
        theValue := 0.
    ]
    
    #constructor new &short:aCode
    [
        theValue := aCode.
    ]
    
    #constructor new &int:aCode
    [
        theValue := aCode short.
    ]

    #method char = $self.

    #method equal &int:anOperand 
        => system'internal'int_equal.

    #method less &int:anOperand 
        => system'internal'int_less.

    #method equal &char:anOperand 
        => system'internal'int_equal.

    #method less &char:anOperand 
        => system'internal'int_less.

    #method equal &literal:aLiteral
        = $self literal equal &literal:aLiteral.

    #method less &literal:aLiteral
        = $self literal less &literal:aLiteral.
    
//    #method short = ShortNumber new &short:theValue.

    #method int = IntNumber new &short:theValue.

    #method literal = system'LiteralValue new &length:1 &short:theValue literal.

    #method clone = CharValue new &short:theValue.
    
    #method cast:aVerb &to:aTarget = aTarget::aVerb char:$self.
}

// === Basic Factory symbols ==

#class Variable
{
    #field theValue.
    
    #constructor new
    [
    ]        
        
    #constructor new : aValue
    [
        theValue := aValue.
    ]
  
    #method write : aValue
    [
        theValue := aValue.
    ]
        
    #method value = theValue.        
        
    #method append : aValue
    [
        (nil == theValue)
            ? [ theValue := aValue. ]
            ! [ theValue := theValue + aValue. ].
    ]
    
    #method reduce : aValue
    [
        (nil == theValue)
            ? [ theValue := aValue. ]
            ! [ theValue := theValue - aValue. ].
    ]

    #method multiplyBy : aValue
    [
        (nil == theValue)
            ? [ theValue := aValue. ]
            ! [ theValue := theValue * aValue. ].
    ]
    
    #method divideInto : aValue
    [
        (nil == theValue)
            ? [ theValue := aValue. ]
            ! [ theValue := theValue / aValue. ].
    ]
        
    #method => theValue.
}

// === Basic Convertors ===

// --- convertor helpers ---

#symbol intConvertor =
{
    convert &literal:aValue &out'int:aRetVal
        => system'internal'int_copystr.

    convert &literal:aLiteral
    [
        #var(type:int)aRetVal := $self convert &literal:aLiteral.
        
        ^ IntNumber new &int:aRetVal.
    ]

    convert &real:aReal &out'int:aRetVal
        => system'internal'int_copyreal.

    convert &real:aReal
    [
        #var(type:int)aRetVal := $self convert &real:aReal.
        
        ^ IntNumber new &int:aRetVal.
    ]
                                    
    convert &int:anInt
        = anInt.
}.

#symbol longConvertor =
{
    convert &int:aValue
        = LongNumber new &int:aValue.

    convert &literal:aValue &out'long:aRetVal
        => system'internal'long_copystr.
    
    convert &literal:aLiteral
    [
        #var(type:long)aRetVal := $self convert &literal:aLiteral.
        
        ^ LongNumber new &long:aRetVal.
    ]
    
    convert &long:aLong
        = aLong.
}.

#symbol realConvertor =
{
    convert &int:aValue &out'real:aRetVal
        => system'internal'real_saveint.

    convert &literal:aValue &out'real:aRetVal
        => system'internal'real_copystr.
        
    convert &literal:aLiteral
    [
        #var(type:real)aRetVal := $self convert &literal:aLiteral.
        
        ^ RealNumber new &real:aRetVal.
    ]

    convert &real:aReal
        = aReal.
        
    convert &int:anInt
        = RealNumber new &int:anInt.
        
    convert &long:aLong
        = RealNumber new &long:aLong.
}.

// === Typecaster ==

#symbol typeCaster
    = { ! : anObject = $subject. }.
