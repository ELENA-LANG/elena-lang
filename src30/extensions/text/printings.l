#define system.
#define system'text.

// --- EIntPresenter ---

#class(role) EIntPresenter
{
    #method read &type'widestr &radix:aRadix
    [
        #var anOutput := LiteralValue new &type'length:20.
        
        system'external'CoreAPI convert &out'type'widestr:anOutput &type'int::self &type'int::aRadix.
        
        ^ anOutput.
    ]
}

// --- EListPresenter ---

#class(role) EListPresenter
{
    #method Literal
    [
        #var theOutput := TextBuffer new.
        
        self~EList run: &&:item [ theOutput append:(item Literal) append:"," ].
        
        #var aLength := theOutput Length.
        (0 < aLength)
            ? [ theOutput set &Length:(-1 + aLength). ].
        
        ^ theOutput Literal.
    ]
}

// --- ListPresenter ---

#class ListPresenter
{
    #field theList.

    #constructor new : aList
    [
        theList := aList.
    ]
    
    #method Literal = theList~EListPresenter Literal.
}

// --- IntPresenter ---

#class IntPresenter
{
    #field theNumber.    
    #field theRadix.

    #method Literal
        = theNumber~EIntPresenter read &type'widestr &radix:theRadix.
    
    #constructor new:aNumber &radix:aRadix
    [
        theNumber := aNumber.
        theRadix := aRadix.
    ]
}

//// --- IntFormatter ---
//
//#class IntFormatter : EIntFormatter
//{
//    #field theLiteral.    
//    #field theRadix.
//    
//    #method Literal = theLiteral.
//    
//    #method Radix = theRadix.
//    
//    #constructor new &Literal:aLiteral &Radix:aRadix
//    [
//        theLiteral := aLiteral.
//        theRadix := aRadix.
//    ]
//    
//    #constructor new &Literal:aLiteral
//    [
//        theLiteral := aLiteral.
//        theRadix := 10.
//    ]
//}

// --- presenterClassFactory ---

#symbol presenterClassFactory =
{
    new &type'int:anObject
        = IntPresenter.
}.

#symbol presenterFactory =
{
    new:anObject &radix:aRadix
        = presenterClassFactory new::anObject new:anObject &radix:aRadix.
}.

// --- printingLn ---

#symbol printingLn = &&:aLine [ 'program'output writeLine:aLine ].