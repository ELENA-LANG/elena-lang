#import system.
#import system'dynamic.
#import system'text.
#import extensions.

#class(exceptionof:MatrixError) MatrixException
{
    #constructor new : message
    [
        theMessage := message literal.
        theCallStack := CallStack new.
    ]
}

#class(extension:realmatrix)matrixOp
{
    #method(realmatrix) clone
    [
        #var(int)i := 0.
        #var(int)j := 0.
        #var(int)n.
        #var(int)m.
        
        self readLengths &vint:n &vint:m.
        
        #var(realmatrix)r := RealMatrix new &int:n &int:m.
        #loop (i < n)?
        [
            j := 0.
            #loop (j < m)?
            [
                #var(real)v.
        
                self read &int:i &int:j &vreal:v.
                r write &int:i &int:j &real:v.
                
                j += 1.
            ].
            
            i += 1.
        ].
        
        ^ r.
    ]
    
    #method set &every_ij:aFunction
        = self::matrixOp set &nnfunc:(aFunction nnfunc).
    
    #method set &nnfunc:aFunction
    [
        #var(int)i := 0.
        #var(int)j := 0.
        #var(int)n.
        #var(int)m.
        
        self readLengths &vint:n &vint:m.
        
        #loop (i < n)?
        [
            j := 0.
            #loop (j < m)?
            [
                self write &int:i &int:j &real:(aFunction eval &int:i &int:j real).
                
                j += 1.
            ].
            
            i += 1.
        ].
        
        ^ self.
    ]
    
    #method add : anObject
        = self::matrixOp add &realmatrix:(anObject realmatrix).
    
    #method add &realmatrix:b
    [
        #var(int)i.
        #var(int)j.
        #var(int)n.
        #var(int)m.
        
        self readLengths &vint:n &vint:m.
        b readLengths &vint:i &vint:j.
        
        (n == i)
            ! [ #throw MatrixException new:"Two matrices must have an equal number of rows and columns to be added". ].
        
        (m == j)
            ! [ #throw MatrixException new:"Two matrices must have an equal number of rows and columns to be added". ].
            
        i := 0.                        
        #var(realmatrix)r := RealMatrix new &int:n &int:m.
        #loop (i < n)?
        [
            j := 0.                        
            #loop (j < m)?
            [
                #var(real)r1.
                #var(real)r2.
                self read &int:i &int:j &vreal:r1.
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
        #var(int)i.
        #var(int)j.
        #var(int)n.
        #var(int)m.
        
        self readLengths &vint:n &vint:m.
        b readLengths &vint:i &vint:j.
        
        (n == i)
            ! [ #throw MatrixException new:"Two matrices must have an equal number of rows and columns to be added". ].
        
        (m == j)
            ! [ #throw MatrixException new:"Two matrices must have an equal number of rows and columns to be added". ].
            
        i := 0.                        
        #var(realmatrix)r := RealMatrix new &int:n &int:m.
        #loop (i < n)?
        [
            j := 0.                        
            #loop (j < m)?
            [
                #var(real)r1.
                #var(real)r2.
                self read &int:i &int:j &vreal:r1.
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
        #var(int)i.
        #var(int)j.
        #var(int)k.
        #var(int)n.
        #var(int)p.
        #var(int)m.
        
        self readLengths &vint:n &vint:j.
        b readLengths &vint:m &vint:p.
        
        (j == m)
            ! [ #throw MatrixException new:"Invalid argument". ].

        i := 0.                        
        #var(realmatrix)r := RealMatrix new &int:n &int:p.
        #loop (i < n)?
        [
            j := 0.                        
            #loop (j < p)?
            [
                k := 0.
                #var(real)v := 0.0r.
                #loop (k < m)?
                [
                    #var(real)r1.
                    #var(real)r2.
                    self read &int:i &int:k &vreal:r1.
                    b read &int:k &int:j &vreal:r2.
                    
                    r1 := r1 * r2.
                    v := v + r1.
                    
                    k := k + 1.
                ].
                
                r write &int:i &int:j &real:v.
                
                j += 1.
            ].
            
            i += 1.
        ].
        
        ^ r.
    ]
    
    #method decomposed
    [
        // create matrix copy
        #var(realmatrix) a := self::matrixOp clone.

        #var(int)i := 0.
        #var(int)j := 0.
        #var(int)k := 0.
        #var(int)n.
        #var(int)m.
        #var(real)v.
        #var(real)t.
        
        a readLengths &vint:n &vint:m.
        
        (n != m)
            ? [ #throw MatrixException new:"Invalid operation". ].
        
        // toggle tracks row swaps. +1 -> even, -1 -> odd. used by MatrixDeterminant
        #var(bool) even := true.          

        // permutation matrix
        #var(intarray) pm := IntArray new &int:n.
        pm set &nfunc: (&int:i) [ IntNumber new &int:i ].

        // each column, j is counter for coulmns
        #var(int)  max_col := 0.
        #var(real) max := 0.0r.
        
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
                
                i += 1.
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

                 even := even not.
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
                    #var(real)c.
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

        ^ Extension({ Even = even. }, a).
    ]

    #method(w2) determinant
    [
        #var lum := self::matrixOp decomposed.
        #var(real) deter := 1.0r.
        #var(real) v.

        (lum Even bool)
            ! [ deter := deter * (-1.0r). ].
        
        #var(realmatrix)a := lum realmatrix.        
        #var(int) n.
        #var(int) m.
        a readLengths &vint:n &vint:m.
        
        #var(int)i := 0.
        #loop (i < n)?
        [
            a read &int:i &int:i &vreal:v.
            
            deter := deter * v.
            
            i += 1.
        ].
            
       ^ RealNumber new &real:deter.
    ]
            
    #method literal
    [
        #var(textbuffer) anOutput := TextBuilder new.
        
        #var(int)i := 0.
        #var(int)j := 0.
        #var(int)n.
        #var(int)m.
        #var(int)l.
        
        self readLengths &vint:n &vint:m.
        
        #loop (i < n)?
        [
            j := 0.
            #loop (j < m)?
            [
                #var(real)r.
                self read &int:i &int:j &vreal:r.
                
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
