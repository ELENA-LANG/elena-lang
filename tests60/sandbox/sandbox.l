static Reference<int> Seed = 0;

int getSeed()
{
   int newVal := Seed + 1;

   Seed.Value := newVal;

   ^ newVal;
}

public program()
{
   console.writeLine(getSeed());
   console.writeLine(getSeed());
   console.writeLine(getSeed());
}