// --- intOp ---

#symbol intOp =
{
    shift &type'int:aValue &type'int:aShift &out'type'int:aRetVal
        <int_shift>.
        
    shift &type'long:aValue &type'int:aShift &out'type'long:aRetVal
        <long_shift>.
        
    invert &type'int:aValue &out'type'int:aRetVal
        <int_not>.
        
    invert &type'long:aValue &out'type'long:aRetVal
        <long_not>.
}.

// --- strOp ---

#symbol strOp =
{
    convert &type'int:aValue
        <ws_copyint>.
         
    convert &type'int:aValue &type'index:aRadix
        <ws_copyint_radix>.
        
    convert &type'long:aValue
        <ws_copylong>.
        
    convert &type'long:aValue &type'index:aRadix
        <ws_copylong_radix>.
        
    convert &type'real:aValue
        <ws_copyreal>.
        
    convert &type'real:aValue &type'length:aLength
        <ws_copyreal_len>.
        
    convert &type'bytearray:aValue &type'length:aLength
        <ws_copybuf>.
        
    convert &type'widestr:aValue &out'type'int:aRetVal
        <n_copystr>.
        
    convert &type'widestr:aValue &out'type'long:aRetVal
        <l_copystr>.
        
    convert &type'widestr:aValue &out'type'real:aRetVal
        <r_copystr>.
        
    insert &type'widestr:anS &type'index:anIndex &type'widestr:aSubS
        <ws_insert>.
        
    delete &type'widestr:anS &type'index:anIndex &type'length:aLength
        <ws_delete>.
        
    substring &type'widestr:anS &type'index:anIndex &type'length:aLength
        <ws_subs>.
        
    refresh &type'widestr:anS
        <ws_refresh>.
}.

// --- byteArrayOp ---

#symbol byteArrayOp =
{
    equal &type'bytearray:anArray &type'index:anIndex &type'short:aWord
        <bs_equalword>.
       
    equal &type'bytearray:anArray &type'index:anIndex &type'int:aNumber
        <bs_equalint>.
       
    write &type'bytearray:anArray &type'index:anIndex &type'short:aWord
        <bs_writeword>.
        
    read &type'bytearray:anArray &type'index:anIndex &out'type'short:aWord
        <bs_readword>.
}.

// --- arrayOp ---

#symbol arrayOp =
{
    read &type'length:anObject &out'type'int:aRetVal
        <array_getlen>.
}.