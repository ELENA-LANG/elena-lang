// --- Symbol ---

/// A dynamic symbol
public sealed const struct Symbol
{
    __symbol theValue;
    
    /// Loads a symbol
    constructor(string name)
    {
        intern system'core_routines'__symbol.load(self, name)
    }
    
    constructor(dirty_ptr ptr)
    {
        intern system'core_routines'__dptr.copyTo(ptr, self)
    }
    
    static Symbol tryLoad(string ns, string name)
    {
        intern system'core_routines'__symbol.tryLoad(ref dirty_ptr ptr, ns, name);
        
        ifnot (ptr.isEmpty())
        {
            ^ ptr
        }
        else
        {
            ^ nil
        }
    }    
                
    /// Redirects incoming messages to the symbol
    dispatch() => intern system'core_routines'__symbol;
}

public sealed const struct ClassReference
{
    embeddable __raw theValue[4];
    
    /// Loads a symbol
    constructor(string name)
    {
        intern system'core_routines'__class.load(self, name)
    }
        
    /// Redirects incoming messages to the symbol
    dispatch() => intern system'core_routines'__class;
}

// --- Message ---

[info:"A message constant"]
public sealed const struct Message
{
    embeddable __mssg theValue[4];

    constructor(string name)
    {
        intern system'core_routines'__message.load(self, name)
    }
    
    /// Compares the values
    bool equal(Message mssg)
        = theValue == mssg;
    
    get MessageName MessageName()
    {
        intern system'core_routines'__message.getMessageName(self, ref MessageName retVal);
        
        ^ retVal
    }

    string toPrintable()
    {
        intern system'core_routines'__message.getWeakMessage(self, ref Message retVal);
        
        byte buffer[256];
        int  len := extern elenart.LoadMessageName(retVal, buffer, 255);
        
        ^ String.fromByteArray(0, len, buffer)
    }

    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
    dispatch() => intern system'core_routines'__message;
}

// --- ExtensionMessage ---

[info:"An extended message constant"]
public sealed const struct ExtensionMessage
{
    embeddable __mssg theValue[8];
    
    [info:"Compares the values"]
    bool equal(ExtensionMessage ext_mssg)
        = theValue == ext_mssg;

    [info:"Dequalifies incoming messages if they are equal to its value and redirects to the target"]
    dispatch() => intern system'core_routines'__extmessage;
}

// --- MessageName ---

/// A dynamic message extender
public sealed const struct MessageName
{
    embeddable __subj theValue[4];
    
    /// Creates a dynamic signature
    constructor(string s)
    {
        intern system'core_routines'__signature.load(self, s)
    }

    Message getPropertyMessage()
    {
        intern system'core_routines'__signature.getProperty(self, ref Message message);
        
        ^ message
    }

    Message setPropertyMessage()
    {
        intern system'core_routines'__signature.setProperty(self, ref Message message);
        
        ^ message        
    }

    get Message VariadicMessage()
    {
        intern system'core_routines'__signature.varidadic(self, ref Message message);
        
        ^ message        
    }

    /// Compares the values
    bool equal(MessageName subject)
        = theValue == subject;

    string toPrintable()
    {
        byte buffer[256];
        int  length := extern elenart.LoadSubjectName(self, buffer, 255);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Qualifies incoming messages and redirects to the target
    dispatch() => intern system'core_routines'__signature;
}

// --- Extension ---

public mixin Extension
{
    object target;
    object extender;
    
    constructor(target, extender)
    {
        this target := target;
        this extender := extender;
    }
    
    dispatch() => intern system'core_routines'__extension;
}

// --- ExplicitExtension ---

public mixin ExtensionVariable
{
    object extension;
    object target;
    
    constructor(extension, target)
    {
        this target := target;
        this extension := extension;
    }
    
    dispatch() => intern system'core_routines'__extensionvar;
}
