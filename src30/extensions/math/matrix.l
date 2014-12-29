//#define system.
//#define system'routines.
//#define system'dynamic.
//#define system'text.
//#define extensions.
//
//#type(unforced) matrix.
//#type(class:system'IndexFunction2) matrixItem.
//
//// --- MatrixException ---
//
//#type(class:system'BaseFunction1) MatrixError.
//
//#class MatrixException :: Exception
//{
//    #constructor new : aMessage
//        <= %new &message:aMessage &error_type: %MatrixError.
//}
//
//// --- EMatrix ---
//
//#class(extension:matrix) matrixOp
//{
//    #method add : aMatrix
//    [
//        #var(type:array) a := self array.
//        #var(type:int) n := a length.
//        #var(type:int) m := a getAt &index:0 length.
//
//        #var(type:array) b := aMatrix array.
//    
//        ^ arrayControl new &length:n &every: (&index: i)
//            [ arrayControl new &length:m &every: (&index: j) [ a getAt &index:i getAt &index:j + b getAt &index:(i int) getAt &index:j ] ].
//    ]
//    
//    #method subtract : aMatrix
//    [
//        #var(type:array) a := self array.
//        #var(type:int) n := a length.
//        #var(type:int) m := a getAt &index:0 length.
//
//        #var(type:array) b := aMatrix array.
//    
//        ^ arrayControl new &length:n &every: (&index: i)
//            [ arrayControl new &length:m &every: (&index: j) [ a getAt &index:i getAt &index:j - b getAt &index:(i int) getAt &index:j ] ].
//    ]
//
//    #method multiply : aMatrix
//    [
//        #var(type:array) a := self array.
//        #var(type:array) b := aMatrix array.
//        #var(type:int) n := a length.
//        
//        #var(type:array)c := arrayControl new &length:n &every: (&index: i) [ Array new &length:n ] array.
//        
//        (&index:i &index:j &index:k)
//            [
//                #var aValue := c getAt &index:i getAt &index:j.
//                
//                (nil == aValue)
//                    ? [ aValue := a getAt &index:i getAt &index:k * b getAt &index:k getAt &index:j. ]
//                    ! [ aValue := aValue + (a getAt &index:i getAt &index:k * b getAt &index:k getAt &index:j). ].
//                    
//                c getAt &index:i setAt &index:j &object:aValue.
//            ]
//            run &coord &int:0 &int:0 &int:0 &int:(n - 1) &int:(n - 1) &int:(n - 1) &int:1.
//        
//        ^ c.            
//    ]
//
//    #method deepClone
//    [
//        #var(type:array) anArray := self array.
//        
//        ^ arrayControl new &length:anArray &every: (&index:i)
//            [ (self@i)::arrayOp deepClone ].
//    ]
//    
//    #method decomposed
//    [
//        #var(type:array) anArray := self array.
//        #var(type:int) aCount := anArray length.
//        
//        // toggle tracks row swaps. +1 -> even, -1 -> odd. used by MatrixDeterminant
//        #var even := Boolean new:true.          
//
//        // create matrix copy
//        #var a := anArray::matrixOp deepClone.
//        
//        // permutation matrix
//        #var pm := arrayControl new &length:aCount &every: (&index:i) [ IntNumber new &int:i ].
//         
//        // each column, j is counter for coulmns
//        #var max_row := Integer new:0.
//        #var max := Real new:0.0r.
//        control run &int:0 &int:(aCount - 2) &forEvery: (&index:j)
//        [
//            // find largest value in col j
//            max << (abs:(a@j@j)).
//            max_row << j.
//            
//            control run &int:(j + 1) &int:(aCount-1) &forEvery: (&index:i)
//            [
//                #var val := abs:(a@i@j).
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
//                control run &int:0 &int::(aCount-1) &forEvery: (&index:k)
//                [ 
//                    #var tmp := a@j@k.
//                    
//                    (a@j) setAt:k:(a@max_row@k).
//                    (a@max_row) setAt:k:tmp.
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
//            #var val := a@j@j.
//            control run &int:(j + 1) &int::(aCount-1) &forEvery: (&index:i)
//            [
//                (a@i)setAt:j:((a@i@j) / val).
//                
//                control run &int::(j+1) &int::(aCount-1) &forEvery: (&index:k)
//                [
//                    (a@i)setAt:k:((a@i@k) - (a@i@j) * (a@j@k)).
//                ].
//            ]
//        ].
//        
//        ^ Extension(%Even[ even value ], a).
//    ]
//
//    #method determinant
//    [
//        #var(type:array) anArray := self array.
//        #var lum := anArray~matrixOp decomposed.
//        #var deter := Real new &int:1.
//
//        (lum Even)
//            ! [ deter *= -1. ].
//        
//        control run &int:0 &int:(anArray length - 1) &forEvery: (&index:i)
//            [ deter << deter * (lum@i@i). ].
//            
//       ^ deter value.
//    ]
//            
//    #method literal
//    [
//        #var(type:textbuffer) anOutput := TextBuffer new.
//        
//        #var(type:array)matrix := self array.
//        #var(type:int)n := matrix length.
//        
//        #var(type:int)i := 0.
//        #var(type:int)j := 0.
//        #loop (i < n) ?
//        [
//            #var(type:array)row := matrix getAt &index:i array.
//            #var(type:int)m := row length.
//            
//            j := 0.
//            #loop (j < m) ?
//            [
//                anOutput write &literal::(convertor toLiteral:(row getAt &index:j)) write:",".
//                
//                j := j + 1.
//            ].
//            
//            anOutput set &length:(-1 + anOutput length).
//            anOutput write:"%n".
//            
//            i := i + 1.
//        ].
//        
//        ^ anOutput literal.
//    ]
//}
//
//// --- matrixControl ---
//
//#symbol matrixControl =
//{
//    new &n:anN &m:anM &matrixItem:anAction
//        = arrayControl new &length:(anN int) &every: (&index: i)
//            [ arrayControl new &length:(anM int) &every: (&index: j) [ anAction eval &index:i &index:j ] ].
//
//    sumOf : aMatrix1 : aMatrix2
//        = aMatrix1~matrixOp add:aMatrix2.
//            
//    differenceOf : aMatrix1 : aMatrix2
//        = aMatrix1~matrixOp subtract:aMatrix2.
//        
//    productOf : aMatrix1 : aMatrix2
//        = aMatrix1~matrixOp multiply:aMatrix2.
//  
//    determinantOf : aMatrix        
//        = aMatrix~matrixOp determinant.
//                    
////    toLiteral : aMatrix
////        = aMatrix~EMatrix literal.
//}.