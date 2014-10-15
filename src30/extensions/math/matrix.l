#define system.
#define system'routines.
#define system'dynamic.
#define system'text.

// --- MatrixException ---

#class(type:MattrixError) MatrixException : Exception
{
}

// --- EMatrix ---

#class(role) EMatrix
{
    #method add : aMatrix
    [
        #var aCount := self length.

        ^ arrayControl new &length:(aCount int) &each: i
            [ arrayControl new &length:(aCount int) &each: j 
                [ self@i@j + aMatrix@i@j ] ].
    ]
    
    #method subtract : aMatrix
    [
        #var aCount := self length.

        ^ arrayControl new &length:(aCount int) &each: i
            [ arrayControl new &length:(aCount int) &each: j
                [ self@i@j - aMatrix@i@j ] ].
    ]
    
    #method multiply : aMatrix
    [
        #var aCount := self length.

        #var aResult := arrayControl new &length:(aCount int) &each: i
            [ arrayControl new &length:(aCount int) &each: j [ Real new &int:0 ] ].
  
        control forrange &int:0 &int::(aCount-1) &do: (&int:i)
        [
            control forrange &int:0 &int::(aCount-1) &do: (&int:j)
            [
                control forrange &int:0 &int::(aCount-1) &do: (&int:k)
                [
                    aResult@i@j += self@i@k * aMatrix@k@j.
                ].
            ].
        ].
        
        ^aResult.            
    ]

    #method Clone
        = arrayControl new &length::self &each: i
            [ (self@i)~indexable deepClone ].

    #method Decomposed
    [
        #var(type:int) aCount := self length.
        
        // toggle tracks row swaps. +1 -> even, -1 -> odd. used by MatrixDeterminant
        #var even := Boolean new:true.          

        // create matrix copy
        #var a := self~indexable deepClone.
        
        // permutation matrix
        #var pm := arrayControl new &length:aCount &each: i [ i ].
        
        // each column, j is counter for coulmns
        #var max_row := Integer new:0.
        #var max := Real new:0.0r.
        control forrange &int:0 &int:(aCount - 2) &do: (&int:j)
        [
            // find largest value in col j
            max << (abs:(a@j@j)).
            max_row << j.
            
            control forrange &int:(j + 1) &int::(aCount-1) &do: (&int:i)
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
                control forrange &int:0 &int::(aCount-1) &do: (&int:k)
                [ 
                    #var tmp := a@j@k.
                    
                    (a@j) setAt:k:(a@max_row@k).
                    (a@max_row) setAt:k:tmp.
                ].
                    
                 // switch two rows in permutation matrix
                 pm~indexable exchange:j:max_row.
                 
                 even invert.
            ].
                        
            (0.0r == max)
                ? [ #throw MatrixException new:"The matrix is singular!". ].

            #var val := a@j@j.
            control forrange &int:(j + 1) &int::(aCount-1) &do: (&int:i)
            [
                (a@i)setAt:j:((a@i@j) / val).
                
                control forrange &int::(j+1) &int::(aCount-1) &do: (&int:k)
                [
                    (a@i)setAt:k:((a@i@k) - (a@i@j) * (a@j@k)).
                ].
            ]
        ].
        
        ^ Extension(%Even[ even value ], a).
    ]

    #method Determinant
    [
        #var lum := self~EMatrix Decomposed.
        #var deter := Real new &int:1.

        (lum Even)
            ! [ deter *= -1. ].
        
        control forrange &int:0 &int:(self length - 1) &do: (&int:i)
            [ deter << deter * (lum@i@i). ].
            
       ^ deter.
    ]
            
    #method literal
    [
        #var anOutput := TextBuffer new.
        
        control foreach:self &do: aRow
        [
            control foreach:aRow &do: anItem [ anOutput write:(anItem literal) write:",". ].
            
            anOutput set &length:(-1 + anOutput length).
            
            anOutput write:"%n".
        ].
        
        ^ anOutput literal.
    ]
}

// --- matrixControl ---

#symbol matrixControl =
{
    new &m:anM &n:anN &each:anAction
        = arrayControl new &length:(anM int) &each: i
            [ arrayControl new &length:(anN int) &each: j [ anAction:i:j ] ].
            
    sumOf : aMatrix1 : aMatrix2
        = aMatrix1~EMatrix add:aMatrix2.
            
    differenceOf : aMatrix1 : aMatrix2
        = aMatrix1~EMatrix subtract:aMatrix2.
        
    productOf : aMatrix1 : aMatrix2
        = aMatrix1~EMatrix multiply:aMatrix2.
  
    determinantOf : aMatrix        
        = aMatrix~EMatrix Determinant.
                    
    toLiteral : aMatrix
        = aMatrix~EMatrix literal.
}.