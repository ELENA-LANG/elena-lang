#define system.

// --- ScriptException ---

#class ScriptException : Exception
{
    #method onScriptError : aHandler    
    [
        aHandler eval:$self.
    ]
        
    #method ifFailed &on:aCatchBlock
    [
        aCatchBlock onScriptError:$self.
    ]
}

// --- scriptControl --

#symbol scriptControl =
{
    load &path:aPath
        => &load &literal::aPath &int:0 &int:-1.
        
    eval &literal:aScript
        <vmscript_eval>.
    
    load &literal:aScript &int:anEncoding &int:autodetect
        <vmscript_load>.
}.

#symbol scriptLastError = control do:
[
    #var anErrorPtr := IntNumber new &int:(system'external'elenasm GetStatus).
    ^ (anErrorPtr != 0) ?
    [
        #var anErrorLen := IntNumber new &int:(system'external'KERNEL32 lstrlenW &int:anErrorPtr).
        #var anError := LiteralValue new &length:anErrorLen.
        system'external'kernel32 lstrcpyW 
            &out'literal:anError &int:anErrorPtr.
    
        ^ anError.
    ]
    ! [ emptyLiteralValue ].
].
