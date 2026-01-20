public sealed struct X
{
    int dVal;
    int dLen;

    test()
    {
        int i := 3;

        if (dLen < i)
        {
            dLen := i;
        };
    }       
}

public Program()
{
   X n := default;
   
   n.test();
}