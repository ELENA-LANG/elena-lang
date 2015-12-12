#import system.
#import system'dynamic.
#import system'routines.
#import system'io.
#import extensions.

#import system'collections.

#symbol program =
[
    #var source := $nil.
    #var output := $nil.
    
    #var i := 1.
    #var arg := $nil.
    #loop (i < 'program'arguments length)?
    [
        arg := 'program'arguments@i.
        (arg == "-o")
            ? [ 
                i := i + 1. 
                output := 'program'arguments@i. 
              ]
            ! [ source := 'program'arguments@i. ].
        i := i + 1.
    ].

    #var lexer := assembler'Lexer new &sourceFile:source.
    lexer scan.
    
    #var parser := assembler'Parser new.
    parser processTokens &tokens:(lexer TokenList).

    #var bin := assembler'BinaryFile new.
    #var cgen := assembler'CodeGenerator new:bin.
    cgen assemble:(parser Output).
    bin finalizeExecutable.
    bin generate:output.
].