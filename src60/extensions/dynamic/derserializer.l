import extensions;
import system'dynamic;

public singleton Serializer<T>
{
   T fromJson(string json)
   {
      T target := new T();
 
      var dto := json.fromJson();

      auto properties := class T.__getProperties();
      foreach(var p; in properties) {
         var val := p.getPropertyValue(dto) ?? nil;
         if (val != nil) {
            p.setPropertyValue(target, val);
         }
      };

      ^ target
   }
}
