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
        int get() = theX;
        
        set(int v)
        {
            theX := v
        }
    }
    
    int Y
    {
        int get() = theY;
        
        set(int v)
        {
            theY := v
        }
    }
    
    Point clone() = new Point(theX, theY);
    
//    stacksafe write int:anX int:anY
//    [
//        theX := anX.
//        theY := anY
//    ]
//    
//    
//    stacksafe read vint:anX vint:anY
//    [
//        anX int := theX.
//        anY int := theY
//    ]
    
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
    
//    stacksafe constructor new int:aWidth int:aHeight
//    [
//        theWidth := aWidth.
//        theHeight := aHeight.
//    ]
//    
    int Width
    {
        int get() = theWidth;
        
        set(int v)
        {
            theWidth := v
        }
    }

    int Height
    {
        int get() = theHeight;
        
        set(int v)
        {
            theHeight := v
        }
    }
//    
//    stacksafe read vint:aWidth vint:aHeight
//    [
//        aWidth int := theWidth.
//        aHeight int := theHeight
//    ]
//    
//    clone = Dimension new int:theWidth int:theHeight.
//    
//    bool equal dimension:aDimension
//    [
//        int aWidth := 0.
//        int aHeight := 0.
//        
//        aDimension read vint:aWidth vint:aHeight.
//        
//        ^ (theWidth == aWidth) and:(theHeight == aHeight)
//    ]
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
    
//    constructor new x:anX y:anY width:aWidth height:aHeight
//    [
//        theX := anX int.
//        theY := anY int.
//        theWidth := aWidth int.
//        theHeight := aHeight int.
//    ]
//    
//    stacksafe constructor new int:anX int:anY int:aWidth int:aHeight
//    [
//        theX := anX.
//        theY := anY.
//        theWidth := aWidth.
//        theHeight := aHeight.
//    ]

    X
    {
        int get()
        {
            ^ theX
        }
        
        set(int val)
        {
            theX := val
        }
    }

    Y
    {
        int get()
        {
            ^ theY
        }
        
        set(int val)
        {
            theY := val
        }
    }

    Width
    {
        int get()
        {
            ^ theWidth
        }
        
        set(int val)
        {
            theWidth := val
        }
    }

    Height
    {
        int get()
        {
            ^ theHeight
        }
        
        set(int val)
        {
            theHeight := val
        }
    }

//    clone = Region new int:theX int:theY int:theWidth int:theHeight.
    
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
        
//    stacksafe readXTo vint:anX
//    [
//        anX int := theX.
//    ]
//        
//    stacksafe readYTo vint:anY
//    [
//        anY int := theY.
//    ]
//        
//    stacksafe readWidthTo vint:aWidth
//    [
//        aWidth int := theWidth.
//    ]
//        
//    stacksafe readHeightTo vint:aHeight
//    [
//        aHeight int := theHeight.
//    ]
//        
//    stacksafe writeX int:aValue
//    [
//        theX := aValue.
//    ]
//    
//    stacksafe writeY int:aValue
//    [
//        theY := aValue.
//    ]
//    
//    stacksafe writeWidth int:aValue
//    [
//        theWidth := aValue.
//    ]
//    
//    stacksafe writeHeight int:aValue
//    [
//        theHeight := aValue.
//    ]
//    
//    stacksafe writeLocation int:anX int:anY
//    [
//        theX := anX.
//        theY := anY.
//    ]
    
    writeDimension(int width, int height)
    {
        theWidth := width;
        theHeight := height
    }
    
//    stacksafe readLocationTo vint:anX vint:anY
//    [
//        anX int := theX.
//        anY int := theY.
//    ]
    
    readDimensionTo(ref int width, ref int height)
    {
        width := theWidth;
        height := theHeight;
    }
    
//    point
//        = Point new int:theX int:theY.
    
    Dimension Dimension
        = new Dimension(theWidth, theHeight);
    
//    bool equal region:aRegion
//    [
//        int aWidth := 0.
//        int aHeight := 0.
//        int anX := 0.
//        int anY := 0.
//        
//        aRegion read vint:anX vint:anY vint:aWidth vint:aHeight.
//        
//        ^ (theWidth == aWidth) && (theHeight == aHeight) && (theX == anX) && (theY == anY)
//    ]
}
