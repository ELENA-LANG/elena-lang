//// --- Symbol ---
//
///// A dynamic symbol
//sealed struct Symbol
//{
//    __symbol theValue.
//    
//    /// Loads a symbol
//    constructor new literal:name
//    [
//        system'core_routines'__symbol load symbol_ref:$self literal:name
//    ]
//        
//    /// Redirects incoming messages to the symbol
//    dispatch => system'core_routines'__symbol.
//}

// --- Message ---

/// A message constant
public sealed struct Message
{
    __mssg theValue.

    constructor new(literal name)
    [
        system'core_routines'__message load(self, name)
    ]
    
//    stacksafe constructor $new(int message)
//    [
//        system'core_routines'__message copy mssg:$self int:message.
//    ]
    
    /// Loads a message
    constructor new(mssg message)
        = message.

    T<Signature> signature
        => system'core_routines'__message.

    T<literal> literal
    [
        T<byte,256> aBuffer.
        int         aLength := system'external LoadMessageName(self, T<ByteArray>(aBuffer), 255).
        
        ^ LiteralValue fromByteArray(0, aLength, T<ByteArray>(aBuffer))
    ]

    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
    dispatch => system'core_routines'__message.
}

// --- ExtensionMessage ---

/// An extended message constant
public sealed struct ExtensionMessage
{
    __extmssg theValue.
    
    /// Loads a message
    constructor new(ExtensionMessage message)
        = message.

    T<Signature> signature
        => system'core_routines'__message.

    stacksafe bool equal(ExtensionMessage aSubject)
        => system'core_routines'__extmessage.

    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
    dispatch => system'core_routines'__extmessage.
}

// --- Signature ---

/// A dynamic signature extender
public sealed struct Signature
{
    __subj theValue.
    
    /// Creates a dynamic signature
    stacksafe constructor new(mssg message)
    [
        system'core_routines'__signature load(self, message)
    ]
        
    /// Creates a dynamic signature
    constructor new(Signature subject)
        = subject.

    /// Creates a dynamic signature
    constructor new(literal aSubjectName)
    [
        system'core_routines'__signature load(self, aSubjectName)
    ]

    /// Creates a dynamic signature
    constructor new(wide aSubjectName)
        <= new(T<literal>(aSubjectName)).

    /// Compares the values
    stacksafe bool equal(Signature subject)
        = theValue == subject.

    T<literal> literal
    [
        T<byte,256> aBuffer.
        int         aLength := system'external LoadSubjectName(self, T<ByteArray>(aBuffer), 255).
        
        ^ LiteralValue fromByteArray(0, aLength, T<ByteArray>(aBuffer))
    ]

    /// Qualifies incoming messages and redirects to the target
    stacksafe dispatch => system'core_routines'__signature.
}
