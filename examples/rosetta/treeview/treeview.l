#import system'routines.
#import extensions.

class Node
{
    object theValue.
    object theChildren.
    
    constructor new : value &children:children
    [
        theValue := value.
                
        theChildren := children toArray.
    ]
    
    constructor new : value
        <= new:value &children:nil.
        
    constructor new &children:children
        <= new:emptyLiteralValue &children:children.
        
    constructor new : value &child:child
        <= new:value &children:(Array new &object:child).
        
    get = theValue.
    
    children = theChildren.
}

extension treeOp
{
    writeTree:node:prefix &subject:childrenProp
    [
        var children := node::childrenProp get.
        var length := children length.
        
        children zip:(RangeEnumerator new &from:1 &to:length) &eachPair:(:child:index)
        [
            self writeLine:prefix:"|".                
            self writeLine:prefix:"+---":(child get).                
            
            var nodeLine := prefix + (index==length)iif:"    ":"|   ".
            
            self writeTree:child:nodeLine &subject:childrenProp.
        ].
        
        ^ self.
    ]
    
    writeTree:node &subject:childrenProp
        = self writeTree:node:"" &subject:childrenProp.
}

program =
[
    var tree := Node new &children:
                 (
                    Node new:"a" &children:
                    (
                        Node new:"b" &child:(Node new:"c"),
                        Node new:"d"
                    ),
                    Node new:"e"
                 ).
                 
    console writeTree:tree &subject:%children readChar.
].
