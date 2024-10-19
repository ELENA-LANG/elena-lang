import system'calendar;
//import system'collections.
import extensions;

const int ARRAY_SIZE = 32768;

public program()
{
    int casas := 50;
    if(Program_arguments.Length > 1)
        { casas := Program_arguments[1].toInt() };
    
    var start := Now;
    
	//#var x := system'Array new &length:ARRAY_SIZE.
    auto x := new int[](ARRAY_SIZE);
	int xc := 0;
	int cnt := 0;
    for (int cnt := 0; cnt < ARRAY_SIZE; cnt += 1) { x[cnt] := 0 };
	xc := xc + 1;
	x[xc] := x[xc] + casas;
	while (x[xc] != 0)
	{
		xc := xc - 1;
		x[xc] := x[xc] + 1;
		xc := xc + 8;
		x[xc] := x[xc] + 10;
		xc := xc -7;
		x[xc] := x[xc] - 1;
	};
	xc := xc + 1;
	x[xc] := x[xc] + 5;
	while (x[xc] != 0)
	{
		xc := xc - 1;
		x[xc] := x[xc] + 9;
		xc := xc + 1;
		x[xc] := x[xc] - 1
	};
	x[xc] := x[xc] + 1;
	xc := xc + 6;
	x[xc] := x[xc] + 1;
	while (x[xc] != 0)
	{
		xc := xc - 2;
		x[xc] := x[xc] + 3;
		while (x[xc] != 0) 
		{		
            xc := xc + 2;
			while (x[xc] != 0)
			{
				x[xc] := x[xc] - 1;
				xc := xc - 1
			};
			xc := xc - 1;
			while (x[xc] != 0)
			{
				xc := xc + 1
			};
			xc := xc - 1;
			x[xc] := x[xc] - 1
		};
		xc := xc + 2;
		while (x[xc] != 0)
		{
			xc := xc + 1;
			x[xc] := x[xc] + 1;
			xc := xc + 1
		};
		xc := xc - 1;
		while (x[xc] != 0)
		{
			xc := xc - 1
		};
		xc := xc + 1
	};
	xc := xc + 1;
	while (x[xc] != 0)
	{
		while (x[xc] != 0)
		{
			x[xc] := x[xc] - 1;
			xc := xc + 4;
			x[xc] := x[xc] + 1;
			xc := xc -4
		};
		xc := xc + 3;
		x[xc] := x[xc] + 3;
		xc := xc + 1;
		x[xc] := x[xc] - 1
	};
	xc := xc - 1;
	while (x[xc] != 0)
	{
		xc := xc -4;
	};
	xc := xc -8;
	x[xc] := x[xc] + 1;
	while (x[xc] != 0)
	{
		x[xc] := x[xc] - 1;
		xc := xc + 12;
		while (x[xc] != 0)
		{
			xc := xc - 1;
			x[xc] := x[xc] + 1;
			while (x[xc] != 0)
			{
				x[xc] := x[xc] - 1;
				xc := xc + 4;
				x[xc] := x[xc] + 1;
				xc := xc -4
			};
			xc := xc + 1;
			xc := xc + 4
		};
		xc := xc -4;
		while (x[xc] != 0)
		{
			xc := xc + 5;
			while (x[xc] != 0)
			{
				xc := xc -4;
				x[xc] := x[xc] + 1;
				xc := xc + 4;
				x[xc] := x[xc] - 1
			};
			xc := xc -5;
			x[xc] := x[xc] - 1;
			while (x[xc] != 0)
			{
				xc := xc -2;
				x[xc] := x[xc] + 10;
				xc := xc + 2;
				x[xc] := x[xc] - 1
			};
			xc := xc + 3;
			while (x[xc] != 0)
			{
				xc := xc -2;
				while (x[xc] != 0)
				{
					xc := xc - 1;
					x[xc] := x[xc] + 1;
					xc := xc -2;
					x[xc] := x[xc] + 1;
					xc := xc + 3;
					x[xc] := x[xc] - 1;
				};
				xc := xc - 1;
				while (x[xc] != 0)
				{
					xc := xc + 1;
					x[xc] := x[xc] + 1;
					xc := xc - 1;
					x[xc] := x[xc] - 1
				};
				xc := xc - 1;
				x[xc] := x[xc] + 2;
				xc := xc -2;
				x[xc] := x[xc] + 1;
				xc := xc + 6;
				x[xc] := x[xc] - 1
			};
			xc := xc -2;
			while (x[xc] != 0)
			{
				x[xc] := x[xc] - 1
			};
			xc := xc -2;
			x[xc] := x[xc] - 1;
			xc := xc - 1;
			while (x[xc] != 0)
			{
				x[xc] := x[xc] - 1;
				xc := xc + 2;
				x[xc] := x[xc] + 1;
				xc := xc - 1;
				x[xc] := x[xc] - 1;
				while (x[xc] != 0)
				{
					xc := xc + 3
				};
				xc := xc + 1;
				while (x[xc] != 0)
				{
					while (x[xc] != 0)
					{
						xc := xc - 1;
						x[xc] := x[xc] + 1;
						xc := xc + 1;
						x[xc] := x[xc] - 1
					};
					xc := xc + 1;
					x[xc] := x[xc] + 1;
					xc := xc + 2
				};
				xc := xc -5
			};
			xc := xc + 1;
			while (x[xc] != 0)
			{
				x[xc] := x[xc] - 1
			};
			xc := xc + 1;
			x[xc] := x[xc] + 1;
			xc := xc -3;
			x[xc] := x[xc] - 1;
			while (x[xc] != 0)
			{
				xc := xc + 2;
				x[xc] := x[xc] + 1;
				xc := xc -2;
				x[xc] := x[xc] - 1
			};
			xc := xc - 1
		};
		xc := xc -4;
		x[xc] := x[xc] + 1;
		xc := xc + 7;
		xc := xc + 1;
		while (x[xc] != 0)
		{
			x[xc] := x[xc] - 1
		};
		xc := xc + 1;
		while (x[xc] != 0)
		{
			xc := xc -3;
			x[xc] := x[xc] + 1;
			xc := xc + 3;
			x[xc] := x[xc] - 1
		};
		xc := xc -2;
		x[xc] := x[xc] + 10;
		xc := xc - 1;
		while (x[xc] != 0)
		{
			x[xc] := x[xc] - 1;
			xc := xc + 2;
			x[xc] := x[xc] + 1;
			xc := xc - 1;
			x[xc] := x[xc] - 1;
			while (x[xc] != 0)
			{
				xc := xc + 3
			};
			xc := xc + 1;
			while (x[xc] != 0)
			{
				while (x[xc] != 0)
				{
					xc := xc - 1;
					x[xc] := x[xc] + 1;
					xc := xc + 1;
					x[xc] := x[xc] - 1
				};
				xc := xc + 1;
				x[xc] := x[xc] + 1;
				xc := xc + 2
			};
			xc := xc -5
		};
		xc := xc + 1;
		while (x[xc] != 0)
		{
			x[xc] := x[xc] - 1
		};
		xc := xc + 1;
		x[xc] := x[xc] + 1;
		xc := xc + 1;
		while (x[xc] != 0)
		{
			xc := xc -2;
			x[xc] := x[xc] + 1;
			xc := xc - 1;
			x[xc] := x[xc] + 1;
			xc := xc + 3;
			x[xc] := x[xc] - 1
		};
		xc := xc -3;
		xc := xc - 1;
		x[xc] := x[xc] + 1;
		xc := xc - 1;
		x[xc] := x[xc] + 1;
		xc := xc + 2;
		while (x[xc] != 0)
		{
			x[xc] := x[xc] - 1;
			while (x[xc] != 0)
			{
				x[xc] := x[xc] - 1;
				while (x[xc] != 0)
				{
					x[xc] := x[xc] - 1;
					while (x[xc] != 0)
					{
						x[xc] := x[xc] - 1;
						while (x[xc] != 0)
						{
							x[xc] := x[xc] - 1;
							while (x[xc] != 0)
							{
								x[xc] := x[xc] - 1;
								while (x[xc] != 0)
								{
									x[xc] := x[xc] - 1;
									while (x[xc] != 0)
									{
										x[xc] := x[xc] - 1;
										while (x[xc] != 0)
										{
											x[xc] := x[xc] - 1;
											xc := xc - 1;
											x[xc] := x[xc] - 1;
											xc := xc + 1;
											while (x[xc] != 0)
											{
												x[xc] := x[xc] - 1;
												xc := xc - 1;
												x[xc] := x[xc] + 1;
												xc := xc - 1;
												x[xc] := x[xc] - 1;
												xc := xc + 2
											}
										}
									}
								}
							}
						}
					}
				}
			}
		};
		xc := xc - 1;
		while (x[xc] != 0)
		{
			x[xc] := x[xc] + 5;
			while (x[xc] != 0)
			{
				xc := xc -3;
				x[xc] := x[xc] + 8;
				xc := xc - 1;
				x[xc] := x[xc] + 8;
				xc := xc + 4;
				x[xc] := x[xc] - 1
			};
			xc := xc - 1;
			xc := xc -3;
			x[xc] := x[xc] + 1;
			xc := xc - 1;
			x[xc] := x[xc] - 1;
			xc := xc + 4;
			while (x[xc] != 0)
			{
				xc := xc + 1;
				x[xc] := x[xc] + 1;
				xc := xc -3;
				x[xc] := x[xc] + 9;
				xc := xc - 1;
				x[xc] := x[xc] - 1;
				xc := xc + 3;
				x[xc] := x[xc] - 1
			};
			xc := xc -5;
			while (x[xc] != 0)
			{
				xc := xc + 2;
				x[xc] := x[xc] + 1;
				xc := xc -2;
				x[xc] := x[xc] - 1
			};
			x[xc] := x[xc] + 1;
			xc := xc - 1;
			while (x[xc] != 0)
			{
				x[xc] := x[xc] - 1;
				xc := xc + 1;
				x[xc] := x[xc] - 1;
				xc := xc - 1
			};
			xc := xc + 1;
			while (x[xc] != 0)
			{
				xc := xc + 2;
				Console.write(CharConvertor.convert(x[xc]));
    	        xc := xc -4;
				while (x[xc] != 0)
				{
					x[xc] := x[xc] + 1;
					Console.write(CharConvertor.convert(x[xc]));
					while (x[xc] != 0)
					{
						x[xc] := x[xc] - 1
					}
				};
				xc := xc + 2;
				x[xc] := x[xc] - 1
			};
			xc := xc + 1;
			while (x[xc] != 0)
			{
				xc := xc + 2;
				Console.write(CharConvertor.convert(x[xc]));
				xc := xc -2;
				x[xc] := x[xc] - 1
			};
			xc := xc + 1;
			while (x[xc] != 0)
			{
				x[xc] := x[xc] - 1
			};
			xc := xc + 1;
			while (x[xc] != 0)
			{
				x[xc] := x[xc] - 1
			};
			xc := xc + 3;
			while (x[xc] != 0)
			{
				xc := xc + 2;
				while (x[xc] != 0)
				{
					xc := xc -8;
					x[xc] := x[xc] + 1;
					xc := xc + 8;
					x[xc] := x[xc] - 1
				};
				xc := xc -2;
				x[xc] := x[xc] - 1
			}
		};
		xc := xc + 2;
		while (x[xc] != 0)
		{
			x[xc] := x[xc] - 1
		};
		xc := xc -3;
		while (x[xc] != 0)
		{
			x[xc] := x[xc] - 1
		};
		xc := xc -8
	};
	x[xc] := x[xc] + 10;
	Console.write(CharConvertor.convert(x[xc]));

    var end := Now;
 
    Console.writeLine();
     
    var diff := end - start;
    Console.printLine("Time elapsed in msec:",diff.Milliseconds);

    Console.readChar()
}
