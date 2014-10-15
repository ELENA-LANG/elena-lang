// === basic data classes ===

#define LongNumber = system'LongNumber.
#define RealNumber = system'RealNumber.

#define(const)intConvertor  = system'intConvertor.
#define(const)longConvertor = system'longConvertor.
#define(const)realConvertor = system'realConvertor.
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
        
    #method negative = 0 subtract::$self.
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

#class(sealed,type:byte,dbg:int) ByteNumber : IntBaseNumber
{
    #field(size:1)theData.
}

// --- ShortNumber ---

#symbol(const)shortMaximalValue = 65535.

#class(sealed,type:short,dbg:int) ShortNumber : IntBaseNumber
{
    #field(size:2)theData.
    
    #constructor new &int:anOperand
        <int_copyshort>.
        
    #constructor new &short:anOperand
        <int_copy>.
        
    #method short = $self.
    
    #method int = IntNumber new &short:$self int.
}

// --- IntNumber ---

#class(sealed,type:int,dbg:int) IntNumber : IntBaseNumber
{
    #field(size:4)theData.

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

#class(sealed,type:long,dbg:long) LongNumber : IntBaseNumber
{
    #field(size:8)theData.

    #constructor new &long:anOperand 
        <long_copy>.

    #constructor new
        <= %new &long:0l.

    #constructor new &int:anOperand 
    [
        longConvertor convert &int:anOperand &out'long:$self.
    ]
  
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

#class(sealed,type:real,dbg:real) RealNumber : BaseNumber
{
    #field(size:8)theData.

    #constructor new
        <= %new &real:0.0r.

    #constructor new &real:anOperand 
        <long_copy>.
  
    #constructor new &int:anOperand 
    [
        realConvertor convert &int:anOperand &out'real:$self.
    ]
        
    #constructor new &long:anOperand 
    [
        realConvertor convert &long:anOperand &out'real:$self.
    ]
        
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

    #method literal = literalConvertor convert &real:$self literal.
    
    #method value = $self.

    #method clone = RealNumber new &real:$self.
    
    #method cast : aTypecaster = aTypecaster real:$self.
}

// --- Integer ---

#class(dbg:int,sealed) Integer : IntBaseVariable
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

#class(dbg:long,sealed) Long : IntBaseVariable
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
    [
        longConvertor convert &int:aValue &out'long:theValue.
    ]
    
    #method long = LongNumber new &long:theValue.
    
    #method write &long:anOperand
    [
        theValue := anOperand.
    ]
    
    #method write &int:anOperand
    [
        longConvertor convert &int:anOperand &out'long:theValue.
    ]
    
    #method read &out'long:aRetVal
    [
        aRetVal := theValue.
    ]
    
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
    
    #method cast : aTypecaster = aTypecaster eval &long::$self.

    #method value <= %long.

    #method literal = system'literalConvertor convert &long:theValue literal.

    #method clone = Long new &long:theValue.
    
    #method not
        <long_not>.
        
    #method shift &index:anOrder
        <long_shift>.
}

// --- Real ---

#class(dbg:real,sealed) Real : BaseVariable
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
    
    #constructor new &long:aValue
    [
        realConvertor convert &long:aValue &out'real:theValue.
    ]
    
    #constructor new &int:aValue
    [
        realConvertor convert &int:aValue &out'real:theValue.
    ]
    
    #method real = RealNumber new &real:theValue.
    
    #method write &real:anOperand
    [
        theValue := anOperand.
    ]
    
    #method write &int:anOperand
    [
        theValue := RealNumber new &int:anOperand.
    ]
    
    #method read &out'real:aRetVal
    [
        aRetVal := theValue.
    ]
    
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

#class(dbg:int) CharValue : BaseValue
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
    
    #method short = ShortNumber new &short:theValue.

    #method int = IntNumber new &short:theValue.

    #method literal = system'LiteralValue new &length:1 &short:theValue literal.
    
    #method clone = CharValue new &short:theValue.
    
    #method value = ShortNumber new &short:theValue.
    
    #method cast : aSelector
        = aSelector isNumeric 
            ? [ aSelector eval &int::$self. ]
            ! [aSelector eval &literal::$self. ].
}

// --- emptyliteralvalue ---

#symbol(const) emptyLiteralValue = "".

// --- literalvalue ---

#class LiteralIterator
{
    #field(type:literal) theLiteral.
    #field theIndex.
    
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := system'Integer new &int:(-1).
    ]

    #method index = theIndex int.

    #method reset
    [
        theIndex write &int:-1.
    ]

    #method append &index:anIndex
    [
        theIndex append &int:anIndex.
    ]

    #method set &index:anIndex
    [
        theIndex write &int:anIndex.
    ]

    #method available = theIndex less &int:(theLiteral length).
    
    #method next
    [
        theIndex append &int:1.

        ^ theIndex less &int:(theLiteral length).
    ]
    
    #method get
    [
        #var(type:int)anIndex.
        theIndex read &out'int:anIndex.
        
        #var(type:short)aChar.
        theLiteral read &index:anIndex &out'short:aChar.
        
        ^ CharValue new &short:aChar.
    ]
}

#class(type:literal,sealed,dbg:literal) LiteralValue : BaseValue
{
    #field(itemsize:2)theData.

    #constructor new &length:aSize 
        <ws_create>.

    #constructor new &length:aSize &literal:aLiteral
        <ws_copy>.

    #constructor new &length:aSize &shortarray:anArray
        <ws_copyarr>.

    #constructor new &length:aSize &index:anIndex &shortarray:anArray
        <ws_copysubarr>.

    #constructor new &length:aSize &short:aChar
        <ws_fill>.

    #constructor new &literal:aLiteral
        <= %new &length::aLiteral &literal:aLiteral.

    #method literal = $self.

    #method equal &literal : aLiteral
        <ws_equal>.

    #method less &literal : aLiteral
        <ws_less>.

    #method add : aLiteral
        = $self add &literal::aLiteral.

    #method add &literal : aLiteral
        <ws_add>.

    #method getAt : aParam
        <= %getAt &index:(aParam int).

    #method getAt &index:anIndex
    [
        #var(type:short)aChar.
        $self read &index:anIndex &out'short:aChar.
        
        ^ CharValue new &short:aChar.
    ]

    #method read &index:anIndex &out'short:aChar
        <ws_getat>.

    #method indexOf &index:anIndex &literal:aLiteral &out'int:aRetVal
        <ws_indexof>.

    #method indexOf &index:anIndex &literal:aLiteral
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &literal:aLiteral &out'int:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method indexOf &index:anIndex &char:aChar &out'int:aRetVal
        <= %indexOf &index:anIndex &literal::aChar &out'int:aRetVal.

    #method indexOf &index:anIndex &char:aChar
        <= %indexOf &index:anIndex &literal::aChar.

    #method insert &index:anIndex &literal:aLiteral
        <ws_insert>.

    #method delete &index:anIndex &length:aLength
        <ws_delete>.

    #method Substring &index:anIndex &length:aLength
        <ws_subs>.

    #method length
    [
        #var(type:int)aLength.
        $self read &out'length:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method read &out'length:aLength
        <ws_getlen>.

    #method enumerator = LiteralIterator new &literal:$self.
    
    #method cast : aTypecaster = aTypecaster literal:$self.
    
    #method clone = LiteralValue new &literal:$self.
    
    #method value = $self.
}

// --- String ---

#class String : BaseValue
{
    #field(type:literal)theValue.

    #constructor new
    [
        theValue := emptyLiteralValue.
    ]
            
    #constructor new : aValue
    [
        theValue := aValue literal.
    ]
    
    #method write : anOperand
    [
        theValue := anOperand literal.
    ]
    
    #method write &literal:aLiteral
    [
        theValue := aLiteral.
    ]
    
    #method append : anOperand
    [
        theValue := theValue + anOperand.
    ]
    
    #method append &literal:anOperand
    [
        theValue := theValue add &literal:anOperand.
    ]
    
    #method insert &index:anIndex &literal:aLiteral
    [
        (anIndex == 0)
            ? [ theValue := aLiteral + theValue. ]
            ! [ theValue := theValue insert &index:anIndex &literal:aLiteral. ].
    ]
            
    #method delete &index:anIndex &length:aLength
    [
        theValue := theValue delete &index:anIndex &length:aLength.
    ]
                        
    #method clear
    [
        theValue := emptyLiteralValue.
    ]
    
    #method clone = String new &literal:theValue.
                
    #method cast : aTypecaster = aTypecaster literal:theValue.
    
    #method literal = theValue.
    
    #method value = theValue.
    
    #method => theValue.
}

// --- Boolean ---

#class Boolean : BaseValue
{
    #field theValue.

    #constructor new : aValue
    [        
        theValue := true == aValue.
    ]

    #method value = theValue.

    #method write : aValue
    [
        theValue := true == aValue.
    ]

    #method bool = theValue bool.

    #method append : aValue
    [
        theValue := theValue or:aValue.
    ]
            
    #method multiplyBy : aValue
    [
        theValue := theValue and:aValue.
    ]
            
    #method invert
    [
        theValue := theValue not.
    ]            

    #method equal &bool:aValue
        => theValue.
            
    #method clone = Boolean new:($self value).

    #method cast : aTypecaster = aTypecaster bool:($self value).
            
    #method => theValue.
}

// --- ArrayIterator ---

#class ArrayIterator
{
    #field theArray.
    #field theIndex.
    #field(type:int)theCount.
    
    #constructor new &array:anArray
    [
        theArray := anArray.
        theCount := anArray length.
        theIndex := Integer new &int:-1.
    ]
    
    #method index = theIndex int.

    #method reset
    [
        theIndex write &int:-1.
        theCount := theArray length.
    ]

    #method set &index:anIndex
    [
        theIndex write &int:anIndex.
    ]
    
    #method append &index:anIndex
    [
        theIndex append &int:anIndex.
    ]

    #method available = theIndex less &int:theCount.
  
    #method next
    [
        theIndex append &int:1.

        ^ theIndex less &int:theCount.
    ]

    #method get
    [
        #var(type:int)anIndex.
        theIndex read &out'int:anIndex.
        
        ^ theArray getAt &index:anIndex.
    ]

    #method set : anObject
    [
        #var(type:int)anIndex.
        theIndex read &out'int:anIndex.
        
        theArray setAt &index:anIndex &object:anObject.
    ]
}

#class BaseArray
{
    #field(item)theItem.

    #constructor new &length:aLength
        <arr_create>.
}

// --- Array ---

#class(sealed,type:array,dbg:array) Array : BaseArray
{
    #method getAt : anIndex
        = $self getAt &index:(anIndex int).

    #method getAt &index:anIndex
        <arr_getat>.

    #method setAt:anIndex:anObject
    [
        $self setAt &index:(anIndex int) &object:anObject.
    ]
    
    #method setAt &index:anIndex &object:anObject
        <arr_setat>.
        
    #method add : anObject
        = $self add &array::anObject.
        
    #method add &array:anArray
    [
        #var(type:int)aLength1. 
        $self read &out'length:aLength1.
        
        #var(type:int)aLength2.
        anArray read &out'length:aLength2.
        
        #var aRetVal := Array new &length:(aLength1 + aLength2).

        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength1) ?
        [
            aRetVal setAt &index:anIndex &object:(self getAt &index:anIndex).
            
            anIndex := anIndex + 1.
        ].

        anIndex := 0.
        #loop (anIndex < aLength2) ?
        [
            aRetVal setAt &index:(aLength1 + anIndex) &object:(anArray getAt &index:anIndex).
            
            anIndex := anIndex + 1.
        ].
            
        ^ aRetVal.
    ]

    #method array = $self.

    #method length
    [
        #var(type:int)aLength.
        $self read &out'length:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method read &out'length:aLength
        <arr_getlen>.

    #method enumerator = ArrayIterator new &array:$self.

    #method clone 
    [
        #var(type:int)aLength. 
        $self read &out'length:aLength.
        
        #var aRetVal := Array new &length:aLength.
        
        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength) ?
        [
            aRetVal setAt &index:anIndex &object:($self getAt &index:anIndex).
            
            anIndex := anIndex + 1.
        ].
            
        ^ aRetVal.
    ]
}

// --- paramsOp ---

#class(extension:params)$paramsOp
{
    #method length
    [
        #var(type:int)aLength.
        $self read &out'length:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method read &out'length:aLength
        <args_getlen>.
}

// === Basic Convertors ===

// --- convertor helpers ---

#class(role) intConvertor
{
    #method convert &literal:aValue &out'int:aRetVal
        <int_copystr>.    
    
    #method convert &literal:aLiteral
    [
        #var(type:int)aRetVal.
        $self convert &literal:aLiteral &out'int:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method convert &real:aReal &out'int:aRetVal
        <int_copyreal>.

    #method convert &real:aReal
    [
        #var(type:int)aRetVal.
        $self convert &real:aReal &out'int:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
                                    
    #method convert &int:anInt
        = anInt.
}

#class(role) longConvertor
{
    #method convert &int:aValue &out'long:aRetVal
        <long_copyint>.    
    
    #method convert &literal:aValue &out'long:aRetVal
        <long_copystr>.
    
    #method convert &literal:aLiteral
    [
        #var(type:long)aRetVal.
        $self convert &literal:aLiteral &out'long:aRetVal.
        
        ^ LongNumber new &long:aRetVal.
    ]
    
    #method convert &long:aLong
        = aLong.
}

#class(role) realConvertor
{
    #method convert &int:aValue &out'real:aRetVal
        <real_copyint>.    

    #method convert &literal:aValue &out'real:aRetVal
        <real_copystr>.
        
    #method convert &literal:aLiteral
    [
        #var(type:real)aRetVal.
        $self convert &literal:aLiteral &out'real:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method convert &real:aReal
        = aReal.
        
    #method convert &int:anInt
        = RealNumber new &int:anInt.
        
    #method convert &long:aLong
        = RealNumber new &long:aLong.
}

#class(role) literalConvertor
{
    #method convert &int:aNumber
        <ws_copyint>.

    #method convert &int:aNumber &base:aRadix
        <ws_copyint_radix>.

//    #method convert &long:aNumber &base:aRadix
//        = literalPrimitiveOp toLiteral &long:aNumber &index::aRadix.

    #method convert &long:aNumber
        <ws_copylong>.

    #method convert &real:aNumber
        <ws_copyreal>.
        
    #method convert &object:anObject
        = anObject literal.
        
    #method convert &literal:aLiteral
        = aLiteral.
}

// --- Variant ---

#class Variant : String
{
    #method int
    [
        #var(type:int)retVal.
        intConvertor convert &literal::theValue &out'int:retVal.
        
        ^ IntNumber new &int:retVal.
    ]

    #method long
    [
        #var(type:long)retVal.
        longConvertor convert &literal::theValue &out'long:retVal.
        
        ^ LongNumber new &long:retVal.
    ]

    #method real
    [
        #var(type:real)retVal.
        realConvertor convert &literal::theValue &out'real:retVal.
        
        ^ RealNumber new &real:retVal.
    ]

    #method literal = theValue.
}

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
