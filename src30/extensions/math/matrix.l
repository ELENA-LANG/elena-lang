#define system.
#define system'text.

// --- EMatrix ---

#class(role) EMatrix
{
    #method add : aMatrix
    [
        #var aCount := self type'length.

        ^ Array new &type'length:aCount &function: &&:i
            [ Array new &type'length:aCount &function: &&:j 
                [ (self@i@j) + (aMatrix@i@j) ] ].
    ]
    
    #method subtract : aMatrix
    [
        #var aCount := self type'length.

        ^ Array new &type'length:aCount &function: &&:i
            [ Array new &type'length:aCount &function: &&:j 
                [ (self@i@j) - (aMatrix@i@j) ] ].
    ]
    
    #method multiply : aMatrix
    [
        #var aCount := self type'length.

        #var aResult := Array new &type'length:aCount &function: &&:i
            [ Array new &type'length:aCount &function: &&:j [ Real new &type'int:0 ] ].
  
        control from:0 &till:aCount &do: &&:i
        [
            control from:0 &till:aCount &do: &&:j
            [
                control from:0 &till:aCount &do: &&:k
                [
                    (aResult@i@j) += (self@i@k) * (aMatrix@k@j).
                ].
            ].
        ].
        
        ^aResult.            
    ]

    #method Clone
        = Array new &type'length::self &function: &&:i
            [ (self@i)~EArray DeepClone ].

    #method Determinant
    [
        #var aCount := self type'length.

        // create matrix copy
        #var a := self~EArray DeepClone.

        #var deter := Real new:1.

        control from:0 &till:aCount &do: &&:k
            [
                (0.0r == (a@k@k))?
                [
                    #var j := control from:k &till:aCount &do: &&:j [ (0.0r != (a@j@k)) ? [ #break j. ] ].
                    nil == j ?
                        [ deter << 0. #break deter. ].
                    
                    control from:k &till:aCount &do: &&:i
                    [
                        #var temp := Real new:(a@j@i).
                        a@j@i << a@k@i.
                        a@k@i << temp.
                    ].
    
                    deter *= -1.
                ].
                deter *= a@k@k.

                control from:(k + 1) &till:aCount &do: &&:i
                [
                    control from:(k + 1) &till:aCount &do: &&:j
                    [
                        a@i@j -= (a@i@k * a@k@j / a@k@k).
                    ].
                ].
            ].
            
        ^ deter.
    ]
            
    #method Literal
    [
        #var anOutput := TextBuffer new.
        
        control foreach:self &do: &&:aRow
        [
            control foreach:aRow &do: &&:anItem [ anOutput append:(anItem Literal) append:",". ].
            
            anOutput set &type'length:(-1 + anOutput type'length).
            
            anOutput append:"%n".
        ].
        
        ^ anOutput Literal.
    ]
}

// --- matrixControl ---

#symbol matrixControl =
{
    new &m:anM &n:anN &function:anAction
        = Array new &type'length:anM &function: &&:i
            [ Array new &type'length:anN &function: &&:j [ anAction:i:j ] ].
            
    sumOf : aMatrix1 : aMatrix2
        = aMatrix1~EMatrix add:aMatrix2.
            
    differenceOf : aMatrix1 : aMatrix2
        = aMatrix1~EMatrix subtract:aMatrix2.
        
    productOf : aMatrix1 : aMatrix2
        = aMatrix1~EMatrix multiply:aMatrix2.
  
    determinantOf : aMatrix        
        = aMatrix~EMatrix Determinant.
                    
    toLiteral : aMatrix
        = aMatrix~EMatrix Literal.
}.