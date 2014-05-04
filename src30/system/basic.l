// === basic data classes ===

#define LongNumber = system'LongNumber.
#define RealNumber = system'RealNumber.

// --- BaseValue ---

#class BaseValue
{
    #method equal : anObject = $self equal::anObject.
    
    #method greater : anObject = anObject less:$self.
    
    #method less : anObject = $self less::anObject.
    
    #method notless : anObject = $self notless::anObject.
    
    #method notgreater : anObject = anObject less:$self not.
}

// --- BaseNumber ---

#class BaseNumber : BaseValue
{
    #method add : anOperand = $self add::anOperand.
    
    #method subtract : anOperand = $self subtract::anOperand.
    
    #method multiply : anOperand = $self multiply::anOperand.
    
    #method divide : anOperand = $self divide::anOperand.
}

// --- IntBaseNumber ---

#class IntBaseNumber : BaseNumber
{
    #method and : anOperand = $self and::anOperand.
    
    #method or : anOperand = $self or::anOperand.
    
    #method xor : anOperand = $self xor::anOperand.
    
    #method allMask : anOperand
        = $self and:anOperand == anOperand.
        
    #method anyMask : anOperand
        = 0 != $self and:anOperand.
}

// --- IntNumber ---

#class IntNumber : IntBaseNumber
{
    #field(type:int)theData.

    #constructor new &int:anOperand
    [
        theData << anOperand.
    ]

    #constructor new
    [
        theData << 0.
    ]

    #method int = $self.

    #method index = $self.

    #method equal &int:anOperand 
        = theData == anOperand.

    #method less &int:anOperand
        = theData < anOperand.

    #method notless &int:anOperand
        = theData >= anOperand.

    #method add &int:anOperand
        = IntNumber new &int:(theData + anOperand).

    #method subtract &int:anOperand
        = IntNumber new &int:(theData - anOperand).

    #method multiply &int:anOperand
        = IntNumber new &int:(theData * anOperand).

    #method divide &int:anOperand
        = IntNumber new &int:(theData / anOperand).

    #method and &int:anOperand
        = IntNumber new &int:(theData ^& anOperand).

    #method or &int:anOperand
        = IntNumber new &int:(theData ^| anOperand).

    #method xor &int:anOperand
        = IntNumber new &int:(theData ^^ anOperand).

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

    #method shift : anOrder
    [
        #var(type:int)retVal.
        primitive shift &int:$self &int::anOrder &out'int:retVal.
        
        ^ IntNumber new &int:retVal.
    ]

    #method literal = primitive toLiteral &int:$self.

    #method negative = 0 - $self.

    #method not
    [
        #var(type:int)retVal.
        primitive invert &int:$self &out'int:retVal.
        
        ^ IntNumber new &int:retVal.
    ]

    #method <= &int:$self.
}

// --- longnumber ---

#class LongNumber : IntBaseNumber
{
    #field(type:long)theData.

    #constructor new
    [
        theData << 0l.
    ]

    #constructor new &long:anOperand 
    [
        theData << anOperand.
    ]

    #constructor new &int:anOperand 
    [
        theData << anOperand.
    ]
  
    #method long = $self.

    #method equal &long:anOperand
        = theData == anOperand.

    #method less &long:anOperand
        = theData < anOperand.

    #method notless &long:anOperand
        = theData >= anOperand.

    #method add &long:anOperand
        = LongNumber new &long:(theData + anOperand).

    #method subtract &long:anOperand
        = LongNumber new &long:(theData - anOperand).

    #method multiply &long:anOperand
        = LongNumber new &long:(theData * anOperand).

    #method divide &long:anOperand
        = LongNumber new &long:(theData / anOperand).

    #method and &long:anOperand
        = LongNumber new &long:(theData ^& anOperand).

    #method or &long:anOperand
        = LongNumber new &long:(theData ^| anOperand).

    #method xor &long:anOperand
        = LongNumber new &long:(theData ^^ anOperand).

    #method add &int:anOperand
        = $self add &long:(LongNumber new &int:anOperand).

    #method subtract &int:anOperand
        = $self subtract &long:(LongNumber new &int:anOperand).

    #method multiply &int:anOperand
        = $self multiply &long:(LongNumber new &int:anOperand).

    #method divide &int:anOperand
        = $self divide &long:(LongNumber new &int:anOperand).

    #method equal &int:anOperand 
        = $self equal &long:(LongNumber new &int:anOperand).

    #method less &int:anOperand 
        = $self less &long:(LongNumber new &int:anOperand).

    #method add &real:anOperand
        = RealNumber new &long:$self add &real:anOperand.

    #method subtract &real:anOperand
        = RealNumber new &long:$self subtract &real:anOperand.

    #method multiply &real:anOperand
        = RealNumber new &long:$self multiply &real:anOperand.

    #method divide &real:anOperand
        = RealNumber new &long:$self divide &real:anOperand.

    #method shift : anOrder
    [
        #var(type:long)aRetVal.
        primitive shift &long:$self &int::anOrder &out'long:aRetVal.
        
        ^ LongNumber new &long:aRetVal.
    ]

    #method literal = primitive toLiteral &long:$self.

    #method negative = 0l - $self.

    #method not
    [
        #var(type:long)retVal.
        primitive invert &long:$self &out'long:retVal.
        
        ^ LongNumber new &type'long:retVal.
    ]
            
    #method <= &long:$self.
}

// --- realNumber ---

#class RealNumber : BaseNumber
{
    #field(type:real)theData.

    #constructor new
    [
        theData << 0.0r.
    ]

    #constructor new &real:anOperand 
    [
        theData << anOperand.
    ]
  
    #constructor new &int:anOperand 
    [
        theData << anOperand.
    ]
        
    #constructor new &long:anOperand 
    [
        theData << anOperand.
    ]
        
    #method real = $self.

    #method equal &real:anOperand
        = theData == anOperand.

    #method less &real:anOperand
        = theData < anOperand.

    #method notless &real:anOperand
        = theData >= anOperand.

    #method add &real:anOperand
        = RealNumber new &real:(theData + anOperand).

    #method subtract &real:anOperand
        = RealNumber new &real:(theData - anOperand).

    #method multiply &real:anOperand
        = RealNumber new &real:(theData * anOperand).

    #method divide &real:anOperand
        = RealNumber new &real:(theData / anOperand).

    #method add &int:anOperand
        = $self add &real:(RealNumber new &int:anOperand).

    #method subtract &int:anOperand
        = $self subtract &real:(RealNumber new &int:anOperand).

    #method multiply &int:anOperand
        = $self multiply &real:(RealNumber new &int:anOperand).

    #method divide &int:anOperand
        = $self divide &real:(RealNumber new &int:anOperand).

    #method literal = primitive toLiteral &real:$self.
    
    #method <= &real:$self.
}

// --- CharValue ---

#class CharValue : BaseValue
{
    #field(type:int)theData.
    
    #constructor new
    [
        theData << 0.
    ]
    
    #constructor new : aValue
        => &new::aValue.
    
    #constructor new &short:aCode
    [
        theData << aCode.
    ]

    #constructor new &int:aCode
    [
        theData << aCode.
    ]

    #constructor new &char:aCode
        => &new &int:aCode.

    #method equal &int:anOperand 
        = theData == anOperand.

    #method less &int:anOperand 
        = theData < anOperand.

    #method notless &int:anOperand 
        = theData >= anOperand.

    #method equal &char:anOperand 
        = $self equal &int:anOperand .

    #method less &char:anOperand 
        = $self less &int:anOperand .

    #method equal &literal:aLiteral
        = aLiteral equal &char:$self.

    #method char = $self.

    #method int = IntNumber new &int:theData.

    #method literal = system'LiteralValue new &length:1 &char:$self.

    #method <= &char:$self.
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

    #method index = theIndex number.

    #method reset
    [
        theIndex write &int:-1.
    ]

    #method append &index:anIndex
    [
        theIndex append::anIndex.
    ]

    #method set &index:anIndex
    [
        theIndex write::anIndex.
    ]

    #method available = theIndex less &int:(theLiteral length).
    
    #method next
    [
        theIndex append &int:1.

        ^ theIndex less &int:(theLiteral length).
    ]
    
    #method get
    [
        #var(type:short)aChar.
        theLiteral getAt &index:theIndex &out'short:aChar.
        
        ^ CharValue new &int:aChar.
    ]
}

#class LiteralValue : BaseValue
{
    #field(type:literal)theData.

    #constructor new &length:aSize 
    [
    ]

    #constructor new &length:aSize &literal:aLiteral
    [
        theData << aLiteral.
    ]

    #constructor new &length:aSize &char:aChar
        <ws_fill>.

    #constructor new &literal:aLiteral
        => &new &length::aLiteral &literal:aLiteral.

    #method literal = $self.

    #method equal &literal : aLiteral
        = theData == aLiteral.

    #method equal &char : aChar
        <ws_equalchar>.

    #method less &literal : aLiteral
        = theData < aLiteral.

    #method notless &literal : aLiteral
        = theData >= aLiteral.

    #method add : aLiteral
        = $self add::aLiteral.

    #method add &literal : aLiteral
        = primitive concat &literal:$self &literal:aLiteral.

    #method add &char : aChar
        = primitive concat &literal:$self &literal::aChar.

    #method getAt : aParam
    [
        #var(type:short)aChar.
        $self getAt &index::aParam &out'short:aChar.
        
        ^ CharValue new &int:aChar.
    ]

    #method getAt &index:anIndex &out'short:aChar
        <ws_getat>.

    #method indexOf &index:anIndex &literal:aLiteral &out'int:aRetVal
        <ws_indexof>.

    #method indexOf &index:anIndex &literal:aLiteral
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &literal:aLiteral &out'int:aRetVal.
        
        ^ aRetVal.
    ]
    
    #method indexOf &index:anIndex &char:aChar
        => &indexOf &index:anIndex &literal::aChar.
    
    #method insert &index:anIndex &literal:aLiteral
        = primitive insert &literal:$self &index:anIndex &literal:aLiteral.

    #method delete &index:anIndex &length:aLength
        = primitive delete &literal:$self &index:anIndex &length:aLength.

    #method Substring &index:anIndex &length:aLength
        = primitive Substring &literal:$self &index:anIndex &length:aLength.

    #method length
    [
        #var(type:int)aLength := theData length.
        
        ^ IntNumber new &int:aLength.
    ]

    #method enumerator = LiteralIterator new &literal:$self.

    #method <= &literal:$self.
}

// === Basic Variable Classes ===

// --- Integer ---

#class Integer : IntNumber
{
    #constructor new : aValue
    [
        $self write::aValue.
    ]
    
    #method number = IntNumber new &int:$self.

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
        theData << anOperand.
    ]
    
    #method append &int:anOperand    
    [
        theData += anOperand.
    ]
        
    #method reduce &int:anOperand
    [
        theData -= anOperand.
    ]
        
    #method multiplyBy &int:anOperand
    [
        theData *= anOperand.
    ]        
        
    #method divideInto &int:anOperand
    [
        theData /= anOperand.
    ]
}

// --- Long ---

#class Long : LongNumber
{
    #method number = LongNumber new &long:$self.

    #method write : anOperand
    [
        $self write::anOperand.
    ]

    #method append : anOperand
    [
        $self append::anOperand.
    ]
    
    #method reduce : anOperand
    [
        $self reduce::anOperand.
    ]
    
    #method multiplyBy : anOperand
    [
        $self multiplyBy::anOperand.
    ]
    
    #method divideInto : anOperand
    [
        $self divideInto::anOperand.
    ]
        
    #method write &long:anOperand
    [
        theData << anOperand.
    ]
    
    #method append &long:anOperand
    [
        theData += anOperand.
    ]
        
    #method reduce &long:anOperand
    [
        theData -= anOperand.
    ]
        
    #method multiplyBy &long:anOperand
    [
        theData *= anOperand.
    ]        
        
    #method divideInto &long:anOperand
    [
        theData /= anOperand.
    ]
    
    #method write &int:anOperand
    [
        theData << anOperand.
    ]
    
    #method append &int:anOperand
        => &append &long:(LongNumber new &int:anOperand).
        
    #method reduce &int:anOperand
        => &reduce &long:(LongNumber new &int:anOperand).
        
    #method multiplyBy &int:anOperand
        => &multiplyBy &long:(LongNumber new &int:anOperand).
        
    #method divideInto &int:anOperand
        => &divideInto &long:(LongNumber new &int:anOperand).
    
    #method write &variant:anOperand
        => &write &long::anOperand.
}

// --- Real ---

#class Real : RealNumber
{
    #constructor new : aValue
    [
        $self << aValue.
    ]
    
    #method number = RealNumber new &real:$self.

    #method write : anOperand
    [
        $self write::anOperand.
    ]
    
    #method append : anOperand
    [
        $self append::anOperand.
    ]
    
    #method reduce : anOperand
    [
        $self reduce::anOperand.
    ]
    
    #method multiplyBy : anOperand
    [
        $self multiplyBy::anOperand.
    ]
    
    #method divideInto : anOperand
    [
        $self divideInto::anOperand.
    ]
    
    #method write &real:anOperand
    [
        theData << anOperand.
    ]
    
    #method write &int:anOperand
    [
        theData << anOperand.
    ]
    
    #method write &variant:anOperand
        => &write &real::anOperand.
    
    #method append &real:anOperand
    [
        theData += anOperand.
    ]
        
    #method append &int:anOperand
    [
        theData += anOperand.
    ]
        
    #method reduce &real:anOperand
    [
        theData -= anOperand.
    ]
        
    #method reduce &int:anOperand
    [
        theData -= anOperand.
    ]
        
    #method multiplyBy &real:anOperand
    [
        theData *= anOperand.
    ]        
        
    #method multiplyBy &int:anOperand
    [
        theData *= anOperand.
    ]        
        
    #method divideInto &real:anOperand
    [
        theData /= anOperand.
    ]
        
    #method divideInto &int:anOperand
    [
        theData /= anOperand.
    ]
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
        theValue := theValue insert &index:anIndex &literal:aLiteral.
    ]
            
    #method delete &index:anIndex &length:aLength
    [
        theValue := theValue delete &index:anIndex &length:aLength.
    ]
                        
    #method clear
    [
        theValue := emptyLiteralValue.
    ]
            
    #method <= &literal:theValue.
    
    #method => theValue.
}

// --- Boolean ---

#class Boolean
{
    #field theValue.
    
    #constructor new : aValue
    [        
        aValue ? [ theValue := true. ] ! [ theValue := false. ].
    ]

    #method value = theValue.

    #method write : aValue
    [
        aValue ? [ theValue := true. ] ! [ theValue := false. ].
    ]
            
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

    #method equal : aValue
        => theValue.
            
    #method => theValue.
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
    
    #method index = theIndex number.

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

    #method available = theIndex less &int:theCount.
  
    #method next
    [
        theIndex append &int:1.

        ^ theIndex less &int:theCount.
    ]

    #method get = theArray getAt &index:theIndex.

    #method set : anObject
    [
        theArray setAt &index:theIndex &object:anObject.
    ]
}

// --- Array ---

#class Array
{
    #field(type:array)theItems.
    
    #constructor new &length:aLength
    [
    ]

    #method getAt : anIndex
        = $self getAt &index::anIndex.

    #method getAt &index:anIndex
        <array_getat>.

    #method setAt:anIndex:anObject
    [
        $self setAt &index::anIndex &object:anObject.
    ]
    
    #method setAt &index:anIndex &object:anObject
        <array_setat>.

    #method add : anObject
        = $self add::anObject.
        
    #method add &array:anArray
    [
        #var(type:int)aLength1 := $self length.
        #var(type:int)aLength2 := anArray length.
        
        #var aRetVal := Array new &length:(aLength1 + aLength2).
        
        control forrange &int:0 &int:(aLength1-1) &do: (&int:i)
            [ aRetVal setAt &index:i &object:(self getAt &index:i). ].
            
        control forrange &int:0 &int:(aLength2-1) &do: (&int:j)
            [ aRetVal setAt &index:(aLength1 + j) &object:(anArray getAt &index:j). ].
            
        ^ aRetVal.
    ]

    #method array = $self.

    #method length 
    [
        #var(type:int)aLen.
        primitive readLength &array:$self &out'int:aLen.
        
        ^ aLen.
    ]

    #method enumerator = ArrayIterator new &array:$self.

    #method <= &array:$self.
}

// === Basic Convertors ===

// --- convertor helpers ---

#class(role) intConvertor
{
    #method convert &literal:aLiteral
    [
        #var(type:int)aRetVal.
        primitive convert &literal:aLiteral &out'int:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
    
    #method convert &int:anInt
        = anInt.
}

#class(role) longConvertor
{
    #method convert &literal:aLiteral
    [
        #var(type:long)aRetVal.
        primitive convert &literal:aLiteral &out'long:aRetVal.
        
        ^ LongNumber new &long:aRetVal.        
    ]
    
    #method convert &long:aLong
        = aLong.
}

#class(role) realConvertor
{
    #method convert &literal:aLiteral
    [
        #var(type:real)aRetVal.
        primitive convert &literal:aLiteral &out'real:aRetVal.
        
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
        = aNumber literal.
        
    #method convert &int:aNumber &base:aRadix
        = primitive toLiteral &int:aNumber &index::aRadix.
        
    #method convert &long:aNumber &base:aRadix
        = primitive toLiteral &long:aNumber &index::aRadix.
        
    #method convert &long:aNumber
        = aNumber literal.
        
    #method convert &real:aNumber
        = aNumber literal.
        
    #method convert &object:anObject
        = anObject literal.
        
    #method convert &literal:aLiteral
        = aLiteral.
}

// --- Variant ---

#class Variant : String
{
    #method int = intConvertor convert &literal:theValue.
    
    #method long = longConvertor convert &literal:theValue.
    
    #method real = realConvertor convert &literal:theValue.
    
    #method literal = theValue.
    
    #method <= &variant:$self.
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
        
    #method get = theValue.        
        
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
