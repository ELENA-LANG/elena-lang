import system'routines.
import system'dynamic.

public extension<DynamicStruct> dynamicStructOp
{
    serializeTo(object class)
    [
        var obj := class new.
        class __properties; forEach(:prop)
        [
            obj~prop set(self~prop get)
        ].
        
        ^ obj
    ]
}