#import system.

#symbol compareStrings = (:val1 :val2)
[
    (val1 == val2)? [ console writeLine:"The strings are equal". ].
    (val1 != val2)? [ console writeLine:"The strings are not equal". ].
    (val1  > val2)? [ console writeLine:"The first string is lexically after than the second". ].
    (val1  < val2)? [ console writeLine:"The first string is lexically before than the second". ].
    (val1 >= val2)? [ console writeLine:"The first string is not lexically before than the second". ].
    (val1 <= val2)? [ console writeLine:"The first string is not lexically after than the second". ].
].

#symbol program =
[
    #var s1 := "this".
    #var s2 := "that".
    compareStrings :s1 :s2.
].