import extensions;

/*public program()
{
   const byte[] arr := new const byte[] { 1, 3, 4 };
}*/    

    symbol startUp : preloaded = true.then(
    {  
       console.writeLine("Starting up")
    });

    public myFunction()
    {
       console.writeLine("Working");
    }

    public program()
    {
       myFunction()
    }

