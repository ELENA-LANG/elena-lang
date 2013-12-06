#define system.
#define system'text.

// --- MatrixException ---

#class MatrixException : Exception
{
}

// --- EMatrix ---

#class(role) EMatrix
{
    #method add : aMatrix
    [
        #var aCount := self type'length.

        ^ Array new &type'length:aCount &function: &&:i
            [ Array new &type'length:aCount &function: &&:j 
                [ self@i@j + aMatrix@i@j ] ].
    ]
    
    #method subtract : aMatrix
    [
        #var aCount := self type'length.

        ^ Array new &type'length:aCount &function: &&:i
            [ Array new &type'length:aCount &function: &&:j 
                [ self@i@j - aMatrix@i@j ] ].
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
                    aResult@i@j += self@i@k * aMatrix@k@j.
                ].
            ].
        ].
        
        ^aResult.            
    ]

    #method Clone
        = Array new &type'length::self &function: &&:i
            [ (self@i)~EArray DeepClone ].

    #method Decomposed
    [
        #var aCount := self type'length.
        
        // toggle tracks row swaps. +1 -> even, -1 -> odd. used by MatrixDeterminant
        #var even := Boolean new:true.          

        // create matrix copy
        #var a := self~EArray DeepClone.
        
        // permutation matrix
        #var pm := Array new &type'length:aCount &function: &&:i [ i ].
        
        // each column, j is counter for coulmns
        #var max_row := Integer new:0.
        #var max := Real new:0.0r.
        control from:0 &till:(aCount - 1) &do: &&:j
        [
            // find largest value in col j
            max << (abs:(a@j@j)).
            max_row << j.
            
            control from:(j + 1) &till:aCount &do: &&:i
            [
                #var val := abs:(a@i@j).
                (val > max)
                    ? [
                        max << val.
                        max_row << i.
                    ].
            ].
            
            // if largest value not on pivot, swap rows
            (max_row != j) ?
            [
                control from:0 &till:aCount &do: &&:k
                [ 
                    #var tmp := a@j@k.
                    
                    (a@j) setAt:k:(a@max_row@k).
                    (a@max_row) setAt:k:tmp.
                ].
                    
                 // switch two rows in permutation matrix
                 pm~EArray exchange:j:max_row.
                 
                 even invert.
            ].
                        
            (0.0r == max)
                ? [ #throw MatrixException new:"The matrix is singular!". ].

            #var val := a@j@j.
            control from:(j + 1) &till:aCount &do: &&:i
            [
                (a@i)setAt:j:((a@i@j) / val).
                
                control from:(j+1) &till:aCount &do: &&:k
                [
                    (a@i)setAt:k:((a@i@k) - (a@i@j) * (a@j@k)).
                ].
            ]
        ].
        
        ^ Extension(a, %Even(even Value)).
    ]

    #method Determinant
    [
        #var lum := self~EMatrix Decomposed.
        #var deter := Real new:1.

        (lum Even)
            ! [ deter *= -1. ].
        
        control from:0 &till:(self type'length) &do: &&:i
            [ deter << deter * (lum@i@i). ].
            
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