import system'collections;
import system'routines;
import extensions;
import extensions'routines;

const string[] image = new string[]{
        "                                                          ",
        " #################                   #############        ",
        " ##################               ################        ",
        " ###################            ##################        ",
        " ########     #######          ###################        ",
        "   ######     #######         #######       ######        ",
        "   ######     #######        #######                      ",
        "   #################         #######                      ",
        "   ################          #######                      ",
        "   #################         #######                      ",
        "   ######     #######        #######                      ",
        "   ######     #######        #######                      ",
        "   ######     #######         #######       ######        ",
        " ########     #######          ###################        ",
        " ########     ####### ######    ################## ###### ",
        " ########     ####### ######      ################ ###### ",
        " ########     ####### ######         ############# ###### ",
        "                                                          "};

static int[][] nbrs = new int[][]
{
    new int[]{0, -1}, new int[]{1, -1}, new int[]{1, 0},   new int[]{1, 1}, new int[]{0, 1},
    new int[]{-1, 1}, new int[]{-1, 0}, new int[]{-1, -1}, new int[]{0, -1}
};

static int[][][] nbrGroups = new int[][][]
{
    new int[][]{new int[]{0, 2, 4}, new int[]{2, 4, 6}}, 
    new int[][]{new int[]{0, 2, 6}, new int[]{0, 4, 6}}
};

extension zhangsuenOp : Matrix<CharValue>
{
    proceed(r, c, toWhite, firstStep)
    {
        if (self[r][c] != $35)
            { ^ false };
 
        int nn := self.numNeighbors(r,c);
 
        if ((nn < 2) || (nn > 6))
            { ^ false };
 
        if(self.numTransitions(r,c) != 1)
            { ^ false };
 
        if:not (self.atLeastOneIsWhite(r,c,firstStep.iif(0,1)))
            { ^ false };
 
        toWhite.append(new { x = c; y = r; });
 
        ^ true
    }
 
    numNeighbors(r,c)
    {
        int count := 0;
 
        for (int i := 0; i < nbrs.Length - 1; i += 1)
        {
            if (self[r + nbrs[i][1]][c + nbrs[i + 1][0]] == $35)
                { count += 1 }
        };
 
        ^ count;
    }
 
    numTransitions(r,c)
    {
        int count := 0;
 
        for (int i := 0; i < nbrs.Length - 1; i += 1)
        {
            if (self[r + nbrs[i][1]][c + nbrs[i][0]] == $32)
            {
                if (self[r + nbrs[i + 1][1]][c + nbrs[i + 1][0]] == $35)
                {
                    count := count + 1
                }
            }
        };
 
        ^ count
    }  
 
    atLeastOneIsWhite(r, c, step)
    {
        int count := 0;
        var group := nbrGroups[step];
        for(int i := 0; i < 2; i += 1)
        {            
            for(int j := 0; j < group[i].Length; j += 1)
            {
                var nbr := nbrs[group[i][j]];
 
                if (self[r + nbr[1]][c + nbr[0]] == $32)
                    { count := count + 1; :break; };
            }
        };
 
        ^ count > 1
    }    
 
    thinImage()
    {
        bool firstStep := false;
        bool hasChanged := true;
        var toWhite := new List();
 
        while (hasChanged || firstStep)
        {
            hasChanged := false;
            firstStep := firstStep.Inverted;
 
            for(int r := 1; r < self.Rows - 1; r += 1)
            {
                for(int c := 1; c < self.Columns - 1; c += 1)
                {
                    if(self.proceed(r,c,toWhite,firstStep))
                        { hasChanged := true }
                }
            };
 
            toWhite.forEach::(p){ self[p.y][p.x] := $32 };                
            toWhite.clear()
        }
    }
 
    print()
    {
        var it := self.enumerator();
 
        it.forEach::(ch){ Console.print(ch," ") };
        while (it.next())
        {
            Console.writeLine();

            it.forEach::(ch){ Console.print(ch," ") }
        }
    }
}
 
public program()
{
    Matrix<CharValue> grid := class Matrix<CharValue>.load(new
    {
        int Rows = image.Length;
 
        int Columns = image[0].Length;
 
        at(int i, int j)
            = image[i][j];
    });
 
    grid.thinImage();
 
    grid.print();
 
    Console.readChar()
}