#define system.

// --- interpreter ---

#symbol interpreter =
{
    run &name:aName &script:aScript &mode:aMode
    [
        #var aTape := IntNumber new &type'int:(system'externals'elenasm TranslateLVMTape
            &type'widestr::aName
            &type'widestr::aScript
            &type'int::aMode).
            
        system'external'VMAPI interprete &type'int:aTape.
            
        system'externals'elenasm FreeLVMTape
            &type'int:aTape.
    ]
}.