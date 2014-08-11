// === basic data classes ===

#define LongNumber = system'LongNumber.
#define RealNumber = system'RealNumber.

#define(const)intConvertor  = system'intConvertor.
#define(const)longConvertor = system'longConvertor.
#define(const)realConvertor = system'realConvertor.
#define(const)literalConvertor = system'literalConvertor.

// === basic types ===

#type(size:2,role:ShortNumber)      short.
#type(size:4,role:IntNumber)        int.
#type(role:LiteralValue)            literal.
#type(size:8,role:LongNumber)       long.
#type(size:8,role:RealNumber)       real.

#type(size:4,role:IntNumber)        length.
#type(size:4,role:IntNumber)        index.

#type(role:ArgArray)                params.

// --- BaseValue ---

#class BaseValue
{
    #method equal : anObject = $self equal::anObject.
    
    #method greater : anObject = anObject less:$self.
    
    #method less : anObject = $self less::anObject.
    
    #method equal &object:anObject = $self equal::($self cast:anObject).
    
    #method less &object:anObject = $self less::($self cast:anObject).
    
    #method greater &object:anObject = $self greater::($self cast:anObject).
}

// --- BaseNumber ---

#class BaseNumber : BaseValue
{
    #method add : anOperand = $self add::anOperand.
    
    #method subtract : anOperand = $self subtract::anOperand.
    
    #method multiply : anOperand = $self multiply::anOperand.
    
    #method divide : anOperand = $self divide::anOperand.
    
    #method add &object : anObject = $self add::($self cast:anObject).
    
    #method subtract &object : anObject = $self subtract::($self cast:anObject).
    
    #method multiply &object : anObject = $self multiply::($self cast:anObject).
    
    #method divide &object : anObject = $self divide::($self cast:anObject).
}

// --- IntBaseNumber ---

#class IntBaseNumber : BaseNumber
{
    #method and : anOperand = $self and::anOperand.
    
    #method or : anOperand = $self or::anOperand.
    
    #method xor : anOperand = $self xor::anOperand.
    
    #method and &object:anOperand = $self and::($self cast:anOperand).
    
    #method or &object:anOperand = $self or::($self cast:anOperand).
    
    #method xor &object:anOperand = $self xor::($self cast:anOperand).
    
    #method allMask : anOperand
        = $self and:anOperand == anOperand.
        
    #method anyMask : anOperand
        = 0 != $self and:anOperand.
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

    #method negative = 0 - $self.

    #method not
        <int_not>.

    #method cast : anObject = anObject int.

    #method clone = IntNumber new &int:$self.
}


// --- longnumber ---

#class(sealed,type:long,dbg:long) LongNumber : IntBaseNumber
{
    #field(size:8)theData.

    #constructor new &long:anOperand 
        <long_copy>.

    #constructor new
        => &new &long:0l.

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
    
    #method cast : anObject = anObject long.
}

// --- realNumber ---

#class(sealed,type:real,dbg:real) RealNumber : BaseNumber
{
    #field(size:8)theData.

    #constructor new
        => &new &real:0.0r.

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

    #method literal = literalConvertor convert &real:$self literal.

    #method cast : anObject = anObject real.

    #method clone = RealNumber new &real:$self.
}

// --- CharValue ---

#class(type:char,dbg:int) CharValue : BaseValue
{
    #field(type:short)theValue.
    
    #constructor new
    [
        theValue := 0 short.
    ]
    
//    #constructor new : aValue
//        => &new::aValue.
    
    #constructor new &short:aCode
    [
        theValue := aCode.
    ]

    #constructor new &char:aCode
        => &new &short::aCode.

//    #method equal &int:anOperand 
//        = theData == anOperand.
//
//    #method less &int:anOperand 
//        = theData < anOperand.

//    #method equal &char:anOperand 
//        = $self equal &int:anOperand .
//
//    #method less &char:anOperand 
//        = $self less &int:anOperand .
//
//    #method equal &literal:aLiteral
//        = aLiteral equal &char:$self.

    #method char = $self.

    #method short = ShortNumber new &short:theValue.

    #method int = IntNumber new &short:theValue.

    #method literal = system'LiteralValue new &length:1 &short:theValue literal.
    
    #method clone = CharValue new &short:theValue.
}

// --- emptyliteralvalue ---

#symbol emptyLiteralValue = "".

// --- literalvalue ---

#class LiteralIterator
{
    #field theLiteral.
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
        theLiteral getAt &index:anIndex &out'short:aChar.
        
        ^ CharValue new &short:aChar.
    ]
}

#class(type:literal,sealed,dbg:literal) LiteralValue : BaseValue
{
    #field(itemsize:2)theData.

    #constructor => <ws_create>.

    #constructor new &length:aSize 
    [
    ]

    #constructor new &length:aSize &literal:aLiteral
        <ws_copy>.

    #constructor new &length:aSize &shortarray:anArray
        <ws_copyarr>.

    #constructor new &length:aSize &short:aChar
        <ws_fill>.

    #constructor new &literal:aLiteral
        => &new &length::aLiteral &literal:aLiteral.

    #method literal = $self.

    #method equal &literal : aLiteral
        <ws_equal>.

//    #method equal &char : aChar
//        <ws_equalchar>.

    #method less &literal : aLiteral
        <ws_less>.

    #method add : aLiteral
        = $self add::aLiteral.

    #method add &literal : aLiteral
        <ws_add>.

//    #method add &char : aChar
//        = literalPrimitiveOp concat &literal:$self &literal::aChar.

    #method getAt : aParam
    [
        #var(type:short)aChar.
        $self getAt &index:(aParam int) &out'short:aChar.
        
        ^ CharValue new &short:aChar.
    ]

    #method getAt &index:anIndex &out'short:aChar
        <ws_getat>.

    #method indexOf &index:anIndex &literal:aLiteral &out'int:aRetVal
        <ws_indexof>.

    #method indexOf &index:anIndex &literal:aLiteral
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &literal:aLiteral &out'int:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
    
//    #method indexOf &index:anIndex &char:aChar
//        => &indexOf &index:anIndex &literal::aChar.

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
    
    #method clone = LiteralValue new &literal:$self.
    
    #method cast : anObject = anObject literal.
}

// === Basic Variable Classes ===

// --- Integer ---

#class(dbg:int,sealed) Integer : IntBaseNumber
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
    
    #method read &out'int:aRetVal
    [
        aRetVal := theValue.
    ]
    
    #method write : anOperand
    [
        $self write &int::anOperand.
    ]
    
    #method append : anOperand
    [
        $self append &int::anOperand.
    ]
    
    #method reduce : anOperand
    [
        $self reduce &int::anOperand.
    ]
    
    #method multiplyBy : anOperand
    [
        $self multiplyBy &int::anOperand.
    ]
    
    #method divideInto : anOperand
    [
        $self divideInto &int::anOperand.
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
    
    #method cast : anObject = anObject int.

    #method literal = system'literalConvertor convert &int:theValue literal.

    #method clone = Integer new &int:theValue.
}

// --- Long ---

#class(dbg:long,sealed) Long : IntBaseNumber
{
    #field(type:long)theValue.
    
    #constructor new
    [
        theValue := 0l.
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
    
    #method write : anOperand
    [
        $self write &long::anOperand.
    ]
    
    #method append : anOperand
    [
        $self append &long::anOperand.
    ]
    
    #method reduce : anOperand
    [
        $self reduce &long::anOperand.
    ]
    
    #method multiplyBy : anOperand
    [
        $self multiplyBy &long::anOperand.
    ]
    
    #method divideInto : anOperand
    [
        $self divideInto &long::anOperand.
    ]
    
    #method write &long:anOperand
    [
        theValue := anOperand.
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
    
    #method cast : anObject = anObject long.

    #method literal = system'literalConvertor convert &long:theValue literal.

    #method clone = Long new &long:theValue.
}

// --- Real ---

#class(dbg:real,sealed) Real : BaseNumber
{
    #field(type:real)theValue.
    
    #constructor new
    [
        theValue := 0.0r.
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
    
    #method write : anOperand
    [
        $self write &real::anOperand.
    ]
    
    #method append : anOperand
    [
        $self append &real::anOperand.
    ]
    
    #method reduce : anOperand
    [
        $self reduce &real::anOperand.
    ]
    
    #method multiplyBy : anOperand
    [
        $self multiplyBy &real::anOperand.
    ]
    
    #method divideInto : anOperand
    [
        $self divideInto &real::anOperand.
    ]
    
    #method write &real:anOperand
    [
        theValue := anOperand.
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
        = theValue + anOperand.
    
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
    
    #method cast : anObject = anObject real.

    #method literal = system'literalConvertor convert &real:theValue literal.

    #method clone = Real new &real:theValue.
}

// --- String ---

#class String : BaseValue
{
    #field theValue.

    #constructor new
    [
        theValue := emptyLiteralValue.
    ]
            
    #constructor new : aValue
    [
        theValue := aValue literal.
    ]
    
//    #method write : anOperand
//    [
//        theValue := anOperand literal.
//    ]
//    
//    #method write &literal:aLiteral
//    [
//        theValue := aLiteral.
//    ]
//    
//    #method append : anOperand
//    [
//        theValue := theValue + anOperand.
//    ]
//    
//    #method append &literal:anOperand
//    [
//        theValue := theValue add &literal:anOperand.
//    ]
//    
//    #method insert &index:anIndex &literal:aLiteral
//    [
//        theValue := theValue insert &index:anIndex &literal:aLiteral.
//    ]
//            
//    #method delete &index:anIndex &length:aLength
//    [
//        theValue := theValue delete &index:anIndex &length:aLength.
//    ]
//                        
    #method clear
    [
        theValue := emptyLiteralValue.
    ]
    
    #method clone = String new:theValue.
                
    #method cast : anObject = anObject literal.
}

// --- Boolean ---

#class Boolean : BaseValue
{
    #field theValue.

    #constructor new : aValue
    [        
        theValue := true == aValue.
    ]

//    #method value = theValue.

    #method write : aValue
    [
        theValue := true == aValue.
    ]

    #method bool = theValue bool.

//    #method append : aValue
//    [
//        theValue := theValue or:aValue.
//    ]
//            
//    #method multiplyBy : aValue
//    [
//        theValue := theValue and:aValue.
//    ]
//            
//    #method invert
//    [
//        theValue := theValue not.
//    ]            

    #method equal &bool:aValue
        => theValue.
            
    #method clone = Boolean new:($self value).
            
//    #method => theValue.

    #method cast : anObject = anObject bool.
}

// --- ArrayIterator ---

#class ArrayIterator
{
    #field theArray.
    #field theIndex.
    #field theCount.
    
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
    ]

    #method set &index:anIndex
    [
        theIndex write &int:anIndex.
    ]
    
    #method append &index:anIndex
    [
        theIndex append &int:anIndex.
    ]

    #method available = theIndex less &int::theCount.
  
    #method next
    [
        theIndex append &int:1.

        ^ theIndex less &int::theCount.
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

// --- Array ---

#class(sealed,type:array,dbg:array) Array
{
    #field(item)theItem.

    #constructor => <arr_create>.
    
    #constructor new &length:aLength
    [
    ]

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

//    #method add : anObject
//        = $self add::anObject.
//        
//    #method add &array:anArray
//    [
//        #var(type:int)aLength1 := $self length.
//        #var(type:int)aLength2 := anArray length.
//        
//        #var aRetVal := Array new &length:(aLength1 + aLength2).
//        
//        control forrange &int:0 &int:(aLength1-1) &do: (&int:i)
//            [ aRetVal setAt &index:i &object:(self getAt &index:i). ].
//            
//        control forrange &int:0 &int:(aLength2-1) &do: (&int:j)
//            [ aRetVal setAt &index:(aLength1 + j) &object:(anArray getAt &index:j). ].
//            
//        ^ aRetVal.
//    ]

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

//    #method clone 
//    [
//        #var(type:int)aLength := $self length.
//        
//        #var aRetVal := Array new &length:aLength.
//        
//        control forrange &int:0 &int:(aLength-1) &do: (&int:i)
//            [ aRetVal setAt &index:i &object:(self getAt &index:i). ].        
//            
//        ^ aRetVal.
//    ]
}

// --- ArgArray ---

#class(sealed,type:params) ArgArray
{
    #field(item)theItem.
    
    #constructor => <args_create>.
    
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
    
//    #method convert &int:anInt
//        = anInt.
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
//    
//    #method convert &long:aLong
//        = aLong.
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

//    #method convert &real:aReal
//        = aReal.
//        
//    #method convert &int:anInt
//        = RealNumber new &int:anInt.
//        
//    #method convert &long:aLong
//        = RealNumber new &long:aLong.
}

#class(role) literalConvertor
{
    #method convert &int:aNumber
        <ws_copyint>.

//    #method convert &int:aNumber &base:aRadix
//        = literalPrimitiveOp toLiteral &int:aNumber &index::aRadix.
//        
//    #method convert &long:aNumber &base:aRadix
//        = literalPrimitiveOp toLiteral &long:aNumber &index::aRadix.

    #method convert &long:aNumber
        <ws_copylong>.

    #method convert &real:aNumber
        <ws_copyreal>.
        
//    #method convert &object:anObject
//        = anObject literal.
//        
//    #method convert &literal:aLiteral
//        = aLiteral.
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

//    #method long = longConvertor convert &literal:theValue.
//    
//    #method real = realConvertor convert &literal:theValue.
//    
//    #method literal = theValue.
//    
//    #method <= &variant:$self.
}

// === Basic Factory symbols ==

//#class Variable
//{
//    #field theValue.
//    
//    #constructor new : aValue
//    [
//        theValue := aValue.
//    ]
//  
//    #method write : aValue
//    [
//        theValue := aValue.
//    ]
//        
//    #method get = theValue.        
//        
//    #method append : aValue
//    [
//        theValue := theValue + aValue.
//    ]
//    
//    #method reduce : aValue
//    [
//        theValue := theValue - aValue.
//    ]
//
//    #method multiplyBy : aValue
//    [
//        theValue := theValue * aValue.
//    ]
//    
//    #method divideInto : aValue
//    [
//        theValue := theValue / aValue.
//    ]
//        
//    #method => theValue.
//}
