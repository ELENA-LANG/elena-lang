import system'text;

// --- File ---

public sealed /*nonstructural*/ class File
{
    string thePath;
    
    constructor assign(string path)
    {
        thePath := path
    }

    string cast() = thePath;

    wide cast() = thePath;

//    T<literal> rprop path :: thePath.
//
//    T<literal> literal = thePath.
    
    TextReader textreader() = fileControl.newReader(thePath);
    
    TextWriter textwriter() = fileControl.writer(thePath);
    
    TextWriter logger() = fileControl.logger(thePath);
    
    bool Available
        = fileControl.isAvailable(thePath);
    
    string readContent()
    {
        auto output := new TextBuilder();
        
        self.read(output);
        
        ^ output.Value
    }
    
    wide readWideContent()
    {
        auto output := new TextBuilder();
        
        self.read(output);
        
        ^ output.Value
    }
    
    get int Length()
    {
        Stream stream := FileStream.openForRead(thePath);
        int    fileLen := stream.Length;        
        
        stream.close();
        
        ^ fileLen
    }

    read(TextBuilder output)
    {
        TextReader reader := fileControl.newReader(thePath);
        char       buffer[128];
        int        read := 0;
        while (reader.Available)
        {
            read := 128;
            reader.read(buffer, ref read);
            
            output.write(buffer, read)
        };
        
        reader.close()
    }
    
    saveContent(string content)
    {
        TextWriter writer := fileControl.writer(thePath);
        
        writer.write(content);
        
        writer.close();
    }    
    
    saveContent(wide content)
    {
        TextWriter writer := fileControl.writer(thePath);
        
        writer.write(content);
        
        writer.close()
    }    
    
    delete()
        = fileControl.delete(thePath);
}

// --- Directory ---

public sealed class Directory
{
    string thePath;
    
    constructor assign(string path)
    {
        thePath := path
    }

    //T<literal> literal = thePath.
    
    bool Available
        = directoryControl.isAvailable(thePath);    
        
    create()
    {
        directoryControl.create(thePath);
    }     
       
    delete()
    {
        directoryControl.delete(thePath)
    }
}