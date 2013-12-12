// --- namespace shortcuts ---
#define system.
//
////#symbol Terminator = 0.
//
//// --- Syntax Error ---
//
//#symbol SyntaxError
//    = "Invalid syntax".
//
//////#symbol ExpressionError
//////    = "Invalid expression".
//////
//////// --- Token Factories ---
//////
//////#symbol ESubExpression =
//////{
//////    create : aChar
//////    [
//////        #if(40 == aChar)?
//////            [ ^ parser'SubExpression. ]
//////            | [ ^ $next create:aChar. ].
//////    ]
//////}.
//////
//////#class ENumeric
//////{
//////    #field theToken.
//////    
//////    #method new
//////    [
//////        theToken := String.
//////    ]
//////    
//////    #method create : aChar
//////    [
//////        ^ $self if : aChar | back:($next create:aChar).
//////    ]
//////    
//////    #method += aChar
//////    [
//////        #if $self if : aChar
//////            [ theToken += aChar. ]
//////        | [ 
//////            $next += Number::theToken.
//////            
//////            theToken clear.
//////        
//////            self~egroup group_member_replace &from:$self &to:parser'TokenFactory.
//////            
//////            self += aChar.
//////        ].
//////    ]
//////    
//////    #method if : aChar
//////    [
//////        Control if:(48 <= aChar) if:(57 >= aChar) | if:(46 == aChar).
//////    ]
//////}
//////
//////#symbol EOperation : anOperation =
//////{
//////    += aChar
//////    [
//////        $next += anOperation.
//////    
//////        self~egroup group_member_replace &from:$self &to:parser'TokenFactory.
//////    ]
//////}.
//////
//////#symbol ESummary =
//////{
//////    create : aChar
//////    [
//////        #if(43 == aChar)?
//////        [
//////            ^ EOperation::Sum.
//////        ]
//////        | [ ^ $next create:aChar. ].
//////    ]
//////}.
//////
//////#symbol EDifference =
//////{
//////    create : aChar
//////    [
//////        #if(45 == aChar)?
//////        [
//////            ^ EOperation::Difference.
//////        ]
//////        | [ ^ $next create:aChar. ].
//////    ]
//////}.
//////
//////#symbol EProduct =
//////{
//////    create : aChar
//////    [
//////        #if(42 == aChar)?
//////        [
//////            ^ EOperation::Product.
//////        ]
//////        | [ ^ $next create:aChar. ].
//////    ]
//////}.
//////
//////#symbol EFraction =
//////{
//////    create : aChar
//////    [
//////        #if(47 == aChar)?
//////        [
//////            ^ EOperation::Fraction.
//////        ]
//////        | [ ^ $next create:aChar. ].
//////    ]
//////}.
//////
//////// --- SubExpressionExtension ---
//////
//////#class SubExpression
//////{
//////    #field theCounter.
//////    #field theSubExpression.
//////
//////    #role Closed
//////    {
//////        #method += aChar 
//////        [ 
//////            $next += $self.
//////        
//////            self~egroup group_member_replace &from:$self &to:parser'TokenFactory.
//////            
//////            self += aChar.
//////        ]
//////    }
//////
//////    #method += aChar
//////    [
//////        #if Int32Value::aChar
//////            == 40 ? [ theCounter += 1. ]
//////            | == 41 ?
//////            [
//////                theCounter -= 1.
//////                
//////                #if (theCounter==0)?
//////                    [ #shift Closed. ^ $self. ].
//////            ]
//////            | [ theSubExpression += aChar. ].
//////    ]
//////
//////    #method new
//////    [
//////        theCounter := Integer::0.
//////        theSubExpression := String.
//////    ]
//////    
//////    #method save : aWriter = parser'ExpressionFactory::theSubExpression save:aWriter.
//////
//////    #method node_level = 0.
//////}
//
//// --- DerivationTree ---
//
//#role ETreeOps
//{
//    #method + aNode
//    [
//        #if (self order > aNode order)?
//        [
//            self += aNode.
//            
//            ^ self.
//        ]
//        | [
//            aNode += self.
//            
//            ^ aNode.
//        ].
//    ]
//}

#class DerivationTree
{
    #field theRoot.
    #field theLast.

//    #role Empty
//    {
//        #method numeric [ $self fail. ]
//        
//        #method last_node = nil.
//        
//        #method top_node = nil.
//
//        #method += aNode
//        [
//            theRoot := aNode.
//            theLast := aNode.
//            
//            #shift.
//        ]
//    }
//
//    #method last_node = theLast.
//    
//    #method top_node = theRoot.
//
//    #method numeric = theRoot numeric.
//
//    #method += aNode
//    [
//        theLast := aNode.
//        
//        theRoot := theRoot~ETreeOps + aNode.
//    ]

    #method clear
    [
//        #shift Empty.
    ]

    #constructor new
    [
    ]
}

//// --- TreeNode ---
//
//#class TreeBiNode
//{
//    #field theLeft.
//    #field theRight.
//    
//    #field theOperation.
//
//    #role RightAssigned
//    {
//        #method left_node = theLeft.
//        
//        #method right_node = theRight.
//        
//        #method += aToken
//	    [
//            theRight := theRight~ETreeOps + aToken.
//        ]
//    }
//
//    #role LeftAssigned
//    {
//        #method left_node = theLeft.
//        
//	    #method += aToken
//	    [
//            theRight := aToken.
//            
//            #shift RightAssigned.
//        ]
//    }  
//
//    #method += aToken
//    [
//        theLeft := aToken.
//
//        #shift LeftAssigned.
//    ]
//    
//    #method new : anOperation
//    [
//        theOperation := anOperation.
//    ]
//    
//    #join theOperation.
//}
//
//// --- TreeNode ---
//
//#class TreeNode
//{
//    #field theToken.
//    
//    #method append : aChar
//    [
//        theToken += aChar.
//    ]
//
//    #method numeric = theToken numeric.
//            
//    #method order = theToken order.
//            
//    #method new : aToken
//    [
//        theToken := aToken.
//    ]
//}

// --- StateMachine ---

#class StateMachine
{
    #field theLexicalAnalizer.
    #field theCurrentState.
    #field theDerivationTree.
    
    #constructor new
    [
        theDerivationTree := DerivationTree new.

//        #var State0 := GroupVariable.
//        #var State1 := GroupVariable.
//
//        #var aNewNumeric := __succ(
//            __symbolreference::"std'basic'string",
//            __wrap(__append, __wrap(sys'dynamics'group_member, __symbol(__symbolreference::"sys'dynamics'groupvariable", nil))),
//            __wrap(__append, __echo(sys'dynamics'group_member, std'primitives'ewidestrvariant)),
//            __wrap(__append, __echo(sys'dynamics'group_member, __prop(calc'parser'order, ext'patterns'egetprop, 0)))).
//             
//        #var n := aNewNumeric invoke.
//                                              
//        State1 group
//            += __prop(calc'math'digit, __eval, 
//                    __wrap(__append, 
//                        __succ(
//                            __symbol(__wrap(__get, __wrap(calc'parser'last_node, calc'parser'DerivationTree)), nil),
//                            __wrap(ext'patterns'eback, State1)))).
//
//        State1 group
//            += __prop(calc'math'plus, __eval, 
//                    __wrap(__append, 
//                        __succ(
//                            __wrap(ext'patterns'esymbol, __succ(__symbolreference::"calc'math'esumming", __symbolreference::"calc'parser'treebinode")),
//                            calc'parser'DerivationTree, 
//                            __wrap(ext'patterns'eback, State0)))).
//                
//        State0 group
//            += __prop(calc'math'digit, __eval, 
//                    __wrap(__append, 
//                        __succ(
//                            __symbol(__succ(aNewNumeric, __symbolreference::"calc'parser'treenode"), nil),
//                            calc'parser'DerivationTree, 
//                            __wrap(ext'patterns'eback, State1)))).
//                                    
//        theCurrentState := State0.
//        
//        theLexicalAnalizer := math'ELexicalAnalizer.
    ]

    #method clear
    [
        theDerivationTree clear.
    ]

    #method eval : aChar
    [
//        #var aSubject := theLexicalAnalizer find:aChar.
//        
//        theCurrentState := theCurrentState~aSubject eval:aChar.
    ]

//    #method numeric
//    [
//        // append end of expression
//        
//        // return the result
//        ^ DerivationTree numeric.
//    ]
}

// --- Parser ---

#class Parser
{
    #field theStateMachine.
    
    #constructor new
    [
        theStateMachine := StateMachine new.
    ]
    
    #method eval &type'widestr:aLiteral
    [
        theStateMachine clear.
        
        control foreach:aLiteral &do:theStateMachine.
        
        ^ theStateMachine Number.
    ]
}