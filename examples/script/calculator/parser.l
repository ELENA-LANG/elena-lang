import system'routines.
import extensions.
import extensions'text.

extension lexicalOp
{
    lexicalType
    [
        if ((self >= $48)&&(self <= $57))
            [ ^ %digit ].

        if ((self >= $97)&&(self <= $122))
            [ ^ %letter ].

        self =>
            $32 [ ^ %whitespace ];
            $40 [ ^ %opening ];
            $41 [ ^ %closing ];
            $42 [ ^ %star ];
            $43 [ ^ %plus ];
            $44 [ ^ %comma ];
            $45 [ ^ %minus ];
            $46 [ ^ %dot ];
            $47 [ ^ %slash ];
            ! [ ^ %unknown ].
    ]
}

class DerivationTree
{
    object theRoot.
    object theLast.

    last_node = theLast.
    
    value => theRoot.

    append : aNode
    [
        theLast := aNode.

        theRoot := theRoot + aNode.
    ]

    newArgument
    [
        theLast := theLast newArgument.
    ]

    constructor new
    [
        theRoot := BaseTreeNode::{ level = -1. add : aNode = aNode. }
    ]
}

// --- BaseTreeNode ---

class BaseTreeNode
{
    object theStrategy.
    object theLevel.
    
    level = theLevel.
    
    add : aNode
    [
        if (self level < aNode level)
        [
            self append:aNode.
            
            ^ self
        ];
        [
            aNode append:self.
            
            ^ aNode
        ].
    ]

    setFunction : function []
        
    constructor new:strategy baseLevel:baseLevel
    [
        theStrategy := strategy.
        theLevel := baseLevel + strategy order.
    ]
}

// --- TreeNode ---

class TreeNode :: BaseTreeNode
{
    object theLeft.
    object theRight.

    append : aNode
    [
        if (nil == theLeft)
            [ theLeft := aNode ];
            [
                if (nil == theRight)
                    [ theRight := aNode ];
                    [ theRight := theRight + aNode ]
            ]
    ]
    
    value
    [
        if (nil == theLeft)
            [ InvalidArgumentException new; raise ].
        
        if (nil == theRight)
            [ InvalidArgumentException new; raise ].
        
        ^ theStrategy evalNode(theLeft value, theRight value)
    ]
    
    constructor new:aStrategy baseLevel:baseLevel
        <= new:aStrategy baseLevel:baseLevel;
    [
    ]    
}

// --- TreeLeaf ---

class TreeLeaf :: BaseTreeNode
{
    object theToken.
    object theFunction.

    setFunction : function
    [
        theFunction := function.
    ]

    newArgument
    [
        if (nil == theFunction)
            [ InvalidArgumentException new; raise ].
            
        ^ theFunction eval
    ]

    appendContent : aChar
    [
        theToken append:aChar
    ]

    value = theStrategy evalToken:theToken.
            
    constructor new:aStrategy baseLevel:baseLevel
        <= new:aStrategy baseLevel:baseLevel;
    [
        theToken := String new.
    ]
}

// --- FunctionNode ---

class FunctionNode :: TreeLeaf
{
    object theArg1.
    object theArg2.
    
    object theArgState.
    
    value
    [
        var subj := Signature new(theToken).
        
        if (nil == theArg1)
            [ ^ theStrategy~subj get ].
            
        if (nil == theArg2)
            [ ^ theStrategy~subj eval(theArg1 value) ].
            
        ^ theStrategy~subj eval(theArg1 value, theArg2 value).
    ]

    first_arg:node
    [
        if (nil == theArg1)
            [ theArg1 := node setFunction:self ];
            [ theArg1 := theArg1 + node setFunction:self ]
    ]

    second_arg:node
    [
        if (nil == theArg2)
            [ theArg2 := node setFunction:self ];
            [ theArg2 := theArg2 + node setFunction:self ]
    ]
        
    first_arg
    [
        theArgState := %second_arg.
    ]    
        
    second_arg
    [
        theArgState := %third_arg.
    ]    

    eval
        => theArgState.

    append : aNode
        => theArgState.
        
    constructor new:aStrategy baseLevel:baseLevel
        <= new:aStrategy baseLevel:baseLevel;
    [
        theArgState := %first_arg
    ]
}

// --- StateMachine ---

class StateMachine
{
    object theStates.
    object theCurrentState.
    object theDerivationTree.
    
    int prop level :: theLevel.

    constructor new : states
    [
        theDerivationTree := DerivationTree new.
        theStates := states.
        theLevel := 0
    ]

    generic eval
    [
        theCurrentState := theStates~received get
    ]

    opening : ch
    [
        theLevel := theLevel + 10.
        
        self~theCurrentState opening:ch
    ]

    closing : ch
    [
        theLevel := theLevel - 10.
        
        if (0 > theLevel)
            [ InvalidOperationException new; raise ].
        
        self~theCurrentState closing:ch
    ]

    comma : ch
    [
        theDerivationTree newArgument.
        
        self~theCurrentState comma:ch
    ]

    invalidate
    [
        theDerivationTree := nil
    ]

    func1 = (:aChar)
    [
        var type := aChar lexicalType.

        self~type eval:aChar.
    ].

    value
    [
        var aValue := nil.
        
        if (nil == theDerivationTree)
            [ aValue := "Invalid expression" ];
            [
                aValue := theDerivationTree value
                    | back:"Invalid expression"
            ].
            
        ^ aValue
    ]

    newLeaf : strategy
    [
        theDerivationTree append:(TreeLeaf new:strategy baseLevel:theLevel).
    ]

    newNode : strategy
    [
        theDerivationTree append:(TreeNode new:strategy baseLevel:theLevel).
    ]

    newFunction : strategy
    [
        theDerivationTree append:(FunctionNode new:strategy baseLevel:theLevel).
    ]
    
    addContent : content
    [
        theDerivationTree last_node; appendContent:content
    ]
    
    dispatch => theCurrentState.
}

// --- parserOp ---

extension parserOp
{
    evaluatedWith:states
    [
        var aStateMachine := StateMachine new:states; state0.
        
        try (self forEach(aStateMachine func1))
        {
            on(Exception e) [
                console printLine:e.
                
                aStateMachine invalidate
            ]
        }.
        
        ^ aStateMachine value
    ]
}
