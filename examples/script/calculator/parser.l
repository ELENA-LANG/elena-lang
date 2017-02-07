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

        (self == #43)
            ? [ ^ %plus. ].

        (self == #45)
            ? [ ^ %minus. ].
            
        (self == #42)
            ? [ ^ %star. ].
            
        (self == #47)
            ? [ ^ %slash. ].
            
        (self == #40)
            ? [ ^ %opening. ].
            
        (self == #41)
            ? [ ^ %closing. ].
            
        ^ %unknown.            
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

    #constructor new
    [
        theRoot := BaseTreeNode { order = -1. add : aNode = aNode. }
    ]
}

// --- BaseTreeNode ---

#class BaseTreeNode
{
    #field theStrategy.
    
    #method order = theStrategy order.
    
    #method add : aNode
    [
        (self order < aNode order)?
        [
            self += aNode.
            
            ^ self.
        ]
        ! [
            aNode += self.
            
            ^ aNode.
        ].
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
    
    #constructor new : aStrategy
    [
        theStrategy := aStrategy.
    ]
}

// --- TreeLeaf ---

#class TreeLeaf :: BaseTreeNode
{
    #field theToken.    

    #method append : aChar
    [
        theToken += aChar.
    ]

    #method value = theStrategy evalToken:theToken.
            
    #constructor new : aStrategy
    [
        theToken := String new.
        theStrategy := aStrategy.
    ]
}

// --- StateMachine ---

#class StateMachine
{
    #field theStates.
    #field theCurrentState.
    #field theDerivationTree.

    #constructor new : states
    [
        theDerivationTree := DerivationTree new.
        theStates := states.
    ]

    #method(generic)seek
    [
        theCurrentState := theStates::$subject get. 
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
        theDerivationTree append:(TreeLeaf new:strategy).
    ]
    
    #method addContent : content
    [
        theDerivationTree last_node append:content.
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
                aStateMachine invalidate.
            ].
        
        ^ aStateMachine value.
    ]
}
