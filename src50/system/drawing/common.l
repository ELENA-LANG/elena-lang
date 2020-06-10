// --- Point ---

public sealed struct Point// :: BaseNumber
{
    int theX;
    int theY;
    
    embeddable constructor()
    {
        theX := 0;
        theY := 0
    }
    
    embeddable constructor(int x, int y)
    {
        theX := x;
        theY := y
    }
    
    int X
    {
        get() = theX;
        
        set(int v)
        {
            theX := v
        }
    }
    
    int Y
    {
        get() = theY;
        
        set(int v)
        {
            theY := v
        }
    }
    
    Point clone() = new Point(theX, theY);
    
    bool equal(Point p)
    {
        int x := p.X;
        int y := p.Y;
        
        ^ (theX == x) && (theY == y)
    }
    
    bool less(Point p)
    {
        int x := p.X;
        int y := p.Y;
        
        if (theY < y)
            { ^ true };
            
        if (theY == y)
            { ^ theX < x };

        ^ false  
    }
    
    Point add(Point p)
    {
        int x := p.X;
        int y := p.Y;
        
        ^ new Point(theX + x, theY + y)
    } 
    
    Point subtract(Point p)
    {
        int x := p.X;
        int y := p.Y;
        
        ^ new Point(theX - x, theY - y)
    }
}

// --- Dimension ---

public sealed struct Dimension //:: BaseValue
{
    int theWidth;
    int theHeight;
    
    embeddable constructor()
    {
        theWidth := 0;
        theHeight := 0
    }
    
    embeddable constructor(int width, int height)
    {
        theWidth := width;
        theHeight := height
    }
    
    int Width
    {
        get() = theWidth;
        
        set(int v)
        {
            theWidth := v
        }
    }

    int Height
    {
        get() = theHeight;
        
        set(int v)
        {
            theHeight := v
        }
    }
}

// --- Region ---

public sealed struct Region //: BaseValue
{
    int theX;
    int theY;
    int theWidth;
    int theHeight;
    
    embeddable constructor()
    {
        theX := 0;
        theY := 0;
        theWidth := 0;
        theHeight := 0;
    }
    
    int X
    {
        get()
        {
            ^ theX
        }
        
        set(int val)
        {
            theX := val
        }
    }

    int Y
    {
        get()
        {
            ^ theY
        }
        
        set(int val)
        {
            theY := val
        }
    }

    int Width
    {
        get()
        {
            ^ theWidth
        }
        
        set(int val)
        {
            theWidth := val
        }
    }

    int Height
    {
        get()
        {
            ^ theHeight
        }
        
        set(int val)
        {
            theHeight := val
        }
    }

    set(int x, int y, int width, int height)
    {
        theX := x;
        theY := y;
        theWidth := width;
        theHeight := height
    }
    
    read(ref int x, ref int y, ref int width, ref int height)
    {
        x := theX;
        y := theY;
        width := theWidth;
        height := theHeight;
    }
    
    writeDimension(int width, int height)
    {
        theWidth := width;
        theHeight := height
    }
    
    readDimensionTo(ref int width, ref int height)
    {
        width := theWidth;
        height := theHeight;
    }
    
    Dimension Dimension
        = new Dimension(theWidth, theHeight);
}
