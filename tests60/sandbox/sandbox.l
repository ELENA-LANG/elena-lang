import extensions;

B
{
   write()
   {
      console.writeLine("B.write")
   }
}

C
{
   B cast()
      = new B();
}

public program()
{
//   var c := new C();

//   var b := c :as B;


   var client := ::{ Parameter = new C(); } ;

   (client.Parameter :as B).write();
   cast B(client.Parameter).write();

//   (client.Parameter :as B).write()

//
//   client.Parameter :as B
//   client.Parameter as: B
//
//
//
//   if (x :of B) {
//   }
}
