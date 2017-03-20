/// a program which parses and evaluates arithmetic expressions. 

#import system'routines.
#import extensions.
#import extensions'text.

// --- Token ---

class Token
{
    object theValue.
    object theLevel.
    
    constructor new &level:aLevel
    [
        theValue := StringWriter new.
        theLevel := aLevel + 9.
    ]
    
    level = theLevel.
    
    append : aChar
    [
        theValue << aChar.
    ]
    
    number = theValue get toReal.
}

// --- Node ---

class Node
{
    object theLeft.
    object theRight.
    object theLevel.

    constructor new &level:aLevel
    [
        theLevel := aLevel.
    ]
    
    level = theLevel.

    left = theLeft.

    right = theRight.

    set &left:anObject [ theLeft := anObject. ]

    set &right:anObject [ theRight := anObject. ]
}

// --- SummaryNode

class SummaryNode :: Node
{
    constructor new &level:aLevel
        <= new &level:(aLevel + 1).
    
    number = theLeft number + theRight number.
}

// --- DifferenceNode ---

class DifferenceNode :: Node
{
    constructor new &level:aLevel
        <= new &level:(aLevel + 1).
    
    number = theLeft number - theRight number.
}

// --- ProductNode ---

class ProductNode :: Node
{
    constructor new &level:aLevel
        <= new &level:(aLevel + 2).
    
    number = theLeft number * theRight number.
}

// --- FractionNode ---

class FractionNode :: Node
{
    constructor new &level:aLevel
        <= new &level:(aLevel + 2).
    
    number = theLeft number / theRight number.
}

// --- Expression ---

class Expression
{
    object theLevel.
    object theTop.
    
    constructor new &level:aLevel
    [
        theLevel := aLevel.
    ]
    
    top = theTop.

    set &top:aNode [ theTop := aNode. ]
            
    right = theTop.

    set &right:aNode [ theTop := aNode. ]
            
    level = theLevel.
    
    number => theTop.
}

// --- States ---

operatorState = (:ch)
[
    ch =>
        #40 ? [      // (
            self new &bracket goto &start.
        ]
        ! [
            self new &token append:ch goto &token.
        ].
].

tokenState = (:ch)
[
    ch =>
        #41 ? [      // )
            self close &bracket goto &token.
        ]
        #42 ? [      // *
            self new &product goto &operator.
        ]
        #43 ? [      // +
            self new &summary goto &operator.
        ]
        #45 ? [      // -
            self new &difference goto &operator.
        ]
        #47 ?       // /
        [
            self new &fraction goto &operator.
        ]
        ! [
            self append:ch.
        ].
].

startState = (:ch)
[
    ch =>
        #40 ? [      // (
            self new &bracket goto &start.
        ]
        #45 ? [ // -
            self new &token append &literal:"0" new &difference goto &operator.
        ]
        ! [
            self new &token append:ch goto &token.
        ].
].

// --- Scope ---

class Scope
{
    object theState.
    object theLevel.
    object theParser.
    object theToken.
    object theExpression.
    
    constructor new &parser:aParser
    [
        theState := startState.
        theLevel := 0.
        theExpression := Expression new &level:0.
        theParser := aParser.
    ]
                                       
    new &token
    [
        theToken := theParser appendToken &expression:theExpression &level:theLevel.
    ]                                                
    
    new &summary
    [
        theToken := nil.
        
        theParser appendSummary &expression:theExpression &level:theLevel.
    ]                                                
    
    new &difference
    [
        theToken := nil.
        
        theParser appendDifference &expression:theExpression &level:theLevel.
    ]                                                
      
    new &product
    [
        theToken := nil.
        
        theParser appendProduct &expression:theExpression &level:theLevel.
    ]                                                
    
    new &fraction
    [
        theToken := nil.
        
        theParser appendFraction &expression:theExpression &level:theLevel.
    ]                                                

    new &bracket
    [
        theToken := nil.
        
        theLevel := theLevel + 10.
        
        theParser appendSubexpression &expression:theExpression &level:theLevel.
    ]            

    close &bracket
    [
        (theLevel < 10)
            ? [ InvalidArgumentException new &message:"Invalid expression" raise. ].
        
        theLevel := theLevel - 10.
    ]            
                                    
    append:ch
    [
        ((ch >= #48) and:(ch < #58))
            ? [ theToken append:ch. ]
            ! [ InvalidArgumentException new &message:"Invalid expression" raise. ].
    ]
            
    append &literal:aLiteral
    [
        aLiteral run &each: ch [ self append:ch. ].
    ]
                                                         
    goto &start
    [
        theState := startState.
    ]
                                                                  
    goto &token
    [
        theState := tokenState.
    ]
                                                                  
    goto &operator
    [
        theState := operatorState.
    ]
    
    number => theExpression.
    
    dispatch => theState.
}

// ---- Parser ----

class Parser
{
    appendToken &expression:anExpression &level:aLevel
    [
        var aToken := Token new &level:aLevel.
        
        anExpression set &top:($self append &last:(anExpression top) &new:aToken).
        
        ^ aToken.
    ]

    appendSummary &expression:anExpression &level:aLevel
    [
        anExpression set &top:($self append &last:(anExpression top) &new:(SummaryNode new &level:aLevel)).
    ]

    appendDifference &expression:anExpression &level:aLevel
    [
        anExpression set &top:($self append &last:(anExpression top) &new:(DifferenceNode new &level:aLevel)).
    ]

    appendProduct &expression:anExpression &level:aLevel
    [
        anExpression set &top:($self append &last:(anExpression top) &new:(ProductNode new &level:aLevel)).
    ]

    appendFraction &expression:anExpression &level:aLevel
    [
        anExpression set &top:($self append &last:(anExpression top) &new:(FractionNode new &level:aLevel)).
    ]

    appendSubexpression &expression:anExpression &level:aLevel
    [
        anExpression set &top:($self append &last:(anExpression top) &new:(Expression new &level:aLevel)).
    ]

    append &last:aLastNode &new:aNewNode
    [
        ($nil == aLastNode)
            ? [ ^ aNewNode. ].
        
        (aNewNode level <= aLastNode level)
            ? [ aNewNode set &left:aLastNode. ^ aNewNode. ].
            
        var aParent := aLastNode.
        var aCurrent := aLastNode right.
        while (($nil != aCurrent) and:[ aNewNode level > aCurrent level ])
            [ aParent := aCurrent. aCurrent := aCurrent right. ].
        
        ($nil == aCurrent)
            ? [ aParent set &right:aNewNode. ]
            ! [ aNewNode set &left:aCurrent. aParent set &right:aNewNode. ].
            
        ^ aLastNode.
    ]
    
    run : aText
    [
        var aScope := Scope new &parser:$self.

        aText run &each: ch [ aScope eval:ch. ].

        ^ aScope number.
    ]
}

program =
[
    var aText := String new.
    var aParser := Parser new.

    [ console readLine save &to:aText length > 0] doWhile:
    [
        console writeLine:"=" :(aParser run:aText)
            | if &Error:e [
                    console writeLine:"Invalid Expression".
                ].
                
        aText clear.                
    ].
].