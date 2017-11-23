// --- lock statement

__block template lock :: expr
[
    expr enterCriticalSection; doFinally([],[ expr leaveCriticalSection ]).
]

    
