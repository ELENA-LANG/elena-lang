import system'dynamic.
import system'text.
//import extensions.

class MatrixException :: Exception
{
    constructor new(literal l)
        <= new(l).
}

public extension<T<RealMatrix>> matrixOp
{
    T<RealMatrix> clone
    [
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        
        self readLengthsTo(&n, &m).
        
        auto r := RealMatrix new(n, m).
        while (i < n)
        [
            j := 0.
            while (j < m)
            [
                real v := 0.0r.
        
                self read(i, j, &v).
                r write(i, j, v).
                
                j := j + 1
            ].
            
            i := i + 1
        ].
        
        ^ r
    ]
    
    populate(Func<int,int,real> f)
    [
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        
        self readLengthsTo(&n, &m).
        
        while (i < n)
        [
            j := 0.
            while (j < m)
            [
                self write(i, j, f(i,j)).
                
                j := j + 1
            ].
            
            i := i + 1
        ].
        
        ^ self
    ]
    
    T<RealMatrix> add : anObject
        = self~matrixOp add(T<RealMatrix>(anObject)).
    
    T<RealMatrix> add(RealMatrix b)
    [
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        
        self readLengthsTo(&n, &m).
        b readLengthsTo(&i, &j).
        
        ifnot(n == i)
            [ MatrixException new("Two matrices must have an equal number of rows and columns to be added"); raise ].
        
        ifnot(m == j)
            [ MatrixException new("Two matrices must have an equal number of rows and columns to be added"); raise ].
            
        i := 0.                        
        auto r := RealMatrix new(n, m).
        while (i < n)
        [
            j := 0.                        
            while (j < m)
            [
                real r1 := 0.0r.
                real r2 := 0.0r.
                self read(i, j, &r1).
                b read(i, j, &r2).
                
                r write(i, j, r1 + r2).
                
                j := j + 1
            ].
            i := i + 1
        ].
        
        ^ r
    ]
    
    T<RealMatrix> subtract : anObject
        = self~matrixOp subtract(T<RealMatrix>(anObject)).
    
    T<RealMatrix> subtract(RealMatrix b)
    [
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        
        self readLengthsTo(&n, &m).
        b readLengthsTo(&i, &j).
        
        ifnot(n == i)
            [ MatrixException new("Two matrices must have an equal number of rows and columns to be added"); raise ].
        
        ifnot(m == j)
            [ MatrixException new("Two matrices must have an equal number of rows and columns to be added"); raise ].
            
        i := 0.                        
        auto r := RealMatrix new(n, m).
        while (i < n)
        [
            j := 0.                        
            while (j < m)
            [
                real r1 := 0.
                real r2 := 0.
                self read(i, j, &r1).
                b read(i, j, &r2).
                
                r write(i, j, r1 - r2).
                
                j := j + 1
            ].
            
            i := i + 1
        ].
        
        ^ r
    ]
    
    T<RealMatrix> multiply : anObject
        = self~matrixOp multiply(T<RealMatrix>(anObject)).
    
    T<RealMatrix> multiply(RealMatrix b)
    [
        int i := 0.
        int j := 0.
        int k := 0.
        int n := 0.
        int p := 0.
        int m := 0.
        
        self readLengthsTo(&n, &j).
        b readLengthsTo(&m, &p).
        
        ifnot(j == m)
            [ MatrixException new:"Invalid argument"; raise ].

        i := 0.                        
        auto r := RealMatrix new(n, p).
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
                    self read(i, k, &r1).
                    b read(k, j, &r2).
                    
                    r1 := r1 * r2.
                    v := v + r1.
                    
                    k := k + 1.
                ].
                
                r write(i, j, v).
                
                j := j + 1
            ].
            
            i := i + 1
        ].
        
        ^ r
    ]
    
    decomposed
    [
        // create matrix copy
        auto a := self~matrixOp clone.

        int  i := 0.
        int  j := 0.
        int  k := 0.
        int  n := 0.
        int  m := 0.
        real v := 0.0r.
        real t := 0.0r.
        
        a readLengthsTo(&n, &m).
        
        if(n != m)
            [ MatrixException new:"Invalid operation"; raise ].
        
        // toggle tracks row swaps. +1 -> even, -1 -> odd. used by MatrixDeterminant
        bool even := true.          

        // permutation matrix
        auto pm := IntArray new(n).
        pm populate(:i)<int,int> [ ^i ].

        // each column, j is counter for coulmns
        int  max_col := 0.
        real max := 0.0r.
        
        while (j < n)
        [
            // find largest value in col j
            a read(j, j, &max).
            realOpController abs(max, &max).
            
            max_col := j.
            i := j + 1.
            while (i < n)
            [
                a read(i, j, &v).
                realOpController abs(v, &v).
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
                    a read(j, k, &t).
                    a read(max_col, k, &v).

                    a write(j, k, v).
                    a write(max_col, k, t).
                    
                    k := k + 1
                ].

                 // switch two rows in permutation matrix
                 pm/*~intArrayOp*/ exchange(j, max_col).

                 even := even inverted
            ].
                        
            if(0.0r == max)
                [ MatrixException new("The matrix is singular!"); raise ].

            a read(j, j, &t).
            
            i := j + 1.
            while (i < n)
            [
                a read(i, j, &v).
                v := v / t.
                
                a write(i, j, v).

                k := j + 1.
                while (k < n)
                [
                    real c := 0.0r.
                    a read(i, j, &v).
                    a read(j, k, &c).
                    v := v * c.

                    a read(i, k, &c).
                    v := c - v.
                    
                    a write(i, k, v).
                    
                    k := k + 1
                ].
                
                i := i + 1
            ].
            
            j := j + 1
        ].

        ^ Extension::({ Even = even. }, a).
    ]

    real determinant
    [
        var lum := self~matrixOp decomposed.
        real deter := 1.0r.
        real v := 0.0r.

        ifnot(lum Even)
            [ deter := deter * (-1.0r). ].
        
        auto a := T<RealMatrix>(lum).
        int        n := 0.
        int        m := 0.
        a readLengthsTo(&n, &m).
        
        int i := 0.
        while (i < n)
        [
            a read(i, i, &v).
            
            deter := deter * v.
            
            i := i + 1
        ].
            
       ^ deter.
    ]
            
    T<literal> toLiteral
    [
        auto anOutput := TextBuilder new.
        
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        int l := 0.
        
        self readLengthsTo(&n, &m).
        
        while (i < n)
        [
            j := 0.
            while (j < m)
            [
                real r := 0.0r.
                self read(i, j, &r).
                
                anOutput print(RealNumber new(r),",").
                
                j := j + 1
            ].
            anOutput readLengthTo(&l).            
            anOutput setLength(l - 1).
            
            anOutput write:forwards'newLine.
            
            i := i + 1
        ].
        
        ^ anOutput literal
    ]
}
