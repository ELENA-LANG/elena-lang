// ==== ver 3.4.1 ===

#import system.
//#define system'math.
//#define system'routines.
#import system'dynamic.
#import system'text.
#import extensions.

#class MatrixException :: Exception
{
    #constructor new : aMessage
        <= %new &literal:(aMessage literal) &subject:%MatrixError.
}

#class(extension:realmatrix)matrixOp
{
    #method clone
    [
        #var(type:realmatrix)aMatrix := self realmatrix.
        
        #var(type:int)i := 0.
        #var(type:int)j := 0.
        #var(type:int)n.
        #var(type:int)m.
        
        aMatrix readLengths &vint:n &vint:m.
        
        #var(type:realmatrix)r := RealMatrix new &int:n &int:m.
        #loop (i < n)?
        [
            j := 0.
            #loop (j < m)?
            [
                #var(type:real)v.
        
                aMatrix read &int:i &int:j &vreal:v.
                r write &int:i &int:j &real:v.
                
                j := j + 1.
            ].
            
            i := i + 1.
        ].
        
        ^ r.
    ]
    
    #method set &every_ij:aFunction
        = self::matrixOp set &nnfunc:(aFunction nnfunc).
    
    #method set &nnfunc:aFunction
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
                aMatrix write &int:i &int:j &real:(aFunction eval &int:i &int:j real).
                
                j := j + 1.
            ].
            
            i := i + 1.
        ].
        
        ^ self.
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
        #var(type:realmatrix)r := RealMatrix new &int:n &int:m.
        #loop (i < n)?
        [
            j := 0.                        
            #loop (j < m)?
            [
                #var(type:real)r1.
                #var(type:real)r2.
                a read &int:i &int:j &vreal:r1.
                b read &int:i &int:j &vreal:r2.
                
                r write &int:i &int:j &real:(r1 + r2).
                
                j := j + 1.
            ].
            i := i + 1.
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
        #var(type:realmatrix)r := RealMatrix new &int:n &int:m.
        #loop (i < n)?
        [
            j := 0.                        
            #loop (j < m)?
            [
                #var(type:real)r1.
                #var(type:real)r2.
                a read &int:i &int:j &vreal:r1.
                b read &int:i &int:j &vreal:r2.
                
                r write &int:i &int:j &real:(r1 - r2).
                
                j := j + 1.
            ].
            
            i := i + 1.
        ].
        
        ^ r.
    ]
    
    #method multiply : anObject
        = self::matrixOp multiply &realmatrix:(anObject realmatrix).
    
    #method multiply &realmatrix:b
    [
        #var(type:realmatrix)a := self realmatrix.
        #var(type:int)i.
        #var(type:int)j.
        #var(type:int)k.
        #var(type:int)n.
        #var(type:int)p.
        #var(type:int)m.
        
        a readLengths &vint:n &vint:j.
        b readLengths &vint:m &vint:p.
        
        (j == m)
            ! [ #throw MatrixException new:"Invalid argument". ].

        i := 0.                        
        #var(type:realmatrix)r := RealMatrix new &int:n &int:p.
        #loop (i < n)?
        [
            j := 0.                        
            #loop (j < p)?
            [
                k := 0.
                #var(type:real)v := 0.0r.
                #loop (k < m)?
                [
                    #var(type:real)r1.
                    #var(type:real)r2.
                    a read &int:i &int:k &vreal:r1.
                    b read &int:k &int:j &vreal:r2.
                    
                    r1 := r1 * r2.
                    v := v + r1.
                    
                    k := k + 1.
                ].
                
                r write &int:i &int:j &real:v.
                
                j := j + 1.
            ].
            
            i := i + 1.
        ].
        
        ^ r.
    ]
    
    #method decomposed
    [
        // create matrix copy
        #var(type:realmatrix) a := (self::matrixOp clone) realmatrix.

        #var(type:int)i := 0.
        #var(type:int)j := 0.
        #var(type:int)k := 0.
        #var(type:int)n.
        #var(type:int)m.
        #var(type:real)v.
        #var(type:real)t.
        
        a readLengths &vint:n &vint:m.
        
        (n != m)
            ? [ #throw MatrixException new:"Invalid operation". ].
        
        // toggle tracks row swaps. +1 -> even, -1 -> odd. used by MatrixDeterminant
        #var even := Boolean new:true.          

        // permutation matrix
        #var(type:intarray) pm := IntArray new &int:n.
        pm set &nfunc: (&int:i) [ IntNumber new &int:i ].

        // each column, j is counter for coulmns
        #var(type:int)  max_col := 0.
        #var(type:real) max := 0.0r.
        
        #loop (j < n)?
        [
            // find largest value in col j
            a read &int:j &int:j &vreal:max.
            realOpController abs &real:max &vreal:max.
            
            max_col := j.
            i := j + 1.
            #loop (i < n)?
            [
                a read &int:i &int:j &vreal:v.
                realOpController abs &real:v &vreal:v.
                (v > max)?
                [
                    max := v.
                    max_col := i.
                ].
                
                i := i + 1.
            ].
  
            // if largest value not on pivot, swap rows
            (max_col != j) ?
            [
                k := 0.
                #loop (k < n)?
                [
                    a read &int:j &int:k &vreal:t.
                    a read &int:max_col &int:k &vreal:v.

                    a write &int:j &int:k &real:v.
                    a write &int:max_col &int:k &real:t.
                    
                    k := k + 1.
                ].

                 // switch two rows in permutation matrix
                 pm::intArrayOp exchange &int:j &int:max_col.

                 even invert.
            ].
                        
            (0.0r == max)
                ? [ #throw MatrixException new:"The matrix is singular!". ].

            a read &int:j &int:j &vreal:t.
            
            i := j + 1.
            #loop (i < n)?
            [
                a read &int:i &int:j &vreal:v.
                v := v / t.
                
                a write &int:i &int:j &real:v.

                k := j + 1.
                #loop (k < n)?
                [
                    #var(type:real)c.
                    a read &int:i &int:j &vreal:v.
                    a read &int:j &int:k &vreal:c.
                    v := v * c.

                    a read &int:i &int:k &vreal:c.
                    v := c - v.
                    
                    a write &int:i &int:k &real:v.
                    
                    k := k + 1.
                ].
                
                i := i + 1.
            ].
            
            j := j + 1.
        ].

        ^ Extension({ Even = even value. }, a).
    ]

    #method(suppress:w2) determinant
    [
        #var lum := self::matrixOp decomposed.
        #var(type:real) deter := 1.0r.
        #var(type:real) v.

        (lum Even)
            ! [ deter := deter * (-1.0r). ].
        
        #var(type:realmatrix)a := lum realmatrix.        
        #var(type:int) n.
        #var(type:int) m.
        a readLengths &vint:n &vint:m.
        
        #var(type:int)i := 0.
        #loop (i < n)?
        [
            a read &int:i &int:i &vreal:v.
            
            deter := deter * v.
            
            i := i + 1.
        ].
            
       ^ RealNumber new &real:deter.
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
                aMatrix read &int:i &int:j &vreal:r.
                
                anOutput writeLiteral:(RealNumber new &real:r):",".
                
                j := j + 1.
            ].
            anOutput readLength &vint:l.            
            anOutput setLength &int:(l - 1).
            
            anOutput write:'newLine.
            
            i := i + 1.
        ].
        
        ^ anOutput literal.
    ]
}
