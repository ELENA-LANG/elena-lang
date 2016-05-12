/// a routine to perform a bitwise AND, OR, and XOR on two integers, a bitwise NOT on the first integer, a left shift, right

#import system.
#import extensions.

#class(extension) testOp
{
    #method bitwiseTest : y
    [
        console writeLine:self:" and ":y:" = ":(self and:y).
        console writeLine:self:" or ":y:" = ":(self or:y).
        console writeLine:self:" xor ":y:" = ":(self xor:y).
        console writeLine:"not ":self:" = ":(self inverted).
        console writeLine:self:" shr ":y:" = ":(self shift &index:y).
        console writeLine:self:" shl ":y:" = ":(self shift &index:(y negative)).
    ]
}

#symbol program =
[
    console readLine:(Integer new) bitwiseTest:(console readLine:(Integer new)).
].