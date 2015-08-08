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
        #var aLevel := Integer new:0.
        
        aLiteral run &each: ch
        [
            #var node := Dynamic new.
            
            ch =>
                #43 ? [ node set &level:(aLevel + 1) set &operation:%add. ]       // +
                #45 ? [ node set &level:(aLevel + 1) set &operation:%subtract. ]  // -
                #42 ? [ node set &level:(aLevel + 2) set &operation:%multiply. ]  // *
                #47 ? [ node set &level:(aLevel + 2) set &operation:%divide. ]    // /
                #40 ? [ aLevel += 10. ^ $self. ] // (
                #41 ? [ aLevel -= 10. ^ $self. ] // )
                ! [
                    node set &leaf:(ch literal toReal) set &level:((aLevel + 3)).
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
                            #var aTop := theTree.
                            #loop (($nil != aTop right)and:[aTop right level < node level] )
                                ? [ aTop := aTop right. ].
                            
                            node set &left:(aTop right) set &right:$nil.
                            
                            aTop set &right:node.
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
    #var exp := ExpressionTree new:"4-(2-1)".
    
    #var r := exp value.
].