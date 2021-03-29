namespace io
{

    import system'text;
    import system'collections;
    
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
    
        string toPrintable() = thePath;
        
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
    
//    // --- Directory ---
//    
//    public sealed class Directory
//    {
//        string thePath;
//        
//        constructor assign(string path)
//        {
//            thePath := path
//        }
//    
//        string Printable = thePath;
//        
//        bool Available
//            = directoryControl.isAvailable(thePath);    
//            
//        create()
//        {
//            directoryControl.create(thePath);
//        }     
//           
//        delete()
//        {
//            directoryControl.delete(thePath)
//        }
//        
//        string[] getFiles()
//            <= getFiles("*.*");
//        
//        string[] getFiles(string mask)
//        {
//            auto l := new List<string>();
//            SearchFiles(thePath, mask, l);
//    //        auto e := FileEnumerator.assign(thePath + "\" + mask);
//    //        while (e.next())
//    //        {
//    //            auto ddf := e.get();
//    //            if (ddf.isFile())
//    //            {
//    //                string s := thePath + "\" + ddf.FileName;
//    //                
//    //                l.append(s);
//    //            }            
//    //        };
//            
//            ^l.Value;
//        }
//        
//        string[] getDirectories()
//        {
//            auto l := new List<string>();
//            SearchDirectories(thePath, "*.*", l);
//    //        auto e := FileEnumerator.assign(thePath + "\*.*");
//    //        while (e.next())
//    //        {
//    //            auto ddf := e.get();
//    //            if (ddf.isDirectory())
//    //            {
//    //                string s := thePath + "\" + ddf.FileName;
//    //                
//    //                l.append(s);
//    //            }            
//    //        };
//            
//            ^l.Value;
//        }
//    }

}