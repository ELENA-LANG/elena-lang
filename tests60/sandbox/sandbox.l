
singleton Ops
{
    sum(int l, params int[] args)
    {
       int len := args.Length;

       ^ len

        //int sum := l;
        //int len := args.Length;
        //for (int i := 0, i < len, i += 1)
        //{
        //    sum += args[i]
        //};
        
        //^ sum
    }
}

public program()
{
   var o := Ops.sum(1);
//   console.writeLine(Ops.sum(1,2,3).toPrintable());

//   console.printLine(Ops.sum(1));
//   console.printLine(Ops.sum(1,2));
//   console.printLine(Ops.sum(1,2,3,4,5));
}
