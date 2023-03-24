namespace io
{
   public sealed class File
   {
      string _path;

      constructor assign(string path)
      {
         _path := path
      }

      TextReader textreader() = fileControl.newReader(_path);
   }
}