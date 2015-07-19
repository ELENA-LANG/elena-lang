#define system.
#define system'routines.
#define system'dynamic.

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
    #field theCurrentState.
    #field theDerivationTree.
    
    #constructor new
    [
        theDerivationTree := DerivationTree new.

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

        #var aState0 := Variable new.
        #var aState1 := Variable new.

        //{ digit : aChar : aTree [ aTree append:(TreeLeaf new:aNumberStratagy append:aChar). ^ aState1. ] }.
        #var aXCode0 := Struct(%digit, Tape(
            2, extensions'control, %(getAt&args)(1),
            aNumberStratagy, TreeLeaf, 
            %(new)(1), 
            %(append)(1),
            1, extensions'control, %(getAt&args)(1),
            %(append)(1),
            aState1)).
        
//        { plus : aChar : aTree [ aTree append:(TreeNode new:aSumStratagy). ^ aState1. ] 
//          digit : aChar : aTree [ aTree append:(TreeLeaf new:aNumberStratagy append:aChar). ^ aState0. ]}.
        #var aXCode1 := Struct(
            %digit, Tape(
                2, extensions'control, %(getAt&args)(1),
                aNumberStratagy, TreeLeaf, 
                %(new)(1), 
                %(append)(1),
                1, extensions'control, %(getAt&args)(1),
                %(append)(1),
                aState0),
            
            %plus, Tape(
                aSumStratagy, TreeNode,
                %(new)(1),
                1, extensions'control, %(getAt&args)(1),
                %(append)(1),
                aState0)
            ).

        aState0 write:aXCode0.
        aState1 write:aXCode1.

        theCurrentState := aState0.
    ]

    #method func1 = (:aChar)
    [
        #var aType := aChar lexicalType.

        theCurrentState := theCurrentState::aType eval:aChar:theDerivationTree.
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
