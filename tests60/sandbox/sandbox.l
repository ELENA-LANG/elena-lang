import extensions;
import system'collections;
import system'routines;

const int Yellow = 1;
const int Orange = 2;
const int Purple = 3;
const int Blue = 4;
const int Red = 5;

const int Pink = 6;

string[] Colors = new string[]{"None","Yellow","Orange","Purple","Blue","Red","Pink"};

class Tree
{
    int Star := 0;
    int[] Balls := new int[](7);
    
    constructor(star, balls)
    {
        Star := star;
        Balls := balls
    }
    
    constructor()
    {
    }
    
    contains(count,color)
        = Balls[color] == count;
        
    containsAny(color)
        = Balls[color] != 0;
 
    add(count,color)
    {
        Balls[color] := Balls[color] + count;
    }
                      
    testStar(color)
        = Star == color;
        
    setStar(color)
    {
        Star := color;
    }
    
    display()
    {
        Console.printLine("-Tree-");
        Console.printLine("Star:",Colors[Star]);
        for (int i := 1; i < 7; i++)
        {
            Console.printLine(Colors[i]," balls:",Balls[i]);
        }
    }
    
    clone()
        = new Tree(Star, Balls.clone());
}

Solution
{
    object[] conditions;
    object[] trees;
    Solver   solver;

    constructor(conditions, solver)
    {
        trees := new object[](3);
        trees[0] := new Tree();
        trees[1] := new Tree();
        trees[2] := new Tree();
        
        this solver := solver;
        this conditions := conditions;
    }
            
    constructor(conditions, trees, solver)
    {
        this conditions := conditions;
        this trees := trees;
        this solver := solver;
    }
    
    at(index)
        = trees[index];
    
    clone()
        = solver.newSolution(new Solution(conditions.clone(), trees.deepClone(), solver));

    removeCondition(id)
    {
        conditions[id] := nil;
    }

    apply()
    {
        int len := conditions.Length;
        for(int i := 0; i < len; i++)
        {
            if (conditions[i] != nil)
            {
                if (conditions[i](self, i))
                {
                    ^ true
                }
            }
        };
        
        ^ false;  
    }
                
    bool isSolved()
    {
        int len := conditions.Length;
        for(int i := 0; i < len; i++)
        {
            if (conditions[i] != nil)
            {
                ^ false
            }
        };
        
        ^ true;  
    }                
                
    solve()
    {
        while (self.apply())
        {
        };
        
        ^ self.isSolved();
    }
}

Solver
{
    object[] conditions;
    List     solutions;
    
    constructor new(params object[] conditions)
    {
        this conditions := conditions;
        this solutions := new List();
        
        self.newSolution(new Solution(this conditions, self));
    }
    
    newSolution(solution)
    {
        solutions.append(solution);
        
        ^ solution
    }
    
    resolve()
    {
        for (int i := 0; i < solutions.Length; i++)
        {
            ifnot(solutions[i].solve())
            {
                solutions[i] := nil
            }
            else
            {
                ^ solutions[i];
            }
        };
        
        ^ nil
    }
}

printTrees(trees)
{
    Console.writeLine();
    for(int i := 0; i < 3; i++)
    {
        trees[i].display();
    }
}

select(id, trees,cond,action)
{
    for(int i := 0; i < 3; i++)
    {
        if (cond(trees[i]))
        {
            action(trees[i]);
            trees.removeCondition(id);
            ^ true
        }
    };
        
    ^ false
}

select2(id, trees,cond,action)
{
    List variants := nil;
    for(int i := 0; i < 3; i++)
    {
        if (cond(trees[i]))
        {
            if (variants == nil)
            {
                variants := new List();
                variants.append(trees);
            }
            else
            {
                variants.append(trees.clone());
            }            
        }
    };
        
    int variant := 0;    
    for(int i := 0; i < 3; i++)
    {
        if (cond(trees[i]))
        {
            var sol := variants[variant];
            sol.removeCondition(id);
            
            action(sol[i]);
            
            variant += 1 
        }
    };
    
    ^ variants != nil
}

apply(cond,trees,count1,color1,count2,color2)
{
    List variants := nil;
    for(int i := 0; i < 3; i++)
    {
        bool matched := false;
        if (trees[i].containsAny(color1).Inverted && trees[i].containsAny(color2).Inverted)
        {
            matched := true
        }
        else if (trees[i].contains(count1,color1) && trees[i].containsAny(color2).Inverted)
        {
            matched := true
        }    
        else if (trees[i].contains(count2,color2) && trees[i].containsAny(color1).Inverted)
        {
            matched := true
        };
        
        if (matched)
        {
            if (variants == nil)
            {
                variants := new List();
                variants.append(trees);
            }
            else
            {
                variants.append(trees.clone());
            }
        }
    };
        
    int variant := 0;    
    for(int i := 0; i < 3; i++)
    {
        if (trees[i].containsAny(color1).Inverted && trees[i].containsAny(color2).Inverted)
        {
            var sol := variants[variant];
            sol.removeCondition(cond);
            
            sol[i].add(count1,color1);
            sol[i].add(count2,color2);
            
            variant += 1 
        }
        else if (trees[i].contains(count1,color1) && trees[i].containsAny(color2).Inverted)
        {
            var sol := variants[variant];
            sol.removeCondition(cond);
            
            sol[i].add(count2,color2);
            
            variant += 1 
        }    
        else if (trees[i].contains(count2,color2) && trees[i].containsAny(color1).Inverted)
        {
            var sol := variants[variant];
            
            sol[i].add(count1,color1);
            
            variant += 1 
        }
    };
    
    ^ variants != nil
}

Puzzle = Solver.new(
    // 1. there are 2 yellow balls on the tree with 3 orange balls
    (trees,id => select(id, trees, (t => t.contains(3,Orange)),(t => t.add(2,Yellow)))),
    // 2. on the tree with the blue star there are 3 orange balls
    (trees,id => select(id, trees, (t => t.testStar(Blue)),(t => t.add(3,Orange)))),
    // 3. the first tree has a purple star
    (trees,id) { trees[0].setStar(Purple); trees.removeCondition(id); ^ true },
    // 4. 3 yellow balls and 6 purple balls on the same tree
    (trees,id => apply(id, trees,3,Yellow,6,Purple)),
    // 5. on the tree with the purple star there are 5 blue balls
    (trees,id => select(id, trees, (t => t.testStar(Purple)),(t => t.add(5,Blue)))),
    // 6. on one tree there are 3 blue balls
    (trees,id => select2(id, trees, (t => t.containsAny(Blue).Inverted),(t => t.add(3,Blue)))),
    // 7. there are six red balls on one tree
    (trees,id => select2(id, trees, (t => t.containsAny(Red).Inverted),(t => t.add(6,Red)))),
    // 8. the tree with the blue star is between the tree with the pink star and  the tree with 5 blur balls
    (trees,id) { if(trees[0].contains(5,Blue)){ trees[1].setStar(Blue); trees[2].setStar(Pink); trees.removeCondition(id); ^ true } 
              else if(trees[2].contains(5,Blue)){ trees[1].setStar(Blue); trees[0].setStar(Pink); trees.removeCondition(id); ^ true }; ^ false },    
    // 9. 4 red balls and 2 orange balls are on the same tree
    (trees,id => apply(id, trees,4,Red,2,Orange)),
    // 10. 6 yellow balls are on the tree with the pink star
    (trees,id => select(id, trees, (t => t.testStar(Pink)),(t => t.add(6,Yellow)))),
    // 11. on the tree with 5 red balls there are 2 purle balls
    (trees,id => select(id, trees, (t => t.contains(5,Red)),(t => t.add(2,Purple)))),
    // 12. on one tree there are four orange balls
    (trees,id => select2(id, trees, (t => t.containsAny(Orange).Inverted),(t => t.add(4,Orange)))),
    // 13. 6 yellow and 5 red balls are one the same tree
    (trees,id => apply(id, trees,6,Yellow,5,Red)),
    // 14. 5 purple and 4 blue balls are on the same tree
    (trees,id => apply(id, trees,5,Purple,4,Blue))    
);

public Program()
{
    var puzzle := Puzzle;

    var solved := puzzle.resolve();
    printTrees(solved);
}