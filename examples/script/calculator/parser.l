import system'routines;
import extensions;
import extensions'text;

extension lexicalOp
{
    get LexicalType()
    {
        if (self >= $48 && self <= $57)
            { ^ subjconst digit };

        if (self >= $97 && self <= $122)
            { ^ subjconst letter };

        self =>
            $32 { ^ subjconst whitespace }
            $40 { ^ subjconst opening }
            $41 { ^ subjconst closing }
            $42 { ^ subjconst star }
            $43 { ^ subjconst plus }
            $44 { ^ subjconst comma }
            $45 { ^ subjconst minus }
            $46 { ^ subjconst dot }
            $47 { ^ subjconst slash }
            : { ^ subjconst unknown }
    }
}

class DerivationTree
{
    theRoot;
    theLast;

//    LastNode = theLast;
    
    get Value() => theRoot;

    append(node)
    {
        theLast := node;

        theRoot := theRoot + node;
    }

//    newArgument()
//    {
//        theLast := theLast.newArgument()
//    }

    constructor()
    {
        theRoot := new BaseTreeNode::{ this Level := -1; add(node) = node; }
    }
}

// --- BaseTreeNode ---

class BaseTreeNode
{
    prop int Level;
    
//    add(node)
//    {
//        if (self.Level < node.Level)
//        {
//            self.append(node);
//            
//            ^ self
//        }
//        else
//        {
//            node.append(self);
//            
//            ^ node
//        }
//    }
//
//    setFunction(function) {}
        
    constructor new(level)
    {
        Level := level
    }
}

//// --- TreeNode ---
//
//class TreeNode : BaseTreeNode
//{
//    theLeft;
//    theRight;
//
//    append(node)
//    {
//        if (nil == theLeft)
//        { 
//            theLeft := node 
//        }
//        else
//        {
//            if (nil == theRight)
//            { 
//                theRight := node 
//            }
//            else
//            { 
//                theRight := theRight + node 
//            }
//        }
//    }
//    
//    get Value()
//    {
//        if (nil == theLeft)
//            { InvalidArgumentException.raise() };
//        
//        if (nil == theRight)
//            { InvalidArgumentException.raise() };
//        
//        ^ theStrategy.evalNode(theLeft.Value, theRight.Value)
//    }
//    
//    constructor(strategy, baseLevel)
//        <= (strategy, baseLevel);
//}

// --- TreeLeaf ---

class TreeLeaf : BaseTreeNode
{
    theToken;
    theFunction;

//    setFunction(function)
//    {
//        theFunction := function
//    }
//
//    newArgument()
//    {
//        if (nil == theFunction)
//            { InvalidArgumentException.raise() };
//            
//        ^ theFunction.eval();
//    }

    appendContent(ch)
    {
        theToken.append(ch)
    }

    Value = theFunction(theToken.Value);
            
    constructor(level, function)
        <= new(level)
    {
        theToken := new StringWriter();
        theFunction := function
    }
}

//// --- FunctionNode ---
//
//class FunctionNode : TreeLeaf
//{
//    theArg1;
//    theArg2;
//    
//    theArgState;
//    
//    get Value()
//    {
//        var subj := new MessageName(cast string(theToken));
//        
//        if (nil == theArg1) {
//            ^ mixin subj(theStrategy).eval()
//        }
//        else if (nil == theArg2) {
//            ^ mixin subj(theStrategy).eval(theArg1.Value)
//        }
//        else {
//            ^ mixin subj(theStrategy).eval(theArg1.Value, theArg2.Value)
//        }
//    }
//
//    first_arg(node)
//    {
//        if (nil == theArg1)
//        { 
//            theArg1 := node.setFunction(self) 
//        }
//        else
//        { 
//            theArg1 := theArg1 + node.setFunction(self) 
//        }
//    }
//
//    second_arg(node)
//    {
//        if (nil == theArg2)
//        { 
//            theArg2 := node.setFunction(self) 
//        }
//        else
//        { 
//            theArg2 := theArg2 + node.setFunction(self) 
//        }
//    }
//        
//    first_arg()
//    {
//        theArgState := subjconst second_arg
//    }    
//        
//    second_arg()
//    {
//        theArgState := subjconst third_arg
//    }    
//
//    eval()
//        => theArgState;
//
//    append(node)
//        => theArgState;
//        
//    constructor(strategy, baseLevel)
//        <= (strategy, baseLevel)
//    {
//        theArgState := subjconst first_arg;
//    }
//}

// --- StateMachine ---

class StateMachine
{
    theStates;
    theCurrentState;
    theDerivationTree;
    theLeaf;
    
    int level;

    private init()
    {
        theDerivationTree := new DerivationTree();
        theCurrentState := theStates.State0;
        
        level := 0
    }

    constructor assign(states)
    {
        theStates := states;
        
        self.init()
    }

    get generic()
    {
        theCurrentState := (__received.getPropertyMessage())(theStates);
    }

//    opening(ch)
//    {
//        Level := Level + 10;
//        
//        mixin theCurrentState(self).opening:ch
//    }
//
//    closing(ch)
//    {
//        Level := Level - 10;
//        
//        if (0 > Level)
//            { InvalidOperationException.raise() };
//        
//        mixin theCurrentState(self).closing(ch)
//    }
//
//    comma(ch)
//    {
//        theDerivationTree.newArgument();
//        
//        mixin theCurrentState(self).comma(ch)
//    }

    invalidate()
    {
        theDerivationTree := nil
    }

    get Value()
    {
        var value := nil;
        
        if (nil == theDerivationTree)
        { 
            value := "Invalid expression" 
        }
        else
        {
            value := theDerivationTree.Value
                | back:"Invalid expression"
        };
            
        ^ value
    }

    newLeaf()
    {
        theLeaf := new TreeLeaf(level, mssgconst toReal<convertorOp>[0]);
        
        theDerivationTree.append(theLeaf)
    }
    
    appendContent(ch)
    {
        theLeaf.appendContent(ch)
    }

//    newLeaf(strategy)
//    {
//        theDerivationTree.append(new TreeLeaf(strategy, Level));
//    }
//
//    newNode(strategy)
//    {
//        theDerivationTree.append(new TreeNode(strategy, Level));
//    }
//
//    newFunction(strategy)
//    {
//        theDerivationTree.append(new FunctionNode(strategy, Level))
//    }
    
    dispatch() => theCurrentState;
}

// --- parserOp ---

extension parserOp
{
    evaluatedWith(states)
    {
        var stateMachine := StateMachine.assign(states);
        
        try
        {
            self.forEach:(ch)
            {
                var type := ch.LexicalType;
        
                type(stateMachine, ch)
            }
        }
        catch(Exception e)
        {
            console.printLine(e);
            
            stateMachine.invalidate()
        };
        
        ^ stateMachine.Value
    }
}
