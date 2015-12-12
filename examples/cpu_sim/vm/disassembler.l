#import system.
#import system'io.
#import system'routines.
#import system'collections.
#import extensions.

#symbol $setMnenomics = aList
[
    aList +=
         "add" += "adc" += "Addu" += "sub" += "sbc" += "subu" += "mul" +=
         "multu" += "div" += "divu" += "mod" += "neg" += "lsa" += "rsa" += 
         "lsh" += "rsh" += "xor" += "or" += "and" += "not" += "li" += "lb" += 
         "lw" += "sb" += "sw" += "br" += "bl" += "cmp" += "tst" += "teq" += 
         "irtn" += "rtn" += "sc" += "push" += "pop" += "lctl" += "sctl" += "livt" += "sivt".
].

#symbol $setCondidions = aList
[
    aList += 
        "br" += 
        "bs" +=
        "bz" +=
        "bc" += 
        "bv" +=
        "be" += 
        "bne" += 
        "bgt" += 
        "bge" +=
        "blt" += 
        "ble" +=
        "bt".
].


#class Disassembler
{
    #field mnenomics.
    #field conditionalBranches.
    
    #constructor new
    [
        mnenomics := List new.
        conditionalBranches := List new.
        
        $setMnenomics:mnenomics.
        $setCondidions:conditionalBranches.
    ]
    
    #method $padZeros : aValue
    [
        #var hexValue := aValue toLiteral &base:16.
        ^((hexValue literal) padLeft &char:(CharValue new &char:#48) &int:4).
    ]
    
    #method disassembleInstruction : aInstruction
    [
        #var opCode := (aInstruction and: 03Fh) byte.
        #var op1    := ((aInstruction shift &index:08) and: 00Fh) byte.
        #var op2    := ((aInstruction shift &index:12) and: 00Fh) byte.
        #var op3    := ((aInstruction shift &index:16) and: 0FFFFh) int.
        
        #var immediate := ((aInstruction and: 80h) == 0).
        (opCode >= mnenomics length)? [ console writeLine:"something wrong is not right...". ^$nil. ].
        
        #var mnenomic := mnenomics@(opCode).
        
        (opCode) =>
            14h ? [ ^(mnenomic + " %r" + (op1 literal) + ", 0x" + $self $padZeros:(op3)). ]
            15h ? [ ^(mnenomic + " %r" + (op1 literal) + ", @r" + (op1 literal) + ":0x" + $self $padZeros:(op3)). ]
            16h ? [ ^(mnenomic + " %r" + (op1 literal) + ", @r" + (op1 literal) + ":0x" + $self $padZeros:(op3)). ]
            17h ? [ ^(mnenomic + " @r" + (op1 literal) + ":0x" + $self $padZeros:(op3) + ", " + (op2 literal)). ]
            18h ? [ ^(mnenomic + " @r" + (op1 literal) + ":0x" + $self $padZeros:(op3) + ", " + (op2 literal)). ]
            19h ? 
                [
                    (op1 >= conditionalBranches length)
                     ? [ console writeLine:"something wrong is not right...". ]
                     ! [ ^(((conditionalBranches@op1) literal) + " 0x" + (op3 literal)). ].
                ]
            1Ah ? [ ^(mnenomic + " 0x" + (op3 literal)). ]
            1Bh ? 
                [
                    (immediate)
                     ? [ ^(mnenomic + "i %r" + (op1 literal) + ", 0x" + $self $padZeros:(op3)). ]
                     ! [ ^(mnenomic + " %r" + (op1 literal) + ", %r" + (op2 literal)). ]  
                ]
            21h ? [ ^(mnenomic + ($self disassembleRegisterList:op3)). ]
            22h ? [ ^(mnenomic + ($self disassembleRegisterList:op3)). ]
            ![
                (immediate)
                    ? [ ^(mnenomic + "i %r" + (op1 literal) + ", %r" + (op2 literal) + ", 0x" + $self $padZeros:(op3)). ]
                    ! [ ^(mnenomic + " %r" + (op1 literal) + ", %r" + (op2 literal) + ", %r" + (op3 literal)). ]
             ].
    ]
    
    #method disassembleRegisterList : list
    [
        #var accum := "".
        #var i := 0.
        #loop (i < 16)?
        [
            (((2 shift &index: (i * -1)) and: list) != 0)?
                [
                    accum := accum + (", r" + (i literal)).
                ]. 
            i := i + 1.
        ].
        ^accum.
    ]
}
