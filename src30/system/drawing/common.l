#import system.

// --- common types ---
#subject(class:system'drawing'Point)coord.
#subject(class:system'drawing'Dimension)dimension.
#subject(class:system'drawing'Region)region.

// --- Point ---

#class(sealed,struct:embeddable)Point :: BaseNumber
{
    #field(type:int)theX.
    #field(type:int)theY.
    
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
    
    #method coord = $self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &coord:$self.
    
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &coord:$self.
    
    #method clone = Point new &x:theX &y:theY.
    
    #method x = IntNumber new &int:theX.
    
    #method y = IntNumber new &int:theX.
    
    #method(stacksafe) write &int:anX &int:anY
    [
        theX := anX.
        theY := anY.
    ]
    
    #method write &x:aValue
    [
        theX := aValue int.
    ]
    
    #method write &y:aValue
    [
        theY := aValue int.
    ]
    
    #method(stacksafe) readX &vint:aValue
    [
        aValue << theX.
    ]
    
    #method(stacksafe) readY &vint:aValue
    [
        aValue << theY.
    ]
    
    #method(stacksafe) read &vint:anX &vint:anY
    [
        anX << theX.
        anY << theY.
    ]
    
    #method(type:bool) equal &coord:aPoint
    [
        #var(type:int)anX.
        #var(type:int)anY.
        
        aPoint readX &vint:anX.
        aPoint readY &vint:anY.
        
        ^ (theX == anX) and:(theY == anY).
    ]
    
    #method(type:bool) less &coord:aPoint
    [
        #var(type:int)anX.
        #var(type:int)anY.
        
        aPoint readX &vint:anX.
        aPoint readY &vint:anY.
        
        (theY < anY)
            ? [ ^ true. ].
            
        (theY == anY)
            ? [ ^ theX < anX. ].

        ^ false.            
    ]
    
    #method add &coord:aPoint
    [
        #var(type:int)anX.
        #var(type:int)anY.
        
        aPoint readX &vint:anX.
        aPoint readY &vint:anY.
        
        ^ Point new &int:(theX + anX) &int:(theY + anY).
    ] 
    
    #method subtract &coord:aPoint
    [
        #var(type:int)anX.
        #var(type:int)anY.
        
        aPoint readX &vint:anX.
        aPoint readY &vint:anY.
        
        ^ Point new &int:(theX - anX) &int:(theY - anY).
    ] 
}

// --- Dimension ---

#class(sealed,struct:embeddable)Dimension :: BaseValue
{
    #field(type:int)theWidth.
    #field(type:int)theHeight.
    
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
    
    #constructor new &int:aWidth &int:aHeight
    [
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    #method dimension = $self.
    
    #method width = IntNumber new &int:theWidth.
    
    #method height = IntNumber new &int:theHeight.
    
    #method write &width:aValue
    [
        theWidth := aValue int.
    ]
    
    #method write &height:aValue
    [
        theHeight := aValue int.
    ]
    
    #method(stacksafe) write &int:aWidth &int:aHeight
    [
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    #method(stacksafe) readWidth &vint:aValue
    [
        aValue << theWidth.
    ]
    
    #method(stacksafe) readHeight &vint:aValue
    [
        aValue << theHeight.
    ]
    
    #method(stacksafe) read &vint:aWidth &vint:aHeight
    [
        aWidth << theWidth.
        aHeight << theHeight.
    ]
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &dimension:$self.
    
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &dimension:$self.
    
    #method clone = Dimension new &width:theWidth &height:theHeight.
    
    #method(type:bool) equal &dimension:aDimension
    [
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
        aDimension read &vint:aWidth &vint:aHeight.
        
        ^ (theWidth == aWidth) and:(theHeight == aHeight).
    ]
}

// --- Region ---

#class(sealed,struct:embeddable)Region :: BaseValue
{
    #field(type:int)theX.
    #field(type:int)theY.
    #field(type:int)theWidth.
    #field(type:int)theHeight.
    
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
    
    #constructor new &int:anX &int:anY &int:aWidth &int:aHeight
    [
        theX := anX.
        theY := anY.
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    #method region = $self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &region:$self.
    
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &region:$self.
    
    #method clone = Region new &int:theX &int:theY &int:theWidth &int:theHeight.
    
    #method x = IntNumber new &int:theX.
    
    #method y = IntNumber new &int:theX.
    
    #method width = IntNumber new &int:theWidth.
    
    #method height = IntNumber new &int:theHeight.

    #method write &int:anX &int:anY &int:aWidth &int:aHeight
    [
        theX := anX.
        theY := anY.
        theWidth := aWidth.
        theHeight := aHeight.
    ]
    
    #method(stacksafe) read &vint:anX &vint:anY &vint:aWidth &vint:aHeight
    [
        anX << theX.
        anY << theY.
        aWidth << theWidth.
        aHeight << theHeight.
    ]
        
    #method(stacksafe) readX &vint:anX
    [
        anX << theX.
    ]
        
    #method(stacksafe) readY &vint:anY
    [
        anY << theY.
    ]
        
    #method(stacksafe) readWidth &vint:aWidth
    [
        aWidth << theWidth.
    ]
        
    #method(stacksafe) readHeight &vint:aHeight
    [
        aHeight << theHeight.
    ]
        
    #method write &x:aValue
    [
        theX := aValue int.
    ]
    
    #method write &y:aValue
    [
        theY := aValue int.
    ]
    
    #method write &width:aValue
    [
        theWidth := aValue int.
    ]
    
    #method write &height:aValue
    [
        theHeight := aValue int.
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
        anX << theX.
        anY << theY.
    ]
    
    #method(stacksafe) readDimension &vint:aWidth &vint:aHeight
    [
        aWidth << theWidth.
        aHeight << theHeight.
    ]
    
    #method point
        = Point new &int:theX &int:theY.
    
    #method dimension
        = Dimension new &int:theWidth &int:theHeight.
    
    #method(type:bool) equal &region:aRegion
    [
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        #var(type:int)anX.
        #var(type:int)anY.
        
        aRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        ^ (theWidth == aWidth) and:(theHeight == aHeight) and:(theX == anX) and:(theY == anY).
    ]
}
