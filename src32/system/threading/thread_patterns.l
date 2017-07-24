
// --- lock statement

#template lock(expr)
[
    finally(expr enterCriticalSection then:[])
        [ expr leaveCriticalSection. ].
].