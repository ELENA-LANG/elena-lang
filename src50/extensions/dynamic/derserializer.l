import system'routines;
import system'dynamic;

singleton copyController
{
    copy(object s, DTO t)
    {
        t.__getClass().__dto_properties().forEach:(prop)
        {
            prop.setPropertyValue(t, prop.getPropertyValue(s))
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