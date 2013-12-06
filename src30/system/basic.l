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
}

// --- intnumber ---

#class IntNumber : IntBaseNumber
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

    #method shift : anOrder
    [
        #var(type:int)retVal := intOp shift &type'int:$self &type'int::anOrder.
        
        ^ IntNumber new &type'int:retVal.
    ]

    #method Literal = strOp convert &type'int:$self.

    #method Negative = 0 - $self.

    #method not
    [
        #var(type:int)retVal := intOp invert &type'int:$self.
        
        ^ IntNumber new &type'int:retVal.
    ]
            
    #method <= &&type'int:$self.
}

// --- longnumber ---

#class LongNumber : IntBaseNumber
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

    #method shift : anOrder
    [
        #var(type:long)aRetVal := intOp shift &type'long:$self &type'int::anOrder.
        
        ^ LongNumber new &type'long:aRetVal.
    ]

    #method Literal = strOp convert &type'long:$self.

    #method Negative = 0l - $self.

    #method not
    [
        #var(type:long)retVal := intOp invert &type'long:$self.
        
        ^ LongNumber new &type'long:retVal.
    ]
            
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

    #method Literal = strOp convert &type'real:$self.
    
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
    
    #constructor new : aValue
        <= new::aValue.
    
    #constructor new &type'int:aCode
    [
        theData << aCode.
    ]

    #constructor new &widechar:aCode
        <= new &type'int:aCode.

    #method widechar = $self.

    #method equal &type'int:anOperand 
        = theData == anOperand.

    #method less &type'int:anOperand 
        = theData < anOperand.

    #method notless &type'int:anOperand 
        = theData >= anOperand.

    #method equal &widechar:anOperand 
        = $self equal &type'int:anOperand .

    #method less &widechar:anOperand 
        = $self less &type'int:anOperand .

    #method equal &type'widestr:aLiteral
        = aLiteral equal &widechar:$self.

    #method Number = IntNumber new &type'int:theData.

    #method Literal = system'LiteralValue new &type'length:1 &widechar:$self.

    #method type'widestr => %Literal. 

    #method <= &&widechar:$self.
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

    Literal => %type'widestr.

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

    #method Index => %type'index.
            
    #method type'index = theIndex Number.

    #method append &type'index:anIndex
    [
        theIndex append::anIndex.
    ]

    #method write &type'index:anIndex
    [
        theIndex write::anIndex.
    ]

    #method EOF = theIndex less &type'int:(theLiteral type'length) ? [ false ] ! [ true ].
    
    #method Available = theIndex less &type'int:(theLiteral type'length).
    
    #method next
    [
        theIndex append &type'int:1.

        ^ theIndex less &type'int:(theLiteral type'length).
    ]
    
    #method get
    [
        #var(type:short)aChar := theLiteral getAt &type'index:theIndex.
        
        ^ CharValue new &type'int:aChar.
    ]
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

    #constructor new &type'length:aSize &widechar:aChar
        <ws_fill>.

    #constructor new &type'widestr:aLiteral
        <= new &type'length::aLiteral &type'widestr:aLiteral.

    #constructor new &type'length:aSize &type'widestr:aLiteral1 &type'widestr:aLiteral2
        <ws_concat>.

    #method type'widestr = $self.

    #method equal &type'widestr : aLiteral
        = theData == aLiteral.

    #method equal &widechar : aChar
        <= equal &type'widestr::aChar.

    #method less &type'widestr : aLiteral
        = theData < aLiteral.

    #method notless &type'widestr : aLiteral
        = theData >= aLiteral.

    #method add : aLiteral
        = $self add::aLiteral.

    #method add &type'widestr : aLiteral
    [
        #var(type:int)aLen := (theData type'length) + (aLiteral type'length).
        
        ^ LiteralValue new &type'length:aLen &type'widestr:theData &type'widestr:aLiteral.
    ]

    #method add &widechar : aChar
        = LiteralValue new &type'length:(theData type'length + 1) &type'widestr:theData &type'widestr::aChar.

    #method getAt : aParam
    [
        #var(type:short)aChar := $self getAt &type'index::aParam.
        
        ^ CharValue new &type'int:aChar.
    ]

    #method getAt &type'index:anIndex &out'type'short:aChar
        <ws_getat>.

    #method indexOf &type'index:anIndex &type'widestr:aLiteral &out'type'int:aRetVal
        <ws_indexof>.

    #method indexOf &type'widestr:aLiteral
    [
        #var(type:int)aRetVal := $self indexOf &type'index:0 &type'widestr:aLiteral.
        
        ^ IntNumber new &type'int:(aRetVal).
    ]
    
    #method indexOf &type'index:anIndex &type'widestr:aLiteral
    [
        #var(type:int)aRetVal := $self indexOf &type'index:anIndex &type'widestr:aLiteral.
        
        ^ IntNumber new &type'int:(aRetVal).
    ]
    
    #method indexOf &type'index:anIndex &widechar:aChar
    [
        #var(type:int)aRetVal := $self indexOf &type'index:anIndex &type'widestr::aChar.
        
        ^ IntNumber new &type'int:(aRetVal).
     ]
     
    #method insert &type'index:anIndex &type'widestr:aLiteral
        = strOp insert &type'widestr:$self &type'index:anIndex &type'widestr:aLiteral.

    #method delete &type'index:anIndex &type'length:aLength
        = strOp delete &type'widestr:$self &type'index:anIndex &type'length:aLength.

    #method Substring &type'index:anIndex &type'length:aLength
        = strOp substring &type'widestr:$self &type'index:anIndex &type'length:aLength.

    #method Substring &type'index:anIndex
        = strOp substring &type'widestr:$self &type'index:anIndex &type'length:(theData type'length - anIndex).

    #method Length <= type'length.

    #method type'length
    [
        #var(type:int)aLength := theData type'length.
        
        ^ IntNumber new &type'int:(aLength).
    ]

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
    #constructor new : aValue
    [
        $self << aValue.
    ]
    
    #method Number = RealNumber new &type'real:$self.

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
    
    #method insert &type'index:anIndex &type'widestr:aLiteral
    [
        theValue := theValue insert &type'index:anIndex &type'widestr:aLiteral.
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

// --- Boolean ---

#class Boolean
{
    #field theValue.
    
    #constructor new : aValue
    [        
        aValue ? [ theValue := true. ] ! [ theValue := false. ].
    ]

    #method Value = theValue.

    #method write : aValue
    [
        aValue ? [ theValue := true. ] ! [ theValue := false. ].
    ]
            
    #method invert
    [
        theValue := theValue not.
    ]            
            
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
    
    #method Index => %type'index.
    
    #method type'index = theIndex Number.

    #method set &type'index:anIndex
    [
        theIndex write::anIndex.
    ]
    
    #method append &type'index:anIndex
    [
        theIndex append::anIndex.
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

    #constructor new &type'length:aCount &function:aFunction
    [
        #var anEnumerator := $self Enumerator.
        
        control while:(anEnumerator next) &do:[ anEnumerator set:(aFunction eval:(anEnumerator Index)) ].
    ]

    #method getAt : anIndex
        = $self getAt &type'index::anIndex.

    #method getAt &type'index:anIndex
        <array_getat>.

    #method setAt:anIndex:anObject
    [
        $self setAt &type'index::anIndex &type'object:anObject.
    ]
    
    #method setAt &type'index:anIndex &type'object:anObject
        <array_setat>.

    #method add : anObject
        = $self add::anObject.
        
    #method add &type'array:anArray
    [
        #var aLength1 := $self type'length.
        #var aLength2 := anArray type'length.
        
        #var aRetVal := Array new &type'length:(aLength1 + aLength2).
        
        control from:0 &till:aLength1 &do: &&:i
            [ aRetVal setAt &type'index:i &type'object:(self getAt &type'index:i). ].
            
        control from:0 &till:aLength2 &do: &&:j
            [ aRetVal setAt &type'index:(aLength1 + j) &type'object:(anArray getAt &type'index:j). ].
            
        ^ aRetVal.
    ]

    #method type'length 
    [
        #var(type:int)aLen := arrayOp read &type'length:$self.
        
        ^ IntNumber new &type'int:aLen.
    ]

    #method Count <= type'length.

    #method Enumerator = ArrayIterator new &type'array:$self.

    #method Clone
        = Array new &type'length::$self &function: &&:i [ self@i ].

    #method <= &&type'array:$self.
}

// === Basic Convertors ===

// --- ELiteralConvertor ---

#class(role) ELiteralConvertor
{
    #method read &type'int
    [
        #var(type:int)aRetVal := strOp convert &type'widestr::self.
        
        ^ IntNumber new &type'int:aRetVal.
    ]

    #method read &type'long
    [
        #var(type:long)aRetVal := strOp convert &type'widestr::self.
        
        ^ LongNumber new &type'long:aRetVal.
    ]

    #method read &type'real
    [
        #var(type:real)aRetVal := strOp convert &type'widestr::self.
        
        ^ RealNumber new &type'real:aRetVal.
    ]
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

// --- BasicType ---

#symbol typeControl =
{
    eval &type'int : anObject = %type'int.
    eval &type'real : anObject = %type'real.
    eval &type'long : anObject = %type'long.
    eval &type'widestr : anObject = %type'widestr.
    eval &widechar : anObject = %widechar.
}.