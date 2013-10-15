#define std'dictionary'*.
#define std'basic'*.
//#define std'routines'*.
#define std'patterns'*.
#define ext'patterns'*.

#define io'* = sys'io'*.

#define factory'* = std'basic'factory'*.

// --- constant forwards ---
#define(const) control      = std'patterns'control.

#subject $next, $previous, $resetLoop.

// --- Tape ---

#class TapeIterator
{
    #field thePointer.
    #field theArray.

    #initializer
    [
        thePointer := Integer << 0.

        theArray := NewArray &&pattern:WideCharVar::0 &count:1024 &:factory'epatterninit.
    ]

    #method $next
    [
        thePointer += 1.
    ]

    #method $previous
    [
        #if (thePointer > 0)?
        [
            thePointer -= 1.    
        ].
    ]

    #join theArray@thePointer.
}

// --- FileCharEnumerator ---

#symbol FileCharEnumerator : aFileName =
{
    run : anAction
    [
        ReaderScan &&io'path:aFileName &:io'AReadOnlyTextFile run:
            aLine = ( Scan::aLine run:anAction).
    ]
}.

// --- Interpreter ---

#class Interpreter
{
    #field theTapeIterator.

    #field theLoopTape.

    #role Looping
    {
        #method $resetLoop
        [
            theLoopTape clear.
        ]

        #method evaluate : aChar
        [
            #if aChar
            == "]" ? 
            [
                #while (0 < theTapeIterator)?
                    [ Scan::theLoopTape run:Interpreter::theTapeIterator. ].

                #shift.
            ]
            | [
                theLoopTape += WideStrValue::aChar.
            ].
        ]
    }
    #initializer : aTapeIterator
    [
        theLoopTape := string.
        theTapeIterator := aTapeIterator.
    ]

    #method evaluate : aChar
    [
        #if aChar
            ifequal:">"   [ theTapeIterator $next. ]
            | ifequal:"<" [ theTapeIterator $previous. ]
            | ifequal:"+" [ theTapeIterator += 1. ]
            | ifequal:"-" [ theTapeIterator -= 1. ]
            | ifequal:"." [ 'program'Output << theTapeIterator. ]
            | ifequal:"," [ 'program'Input get. ]
            | ifequal:"[" [ #shift Looping. self $resetLoop. ].
    ]
}

// --- Program ---

#symbol Program = 
{
    start : Arguments
    [
        #if (Arguments count == 1)?
            [  'program'output << "Please provide the path to the file to interpret". control fail. ].
    
        #var aFileName := Arguments@1.
    
        FileCharEnumerator::aFileName run:Interpreter::TapeIterator.
    ]
}.