import system'io;
import system'text;
import extensions;
import extensions'text;

// --- ReaderEnumerator ---

public class ReaderEnumerator : Enumerator
{
   TextReader _reader;
   object     _line;

   constructor new(source)
   {
      _reader := source.textreader();
   }

   constructor new(Stream stream, Encoder encoder)
   {
      _reader := TextStreamReader.new(stream, encoder)
   }        
                        
   constructor load(string path)
   {
      _reader := TextStreamReader.new(path)
   }
    
   constructor load(string path, Encoder encoder)
   {
      _reader := TextStreamReader.new(path, encoder)
   }
    
   reset() 
   {
      NotSupportedException.raise()
   }
    
   bool next()
   {
      if(_reader.Available)
      {
         _line := _reader.readLine();
            
         ^ true
      };        
        
      _reader.close();
        
      ^ false
   }

   enumerable() = _reader;
            
   get Value() = _line;
}

// --- textExOp --

public extension textExOp
{
    forEachWord(Func1 f)
        = f.run(cast Enumerator(TokenEnumerator.new(self)));
}

// --- fileExOp ---

public extension fileExOp : File
{
    forEachLine(f)
        = f.run(ReaderEnumerator.new(self));
}

// --- printingLn ---

public symbol printingLn = (line){ Console.printLine(line) };

// --- func1Op ---

public extension func1Op : Func1
{
    asComparator()
        = new { bool equal(o) = self(o); };
}
