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

    #var i := Integer new:1.
    #loop (i < 'program'arguments length)?
    [
        #var arg := 'program'arguments@i.
        (arg == "-o")
            ? [ output := 'program'arguments@(i += 1). ]
            ! [ source := arg. ].
        
        i += 1.
    ].

    (($nil == source)or:($nil == output))
        ? [ console writeLine:"Invaid argument list. Usage: assembler -o <output file> <source file>". #throw AbortException new. ].

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