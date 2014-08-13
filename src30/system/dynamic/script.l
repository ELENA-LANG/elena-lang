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
    #var(type:int)anErrorPtr := system'external'elenasm GetStatus.
    ^ (anErrorPtr != 0) ?
    [
        #var(type:shortarray,size:255)aBuffer.
        #var(type:int)anErrorLen := system'external'KERNEL32 lstrlenW &int:anErrorPtr.
        
        system'external'kernel32 lstrcpyW 
            &out'shortarray:aBuffer &int:anErrorPtr.
    
        ^ LiteralValue new &length:anErrorLen &shortarray:aBuffer.
    ]
    ! [ emptyLiteralValue ].
].
