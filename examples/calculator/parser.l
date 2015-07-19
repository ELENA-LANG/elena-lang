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

////////#symbol Terminator = 0.
//////
//////// --- Syntax Error ---
//////
//////#symbol SyntaxError
//////    = "Invalid syntax".
//////
//////////#symbol ExpressionError
//////////    = "Invalid expression".
//////////
//////////// --- Token Factories ---
//////////
//////////#symbol ESubExpression =
//////////{
//////////    create : aChar
//////////    [
//////////        #if(40 == aChar)?
//////////            [ ^ parser'SubExpression. ]
//////////            | [ ^ $next create:aChar. ].
//////////    ]
//////////}.
//////////
//////////#class ENumeric
//////////{
//////////    #field theToken.
//////////    
//////////    #method new
//////////    [
//////////        theToken := String.
//////////    ]
//////////    
//////////    #method create : aChar
//////////    [
//////////        ^ $self if : aChar | back:($next create:aChar).
//////////    ]
//////////    
//////////    #method += aChar
//////////    [
//////////        #if $self if : aChar
//////////            [ theToken += aChar. ]
//////////        | [ 
//////////            $next += Number::theToken.
//////////            
//////////            theToken clear.
//////////        
//////////            self~egroup group_member_replace &from:$self &to:parser'TokenFactory.
//////////            
//////////            self += aChar.
//////////        ].
//////////    ]
//////////    
//////////    #method if : aChar
//////////    [
//////////        Control if:(48 <= aChar) if:(57 >= aChar) | if:(46 == aChar).
//////////    ]
//////////}
//////////
//////////#symbol EOperation : anOperation =
//////////{
//////////    += aChar
//////////    [
//////////        $next += anOperation.
//////////    
//////////        self~egroup group_member_replace &from:$self &to:parser'TokenFactory.
//////////    ]
//////////}.
//////////
//////////#symbol ESummary =
//////////{
//////////    create : aChar
//////////    [
//////////        #if(43 == aChar)?
//////////        [
//////////            ^ EOperation::Sum.
//////////        ]
//////////        | [ ^ $next create:aChar. ].
//////////    ]
//////////}.
//////////
//////////#symbol EDifference =
//////////{
//////////    create : aChar
//////////    [
//////////        #if(45 == aChar)?
//////////        [
//////////            ^ EOperation::Difference.
//////////        ]
//////////        | [ ^ $next create:aChar. ].
//////////    ]
//////////}.
//////////
//////////#symbol EProduct =
//////////{
//////////    create : aChar
//////////    [
//////////        #if(42 == aChar)?
//////////        [
//////////            ^ EOperation::Product.
//////////        ]
//////////        | [ ^ $next create:aChar. ].
//////////    ]
//////////}.
//////////
//////////#symbol EFraction =
//////////{
//////////    create : aChar
//////////    [
//////////        #if(47 == aChar)?
//////////        [
//////////            ^ EOperation::Fraction.
//////////        ]
//////////        | [ ^ $next create:aChar. ].
//////////    ]
//////////}.
//////////
//////////// --- SubExpressionExtension ---
//////////
//////////#class SubExpression
//////////{
//////////    #field theCounter.
//////////    #field theSubExpression.
//////////
//////////    #role Closed
//////////    {
//////////        #method += aChar 
//////////        [ 
//////////            $next += $self.
//////////        
//////////            self~egroup group_member_replace &from:$self &to:parser'TokenFactory.
//////////            
//////////            self += aChar.
//////////        ]
//////////    }
//////////
//////////    #method += aChar
//////////    [
//////////        #if Int32Value::aChar
//////////            == 40 ? [ theCounter += 1. ]
//////////            | == 41 ?
//////////            [
//////////                theCounter -= 1.
//////////                
//////////                #if (theCounter==0)?
//////////                    [ #shift Closed. ^ $self. ].
//////////            ]
//////////            | [ theSubExpression += aChar. ].
//////////    ]
//////////
//////////    #method new
//////////    [
//////////        theCounter := Integer::0.
//////////        theSubExpression := String.
//////////    ]
//////////    
//////////    #method save : aWriter = parser'ExpressionFactory::theSubExpression save:aWriter.
//////////
//////////    #method node_level = 0.
//////////}
//////
//////// --- DerivationTree ---
//////
//////#role ETreeOps
//////{
//////    #method + aNode
//////    [
//////    ]
//////}

#class DerivationTree
{
    #field theRoot.
    #field theLast.

//////    #role Empty
//////    {
//////        #method numeric [ $self fail. ]
//////        
//////        #method last_node = nil.
//////        
//////        #method top_node = nil.
//////
//////        #method += aNode
//////        [
//////            theRoot := aNode.
//////            theLast := aNode.
//////            
//////            #shift.
//////        ]
//////    }

    #method last_node = theLast.
    
//////    #method top_node = theRoot.

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

        aState0 write:{ digit : aChar : aTree [ aTree append:(TreeLeaf new:aNumberStratagy append:aChar). ^ aState1. ] }.

        aState1 write:
            { 
                plus : aChar : aTree [ aTree append:(TreeNode new:aSumStratagy). ^ aState1. ] 
                
                digit : aChar : aTree [ aTree append:(TreeLeaf new:aNumberStratagy append:aChar). ^ aState0. ]
            }.


//////        #var aNewNumeric := __succ(
//////            __symbolreference::"std'basic'string",
//////            __wrap(__append, __wrap(sys'dynamics'group_member, __symbol(__symbolreference::"sys'dynamics'groupvariable", nil))),
//////            __wrap(__append, __echo(sys'dynamics'group_member, std'primitives'ewidestrvariant)),
//////            __wrap(__append, __echo(sys'dynamics'group_member, __prop(calc'parser'order, ext'patterns'egetprop, 0)))).
//////             
//////        #var n := aNewNumeric invoke.
//////                                              
//////        State1 group
//////            += __prop(calc'math'digit, __eval, 
//////                    __wrap(__append, 
//////                        __succ(
//////                            __symbol(__wrap(__get, __wrap(calc'parser'last_node, calc'parser'DerivationTree)), nil),
//////                            __wrap(ext'patterns'eback, State1)))).
//////
//////        State1 group
//////            += __prop(calc'math'plus, __eval, 
//////                    __wrap(__append, 
//////                        __succ(
//////                            __wrap(ext'patterns'esymbol, __succ(__symbolreference::"calc'math'esumming", __symbolreference::"calc'parser'treebinode")),
//////                            calc'parser'DerivationTree, 
//////                            __wrap(ext'patterns'eback, State0)))).
//
//
////        State write:Struct(%digit, 
////            Tape(1, extensions'control, %(getAt&args)(1), String, %new(1), TreeLeaf, %new(1))).
//
//////        State0 group
//////            += __prop(calc'math'digit, __eval, 
//////                    __wrap(__append, 
//////                        __succ(
//////                            __symbol(__succ(aNewNumeric, __symbolreference::"calc'parser'treenode"), nil),
//    ////                            calc'parser'DerivationTree, 
//////                            __wrap(ext'patterns'eback, State1)))).
//                                    
        theCurrentState := aState0.
//        
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
