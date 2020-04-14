import system'routines;
import system'dynamic;
import extensions;

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
        var obj := new class();
        
        ^ copyController.copy(self, obj)
    }
}