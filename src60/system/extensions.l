// --- intOp ---

public extension intOp : IntNumber
{
    int mod(int operand)
    {
        int retVal := self / operand * operand;

        retVal := self - retVal;
        
        ^ retVal
    }    
}