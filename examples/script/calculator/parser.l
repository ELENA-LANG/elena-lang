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

    LastNode = theLast;
    
    get Value() => theRoot;

    append(node)
    {
        theLast := node;

        theRoot := theRoot + node;
    }

    newArgument()
    {
        theLast := theLast.newArgument()
    }

    constructor()
    {
        theRoot := new BaseTreeNode:{ Level = -1; add(node) = node; }
    }
}

// --- BaseTreeNode ---

class BaseTreeNode
{
    theStrategy;
    theLevel;
    
    Level = theLevel;

    add(node)
    {
        if (self.Level < node.Level)
        {
            self.append(node);
            
            ^ self
        }
        else
        {
            node.append(self);
            
            ^ node
        }
    }

    setFunction(function) {}
        
    constructor(strategy, baseLevel)
    {
        theStrategy := strategy;
        theLevel := baseLevel + strategy.Order()
    }
}

// --- TreeNode ---

class TreeNode : BaseTreeNode
{
    theLeft;
    theRight;

    append(node)
    {
        if (nil == theLeft)
        { 
            theLeft := node 
        }
        else
        {
            if (nil == theRight)
            { 
                theRight := node 
            }
            else
            { 
                theRight := theRight + node 
            }
        }
    }
    
    get Value()
    {
        if (nil == theLeft)
            { InvalidArgumentException.raise() };
        
        if (nil == theRight)
            { InvalidArgumentException.raise() };
        
        ^ theStrategy.evalNode(theLeft.Value, theRight.Value)
    }
    
    constructor(strategy, baseLevel)
        <= (strategy, baseLevel);
}

// --- TreeLeaf ---

class TreeLeaf : BaseTreeNode
{
    theToken;
    theFunction;

    setFunction(function)
    {
        theFunction := function
    }

    newArgument()
    {
        if (nil == theFunction)
            { InvalidArgumentException.raise() };
            
        ^ theFunction.eval();
    }

    appendContent(ch)
    {
        theToken.append(ch)
    }

    Value = theStrategy.evalToken(theToken);
            
    constructor(strategy, baseLevel)
        <= (strategy, baseLevel)
    {
        theToken := new StringWriter()
    }
}

// --- FunctionNode ---

class FunctionNode : TreeLeaf
{
    theArg1;
    theArg2;
    
    theArgState;
    
    get Value()
    {
        var subj := new MessageName(cast string(theToken));
        
        if (nil == theArg1) {
            ^ mixin subj(theStrategy).eval()
        }
        else if (nil == theArg2) {
            ^ mixin subj(theStrategy).eval(theArg1.Value)
        }
        else {
            ^ mixin subj(theStrategy).eval(theArg1.Value, theArg2.Value)
        }
    }

    first_arg(node)
    {
        if (nil == theArg1)
        { 
            theArg1 := node.setFunction(self) 
        }
        else
        { 
            theArg1 := theArg1 + node.setFunction(self) 
        }
    }

    second_arg(node)
    {
        if (nil == theArg2)
        { 
            theArg2 := node.setFunction(self) 
        }
        else
        { 
            theArg2 := theArg2 + node.setFunction(self) 
        }
    }
        
    first_arg()
    {
        theArgState := subjconst second_arg
    }    
        
    second_arg()
    {
        theArgState := subjconst third_arg
    }    

    eval()
        => theArgState;

    append(node)
        => theArgState;
        
    constructor(strategy, baseLevel)
        <= (strategy, baseLevel)
    {
        theArgState := subjconst first_arg;
    }
}

// --- StateMachine ---

class StateMachine
{
    theStates;
    theCurrentState;
    theDerivationTree;
    
    prop int Level;

    constructor assign(states)
    {
        theDerivationTree := new DerivationTree();
        theStates := states;
        Level := 0
    }

    generic()
    {
        theCurrentState := mixin __received(theStates).eval()
    }

    opening(ch)
    {
        Level := Level + 10;
        
        mixin theCurrentState(self).opening:ch
    }

    closing(ch)
    {
        Level := Level - 10;
        
        if (0 > Level)
            { InvalidOperationException.raise() };
        
        mixin theCurrentState(self).closing(ch)
    }

    comma(ch)
    {
        theDerivationTree.newArgument();
        
        mixin theCurrentState(self).comma(ch)
    }

    invalidate()
    {
        theDerivationTree := nil
    }

    Func = (ch)
    {
        var type := ch.LexicalType;

        mixin type(self).eval:ch
    };

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

    newLeaf(strategy)
    {
        theDerivationTree.append(new TreeLeaf(strategy, Level));
    }

    newNode(strategy)
    {
        theDerivationTree.append(new TreeNode(strategy, Level));
    }

    newFunction(strategy)
    {
        theDerivationTree.append(new FunctionNode(strategy, Level))
    }
    
    addContent(content)
    {
        theDerivationTree.LastNode.appendContent(content)
    }
    
    dispatch() => theCurrentState;
}

// --- parserOp ---

extension parserOp
{
    evaluatedWith(states)
    {
        var stateMachine := StateMachine.assign(states).State0();
        
        try
        {
            self.forEach(stateMachine.Func)
        }
        catch(Exception e)
        {
            console.printLine(e);
            
            stateMachine.invalidate()
        };
        
        ^ stateMachine.Value
    }
}
