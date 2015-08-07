#define system.
#define system'routines.
#define system'dynamic.
#define extensions.

// --- Expression ---

#class ExpressionTree
{
    #field theTree.
    
    #constructor new : aLiteral
    [
        aLiteral run &each: ch
        [
            #var node := Dynamic new.
            
            ch =>
                #43 ? [ node set &level:2 set &operation:%add. ]       // +
                #45 ? [ node set &level:2 set &operation:%subtract. ]  // -
                #42 ? [ node set &level:1 set &operation:%multiply. ]  // *
                #47 ? [ node set &level:1 set &operation:%divide. ]    // /
                ! [
                    node set &leaf:(ch literal toReal) set &level:3.
                ].
                    
            ($nil == theTree)
                ? [ theTree := node. ]
                ! [
                    (theTree level >= node level)
                        ? [
                            node set &left:theTree set &right:$nil.
                            
                            theTree := node.
                        ]
                        ! [
                            node set &left:(theTree right).
                            
                            theTree set &right:node.
                        ].
                ].
        ].
    ]
    
    #method eval : aNode
    [
        (aNode if &leaf)
            ? [ ^ aNode leaf. ]
            ! [
                #var aLeft := $self eval:(aNode left).
                #var aRight := $self eval:(aNode right).
                
                ^ aLeft::(aNode operation) eval:aRight.
            ]
    ]
    
    #method value
        <= eval:theTree.
}

// --- program ---

#symbol program =
[
    #var exp := ExpressionTree new:"2+3".
    
    #var r := exp value.
].