//// --- common types ---
//type coord     = system'drawing'Point.
//type dimension = system'drawing'Dimension.
//type region    = system'drawing'Region.
//
//// --- Point ---
//
//sealed struct Point :: BaseNumber
//{
//    int theX.
//    int theY.
//    
//    embeddable constructor new
//    [
//        theX := 0.
//        theY := 0.
//    ]
//    
//    constructor new x:anX y:anY
//    [
//        theX := anX int.
//        theY := anY int.
//    ]
//    
//    stacksafe constructor new int:anX int:anY
//    [
//        theX := anX.
//        theY := anY.
//    ]
//    
//    int vprop x :: readXTo, writeX.
//    int vprop y :: readYTo, writeY.
//    
//    clone = Point new int:theX int:theY.
//    
//    stacksafe write int:anX int:anY
//    [
//        theX := anX.
//        theY := anY
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
//    stacksafe readXTo vint:aValue
//    [
//        aValue int := theX.
//    ]
//    
//    stacksafe readYTo vint:aValue
//    [
//        aValue int := theY.
//    ]
//    
//    stacksafe read vint:anX vint:anY
//    [
//        anX int := theX.
//        anY int := theY
//    ]
//    
//    bool equal coord:aPoint
//    [
//        int anX := aPoint x.
//        int anY := aPoint y.
//        
//        ^ (theX == anX) and:(theY == anY)
//    ]
//    
//    bool less coord:aPoint
//    [
//        int anX := aPoint x.
//        int anY := aPoint y.
//        
//        if (theY < anY)
//            [ ^ true ].
//            
//        if (theY == anY)
//            [ ^ theX < anX ].
//
//        ^ false  
//    ]
//    
//    add coord:aPoint
//    [
//        int anX := aPoint x.
//        int anY := aPoint y.
//        
//        ^ Point new int:(theX + anX) int:(theY + anY).
//    ] 
//    
//    subtract coord:aPoint
//    [
//        int anX := aPoint x.
//        int anY := aPoint y.
//        
//        ^ Point new int:(theX - anX) int:(theY - anY)
//    ] 
//}
//
//// --- Dimension ---
//
//sealed struct Dimension :: BaseValue
//{
//    int theWidth.
//    int theHeight.
//    
//    embeddable constructor new
//    [
//        theWidth := 0.
//        theHeight := 0.
//    ]
//    
//    constructor new width:aWidth height:aHeight
//    [
//        theWidth := aWidth int.
//        theHeight := aHeight int.
//    ]
//    
//    stacksafe constructor new int:aWidth int:aHeight
//    [
//        theWidth := aWidth.
//        theHeight := aHeight.
//    ]
//    
//    int vprop width :: readWidthTo, writeWidth.
//    int vprop height :: readHeightTo, writeHeight.
//    
//    stacksafe writeWidth int:aValue
//    [
//        theWidth := aValue
//    ]
//    
//    stacksafe writeHeight int:aValue
//    [
//        theHeight := aValue
//    ]
//    
//    stacksafe write int:aWidth int:aHeight
//    [
//        theWidth := aWidth.
//        theHeight := aHeight
//    ]
//    
//    stacksafe readWidthTo vint:aValue
//    [
//        aValue int := theWidth.
//    ]
//    
//    stacksafe readHeightTo vint:aValue
//    [
//        aValue int := theHeight.
//    ]
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
//}

// --- Region ---

public sealed struct Region : BaseValue
{
    int theX;
    int theY;
    int theWidth;
    int theHeight;
    
    /*embeddable */constructor()
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
//
//    int vprop x      :: readXTo, writeX.
//    int vprop y      :: readYTo, writeY.
//    int vprop width  :: readWidthTo, writeWidth.
//    int vprop height :: readHeightTo, writeHeight.
//        
//    clone = Region new int:theX int:theY int:theWidth int:theHeight.
//    
//    write int:anX int:anY int:aWidth int:aHeight
//    [
//        theX := anX.
//        theY := anY.
//        theWidth := aWidth.
//        theHeight := aHeight.
//    ]
    
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
//    
//    stacksafe writeDimension int:aWidth int:aHeight
//    [
//        theWidth := aWidth.
//        theHeight := aHeight.
//    ]
//    
//    stacksafe readLocationTo vint:anX vint:anY
//    [
//        anX int := theX.
//        anY int := theY.
//    ]
//    
//    stacksafe readDimensionTo vint:aWidth vint:aHeight
//    [
//        aWidth int := theWidth.
//        aHeight int := theHeight.
//    ]
//    
//    point
//        = Point new int:theX int:theY.
//    
//    dimension
//        = Dimension new int:theWidth int:theHeight.
//    
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
