struct DeclA
{
   int x : prop(_x);
   
   int y : prop(_y);
}

struct DeclB
{
   UnsafePointer<DeclA> _a;

   int b : prop(_b);

   DeclA a
      = _a.Value;
}

// --- Regression: the 'info' attribute must be allowed on field-template properties and plain fields ---
// Guards the fix in Compiler::declareFieldMetaInfo (elenasrc3/elc/compiler.cpp).
// If the fix regresses these declarations fail to compile with "unknown template".

public sealed InfoTuple<T1> : info("An immutable fixed-size collection of one element")
{
   T1 Item1 : rprop, info("Getter for the single element");

   constructor(T1 o1)
   {
      Item1 := o1
   }
}

struct InfoDeclStruct
{
   int x : prop(_x), info("a read-write property with a description");

   int y : rprop, info("a read-only property with a description");

   int z : info("a plain field with a description");

   int sumZ()
      = z;
}

class ExtensionMethodOwner
{
   extension printMe<T>(T arg)
   {
      Console.writeLine($"{arg} was called from {self}")
   }
    
   constructor() {}
}
        
public ExtensionMethodInvoker()
{
   auto a := new ExtensionMethodOwner();       
   a.printMe(2);
}
