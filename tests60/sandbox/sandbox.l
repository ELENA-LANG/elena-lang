//import extensions;
//import extensions'text;
//import system'routines;

interface I
{
   abstract int Index
   {
      get();
   }
}

class A
{
   get int Index()
   {
      ^ 2;
   }

   I cast() = new I { embeddable dispatch() => self; };
}

public program()
{
   var a := new A();

   I i := a;

   int n := i.Index;

//   var variants := new string[]{"abc","dfge","ab"}; 
//
//   Indexer i := variants.indexer();
//
//   int l := i.Length;
//
////   var o := variants.sort:(a,b => a.Length > b.Length );
}