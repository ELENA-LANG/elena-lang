import extensions;

A;

B;

C : A;

checkTypeA(object)
{
   if (object.instanceOf(A)) {
      console.printLine(object, " is an instance of A");
   }
   else if (object.subsetOf(A)) {
      console.printLine(object, " is a subset of A");
   }
   else console.printLine(object, " is not an instance of A");
}

public program()
{
   var a := new A();
   var b := new B();
   var c := new C();

   checkTypeA(a);
   checkTypeA(b);
   checkTypeA(c);
}