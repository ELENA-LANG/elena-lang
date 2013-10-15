// --- Operations ---

#class(role)EMathOp
{
    #method modulus : anOperand
        = self - (self / anOperand * anOperand).
}

// --- Functions ---

#symbol modulus = &&:x:y
[
    x~EMathOp modulus:y
].