import extensions.

A = (:k:x1:x2:x3:x4:x5)
[
    var m := Integer new:k.
    var b := [ m reduce:1. ^ A(m,@self,x1,x2,x3,x4) ].
    
    if (m int <= 0)
        [ ^ x4 eval + x5 eval. ];
        [ ^ b eval. ].
].

program =
[
    0 to:14 do(:n)
    [
        console printLine(A(n,[^1],[^-1],[^-1],[^1],[^0])).
    ].
].
