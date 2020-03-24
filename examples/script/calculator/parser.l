import system'routines;
import system'collections;
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

    newArgument()
    {
        theLast := theRoot.newArgument()
    }

    constructor()
    {
        theRoot := new BaseTreeNode{ this Level := -1; add(node) = node; }
    }
}

// --- BaseTreeNode ---

class BaseTreeNode
{
    theFunction;
    
    prop int Level;
    
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

    constructor new(level,function)
    {
        Level := level;
        theFunction := function
    }
    
    constructor new(level)
    {
        Level := level
    }
}

// --- TreeNode ---

class TreeNode : BaseTreeNode
{
    theLeft;
    theRight;

    newArgument()
    {
        if (nil == theRight)
        {
            ^ theRight.newArgument()
        }
        else
        {
            ^ theLeft.newArgument()
        }
    }

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
        
        ^ theFunction(theLeft.Value, theRight.Value)
    }
    
    constructor new(baseLevel, function)
        <= new(baseLevel, function);
}

// --- TreeLeaf ---

class TreeLeaf : BaseTreeNode
{
    theToken;

    appendContent(ch)
    {
        theToken.append(ch)
    }

    Value = theFunction(theToken.Value);
            
    constructor new(level, function)
        <= new(level,function)
    {
        theToken := new StringWriter();
    }
    
    constructor new(level)
        <= new(level)
    {
        theToken := new StringWriter();
    }
}

// --- FunctionNode ---

class FunctionNode : TreeLeaf
{
    List theArgNodes;
    
    newArgument()
    {
        theArgNodes.append(nil);
        
        ^ self
    }
    
    append(node)
    {
        int last := theArgNodes.Length - 1;
        
        var lastNode := theArgNodes[last];
        if (lastNode == nil)
        {
            theArgNodes[last] := node
        }
        else
        {
            theArgNodes[last] := theArgNodes[last] + node
        }
    }
    
    get Value()
    {
        object[] args := new object[](theArgNodes.Length);
        for (int i := 0, i < args.Length, i+=1) {
            args[i] := theArgNodes[i].Value
        };
        
        var symbol := new Symbol(theToken.Value);
        
        ^ symbol(inlinearg args);
    }
        
    constructor new(baseLevel)
        <= new(baseLevel)
    {
        theArgNodes := new List();
        theArgNodes.append(nil);
        
        theToken.append("extensions'math'");
    }
}

// --- StateMachine ---

class StateMachine
{
    theStates;
    theCurrentState;
    theDerivationTree;
    theLeaf;
    theFunction;
    
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
        MessageName mssg := __received.MessageName;
        Message prop := mssg.getPropertyMessage();
        
        theCurrentState := prop(theStates);
    }

    incLevel(diff)
    {
        level := level + diff
    }

    decLevel(diff)
    {
        level := level - diff
    }

    newArgument()
    {
        theDerivationTree.newArgument()
    }

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

    newNumberLeaf(leafLevel)
    {
        theLeaf := TreeLeaf.new(level + leafLevel, mssgconst toReal<convertorOp>[1]);
        
        theDerivationTree.append(theLeaf)
    }
    
    newFunctionLeaf(leafLevel)
    {
        theLeaf := FunctionNode.new(level + leafLevel);
        
        theDerivationTree.append(theLeaf)
    }
    
    appendContent(ch)
    {
        theLeaf.appendContent(ch)
    }

    newNode(nodeLevel, operator)
    {
        var function;
        operator =>
            "+" { function := mssgconst add[2]}
            "-" { function := mssgconst subtract[2]}        
            "*" { function := mssgconst multiply[2]}
            "/" { function := mssgconst divide[2]};
            
        theDerivationTree.append(TreeNode.new(level + nodeLevel, function));
    }

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
