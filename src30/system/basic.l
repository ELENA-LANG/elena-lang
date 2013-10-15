// === basic data classes ===

#define LongNumber = system'LongNumber.
#define RealNumber = system'RealNumber.

// --- BaseValue ---

#class BaseValue
{
    #method equal : anObject = $self equal::anObject.
    
    #method notequal : anObject = $self equal::anObject ? [ false ] ! [ true ].
    
    #method less : anObject = $self less::anObject.
    
    #method greater : anObject = anObject less::$self.
    
    #method notless : anObject = $self notless::anObject.
    
    #method notgreater : anObject = anObject notless::$self.
}

// --- BaseNumber ---

#class BaseNumber : BaseValue
{
    #method add : anOperand = $self add::anOperand.
    
    #method subtract : anOperand = $self subtract::anOperand.
    
    #method multiply : anOperand = $self multiply::anOperand.
    
    #method divide : anOperand = $self divide::anOperand.
}

// --- intnumber ---

#class IntNumber : BaseNumber
{
    #field(type:int)theData.

    #constructor new &type'int:anOperand
    [
        theData << anOperand.
    ]

    #constructor new
    [
        theData << 0.
    ]

    #method type'int = $self.

    #method type'index = $self.

    #method equal &type'int:anOperand 
        = theData == anOperand.

    #method less &type'int:anOperand
        = theData < anOperand.

    #method notless &type'int:anOperand
        = theData >= anOperand.

    #method add &type'int:anOperand
        = IntNumber new &type'int:(theData + anOperand).

    #method subtract &type'int:anOperand
        = IntNumber new &type'int:(theData - anOperand).

    #method multiply &type'int:anOperand
        = IntNumber new &type'int:(theData * anOperand).

    #method divide &type'int:anOperand
        = IntNumber new &type'int:(theData / anOperand).

    #method and &type'int:anOperand
        = IntNumber new &type'int:(theData ^& anOperand).

    #method or &type'int:anOperand
        = IntNumber new &type'int:(theData ^| anOperand).

    #method xor &type'int:anOperand
        = IntNumber new &type'int:(theData ^^ anOperand).

    #method equal &type'long:anOperand 
        = LongNumber new &type'int:$self equal &type'int:anOperand.

    #method less &type'long:anOperand 
        = LongNumber new &type'int:$self less &type'int:anOperand.

    #method add &type'long:anOperand
        = LongNumber new &type'int:$self add &type'long:anOperand.

    #method subtract &type'long:anOperand
        = LongNumber new &type'int:$self subtract &type'long:anOperand.

    #method multiply &type'long:anOperand
        = LongNumber new &type'int:$self multiply &type'long:anOperand.

    #method divide &type'long:anOperand
        = LongNumber new &type'int:$self divide &type'long:anOperand.

    #method add &type'real:anOperand
        = RealNumber new &type'int:$self add &type'real:anOperand.

    #method subtract &type'real:anOperand
        = RealNumber new &type'int:$self subtract &type'real:anOperand.

    #method multiply &type'real:anOperand
        = RealNumber new &type'int:$self multiply &type'real:anOperand.

    #method divide &type'real:anOperand
        = RealNumber new &type'int:$self divide &type'real:anOperand.

    #method Literal = system'external'CoreAPI convert &out'type'widestr:(system'LiteralValue new &type'length:10) &type'int:$self.

    #method Negative = 0 - $self.
    
    #method <= &&type'int:$self.
}

// --- longnumber ---

#class LongNumber : BaseNumber
{
    #field(type:long)theData.

    #constructor new
    [
        theData << 0l.
    ]

    #constructor new &type'long:anOperand 
    [
        theData << anOperand.
    ]

    #constructor new &type'int:anOperand 
    [
        theData << anOperand.
    ]
  
    #method type'long = $self.

    #method equal &type'long:anOperand
        = theData == anOperand.

    #method less &type'long:anOperand
        = theData < anOperand.

    #method notless &type'long:anOperand
        = theData >= anOperand.

    #method add &type'long:anOperand
        = LongNumber new &type'long:(theData + anOperand).

    #method subtract &type'long:anOperand
        = LongNumber new &type'long:(theData - anOperand).

    #method multiply &type'long:anOperand
        = LongNumber new &type'long:(theData * anOperand).

    #method divide &type'long:anOperand
        = LongNumber new &type'long:(theData / anOperand).

    #method and &type'long:anOperand
        = LongNumber new &type'long:(theData ^& anOperand).

    #method or &type'long:anOperand
        = LongNumber new &type'long:(theData ^| anOperand).

    #method xor &type'long:anOperand
        = LongNumber new &type'long:(theData ^^ anOperand).

    #method add &type'int:anOperand
        = $self add &type'long:(LongNumber new &type'int:anOperand).

    #method subtract &type'int:anOperand
        = $self subtract &type'long:(LongNumber new &type'int:anOperand).

    #method multiply &type'int:anOperand
        = $self multiply &type'long:(LongNumber new &type'int:anOperand).

    #method divide &type'int:anOperand
        = $self divide &type'long:(LongNumber new &type'int:anOperand).

    #method equal &type'int:anOperand 
        = $self equal &type'long:(LongNumber new &type'int:anOperand).

    #method less &type'int:anOperand 
        = $self less &type'long:(LongNumber new &type'int:anOperand).

    #method add &type'real:anOperand
        = RealNumber new &type'long:$self add &type'real:anOperand.

    #method subtract &type'real:anOperand
        = RealNumber new &type'long:$self subtract &type'real:anOperand.

    #method multiply &type'real:anOperand
        = RealNumber new &type'long:$self multiply &type'real:anOperand.

    #method divide &type'real:anOperand
        = RealNumber new &type'long:$self divide &type'real:anOperand.

    #method Literal = system'external'CoreAPI convert &out'type'widestr:(system'LiteralValue new &type'length:20) &type'long: theData.

    #method Negative = 0l - $self.

    #method <= &&type'long:$self.
}

// --- realNumber ---

#class RealNumber : BaseNumber
{
    #field(type:real)theData.

    #constructor new
    [
        theData << 0.0r.
    ]

    #constructor new &type'real:anOperand 
    [
        theData << anOperand.
    ]
  
    #constructor new &type'int:anOperand 
    [
        theData << anOperand.
    ]
        
    #constructor new &type'long:anOperand 
    [
        theData << anOperand.
    ]
        
    #method type'real = $self.

    #method equal &type'real:anOperand
        = theData == anOperand.

    #method less &type'real:anOperand
        = theData < anOperand.

    #method notless &type'real:anOperand
        = theData >= anOperand.

    #method add &type'real:anOperand
        = RealNumber new &type'real:(theData + anOperand).

    #method subtract &type'real:anOperand
        = RealNumber new &type'real:(theData - anOperand).

    #method multiply &type'real:anOperand
        = RealNumber new &type'real:(theData * anOperand).

    #method divide &type'real:anOperand
        = RealNumber new &type'real:(theData / anOperand).

    #method add &type'int:anOperand
        = $self add &type'real:(RealNumber new &type'int:anOperand).

    #method subtract &type'int:anOperand
        = $self subtract &type'real:(RealNumber new &type'int:anOperand).

    #method multiply &type'int:anOperand
        = $self multiply &type'real:(RealNumber new &type'int:anOperand).

    #method divide &type'int:anOperand
        = $self divide &type'real:(RealNumber new &type'int:anOperand).

    #method Literal = system'external'CoreAPI convert &out'type'widestr:(system'LiteralValue new &type'length:30) &type'real:theData &type'int:24.
    
    #method <= &&type'real:$self.
}

// --- CharValue ---

#class CharValue : BaseValue
{
    #field(type:int)theData.
    
    #constructor new
    [
        theData << 0.
    ]
    
    #constructor new &type'int:aCode
    [
        theData << aCode.
    ]

    #constructor new &type'short:aCode
    [
        theData << aCode.
    ]

    #method type'short = $self.

    #method equal &type'int:anOperand 
        = theData == anOperand.

    #method less &type'int:anOperand 
        = theData < anOperand.

    #method notless &type'int:anOperand 
        = theData >= anOperand.

    #method equal &type'short:anOperand 
        = $self equal &type'int:anOperand .

    #method less &type'short:anOperand 
        = $self less &type'int:anOperand .

    #method Number = IntNumber new &type'int:theData.

    #method Literal = system'external'CoreAPI convert &out'type'widestr:(system'LiteralValue new &type'length:1) &type'short:theData.

    #method type'widestr => %Literal. 

    #method <= &&type'short:$self.
}

// --- emptyliteralvalue ---

#symbol emptyLiteralValue =
{
    type'widestr = "".
   
    equal &type'widestr : aLiteral
        = 0 equal &type'int:(aLiteral type'length).
        
    less &type'widestr : aLiteral
        = 0 less &type'int:(aLiteral type'length).

    notless &type'widestr : aLiteral
        = 0 notless &type'int:(aLiteral type'length).

    add : anObject = anObject type'widestr.
                        
    add &type'widestr : aLiteral
        = aLiteral.

    Literal = $self type'widestr.
   
    Length = 0.
   
    <= &&type'widestr:($self type'widestr).
}.

// --- literalvalue ---

#class LiteralIterator
{
    #field theLiteral.
    #field theIndex.
    
    #constructor new &type'widestr:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := system'Integer new &type'int:(-1).
    ]
    
    #method Index = theIndex Number.

    #method append &Index:anIndex
    [
        theIndex append::anIndex.
    ]

    #method write &Index:anIndex
    [
        theIndex write::anIndex.
    ]

    #method EOF = theIndex less &type'int:(theLiteral type'length) ? [ false ] ! [ true ].
    
    #method next
    [
        theIndex append &type'int:1.

        ^ theIndex less &type'int:(theLiteral type'length).
    ]
    
    #method get = theLiteral getAt &type'index:theIndex.
}

#class LiteralValue : BaseValue
{
    #field(type:literal)theData.

    #constructor new &type'length:aSize 
    [
    ]

    #constructor new &type'length:aSize &type'widestr:aLiteral
    [
        theData << aLiteral.
    ]

    #constructor new &type'widestr:aLiteral
        <= new &type'length::aLiteral &type'widestr:aLiteral.

    #constructor new &type'length:aSize &type'widestr:aLiteral1 &type'widestr:aLiteral2
    [
        theData << aLiteral1.
        theData += aLiteral2.
    ]

    #method type'widestr = $self.

    #method equal &type'widestr : aLiteral
        = theData == aLiteral.

    #method equal &type'short : aChar
        = theData == aChar.

    #method less &type'widestr : aLiteral
        = theData < aLiteral.

    #method notless &type'widestr : aLiteral
        = theData >= aLiteral.

    #method add : aLiteral
        = $self add::aLiteral.

    #method add &type'widestr : aLiteral
        = LiteralValue new &type'length:(theData type'length + aLiteral type'length) &type'widestr:theData &type'widestr:aLiteral.

    #method add &type'short : aChar
        = LiteralValue new &type'length:(theData type'length + 1) &type'widestr:theData &type'widestr:(aChar Literal).

    #method getAt : aParam
        = $self getAt &type'index::aParam.

    #method getAt &type'index:anIndex
        = CharValue new &type'short:(theData @ anIndex).

    #method indexOf &type'widestr:aLiteral
        = IntNumber new &type'int:(system'external'CoreAPI find &type'widestr:theData &type'index:0 &type'widestr:aLiteral).

    #method indexOf &type'widestr:aLiteral &type'index:anIndex
        = IntNumber new &type'int:(system'external'CoreAPI find &type'widestr:theData &type'index:anIndex &type'widestr:aLiteral).

    #method indexOf &type'char:aChar &type'index:anIndex
        = IntNumber new &type'int:(system'external'CoreAPI find &type'widestr:theData &type'index:anIndex &type'short:aChar).

    #method insert &type'widestr:aLiteral &type'index:anIndex
        = system'external'CoreAPI insert &type'widestr:theData &type'index:anIndex &type'widestr:aLiteral 
            &out'type'widestr:(LiteralValue new &type'length:(theData type'length + aLiteral type'length)).
            
    #method delete &type'index:anIndex &type'length:aLength
        = system'external'CoreAPI delete &type'widestr:theData &type'index:anIndex &type'int:aLength
            &out'type'widestr:(LiteralValue new &type'length:(theData type'length - aLength)).

    #method Substring &type'index:anIndex &type'length:aLength
        = system'external'CoreAPI substring &type'widestr:$self &type'index:anIndex
            &out'type'widestr:(LiteralValue new &type'length:aLength).

    #method Substring &type'index:anIndex
        = system'external'CoreAPI substring &type'widestr:$self &type'index:anIndex
            &out'type'widestr:(LiteralValue new &type'length:(theData type'length - anIndex)).

    #method Length <= type'length.

    #method type'length = IntNumber new &type'int:(system'external'CoreAPI lengthOf &type'widestr:$self).

    #method Literal = $self.

    #method Enumerator = LiteralIterator new &type'widestr:$self.
                                    
    #method <= &&type'widestr:$self.
}

// === Basic Variable Classes ===

// --- Integer ---

#class Integer : IntNumber
{
    #constructor new : aValue
    [
        $self write::aValue.
    ]
    
    #method Number = IntNumber new &type'int:$self.

    #method write : anOperand
    [
        $self write &type'int::anOperand.
    ]
    
    #method append : anOperand
    [
        $self append &type'int::anOperand.
    ]
    
    #method reduce : anOperand
    [
        $self reduce &type'int::anOperand.
    ]
    
    #method multiplyBy : anOperand
    [
        $self multiplyBy &type'int::anOperand.
    ]
    
    #method divideInto : anOperand
    [
        $self divideInto &type'int::anOperand.
    ]
    
    #method write &type'int:anOperand
    [
        theData << anOperand.
    ]
    
    #method append &type'int:anOperand    
    [
        theData += anOperand.
    ]
        
    #method reduce &type'int:anOperand
    [
        theData -= anOperand.
    ]
        
    #method multiplyBy &type'int:anOperand
    [
        theData *= anOperand.
    ]        
        
    #method divideInto &type'int:anOperand
    [
        theData /= anOperand.
    ]
}

// --- Long ---

#class Long : LongNumber
{
    #method Number = LongNumber new &type'long:$self.

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
        
    #method write &type'long:anOperand
    [
        theData << anOperand.
    ]
    
    #method append &type'long:anOperand
    [
        theData += anOperand.
    ]
        
    #method reduce &type'long:anOperand
    [
        theData -= anOperand.
    ]
        
    #method multiplyBy &type'long:anOperand
    [
        theData *= anOperand.
    ]        
        
    #method divideInto &type'long:anOperand
    [
        theData /= anOperand.
    ]
    
    #method write &type'int:anOperand
    [
        theData << anOperand.
    ]
    
    #method append &type'int:anOperand
        <= append &type'long:(LongNumber new &type'int:anOperand).
        
    #method reduce &type'int:anOperand
        <= reduce &type'long:(LongNumber new &type'int:anOperand).
        
    #method multiplyBy &type'int:anOperand
        <= multiplyBy &type'long:(LongNumber new &type'int:anOperand).
        
    #method divideInto &type'int:anOperand
        <= divideInto &type'long:(LongNumber new &type'int:anOperand).
}

// --- Real ---

#class Real : RealNumber
{
    #method Number = RealNumber new &type'real:$self.

    #method write : anOperand
    [
        $self write::(anOperand type'real).
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
    
    #method write &type'real:anOperand
    [
        theData << anOperand.
    ]
    
    #method write &type'int:anOperand
    [
        theData << anOperand.
    ]
    
    #method append &type'real:anOperand
    [
        theData += anOperand.
    ]
        
    #method append &type'int:anOperand
    [
        theData += anOperand.
    ]
        
    #method reduce &type'real:anOperand
    [
        theData -= anOperand.
    ]
        
    #method reduce &type'int:anOperand
    [
        theData -= anOperand.
    ]
        
    #method multiplyBy &type'real:anOperand
    [
        theData *= anOperand.
    ]        
        
    #method multiplyBy &type'int:anOperand
    [
        theData *= anOperand.
    ]        
        
    #method divideInto &type'real:anOperand
    [
        theData /= anOperand.
    ]
        
    #method divideInto &type'int:anOperand
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
        theValue := aValue type'widestr.
    ]
    
    #method write : anOperand
    [
        theValue := anOperand type'widestr.
    ]
    
    #method write &type'widestr:aLiteral
    [
        theValue := aLiteral.
    ]
    
    #method append : anOperand
    [
        theValue := theValue + anOperand.
    ]
    
    #method insert &type'widestr:aLiteral &type'index:anIndex
    [
        theValue := theValue insert &type'widestr:aLiteral &type'index:anIndex.
    ]
            
    #method delete &type'index:anIndex &type'length:aLength
    [
        theValue := theValue delete &type'index:anIndex &type'length:aLength.
    ]
                        
    #method clear
    [
        theValue := emptyLiteralValue.
    ]
            
    #method <= &&type'widestr:theValue.
    
    #method => theValue.
}

// --- ArrayIterator ---

#class ArrayIterator
{
    #field theArray.
    #field theIndex.
    #field theCount.
    
    #constructor new &type'array:anArray
    [
        theArray := anArray.
        theCount := anArray Count.
        theIndex := Integer new &type'int:-1.
    ]
    
    #method Index = theIndex Number.

    #method set &Index:anIndex
    [
        theIndex write::anIndex.
    ]
    
    #method append &Index:anIndex
    [
        theIndex append::anIndex.
    ]

    #method write &Index:anIndex
    [
        theIndex write::anIndex.
    ]

    #method EOF = theIndex >= theCount.
    
    #method next
    [
        theIndex append &type'int:1.

        ^ theIndex less &type'int:theCount.
    ]

    #method get = theArray getAt &type'index:theIndex.

    #method set : anObject
    [
        theArray setAt &type'index:theIndex &type'object:anObject.
    ]
}

// --- Array ---

#class Array
{
    #field(type:array)theItems.
    
    #constructor new &type'length:aLength
    [
    ]

    #constructor new &type'length:aCount &action:anAction
    [
        #var anEnumerator := $self Enumerator.
        
        control while:(anEnumerator next) &do:[ anEnumerator set:(anAction eval:(anEnumerator Index)) ].
    ]

    #method getAt : anIndex
        = $self getAt &type'index:(anIndex type'int).

    #method getAt &type'index:anIndex
        = theItems @ anIndex.

    #method setAt:anIndex:anObject
    [
        $self setAt &type'index:(anIndex type'int) &type'object:anObject.
    ]
    
    #method setAt &type'index:anIndex &type'object:anObject
    [
        theItems @ anIndex << anObject.
    ]

    #method type'length = IntNumber new &type'int:(system'external'CoreAPI lengthOf &type'object:$self).

    #method Count = IntNumber new &type'int:(system'external'CoreAPI lengthOf &type'object:$self).

    #method Enumerator = ArrayIterator new &type'array:$self.

    #method Clone
        = Array new &type'length::$self &action: &&:i [ self@i ].

    #method <= &&type'array:$self.
}

// === Basic Convertors ===

// --- ELiteralConvertor ---

#class(role) ELiteralConvertor
{
    #method read &type'int
        = IntNumber new &type'int:(system'external'CoreAPI toInt &type'widestr::self | raise:(system'FormatException new:"An integer number is not in the correct format")).
        
    #method read &type'long
        = LongNumber new &type'long:(system'external'CoreAPI toLong &type'widestr::self | raise:(system'FormatException new:"A long integer number is not in the correct format")).
        
    #method read &type'real
        = RealNumber new &type'real:(system'external'CoreAPI toReal &type'widestr::self | raise:(system'FormatException new:"A real number is not in the correct format")).
}

// --- EIntConvertor ---

#class(role) EIntConvertor
{
    #method read &type'widestr
        = self Literal.
}

// --- ELongConvertor ---

#class(role) ELongConvertor
{
    #method read &type'widestr
        = self Literal.
}

// --- ERealConvertor ---

#class(role) ERealConvertor
{
    #method read &type'widestr
        = self Literal.
}

// --- Variant ---

#class Variant : String
{
    #method type'int = theValue~ELiteralConvertor read &type'int.
    
    #method type'long = theValue~ELiteralConvertor read &type'long.
    
    #method type'real = theValue~ELiteralConvertor read &type'real.
    
    #method type'widestr = theValue.
}

// === Basic Factory symbols ==

#class Variable
{
    #field theValue.
    
    #constructor new : aValue
    [
        theValue := aValue.
    ]
    
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
