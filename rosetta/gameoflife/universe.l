#define system.
#define system'collections.

// --- Point ---

#class Point
{
    #field theX.
    #field theY.
    
    #constructor new
    [
        theX := 0.
        theY := 0.
    ]
    
    #constructor new &x:x &y:y
    [
        theX := x int.
        theY := y int.
    ]

    #method x = theX.
    
    #method y = theY.
            
    #method add &width:w &height:h
    [
        #var(type:int)x := theX + w - 1.
        #var(type:int)y := theY + h - 1.
        
        ^ Point new &x:x &y:y.
    ]
}

// --- Cellular ---

#class Cellular
{
    #field theCoordinate.
    
    #constructor new &x:x &y:y
    [
        theCoordinate := Point new &x:x &y:y.
    ]
    
    #method xy = theCoordinate.
    
    #method x => theCoordinate.
    
    #method y => theCoordinate.   
}

// --- Scope ---

#class Scope
{
    #field theSpace.
    
    #field theObjects.
    #field theTopLeft.
    #field theBottomRight.

    #field theNorthWest.
    #field theNorth.
    #field theNorthEast.
    #field theEast.
    #field theSouthEast.
    #field theSouth.    
    #field theSouthWest.
    #field theWest.

    #constructor new &space:aSpace &x:x &y:y &width:w &height:h
    [
        theSpace := aSpace.
        
        theTopLeft := Point new &x:x &y:y.
        theBottomRight := theTopLeft add &width:w &height:h.
    ]

    #constructor new &space:aSpace &x:x &y:y &width:w &height:h &north:aNorth &south:aSouth
    [
        theSpace := aSpace.
        theNorth := aNorth.
        theSouth := aSouth.
        
        theTopLeft := Point new &x:x &y:y.
        theBottomRight := theTopLeft add &width:w &height:h.
    ]

    #constructor new &space:aSpace &x:x &y:y &width:w &height:h &east:anEast &west:aWest
    [
        theSpace := aSpace.
        theEast := anEast.
        theWest := aWest.
        
        theTopLeft := Point new &x:x &y:y.
        theBottomRight := theTopLeft add &width:w &height:h.
    ]

    #constructor new &space:aSpace &x:x &y:y &width:w &height:h &nothEast:aNorthEast &southWest:aSouthWest
    [
        theSpace := aSpace.
        theNorthEast := aNorthEast.
        theSouthWest := aSouthWest.
        
        theTopLeft := Point new &x:x &y:y.
        theBottomRight := theTopLeft add &width:w &height:h.
    ]

    #constructor new &space:aSpace &x:x &y:y &width:w &height:h &northWest:aNorthWest &southEast:aSouthEast
    [
        theSpace := aSpace.
        theNorthWest := aNorthWest.
        theSouthEast := aSouthEast.
        
        theTopLeft := Point new &x:x &y:y.
        theBottomRight := theTopLeft add &width:w &height:h.
    ]

    #method northWest = theNorthWest.
    
    #method north = theNorth.
    
    #method northEast = theNorthEast.
    
    #method east = theEast.
    
    #method southEast = theSouthEast.
    
    #method south = theSouth.
    
    #method southWest = theSouthWest.
    
    #method west = theWest.

    #method center = $self.

    #method append &center : anObject
    [
        (nil == theObjects)
            ? [ theObjects := List new. ].
            
        theObjects append:anObject.
    ]

    #method append &northWest : anObject
    [
        theNorthWest := theSpace create &scope &xy::anObject &southEast:$self.
            
        theNorthWest append:anObject.
    ]    

    #method append &north : anObject
    [
        theNorth := theSpace create &scope &xy::anObject &south:$self.
            
        theNorth append:anObject.
    ]    

    #method append &northEast : anObject
    [
        theNorthEast := theSpace create &scope &xy::anObject &southWest:$self.
            
        theNorthEast append:anObject.
    ]    

    #method append &east : anObject
    [
        theEast := theSpace create &scope &xy::anObject &west:$self.
            
        theEast append:anObject.
    ]    

    #method append &southEast : anObject
    [
        theSouthEast := theSpace create &scope &xy::anObject &northWest:$self.
            
        theSouthEast append:anObject.
    ]    

    #method append &south : anObject
    [
        theSouth := theSpace create &scope &xy::anObject &north:$self.
            
        theSouth append:anObject.
    ]    

    #method append &southWest : anObject
    [
        theSouthWest := theSpace create &scope &xy::anObject &northEast:self.
            
        theSouthWest append:anObject.
    ]    

    #method append &west : anObject
    [
        theWest := theSpace create &scope &xy::anObject &east:$self.
            
        theWest append:anObject.
    ]    

    #method findCourse &x:x &y:y
    [
        #var x1 := theTopLeft x.
        #var y1 := theTopLeft y.
        #var x2 := theBottomRight x.
        #var y2 := theBottomRight y.
        
        ^ y =>
            < y1 ? [ 
                     x => 
                        < x1 ? [ %northWest ]
                        > x2 ? [ %northEast ]
                        ! [ %north ] 
                   ]
            > y2 ? [ 
                     x => 
                        < x1 ? [ %southWest ]
                        > x2 ? [ %southEast ]
                        ! [ %south ] 
                   ]
           ! [
                 x => 
                    < x1 ? [ %west ]
                    > x2 ? [ %east ]
                    ! [ %center ]               
           ].
    ]

    #method find &xy:aPoint
    [
        #var aDirection := $self findCourse &x::aPoint &y::aPoint.
        
        #var aScope := $self~aDirection get.

        ^ (nil == aScope) ? [ $self ] ! [ aScope ].
                        
    ]

    #method append : anObject
    [
        #var aDirection := $self findCourse &x::anObject &y::anObject.
        
        ^ $self~aDirection append:anObject.
    ]
}

// --- Space ---

#class Space
{
    #field theScope.
    #field theScale.
    
    #constructor new : theObjects &scale:aScale
    [
        theScale := aScale int.
        theScope := Scope new &space:$self &x:0 &y:0 &width:theScale &height:theScale.
        
        control foreach:theObjects &do: anObject [ $self append &cell:anObject ].
    ]

    #method create &scope &xy:aPoint &north:aNorth
        = Scope new &space:$self &x:(aPoint x / theScale * theScale) &y:(aPoint y / theScale * theScale) &width:theScale &height:theScale &north:aNorth &south::aNorth.

    #method create &scope &xy:aPoint &south:aSouth
        = Scope new &space:$self &x:(aPoint x / theScale * theScale) &y:(aPoint y / theScale * theScale) &width:theScale &height:theScale &north::aSouth &south:aSouth.

    #method create &scope &xy:aPoint &east:anEast
        = Scope new &space:$self &x:(aPoint x / theScale * theScale) &y:(aPoint y / theScale * theScale) &width:theScale &height:theScale &east:anEast &west::anEast.

    #method create &scope &xy:aPoint &west:aWest
        = Scope new &space:$self &x:(aPoint x / theScale * theScale) &y:(aPoint y / theScale * theScale) &width:theScale &height:theScale &east::aWest &west:aWest.

    #method create &scope &xy:aPoint &northWest:aNorthWest
        = Scope new &space:$self &x:(aPoint x / theScale * theScale) &y:(aPoint y / theScale * theScale) &width:theScale &height:theScale &northWest:aNorthWest &southEast::aNorthWest.

    #method create &scope &xy:aPoint &northEast:aNorthEast
        = Scope new &space:$self &x:(aPoint x / theScale * theScale) &y:(aPoint y / theScale * theScale) &width:theScale &height:theScale &northEast:aNorthEast &southWest::aNorthEast.

    #method create &scope &xy:aPoint &southWest:aSouthWest
        = Scope new &space:$self &x:(aPoint x / theScale * theScale) &y:(aPoint y / theScale * theScale) &width:theScale &height:theScale &southWest:aSouthWest &northEast::aSouthWest.

    #method create &scope &xy:aPoint &southEast:aSouthEast
        = Scope new &space:$self &x:(aPoint x / theScale * theScale) &y:(aPoint y / theScale * theScale) &width:theScale &height:theScale &northWest::aSouthEast &southEast:aSouthEast.
    
    #method append &cell:anObject
    [
        #var aScope := theScope find &xy:(anObject xy).

        aScope append:anObject.
    ]

//    #method next
//    [
//    ]
}