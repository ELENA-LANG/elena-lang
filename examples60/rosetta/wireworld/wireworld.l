import system'routines;
import extensions;
import cellular;
 
const string sample = 
" tH......
.        ......
 ...Ht...      .
              ....
              .  .....
              ....
 ......tH      .
.        ......
 ...Ht...";
 
const string conductorLabel = ".";
const string headLabel = "H";
const string tailLabel = "t";
const string emptyLabel = " ";
 
const int empty = 0;
const int conductor = 1;
const int electronHead = 2;
const int electronTail = 3;
 
wireWorldRuleSet = new RuleSet
{
    int proceed(Space s, int x, int y)
    {
        int cell := s.at(x, y);
 
        cell =>
            conductor:
            {
                int number := s.LiveCell(x, y, electronHead);
                if (number == 1 || number == 2)
                {
                    ^ electronHead
                }
                else
                {
                    ^ conductor
                }                
            }
            electronHead:
            {
                ^ electronTail
            }
            electronTail:
            {
                ^ conductor
            }
            !:{
                ^ cell
            }
    }
};
 
sealed class Model
{
    Space   theSpace;
 
    constructor load(string stateString, int maxX, int maxY)
    {
        var strings := stateString.splitBy(NewLineConstant).selectBy::(s => s.toArray()).toArray();
 
        theSpace := IntMatrixSpace.allocate(maxX, maxY, RuleSet
        {
            int proceed(Space s, int x, int y)
            {
                int retVal := 0;
                if (x < strings.Length)
                {
                    var l := strings[x];
                    if (y < l.Length)
                    {
                        (l[y]) =>
                            conductorLabel : { retVal := conductor }
                            headLabel      : { retVal := electronHead }
                            tailLabel      : { retVal := electronTail }
                            emptyLabel     : { retVal := empty }
                    }
                    else
                    {
                        retVal := empty
                    }                 
                }
                else
                {
                    retVal := empty
                };
 
                ^ retVal
            }
        })
    }
 
    run()
    {
        theSpace.update(wireWorldRuleSet)
    }    
 
    print()
    {
        int columns := theSpace.Columns;
        int rows := theSpace.Rows;
 
        int i := 0;
        int j := 0;
        while (i < rows)
        {
            j := 0;
 
            while (j < columns)
            {
                var label := emptyLabel;
                int cell := theSpace.at(i, j);
 
                cell =>
                    conductor    : { label := conductorLabel }
                    electronHead : { label := headLabel }
                    electronTail : { label := tailLabel };
 
                Console.write(label);
 
                j := j + 1
            };
 
            i := i + 1;
            Console.writeLine()
        }
    }
}
 
public program()
{
    Model model := Model.load(sample,10,30);
    for(int i := 0; i < 10; i += 1)
    {
        Console.printLineFormatted("Iteration {0}",i);
        model.print().run()
    }    
}