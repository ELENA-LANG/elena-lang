// --- referenceTableControl ---

#symbol referenceTableControl =
{
    allocate &object:anObject &out'int:anIndex
        <api_alloc_index>.

    free &int:anIndex
        <api_free_index>.
}.

// --- primitive ---

#symbol primitive =
{
    shift &int:aValue &int:aShift &out'int:aRetVal
        <int_shift>.
        
    shift &long:aValue &int:aShift &out'long:aRetVal
        <long_shift>.
        
    invert &int:aValue &out'int:aRetVal
        <int_not>.
        
    invert &long:aValue &out'long:aRetVal
        <long_not>.

    toLiteral &int:aValue
        <ws_copyint>.
         
    toLiteral &int:aValue &index:aRadix
        <ws_copyint_radix>.
        
    toLiteral &long:aValue
        <ws_copylong>.

    toLiteral &long:aValue &index:aRadix
        <ws_copylong_radix>.
                
    toLiteral &bytearray:aValue &length:aLength
        <ws_copybuf>.
        
    toLiteral &real:aValue
        <ws_copyreal>.
        
    toLiteral &real:aValue &length:aLength
        <ws_copyreal_len>.
        
    concat &literal:s1 &literal:s2
        <ws_concat>.

    readLength &array:anObject &out'int:aRetVal
        <array_getlen>.
        
    convert &literal:aValue &out'int:aRetVal
        <n_copystr>.
        
    convert &literal:aValue &out'long:aRetVal
        <l_copystr>.
        
    convert &literal:aValue &out'real:aRetVal
        <r_copystr>.
        
    insert &literal:anS &index:anIndex &literal:aSubS
        <ws_insert>.
        
    delete &literal:anS &index:anIndex &length:aLength
        <ws_delete>.
        
    Substring &literal:anS &index:anIndex &length:aLength
        <ws_subs>.
        
    refresh &literal:anS
        <ws_refresh>.
        
    write &bytearray:anArray &index:anIndex &short:aWord
        <bs_writeword>.
        
    read &bytearray:anArray &index:anIndex &out'short:aWord
        <bs_readword>.
        
//    read &bytearray:anArray &index:anIndex &out'byte:aWord
//        <bs_readbyte>.
        
    equal &bytearray:anArray &index:anIndex &short:aWord
        <bs_equalword>.
       
    equal &bytearray:anArray &index:anIndex &int:aNumber
        <bs_equalint>.
}.

// --- mathPrimitive ---

#symbol mathPrimitive =
{
    Exp &real:aValue &out'real:aRetVal
        <r_exp>.
        
    Ln &real:aValue &out'real:aRetVal
        <r_ln>.
        
    Abs &real:aValue &out'real:aRetVal
        <r_abs>.
        
    Round &real:aValue &out'real:aRetVal
        <r_round>.
        
    Truncate &real:aValue &out'real:aRetVal
        <r_truncate>.
}.

