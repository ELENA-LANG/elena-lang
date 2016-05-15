#import system.

// --- common types ---
#subject coord     = system'drawing'Point.
#subject dimension = system'drawing'Dimension.
#subject region    = system'drawing'Region.

// --- Point ---

#class(sealed,struct:embeddable,dispatchable:coord)Point :: BaseNumber
{
    #field(int)theX.
    #field(int)theY.
    
    #constructor new
    [
        theX := 0.
        theY := 0.
    ]
    
    #constructor new &x:anX &y:anY
    [
        theX := anX int.
        theY := anY int.
    ]
    
    #constructor(stacksafe) new &int:anX &int:anY
    [
        theX := anX.
        theY := anY.
    ]
    
    #method clone = Point new &int:theX &int:theY.
    
    #method(stacksafe) write &int:anX &int:anY
    [
        theX := anX.
        theY := anY.
    ]
    
    #method(stacksafe,innprop:x) writeX &int:aValue
    [
        theX := aValue.
    ]
    
    #method(stacksafe,innprop:y) writeY &int:aValue
    [
        theY := aValue.
    ]
    
    #method(stacksafe,outnprop:x) readX &vint:aValue
    [
        aValue := theX.
    ]
    
    #method(stacksafe,outnprop:y) readY &vint:aValue
    [
        aValue := theY.
    ]
    
    #method(stacksafe) read &vint:anX &vint:anY
    [
        anX := theX.
        anY := theY.
    ]
    
    #method(bool,suppress:w2) equal &coord:aPoint
    [
        #var(int)anX := aPoint x.
        #var(int)anY := aPoint y.
        
        ^ (theX == anX) and:(theY == anY).
    ]
    
    #method(bool) less &coord:aPoint
    [
        #var(int)anX := aPoint x.
        #var(int)anY := aPoint y.
        
        (theY < anY)
            ? [ ^ true. ].
            
        (theY == anY)
            ? [ ^ theX < anX. ].

        ^ false.            
    ]
    
    #method add &coord:aPoint
    [
        #var(int)anX := aPoint x.
        #var(int)anY := aPoint y.
        
        ^ Point new &int:(theX + anX) &int:(theY + anY).
    ] 
    
    #method subtract &coord:aPoint
    [
        #var(int)anX := aPoint x.
        #var(int)anY := aPoint y.
        
        ^ Point new &int:(theX - anX) &int:(theY - anY).
    ] 
}

// --- Dimension ---

#class(sealed,struct:embeddable,dispatchable:dimension)Dimension :: BaseValue
{
    #field(int)theWidth.
    #field(int)theHeight.
    
    #constructor new
    [
        theWidth := 0.
        theHeight := 0.
    ]
    
    #constructor new &width:aWidth &height:aHeight
    [
        theWidth := aWidth int.
        theHeight := aHeight int.
    ]
    
    #constructor(stacksafe) new &int:aWidth &int:aHeight
    [
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    #method(stacksafe,innprop:width) writeWidth &int:aValue
    [
        theWidth := aValue.
    ]
    
    #method(stacksafe,innprop:height) writeHeight &int:aValue
    [
        theHeight := aValue.
    ]
    
    #method(stacksafe) write &int:aWidth &int:aHeight
    [
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    #method(stacksafe,outnprop:width) readWidth &vint:aValue
    [
        aValue := theWidth.
    ]
    
    #method(stacksafe,outnprop:height) readHeight &vint:aValue
    [
        aValue := theHeight.
    ]
    
    #method(stacksafe) read &vint:aWidth &vint:aHeight
    [
        aWidth := theWidth.
        aHeight := theHeight.
    ]
    
    #method clone = Dimension new &int:theWidth &int:theHeight.
    
    #method(bool,suppress:w2) equal &dimension:aDimension
    [
        #var(int)aWidth.
        #var(int)aHeight.
        
        aDimension read &vint:aWidth &vint:aHeight.
        
        ^ (theWidth == aWidth) and:(theHeight == aHeight).
    ]
}

// --- Region ---

#class(sealed,struct:embeddable,dispatchable:region)Region :: BaseValue
{
    #field(int)theX.
    #field(int)theY.
    #field(int)theWidth.
    #field(int)theHeight.
    
    #constructor new
    [
        theX := 0.
        theY := 0.
        theWidth := 0.
        theHeight := 0.
    ]
    
    #constructor new &x:anX &y:anY &width:aWidth &height:aHeight
    [
        theX := anX int.
        theY := anY int.
        theWidth := aWidth int.
        theHeight := aHeight int.
    ]
    
    #constructor(stacksafe) new &int:anX &int:anY &int:aWidth &int:aHeight
    [
        theX := anX.
        theY := anY.
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    #method clone = Region new &int:theX &int:theY &int:theWidth &int:theHeight.
    
    #method write &int:anX &int:anY &int:aWidth &int:aHeight
    [
        theX := anX.
        theY := anY.
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    #method(stacksafe) read &vint:anX &vint:anY &vint:aWidth &vint:aHeight
    [
        anX := theX.
        anY := theY.
        aWidth := theWidth.
        aHeight := theHeight.
    ]
        
    #method(stacksafe,outnprop:x) readX &vint:anX
    [
        anX := theX.
    ]
        
    #method(stacksafe,outnprop:y) readY &vint:anY
    [
        anY := theY.
    ]
        
    #method(stacksafe,outnprop:width) readWidth &vint:aWidth
    [
        aWidth := theWidth.
    ]
        
    #method(stacksafe,outnprop:height) readHeight &vint:aHeight
    [
        aHeight := theHeight.
    ]
        
    #method(stacksafe,innprop:x) writeX &int:aValue
    [
        theX := aValue.
    ]
    
    #method(stacksafe,innprop:y) writeY &int:aValue
    [
        theY := aValue.
    ]
    
    #method(stacksafe,innprop:width) writeWidth &int:aValue
    [
        theWidth := aValue.
    ]
    
    #method(stacksafe,innprop:height) writeHeight &int:aValue
    [
        theHeight := aValue.
    ]
    
    #method(stacksafe) writeLocation &int:anX &int:anY
    [
        theX := anX.
        theY := anY.
    ]
    
    #method(stacksafe) writeDimension &int:aWidth &int:aHeight
    [
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    #method(stacksafe) readLocation &vint:anX &vint:anY
    [
        anX := theX.
        anY := theY.
    ]
    
    #method(stacksafe) readDimension &vint:aWidth &vint:aHeight
    [
        aWidth := theWidth.
        aHeight := theHeight.
    ]
    
    #method point
        = Point new &int:theX &int:theY.
    
    #method dimension
        = Dimension new &int:theWidth &int:theHeight.
    
    #method(bool,suppress:w2) equal &region:aRegion
    [
        #var(int)aWidth.
        #var(int)aHeight.
        #var(int)anX.
        #var(int)anY.
        
        aRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        ^ (theWidth == aWidth) and:(theHeight == aHeight) and:(theX == anX) and:(theY == anY).
    ]
}
