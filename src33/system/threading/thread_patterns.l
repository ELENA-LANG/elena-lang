// --- lock statement

template lock :: expr =
    expr enterCriticalSection; doFinally([],[ expr leaveCriticalSection ]).
