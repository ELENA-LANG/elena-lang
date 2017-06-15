import system'dynamic.
import system'text.
import extensions.

class MatrixException :: exceptionof(MatrixError)
{
    constructor new : message
    [
        theMessage := message literal.
        theCallStack := CallStack new.
    ]
}

realmatrix extension matrixOp
{
    realmatrix clone
    [
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        
        self readLengthsTo vint:n vint:m.
        
        realmatrix r := RealMatrix new int:n int:m.
        while (i < n)
        [
            j := 0.
            while (j < m)
            [
                real v := 0.0r.
        
                self read int:i int:j vreal:v.
                r write int:i int:j real:v.
                
                j := j + 1
            ].
            
            i := i + 1
        ].
        
        ^ r
    ]
    
    set every_ij:aFunction
        = self~matrixOp set nnfunc:(aFunction nnfunc).
    
    set nnfunc:aFunction
    [
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        
        self readLengthsTo vint:n vint:m.
        
        while (i < n)
        [
            j := 0.
            while (j < m)
            [
                self write int:i int:j real:(aFunction eval int:i int:j).
                
                j := j + 1
            ].
            
            i := i + 1
        ].
        
        ^ self
    ]
    
    add : anObject
        = self~matrixOp add realmatrix:(anObject realmatrix).
    
    add realmatrix:b
    [
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        
        self readLengthsTo vint:n vint:m.
        b readLengthsTo vint:i vint:j.
        
        ifnot(n == i)
            [ MatrixException new:"Two matrices must have an equal number of rows and columns to be added"; raise ].
        
        ifnot(m == j)
            [ MatrixException new:"Two matrices must have an equal number of rows and columns to be added"; raise ].
            
        i := 0.                        
        realmatrix r := RealMatrix new int:n int:m.
        while (i < n)
        [
            j := 0.                        
            while (j < m)
            [
                real r1 := 0.0r.
                real r2 := 0.0r.
                self read int:i int:j vreal:r1.
                b read int:i int:j vreal:r2.
                
                r write int:i int:j real:(r1 + r2).
                
                j := j + 1
            ].
            i := i + 1
        ].
        
        ^ r
    ]
    
    subtract : anObject
        = self~matrixOp subtract realmatrix:(anObject realmatrix).
    
    subtract realmatrix:b
    [
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        
        self readLengthsTo vint:n vint:m.
        b readLengthsTo vint:i vint:j.
        
        ifnot(n == i)
            [ MatrixException new:"Two matrices must have an equal number of rows and columns to be added"; raise ].
        
        ifnot(m == j)
            [ MatrixException new:"Two matrices must have an equal number of rows and columns to be added"; raise ].
            
        i := 0.                        
        realmatrix r := RealMatrix new int:n int:m.
        while (i < n)
        [
            j := 0.                        
            while (j < m)
            [
                real r1 := 0.
                real r2 := 0.
                self read int:i int:j vreal:r1.
                b read int:i int:j vreal:r2.
                
                r write int:i int:j real:(r1 - r2).
                
                j := j + 1
            ].
            
            i := i + 1
        ].
        
        ^ r
    ]
    
    multiply : anObject
        = self~matrixOp multiply realmatrix:(anObject realmatrix).
    
    multiply realmatrix:b
    [
        int i := 0.
        int j := 0.
        int k := 0.
        int n := 0.
        int p := 0.
        int m := 0.
        
        self readLengthsTo vint:n vint:j.
        b readLengthsTo vint:m vint:p.
        
        ifnot(j == m)
            [ MatrixException new:"Invalid argument"; raise ].

        i := 0.                        
        realmatrix r := RealMatrix new int:n int:p.
        while (i < n)
        [
            j := 0.                        
            while (j < p)
            [
                k := 0.
                real v := 0.0r.
                while (k < m)
                [
                    real r1 := 0.0r.
                    real r2 := 0.0r.
                    self read int:i int:k vreal:r1.
                    b read int:k int:j vreal:r2.
                    
                    r1 := r1 * r2.
                    v := v + r1.
                    
                    k := k + 1.
                ].
                
                r write int:i int:j real:v.
                
                j := j + 1
            ].
            
            i := i + 1
        ].
        
        ^ r
    ]
    
    decomposed
    [
        // create matrix copy
        realmatrix a := self~matrixOp clone.

        int  i := 0.
        int  j := 0.
        int  k := 0.
        int  n := 0.
        int  m := 0.
        real v := 0.0r.
        real t := 0.0r.
        
        a readLengthsTo vint:n vint:m.
        
        if(n != m)
            [ MatrixException new:"Invalid operation"; raise ].
        
        // toggle tracks row swaps. +1 -> even, -1 -> odd. used by MatrixDeterminant
        bool even := true.          

        // permutation matrix
        intarray pm := IntArray new int:n.
        pm set nfunc: (&int:i) [ ^i ].

        // each column, j is counter for coulmns
        int  max_col := 0.
        real max := 0.0r.
        
        while (j < n)
        [
            // find largest value in col j
            a read int:j int:j vreal:max.
            realOpController abs real:max vreal:max.
            
            max_col := j.
            i := j + 1.
            while (i < n)
            [
                a read int:i int:j vreal:v.
                realOpController abs real:v vreal:v.
                if(v > max)
                [
                    max := v.
                    max_col := i
                ].
                
                i := i + 1
            ].
  
            // if largest value not on pivot, swap rows
            if(max_col != j)
            [
                k := 0.
                while(k < n)
                [
                    a read int:j int:k vreal:t.
                    a read int:max_col int:k vreal:v.

                    a write int:j int:k real:v.
                    a write int:max_col int:k real:t.
                    
                    k := k + 1
                ].

                 // switch two rows in permutation matrix
                 pm~intArrayOp exchange int:j int:max_col.

                 even := even not
            ].
                        
            if(0.0r == max)
                [ MatrixException new:"The matrix is singular!"; raise ].

            a read int:j int:j vreal:t.
            
            i := j + 1.
            while (i < n)
            [
                a read int:i int:j vreal:v.
                v := v / t.
                
                a write int:i int:j real:v.

                k := j + 1.
                while (k < n)
                [
                    real c := 0.0r.
                    a read int:i int:j vreal:v.
                    a read int:j int:k vreal:c.
                    v := v * c.

                    a read int:i int:k vreal:c.
                    v := c - v.
                    
                    a write int:i int:k real:v.
                    
                    k := k + 1
                ].
                
                i := i + 1
            ].
            
            j := j + 1
        ].

        ^ Extension::({ Even = even. }, a).
    ]

    determinant
    [
        var lum := self~matrixOp decomposed.
        real deter := 1.0r.
        real v := 0.0r.

        ifnot(lum Even)
            [ deter := deter * (-1.0r). ].
        
        realmatrix a := lum realmatrix.        
        int        n := 0.
        int        m := 0.
        a readLengthsTo vint:n vint:m.
        
        int i := 0.
        while (i < n)
        [
            a read int:i int:i vreal:v.
            
            deter := deter * v.
            
            i := i + 1
        ].
            
       ^ deter.
    ]
            
    literal
    [
        textbuffer anOutput := TextBuilder new.
        
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        int l := 0.
        
        self readLengthsTo vint:n vint:m.
        
        while (i < n)
        [
            j := 0.
            while (j < m)
            [
                real r := 0.0r.
                self read int:i int:j vreal:r.
                
                anOutput writeLiteral:(RealNumber new real:r):",".
                
                j := j + 1
            ].
            anOutput readLengthTo vint:l.            
            anOutput setLength int:(l - 1).
            
            anOutput write:'newLine.
            
            i := i + 1
        ].
        
        ^ anOutput literal
    ]
}
