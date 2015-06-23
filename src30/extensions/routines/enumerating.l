#define system.
#define extensions'text.

#subject(class:system'BaseFunction1)eachWord.

// --- stringExOp ---

#class(extension)stringExOp
{
    #method run &eachWord:aFunction
        = aFunction run &enumerator:(WordEnumerator new &literal:(self literal)).
}
