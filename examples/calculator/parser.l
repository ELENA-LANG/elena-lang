#define system.
#define system'routines.
#define system'dynamic.
#define extensions'dynamic.

#class(extension) lexicalOp
{
    #method lexicalType
    [
        ((self >= #48)and:(self <= #57))
            ? [ ^ %digit. ].

        (self == #43)
            ? [ ^ %plus. ].
            
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
        (self order > aNode order)?
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
    
    #method value = theStrategy evalNode:(theLeft value):(theRight value).
    
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
        //scriptEngine load &path:"~\scripts\eson.es".
        
        #var aNumberStratagy := 
            {
                order = 0.
                
                evalToken : aToken = realConvertor convert:aToken.
            }.    

        #var aSumStratagy := 
            {
                order = 2.
                
                evalNode : aLeftToken : aRightToken = aLeftToken + aRightToken.
            }.    
            
        //{ digit : aChar : aTree [ aTree append:(TreeLeaf new:aNumberStratagy append:aChar). ^ aState1. ] }.

//        { plus : aChar : aTree [ aTree append:(TreeNode new:aSumStratagy). ^ aState0. ] 
//          digit : aChar : aTree [ aTree last_node append:aChar. ^ aState1. ]}.
        theStatemachine := Struct
        (
            %state0,
            Struct 
            (
                %digit,
                Tape(
                    2, tapeControl, %(getAt&args)(1),
                    aNumberStratagy, TreeLeaf, 
                    %(new)(1), 
                    %(append)(1),
                    1, tapeControl, %(getAt&args)(1),
                    %(append)(1),
                    3, tapeControl, %(getAt&args)(1),
                    %(get&state1)(0))
            ),
            %state1,
            Struct
            (
                %digit,
                Tape(
                    2, tapeControl, %(getAt&args)(1),
                    1, tapeControl, %(getAt&args)(1),
                    %(get&last_node)(0),
                    %(append)(1),
                    3, tapeControl, %(getAt&args)(1),
                    %(get&state1)(0)),
                    
                %plus,
                Tape(
                    aSumStratagy, TreeNode,
                    %(new)(1),
                    1, tapeControl, %(getAt&args)(1),
                    %(append)(1),
                    3, tapeControl, %(getAt&args)(1),
                    %(get&state0)(0))
            ),
            
            %start_state,
            Tape(
                    1, tapeControl, %(getAt&args)(1),
                    %(get&state0)(0))            
        ).

        theCurrentState := theStatemachine start_state:theStatemachine.
    ]
    
    #constructor new
    [
        theDerivationTree := DerivationTree new.

        $self $loadBuiltinStatemachine.
    ]

    #method func1 = (:aChar)
    [
        #var aType := aChar lexicalType.

        theCurrentState := theCurrentState::aType eval:theStatemachine:aChar:theDerivationTree.
    ].

    #method value => theDerivationTree.
}

// --- parserOp ---

#class(extension)parserOp
{
    #method evaluated
    [
        #var aStateMachine := StateMachine new.
        
        self run &each:(aStateMachine func1).
        
        ^ aStateMachine value.
    ]
}
