// --- wgl_func ---

public template wgl_func<T>(params member)::
{
   __winapi_fn_ptr func;

   constructor load(WGLGetProcAddress get_proc, string func_name) 
   {
      func := get_proc(func_name);
   }

   constructor loadDirectly(handle lib_ptr, string func_name) 
   {
      func := extern KERNEL32.GetProcAddress(lib_ptr, func_name);
   }

   T function(
#for(member) 
      member
#endfor
   ) {
      T retVal := func(
#for(member) 
         $value member
#endfor      
      );
      
      ^ retVal
   }
   
   bool isUnassigned()
   {
      //^ func == default;
      ^ func == UnsafePointer.Default // !! temporally
   }
}


public template wgl_func(params member)::
{
   __winapi_fn_ptr func;

   constructor load(WGLGetProcAddress get_proc, string func_name) 
   {
      func := get_proc(func_name);
   }

   constructor loadDirectly(handle lib_ptr, string func_name) 
   {
      func := extern KERNEL32.GetProcAddress(lib_ptr, func_name);
   }

   function(
#for(member) 
      member
#endfor
   ) {
      func(
#for(member) 
         $value member
#endfor      
      );
   }
   
   bool isUnassigned()
   {
      //^ func == default;
      ^ func == UnsafePointer.Default // !! temporally
   }   
}
