#define system.
#define system'routines.
#define system'dynamic.
#define extensions'dynamic.

#static esonScript
    = scriptEngine load &path:"~\scripts\eson.es" eval &path:"calc.es".

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
    #field theStatemachine.
    #field theCurrentState.
    #field theDerivationTree.
    
    #method $loadBuiltinStatemachine
    [
        theStatemachine := esonScript.

        theCurrentState := theStatemachine start_state:theStatemachine.
    ]
    
    #constructor new
    [
        theDerivationTree := DerivationTree new.

        $self $loadBuiltinStatemachine.
    ]

    #method invalidate
    [
        theDerivationTree := $nil.
    ]

    #method func1 = (:aChar)
    [
        #var aType := aChar lexicalType.

        theCurrentState := theCurrentState::aType eval:theStatemachine:aChar:theDerivationTree.
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
}

// --- parserOp ---

#class(extension)parserOp
{
    #method evaluated
    [
        #var aStateMachine := StateMachine new.
        
        self run &each:(aStateMachine func1)
            | if &Error: e [
                aStateMachine invalidate.
            ].
        
        ^ aStateMachine value.
    ]
}
