import extensions;

public class PropVariant
{
    object owner;
    string value;
    
    constructor(object owner, string value)
    {
        this owner := owner;
        this value := value;
    }
    
    int cast()
        = value.toInt();
        
    string cast()
        = value;    
        
    Func1 cast()
    {
        auto subj := new MessageName(value);
        
        ^ (arg){ subj(owner,arg) }
    }
}
