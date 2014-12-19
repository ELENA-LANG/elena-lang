// === basic data classes ===

//#define LongNumber = system'LongNumber.
//#define RealNumber = system'RealNumber.

//#define(const)intConvertor  = system'intConvertor.
//#define(const)longConvertor = system'longConvertor.
//#define(const)realConvertor = system'realConvertor.
#define(const)literalConvertor = system'literalConvertor.

// --- BaseValue ---

#class BaseValue
{
    #method equal : anObject = $self equal::(anObject value).
    
    #method less : anObject = $self less::(anObject value).
    
    #method greater : anObject = anObject less:$self.
}

// --- BaseNumber ---

#class BaseNumber : BaseValue
{
    #method add : anObject = $self add::(anObject value).
    
    #method subtract : anObject = $self subtract::(anObject value).
    
    #method multiply : anObject = $self multiply::(anObject value).
    
    #method divide : anObject = $self divide::(anObject value).    
}

// --- IntBaseNumber ---

#class IntBaseNumber : BaseNumber
{
    #method and : anOperand = $self and::(anOperand value).
    
    #method or : anOperand = $self or::(anOperand value).
    
    #method xor : anOperand = $self xor::(anOperand value).
    
    #method allMask : anOperand
        = $self and:anOperand == anOperand.
        
    #method anyMask : anOperand
        = 0 != $self and:anOperand.
        
    #method negative = 0 subtract:$self.
}

// --- IntBaseVariable ---

#class IntBaseVariable : IntBaseNumber
{
    #method write : anOperand
    [
        $self write::(anOperand value).
    ]
    
    #method append : anOperand
    [
        $self append::(anOperand value).
    ]
    
    #method reduce : anOperand
    [
        $self reduce::(anOperand value).
    ]
    
    #method multiplyBy : anOperand
    [
        $self multiplyBy::(anOperand value).
    ]
    
    #method divideInto : anOperand
    [
        $self divideInto::(anOperand value).
    ]
}

// --- BaseNumber ---

#class BaseVariable : BaseNumber
{
    #method write : anOperand
    [
        $self write::(anOperand value).
    ]
    
    #method append : anOperand
    [
        $self append::(anOperand value).
    ]
    
    #method reduce : anOperand
    [
        $self reduce::(anOperand value).
    ]
    
    #method multiplyBy : anOperand
    [
        $self multiplyBy::(anOperand value).
    ]
    
    #method divideInto : anOperand
    [
        $self divideInto::(anOperand value).
    ]
}

// --- ByteNumber ---

#class(sealed,type:byte,integer:1) ByteNumber : IntBaseNumber
{
}

// --- ShortNumber ---

#symbol(const)shortMaximalValue = 65535.

#class(sealed,type:short,integer:2) ShortNumber : IntBaseNumber
{
    #constructor new &int:anOperand
        <int_copyshort>.
        
    #constructor new &short:anOperand
        <int_copy>.
        
    #method short = $self.
    
    #method int = IntNumber new &short:$self int.
}

// --- IntNumber ---

#class(sealed,type:int,integer:4) IntNumber : IntBaseNumber
{
    #constructor new &int:anOperand
        <int_copy>.

    #constructor new &short:anOperand
        <int_copyshort>.

    #constructor new
        <int_init>.

    #method int = $self.

    #method short = ShortNumber new &int:$self.

    #method equal &int:anOperand 
        <int_equal>.

    #method less &int:anOperand
        <int_less>.

    #method add &int:anOperand
        <int_add>.

    #method subtract &int:anOperand
        <int_sub>.

    #method multiply &int:anOperand
        <int_mul>.

    #method divide &int:anOperand
        <int_div>.

    #method and &int:anOperand
        <int_and>.

    #method or &int:anOperand
        <int_or>.

    #method xor &int:anOperand
        <int_xor>.

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

    #method shift &index:anOrder
        <int_shift>.

    #method read &out'short:aLength
        <int_saveshort>.

    #method literal = literalConvertor convert &int:$self literal.

    #method not
        <int_not>.

    #method value = $self.
    
    #method clone = IntNumber new &int:$self.
    
    #method cast : aTypecaster = aTypecaster int:$self.
}

// --- longnumber ---

#class(sealed,type:long,integer:8) LongNumber : IntBaseNumber
{
    #constructor new &long:anOperand 
        <long_copy>.

    #constructor new
        <= %new &long:0l.

    #constructor new &int:anOperand 
        <long_copyint>.

    #method long = $self.

    #method equal &long:anOperand
        <long_equal>.

    #method less &long:anOperand
        <long_less>.

    #method add &long:anOperand
        <long_add>.

    #method subtract &long:anOperand
        <long_sub>.

    #method multiply &long:anOperand
        <long_mul>.

    #method divide &long:anOperand
        <long_div>.

    #method and &long:anOperand
        <long_and>.

    #method or &long:anOperand
        <long_or>.

    #method xor &long:anOperand
        <long_xor>.

    #method add &int:anOperand
        = $self add &long::(LongNumber new &int:anOperand).

    #method subtract &int:anOperand
        = $self subtract &long::(LongNumber new &int:anOperand).

    #method multiply &int:anOperand
        = $self multiply &long::(LongNumber new &int:anOperand).

    #method divide &int:anOperand
        = $self divide &long::(LongNumber new &int:anOperand).

    #method equal &int:anOperand 
        = $self equal &long::(LongNumber new &int:anOperand).

    #method less &int:anOperand 
        = $self less &long::(LongNumber new &int:anOperand).

    #method add &real:anOperand
        = RealNumber new &long:$self add &real:anOperand.

    #method subtract &real:anOperand
        = RealNumber new &long:$self subtract &real:anOperand.

    #method multiply &real:anOperand
        = RealNumber new &long:$self multiply &real:anOperand.

    #method divide &real:anOperand
        = RealNumber new &long:$self divide &real:anOperand.

    #method shift &index:anOrder
        <long_shift>.

    #method literal = literalConvertor convert &long:$self literal.

    #method negative = 0l - $self.

    #method not
        <long_not>.

    #method clone = LongNumber new &long:$self.
    
    #method value = $self.
    
    #method cast : aTypecaster = aTypecaster long:$self.
}

// --- realNumber ---

#class(sealed,type:real,floating:8) RealNumber : BaseNumber
{
    #constructor new
        <= %new &real:0.0r.

    #constructor new &real:anOperand 
        <long_copy>.
  
    #constructor new &int:anOperand 
        <real_copyint>.

//    #constructor new &long:anOperand 
//        <real_copylong>.
        
    #method real = $self.

    #method equal &real:anOperand
        <real_equal>.

    #method less &real:anOperand
        <real_less>.

    #method equal &int:anOperand
        = $self equal &real::(RealNumber new &int:anOperand).

    #method less &int:anOperand
        = $self less &real::(RealNumber new &int:anOperand).

    #method add &real:anOperand
        <real_add>.

    #method subtract &real:anOperand
        <real_sub>.

    #method multiply &real:anOperand
        <real_mul>.

    #method divide &real:anOperand
        <real_div>.

    #method add &int:anOperand
        = $self add &real::(RealNumber new &int:anOperand).

    #method subtract &int:anOperand
        = $self subtract &real::(RealNumber new &int:anOperand).

    #method multiply &int:anOperand
        = $self multiply &real::(RealNumber new &int:anOperand).

    #method divide &int:anOperand
        = $self divide &real::(RealNumber new &int:anOperand).

    #method add &long:anOperand
        = $self add &real::(RealNumber new &long:anOperand).

    #method subtract &long:anOperand
        = $self subtract &real::(RealNumber new &long:anOperand).

    #method multiply &long:anOperand
        = $self multiply &real::(RealNumber new &long:anOperand).

    #method divide &long:anOperand
        = $self divide &real::(RealNumber new &long:anOperand).

    #method modulus &out'real:aRetVal
        <r_abs>.
        
    #method literal = literalConvertor convert &real:$self literal.
    
    #method value = $self.

    #method clone = RealNumber new &real:$self.
    
    #method cast : aTypecaster = aTypecaster real:$self.
}

// --- Integer ---

#class(sealed,type:intvar) Integer : IntBaseVariable
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
    
    #method equal &int:anOperand 
        = theValue == anOperand.

    #method less &int:anOperand
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
        = theValue || anOperand.
        
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
    
    #method cast : aTypecaster = aTypecaster eval &int::$self.

    #method value <= %int.

    #method literal = system'literalConvertor convert &int:theValue literal.

    #method clone = Integer new &int:theValue.
    
    #method not
        <int_not>.
        
    #method shift &index:anOrder
        <int_shift>.
}

// --- Long ---

#class(sealed,type:longvar) Long : IntBaseVariable
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
        <long_copyint>.

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
    
    #method equal &long:anOperand 
        = theValue == anOperand.

    #method less &long:anOperand
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
        = theValue || anOperand.
        
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

    #method literal = system'literalConvertor convert &long:theValue literal.

    #method clone = Long new &long:theValue.
    
    #method not
        <long_not>.
        
    #method shift &index:anOrder
        <long_shift>.
}

// --- Real ---

#class(sealed,type:realvar) Real : BaseVariable
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
        <real_copyint>.
    
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
    
    #method equal &real:anOperand 
        = theValue == anOperand.

    #method less &real:anOperand
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
    
    #method cast : aTypecaster = aTypecaster eval &real::$self.

    #method value <= %real.

    #method literal = system'literalConvertor convert &real:theValue literal.

    #method clone = Real new &real:theValue.
}

// --- CharValue ---

#class(sealed,type:char,struct) CharValue : BaseValue
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
        <int_equal>.

    #method less &int:anOperand 
        <int_less>.

    #method equal &short:anOperand 
        <int_equal>.

    #method less &short:anOperand 
        <int_less>.

    #method equal &literal:aLiteral
        = $self literal equal &literal:aLiteral.

    #method less &literal:aLiteral
        = $self literal less &literal:aLiteral.
    
//    #method short = ShortNumber new &short:theValue.

    #method int = IntNumber new &short:theValue.

    #method literal = system'LiteralValue new &length:1 &short:theValue literal.

    #method clone = CharValue new &short:theValue.
    
    #method value = ShortNumber new &short:theValue.

    #method cast : aSelector
        = aSelector is &numeric 
            ? [ aSelector eval &int::$self. ]
            ! [ aSelector eval &char:$self. ].
}

// === Basic prototypes ===

// --- Enumerator ---

#class(limited,type:enumerator) Enumerator
{
    #method enumerator = $self.
    
    #method get = nil.
    
    #method next = system'false.
    
    #method reset []
}

// --- Iterator ---

#class(limited,type:iterator) Iterator
{
    #method available = false.

    #method get = nil.
    
    #method set : anObject []
    
    #method read &out'index:anIndex []
    
    #method write &index:anIndex []
    
    #method append &index:anOffset
    [
        #var(type:int)anIndex.
        $self read &out'index:anIndex.
        
        $self write &index:(anIndex + anOffset).
    ]
        
    #method reduce &index:anOffset
    [
        #var(type:int)anIndex.
        $self read &out'index:anIndex.
        
        $self write &index:(anIndex - anOffset).
    ]
    
    #method index
    [
        #var(type:int)anIndex.
        $self read &out'index:anIndex.
        
        ^ IntNumber new &int:anIndex.
    ]
}

// === Basic Convertors ===

// --- convertor helpers ---

#symbol intConvertor =
{
    convert &literal:aValue &out'int:aRetVal
        <int_copystr>.    
    
    convert &literal:aLiteral
    [
        #var(type:int)aRetVal.
        $self convert &literal:aLiteral &out'int:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    convert &real:aReal &out'int:aRetVal
        <int_copyreal>.

    convert &real:aReal
    [
        #var(type:int)aRetVal.
        $self convert &real:aReal &out'int:aRetVal.
        
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
        <long_copystr>.
    
    convert &literal:aLiteral
    [
        #var(type:long)aRetVal.
        $self convert &literal:aLiteral &out'long:aRetVal.
        
        ^ LongNumber new &long:aRetVal.
    ]
    
    convert &long:aLong
        = aLong.
}.

#symbol realConvertor =
{
    convert &int:aValue &out'real:aRetVal
        <real_saveint>.    

    convert &literal:aValue &out'real:aRetVal
        <real_copystr>.
        
    convert &literal:aLiteral
    [
        #var(type:real)aRetVal.
        $self convert &literal:aLiteral &out'real:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    convert &real:aReal
        = aReal.
        
    convert &int:anInt
        = RealNumber new &int:anInt.
        
    convert &long:aLong
        = RealNumber new &long:aLong.
}.

#symbol literalConvertor =
{
    convert &int:aNumber
        <ws_copyint>.

    convert &int:aNumber &index:aRadix
        <ws_copyint_radix>.

//    #method convert &long:aNumber &base:aRadix
//        = literalPrimitiveOp toLiteral &long:aNumber &index::aRadix.

    convert &long:aNumber
        <ws_copylong>.

    convert &real:aNumber
        <ws_copyreal>.
        
    convert &object:anObject
        = anObject literal.
        
    convert &literal:aLiteral
        = aLiteral.
}.

// === Basic Factory symbols ==

#class Variable
{
    #field theValue.
    
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
        theValue := theValue + aValue.
    ]
    
    #method reduce : aValue
    [
        theValue := theValue - aValue.
    ]

    #method multiplyBy : aValue
    [
        theValue := theValue * aValue.
    ]
    
    #method divideInto : aValue
    [
        theValue := theValue / aValue.
    ]
        
    #method => theValue.
}

////// === Typecaster ==
////
////#symbol literalDetector
////    = { isLiteral = true. literal : aLiteral = true. ! : anObject = false. ! = false. }.
////
////#symbol intDetector
////    = { isNumeric = true. int : anInt = true. long : aLong = true. ! : anObject = false. ! = false. }.
////
////#symbol numericDetector
////    = { isNumeric = true. int : anInt = true. long : aLong = true. real : aReal = true. ! : anObject = false. ! = false. }.
