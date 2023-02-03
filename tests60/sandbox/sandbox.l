
singleton Ops
{
    sum(int l, params int[] args)
    {
        int sum := l;
        int len := args.Length;
        for (int i := 0, i < len, i += 1)
        {
            sum := sum + args[i]
        };
        
        ^ sum
    }
}

public program()
{
     console.writeLine(Ops.sum(1));
     console.writeLine(Ops.sum(1,2));
     console.writeLine(Ops.sum(1,2,3));
     console.writeLine(Ops.sum(1,2,3,4));
     console.writeLine(Ops.sum(1,2,3,4,5));
}
