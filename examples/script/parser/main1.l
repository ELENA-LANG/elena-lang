import extensions;
import extensions'text;
import system'routines;

class Node
{
    prop string Text;
    prop Node   Left;
    prop Node   Right;
    
    constructor()
    {
    }    
    constructor new(string text)
    {
        this Text := text;
    }
    constructor new(Node node)
    {
        this Left := node;
    }
    
    append(Node node)
    {
        if (Left == nil)
        {
            Left := node
        }
        else 
        {
            Right := node
        }
    }
    
    eval()
    {
        if (Text==nil)
        {
            ^ Left.eval()
        }
        else
        {
            Text =>
                "+" { ^ Left.eval() + Right.eval() }
                "-" { ^ Left.eval() - Right.eval() }
                "*" { ^ Left.eval() * Right.eval() }
                "/" { ^ Left.eval() / Right.eval() }
                : { ^ Text.toReal() }
        }        
    }
}

buildTree(TokenEnumerator reader)
{
    Node node := new Node();
    while (reader.next()) {
        string token := reader.get();
        if (token == "(") {
            node.append(buildTree(reader));
        }
        else if (token == ")") {
            ^ node;
        }
        else if (token == ",") {
           node := Node.new(node);
        }
        else {
            node.Text := token;
        }
    };
    
    ^ node          
}

public program()
{
    auto engine := new extensions'scripting'ScriptEngine();
    engine.loadPath("grammar1.es");
    
    auto s := engine.loadPathAsText("script1.txt");
    
    Node root := buildTree(new TokenEnumerator(s, new ExpressionStatemachine()));
    
    console.printLine(s,"=",root.eval());
}