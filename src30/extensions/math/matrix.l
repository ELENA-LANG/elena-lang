#define system.
//#define system'routines.
//#define system'dynamic.
#define system'text.
#define extensions.

#subject(class:system'IndexFunction2) every_ij.

// --- MatrixException ---

#subject(class:system'BaseFunction1) MatrixError.

#class MatrixException :: Exception
{
    #constructor new : aMessage
        <= %new &message:aMessage &error_type: %MatrixError.
}

#class(extension:realmatrix)matrixOp
{
    #method set &every_ij:aFunction
    [
        #var(type:realmatrix)aMatrix := self realmatrix.
        
        #var(type:int)i := 0.
        #var(type:int)j := 0.
        #var(type:int)n.
        #var(type:int)m.
        
        aMatrix readLengths &vint:n &vint:m.
        
        #loop (i < n)?
        [
            j := 0.
            #loop (j < m)?
            [
                aMatrix write &index:i &index:j &real:(aFunction eval &index:i &index:j real).
            ].
        ].
    ]
    
    #method add : anObject
        = self::matrixOp add &realmatrix:(anObject realmatrix).
    
    #method add &realmatrix:b
    [
        #var(type:realmatrix)a := self realmatrix.
        #var(type:int)i.
        #var(type:int)j.
        #var(type:int)n.
        #var(type:int)m.
        
        a readLengths &vint:n &vint:m.
        b readLengths &vint:i &vint:j.
        
        (n == i)
            ! [ #throw MatrixException new:"Two matrices must have an equal number of rows and columns to be added". ].
        
        (m == j)
            ! [ #throw MatrixException new:"Two matrices must have an equal number of rows and columns to be added". ].
            
        i := 0.                        
        #var(type:realmatrix)r := RealMatrix new &length:n &length:m.
        #loop (i < n)?
        [
            j := 0.                        
            #loop (j < m)?
            [
                #var(type:real)r1.
                #var(type:real)r2.
                a read &index:i &index:j &vreal:r1.
                b read &index:i &index:j &vreal:r2.
                
                r write &index:i &index:j &real:(r1 + r2).
            ].
        ].
        
        ^ r.
    ]
    
    #method subtract : anObject
        = self::matrixOp subtract &realmatrix:(anObject realmatrix).
    
    #method subtract &realmatrix:b
    [
        #var(type:realmatrix)a := self realmatrix.
        #var(type:int)i.
        #var(type:int)j.
        #var(type:int)n.
        #var(type:int)m.
        
        a readLengths &vint:n &vint:m.
        b readLengths &vint:i &vint:j.
        
        (n == i)
            ! [ #throw MatrixException new:"Two matrices must have an equal number of rows and columns to be added". ].
        
        (m == j)
            ! [ #throw MatrixException new:"Two matrices must have an equal number of rows and columns to be added". ].
            
        i := 0.                        
        #var(type:realmatrix)r := RealMatrix new &length:n &length:m.
        #loop (i < n)?
        [
            j := 0.                        
            #loop (j < m)?
            [
                #var(type:real)r1.
                #var(type:real)r2.
                a read &index:i &index:j &vreal:r1.
                b read &index:i &index:j &vreal:r2.
                
                r write &index:i &index:j &real:(r1 - r2).
            ].
        ].
        
        ^ r.
    ]
            
    #method literal
    [
        #var(type:textbuffer) anOutput := TextBuffer new.
        #var(type:realmatrix)aMatrix := self realmatrix.
        
        #var(type:int)i := 0.
        #var(type:int)j := 0.
        #var(type:int)n.
        #var(type:int)m.
        #var(type:int)l.
        
        aMatrix readLengths &vint:n &vint:m.
        
        #loop (i < n)?
        [
            j := 0.
            #loop (j < m)?
            [
                #var(type:real)r.
                aMatrix read &index:i &index:j &vreal:r.
                
                anOutput writeLiteral:(RealNumber new &real:r):",".
            ].
            anOutput readLength &vint:l.            
            anOutput set &length:(l - 1).
            
            anOutput write:'newLine.
        ].
        
        ^ anOutput literal.
    ]
}

//// --- Matrix ---
//
//#class Matrix
//{
//    #field(type:array) the2DArray.
//    #field(type:int)theN.
//    #field(type:int)theM.
//    
//    #constructor new &n:n &m:m &every_ij:aFunction
//    [
//        theN := n.
//        theM := m.
//        
//        the2DArray := Array new &length:n set &every: (&index: i)
//            [ Array new &length:m set &every: (&index: j) [ aFunction eval &index:i &index:j ] ].
//    ]
//
//    #constructor new &n:n &every_ij:aFunction
//        <= %new &n:n &m:n &every_ij:aFunction.
//        
//    #constructor new &matrix:aMatrix
//    [
//        theN := aMatrix n.
//        theM := aMatrix m.
//        
//        the2DArray := Array new &length:theN set &every: (&index: i)
//            [ Array new &length:(theN int) set &every: (&index: j) [ aMatrix getAt &index:i &index:j clone ] ].
//    ]
//    
//    #method n = theN.
//    
//    #method m = theM.
//        
//    #method matrix = $self.
//    
//    #method enumerator => the2DArray.
//    
//    #method getAt &index:i &index:j
//        = the2DArray getAt &index:i array getAt &index:j.
//    
//    #method setAt &index:i &index:j &value:aValue
//        = the2DArray getAt &index:i array setAt &index:j &object:aValue.
//    
//        
//    #method subtract : anObject
//        = $self subtract &matrix:(anObject matrix).
//    
//    #method subtract &matrix:aMatrix
//        = Matrix new &n:theN &m:theM &every_ij:(&index:i &index:j) [ $self getAt &index:i &index:j - aMatrix getAt &index:i &index:j ].
//
//    #method multiply : anObject
//        = $self multiply &matrix:(anObject matrix).
//    
//    #method multiply &matrix:aMatrix
//    [
//        (theM != aMatrix n)
//            ? [ #throw MatrixException new:"Invalid argument". ].
//        
//        ^ Matrix new &n:theN &m:(aMatrix m) &every_ij:(&index:i &index:j) 
//            [
//                #var aResult := Variable new.
//                #var(type:int)k := 0.
//                #loop (k < theM) ?
//                [
//                    aResult append:(self getAt &index:i &index:k * aMatrix getAt &index:k &index:j).
//                    
//                    k := k + 1.
//                ].
//                
//                ^ aResult value.
//            ].
//    ]
//    
//    #method clone
//        = Matrix new &matrix:$self.
//    
//    #method decomposed
//    [
//        (theM != theN)
//            ? [ #throw MatrixException new:"Invalid operation". ].
//        
//        // toggle tracks row swaps. +1 -> even, -1 -> odd. used by MatrixDeterminant
//        #var even := Boolean new:true.          
//
//        // create matrix copy
//        #var a := $self clone.
//
//        // permutation matrix
//        #var pm := Array new &length:theN set &every: (&index:i) [ IntNumber new &int:i ].
//         
//        // each column, j is counter for coulmns
//        #var max_row := Integer new:0.
//        #var max := Variable new:0.
//        control run &int:0 &int:(theN - 2) &forEvery: (&index:j)
//        [
//            // find largest value in col j
//            max << mathControl abs:(a getAt &index:j &index:j).
//            max_row << j.
//            
//            control run &int:(j + 1) &int:(theN-1) &forEvery: (&index:i)
//            [
//                #var val := mathControl abs:(a getAt &index:i &index:j).
//                (val > max)
//                    ? [
//                        max << val.
//                        max_row << i.
//                    ].
//            ].
//            
//            // if largest value not on pivot, swap rows
//            (max_row != j) ?
//            [
//                control run &int:0 &int:(theN-1) &forEvery: (&index:k)
//                [ 
//                    #var tmp := a getAt &index:j &index:k.
//                    
//                    a setAt &index:(j int) &index:k &value:(a getAt &index:(max_row int) &index:k).
//                    a setAt &index:(max_row int) &index:k &value:tmp.
//                ].
//                    
//                 // switch two rows in permutation matrix
//                 pm~indexable exchange:j:max_row.
//                 
//                 even invert.
//            ].
//                        
//            (0.0r == max)
//                ? [ #throw MatrixException new:"The matrix is singular!". ].
//
//            #var val := a getAt &index:j &index:j.
//            control run &int:(j + 1) &int:(theN-1) &forEvery: (&index:i)
//            [
//                a setAt &index:i &index:j &value:(a getAt &index:i &index:(j int) / val).
//                
//                control run &int:(j+1) &int:(theN - 1) &forEvery: (&index:k)
//                [
//                    a setAt &index:i &index:k &value:(a getAt &index:(i int) &index:k - a getAt &index:(i int) &index:(j int) * a getAt &index:(j int) &index:k).
//                ].
//            ].
//        ].
//        
//        ^ Extension({ Even = even value. }, a).
//    ]
//
//    #method determinant
//    [
//        #var lum := $self decomposed.
//        #var deter := Real new &int:1.
//
//        (lum Even)
//            ! [ deter *= -1. ].
//        
//        control run &int:0 &int:(theN - 1) &forEvery: (&index:i)
//            [ deter << deter * (lum getAt &index:i &index:i). ].
//            
//       ^ deter value.
//    ]
//
//    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &matrix:$self.
//    
//    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &matrix:$self.
//}
