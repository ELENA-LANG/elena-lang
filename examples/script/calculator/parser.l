#import system.
#import system'routines.
//#import system'dynamic.
#import extensions'text.
//#import extensions'scripting.

#class(extension) lexicalOp
{
    #method lexicalType
    [
        ((self >= #48)and:(self <= #57))
            ? [ ^ %digit. ].

        ((self >= #97)and:(self <= #122))
            ? [ ^ %letter. ].

        self =>
            #32 ? [ ^ %whitespace. ]
            #40 ? [ ^ %opening. ]
            #41 ? [ ^ %closing. ]
            #42 ? [ ^ %star. ]
            #43 ? [ ^ %plus. ]
            #44 ? [ ^ %comma. ]
            #45 ? [ ^ %minus. ]
            #46 ? [ ^ %dot. ]
            #47 ? [ ^ %slash. ]
            ! [ ^ %unknown. ].
    ]
}

#class DerivationTree
{
    #field theRoot.
    #field theLast.

    #method last_node = theLast.
    
    #method value => theRoot.

    #method append : aNode
    [
        theLast := aNode.

        theRoot := theRoot + aNode.
    ]

    #method newArgument
    [
        theLast := theLast newArgument.
    ]

    #constructor new
    [
        theRoot := BaseTreeNode { level = -1. add : aNode = aNode. }
    ]
}

// --- BaseTreeNode ---

#class BaseTreeNode
{
    #field theStrategy.
    #field theLevel.
    
    #method level = theLevel.
    
    #method add : aNode
    [
        (self level < aNode level)?
        [
            self += aNode.
            
            ^ self.
        ]
        ! [
            aNode += self.
            
            ^ aNode.
        ].
    ]

    #method setFunction : function []
        
    #constructor new:strategy &baseLevel:baseLevel
    [
        theStrategy := strategy.
        theLevel := baseLevel + strategy order.
    ]
}

// --- TreeNode ---

#class TreeNode :: BaseTreeNode
{
    #field theLeft.
    #field theRight.

    #method append : aNode
    [
        ($nil == theLeft)
            ? [ theLeft := aNode. ]
            ! [
                ($nil == theRight)
                    ? [ theRight := aNode. ]
                    ! [ theRight := theRight + aNode. ].
            ].
    ]
    
    #method value
    [
        ($nil == theLeft)
            ? [ #throw InvalidArgumentException new. ].
        
        ($nil == theRight)
            ? [ #throw InvalidArgumentException new. ].
        
        ^ theStrategy evalNode:(theLeft value):(theRight value).
    ]
    
    #constructor new:aStrategy &baseLevel:baseLevel
        <= (new:aStrategy &baseLevel:baseLevel)
    [
    ]    
}

// --- TreeLeaf ---

#class TreeLeaf :: BaseTreeNode
{
    #field theToken.
    #field theFunction.

    #method setFunction : function
    [
        theFunction := function.
    ]

    #method newArgument
    [
        ($nil == theFunction)
            ? [ #throw InvalidArgumentException new. ].
            
        ^ theFunction set.
    ]

    #method appendContent : aChar
    [
        theToken += aChar.
    ]

    #method value = theStrategy evalToken:theToken.
            
    #constructor new:aStrategy &baseLevel:baseLevel
        <= (new:aStrategy &baseLevel:baseLevel)
    [
        theToken := String new.
    ]
}

// --- FunctionNode ---

#class FunctionNode :: TreeLeaf
{
    #field theArg1.
    #field theArg2.
    
    #field theArgState.
    
    #method value
    [
        #var subj := Signature new &literal:theToken.
        
        ($nil == theArg1)
            ? [ ^ theStrategy::subj get. ].
            
        ($nil == theArg2)
            ? [ ^ theStrategy::subj eval:(theArg1 value). ].
            
        ^ theStrategy::subj eval:(theArg1 value):(theArg2 value).
    ]

    #method append &first_arg:node
    [
        ($nil == theArg1)
            ? [ theArg1 := node setFunction:$self. ]
            ! [ theArg1 := theArg1 + node setFunction:$self. ].
    ]

    #method append &second_arg:node
    [
        ($nil == theArg2)
            ? [ theArg2 := node setFunction:$self. ]
            ! [ theArg2 := theArg2 + node setFunction:$self. ].
    ]

    #method set
        => theArgState.
        
    #method set &first_arg
    [
        theArgState := %second_arg.
    ]    
        
    #method set &second_arg
    [
        theArgState := %third_arg.
    ]    

    #method append : aNode
        => theArgState.
        
    #constructor new:aStrategy &baseLevel:baseLevel
        <= (new:aStrategy &baseLevel:baseLevel)
    [
        theArgState := %first_arg.
    ]
}

// --- StateMachine ---

#class StateMachine
{
    #field theStates.
    #field theCurrentState.
    #field theDerivationTree.
    
    #field(prop:level)theLevel.

    #constructor new : states
    [
        theDerivationTree := DerivationTree new.
        theStates := states.
        theLevel := 0.
    ]

    #method(generic)seek
    [
        theCurrentState := theStates::$subject get. 
    ]

    #method opening : ch
    [
        theLevel := theLevel + 10.
        
        $self::theCurrentState opening:ch.
    ]

    #method closing : ch
    [
        theLevel := theLevel - 10.
        
        (0 > theLevel)
            ? [ #throw InvalidOperationException new. ].
        
        $self::theCurrentState closing:ch.
    ]

    #method comma : ch
    [
        theDerivationTree newArgument.
        
        $self::theCurrentState comma:ch.
    ]

    #method invalidate
    [
        theDerivationTree := $nil.
    ]

    #method func1 = (:aChar)
    [
        #var type := aChar lexicalType.

        self::type eval:aChar.
    ].

    #method value
    [
        #var aValue := $nil.
        
        ($nil == theDerivationTree)
            ? [ aValue := "Invalid expression". ]
            ! [
                aValue := theDerivationTree value
                    | back:"Invalid expression".
            ].
            
        ^ aValue.
    ]

    #method newLeaf : strategy
    [
        theDerivationTree append:(TreeLeaf new:strategy &baseLevel:theLevel).
    ]

    #method newNode : strategy
    [
        theDerivationTree append:(TreeNode new:strategy &baseLevel:theLevel).
    ]

    #method newFunction : strategy
    [
        theDerivationTree append:(FunctionNode new:strategy &baseLevel:theLevel).
    ]
    
    #method addContent : content
    [
        theDerivationTree last_node appendContent:content.
    ]
    
    #method => theCurrentState.
}

// --- parserOp ---

#class(extension)parserOp
{
    #method evaluated &with:states
    [
        #var aStateMachine := StateMachine new:states seek &state0.
        
        self run &each:(aStateMachine func1)
            | if &Error: e [
                console writeLine:e.
                
                aStateMachine invalidate.
            ].
        
        ^ aStateMachine value.
    ]
}
