#define system.
#define extensions.

// --- Token ---

#class Token
{
    #field theValue.
    
    #constructor new
    [
        theValue := String new.
    ]
    
    #constructor new : aValue
    [
        theValue := String new:aValue.
    ]
    
    #method ParseOrder = 0.
    
    #method append : aChar
    [
        theValue += aChar.
    ]
    
    #method add : aNode
    [
        ^ aNode += self.
    ]

    #method Number = convertControl toReal:theValue.    
}

// --- Node ---

#class(role) ENoneAssigned
{
    #method append : anObject
        => %setLeft.
}

#class(role) ELeftAssigned
{
    #method append : anObject
        => %setRight.
}

#class(role) ERightAssigned
{
    #method append : anObject
        => %appendRight.
}

#class Node
{
    #field theLeft.
    #field theRight.
    #field theState.
    
    #method setRight : aNode
    [
        theRight := aNode.
        
        theState := ERightAssigned.
    ]
    
    #method setLeft : aNode
    [
        theLeft := aNode.
        
        theState := ELeftAssigned.
    ]
    
    #constructor new
    [
        theState := ENoneAssigned.
    ]

    #method add : aNode
        = (self ParseOrder > aNode ParseOrder)
            ? [
                self += aNode.
                
                ^ self.
            ]
            ! [
                aNode += self.
                
                ^ aNode.
            ].
        
    #method appendRight : aNode
    [
        (theRight ParseOrder > aNode ParseOrder)
        ? [
            theRight += aNode.
        ]
        ! [
            theRight := aNode += theRight.
        ].        
    ]
    
    #method => theState.
}

// --- SummaryNode

#class SummaryNode : Node
{
    #method ParseOrder = 2.
    
    #method Number = theLeft Number + theRight Number.
}

// --- DifferenceNode ---

#class DifferenceNode : Node
{
    #method ParseOrder = 2.
    
    #method Number = theLeft Number - theRight Number.
}

// --- ProductNode ---

#class ProductNode : Node
{
    #method ParseOrder = 1.
    
    #method Number = theLeft Number * theRight Number.
}

// --- FractionNode ---

#class FractionNode : Node
{
    #method ParseOrder = 1.
    
    #method Number = theLeft Number / theRight Number.
}

// --- SubExpression ---

#class SubExpression
{
    #field theParser.
    #field theCounter.
    
    #constructor new
    [
        theParser := arithmeval'Parser new.
        theCounter := Integer new:1.
    ]

    #method ParseOrder = 0.
    
    #method add : aNode
    [
        ^ aNode += self.
    ]
    
    #method validate
    [
        (theCounter < 0)
            ? [ #throw Exception new:"Invalid expression". ].
            
        ^ (0 == theCounter).
    ]
    
    #method append : aChar
    [
        aChar =>
            41 ? [ 
                theCounter -= 1. 
            ]
            40 ? [ theCounter += 1 ]
            ! [ theParser evaluate:aChar ].
    ]

    #method Number
        = $self validate
            ? [ theParser Number ]
            ! [ #throw Exception new:"Invalid expression". ].
}

// ---- Parser ----

#class(role)EStartState
{
    #method eval : aChar
        => %onStart.
}

#class(role)EBracketState
{
    #method eval : aChar
        => %onBrackets.
}

#class(role)EOperatorState
{
    #method eval : aChar
        => %onOperator.
}

#class(role)ENumberState
{
    #method eval : aChar
        => %onDigit.
}

#class Parser : system'routines'BasePattern
{
    #field theToken.
    #field theTopNode.
    #field theRole.
    
    #method onBrackets : aChar
    [
        theToken += aChar.
                
        (theToken validate)
          ? [
              theRole := ENumberState.
          ].
    ]

    #method onStart : aChar
    [
        aChar =>
             40 ? [ // (
                theToken := SubExpression new.
                theTopNode := theToken.
  
                theRole := EBracketState.
            ]
            45 ? [ // -
                theToken := DifferenceNode new add:(Token new:"0").
                
                theTopNode := theToken.
                
                theRole := EOperatorState.
            ]
            ! [
                theToken := Token new.
                theTopNode := theToken.
                theRole := ENumberState.
                
                $self appendDigit:aChar.                
            ].
    ]

    #method onOperator : aChar
    [
        aChar =>
            40 ? [
                theToken := SubExpression new.
                theTopNode += theToken.
                theRole := EBracketState.
            ]
            ! [
                theToken := Token new.
                theTopNode += theToken.
                theRole := ENumberState.

                $self appendDigit:aChar.
            ].
    ]

    #constructor new
    [
        theRole := EStartState.
    ]

    #method Number = theTopNode Number.

    #method appendDigit : aChar
    [
        (aChar >= 48) and:(aChar < 58)
        ? [
            theToken += aChar.
        ]
        ! [
            #throw Exception new:"Invalid expression".
        ]
        
    ]

    #method onDigit : aChar
    [
        aChar =>
            40 ? [      // (
                theToken := SubExpression new.
                theTopNode := theToken.
                theRole := EBracketState.
            ]
            42 ? [      // *
                theTopNode := theTopNode + ProductNode new.

                theRole := EOperatorState.
            ]
            43 ? [      // +
                theTopNode := theTopNode + SummaryNode new.

                theRole := EOperatorState.
            ]
            45 ? [      // -
                theTopNode := theTopNode + DifferenceNode new.

                theRole := EOperatorState.
            ]
            47 ?       // /
            [
                theTopNode := theTopNode + FractionNode new.

                theRole := EOperatorState.
            ]
            ! [
                $self appendDigit:aChar.
            ].
    ]

    #method => theRole.
}

#symbol program =
[
    #var aText := String new.

    control while:(consoleEx readLine:aText Length > 0) &do:
    [
        #var aParser := Parser new.

        consoleEx writeLine:"=" :(aParser foreach:aText Number)
            | ifFailed: 
                [
                    consoleEx writeLine:"Invalid Expression".
                ].
    ].
].