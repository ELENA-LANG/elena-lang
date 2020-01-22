//// --- Symbol ---
//
///// A dynamic symbol
//public sealed const struct Symbol
//{
//    __symbol theValue;
//    
//    /// Loads a symbol
//    constructor(string name)
//    {
//        intern system'core_routines'__symbol.load(self, name)
//    }
//        
//    /// Redirects incoming messages to the symbol
//    dispatch() => system'core_routines'__symbol;
//}
//
//public sealed const struct ClassReference
//{
//    embeddable __raw theValue[4];
//    
//    /// Loads a symbol
//    constructor(string name)
//    {
//        intern system'core_routines'__class.load(self, name)
//    }
//        
//    /// Redirects incoming messages to the symbol
//    dispatch() => system'core_routines'__class;
//}

// --- Message ---

/// A message constant
public sealed const struct Message
{
    embeddable __mssg theValue[4];

//    constructor(string name)
//    {
//        intern system'core_routines'__message.load(self, name)
//    }
//    
////    internal constructor newInteral(int message)
////    [
////        system'core_routines'__message copy(self, message).
////    ]
////    
////    /// Loads a message
////    constructor new(mssg message)
////        = message.
//
//    get MessageName MessageName()
//    {
//        intern system'core_routines'__message.getMessageName(self, ref MessageName retVal);
//        
//        ^ retVal
//    }

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

/// An extended message constant
public sealed const struct ExtensionMessage
{
    embeddable __mssg theValue[8];
    
////    /// Loads a message
////    constructor new(ExtensionMessage message)
////        = message.
////
////    T<Signature> signature
////        => system'core_routines'__message.
////
////    bool equal(ExtensionMessage aSubject)
////        => system'core_routines'__extmessage.

    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
    dispatch() => intern system'core_routines'__extmessage;
}

// --- MessageName ---

/// A dynamic message extender
public sealed const struct MessageName
{
    embeddable __subj theValue[4];
    
////    /// Creates a dynamic signature
////    constructor new(mssg message)
////    [
////        system'core_routines'__signature load(self, message)
////    ]
////        
////    /// Creates a dynamic signature
////    constructor new(Signature subject)
////        = subject.
//
//    /// Creates a dynamic signature
//    constructor(string s)
//    {
//        intern system'core_routines'__signature.load(self, s)
//    }
//
//    saveTo(ref mssg message)
//    {
//        intern system'core_routines'__signature.saveTo(self, ref message)
//    }
//
//    mssg getPropertyMessage()
//    {
//        intern system'core_routines'__signature.getProperty(self, ref mssg message);
//        
//        ^ message
//    }
//
//    mssg setPropertyMessage()
//    {
//        intern system'core_routines'__signature.setProperty(self, ref mssg message);
//        
//        ^ message        
//    }
//
////    /// Creates a dynamic signature
////    constructor new(wide aSubjectName)
////        <= new(T<literal>(aSubjectName)).
//
//    /// Compares the values
//    bool equal(MessageName subject)
//        = theValue == subject;

    string toPrintable()
    {
        byte buffer[256];
        int  length := extern elenart.LoadSubjectName(self, buffer, 255);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Qualifies incoming messages and redirects to the target
    dispatch() => intern system'core_routines'__signature;
}

//// --- Extension ---
//
//public mixin Wrap
//{
//    object[] items;
//    
//    dispatch() => system'core_routines'__wrap;
//}
//
//// --- Wrap ---
//
//public mixin Extension
//{
//    object[] items;
//    
//    dispatch() => system'core_routines'__extension;
//}
