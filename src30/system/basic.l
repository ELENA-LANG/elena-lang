// === basic data classes ===

// --- BaseValue ---

#class BaseValue
{
    #method equal : anObject = anObject cast:%equal &to:$self bool.

    #method(type:bool) less : anObject = anObject cast:%less &to:$self bool.

    #method(type:bool) greater : anObject = anObject less:$self bool.
}

// --- BaseNumber ---

#class BaseNumber :: BaseValue
{
    #method add : anObject = anObject cast:%add &to:$self.

    #method subtract : anObject = anObject cast:%subtract &to:$self.

    #method multiply : anObject = anObject cast:%multiply &to:$self.

    #method divide : anObject = anObject cast:%divide &to:$self.
}

// --- IntBaseNumber ---

#class IntBaseNumber :: BaseNumber
{
    #method and : anObject = anObject cast:%and &to:$self.
    
    #method or : anObject = anObject cast:%or &to:$self.
    
    #method xor : anObject = anObject cast:%xor &to:$self.
    
    #method allMask : anOperand
        = $self and:anOperand == anOperand.
        
    #method anyMask : anOperand
        = 0 != $self and:anOperand.
        
    #method negative = 0 subtract:$self.
}

// --- IntBaseVariable ---

#class IntBaseVariable :: IntBaseNumber
{
    #method write : anOperand
    [
        anOperand cast:%write &to:$self.
    ]
    
    #method append : anOperand
    [
        anOperand cast:%append &to:$self.
    ]
    
    #method reduce : anOperand
    [
        anOperand cast:%reduce &to:$self.
    ]
    
    #method multiplyBy : anOperand
    [
        anOperand cast:%multiplyBy &to:$self.
    ]
    
    #method divideInto : anOperand
    [
        anOperand cast:%divideInto &to:$self.
    ]
}

// --- BaseNumber ---

#class BaseVariable :: BaseNumber
{
    #method write : anOperand
    [
        anOperand cast:%write &to:$self.
    ]
    
    #method append : anOperand
    [
        anOperand cast:%append &to:$self.
    ]
    
    #method reduce : anOperand
    [
        anOperand cast:%reduce &to:$self.
    ]
    
    #method multiplyBy : anOperand
    [
        anOperand cast:%multiplyBy &to:$self.
    ]
    
    #method divideInto : anOperand
    [
        anOperand cast:%divideInto &to:$self.
    ]
}

// --- ByteNumber ---

#class(sealed,integer:1) ByteNumber :: IntBaseNumber
{
    #constructor min
        => system'internal'byte_min.
    
    #constructor max
        => system'internal'byte_max.
    
    #constructor new &byte:anOperand
        => system'internal'int_copy.

    #constructor new
        => system'internal'int_init.

    #method byte = $self.

    #method short = ShortNumber new &byte:$self.

    #method int = IntNumber new &byte:$self.

    #method long = LongNumber new &byte:$self.

    #method real = RealNumber new &byte:$self.

    #method(type:bool) equal &byte:anOperand
        => system'internal'int_equal.
        
    #method(type:bool) less &byte:anOperand
        => system'internal'int_less.

    #method(type:bool) equal &short:anOperand
        => system'internal'int_equal.
        
    #method(type:bool) less &short:anOperand
        = ShortNumber new &byte:$self less &short:anOperand.

    #method(type:bool) equal &int:anOperand
        => system'internal'int_equal.
        
    #method(type:bool) less &int:anOperand
        = IntNumber new &byte:$self less &int:anOperand.

    #method(type:bool) equal &long:anOperand
        = LongNumber new &byte:$self equal &long:anOperand.
        
    #method(type:bool) less &long:anOperand
        = LongNumber new &byte:$self less &long:anOperand.

    #method(type:bool) equal &real:anOperand
        = RealNumber new &byte:$self equal &real:anOperand.
        
    #method(type:bool) less &real:anOperand
        = RealNumber new &byte:$self less &real:anOperand.

    #method(type:byte) add &byte:anOperand
        => system'internal'byte_add.

    #method(type:byte) subtract &byte:anOperand
        => system'internal'byte_sub.

    #method(type:byte) multiply &byte:anOperand
        => system'internal'byte_mul.

    #method(type:byte) divide &byte:anOperand
        => system'internal'byte_div.

    #method add &short:anOperand
        = ShortNumber new &byte:$self add &short:anOperand.

    #method subtract &short:anOperand
        = ShortNumber new &byte:$self subtract &short:anOperand.

    #method multiply &short:anOperand
        = ShortNumber new &byte:$self multiply &short:anOperand.

    #method divide &short:anOperand
        = ShortNumber new &byte:$self divide &short:anOperand.

    #method add &int:anOperand
        = IntNumber new &byte:$self add &int:anOperand.

    #method subtract &int:anOperand
        = IntNumber new &byte:$self subtract &int:anOperand.

    #method multiply &int:anOperand
        = IntNumber new &byte:$self multiply &int:anOperand.

    #method divide &int:anOperand
        = IntNumber new &byte:$self divide &int:anOperand.

    #method add &long:anOperand
        = LongNumber new &byte:$self add &long:anOperand.

    #method subtract &long:anOperand
        = LongNumber new &byte:$self subtract &long:anOperand.

    #method multiply &long:anOperand
        = LongNumber new &byte:$self multiply &long:anOperand.

    #method divide &long:anOperand
        = LongNumber new &byte:$self divide &long:anOperand.

    #method add &real:anOperand
        = RealNumber new &byte:$self add &real:anOperand.

    #method subtract &real:anOperand
        = RealNumber new &byte:$self subtract &real:anOperand.

    #method multiply &real:anOperand
        = RealNumber new &byte:$self multiply &real:anOperand.

    #method divide &real:anOperand
        = RealNumber new &byte:$self divide &real:anOperand.

    #method(type:byte) and &byte:anOperand
        => system'internal'byte_and.

    #method(type:byte) or &byte:anOperand
        => system'internal'byte_or.

    #method(type:byte) xor &byte:anOperand
        => system'internal'byte_xor.

    #method and &short:anOperand
        = ShortNumber new &byte:$self and &short:anOperand.

    #method or &short:anOperand
        = ShortNumber new &byte:$self or &short:anOperand.

    #method xor &short:anOperand
        = ShortNumber new &byte:$self xor &short:anOperand.

    #method(type:int) and &int:anOperand
        => system'internal'int_and.

    #method(type:int) or &int:anOperand
        => system'internal'int_or.

    #method(type:int) xor &int:anOperand
        => system'internal'int_xor.

    #method(type:long) and &long:anOperand
        => system'internal'long_and.

    #method(type:long) or &long:anOperand
        => system'internal'long_or.

    #method(type:long) xor &long:anOperand
        => system'internal'long_xor.

    #method(type:byte) shift &index:anOrder
        => system'internal'byte_shift.

    #method literal = literalConvertor convert &byte:$self.

    #method wide = wideConvertor convert &byte:$self.

    #method(type:byte) not
        => system'internal'byte_not.

    #method clone = ByteNumber new &byte:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &byte:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &byte:$self.
}

// --- ShortNumber ---

#class(sealed,integer:2) ShortNumber :: IntBaseNumber
{
    #constructor min
        => system'internal'short_min.
    
    #constructor max
        => system'internal'short_max.
    
    #constructor new &byte:anOperand
        => system'internal'int_copy.
        
    #constructor new &short:anOperand
        => system'internal'int_copy.

    #method byte = byteConvertor convert &short:$self.
                
    #method short = $self.
    
    #method int = IntNumber new &short:$self.
    
    #method long = LongNumber new &short:$self.
    
    #method real = RealNumber new &short:$self.
    
    #method(type:bool) equal &int:anOperand 
        => system'internal'int_equal.
        
    #method(type:bool) less &int:anOperand
        => system'internal'int_less.
    
    #method(type:bool) equal &short:anOperand 
        => system'internal'int_equal.
        
    #method(type:bool) less &short:anOperand
        => system'internal'int_less.
    
    #method(type:bool) equal &byte:anOperand 
        => system'internal'int_equal.
        
    #method(type:bool) less &byte:anOperand
        => system'internal'int_less.
    
    #method(type:bool) equal &long:anOperand 
        = LongNumber new &short:$self equal &long:anOperand.
        
    #method(type:bool) less &long:anOperand
        = LongNumber new &short:$self less &long:anOperand.
    
    #method(type:bool) equal &real:anOperand 
        = RealNumber new &short:$self equal &real:anOperand.
        
    #method(type:bool) less &real:anOperand
        = RealNumber new &short:$self less &real:anOperand.

    #method(type:short) add &short:anOperand
        => system'internal'short_add.

    #method(type:short) subtract &short:anOperand
        => system'internal'short_sub.

    #method(type:short) multiply &short:anOperand
        => system'internal'short_mul.

    #method(type:short) divide &short:anOperand
        => system'internal'short_div.

    #method add &byte:anOperand
        = $self add &short:(ShortNumber new &byte:anOperand).

    #method subtract &byte:anOperand
        = $self subtract &short:(ShortNumber new &byte:anOperand).

    #method multiply &byte:anOperand
        = $self multiply &short:(ShortNumber new &byte:anOperand).

    #method divide &byte:anOperand
        = $self divide &short:(ShortNumber new &byte:anOperand).

    #method add &int:anOperand
        = IntNumber new &short:$self add &int:anOperand.

    #method subtract &int:anOperand
        = IntNumber new &short:$self subtract &int:anOperand.

    #method multiply &int:anOperand
        = IntNumber new &short:$self multiply &int:anOperand.
        
    #method divide &int:anOperand
        = IntNumber new &short:$self divide &int:anOperand.

    #method add &long:anOperand
        = LongNumber new &short:$self add &long:anOperand.

    #method subtract &long:anOperand
        = LongNumber new &short:$self subtract &long:anOperand.

    #method multiply &long:anOperand
        = LongNumber new &short:$self multiply &long:anOperand.
        
    #method divide &long:anOperand
        = LongNumber new &short:$self divide &long:anOperand.

    #method add &real:anOperand
        = RealNumber new &short:$self add &real:anOperand.

    #method subtract &real:anOperand
        = RealNumber new &short:$self subtract &real:anOperand.

    #method multiply &real:anOperand
        = RealNumber new &short:$self multiply &real:anOperand.
        
    #method divide &real:anOperand
        = RealNumber new &short:$self divide &real:anOperand.

    #method(type:short) and &short:anOperand
        => system'internal'short_and.

    #method(type:short) or &short:anOperand
        => system'internal'short_or.

    #method(type:short) xor &short:anOperand
        => system'internal'short_xor.

    #method and &byte:anOperand
        => system'internal'short_and.

    #method or &byte:anOperand
        => system'internal'short_or.

    #method xor &byte:anOperand
        => system'internal'short_xor.
        
    #method and &int:anOperand
        = IntNumber new &short:$self and &int:anOperand.        
        
    #method or &int:anOperand
        = IntNumber new &short:$self or &int:anOperand.        
        
    #method xor &int:anOperand
        = IntNumber new &short:$self xor &int:anOperand.        
        
    #method and &long:anOperand
        = LongNumber new &short:$self and &long:anOperand.        
        
    #method or &long:anOperand
        = LongNumber new &short:$self or &long:anOperand.        
        
    #method xor &long:anOperand
        = LongNumber new &short:$self xor &long:anOperand.        

    #method(type:short) shift &index:anOrder
        => system'internal'short_shift.

    #method literal = literalConvertor convert &short:$self.

    #method wide = wideConvertor convert &short:$self.

    #method(type:byte) not
        => system'internal'short_not.

    #method clone = ShortNumber new &short:$self.    
                
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &short:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &short:$self.
}

// --- IntNumber ---

#class(sealed,integer:4) IntNumber :: IntBaseNumber
{
    #constructor min
        => system'internal'int_min.
    
    #constructor max
        => system'internal'int_max.
    
    #constructor new &int:anOperand
        => system'internal'int_copy.

    #constructor new &short:anOperand
        => system'internal'int_copyshort.

    #constructor new &byte:anOperand
        => system'internal'int_copy.

    #constructor new
        => system'internal'int_init.

    #method int = $self.
    
    #method short = shortConvertor convert &int:$self.
    
    #method byte = byteConvertor convert &int:$self.

    #method long = LongNumber new &int:$self.

    #method real = RealNumber new &int:$self.

    #method(type:bool) equal &int:anOperand 
        => system'internal'int_equal.
        
    #method(type:bool) equal &short:anOperand 
        => system'internal'int_equal.
        
    #method(type:bool) equal &byte:anOperand 
        => system'internal'int_equal.
        
    #method equal &long:anOperand 
        = LongNumber new &int:$self equal &long:anOperand.

    #method equal &real:anOperand 
        = RealNumber new &int:$self equal &real:anOperand.

    #method(type:bool) less &int:anOperand
        => system'internal'int_less.

    #method(type:bool) less &short:anOperand
        => system'internal'int_less.

    #method(type:bool) less &byte:anOperand
        => system'internal'int_less.
        
    #method less &long:anOperand 
        = LongNumber new &int:$self less &long:anOperand.

    #method less &real:anOperand 
        = RealNumber new &int:$self less &real:anOperand.

    #method(type:int) add &int:anOperand
        => system'internal'int_add.
        
    #method(type:int) add &short:anOperand
        => system'internal'int_add.

    #method(type:int) add &byte:anOperand
        => system'internal'int_add.

    #method add &long:anOperand
        = LongNumber new &int:$self add &long:anOperand.

    #method add &real:anOperand
        = RealNumber new &int:$self add &real:anOperand.

    #method(type:int) subtract &int:anOperand
        => system'internal'int_sub.

    #method(type:int) subtract &short:anOperand
        => system'internal'int_sub.

    #method(type:int) subtract &byte:anOperand
        => system'internal'int_sub.

    #method subtract &long:anOperand
        = LongNumber new &int:$self subtract &long:anOperand.

    #method subtract &real:anOperand
        = RealNumber new &int:$self subtract &real:anOperand.

    #method(type:int) multiply &int:anOperand
        => system'internal'int_mul.
        
    #method(type:int) multiply &short:anOperand
        => system'internal'int_mul.

    #method(type:int) multiply &byte:anOperand
        => system'internal'int_mul.

    #method multiply &long:anOperand
        = LongNumber new &int:$self multiply &long:anOperand.

    #method multiply &real:anOperand
        = RealNumber new &int:$self multiply &real:anOperand.

    #method(type:int) divide &int:anOperand
        => system'internal'int_div.

    #method(type:int) divide &short:anOperand
        => system'internal'int_div.

    #method(type:int) divide &byte:anOperand
        => system'internal'int_div.

    #method divide &long:anOperand
        = LongNumber new &int:$self divide &long:anOperand.

    #method divide &real:anOperand
        = RealNumber new &int:$self divide &real:anOperand.

    #method(type:int) and &int:anOperand
        => system'internal'int_and.

    #method(type:int) and &short:anOperand
        => system'internal'int_and.

    #method(type:int) and &byte:anOperand
        => system'internal'int_and.

    #method and &long:anOperand
        = LongNumber new &int:$self and &long:anOperand.

    #method(type:int) or &int:anOperand
        => system'internal'int_or.

    #method(type:int) or &short:anOperand
        => system'internal'int_or.

    #method(type:int) or &byte:anOperand
        => system'internal'int_or.

    #method or &long:anOperand
        = LongNumber new &int:$self or &long:anOperand.

    #method(type:int) xor &int:anOperand
        => system'internal'int_xor.

    #method(type:int) xor &short:anOperand
        => system'internal'int_xor.

    #method(type:int) xor &byte:anOperand
        => system'internal'int_xor.

    #method xor &long:anOperand
        = LongNumber new &int:$self xor &long:anOperand.

    #method(type:int) shift &index:anOrder
        => system'internal'int_shift.

    #method(type:int) shift &index:anOrder &vint:aResult
        => system'internal'int_shift2.

    #method literal = literalConvertor convert &int:$self.

    #method wide = wideConvertor convert &int:$self.

    #method(type:int) not
        => system'internal'int_not.

    #method negative
        = 0 - $self.

    #method clone = IntNumber new &int:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &int:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &int:$self.
}

// --- longnumber ---

#class(sealed,integer:8) LongNumber :: IntBaseNumber
{
    #constructor min
        => system'internal'long_min.
    
    #constructor max
        => system'internal'long_max.
    
    #constructor new
        <= new &long:0l.

    #constructor new &long:anOperand 
        => system'internal'long_copy.

    #constructor new &int:anOperand 
        => system'internal'long_copyint.

    #constructor new &short:anOperand 
        => system'internal'long_copyint.

    #constructor new &byte:anOperand 
        => system'internal'long_copyint.
        
    #method int = intConvertor convert &long:$self.
    
    #method short = shortConvertor convert &long:$self.
    
    #method byte = byteConvertor convert &long:$self.

    #method long = $self.

    #method real = RealNumber new &long:$self.

    #method(type:bool) equal &long:anOperand
        => system'internal'long_equal.

    #method(type:bool) equal &int:anOperand 
        = $self equal &long:(system'LongNumber new &int:anOperand).

    #method(type:bool) equal &short:anOperand 
        = $self equal &long:(system'LongNumber new &short:anOperand).

    #method(type:bool) equal &byte:anOperand 
        = $self equal &long:(system'LongNumber new &byte:anOperand).

    #method equal &real:anOperand 
        = RealNumber new &long:$self equal &real:anOperand.

    #method(type:bool) less &long:anOperand
        => system'internal'long_less.

    #method(type:bool) less &int:anOperand 
        = $self less &long:(system'LongNumber new &int:anOperand).

    #method(type:bool) less &short:anOperand 
        = $self less &long:(system'LongNumber new &short:anOperand).

    #method(type:bool) less &byte:anOperand 
        = $self less &long:(system'LongNumber new &byte:anOperand).

    #method less &real:anOperand 
        = RealNumber new &long:$self less &real:anOperand.

    #method(type:long) add &long:anOperand
        => system'internal'long_add.

    #method add &int:anOperand
        = $self add &long:(system'LongNumber new &int:anOperand).

    #method add &short:anOperand
        = $self add &long:(system'LongNumber new &short:anOperand).

    #method add &byte:anOperand
        = $self add &long:(system'LongNumber new &byte:anOperand).

    #method add &real:anOperand
        = RealNumber new &long:$self add &real:anOperand.

    #method(type:long) subtract &long:anOperand
        => system'internal'long_sub.

    #method subtract &int:anOperand
        = $self subtract &long:(system'LongNumber new &int:anOperand).

    #method subtract &short:anOperand
        = $self subtract &long:(system'LongNumber new &short:anOperand).

    #method subtract &byte:anOperand
        = $self subtract &long:(system'LongNumber new &byte:anOperand).

    #method subtract &real:anOperand
        = RealNumber new &long:$self subtract &real:anOperand.

    #method(type:long) multiply &long:anOperand
        => system'internal'long_mul.

    #method multiply &real:anOperand
        = RealNumber new &long:$self multiply &real:anOperand.

    #method multiply &int:anOperand
        = $self multiply &long:(system'LongNumber new &int:anOperand).

    #method multiply &byte:anOperand
        = $self multiply &long:(system'LongNumber new &byte:anOperand).

    #method multiply &short:anOperand
        = $self multiply &long:(system'LongNumber new &short:anOperand).

    #method(type:long) divide &long:anOperand
        => system'internal'long_div.

    #method divide &int:anOperand
        = $self divide &long:(system'LongNumber new &int:anOperand).

    #method divide &short:anOperand
        = $self divide &long:(system'LongNumber new &short:anOperand).

    #method divide &byte:anOperand
        = $self divide &long:(system'LongNumber new &byte:anOperand).

    #method divide &real:anOperand
        = RealNumber new &long:$self divide &real:anOperand.
        
    #method(type:long) and &long:anOperand
        => system'internal'long_and.

    #method and &int:anOperand
        = $self and &long:(system'LongNumber new &int:anOperand).

    #method and &short:anOperand
        = $self and &long:(system'LongNumber new &short:anOperand).

    #method and &byte:anOperand
        = $self and &long:(system'LongNumber new &byte:anOperand).

    #method(type:long) or &long:anOperand
        => system'internal'long_or.

    #method or &int:anOperand
        = $self or &long:(system'LongNumber new &int:anOperand).

    #method or &short:anOperand
        = $self or &long:(system'LongNumber new &short:anOperand).

    #method or &byte:anOperand
        = $self or &long:(system'LongNumber new &byte:anOperand).        

    #method(type:long) xor &long:anOperand
        => system'internal'long_xor.

    #method xor &int:anOperand
        = $self xor &long:(system'LongNumber new &int:anOperand).

    #method xor &short:anOperand
        = $self xor &long:(system'LongNumber new &short:anOperand).

    #method xor &byte:anOperand
        = $self xor &long:(system'LongNumber new &byte:anOperand).

    #method(type:long) shift &index:anOrder
        => system'internal'long_shift.

    #method literal = literalConvertor convert &long:$self.

    #method wide = wideConvertor convert &long:$self.

    #method(type:long) negative = 0l - $self.

    #method(type:long) not
        => system'internal'long_not.

    #method clone = LongNumber new &long:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &long:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &long:$self.
}

// --- realNumber ---

#class(sealed,floating:8) RealNumber :: BaseNumber
{
    #constructor min
        <= new &real: -1.7976931348623157e+308r.
    
    #constructor max
        <= new &real: 1.7976931348623157e+308r.
    
    #constructor new
        <= new &real:0.0r.

    #constructor new &real:anOperand 
        => system'internal'long_copy.
  
    #constructor new &int:anOperand 
        => system'internal'real_copyint.

    #constructor new &short:anOperand 
        => system'internal'real_copyint.

    #constructor new &byte:anOperand 
        => system'internal'real_copyint.

    #constructor new &long:anOperand 
        => system'internal'real_copylong.
        
    #method real = $self.
    
    #method(type:bool) equal &real:anOperand
        => system'internal'real_equal.

    #method(type:bool) equal &int:anOperand
        = $self equal &real:(system'RealNumber new &int:anOperand).

    #method(type:bool) equal &byte:anOperand
        = $self equal &real:(system'RealNumber new &byte:anOperand).

    #method(type:bool) equal &short:anOperand
        = $self equal &real:(system'RealNumber new &short:anOperand).

    #method(type:bool) equal &long:anOperand
        = $self equal &real:(system'RealNumber new &long:anOperand).

    #method(type:bool) less &real:anOperand
        => system'internal'real_less.

    #method(type:bool) less &int:anOperand
        = $self less &real:(system'RealNumber new &int:anOperand).

    #method(type:bool) less &byte:anOperand
        = $self less &real:(system'RealNumber new &byte:anOperand).

    #method(type:bool) less &short:anOperand
        = $self less &real:(system'RealNumber new &short:anOperand).

    #method(type:bool) less &long:anOperand
        = $self less &real:(system'RealNumber new &long:anOperand).

    #method(type:real) add &real:anOperand
        => system'internal'real_add.

    #method(type:real) add &int:anOperand
        = $self add &real:(system'RealNumber new &int:anOperand).

    #method(type:real) add &byte:anOperand
        = $self add &real:(system'RealNumber new &byte:anOperand).

    #method(type:real) add &short:anOperand
        = $self add &real:(system'RealNumber new &short:anOperand).

    #method(type:real) add &long:anOperand
        = $self add &real:(system'RealNumber new &long:anOperand).

    #method(type:real) subtract &real:anOperand
        => system'internal'real_sub.

    #method(type:real) subtract &int:anOperand
        = $self subtract &real:(system'RealNumber new &int:anOperand).

    #method(type:real) subtract &byte:anOperand
        = $self subtract &real:(system'RealNumber new &byte:anOperand).

    #method(type:real) subtract &short:anOperand
        = $self subtract &real:(system'RealNumber new &short:anOperand).

    #method(type:real) subtract &long:anOperand
        = $self subtract &real:(system'RealNumber new &long:anOperand).

    #method(type:real) multiply &real:anOperand
        => system'internal'real_mul.

    #method(type:real) multiply &int:anOperand
        = $self multiply &real:(system'RealNumber new &int:anOperand).

    #method(type:real) multiply &byte:anOperand
        = $self multiply &real:(system'RealNumber new &byte:anOperand).

    #method(type:real) multiply &short:anOperand
        = $self multiply &real:(system'RealNumber new &short:anOperand).

    #method(type:real) multiply &long:anOperand
        = $self multiply &real:(system'RealNumber new &long:anOperand).

    #method(type:real) divide &real:anOperand
        => system'internal'real_div.

    #method(type:real) divide &int:anOperand
        = $self divide &real:(system'RealNumber new &int:anOperand).

    #method(type:real) divide &byte:anOperand
        = $self divide &real:(system'RealNumber new &byte:anOperand).
        
    #method(type:real) divide &short:anOperand
        = $self divide &real:(system'RealNumber new &short:anOperand).

    #method(type:real) divide &long:anOperand
        = $self divide &real:(system'RealNumber new &long:anOperand).

    #method negative = 0r - $self.

    #method literal = literalConvertor convert &real:$self.
    
    #method wide = wideConvertor convert &real:$self.
    
    #method clone = RealNumber new &real:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &real:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &real:$self.
}

// --- Byte ---

#class(sealed,struct:embeddable) Byte :: IntBaseVariable
{
    #field(type:byte)theValue.
    
    #constructor new
    [
        theValue := 0.
    ]
    
    #constructor new : aValue
    [
        theValue := aValue byte.
    ]
    
    #constructor new &byte:aValue
    [
        theValue := aValue.
    ]
    
    #method vbyte = $self.
    
    #method byte = ByteNumber new &byte:theValue.
                                                
    #method short = ShortNumber new &byte:theValue.
                                                
    #method int = IntNumber new &byte:theValue.
                                                
    #method real = RealNumber new &byte:theValue.
    
    #method long = LongNumber new &byte:theValue.
    
    #method write &byte:anOperand
    [
        theValue := anOperand.
    ]

    #method append &byte:anOperand
    [
        theValue := theValue + anOperand.
    ]
        
    #method reduce &byte:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    #method multiplyBy &byte:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    #method divideInto &byte:anOperand
    [
        theValue := theValue / anOperand.
    ]
    
    #method(type:bool) equal &byte:anOperand 
        = theValue == anOperand.

    #method(type:bool) less &byte:anOperand
        = theValue < anOperand.
                
    #method(type:byte) add &byte:anOperand
        => system'internal'byte_add.
    
    #method(type:byte) subtract &byte:anOperand
        => system'internal'byte_sub.
            
    #method(type:byte) multiply &byte:anOperand
        => system'internal'byte_mul.
    
    #method(type:byte) divide &byte:anOperand
        => system'internal'byte_div.
        
    #method(type:byte) and &byte:anOperand
        => system'internal'byte_and.
        
    #method(type:byte) or &byte:anOperand
        => system'internal'byte_or.
        
    #method(type:byte) xor &byte:anOperand
        => system'internal'byte_xor.

    #method value = ByteNumber new &byte:theValue.

    #method literal = literalConvertor convert &byte:theValue.

    #method wide = wideConvertor convert &byte:theValue.

    #method clone = Byte new &byte:theValue.

    #method => theValue.    
}

// --- Short ---

#class(sealed,struct:embeddable) Short :: IntBaseVariable
{
    #field(type:short)theValue.
    
    #constructor new
    [
        theValue := 0.
    ]
    
    #constructor new : aValue
    [
        theValue := aValue short.
    ]
    
    #constructor new &short:aValue
    [
        theValue := aValue.
    ]
    
    #constructor new &byte:aValue
    [
        theValue := aValue short.
    ]
    
    #method vshort = $self.
    
    #method short = ShortNumber new &short:theValue.
                                                
    #method int = IntNumber new &short:theValue.
                                                
    #method real = RealNumber new &short:theValue.
    
    #method long = LongNumber new &short:theValue.
    
    #method write &short:anOperand
    [
        theValue := anOperand.
    ]

    #method write &byte:anOperand
    [
        theValue := anOperand short.
    ]

    #method append &short:anOperand
    [
        theValue := theValue + anOperand.
    ]

    #method append &byte:anOperand
    [
        theValue := theValue + anOperand short.
    ]
        
    #method reduce &short:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    #method reduce &byte:anOperand
    [
        theValue := theValue - anOperand short.
    ]
        
    #method multiplyBy &short:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    #method multiplyBy &byte:anOperand
    [
        theValue := theValue * anOperand short.
    ]        
        
    #method divideInto &short:anOperand
    [
        theValue := theValue / anOperand.
    ]
        
    #method divideInto &byte:anOperand
    [
        theValue := theValue / anOperand short.
    ]
    
    #method(type:bool) equal &short:anOperand 
        = theValue == anOperand.

    #method(type:bool) less &short:anOperand
        = theValue < anOperand.
                
    #method(type:short) add &short:anOperand
        => system'internal'short_add.
    
    #method(type:short) subtract &short:anOperand
        => system'internal'short_sub.
    
    #method(type:short) multiply &short:anOperand
        => system'internal'short_mul.
    
    #method(type:short) divide &short:anOperand
        => system'internal'short_div.
        
    #method(type:short) and &short:anOperand
        => system'internal'short_and.
        
    #method(type:short) or &short:anOperand
        => system'internal'short_or.
        
    #method(type:short) xor &short:anOperand
        => system'internal'short_xor.

    #method value = ShortNumber new &short:theValue.

    #method literal = literalConvertor convert &short:theValue.

    #method wide = wideConvertor convert &short:theValue.

    #method clone = Short new &short:theValue.

    #method => theValue.    
}

// --- Integer ---

#class(sealed,struct:embeddable) Integer :: IntBaseVariable
{
    #field(type:int)theValue.
    
    #constructor new
    [
        theValue := 0.
    ]
    
    #constructor new : aValue
    [
        theValue := aValue int.
    ]
    
    #constructor new &int:aValue
    [
        theValue := aValue.
    ]
    
    #constructor new &short:aValue
    [
        theValue := aValue int.
    ]
    
    #constructor new &byte:aValue
    [
        theValue := aValue int.
    ]
    
    #method vint = $self.
    
    #method int = IntNumber new &int:theValue.
                                                
    #method real = RealNumber new &int:theValue.
    
    #method long = LongNumber new &int:theValue.
    
    #method write &int:anOperand
    [
        theValue := anOperand.
    ]

    #method write &short:anOperand
    [
        theValue := anOperand int.
    ]

    #method write &byte:anOperand
    [
        theValue := anOperand int.
    ]

    #method append &int:anOperand
    [
        theValue := theValue + anOperand.
    ]

    #method append &short:anOperand
    [
        theValue := theValue + anOperand int.
    ]

    #method append &byte:anOperand
    [
        theValue := theValue + anOperand int.
    ]
        
    #method reduce &int:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    #method reduce &short:anOperand
    [
        theValue := theValue - anOperand int.
    ]
        
    #method reduce &byte:anOperand
    [
        theValue := theValue - anOperand int.
    ]
        
    #method multiplyBy &int:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    #method multiplyBy &short:anOperand
    [
        theValue := theValue * anOperand int.
    ]        
        
    #method multiplyBy &byte:anOperand
    [
        theValue := theValue * anOperand int.
    ]        
        
    #method divideInto &int:anOperand
    [
        theValue := theValue / anOperand.
    ]
        
    #method divideInto &short:anOperand
    [
        theValue := theValue / anOperand int.
    ]
        
    #method divideInto &byte:anOperand
    [
        theValue := theValue / anOperand int.
    ]
    
    #method(type:bool) equal &int:anOperand 
        = theValue == anOperand.

    #method(type:bool) less &int:anOperand
        = theValue < anOperand.
                
    #method(type:int) add &int:anOperand
        = IntNumber new &int:(theValue + anOperand).
    
    #method(type:int) subtract &int:anOperand
        = IntNumber new &int:(theValue - anOperand).
    
    #method(type:int) multiply &int:anOperand
        = IntNumber new &int:(theValue * anOperand).
    
    #method(type:int) divide &int:anOperand
        = IntNumber new &int:(theValue + anOperand).
        
    #method(type:int) and &int:anOperand
        = IntNumber new &int:(theValue && anOperand).
        
    #method(type:int) or &int:anOperand
        = IntNumber new &int:(theValue || anOperand).
        
    #method(type:int) xor &int:anOperand
        = IntNumber new &int:(theValue ^^ anOperand).

    #method value = IntNumber new &int:theValue.

    #method literal = literalConvertor convert &int:theValue.

    #method wide = wideConvertor convert &int:theValue.

    #method clone = Integer new &int:theValue.

    #method => theValue.    
}

// --- Long ---

#class(sealed,struct:embeddable) Long :: IntBaseVariable
{
    #field(type:long)theValue.
    
    #constructor new
    [
        theValue := 0l.
    ]
    
    #constructor new : aValue
    [
        theValue := aValue long.
    ]
    
    #constructor new &long:aValue
    [
        theValue := aValue.
    ]
    
    #constructor new &int:aValue
        => system'internal'long_copyint.
    
    #constructor new &short:aValue
    [
        theValue := aValue long.
    ]
    
    #constructor new &byte:aValue
    [
        theValue := aValue long.
    ]
    
    #method vlong = $self.
            
    #method long = LongNumber new &long:theValue.
    
    #method real = RealNumber new &long:theValue.

    #method write &long:anOperand
    [
        theValue := anOperand.
    ]

    #method write &int:anOperand
    [
        theValue := anOperand long.
    ]

    #method write &short:anOperand
    [
        theValue := anOperand long.
    ]

    #method write &byte:anOperand
    [
        theValue := anOperand long.
    ]
                        
    #method append &long:anOperand    
    [
        theValue := theValue + anOperand.
    ]
                        
    #method append &int:anOperand    
    [
        theValue := theValue + anOperand long.
    ]
                        
    #method append &short:anOperand    
    [
        theValue := theValue + anOperand long.
    ]
                        
    #method append &byte:anOperand    
    [
        theValue := theValue + anOperand long.
    ]
        
    #method reduce &long:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    #method reduce &int:anOperand
    [
        theValue := theValue - anOperand long.
    ]
        
    #method reduce &short:anOperand
    [
        theValue := theValue - anOperand long.
    ]
        
    #method reduce &byte:anOperand
    [
        theValue := theValue - anOperand long.
    ]
        
    #method multiplyBy &long:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    #method multiplyBy &int:anOperand
    [
        theValue := theValue * LongNumber new &int:anOperand.
    ]        
        
    #method multiplyBy &short:anOperand
    [
        theValue := theValue * anOperand long.
    ]        
        
    #method multiplyBy &byte:anOperand
    [
        theValue := theValue * anOperand long.
    ]        
        
    #method divideInto &long:anOperand
    [
        theValue := theValue / anOperand.
    ]
        
    #method divideInto &int:anOperand
    [
        theValue := theValue / LongNumber new &int:anOperand.
    ]
        
    #method divideInto &short:anOperand
    [
        theValue := theValue / anOperand long.
    ]
        
    #method divideInto &byte:anOperand
    [
        theValue := theValue / anOperand long.
    ]
    
    #method(type:bool) equal &long:anOperand 
        = theValue == anOperand.

    #method(type:bool) less &long:anOperand
        = theValue < anOperand.

    #method(type:bool) equal &int:anOperand 
        = $self equal &long:(LongNumber new &int:anOperand).

    #method(type:bool) less &int:anOperand
        = $self less &long:(LongNumber new &int:anOperand).
        
    #method(type:long) add &long:anOperand
        = LongNumber new &long:(theValue + anOperand).
    
    #method(type:long) subtract &long:anOperand
        = LongNumber new &long:(theValue - anOperand).
    
    #method(type:long) multiply &long:anOperand
        = LongNumber new &long:(theValue * anOperand).
    
    #method(type:long) divide &long:anOperand
        = LongNumber new &long:(theValue + anOperand).
        
    #method(type:long) and &long:anOperand
        = LongNumber new &long:(theValue && anOperand).
        
    #method(type:long) or &long:anOperand
        = LongNumber new &long:(theValue || anOperand).
        
    #method(type:long) xor &long:anOperand
        = LongNumber new &long:(theValue ^^ anOperand).

    #method value = LongNumber new &long:theValue.

    #method literal = literalConvertor convert &long:theValue.

    #method wide = wideConvertor convert &long:theValue.

    #method clone = Long new &long:theValue.
    
    #method => theValue.    
}

// --- Real ---

#class(sealed,struct:embeddable) Real :: BaseVariable
{
    #field(type:real)theValue.
    
    #constructor new
    [
        theValue := 0.0r.
    ]

    #constructor new : aValue
    [
        theValue := aValue real.
    ]
    
    #constructor new &real:aValue
    [
        theValue := aValue.
    ]
    
    #constructor new &long:aValue
    [
        theValue := aValue real.
    ]
    
    #constructor new &int:aValue
        => system'internal'real_copyint.
    
    #constructor new &short:aValue
        => system'internal'real_copyint.
    
    #constructor new &byte:aValue
        => system'internal'real_copyint.
    
    #method vreal = $self.
    
    #method real = RealNumber new &real:theValue.
        
    #method write &real:anOperand
    [
        theValue := anOperand.
    ]
    
    #method write &int:anOperand
    [
        theValue := RealNumber new &int:anOperand.
    ]
    
    #method write &long:anOperand
    [
        theValue := RealNumber new &long:anOperand.
    ]
    
    #method write &short:anOperand
    [
        theValue := RealNumber new &short:anOperand.
    ]
    
    #method write &byte:anOperand
    [
        theValue := RealNumber new &byte:anOperand.
    ]
        
    #method append &real:anOperand    
    [
        theValue := theValue + anOperand.
    ]
    
    #method append &int:anOperand    
    [
        theValue := theValue add &int:anOperand.
    ]
    
    #method append &long:anOperand    
    [
        theValue := theValue add &long:anOperand.
    ]
        
    #method append &short:anOperand    
    [
        theValue := theValue + anOperand real.
    ]
        
    #method append &byte:anOperand    
    [
        theValue := theValue + anOperand real.
    ]
        
    #method reduce &real:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    #method reduce &int:anOperand
    [
        theValue := theValue subtract &int:anOperand.
    ]    
        
    #method reduce &long:anOperand
    [
        theValue := theValue - anOperand real.
    ]
        
    #method reduce &short:anOperand
    [
        theValue := theValue - anOperand real.
    ]
        
    #method reduce &byte:anOperand
    [
        theValue := theValue - anOperand real.
    ]
        
    #method multiplyBy &real:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    #method multiplyBy &int:anOperand
    [
        theValue := theValue multiply &int:anOperand.
    ]        
        
    #method multiplyBy &long:anOperand
    [
        theValue := theValue * anOperand real.
    ]        
        
    #method multiplyBy &short:anOperand
    [
        theValue := theValue * anOperand real.
    ]        
        
    #method multiplyBy &byte:anOperand
    [
        theValue := theValue * anOperand real.
    ]        
        
    #method divideInto &real:anOperand
    [
        theValue := theValue / anOperand.
    ]
        
    #method divideInto &int:anOperand
    [
        theValue := theValue divide &int:anOperand.
    ]
        
    #method divideInto &long:anOperand
    [
        theValue := theValue / anOperand real.
    ]
        
    #method divideInto &short:anOperand
    [
        theValue := theValue / anOperand real.
    ]
        
    #method divideInto &byte:anOperand
    [
        theValue := theValue / anOperand real.
    ]
    
    #method(type:bool) equal &real:anOperand 
        = theValue == anOperand.

    #method(type:bool) less &real:anOperand
        = theValue < anOperand.
        
    #method add &real:anOperand
        = RealNumber new &real:(theValue + anOperand).
    
    #method subtract &real:anOperand
        = RealNumber new &real:(theValue - anOperand).
    
    #method multiply &real:anOperand
        = RealNumber new &real:(theValue * anOperand).
    
    #method divide &real:anOperand
        = RealNumber new &real:(theValue / anOperand).

    #method value = RealNumber new &real:theValue.

    #method literal = literalConvertor convert &real:theValue.

    #method wide = wideConvertor convert &real:theValue.

    #method clone = Real new &real:theValue.
    
    #method => theValue.
}

// --- CharValue ---

#class(sealed,integer:4) CharValue :: BaseValue
{
    #constructor new
        => system'internal'int_init.        

    #constructor new &char:anOperand
        => system'internal'int_copy.

    #constructor min
        <= %new &char:#0.

    #constructor max
        <= %new &char:#010FFFFh.

    #method char = $self.

    #method int = intConvertor convert &char:$self.
    
    #method equal &char:anOperand 
        => system'internal'int_equal.

    #method less &char:anOperand 
        => system'internal'int_less.

    #method equal &int:anOperand 
        => system'internal'int_equal.

    #method less &int:anOperand 
        => system'internal'int_less.

    #method clone = CharValue new &char:$self.
    
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &char:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &char:$self.

    #method literal = literalConvertor convert &char:$self.

    #method wide = wideConvertor convert &char:$self.
    
    #method readLength &vint:aLength
        => system'internal'char_getlen.
        
    #method length
    [
        #var(type:vint)aLength := 0.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
}

// --- Char ---

#class(sealed,struct:embeddable) Char :: BaseVariable
{
    #field(type:char)theValue.
    
    #constructor new
    [
        theValue := #0.
    ]
    
    #constructor new : aValue   
    [
        theValue := aValue char.
    ]
    
    #constructor new &char:aValue
    [
        theValue := aValue.
    ]
    
    #method vchar = $self.
    
    #method char = CharValue new &char:theValue.
    
    #method write &char:anOperand
    [
        theValue := anOperand.
    ]
    
    #method(type:bool) equal &char:anOperand 
        = theValue == anOperand.

    #method(type:bool) less &char:anOperand
        = theValue < anOperand.
        
    #method value = CharValue new &char:theValue.

    #method literal = literalConvertor convert &char:theValue.

    #method wide = wideConvertor convert &char:theValue.

    #method clone = Char new &char:theValue.
    
    #method readLength &vint:aLength
        => system'internal'char_getlen.
        
    #method length
    [
        #var(type:vint)aLength := 0.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method => theValue.    
}

// === Basic Factory symbols ==

#class Variable
{
    #field theValue.
    
    #constructor new
    [
    ]        
        
    #constructor new : aValue
    [
        theValue := aValue.
    ]
  
    #method write : aValue
    [
        theValue := aValue.
    ]
        
    #method value = theValue.        
        
    #method append : aValue
    [
        (nil == theValue)
            ? [ theValue := aValue. ]
            ! [ theValue := theValue + aValue. ].
    ]
    
    #method reduce : aValue
    [
        (nil == theValue)
            ? [ theValue := aValue. ]
            ! [ theValue := theValue - aValue. ].
    ]

    #method multiplyBy : aValue
    [
        (nil == theValue)
            ? [ theValue := aValue. ]
            ! [ theValue := theValue * aValue. ].
    ]
    
    #method divideInto : aValue
    [
        (nil == theValue)
            ? [ theValue := aValue. ]
            ! [ theValue := theValue / aValue. ].
    ]
        
    #method => theValue.
}

// === Basic Convertors ===

// --- convertor helpers ---

#symbol byteConvertor =
{
    #method convert &char:aValue &vbyte:aRetVal
        => system'internal'byte_copychar.
        
    #method convert &short:aValue &vbyte:aRetVal
        => system'internal'byte_copyword.
        
    #method convert &int:aValue &vbyte:aRetVal
        => system'internal'byte_copyint.
        
    #method convert &long:aValue &vbyte:aRetVal
        => system'internal'byte_copylong.
        
    #method convert &real:aValue &vbyte:aRetVal
        => system'internal'byte_copyreal.
        
    #method convert &literal:aValue &vbyte:aRetVal
        => system'internal'byte_copystr.
        
    #method convert &wide:aValue &vbyte:aRetVal
        => system'internal'byte_copywstr.
        
    #method(type:byte) convert &byte:anInt
        = ByteNumber new &byte:(anInt).
        
    #method(type:byte) convert &char:aValue
    [
        #var(type:vbyte)aRetVal := 0.
        $self convert &char:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
        
    #method(type:byte) convert &short:aValue
    [
        #var(type:vbyte)aRetVal.
        $self convert &short:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
        
    #method(type:byte) convert &int:aValue
    [
        #var(type:vbyte)aRetVal.
        $self convert &int:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
        
    #method(type:byte) convert &long:aValue
    [
        #var(type:vbyte)aRetVal.
        $self convert &long:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
        
    #method(type:byte) convert &real:aValue
    [
        #var(type:vbyte)aRetVal.
        $self convert &real:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
                
    #method(type:byte) convert &literal:aLiteral
    [
        #var(type:vbyte)aValue.
        $self convert &literal:aLiteral &vbyte:aValue.
        
        ^ ByteNumber new &byte:aValue.
    ]
                
    #method(type:byte) convert &wide:aLiteral
    [
        #var(type:vbyte)aValue. 
        $self convert &wide:aLiteral &vbyte:aValue.
        
        ^ ByteNumber new &byte:aValue.
    ]
}.

#symbol shortConvertor =
{
    #method convert &char:aValue &vshort:aRetVal
        => system'internal'word_copychar.
        
    #method convert &byte:aValue &vshort:aRetVal
        => system'internal'word_copybyte.
        
    #method convert &int:aValue &vshort:aRetVal
        => system'internal'word_copyint.
        
    #method convert &long:aValue &vshort:aRetVal
        => system'internal'word_copylong.
        
    #method convert &real:aValue &vshort:aRetVal
        => system'internal'word_copyreal.
        
    #method convert &literal:aValue &vshort:aRetVal
        => system'internal'word_copystr.
        
    #method convert &wide:aValue &vshort:aRetVal
        => system'internal'word_copywstr.
        
    #method(type:short) convert &short:anInt
        = ShortNumber new &short:anInt.
        
    #method(type:short) convert &byte:aValue
    [
        #var(type:vshort)aRetVal.
        $self convert &byte:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    #method(type:short) convert &int:aValue
    [
        #var(type:vshort)aRetVal.
        $self convert &int:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    #method(type:short) convert &long:aValue
    [
        #var(type:vshort)aRetVal.
        $self convert &long:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    #method(type:short) convert &real:aValue
    [
        #var(type:vshort)aRetVal.
        $self convert &real:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    #method(type:short) convert &char:aValue
    [
        #var(type:vshort)aRetVal.
        $self convert &char:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    #method(type:short) convert &literal:aValue
    [
        #var(type:vshort)aRetVal.
        $self convert &literal:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    #method(type:short) convert &wide:aValue
    [
        #var(type:vshort)aRetVal.
        $self convert &wide:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
}.

#symbol charConvertor =
{
    #method convert &byte:aValue &vchar:aRetVal
        => system'internal'char_savebyte.
        
    #method convert &short:aValue &vchar:aRetVal
        => system'internal'char_saveshort.
        
    #method convert &int:aValue &vchar:aRetVal
        => system'internal'char_saveint.
        
    #method(type:char)convert &char:aChar
        = CharValue new &char:aChar.
  
    #method(type:char) $convert &index:anIndex &literal:aLiteral &vchar:char
        => system'internal'char_copystr.
  
    #method(type:char) $convert &index:anIndex &bytearray:aLiteral &vchar:char
        => system'internal'char_copystr.
  
    #method(type:char) $convert &index:anIndex &wide:aLiteral &vchar:char
        => system'internal'char_copywstr.
        
    #method(type:char) $convert &index:anIndex &shortarray:aLiteral &vchar:char
        => system'internal'char_copywstr.
        
    #method(type:char) convert &literal:aValue
    [
        #var(type:vchar)aRetVal.
        $self $convert &index:0 &literal:aValue &vchar:aRetVal.
        
        (aRetVal length != aValue length)
            ? [ #throw system'InvalidArgumentException new. ].
        
        ^ CharValue new &char:aRetVal.
    ]        
        
    #method(type:char) convert &wide:aValue
    [
        #var(type:vchar)aRetVal.
        $self $convert &index:0 &wide:aValue &vchar:aRetVal.
        
        (((aRetVal length + 1) / 2) != aValue length)
            ? [ #throw system'InvalidArgumentException new. ].
        
        ^ CharValue new &char:aRetVal.
    ]        
        
    #method(type:char) convert &byte:aValue
    [
        #var(type:vchar)aRetVal.
        $self convert &byte:aValue &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]        
        
    #method(type:char) convert &short:aValue
    [
        #var(type:vchar)aRetVal.
        $self convert &short:aValue &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]        
        
    #method(type:char) convert &int:aValue
    [
        #var(type:vchar)aRetVal.
        $self convert &int:aValue &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]
    
    #method save &char:aChar &index:anIndex &shortarray:aCharArray &vlength:aLength
        => system'internal'sa_copychar.
    
    #method save &char:aChar &index:anIndex &bytearray:aCharArray &vlength:aLength
        => system'internal'ba_copychar.
}.

#symbol intConvertor =
{
    convert &literal:aValue &vint:aRetVal
        => system'internal'int_copystr.

    convert &wide:aValue &vint:aRetVal
        => system'internal'int_copywstr.

    convert &char:aValue &vint:aRetVal
        => system'internal'int_saveint.

    convert &short:aValue &vint:aRetVal
        => system'internal'int_saveint.

    convert &byte:aValue &vint:aRetVal
        => system'internal'int_saveint.

    convert &long:aValue &vint:aRetVal
        => system'internal'int_savelong.
        
    convert &real:aReal &vint:aRetVal
        => system'internal'int_copyreal.
        
    #method(type:int) convert &literal:aLiteral
    [
        #var(type:vint)aRetVal.
        $self convert &literal:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    #method(type:int) convert &wide:aLiteral
    [
        #var(type:vint)aRetVal.
        $self convert &wide:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    #method(type:int) convert &char:aValue
    [
        #var(type:vint)aRetVal.
        $self convert &char:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    #method(type:int) convert &byte:aValue
    [
        #var(type:vint)aRetVal.
        $self convert &byte:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    #method(type:int) convert &short:aValue
    [
        #var(type:vint)aRetVal.
        $self convert &short:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    #method(type:int) convert &long:aValue
    [
        #var(type:vint)aRetVal.
        $self convert &long:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method(type:int) convert &real:aReal
    [
        #var(type:vint)aRetVal.
        $self convert &real:aReal &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
                                    
    #method(type:int) convert &int:anInt
        = IntNumber new &int:anInt.
}.

#symbol longConvertor =
{
    convert &literal:aValue &vlong:aRetVal
        => system'internal'long_copystr.

    convert &wide:aValue &vlong:aRetVal
        => system'internal'long_copywstr.

    convert &real:aValue &vlong:aRetVal
        => system'internal'long_copyreal.

    convert &int:aValue
        = LongNumber new &int:aValue.

    convert &byte:aValue
        = LongNumber new &byte:aValue.

    convert &short:aValue
        = LongNumber new &short:aValue.

    convert &real:aValue
    [
        #var(type:vlong)aRetVal.
        $self convert &real:aValue &vlong:aRetVal.
        
        ^ LongNumber new &long:aRetVal.
    ]

    convert &char:aValue
    [
        #var(type:vint)aRetVal.
        intConvertor convert &char:aValue &vint:aRetVal.
        
        ^ aRetVal long.
    ]

    convert &literal:aLiteral
    [
        #var(type:vlong)aRetVal.
        $self convert &literal:aLiteral &vlong:aRetVal.
        
        ^ LongNumber new &long:aRetVal.
    ]

    convert &wide:aLiteral
    [
        #var(type:vlong)aRetVal.
        $self convert &wide:aLiteral &vlong:aRetVal.
        
        ^ LongNumber new &long:aRetVal.
    ]
    
    convert &long:aLong
        = LongNumber new &long:aLong.
}.

#symbol realConvertor =
{
    convert &int:aValue &vreal:aRetVal
        => system'internal'real_saveint.

    convert &literal:aValue &vreal:aRetVal
        => system'internal'real_copystr.
        
    convert &wide:aValue &vreal:aRetVal
        => system'internal'real_copywstr.
        
    convert &literal:aLiteral
    [
        #var(type:vreal)aRetVal.
        $self convert &literal:aLiteral &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
        
    convert &wide:aLiteral
    [
        #var(type:vreal)aRetVal.
        $self convert &wide:aLiteral &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    convert &real:aReal
        = RealNumber new &real:aReal.
        
    convert &int:anInt
        = RealNumber new &int:anInt.
        
    convert &long:aLong
        = RealNumber new &long:aLong.
        
    convert &byte:aValue
        = RealNumber new &byte:aValue.
        
    convert &short:aValue
        = RealNumber new &short:aValue.
}.

// --- literalConvertor ---

#symbol literalConvertor =
{
    #method(type:literal) convert &char:aValue
        => system'internal's_copychar.
    
    #method(type:literal) convert &int:aNumber
        => system'internal's_copyint.

    #method(type:literal) convert &short:aNumber
        => system'internal's_copyint.

    #method(type:literal) convert &byte:aNumber
        => system'internal's_copyint.

    #method(type:literal) convert &long:aNumber
        => system'internal's_copylong.

    #method(type:literal) convert &int:aNumber &index:aRadix
        => system'internal's_copyint_radix.

    #method(type:literal) convert &short:aNumber &index:aRadix
        => system'internal's_copyint_radix.

    #method(type:literal) convert &byte:aNumber &index:aRadix
        => system'internal's_copyint_radix.

    #method(type:literal) convert &long:aNumber &index:aRadix
        => system'internal's_copylong_radix.

    #method(type:literal) convert &real:aNumber
        => system'internal's_copyreal.

    #method(type:literal) convert &wide:aLiteral
        => system'internal'str_copywstr.
        
    #method(type:literal) convert : anObject
        = anObject literal.
        
    #method(type:literal) convert &literal:aLiteral
        = aLiteral.
}.

// --- wideConvertor ---

#symbol wideConvertor =
{
    #method(type:wide) convert &char:aValue
        => system'internal'ws_copychar.
    
    #method(type:wide) convert &int:aNumber
        => system'internal'ws_copyint.

    #method(type:wide) convert &short:aNumber
        => system'internal'ws_copyint.

    #method(type:wide) convert &byte:aNumber
        => system'internal'ws_copyint.

    #method(type:wide) convert &long:aNumber
        => system'internal'ws_copylong.

    #method(type:wide) convert &int:aNumber &index:aRadix
        => system'internal'ws_copyint_radix.

    #method(type:wide) convert &short:aNumber &index:aRadix
        => system'internal'ws_copyint_radix.

    #method(type:wide) convert &byte:aNumber &index:aRadix
        => system'internal'ws_copyint_radix.

    #method(type:wide) convert &long:aNumber &index:aRadix
        => system'internal'ws_copylong_radix.

    #method(type:wide) convert &real:aNumber
        => system'internal'ws_copyreal.

    #method(type:wide) convert &literal:aLiteral
        => system'internal'wstr_copystr.
        
    #method(type:wide) convert : anObject
        = anObject wide.
        
    #method(type:wide) convert &wide:aLiteral
        = aLiteral.
}.

// === Typecaster ==

#symbol typeCaster
    = { ! : anObject = $subject. }.
