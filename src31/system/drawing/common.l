// --- common types ---
#subject coord     = system'drawing'Point.
#subject dimension = system'drawing'Dimension.
#subject region    = system'drawing'Region.

// --- Point ---

sealed embeddable dispatchable(coord) struct Point :: BaseNumber
{
    int theX.
    int theY.
    
    constructor new
    [
        theX := 0.
        theY := 0.
    ]
    
    constructor new &x:anX &y:anY
    [
        theX := anX int.
        theY := anY int.
    ]
    
    stacksafe constructor new &int:anX &int:anY
    [
        theX := anX.
        theY := anY.
    ]
    
    clone = Point new &int:theX &int:theY.
    
    stacksafe write &int:anX &int:anY
    [
        theX := anX.
        theY := anY.
    ]
    
    stacksafe innprop(x) writeX &int:aValue
    [
        theX := aValue.
    ]
    
    stacksafe innprop(y) writeY &int:aValue
    [
        theY := aValue.
    ]
    
    stacksafe outnprop(x) readX &vint:aValue
    [
        aValue := theX.
    ]
    
    stacksafe outnprop(y) readY &vint:aValue
    [
        aValue := theY.
    ]
    
    stacksafe read &vint:anX &vint:anY
    [
        anX := theX.
        anY := theY.
    ]
    
    bool equal &coord:aPoint
    [
        int anX := aPoint x.
        int anY := aPoint y.
        
        ^ (theX == anX) and:(theY == anY).
    ]
    
    bool less &coord:aPoint
    [
        int anX := aPoint x.
        int anY := aPoint y.
        
        if (theY < anY)
            [ ^ true. ].
            
        if (theY == anY)
            [ ^ theX < anX. ].

        ^ false.            
    ]
    
    add &coord:aPoint
    [
        int anX := aPoint x.
        int anY := aPoint y.
        
        ^ Point new &int:(theX + anX) &int:(theY + anY).
    ] 
    
    subtract &coord:aPoint
    [
        int anX := aPoint x.
        int anY := aPoint y.
        
        ^ Point new &int:(theX - anX) &int:(theY - anY).
    ] 
}

// --- Dimension ---

sealed embeddable dispatchable(dimension) struct Dimension :: BaseValue
{
    int theWidth.
    int theHeight.
    
    constructor new
    [
        theWidth := 0.
        theHeight := 0.
    ]
    
    constructor new &width:aWidth &height:aHeight
    [
        theWidth := aWidth int.
        theHeight := aHeight int.
    ]
    
    stacksafe constructor new &int:aWidth &int:aHeight
    [
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    stacksafe innprop(width) writeWidth &int:aValue
    [
        theWidth := aValue.
    ]
    
    stacksafe innprop(height) writeHeight &int:aValue
    [
        theHeight := aValue.
    ]
    
    stacksafe write &int:aWidth &int:aHeight
    [
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    stacksafe outnprop(width) readWidth &vint:aValue
    [
        aValue := theWidth.
    ]
    
    stacksafe outnprop(height) readHeight &vint:aValue
    [
        aValue := theHeight.
    ]
    
    stacksafe read &vint:aWidth &vint:aHeight
    [
        aWidth := theWidth.
        aHeight := theHeight.
    ]
    
    clone = Dimension new &int:theWidth &int:theHeight.
    
    bool equal &dimension:aDimension
    [
        int aWidth := 0.
        int aHeight := 0.
        
        aDimension read &vint:aWidth &vint:aHeight.
        
        ^ (theWidth == aWidth) and:(theHeight == aHeight).
    ]
}

// --- Region ---

sealed embeddable dispatchable(region) struct Region :: BaseValue
{
    int theX.
    int theY.
    int theWidth.
    int theHeight.
    
    constructor new
    [
        theX := 0.
        theY := 0.
        theWidth := 0.
        theHeight := 0.
    ]
    
    constructor new &x:anX &y:anY &width:aWidth &height:aHeight
    [
        theX := anX int.
        theY := anY int.
        theWidth := aWidth int.
        theHeight := aHeight int.
    ]
    
    stacksafe constructor new &int:anX &int:anY &int:aWidth &int:aHeight
    [
        theX := anX.
        theY := anY.
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    clone = Region new &int:theX &int:theY &int:theWidth &int:theHeight.
    
    write &int:anX &int:anY &int:aWidth &int:aHeight
    [
        theX := anX.
        theY := anY.
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    stacksafe read &vint:anX &vint:anY &vint:aWidth &vint:aHeight
    [
        anX := theX.
        anY := theY.
        aWidth := theWidth.
        aHeight := theHeight.
    ]
        
    stacksafe outnprop(x) readX &vint:anX
    [
        anX := theX.
    ]
        
    stacksafe outnprop(y) readY &vint:anY
    [
        anY := theY.
    ]
        
    stacksafe outnprop(width) readWidth &vint:aWidth
    [
        aWidth := theWidth.
    ]
        
    stacksafe outnprop(height) readHeight &vint:aHeight
    [
        aHeight := theHeight.
    ]
        
    stacksafe innprop(x) writeX &int:aValue
    [
        theX := aValue.
    ]
    
    stacksafe innprop(y) writeY &int:aValue
    [
        theY := aValue.
    ]
    
    stacksafe innprop(width) writeWidth &int:aValue
    [
        theWidth := aValue.
    ]
    
    stacksafe innprop(height) writeHeight &int:aValue
    [
        theHeight := aValue.
    ]
    
    stacksafe writeLocation &int:anX &int:anY
    [
        theX := anX.
        theY := anY.
    ]
    
    stacksafe writeDimension &int:aWidth &int:aHeight
    [
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    stacksafe readLocation &vint:anX &vint:anY
    [
        anX := theX.
        anY := theY.
    ]
    
    stacksafe readDimension &vint:aWidth &vint:aHeight
    [
        aWidth := theWidth.
        aHeight := theHeight.
    ]
    
    point
        = Point new &int:theX &int:theY.
    
    dimension
        = Dimension new &int:theWidth &int:theHeight.
    
    bool equal &region:aRegion
    [
        int aWidth := 0.
        int aHeight := 0.
        int anX := 0.
        int anY := 0.
        
        aRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        ^ (theWidth == aWidth) and:(theHeight == aHeight) and:(theX == anX) and:(theY == anY).
    ]
}
