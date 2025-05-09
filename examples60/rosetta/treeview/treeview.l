/// a program to produce a visual representation of some tree. 

import system'routines;
import extensions;

class Node
{
    string theValue;
    Node[] theChildren;
    
    constructor new(string value, Node[] children)
    {
        theValue := value;
                
        theChildren := children;
    }
    
    constructor new(string value)
        <= new(value, new Node[](0));
        
    constructor new(Node[] children)
        <= new(EmptyString, children);
        
    get() = theValue;
    
    Children = theChildren;
}

extension treeOp
{
    writeTree(node, prefix)
    {
        var children := node.Children;
        var length := children.Length;
        
        children.zipForEach(new Range(1, length), (child,index)
        {
            self.printLine(prefix,"|");
            self.printLine(prefix,"+---",child.get());                
            
            var nodeLine := prefix + (index==length).iif("    ","|   ");
            
            self.writeTree(child,nodeLine);
        });
        
        ^ self
    }
    
    writeTree(node)
        = self.writeTree(node,"");
}

public program()
{
    var tree := Node.new(
                 new Node[]{
                    Node.new("a", new Node[]
                    {
                        Node.new("b", new Node[]{Node.new("c")}),
                        Node.new("d")
                    }),
                    Node.new("e")
                 });
                 
    Console.writeTree(tree).readChar()
}
