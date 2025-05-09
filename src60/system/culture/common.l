namespace culture
{
   public interface ILocale
   {
      abstract string toUppercase(string s);
      abstract wide toUppercase(wide s);

      abstract string toLowercase(string s);
      abstract wide toLowercase(wide s);
   }

   public extension stringCultureOp : String
   {
      string toUpper(ILocale locale)
         = locale.toUppercase(self);
   
      string toUpper()
         = currentLocale.toUppercase(self);

      string toLower(ILocale locale)
         = locale.toLowercase(self);
   
      string toLower()
         = currentLocale.toLowercase(self);
   }

   public extension wideStringCultureOp : WideString
   {
      wide toUpper(ILocale locale)
         = locale.toUppercase(self);
   
      wide toUpper()
         = currentLocale.toUppercase(self);

      wide toLower(ILocale locale)
         = locale.toLowercase(self);
   
      wide toLower()
         = currentLocale.toLowercase(self);
   }
}