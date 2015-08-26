#import system.
#import system'calendar.
#import system'collections.
#import extensions.

#symbol(const,type:int)ARRAY_SIZE = 32768.

#symbol program =
[
    #var casas := 1000.
    ('program'arguments length > 1)?
        [ casas := 'program'arguments@1 toInt. ].
    
    #var aStart := now.
    
	//#var x := system'Array new &length:ARRAY_SIZE.
    #var(type:intarray) x := system'IntArray new:ARRAY_SIZE.
	#var(type:int) xc := 0.
	#var(type:int) cnt := 0.
	#loop (cnt < ARRAY_SIZE) ? [ x@cnt := 0. cnt := cnt + 1. ].
	xc := xc + 1.
	x@xc := x@xc + casas int.
	#loop (x@xc != 0) ?
	[
		xc := xc - 1.
		x@xc := x@xc + 1.
		xc := xc + 8.
		x@xc := x@xc + 10.
		xc := xc -7.
		x@xc := x@xc - 1.
	].
	xc := xc + 1.
	x@xc := x@xc + 5.
	#loop (x@xc != 0) ?
	[
		xc := xc - 1.
		x@xc := x@xc + 9.
		xc := xc + 1.
		x@xc := x@xc - 1.
	].
	x@xc := x@xc + 1.
	xc := xc + 6.
	x@xc := x@xc + 1.
	#loop (x@xc != 0) ?
	[
		xc := xc -2.
		x@xc := x@xc + 3.
		#loop (x@xc != 0) ?
		[
			xc := xc + 2.
			#loop (x@xc != 0) ?
			[
				x@xc := x@xc - 1.
				xc := xc - 1.
			].
			xc := xc - 1.
			#loop (x@xc != 0) ?
			[
				xc := xc + 1.
			].
			xc := xc - 1.
			x@xc := x@xc - 1.
		].
		xc := xc + 2.
		#loop (x@xc != 0) ?
		[
			xc := xc + 1.
			x@xc := x@xc + 1.
			xc := xc + 1.
		].
		xc := xc - 1.
		#loop (x@xc != 0) ?
		[
			xc := xc - 1.
		].
		xc := xc + 1.
	].
	xc := xc + 1.
	#loop (x@xc != 0) ?
	[
		#loop (x@xc != 0) ?
		[
			x@xc := x@xc - 1.
			xc := xc + 4.
			x@xc := x@xc + 1.
			xc := xc -4.
		].
		xc := xc + 3.
		x@xc := x@xc + 3.
		xc := xc + 1.
		x@xc := x@xc - 1.
	].
	xc := xc - 1.
	#loop (x@xc != 0) ?
	[
		xc := xc -4.
	].
	xc := xc -8.
	x@xc := x@xc + 1.
	#loop (x@xc != 0) ?
	[
		x@xc := x@xc - 1.
		xc := xc + 12.
		#loop (x@xc != 0) ?
		[
			xc := xc - 1.
			x@xc := x@xc + 1.
			#loop (x@xc != 0) ?
			[
				x@xc := x@xc - 1.
				xc := xc + 4.
				x@xc := x@xc + 1.
				xc := xc -4.
			].
			xc := xc + 1.
			xc := xc + 4.
		].
		xc := xc -4.
		#loop (x@xc != 0) ?
		[
			xc := xc + 5.
			#loop (x@xc != 0) ?
			[
				xc := xc -4.
				x@xc := x@xc + 1.
				xc := xc + 4.
				x@xc := x@xc - 1.
			].
			xc := xc -5.
			x@xc := x@xc - 1.
			#loop (x@xc != 0) ?
			[
				xc := xc -2.
				x@xc := x@xc + 10.
				xc := xc + 2.
				x@xc := x@xc - 1.
			].
			xc := xc + 3.
			#loop (x@xc != 0) ?
			[
				xc := xc -2.
				#loop (x@xc != 0) ?
				[
					xc := xc - 1.
					x@xc := x@xc + 1.
					xc := xc -2.
					x@xc := x@xc + 1.
					xc := xc + 3.
					x@xc := x@xc - 1.
				].
				xc := xc - 1.
				#loop (x@xc != 0) ?
				[
					xc := xc + 1.
					x@xc := x@xc + 1.
					xc := xc - 1.
					x@xc := x@xc - 1.
				].
				xc := xc - 1.
				x@xc := x@xc + 2.
				xc := xc -2.
				x@xc := x@xc + 1.
				xc := xc + 6.
				x@xc := x@xc - 1.
			].
			xc := xc -2.
			#loop (x@xc != 0) ?
			[
				x@xc := x@xc - 1.
			].
			xc := xc -2.
			x@xc := x@xc - 1.
			xc := xc - 1.
			#loop (x@xc != 0) ?
			[
				x@xc := x@xc - 1.
				xc := xc + 2.
				x@xc := x@xc + 1.
				xc := xc - 1.
				x@xc := x@xc - 1.
				#loop (x@xc != 0) ?
				[
					xc := xc + 3.
				].
				xc := xc + 1.
				#loop (x@xc != 0) ?
				[
					#loop (x@xc != 0) ?
					[
						xc := xc - 1.
						x@xc := x@xc + 1.
						xc := xc + 1.
						x@xc := x@xc - 1.
					].
					xc := xc + 1.
					x@xc := x@xc + 1.
					xc := xc + 2.
				].
				xc := xc -5.
			].
			xc := xc + 1.
			#loop (x@xc != 0) ?
			[
				x@xc := x@xc - 1.
			].
			xc := xc + 1.
			x@xc := x@xc + 1.
			xc := xc -3.
			x@xc := x@xc - 1.
			#loop (x@xc != 0) ?
			[
				xc := xc + 2.
				x@xc := x@xc + 1.
				xc := xc -2.
				x@xc := x@xc - 1.
			].
			xc := xc - 1.
		].
		xc := xc -4.
		x@xc := x@xc + 1.
		xc := xc + 7.
		xc := xc + 1.
		#loop (x@xc != 0) ?
		[
			x@xc := x@xc - 1.
		].
		xc := xc + 1.
		#loop (x@xc != 0) ?
		[
			xc := xc -3.
			x@xc := x@xc + 1.
			xc := xc + 3.
			x@xc := x@xc - 1.
		].
		xc := xc -2.
		x@xc := x@xc + 10.
		xc := xc - 1.
		#loop (x@xc != 0) ?
		[
			x@xc := x@xc - 1.
			xc := xc + 2.
			x@xc := x@xc + 1.
			xc := xc - 1.
			x@xc := x@xc - 1.
			#loop (x@xc != 0) ?
			[
				xc := xc + 3.
			].
			xc := xc + 1.
			#loop (x@xc != 0) ?
			[
				#loop (x@xc != 0) ?
				[
					xc := xc - 1.
					x@xc := x@xc + 1.
					xc := xc + 1.
					x@xc := x@xc - 1.
				].
				xc := xc + 1.
				x@xc := x@xc + 1.
				xc := xc + 2.
			].
			xc := xc -5.
		].
		xc := xc + 1.
		#loop (x@xc != 0) ?
		[
			x@xc := x@xc - 1.
		].
		xc := xc + 1.
		x@xc := x@xc + 1.
		xc := xc + 1.
		#loop (x@xc != 0) ?
		[
			xc := xc -2.
			x@xc := x@xc + 1.
			xc := xc - 1.
			x@xc := x@xc + 1.
			xc := xc + 3.
			x@xc := x@xc - 1.
		].
		xc := xc -3.
		xc := xc - 1.
		x@xc := x@xc + 1.
		xc := xc - 1.
		x@xc := x@xc + 1.
		xc := xc + 2.
		#loop (x@xc != 0) ?
		[
			x@xc := x@xc - 1.
			#loop (x@xc != 0) ?
			[
				x@xc := x@xc - 1.
				#loop (x@xc != 0) ?
				[
					x@xc := x@xc - 1.
					#loop (x@xc != 0) ?
					[
						x@xc := x@xc - 1.
						#loop (x@xc != 0) ?
						[
							x@xc := x@xc - 1.
							#loop (x@xc != 0) ?
							[
								x@xc := x@xc - 1.
								#loop (x@xc != 0) ?
								[
									x@xc := x@xc - 1.
									#loop (x@xc != 0) ?
									[
										x@xc := x@xc - 1.
										#loop (x@xc != 0) ?
										[
											x@xc := x@xc - 1.
											xc := xc - 1.
											x@xc := x@xc - 1.
											xc := xc + 1.
											#loop (x@xc != 0) ?
											[
												x@xc := x@xc - 1.
												xc := xc - 1.
												x@xc := x@xc + 1.
												xc := xc - 1.
												x@xc := x@xc - 1.
												xc := xc + 2.
											].
										].
									].
								].
							].
						].
					].
				].
			].
		].
		xc := xc - 1.
		#loop (x@xc != 0) ?
		[
			x@xc := x@xc + 5.
			#loop (x@xc != 0) ?
			[
				xc := xc -3.
				x@xc := x@xc + 8.
				xc := xc - 1.
				x@xc := x@xc + 8.
				xc := xc + 4.
				x@xc := x@xc - 1.
			].
			xc := xc - 1.
			xc := xc -3.
			x@xc := x@xc + 1.
			xc := xc - 1.
			x@xc := x@xc - 1.
			xc := xc + 4.
			#loop (x@xc != 0) ?
			[
				xc := xc + 1.
				x@xc := x@xc + 1.
				xc := xc -3.
				x@xc := x@xc + 9.
				xc := xc - 1.
				x@xc := x@xc - 1.
				xc := xc + 3.
				x@xc := x@xc - 1.
			].
			xc := xc -5.
			#loop (x@xc != 0) ?
			[
				xc := xc + 2.
				x@xc := x@xc + 1.
				xc := xc -2.
				x@xc := x@xc - 1.
			].
			x@xc := x@xc + 1.
			xc := xc - 1.
			#loop (x@xc != 0) ?
			[
				x@xc := x@xc - 1.
				xc := xc + 1.
				x@xc := x@xc - 1.
				xc := xc - 1.
			].
			xc := xc + 1.
			#loop (x@xc != 0) ?
			[
				xc := xc + 2.
				console write:(charConvertor convert &int:(x@xc)).
				xc := xc -4.
				#loop (x@xc != 0) ?
				[
					x@xc := x@xc + 1.
					console write:(charConvertor convert &int:(x@xc)).
					#loop (x@xc != 0) ?
					[
						x@xc := x@xc - 1.
					].
				].
				xc := xc + 2.
				x@xc := x@xc - 1.
			].
			xc := xc + 1.
			#loop (x@xc != 0) ?
			[
				xc := xc + 2.
				console write:(charConvertor convert &int:(x@xc)).
				xc := xc -2.
				x@xc := x@xc - 1.
			].
			xc := xc + 1.
			#loop (x@xc != 0) ?
			[
				x@xc := x@xc - 1.
			].
			xc := xc + 1.
			#loop (x@xc != 0) ?
			[
				x@xc := x@xc - 1.
			].
			xc := xc + 3.
			#loop (x@xc != 0) ?
			[
				xc := xc + 2.
				#loop (x@xc != 0) ?
				[
					xc := xc -8.
					x@xc := x@xc + 1.
					xc := xc + 8.
					x@xc := x@xc - 1.
				].
				xc := xc -2.
				x@xc := x@xc - 1.
			].
		].
		xc := xc + 2.
		#loop (x@xc != 0) ?
		[
			x@xc := x@xc - 1.
		].
		xc := xc -3.
		#loop (x@xc != 0) ?
		[
			x@xc := x@xc - 1.
		].
		xc := xc -8.
	].
	x@xc := x@xc + 10.
	console write:(charConvertor convert &int:(x@xc)).

    #var anEnd := now.
 
    console writeLine.
     
    #var aDiff := anEnd - aStart.
    console writeLine:"Time elapsed in msec:":(aDiff milliseconds).    

    console readChar.
].
