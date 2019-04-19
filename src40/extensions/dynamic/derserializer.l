import system'routines;
import system'dynamic;

singleton copyController
{
    copy(object s, DTO t)
    {
        t.__dto_properties().forEach:(prop)
        {
            var v := mixin prop(s).prop;
            
            mixin prop(t).prop := v
        };
        
        ^ t
    }
}

public extension dynamicStructOp
{
    serializeTo(class)
    {
        var obj := class.new();
        
        ^ copyController.copy(self, obj)
    }
}