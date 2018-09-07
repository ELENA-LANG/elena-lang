import extensions.

A = (:k:x1:x2:x3:x4:x5)
[
    var m := Integer new:k.
    var b := [ m reduce:1. ^ A(m,@self,x1,x2,x3,x4) ].
    
    if (m <= 0)
        [ ^ x4() + x5(). ];
        [ ^ b(). ].
].

public program
[
    0 to:14 do(:n)
    [
        console printLine(A(n,[^1],[^-1],[^-1],[^1],[^0])).
    ].
]
