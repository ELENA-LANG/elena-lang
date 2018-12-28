//// --- Symbol ---
//
///// A dynamic symbol
//public sealed struct Symbol
//{
//    __symbol theValue.
//    
//    /// Loads a symbol
//    constructor new(literal name)
//    [
//        system'core_routines'__symbol load(self, name)
//    ]
//        
//    /// Redirects incoming messages to the symbol
//    dispatch => system'core_routines'__symbol.
//}

// --- Message ---

/// A message constant
public sealed struct Message
{
    embeddable __mssg theValue[4];

//    constructor new(literal name)
//    [
//        system'core_routines'__message load(self, name)
//    ]
//    
//    internal constructor newInteral(int message)
//    [
//        system'core_routines'__message copy(self, message).
//    ]
//    
//    /// Loads a message
//    constructor new(mssg message)
//        = message.
//
//    T<Signature> signature
//        => system'core_routines'__message.

    get string Printable()
    {
        byte buffer[256];
        int  len := extern elenart.LoadMessageName(self, buffer, 255);
        
        ^ String.fromByteArray(0, len, buffer)
    }

//    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
//    dispatch => system'core_routines'__message.
}

//// --- ExtensionMessage ---
//
///// An extended message constant
//public sealed struct ExtensionMessage
//{
//    __extmssg theValue.
//    
//    /// Loads a message
//    constructor new(ExtensionMessage message)
//        = message.
//
//    T<Signature> signature
//        => system'core_routines'__message.
//
//    bool equal(ExtensionMessage aSubject)
//        => system'core_routines'__extmessage.
//
//    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
//    dispatch => system'core_routines'__extmessage.
//}

// --- MessageName ---

/// A dynamic message extender
public sealed struct MessageName
{
    embeddable __subj theValue[4];
    
//    /// Creates a dynamic signature
//    constructor new(mssg message)
//    [
//        system'core_routines'__signature load(self, message)
//    ]
//        
//    /// Creates a dynamic signature
//    constructor new(Signature subject)
//        = subject.
//
//    /// Creates a dynamic signature
//    constructor new(literal aSubjectName)
//    [
//        system'core_routines'__signature load(self, aSubjectName)
//    ]
//
//    /// Creates a dynamic signature
//    constructor new(wide aSubjectName)
//        <= new(T<literal>(aSubjectName)).
//
//    /// Compares the values
//    bool equal(Signature subject)
//        = theValue == subject.
//
//    T<literal> literal
//    [
//        T<byte,256> aBuffer.
//        int         aLength := system'external LoadSubjectName(self, T<ByteArray>(aBuffer), 255).
//        
//        ^ LiteralValue fromByteArray(0, aLength, T<ByteArray>(aBuffer))
//    ]
//
//    /// Qualifies incoming messages and redirects to the target
//    dispatch => system'core_routines'__signature.
}
