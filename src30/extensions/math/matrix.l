#define system.
#define system'math.
#define system'routines.
#define system'dynamic.
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
    #method clone
    [
        #var(type:realmatrix)aMatrix := self realmatrix.
        
        #var(type:int)i := 0.
        #var(type:int)j := 0.
        #var(type:int)n.
        #var(type:int)m.
        
        aMatrix readLengths &vint:n &vint:m.
        
        #var(type:realmatrix)r := RealMatrix new &length:n &length:m.
        #loop (i < n)?
        [
            j := 0.
            #loop (j < m)?
            [
                #var(type:real)v.
        
                aMatrix read &index:i &index:j &vreal:v.
                r write &index:i &index:j &real:v.
                
                j := j + 1.
            ].
            
            i := i + 1.
        ].
        
        ^ r.
    ]
    
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
        #var(type:realmatrix)r := RealMatrix new &length:n &length:p.
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
                    a read &index:i &index:k &vreal:r1.
                    b read &index:k &index:j &vreal:r2.
                    
                    r1 := r1 * r2.
                    v := v + r1.
                    
                    k := k + 1.
                ].
                
                r write &index:i &index:j &real:v.
                
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
        #var(type:intarray) pm := IntArray new &length:n.
        pm set &every: (&index:i) [ IntNumber new &int:i ].

        // each column, j is counter for coulmns
        #var(type:int)  max_col := 0.
        #var(type:real) max := 0.0r.
        
        #loop (j < n)?
        [
            // find largest value in col j
            a read &index:j &index:j &vreal:max.
            realOpController abs &real:max &vreal:max.
            
            max_col := j.
            i := j + 1.
            #loop (i < n)?
            [
                a read &index:i &index:j &vreal:v.
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
                    a read &index:j &index:k &vreal:t.
                    a read &index:max_col &index:k &vreal:v.

                    a write &index:j &index:k &real:v.
                    a write &index:max_col &index:k &real:t.
                    
                    k := k + 1.
                ].

                 // switch two rows in permutation matrix
                 pm::intArrayOp exchange &index:j &index:max_col.

                 even invert.
            ].
                        
            (0.0r == max)
                ? [ #throw MatrixException new:"The matrix is singular!". ].

            a read &index:j &index:j &vreal:t.
            
            i := j + 1.
            #loop (i < n)?
            [
                a read &index:i &index:j &vreal:v.
                v := v / t.
                
                a write &index:i &index:j &real:v.

                k := j + 1.
                #loop (k < n)?
                [
                    #var(type:real)c.
                    a read &index:i &index:j &vreal:v.
                    a read &index:j &index:k &vreal:c.
                    v := v * c.

                    a read &index:i &index:k &vreal:c.
                    v := c - v.
                    
                    a write &index:i &index:k &real:v.
                    
                    k := k + 1.
                ].
                
                i := i + 1.
            ].
            
            j := j + 1.
        ].

        ^ Extension({ Even = even value. }, a).
    ]

    #method determinant
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
            a read &index:i &index:i &vreal:v.
            
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
                aMatrix read &index:i &index:j &vreal:r.
                
                anOutput writeLiteral:(RealNumber new &real:r):",".
                
                j := j + 1.
            ].
            anOutput readLength &vint:l.            
            anOutput set &length:(l - 1).
            
            anOutput write:'newLine.
            
            i := i + 1.
        ].
        
        ^ anOutput literal.
    ]
}

