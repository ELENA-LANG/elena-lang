//#inline dto
//{
//    this __dto_properties += @subject;
//}
//
//#property dto_prop<__name>
//{
//    field;
//
//    [dto]    
//    get()
//    {
//        ^ __name
//    }
//    
//    set(value)
//    {
//        __name := value
//    }
//}
//
//#property dto_prop<T, __name>
//{
//    field;
//
//    [dto]    
//    get()
//    {
//        ^ __name
//    }
//    
//    set(T value)
//    {
//        __name := value
//    }
//}

// --- using statement ---

#declare using<expr>(block)
{
    auto v := expr;
    
    try
    {
        block;
    }
    finally
    {
        v.close()
    }
}

// === inline operator templates ===
