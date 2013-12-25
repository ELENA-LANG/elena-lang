#define system.

// --- scriptControl --

#symbol scriptControl =
{
    LastScriptError
    [
        #var anErrorPtr := IntNumber new &type'int:(system'external'elenasm GetLSMStatus).
        ^ (anErrorPtr != 0) ?
        [
            #var anErrorLen := IntNumber new &type'int:(system'external'KERNEL32 lstrlenW &type'int:anErrorPtr).
            #var anError := system'external'kernel32 lstrcpyW 
                &out'type'widestr:(LiteralValue new &type'length:anErrorLen)
                &type'int:anErrorPtr.
        
            ^ anError.
        ]
        ! [ emptyLiteralValue ].
    ]
    
    load &path:aPath
    [
        #var aTapePtr := IntNumber new &type'int:(system'external'elenasm TranslateLVMFile &type'widestr:"default" &type'widestr:aPath &type'int:0 &type'int:-1 &type'int:0
            | raise:(system'FormatException new:($self LastScriptError))).
        
        #var retVal := $self eval &vm_script:aTapePtr.
        
        system'external'elenasm FreeLVMTape &type'int:aTapePtr.
        
        ^ retVal.
    ]
    
    eval &vm_script: aTape
        <vmscript_eval>.
}.