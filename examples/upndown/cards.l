//// --- namespace shortcuts ---
//#define std'basic'*.
//////#define list'*  = std'collections'*.
//#define std'patterns'*.
//
//#define math'*  = std'primitives'*.
//#define cards'* = cardgame'cards'*.
//
//#define std'dictionary'*.
//#define game'*    = cardgame'dictionary'*.
//#define player'*  = cardgame'players'*.
//
//// private subjects
//#subject __count.
//
//// --- Card Constants ---
//
//#symbol Diamonds_Six=   1.
//#symbol Diamonds_Seven= 2.
//#symbol Diamonds_Eight= 3.
//#symbol Diamonds_Nine=  4.
//#symbol Diamonds_Ten=   5.
//#symbol Diamonds_Knave= 6.
//#symbol Diamonds_Queen= 7.
//#symbol Diamonds_King=  8.
//#symbol Diamonds_Ace=   9.
//
//#symbol Clubs_Six=   10.
//#symbol Clubs_Seven= 11.
//#symbol Clubs_Eight= 12.
//#symbol Clubs_Nine=  13.
//#symbol Clubs_Ten=   14.
//#symbol Clubs_Knave= 15.
//#symbol Clubs_Queen= 16.
//#symbol Clubs_King=  17.
//#symbol Clubs_Ace=   18.
//
//#symbol Hearts_Six=   19.
//#symbol Hearts_Seven= 20.
//#symbol Hearts_Eight= 21.
//#symbol Hearts_Nine=  22.
//#symbol Hearts_Ten=   23.
//#symbol Hearts_Knave= 24.
//#symbol Hearts_Queen= 25.
//#symbol Hearts_King=  26.
//#symbol Hearts_Ace=   27.
//
//#symbol Spades_Six=   28.
//#symbol Spades_Seven= 29.
//#symbol Spades_Eight= 30.
//#symbol Spades_Nine=  31.
//#symbol Spades_Ten=   32.
//#symbol Spades_Knave= 33.
//#symbol Spades_Queen= 34.
//#symbol Spades_King=  35.
//#symbol Spades_Ace=   36.
//
//#symbol Joker= Clubs_Six.
//
//#symbol Big_Joker=    37.
//#symbol Big_Card=     38.
//
//#symbol Diamonds_Min= -1.
//#symbol Hearts_Min=   -2.
//#symbol Clubs_Min=    -3.
//#symbol Spades_Min=   -4.
//
//// --- Pack ---
//
//#class Pack
//{
//    #field theCards.
//    #field theCurrent.
//
//    #role Open
//    {
//        #method last = (theCards@(theCards count - 1)).
//
//        #method get
//        [
//            theCurrent append &index:1 isnot &eof.
//    
//            ^ theCurrent content.
//        ]
//
//        #method new
//        [
//            #shift.
//            
//            self new.
//        ]
//    }
//
//    #method new
//    [
//        theCards := NewArray::36.
//
//        #var aHand := cards'Hand.
//
//        #var aCounter := Integer::0.
//        #loop (aCounter < 36)?
//        [
//            #var aCard := 1 + ext'utils'RandomValue::36.
//            #if (aHand@aCard)!
//            [
//                theCards refer &indexer:aCounter set &content:aCard.
//                (aHand@aCard) << true.
//
//                aCounter += 1.
//            ].
//        ].
//    ]
//
//    #method get
//    [
//        theCurrent := theCards indexer.
//
//        #shift Open.
//
//        ^ theCurrent content.
//    ]
//}
//
//// --- Hand ---
//
//#class Hand
//{
//    #field theDiamonds.
//    #field theClubs.
//    #field theHearts.
//    #field theSpades.
//    #field theCount.
//
//    #method new
//    [
//        theDiamonds := Integer << 0.
//        theHearts := Integer << 0.
//        theClubs := Integer << 0.
//        theSpades := Integer << 0.
//        
//        theCount := Integer << 0.
//    ]
//
//    #method game'diamonds = theDiamonds.
//
//    #method game'hearts = theHearts.
//
//    #method game'clubs = theClubs.
//
//    #method game'spades = theSpades.
//
//    #method indexer = cards'HandIndexer::self write &index:1.
//
//    #method @ anIndex = cards'HandIndexer::self write &index:anIndex.
//
//    #method clear
//    [
//        theDiamonds << 0.
//        theHearts << 0.
//        theClubs << 0.
//        theSpades << 0.
//        
//        theCount << 0.
//    ]
//
////    #method save : aWriter = aWriter write &indexable:$self.
//
//    #method __count'append : anIndex
//    [
//        theCount += anIndex.
//    ]
//    
//    #method count = NewInt32Value::theCount.    
//}
//
//// --- HandIndexer ---
//
//#class HandIndexer
//{
//    #field theHand.
//    #field theCard.
//
//    #role EOF
//    {
//        #method eof'is []
//        
//        #method eof'isnot [ $self fail. ]
//        
//        #method content = nil.
//        
//        #method save = $self fail.
//    }
//    
////    #method count'get = theHand count'get.
//    
////    #method offset'get = theCard clone.
//
//    #method eof'isnot [ ]
//    
//    #method new : aHand
//    [
//        theHand := aHand.
//        theCard := Integer::0.
//    ]
//
//    #method content
//    [
//        #if (theCard < Clubs_Six)?
//        [
//            #var aCode := 1~std'primitives'EBitwise32op add &std'primitives'__shift:(Diamonds_Six - theCard).
//
//            ^ (theHand game'diamonds)~std'primitives'EBitwise32op if &all:aCode back:true | back:false.
//        ].
//
//        #if (theCard < Hearts_Six)?
//        [
//            #var aCode := 1~std'primitives'EBitwise32op add &std'primitives'__shift:(Clubs_Six - theCard).
//
//            ^ (theHand game'clubs)~std'primitives'EBitwise32op if &all:aCode back:true | back:false.
//        ].
//
//        #if (theCard < Spades_Six)?
//        [
//            #var aCode := 1~std'primitives'EBitwise32op add &std'primitives'__shift:(Hearts_Six - theCard).
//
//            ^ (theHand game'hearts)~std'primitives'EBitwise32op if &all:aCode back:true | back:false.
//        ].
//
//        #if (theCard < Big_Joker)?
//        [
//            #var aCode := 1~std'primitives'EBitwise32op add &std'primitives'__shift:(Spades_Six - theCard).
//
//            ^ (theHand game'spades)~std'primitives'EBitwise32op if &all:aCode back:true | back:false.
//        ].
//        
//        $self fail.
//    ]
//
//    #method write : aBool
//    [
//        #var aCode := nil.
//        #var aBitRange := nil.
//
//        #if (theCard < Clubs_Six)?
//        [
//            aCode := 1~std'primitives'EBitwise32op add &std'primitives'__shift:(Diamonds_Six - theCard).
//            aBitRange := theHand game'diamonds.
//        ]
//        | [
//            #if (theCard < Hearts_Six)?
//            [
//                aCode := 1~std'primitives'EBitwise32op add &std'primitives'__shift:(Clubs_Six - theCard).
//                aBitRange := theHand game'clubs.
//
//            ]
//            | [
//                #if (theCard < Spades_Six)?
//                [
//                    aCode := 1~std'primitives'EBitwise32op add &std'primitives'__shift:(Hearts_Six - theCard).
//                    aBitRange := theHand game'hearts.
//                ]
//                | [
//                    #if (theCard < Big_Joker)?
//                    [
//                        aCode := 1~std'primitives'EBitwise32op add &std'primitives'__shift:(Spades_Six - theCard).
//                        aBitRange := theHand game'spades.
//                    ].
//                ].
//            ].
//        ].
//
//        #if aBool
//        ? [
//            #if aBitRange~std'primitives'EBitwise32op if &all:aCode
//            | [
//                theHand __count'append:1. 
//                
//                aBitRange << aBitRange or:aCode.
//            ].
//        ]
//        | ! [
//            #if aBitRange~std'primitives'EBitwise32op if &all:aCode
//            [
//                theHand __count'append:-1. 
//                
//                aBitRange << aBitRange and:(aCode~std'primitives'EBitwise32op inverted).
//            ].
//        ]
//        | [ $self fail. ].
//    ]
//
//
//    #method index'save : aWriter = theCard save:aWriter.
//
//    #method index'if : anIndex
//    [
//        (0 < anIndex)and:(anIndex <= 36)?.
//    ]
//        
//    #method index'write : anIndex
//    [
//        theCard << anIndex.
//        
//        #if $self if &index:theCard
//            | [ #shift EOF. ].
//    ]
//        
//    #method index'append : anIndex
//    [
//        theCard += anIndex.
//        
//        #if $self if &index:theCard
//            | [ #shift EOF. ].
//    ]
//        
//    #method index'reduce : anIndex
//    [
//        theCard -= anIndex.
//        
//        #if $self if &index:theCard
//            | [ #shift EOF. ].
//    ]
//
//    #method index = __wrap(EIndexAdapter, self).
//
//    #method ? [ control if:(self content). ]
//
//    #method ! [ control ifNot:(self content). ]
//}
//
//// --- Suit ---
//
//#class $Suit (Magnitude)
//{
//    #method player'hand'if : aHand
//    [
//        #var aFound := Boolean << false.
//        
//        aHand run: aCard =>
//        [
//            #if (self == cards'Suit::aCard)?
//                [ aFound << true. ^false. ].
//        ].
//        
//        aFound?.
//    ]
//}
//
//// --- Suits ---
//
//#symbol Diamonds = $Suit
//{
//   $isDiamonds []
//   
//   ifequal : aSuit [ aSuit $isDiamonds. ]
//}.
//
//#symbol Clubs = $Suit
//{
//   $isClubs []
//   
//   ifequal : aSuit [ aSuit $isClubs. ]
//}.
//
//#symbol Hearts = $Suit
//{
//   $isHearts []
//   
//   ifequal : aSuit [ aSuit $isHearts. ]
//}.
//
//#symbol Spades = $Suit
//{
//   $isSpades []
//   
//   ifequal : aSuit [ aSuit $isSpades. ]
//}.
//
//#symbol None = $Suit
//{
//   ifequal [ $self fail. ]
//}.
//
//#symbol Suit : aCard
//    = Int32Value::aCard
//        ifequal:Hearts_Min back:Hearts
//        | ifequal:Clubs_Min back:Clubs
//        | ifequal:Spades_Min back:Spades
//        | ifequal:Diamonds_Min back:Diamonds
//        | ifequal:0 back:None
//        | ifless:Clubs_Six back:Diamonds
//        | ifless:Hearts_Six back:Clubs
//        | ifless:Spades_Six back:Hearts
//        | ifless:Big_Joker back:Spades.
//
//// --- BigestCard ---
//
//#symbol BigestCard : aSuit =
//{
//    find : aHand
//    [
//        #var aBiggestCard := Integer::0.
//        
//        aHand run: aCard =>
//        [
//            #if Control if:(aSuit == Suit::aCard) if:(aBiggestCard < aCard)
//                [ aBiggestCard << aCard. ].
//        ].
//        
//        ^ aBiggestCard.
//    ]
//}.
