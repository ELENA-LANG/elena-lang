#import system.
#import system'routines.
#import system'collections.

#symbol AssemblerData =
{
    #method assemble &codeGen:codeGen &instruction:ins &byteCodeStream:output
    [
        #var listTmp := List new.
        (ins Operands) run &each: op
        [
            (op is &string_literal)
                ? [
                    op Value select &each: x [ byteConvertor convert:x ] summarize:listTmp.
                ]
                ! [
                   (op is &numeric) ?
                   [
                       listTmp += byteConvertor convert:(op Value).
                   ]
                   ! [ assembler'feedback raiseError:"Unsupported addressing mode for instruction '":(ins Name):"'". ].                   
                ].
        ].
        output emit &data:(assembler'CpuData new &data:listTmp).
    ]

    #method Mnemonic = ("data", ".data").
}.
