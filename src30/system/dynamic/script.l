#define system.

// --- scriptControl --

#symbol scriptControl =
{
    load &literal:aScript
    [
        #var aRetVal := system'external'elenasm EvaluateScript &literal:aScript.
        
        ^ aRetVal.
    ]
    
//    LastScriptError
//    [
//        #var anErrorPtr := IntNumber new &int:(system'external'elenasm GetLSMStatus).
//        ^ (anErrorPtr != 0) ?
//        [
//            #var anErrorLen := IntNumber new &int:(system'external'KERNEL32 lstrlenW &int:anErrorPtr).
//            #var anError := LiteralValue new &length:anErrorLen.
//            system'external'kernel32 lstrcpyW 
//                &out'literal:anError &int:anErrorPtr.
//        
//            ^ anError.
//        ]
//        ! [ emptyLiteralValue ].
//    ]
//    
//    load &path:aPath
//    [
//        #var aTapePtr := IntNumber new &int:(system'external'elenasm TranslateLVMFile &literal:"default" &literal:aPath &int:0 &int:-1 &int:0
//            | raise:(system'FormatException new:($self LastScriptError))).
//        
//        #var retVal := $self eval &vm_script:aTapePtr.
//        
//        system'external'elenasm FreeLVMTape &int:aTapePtr.
//        
//        ^ retVal.
//    ]
//    
//    eval &vm_script: aTape
//        <vmscript_eval>.
}.