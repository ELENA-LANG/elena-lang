public struct WGLGetProcAddress      : extern_winapi_func<pointer>(string name);
public struct GLGetStringPTR         : extern_winapi_func<pointer>(int name);

use intptr = UnsafePointer;
use double = LongNumber;
use ulong  = LongNumber;

// --- external function wrappers

public struct GLuint_int_int_int_int_int_uint_int_int_int_int_int_int_int_int : wgl_func(uint arg1,int arg2,int arg3,int arg4,int arg5,int arg6,uint arg7,int arg8,int arg9,int arg10,int arg11,int arg12,int arg13,int arg14,int arg15);

public struct GLuint_uint_int_int_int_int_int_int_int_int_int_int : wgl_func(uint arg1,uint arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12);
public struct GLuint_int_int_int_int_int_int_int_int_int_ptr      : wgl_func(uint arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, pointer arg11);

public struct GLuint_int_int_int_int_int_int_int_int              : wgl_func(uint arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9);
public struct GLuint_int_int_int_int_int_int_int_ptr              : wgl_func(uint arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, pointer arg9);

public struct GLuint_int_int_int_int_int_int_int                  : wgl_func(uint arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8);
public struct GLuint_int_ptr_ptr_ptr_ptr_ptr_ptr                  : wgl_func(uint arg1,int arg2,pointer arg3,pointer arg4,pointer arg5,pointer arg6,pointer arg7,pointer arg8);
public struct GLint_int_int_int_int_int_int_int                   : wgl_func(int arg1,int arg2,int arg3,int arg4,int arg5,int arg6,int arg7,int arg8);

public struct GLuint_int_int_int_int_int_int                      : wgl_func(uint arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7);
public struct GLuint_int_int_int_int_int_ptr                      : wgl_func(uint arg1, int arg2, int arg3, int arg4, int arg5, int arg6, pointer arg7);
public struct GLuint_int_ptr_int_int_int_int                      : wgl_func(uint arg1,int arg2,pointer arg3,int arg4,int arg5,int arg6,int arg7);
public struct GLuint_int_intptr_intptr_int_int_ptr                : wgl_func(uint arg1,int arg2,intptr arg3,intptr arg4,int arg5,int arg6,pointer arg7);
public struct GLint_int_intptr_intptr_int_int_ptr                 : wgl_func(int arg1,int arg2,intptr arg3,intptr arg4,int arg5,int arg6,pointer arg7);
public struct GLint_int_int_ptr_int_int_uint                      : wgl_func(int arg1,int arg2,int arg3,pointer arg4,int arg5,int arg6,uint arg7);
public struct GLint_int_int_int_int_int_int                       : wgl_func(int arg1,int arg2,int arg3,int arg4,int arg5,int arg6,int arg7);
public struct GLint_uint_uint_int_int_ptr_int                     : wgl_func(int arg1,uint arg2,uint arg3,int arg4,int arg5,pointer arg6,int arg7);

public struct GLuint_int_int_int_int_int                          : wgl_func(uint arg1, int arg2, int arg3, int arg4, int arg5, int arg6);
public struct GLint_int_uint_int_int_ptr                          : wgl_func(int arg1,int arg2,uint arg3,int arg4,int arg5,pointer arg6);
public struct GLint_int_int_int_ptr_int                           : wgl_func(int arg1,int arg2,int arg3,int arg4,pointer arg5,int arg6);
public struct GLuint_uint_uint_uint_uint_uint                     : wgl_func(uint arg1,uint arg2,uint arg3,uint arg4,uint arg5,uint arg6);
public struct GLint_int_int_ptr_int_uint                          : wgl_func(int arg1,int arg2,int arg3,pointer arg4,int arg5,uint arg6);
public struct GLuint_int_uint_int_ptr_ptr                         : wgl_func(uint arg1,int arg2,uint arg3,int arg4,pointer arg5,pointer arg6);
public struct GLint_int_int_int_int_int                           : wgl_func(int arg1,int arg2,int arg3,int arg4,int arg5,int arg6);
public struct GLint_ptr_int_ptr_int_ptr                           : wgl_func(int arg1,pointer arg2,int arg3,pointer arg4,int arg5,pointer arg6);
public struct GLint_int_int_ptr_int_int                           : wgl_func(int arg1,int arg2,int arg3,pointer arg4,int arg5,int arg6);

public struct GLuint_int_int_int_int                              : wgl_func(uint arg1, int arg2, int arg3, int arg4, int arg5);
public struct GLuint_int_uint_intptr_intptr                       : wgl_func(uint arg1, int arg2, uint arg3, intptr arg4, intptr arg5);
public struct GLuint_int_uint_int_int                             : wgl_func(uint arg1,int arg2,uint arg3,int arg4,int arg5);
public struct GLuint_int_int_int_ptr                              : wgl_func(uint arg1,int arg2,int arg3,int arg4,pointer arg5);
public struct GLuint_uint_intptr_intptr_intptr                    : wgl_func(uint arg1,uint arg2,intptr arg3,intptr arg4,intptr arg5);
public struct GLuint_uint_uint_intptr_intptr                      : wgl_func(uint arg1,uint arg2,uint arg3,intptr arg4,intptr arg5);
public struct GLint_int_int_int_ptr                               : wgl_func(int arg1,int arg2,int arg3,int arg4,pointer arg5);
public struct Gint_int_int_int_uint                               : wgl_func(int arg1,int arg2,int arg3,int arg4,uint arg5);
public struct GLint_ptr_int_ptr_int                               : wgl_func(int arg1,pointer arg2,int arg3,pointer arg4,int arg5);
public struct GLint_int_ptr_int_int                               : wgl_func(int arg1,int arg2,pointer arg3,int arg4,int arg5);
public struct GLuint_int_uint_int_ptr                             : wgl_func(uint arg1,int arg2,uint arg3,int arg4,pointer arg5);
public struct GLint_double_double_double_double                   : wgl_func(int arg1,double arg2,double arg3,double arg4,double arg5);
public struct GLptr_int_int_ptr_ptr                               : wgl_func(pointer arg1,int arg2,int arg3,pointer arg4,pointer arg5);
public struct GLint_int_int_ptr_int                               : wgl_func(int arg1,int arg2,int arg3,pointer arg4,int arg5);

public struct GLuint_int_uint_int                                 : wgl_func(uint arg1,int arg2,uint arg3,int arg4);
public struct GLuint_int_int_int                                  : wgl_func(uint arg1, int arg2, int arg3, int arg4);
public struct GLuint_int_int_ptr                                  : wgl_func(uint arg1, int arg2, int arg3, pointer arg4);
public struct GLuint_int_int_uint                                 : wgl_func(uint arg1,int arg2,int arg3,uint arg4);
public struct GLuint_intptr_intptr_ptr                            : wgl_func(uint arg1,intptr arg2,intptr arg3,pointer arg4);
public struct GLuint_intptr_ptr_int                               : wgl_func(uint arg1,intptr arg2,pointer arg3,int arg4);
public struct GLuint_int_uint_ptr                                 : wgl_func(uint arg1,int arg2,uint arg3,pointer arg4);
public struct GLint_intptr_ptr_int                                : wgl_func(int arg1,intptr arg2,pointer arg3,int arg4);
public struct GLint_int_ptr_ptr                                   : wgl_func(int arg1,int arg2,pointer arg3,pointer arg4);
public struct GLint_ptr_int_int                                   : wgl_func(int arg1,pointer arg2,int arg3,int arg4);
public struct GLint_uint_int_ptr                                  : wgl_func(int arg1,uint arg2,int arg3,pointer arg4);
public struct GLint_int_int_ptr                                   : wgl_func(int arg1,int arg2,int arg3,pointer arg4);
public struct GLint_double_double_double                          : wgl_func(int arg1,double arg2,double arg3,double arg4);
public struct GLuint_uint_uint_string                             : wgl_func(uint arg1,uint arg2,uint arg3,string arg4);
public struct GLint_int_uint_int                                  : wgl_func(int arg1,int arg2,uint arg3,int arg4);
public struct ptr__GLuint_intptr_intptr_int                       : wgl_func<pointer>(uint arg1,intptr arg2,intptr arg3,int arg4);

public struct GLuint_int_int                                      : wgl_func(uint arg1,int arg2,int arg3);
public struct GLuint_int_uint                                     : wgl_func(uint arg1, int arg2, uint arg3);
public struct GLint_int_ptr                                       : wgl_func(int arg1, int arg2, pointer arg3);
public struct GLuint_int_ptr                                      : wgl_func(uint arg1, int arg2, pointer arg3);
public struct GLuint_intptr_intptr                                : wgl_func(uint arg1,intptr arg2,intptr arg3);
public struct GLuint_uint_uint                                    : wgl_func(uint arg1,uint arg2,uint arg3);
public struct GLint_uint_uint                                     : wgl_func(int arg1,uint arg2,uint arg3);
public struct GLint_double_double                                 : wgl_func(int arg1,double arg2,double arg3);
public struct GLint_int_uint                                      : wgl_func(int arg1,int arg2,uint arg3);
public struct GLint_uint_ptr                                      : wgl_func(int arg1,uint arg2,pointer arg3);
public struct GLptr_int_long                                      : wgl_func(pointer arg1,int arg2,long arg3);
public struct uint__GLuint_int_string                             : wgl_func<uint>(uint arg1,int arg2,string arg3);
public struct int__GLuint_int_string                              : wgl_func<int>(uint arg1,int arg2,string arg3);
public struct int__GLptr_int_ulong                                : wgl_func<int>(pointer arg1,int arg2,ulong arg3);

public struct GLint_ptr                                           : wgl_func(int arg1, pointer arg2);
public struct GLuint_int                                          : wgl_func(uint arg1,int arg2);
public struct GLint_int                                           : wgl_func(int arg1,int arg2);
public struct GLuint_uint                                         : wgl_func(uint arg1,uint arg2);
public struct GLint_uint                                          : wgl_func(int arg1,uint arg2);
public struct GLptr_ptr                                           : wgl_func(pointer arg1,pointer arg2);
public struct GLint_double                                        : wgl_func(int arg1,double arg2);
public struct int__GLuint_string                                  : wgl_func<int>(uint arg1,string arg2);
public struct int__GLuint_int                                     : wgl_func<int>(uint arg1, int arg2);
public struct ptr__GLuint_int                                     : wgl_func<pointer>(uint arg1,int arg2);
public struct ptr__GLint_int                                      : wgl_func<pointer>(int arg1,int arg2);

public struct GLint__GLuint                                       : wgl_func<int>(uint arg1);
public struct GLFUNC_int__ptr                                     : wgl_func<int>(pointer arg1);
public struct GLFUNC_intptr                                       : wgl_func(intptr arg1);
public struct GLFUNC_int                                          : wgl_func(int arg1);
public struct GLFUNC_ptr                                          : wgl_func(pointer arg1);

public struct GLFUNC_VOID                                         : wgl_func();