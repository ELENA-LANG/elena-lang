class MyDTO
{ 
    string X : dto_prop; 
    
    int Y : dto_prop;

    printMe()
    {
       Console.printLine(self,"{x:",X,",y:",Y,"}")
    }
}

/*public Program()
{
}*/