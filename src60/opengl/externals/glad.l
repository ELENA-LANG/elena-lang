// -----------------------------
import extensions;
// -----------------------------
import opengl'common;
import opengl'externals'primitives;

use GLFUNC_GLuint__VOID                                                       = GLFUNC_uint_VOID;

use GLFUNC_GLintptr                                                           = GLFUNC_intptr;
use GLFUNC_GLbitfield                                                         = GLFUNC_int;
use GLFUNC_GLfloat                                                            = GLFUNC_int;
use GLFUNC_GLdouble                                                           = GLFUNC_double;
use GLFUNC_GLenum                                                             = GLFUNC_int;
use GLFUNC_GLsync                                                             = GLFUNC_ptr;
use GLFUNC_GLuint                                                             = GLFUNC_uint;
use GLFUNC_GLint                                                              = GLFUNC_int;
use GLFUNC_GLsizei                                                            = GLFUNC_int;
use GLFUNC_ubyte                                                              = GLFUNC_ubyte;
use GLFUNC_GLbyte                                                             = GLFUNC_byte;
use GLFUNC_GLshort                                                            = GLFUNC_short;
use GLFUNC_GLboolean                                                          = GLFUNC_int;
use GLboolean__GLuint                                                         = GLFUNC_int__uint;
use GLFUNC_GLboolean__GLsync                                                  = GLFUNC_int__ptr;
use GLFUNC_GLboolean__GLuint                                                  = GLFUNC_int__uint;
use GLFUNC_GLboolean__GLenum                                                  = GLFUNC_int__int;
use GLFUNC_GLenum__GLenum                                                     = GLFUNC_int__int;
use GLFUNC_GLuint__GLenum                                                     = GLFUNC_uint__int;
use GLFUNC_GLint__GLenum                                                      = GLFUNC_int__int;

use GLsizei_ptr                                                               = GLint_ptr;
use GLuint_GLenum                                                             = GLuint_int;
use GLfloat_GLfloat                                                           = GLint_int;
use GLenum_GLenum                                                             = GLint_int;
use GLenum_GLuint                                                             = GLint_uint;
use GLenum_GLint                                                              = GLint_int;
use GLenum_ptr                                                                = GLint_ptr;
use ptr_ptr                                                                   = GLptr_ptr;
use GLint_GLdouble                                                            = GLint_double;
use GLuint_GLuint                                                             = GLuint_uint;
use GLuint_GLbitfield                                                         = GLuint_int;
use GLuint_ptr                                                                = GLuint_ptr;
use GLint_GLuint                                                              = GLint_uint;
use GLint_GLint                                                               = GLint_int;
use GLuint_GLint                                                              = GLuint_int;
use GLuint_string                                                             = GLuint_string;
use GLuint_GLshort                                                            = GLuint_short;
use GLuint_GLfloat                                                            = GLuint_int;
use GLint_GLfloat                                                             = GLint_int;
use GLuint_GLdouble                                                           = GLuint_double;
use GLshort_GLshort                                                           = GLshort_short;
use GLdouble_GLdouble                                                         = GLdouble_double;
use GLenum_GLfloat                                                            = GLint_int;
use GLenum_GLshort                                                            = GLint_short;
use GLenum_GLdouble                                                           = GLint_double;
use GLfloat_GLboolean                                                         = GLint_int;
use GLint_GLushort                                                            = GLint_ushort;
use GLuint_GLsizei                                                            = GLuint_int;
use GLenum__GLuint_GLenum                                                     = int__GLuint_int;
use pointer__GLuint_GLenum                                                    = ptr__GLuint_int;
use GLint__GLuint_string                                                      = int__GLuint_string;
use GLsync__GLenum_GLbitfield                                                 = ptr__GLint_int;
use ptr__GLenum_GLuint                                                        = ptr__GLint_uint;
use ptr__GLenum_GLenum                                                        = ptr__int_int;
use GLboolean__GLenum_GLuint                                                  = int__GLint_uint;

use GLenum_GLsizei_ptr                                                        = GLint_int_ptr;
use GLenum_GLuint_ptr                                                         = GLint_uint_ptr;
use GLuint_GLenum_ptr                                                         = GLuint_int_ptr;
use GLuint_GLsizei_ptr                                                        = GLuint_int_ptr;
use GLuint_GLenum_GLuint                                                      = GLuint_int_uint;
use GLuint_GLenum_GLint                                                       = GLuint_int_int;
use GLuint_GLintptr_GLsizeiptr                                                = GLuint_intptr_intptr;
use GLuint_GLuint_GLuint                                                      = GLuint_uint_uint;
use GLenum_GLuint_GLuint                                                      = GLint_uint_uint;
use GLenum_GLint_ptr                                                          = GLint_int_ptr;
use GLuint_GLint_ptr                                                          = GLuint_int_ptr;
use GLint_GLsizei_ptr                                                         = GLint_int_ptr;
use GLint_GLdouble_GLdouble                                                   = GLint_double_double;
use GLenum_GLenum_ptr                                                         = GLint_int_ptr;
use GLuint_GLenum_GLenum                                                      = GLuint_int_int;
use GLenum_GLenum_GLuint                                                      = GLint_int_uint;
use GLint_GLint_GLint                                                         = GLint_int_int;
use GLuint_GLenum_GLfloat                                                     = GLuint_int_int;
use GLsync_GLbitfield_GLuint64                                                = GLptr_int_long;
use GLenum_GLintptr_GLsizeiptr                                                = GLint_intptr_intptr;
use GLint_GLuint_GLuint                                                       = GLint_uint_uint;
use GLuint_GLuint_string                                                      = GLuint_uint_string;
use GLuint_GLint_GLint                                                        = GLuint_int_int;
use GLuint_GLshort_GLshort                                                    = GLuint_short_short;
use GLuint_GLfloat_GLfloat                                                    = GLuint_int_int;
use GLuint_GLdouble_GLdouble                                                  = GLuint_double_double;
use GLint_GLfloat_GLfloat                                                     = GLint_int_int;
use GLenum_GLfloat_GLfloat                                                    = GLint_int_int;
use GLshort_GLshort_GLshort                                                   = GLshort_short_short;
use GLfloat_GLfloat_GLfloat                                                   = GLint_int_int;
use GLdouble_GLdouble_GLdouble                                                = GLdouble_double_double;
use GLushort_GLushort_GLushort                                                = GLushort_ushort_ushort;
use GLubyte_GLubyte_GLubyte                                                   = GLubyte_ubyte_ubyte;
use GLbyte_GLbyte_GLbyte                                                      = GLbyte_byte_byte;
use GLenum_GLdouble_GLdouble                                                  = GLint_double_double;
use GLenum_GLint_GLint                                                        = GLint_int_int;
use GLenum_GLshort_GLshort                                                    = GLint_short_short;
use GLsizei_ptr_ptr                                                           = GLint_ptr_ptr;
use GLenum_GLint_GLsizei                                                      = GLint_int_int;
use GLsizei_GLenum_ptr                                                        = GLint_int_ptr;
use GLenum_GLenum_GLfloat                                                     = GLint_int_int;
use GLenum_GLenum_GLint                                                       = GLint_int_int;
use GLenum_GLenum_GLdouble                                                    = GLint_int_double;
use GLint_GLint_GLfloat                                                       = GLint_int_int;
use GLuint__GLuint_GLenum_string                                              = uint__GLuint_int_string;
use GLint__GLuint_GLenum_string                                               = int__GLuint_int_string;
use GLenum__GLsync_GLbitfield_GLuint64                                        = int__GLptr_int_ulong;
use GLint__GLuint_ptr                                                         = int__GLuint_ptr;

use GLuint_GLenum_GLenum_ptr                                                  = GLuint_int_int_ptr;
use GLuint_GLenum_GLint_ptr                                                   = GLuint_int_int_ptr;
use GLuint_GLsizei_GLenum_GLsizei                                             = GLuint_int_int_int;
use GLuint_GLenum_GLsizei_GLsizei                                             = GLuint_int_int_int;
use GLenum_GLint_GLenum_ptr                                                   = GLint_int_int_ptr;
use GLuint_GLenum_GLuint_GLint                                                = GLuint_int_uint_int;
use GLuint_GLenum_GLenum_GLuint                                               = GLuint_int_int_uint;
use GLuint_GLintptr_GLsizeiptr_ptr                                            = GLuint_intptr_intptr_ptr;
use GLenum_GLintptr_GLsizeiptr_ptr                                            = GLint_intptr_intptr_ptr;
use GLuint_GLsizeiptr_ptr_GLenum                                              = GLuint_intptr_ptr_int;
use GLuint_GLsizeiptr_ptr_GLbitfield                                          = GLuint_intptr_ptr_int;
use GLuint_GLenum_GLuint_ptr                                                  = GLuint_int_uint_ptr;
use GLenum_GLsizeiptr_ptr_GLbitfield                                          = GLint_intptr_ptr_int;
use GLenum_GLenum_ptr_ptr                                                     = GLint_int_ptr_ptr;
use GLenum_ptr_GLsizei_GLsizei                                                = GLint_ptr_int_int;
use GLenum_GLuint_GLenum_ptr                                                  = GLint_uint_int_ptr;
use GLint_GLsizei_GLboolean_ptr                                               = GLint_int_int_ptr;
use GLenum_GLenum_GLenum_ptr                                                  = GLint_int_int_ptr;
use GLint_GLdouble_GLdouble_GLdouble                                          = GLint_double_double_double;
use GLuint_GLenum_GLboolean_GLuint                                            = GLuint_int_int_uint;
use GLuint_GLenum_GLboolean_ptr                                               = GLuint_int_int_ptr;
use GLuint_GLuint_GLuint_string                                               = GLuint_uint_uint_string;
use GLenum_GLenum_GLuint_GLint                                                = GLint_int_uint_int;
use GLenum_GLenum_GLenum_GLuint                                               = GLint_int_int_uint;
use GLuint_GLuint_GLenum_ptr                                                  = GLuint_uint_int_ptr;
use GLuint_GLsizei_ptr_ptr                                                    = GLuint_int_ptr_ptr;
use GLenum_ptr_ptr_GLsizei                                                    = GLint_ptr_ptr_int;
use GLenum_GLint_GLsizei_GLsizei                                              = GLint_int_int_int;
use GLenum_GLenum_GLsizei_GLsizei                                             = GLint_int_int_int;
use GLenum_GLint_GLfloat_GLint                                                = GLint_int_int_int;
use GLint_GLint_GLint_GLint                                                   = GLint_int_int_int;
use GLint_GLuint_GLuint_GLuint                                                = GLint_uint_uint_uint;
use GLuint_GLuint_GLuint_GLuint                                               = GLuint_uint_uint_uint;
use GLuint_GLint_GLint_GLint                                                  = GLuint_int_int_int;
use GLuint_GLsizei_ptr_GLenum                                                 = GLuint_int_ptr_int;
use GLuint_GLshort_GLshort_GLshort                                            = GLuint_short_short_short;
use GLuint_GLfloat_GLfloat_GLfloat                                            = GLuint_int_int_int;
use GLuint_GLdouble_GLdouble_GLdouble                                         = GLuint_double_double_double;
use GLint_GLfloat_GLfloat_GLfloat                                             = GLint_int_int_int;
use GLenum_GLenum_GLint_GLuint                                                = GLint_int_int_uint;
use GLenum_GLenum_GLenum_GLenum                                               = GLint_int_int_int;
use GLenum_GLsizeiptr_ptr_GLenum                                              = GLint_intptr_ptr_int;
use GLfloat_GLfloat_GLfloat_GLfloat                                           = GLfloat_float_float_float;
use GLint_GLenum_GLsizei_ptr                                                  = GLint_int_int_ptr;
use GLenum_GLdouble_GLdouble_GLdouble                                         = GLint_double_double_double;
use GLenum_GLfloat_GLfloat_GLfloat                                            = GLint_int_int_int;
use GLenum_GLint_GLint_GLint                                                  = GLint_int_int_int;
use GLenum_GLshort_GLshort_GLshort                                            = GLint_short_short_short;
use GLenum_GLsizei_GLenum_ptr                                                 = GLint_int_int_ptr;
use GLdouble_GLdouble_GLdouble_GLdouble                                       = GLdouble_double_double_double;
use GLshort_GLshort_GLshort_GLshort                                           = GLshort_short_short_short;
use GLubyte_GLubyte_GLubyte_GLubyte                                           = GLubyte_ubyte_ubyte_ubyte;
use GLbyte_GLbyte_GLbyte_GLbyte                                               = GLbyte_byte_byte_byte;
use GLushort_GLushort_GLushort_GLushort                                       = GLushort_ushort_ushort_ushort;
use GLint_GLint_GLsizei_GLsizei                                               = GLint_int_int_int;
use ptr__GLuint_GLintptr_GLsizeiptr_GLbitfield                                = ptr__GLuint_intptr_intptr_int;
use GLboolean__GLsizei_ptr_ptr                                                = int__int_ptr_ptr;

use GLuint_GLsizei_GLenum_GLsizei_GLsizei                                     = GLuint_int_int_int_int;
use GLuint_GLenum_GLint_GLfloat_GLint                                         = GLuint_int_int_int_int;
use GLuint_GLenum_GLuint_GLint_GLint                                          = GLuint_int_uint_int_int;
use GLuint_GLenum_GLenum_GLenum_ptr                                           = GLuint_int_int_int_ptr;
use GLuint_GLint_GLenum_GLenum_ptr                                            = GLuint_int_int_int_ptr;
use GLuint_GLuint_GLsizei_ptr_ptr                                             = GLuint_uint_int_ptr_ptr;
use GLuint_GLuint_GLintptr_GLintptr_GLsizeiptr                                = GLuint_uint_intptr_intptr_intptr;
use GLuint_GLuint_GLuint_GLintptr_GLsizeiptr                                  = GLuint_uint_uint_intptr_intptr;
use GLenum_GLenum_GLenum_GLenum_ptr                                           = GLint_int_int_int_ptr;
use GLenum_GLint_GLsizei_GLsizei_GLuint                                       = Gint_int_int_int_uint;
use GLenum_GLint_GLenum_GLenum_ptr                                            = GLint_int_int_int_ptr;
use GLsizei_ptr_GLenum_ptr_GLsizei                                            = GLint_ptr_int_ptr_int;
use GLenum_GLenum_ptr_GLsizei_GLsizei                                         = GLint_int_ptr_int_int;
use GLint_GLint_GLint_GLint_GLint                                             = GLint_GLint_GLint_GLint;
use GLuint_GLenum_GLenum_GLenum_GLenum                                        = GLuint_int_int_int_int;
use GLuint_GLenum_GLuint_GLenum_ptr                                           = GLuint_int_uint_int_ptr;
use GLsync_GLenum_GLsizei_ptr_ptr                                             = GLptr_int_int_ptr_ptr;
use GLuint_GLsizei_ptr_GLenum_ptr                                             = GLuint_int_ptr_int_ptr;
use GLint_GLdouble_GLdouble_GLdouble_GLdouble                                 = GLint_double_double_double_double;
use GLuint_GLdouble_GLdouble_GLdouble_GLdouble                                = GLuint_double_double_double_double;
use GLuint_GLubyte_GLubyte_GLubyte_GLubyte                                    = GLuint_ubyte_ubyte_ubyte_ubyte;
use GLuint_GLfloat_GLfloat_GLfloat_GLfloat                                    = GLuint_int_int_int_int;
use GLint_GLfloat_GLfloat_GLfloat_GLfloat                                     = GLint_int_int_int_int;
use GLenum_GLsizei_GLenum_ptr_GLint                                           = GLint_int_int_ptr_int;
use GLenum_GLenum_GLintptr_GLintptr_GLsizeiptr                                = GLint_int_intptr_intptr_intptr;
use GLenum_GLsizei_GLenum_ptr_GLsizei                                         = GLint_int_int_ptr_int;
use GLenum_GLenum_GLuint_GLint_GLint                                          = GLint_int_uint_int_int;
use GLenum_GLsizei_GLenum_GLsizei_GLsizei                                     = GLint_int_int_int_int;
use GLenum_GLenum_GLenum_GLuint_GLint                                         = GLint_int_int_uint_int;
use GLint_GLuint_GLuint_GLuint_GLuint                                         = GLint_uint_uint_uint_uint;
use GLuint_GLuint_GLuint_GLuint_GLuint                                        = GLuint_uint_uint_uint_uint;
use GLuint_GLint_GLint_GLint_GLint                                            = GLuint_int_int_int_int;
use GLuint_GLint_GLenum_GLsizei_ptr                                           = GLuint_int_int_int_ptr;
use GLenum_GLuint_GLuint_GLintptr_GLsizeiptr                                  = GLint_uint_uint_intptr_intptr;
use GLuint_GLboolean_GLboolean_GLboolean_GLboolean                            = GLuint_int_int_int_int;
use GLenum_GLshort_GLshort_GLshort_GLshort                                    = GLint_short_short_short_short;
use GLuint_GLshort_GLshort_GLshort_GLshort                                    = GLuint_short_short_short_short;
use GLenum_ptr_GLenum_ptr_GLsizei                                             = GLint_ptr_int_ptr_int;
use GLenum_GLint_GLint_GLint_GLint                                            = GLint_int_int_int_int;
use GLenum_GLdouble_GLdouble_GLdouble_GLdouble                                = GLint_double_double_double_double;
use GLenum_GLfloat_GLfloat_GLfloat_GLfloat                                    = GLint_int_int_int_int;
use GLsizei_GLsizei_GLenum_GLenum_ptr                                         = GLint_int_int_int_ptr;
use GLint_GLint_GLsizei_GLsizei_GLenum                                        = GLint_int_int_int_int;
use ptr__GLenum_GLintptr_GLsizeiptr_GLbitfield                                = ptr__GLint_intptr_intptr_int;

use GLuint_GLint_GLint_GLint_GLint_GLsizei                                    = GLuint_int_int_int_int_int;
use GLuint_GLsizei_GLenum_GLsizei_GLsizei_GLboolean                           = GLuint_int_int_int_int_int;
use GLuint_GLenum_GLuint_GLintptr_GLsizeiptr                                  = GLuint_int_uint_intptr_intptr;
use GLuint_GLsizei_GLenum_GLsizei_GLsizei_GLsizei                             = GLuint_int_int_int_int_int;
use GLenum_GLenum_GLuint_GLenum_GLsizei_ptr                                   = GLint_int_uint_int_int_ptr;
use GLenum_GLenum_GLenum_GLsizei_ptr_GLboolean                                = GLint_int_int_int_ptr_int;
use GLuint_GLuint_GLuint_GLuint_GLuint_GLuint                                 = GLuint_uint_uint_uint_uint_uint;
use GLenum_GLsizei_GLenum_ptr_GLsizei_GLuint                                  = GLint_int_int_ptr_int_uint;
use GLuint_GLenum_GLuint_GLsizei_ptr_ptr                                      = GLuint_int_uint_int_ptr_ptr;
use GLenum_GLsizei_GLenum_GLsizei_GLsizei_GLboolean                           = GLint_int_int_int_int_int;
use GLenum_ptr_GLenum_ptr_GLsizei_ptr                                         = GLint_ptr_int_ptr_int_ptr;
use GLenum_GLsizei_GLenum_ptr_GLsizei_GLint                                   = GLint_int_int_ptr_int_int;
use GLenum_GLenum_GLenum_GLuint_GLint_GLint                                   = GLint_int_int_uint_int_int;
use GLuint_GLint_GLenum_GLboolean_GLsizei_ptr                                 = GLuint_int_int_int_int_ptr;
use GLenum_GLint_GLint_GLsizei_GLenum_GLsizei_ptr                             = GLint_int_int_int_int_int_ptr;
use GLenum_GLuint_GLuint_GLsizei_GLenum_ptr                                   = GLint_uint_uint_int_int_ptr;
use GLenum_GLint_GLint_GLint_GLint_GLsizei                                    = GLint_int_int_int_int_int;
use GLdouble_GLdouble_GLdouble_GLdouble_GLdouble_GLdouble                     = GLdouble_double_double_double_double_double;
use GLint_GLdouble_GLdouble_GLint_GLdouble_GLdouble                           = GLint_double_double_int_double_double;
use GLint_GLfloat_GLfloat_GLint_GLfloat_GLfloat                               = GLint_int_int_int_int_int;
use GLenum_GLdouble_GLdouble_GLint_GLint_ptr                                  = GLint_double_double_int_int_ptr;
use GLenum_GLfloat_GLfloat_GLint_GLint_ptr                                    = GLint_int_int_int_int_ptr;

use GLint_GLint_GLsizei_GLsizei_GLenum_GLenum_ptr                             = GLint_int_int_int_int_int_ptr;
use GLuint_GLint_GLint_GLsizei_GLenum_GLsizei_ptr                             = GLuint_int_int_int_int_int_ptr;
use GLuint_GLint_GLint_GLsizei_GLenum_GLenum_ptr                              = GLuint_int_int_int_int_int_ptr;
use GLuint_GLuint_GLsizei_ptr_ptr_ptr_ptr                                     = GLuint_uint_int_ptr_ptr_ptr_ptr;
use GLuint_GLsizei_GLenum_GLsizei_GLsizei_GLsizei_GLboolean                   = GLuint_int_int_int_int_int_int;
use GLuint_GLenum_GLintptr_GLsizeiptr_GLenum_GLenum_ptr                       = GLuint_int_intptr_intptr_int_int_ptr;
use GLuint_GLsizei_ptr_GLint_GLint_GLsizei_GLsizei                            = GLuint_int_ptr_int_int_int_int;
use GLenum_GLenum_GLintptr_GLsizeiptr_GLenum_GLenum_ptr                       = GLint_int_intptr_intptr_int_int_ptr;
use GLenum_GLsizei_GLenum_ptr_GLsizei_GLint_GLuint                            = GLint_int_int_ptr_int_int_uint;
use GLenum_GLsizei_GLenum_GLsizei_GLsizei_GLsizei_GLboolean                   = GLint_int_int_int_int_int_int;
use GLenum_GLuint_GLuint_GLsizei_GLenum_ptr_GLint                             = GLint_uint_uint_int_int_ptr_int;
use GLenum_GLint_GLenum_GLsizei_GLsizei_GLint_GLsizei_ptr                     = GLint_int_int_int_int_int_int_ptr;
use GLenum_GLint_GLenum_GLsizei_GLint_GLsizei_ptr                             = GLint_int_int_int_int_int_ptr;
use GLenum_GLint_GLenum_GLint_GLint_GLsizei_GLint                             = GLint_int_int_int_int_int_int;
use GLenum_GLint_GLint_GLsizei_GLenum_GLenum_ptr                              = GLint_int_int_int_int_int_ptr;
use GLsizei_GLsizei_GLfloat_GLfloat_GLfloat_GLfloat_ptr                       = GLint_int_int_int_int_int_ptr;

use GLuint_GLint_GLint_GLint_GLint_GLint_GLsizei_GLsizei                      = GLuint_int_int_int_int_int_int_int;
use GLuint_GLsizei_ptr_ptr_ptr_ptr_ptr_ptr                                    = GLuint_int_ptr_ptr_ptr_ptr_ptr_ptr;
use GLfloat_GLfloat_GLfloat_GLfloat_GLfloat_GLfloat_GLfloat_GLfloat           = GLint_int_int_int_int_int_int_int;
use GLenum_GLint_GLenum_GLint_GLint_GLsizei_GLsizei_GLint                     = GLint_int_int_int_int_int_int_int;
use GLenum_GLint_GLint_GLint_GLint_GLint_GLsizei_GLsizei                      = GLint_int_int_int_int_int_int_int;

use GLuint_GLint_GLint_GLint_GLint_GLint_GLint_GLsizei_GLsizei                = GLuint_int_int_int_int_int_int_int_int;
use GLuint_GLint_GLint_GLint_GLsizei_GLsizei_GLenum_GLsizei_ptr               = GLuint_int_int_int_int_int_int_int_ptr;
use GLuint_GLint_GLint_GLint_GLsizei_GLsizei_GLenum_GLenum_ptr                = GLuint_int_int_int_int_int_int_int_ptr;
use GLuint_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei_GLenum_GLenum_ptr  = GLuint_int_int_int_int_int_int_int_int_int_ptr;
use GLuint_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei_GLenum_GLsizei_ptr = GLuint_int_int_int_int_int_int_int_int_int_ptr;
use GLenum_GLint_GLenum_GLsizei_GLsizei_GLsizei_GLint_GLsizei_ptr             = GLint_int_int_int_int_int_int_int_ptr;
use GLenum_GLint_GLint_GLint_GLsizei_GLsizei_GLenum_GLsizei_ptr               = GLint_int_int_int_int_int_int_int_ptr;
use GLenum_GLint_GLint_GLint_GLint_GLint_GLint_GLsizei_GLsizei                = GLint_int_int_int_int_int_int_int_int;
use GLenum_GLint_GLint_GLint_GLsizei_GLsizei_GLenum_GLenum_ptr                = GLint_int_int_int_int_int_int_int_ptr;

use GLint_GLint_GLint_GLint_GLint_GLint_GLint_GLint_GLbitfield_GLenum         = GLint_int_int_int_int_int_int_int_int_int;
use GLenum_GLint_GLint_GLsizei_GLsizei_GLsizei_GLint_GLenum_GLenum_ptr        = GLint_int_int_int_int_int_int_int_int_ptr;
use GLenum_GLfloat_GLfloat_GLint_GLint_GLfloat_GLfloat_GLint_GLint_ptr        = GLint_int_int_int_int_int_int_int_int_ptr;
use GLenum_GLdouble_GLdouble_GLint_GLint_GLdouble_GLdouble_GLint_GLint_ptr    = GLint_double_double_int_int_double_double_int_int_ptr;

use GLenum_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei_GLenum_GLsizei_ptr = GLint_int_int_int_int_int_int_int_int_int_ptr;
use GLenum_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei_GLenum_GLenum_ptr  = GLint_int_int_int_int_int_int_int_int_int_ptr;

use GLuint_GLuint_GLint_GLint_GLint_GLint_GLint_GLint_GLint_GLint_GLbitfield_GLenum = GLuint_uint_int_int_int_int_int_int_int_int_int_int;

use GLuint_GLenum_GLint_GLint_GLint_GLint_GLuint_GLenum_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei = GLuint_int_int_int_int_int_uint_int_int_int_int_int_int_int_int;

//use GLenum = system'IntNumber;
//use GLint = system'IntNumber;
//use GLfloat = /*system'FloatNumber*/system'IntNumber;
//use GLsizei = system'IntNumber;
//use GLboolean = system'IntNumber;
//use GLdouble = system'RealNumber;
//use GLuint = system'UIntNumber;
//use GLuint64 = system'LongNumber;
//use GLbyte = system'SByteNumber;
//use GLshort = system'ShortNumber;
//use GLushort = system'UShortNumber;
//use GLubyte = system'ByteNumber;
//use GLbitfield = system'IntNumber;
//use GLsizeiptr = system'LongNumber;
//use GLintptr = system'UnsafePointer;
//use GLsync = system'UnsafePointer;
//
//internal struct GLGetStringPTR         : extern_winapi_func<pointer>(int name);

use GLCullFacePTR          = GLFUNC_int;
use GLFrontFacePTR         = GLFUNC_int;
use GLHintPTR              = GLint_int;
use GLLineWidthPTR         = GLFUNC_GLfloat;
use GLPointSizePTR         = GLFUNC_GLfloat;
use GLPolygonModePTR       = GLint_int;
use GLScissorPTR           = GLint_int_int_int;
use GLTexParameterfPTR     = GLint_GLint_GLfloat;
use GLTexParameterfvPTR    = GLint_int_ptr;
use GLTexParameteriPTR     = GLint_int_int;
use GLTexParameterivPTR    = GLint_int_ptr;
use GLTexImage1DPTR        = GLint_int_int_int_int_int_int_ptr;
use GLTexImage2DPTR        = GLint_int_int_int_int_int_int_int_ptr;
use GLDrawBufferPTR        = GLFUNC_int;
use GLClearPTR             = GLFUNC_int;
use GLClearColorPTR        = GLfloat_GLfloat_GLfloat_GLfloat;
use GLClearStencilPTR      = GLFUNC_double;
use GLClearDepthPTR        = GLFUNC_uint;
use GLStencilMaskPTR       = GLbyte_byte_byte_byte;
use GLColorMaskPTR         = GLFUNC_GLbyte;
use GLDepthMaskPTR         = GLFUNC_int;
use GLDisablePTR           = GLFUNC_int;
use GLEnablePTR            = GLFUNC_int;

use GLFinishPTR            = GLFUNC_VOID;
use GLFlushPTR             = GLFUNC_VOID;
use GLBlendFuncPTR         = GLFUNC_int;
use GLLogicOpPTR           = GLint_int_uint;
use GLStencilFuncPTR       = GLint_int_int;
use GLStencilOpPTR         = GLenum_GLint;
use GLDepthFuncPTR         = GLint_GLfloat;
use GLPixelStorefPTR       = GLenum_GLfloat;
use GLPixelStoreiPTR       = GLenum_GLint;
use GLReadBufferPTR        = GLFUNC_GLenum;
use GLReadPixelsPTR        = GLint_GLint_GLsizei_GLsizei_GLenum_GLenum_ptr;
use GLGetBooleanvPTR       = GLenum_ptr;
use GLGetDoublevPTR        = GLenum_ptr;
use GLGetErrorPTR          = GLFUNC_VOID;
use GLGetFloatvPTR         = GLenum_ptr;
use GLGetIntegervPTR       = GLenum_ptr;
//internal struct GLGetStringPTR         : extern_winapi_func<pointer>(GLenum name);
use GLGetTexImagePTR       = GLenum_GLint_GLenum_GLenum_ptr;
use GLGetTexParameterfvPTR        = GLenum_GLenum_ptr;
use GLGetTexParameterivPTR        = GLenum_GLenum_ptr;
use GLGetTexLevelParameterfvPTR   = GLenum_GLint_GLenum_ptr;
use GLGetTexLevelParameterivPTR   = GLenum_GLint_GLenum_ptr;
use GLIsEnabledPTR         = GLFUNC_GLboolean__GLenum;
use GLDepthRangePTR        = GLdouble_GLdouble;
use GLViewportPTR          = GLint_GLint_GLsizei_GLsizei;
use GLNewListPTR           = GLuint_GLenum;
use GLEndListPTR           = GLFUNC_VOID;
use GLCallListPTR          = GLFUNC_GLuint;
use GLCallListsPTR         = GLsizei_GLenum_ptr;
use GLDeleteListsPTR       = GLuint_GLsizei;
use GLGenListsPTR          = GLFUNC_GLsizei;
use GLListBasePTR          = GLFUNC_GLuint;
use GLBeginPTR             = GLFUNC_GLenum;
use GLBitmapPTR            = GLsizei_GLsizei_GLfloat_GLfloat_GLfloat_GLfloat_ptr;
use GLColor3bPTR           = GLbyte_GLbyte_GLbyte;
use GLColor3bvPTR          = GLFUNC_ptr;
use GLColor3dPTR           = GLdouble_GLdouble_GLdouble;
use GLColor3dvPTR          = GLFUNC_ptr;
use GLColor3fPTR           = GLfloat_GLfloat_GLfloat;
use GLColor3fvPTR          = GLFUNC_ptr;
use GLColor3iPTR           = GLint_GLint_GLint;
use GLColor3ivPTR          = GLFUNC_ptr;
use GLColor3sPTR           = GLshort_GLshort_GLshort;
use GLColor3svPTR        = GLFUNC_ptr;
use GLColor3ubPTR        = GLubyte_GLubyte_GLubyte;
use GLColor3ubvPTR       = GLFUNC_ptr;
use GLColor3uiPTR        = GLuint_GLuint_GLuint;
use GLColor3uivPTR       = GLFUNC_ptr;
use GLColor3usPTR        = GLushort_GLushort_GLushort;
use GLColor3usvPTR       = GLFUNC_ptr;
use GLColor4bPTR           = GLbyte_GLbyte_GLbyte_GLbyte;
use GLColor4bvPTR          = GLFUNC_ptr;
use GLColor4dPTR           = GLdouble_GLdouble_GLdouble_GLdouble;
use GLColor4dvPTR          = GLFUNC_ptr;
use GLColor4fPTR           = GLfloat_GLfloat_GLfloat_GLfloat;
use GLColor4fvPTR          = GLFUNC_ptr;
use GLColor4iPTR           = GLint_GLint_GLint_GLint;
use GLColor4ivPTR          = GLFUNC_ptr;
use GLColor4sPTR           = GLshort_GLshort_GLshort_GLshort;
use GLColor4svPTR        = GLFUNC_ptr;
use GLColor4ubPTR        = GLubyte_GLubyte_GLubyte_GLubyte;
use GLColor4ubvPTR       = GLFUNC_ptr;
use GLColor4uiPTR        = GLuint_GLuint_GLuint_GLuint;
use GLColor4uivPTR       = GLFUNC_ptr;
use GLColor4usPTR        = GLushort_GLushort_GLushort_GLushort;
use GLColor4usvPTR       = GLFUNC_ptr;
use GLEdgeFlagPTR        = GLFUNC_GLboolean;
use GLEdgeFlagvPTR       = GLFUNC_ptr;
use GLEndPTR             = GLFUNC_VOID;
use GLIndexdPTR          = GLFUNC_GLdouble;
use GLIndexdvPTR         = GLFUNC_ptr;
use GLIndexfPTR          = GLFUNC_GLfloat;
use GLIndexfvPTR         = GLFUNC_ptr;
use GLIndexiPTR          = GLFUNC_GLint;
use GLIndexivPTR         = GLFUNC_ptr;
use GLIndexsPTR          = GLFUNC_GLshort;
use GLIndexsvPTR         = GLFUNC_ptr;
use GLNormal3bPTR        = GLbyte_GLbyte_GLbyte;
use GLNormal3bvPTR       = GLFUNC_ptr;
use GLNormal3dPTR        = GLdouble_GLdouble_GLdouble;
use GLNormal3dvPTR       = GLFUNC_ptr;
use GLNormal3fPTR        = GLfloat_GLfloat_GLfloat;
use GLNormal3fvPTR       = GLFUNC_ptr;
use GLNormal3iPTR        = GLint_GLint_GLint;
use GLNormal3ivPTR       = GLFUNC_ptr;
use GLNormal3sPTR        = GLshort_GLshort_GLshort;
use GLNormal3svPTR       = GLFUNC_ptr;
use GLRasterPos2dPTR     = GLdouble_GLdouble;
use GLRasterPos2dvPTR    = GLFUNC_ptr;
use GLRasterPos2fPTR     = GLfloat_GLfloat;
use GLRasterPos2fvPTR    = GLFUNC_ptr;
use GLRasterPos2iPTR     = GLint_GLint;
use GLRasterPos2ivPTR    = GLFUNC_ptr;
use GLRasterPos2sPTR     = GLshort_GLshort;
use GLRasterPos2svPTR    = GLFUNC_ptr;
use GLRasterPos3dPTR     = GLdouble_GLdouble_GLdouble;
use GLRasterPos3dvPTR    = GLFUNC_ptr;
use GLRasterPos3fPTR     = GLfloat_GLfloat_GLfloat;
use GLRasterPos3fvPTR    = GLFUNC_ptr;
use GLRasterPos3iPTR     = GLint_GLint_GLint;
use GLRasterPos3ivPTR    = GLFUNC_ptr;
use GLRasterPos3sPTR     = GLshort_GLshort_GLshort;
use GLRasterPos3svPTR    = GLFUNC_ptr;
use GLRasterPos4dPTR     = GLdouble_GLdouble_GLdouble_GLdouble;
use GLRasterPos4dvPTR    = GLFUNC_ptr;
use GLRasterPos4fPTR     = GLfloat_GLfloat_GLfloat_GLfloat;
use GLRasterPos4fvPTR    = GLFUNC_ptr;
use GLRasterPos4iPTR     = GLint_GLint_GLint_GLint;
use GLRasterPos4ivPTR    = GLFUNC_ptr;
use GLRasterPos4sPTR     = GLshort_GLshort_GLshort_GLshort;
use GLRasterPos4svPTR    = GLFUNC_ptr;
use GLRectdPTR           = GLdouble_GLdouble_GLdouble_GLdouble;
use GLRectdvPTR          = GLFUNC_ptr_ptr;
use GLRectfPTR           = GLfloat_GLfloat_GLfloat_GLfloat;
use GLRectfvPTR          = GLFUNC_ptr_ptr;
use GLRectiPTR           = GLint_GLint_GLint_GLint;
use GLRectivPTR          = GLFUNC_ptr_ptr;
use GLRectsPTR           = GLshort_GLshort_GLshort_GLshort;
use GLRectsvPTR          = GLFUNC_ptr_ptr;
use GLTexCoord1dPTR      = GLFUNC_GLdouble;
use GLTexCoord1dvPTR     = GLFUNC_ptr;
use GLTexCoord1fPTR      = GLFUNC_GLfloat;
use GLTexCoord1fvPTR     = GLFUNC_ptr;
use GLTexCoord1iPTR      = GLFUNC_GLint;
use GLTexCoord1ivPTR     = GLFUNC_ptr;
use GLTexCoord1sPTR      = GLFUNC_GLshort;
use GLTexCoord1svPTR     = GLFUNC_ptr;
use GLTexCoord2dPTR      = GLdouble_GLdouble;
use GLTexCoord2dvPTR     = GLFUNC_ptr;
use GLTexCoord2fPTR      = GLfloat_GLfloat;
use GLTexCoord2fvPTR     = GLFUNC_ptr;
use GLTexCoord2iPTR      = GLint_GLint;
use GLTexCoord2ivPTR     = GLFUNC_ptr;
use GLTexCoord2sPTR      = GLshort_GLshort;
use GLTexCoord2svPTR     = GLFUNC_ptr;
use GLTexCoord3dPTR      = GLdouble_GLdouble_GLdouble;
use GLTexCoord3dvPTR     = GLFUNC_ptr;
use GLTexCoord3fPTR      = GLfloat_GLfloat_GLfloat;
use GLTexCoord3fvPTR     = GLFUNC_ptr;
use GLTexCoord3iPTR      = GLint_GLint_GLint;
use GLTexCoord3ivPTR     = GLFUNC_ptr;
use GLTexCoord3sPTR      = GLshort_GLshort_GLshort;
use GLTexCoord3svPTR     = GLFUNC_ptr;
use GLTexCoord4dPTR      = GLdouble_GLdouble_GLdouble_GLdouble;
use GLTexCoord4dvPTR     = GLFUNC_ptr;
use GLTexCoord4fPTR      = GLfloat_GLfloat_GLfloat_GLfloat;
use GLTexCoord4fvPTR     = GLFUNC_ptr;
use GLTexCoord4iPTR      = GLint_GLint_GLint_GLint;
use GLTexCoord4ivPTR     = GLFUNC_ptr;
use GLTexCoord4sPTR      = GLshort_GLshort_GLshort_GLshort;
use GLTexCoord4svPTR     = GLFUNC_ptr;
use GLVertex2dPTR      = GLdouble_GLdouble;
use GLVertex2dvPTR     = GLFUNC_ptr;
use GLVertex2fPTR      = GLfloat_GLfloat;
use GLVertex2fvPTR     = GLFUNC_ptr;
use GLVertex2iPTR      = GLint_GLint;
use GLVertex2ivPTR     = GLFUNC_ptr;
use GLVertex2sPTR      = GLshort_GLshort;
use GLVertex2svPTR     = GLFUNC_ptr;
use GLVertex3dPTR      = GLdouble_GLdouble_GLdouble;
use GLVertex3dvPTR     = GLFUNC_ptr;
use GLVertex3fPTR      = GLfloat_GLfloat_GLfloat;
use GLVertex3fvPTR     = GLFUNC_ptr;
use GLVertex3iPTR      = GLint_GLint_GLint;
use GLVertex3ivPTR     = GLFUNC_ptr;
use GLVertex3sPTR      = GLshort_GLshort_GLshort;
use GLVertex3svPTR     = GLFUNC_ptr;
use GLVertex4dPTR      = GLdouble_GLdouble_GLdouble_GLdouble;
use GLVertex4dvPTR     = GLFUNC_ptr;
use GLVertex4fPTR      = GLfloat_GLfloat_GLfloat_GLfloat;
use GLVertex4fvPTR     = GLFUNC_ptr;
use GLVertex4iPTR      = GLint_GLint_GLint_GLint;
use GLVertex4ivPTR     = GLFUNC_ptr;
use GLVertex4sPTR      = GLshort_GLshort_GLshort_GLshort;
use GLVertex4svPTR     = GLFUNC_ptr;
use GLClipPlanePTR     = GLenum_ptr;
use GLColorMaterialPTR = GLenum_GLenum;
use GLFogfPTR          = GLenum_GLfloat;
use GLFogfvPTR         = GLenum_ptr;
use GLFogiPTR          = GLenum_GLint;
use GLFogivPTR         = GLenum_ptr;
use GLLightfPTR        = GLenum_GLenum_GLfloat;
use GLLightfvPTR       = GLenum_GLenum_ptr;
use GLLightiPTR        = GLenum_GLenum_GLint;
use GLLightivPTR       = GLenum_GLenum_ptr;
use GLLightModelfPTR   = GLenum_GLfloat;
use GLLightModelfvPTR  = GLenum_ptr;
use GLLightModeliPTR   = GLenum_GLint;
use GLLightModelivPTR  = GLenum_ptr;
use GLLineStipplePTR   = GLint_GLushort;
use GLMaterialfPTR     = GLenum_GLenum_GLfloat;
use GLMaterialfvPTR    = GLenum_GLenum_ptr;
use GLMaterialiPTR     = GLenum_GLenum_GLint;
use GLMaterialivPTR    = GLenum_GLenum_ptr;
use GLPolygonStipplePTR = GLFUNC_ptr;
use GLShadeModelPTR   = GLFUNC_GLenum;
use GLTexEnvfPTR     = GLenum_GLenum_GLfloat;
use GLTexEnvfvPTR    = GLenum_GLenum_ptr;
use GLTexEnviPTR     = GLenum_GLenum_GLint;
use GLTexEnvivPTR    = GLenum_GLenum_ptr;
use GLTexGendPTR     = GLenum_GLenum_GLdouble;
use GLTexGendvPTR    = GLenum_GLenum_ptr;
use GLTexGenfPTR     = GLenum_GLenum_GLfloat;
use GLTexGenfvPTR    = GLenum_GLenum_ptr;
use GLTexGeniPTR     = GLenum_GLenum_GLint;
use GLTexGenivPTR    = GLenum_GLenum_ptr;
use GLFeedbackBufferPTR  = GLsizei_GLenum_ptr;
use GLSelectBufferPTR    = GLsizei_ptr;
use GLRenderModePTR      = GLFUNC_GLint__GLenum;
use GLInitNamesPTR       = GLFUNC_VOID;
use GLLoadNamePTR        = GLFUNC_GLuint;
use GLPassThroughPTR     = GLFUNC_GLfloat;
use GLPopNamePTR         = GLFUNC_VOID;
use GLPushNamePTR        = GLFUNC_GLuint;
use GLClearAccumPTR      = GLfloat_GLfloat_GLfloat_GLfloat;
use GLClearIndexPTR      = GLFUNC_GLfloat;
use GLIndexMaskPTR       = GLFUNC_GLuint;
use GLAccumPTR           = GLenum_GLfloat;
use GLPopAttribPTR       = GLFUNC_VOID;
use GLPushAttribPTR      = GLFUNC_GLbitfield;
use GLMap1dPTR           = GLenum_GLdouble_GLdouble_GLint_GLint_ptr;
use GLMap1fPTR           = GLenum_GLfloat_GLfloat_GLint_GLint_ptr;
use GLMap2dPTR           = GLenum_GLdouble_GLdouble_GLint_GLint_GLdouble_GLdouble_GLint_GLint_ptr;
use GLMap2fPTR           = GLenum_GLfloat_GLfloat_GLint_GLint_GLfloat_GLfloat_GLint_GLint_ptr;
use GLMapGrid1dPTR       = GLint_GLdouble_GLdouble;
use GLMapGrid1fPTR       = GLint_GLfloat_GLfloat;
use GLMapGrid2dPTR       = GLint_GLdouble_GLdouble_GLint_GLdouble_GLdouble;
use GLMapGrid2fPTR       = GLint_GLfloat_GLfloat_GLint_GLfloat_GLfloat;
use GLEvalCoord1dPTR      = GLFUNC_GLdouble;
use GLEvalCoord1dvPTR     = GLFUNC_ptr;
use GLEvalCoord1fPTR      = GLFUNC_GLfloat;
use GLEvalCoord1fvPTR     = GLFUNC_ptr;
use GLEvalCoord2dPTR      = GLdouble_GLdouble;
use GLEvalCoord2dvPTR     = GLFUNC_ptr;
use GLEvalCoord2fPTR      = GLfloat_GLfloat;
use GLEvalCoord2fvPTR     = GLFUNC_ptr;
use GLEvalMesh1PTR        = GLenum_GLint_GLint;
use GLEvalPoint1PTR      = GLFUNC_GLint;
use GLEvalMesh2PTR       = GLenum_GLint_GLint_GLint_GLint;
use GLEvalPoint2PTR      = GLint_GLint;
use GLAlphaFuncPTR       = GLenum_GLfloat;
use GLPixelZoomPTR       = GLfloat_GLfloat;
use GLPixelTransferfPTR  = GLenum_GLfloat;
use GLPixelTransferiPTR  = GLenum_GLint;
use GLPixelMapfvPTR      = GLenum_GLsizei_ptr;
use GLPixelMapuivPTR     = GLenum_GLsizei_ptr;
use GLPixelMapusvPTR     = GLenum_GLsizei_ptr;
use GLCopyPixelsPTR      = GLint_GLint_GLsizei_GLsizei_GLenum;
use GLDrawPixelsPTR      = GLsizei_GLsizei_GLenum_GLenum_ptr;
use GLGetClipPlanePTR    = GLenum_ptr;
use GLGetLightfvPTR      = GLenum_GLenum_ptr;
use GLGetLightivPTR      = GLenum_GLenum_ptr;
use GLGetMapdvPTR        = GLenum_GLenum_ptr;
use GLGetMapfvPTR        = GLenum_GLenum_ptr;
use GLGetMapivPTR        = GLenum_GLenum_ptr;
use GLGetMaterialfvPTR   = GLenum_GLenum_ptr;
use GLGetMaterialivPTR   = GLenum_GLenum_ptr;
use GLGetPixelMapfvPTR   = GLenum_GLenum_ptr;
use GLGetPixelMapuivPTR  = GLenum_GLenum_ptr;
use GLGetPixelMapusvPTR  = GLenum_GLenum_ptr;
use GLGetPolygonStipplePTR = GLFUNC_ptr;
use GLGetTexEnvfvPTR     = GLenum_GLenum_ptr;
use GLGetTexEnvivPTR     = GLenum_GLenum_ptr;
use GLGetTexGendvPTR     = GLenum_GLenum_ptr;
use GLGetTexGenfvPTR     = GLenum_GLenum_ptr;
use GLGetTexGenivPTR     = GLenum_GLenum_ptr;
use GLIsListPTR          = GLFUNC_GLboolean__GLuint;
use GLFrustumPTR         = GLdouble_GLdouble_GLdouble_GLdouble_GLdouble_GLdouble;
use GLLoadIdentityPTR    = GLFUNC_VOID;
use GLLoadMatrixfPTR     = GLFUNC_ptr;
use GLLoadMatrixdPTR     = GLFUNC_ptr;
use GLMatrixModePTR      = GLFUNC_GLenum;
use GLMultMatrixfPTR     = GLFUNC_ptr;
use GLMultMatrixdPTR     = GLFUNC_ptr;

use GLOrthoPTR           = GLdouble_GLdouble_GLdouble_GLdouble_GLdouble_GLdouble;
use GLPopMatrixPTR       = GLFUNC_VOID;
use GLPushMatrixPTR      = GLFUNC_VOID;
use GLRotatedPTR         = GLdouble_GLdouble_GLdouble_GLdouble;
use GLRotatefPTR         = GLfloat_GLfloat_GLfloat_GLfloat;
use GLScaledPTR          = GLdouble_GLdouble_GLdouble;
use GLScalefPTR          = GLfloat_GLfloat_GLfloat;
use GLTranslatedPTR      = GLdouble_GLdouble_GLdouble;
use GLTranslatefPTR      = GLfloat_GLfloat_GLfloat;
// 1.1
use GLDrawArraysPTR     = GLenum_GLint_GLsizei;
use GLDrawElementsPTR   = GLenum_GLsizei_GLenum_ptr;
use GLGetPointervPTR    = GLenum_ptr;
use GLPolygonOffsetPTR  = GLfloat_GLfloat;
use GLCopyTexImage1DPTR = GLenum_GLint_GLenum_GLint_GLint_GLsizei_GLint;
use GLCopyTexImage2DPTR = GLenum_GLint_GLenum_GLint_GLint_GLsizei_GLsizei_GLint;
use GLCopyTexSubImage1DPTR = GLenum_GLint_GLint_GLint_GLint_GLsizei;
use GLCopyTexSubImage2DPTR = GLenum_GLint_GLint_GLint_GLint_GLint_GLsizei_GLsizei;
use GLTexSubImage1DPTR  = GLenum_GLint_GLint_GLsizei_GLenum_GLenum_ptr;
use GLTexSubImage2DPTR  = GLenum_GLint_GLint_GLint_GLsizei_GLsizei_GLenum_GLenum_ptr;
use GLBindTexturePTR     = GLenum_GLuint;
use GLDeleteTexturesPTR  = GLsizei_ptr;
use GLGenTexturesPTR     = GLsizei_ptr;
use GLIsTexturePTR       = GLFUNC_GLboolean__GLuint;
use GLArrayElementPTR    = GLFUNC_GLint;
use GLColorPointerPTR    = GLint_GLenum_GLsizei_ptr;
use GLDisableClientStatePTR = GLFUNC_GLenum;
use GLEdgeFlagPointerPTR = GLsizei_ptr;
use GLEnableClientStatePTR  = GLFUNC_GLenum;
use GLIndexPointerPTR    = GLenum_GLsizei_ptr;
use GLInterleavedArraysPTR  = GLenum_GLsizei_ptr;
use GLNormalPointerPTR      = GLenum_GLsizei_ptr;
use GLTexCoordPointerPTR    = GLint_GLenum_GLsizei_ptr;
use GLVertexPointerPTR      = GLint_GLenum_GLsizei_ptr;
use GLAreTexturesResidentPTR = GLboolean__GLsizei_ptr_ptr;
use GLPrioritizeTexturesPTR = GLsizei_ptr_ptr;
use GLIndexubPTR            = GLFUNC_ubyte;
use GLIndexubvPTR           = GLFUNC_ptr;
use GLPopClientAttribPTR    = GLFUNC_VOID;
use GLPushClientAttribPTR   = GLFUNC_GLbitfield;
// 1.2
use GLDrawRangeElementsPTR  = GLenum_GLuint_GLuint_GLsizei_GLenum_ptr;
use GLTexImage3DPTR         = GLenum_GLint_GLint_GLsizei_GLsizei_GLsizei_GLint_GLenum_GLenum_ptr;
use GLTexSubImage3DPTR      = GLenum_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei_GLenum_GLenum_ptr;
use GLCopyTexSubImage3DPTR  = GLenum_GLint_GLint_GLint_GLint_GLint_GLint_GLsizei_GLsizei;
// 1.3
use GLActiveTexturePTR         = GLFUNC_GLenum;
use GLSampleCoveragePTR        = GLfloat_GLboolean;
use GLCompressedTexImage3DPTR  = GLenum_GLint_GLenum_GLsizei_GLsizei_GLsizei_GLint_GLsizei_ptr;
use GLCompressedTexImage2DPTR  = GLenum_GLint_GLenum_GLsizei_GLsizei_GLint_GLsizei_ptr;
use GLCompressedTexImage1DPTR  = GLenum_GLint_GLenum_GLsizei_GLint_GLsizei_ptr;
use GLCompressedTexSubImage3DPTR = GLenum_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei_GLenum_GLsizei_ptr;
use GLCompressedTexSubImage2DPTR = GLenum_GLint_GLint_GLint_GLsizei_GLsizei_GLenum_GLsizei_ptr;
use GLCompressedTexSubImage1DPTR = GLenum_GLint_GLint_GLsizei_GLenum_GLsizei_ptr;
use GLGetCompressedTexImagePTR = GLenum_GLint_ptr;
use GLClientActiveTexturePTR   = GLFUNC_GLenum;
use GLMultiTexCoord1dPTR      = GLenum_GLdouble;
use GLMultiTexCoord1dvPTR     = GLenum_ptr;
use GLMultiTexCoord1fPTR      = GLenum_GLfloat;
use GLMultiTexCoord1fvPTR     = GLenum_ptr;
use struct GLMultiTexCoord1iPTR      = GLenum_GLint;
use GLMultiTexCoord1ivPTR     = GLenum_ptr;
use GLMultiTexCoord1sPTR      = GLenum_GLshort;
use GLMultiTexCoord1svPTR     = GLenum_ptr;
use GLMultiTexCoord2dPTR      = GLenum_GLdouble_GLdouble;
use GLMultiTexCoord2dvPTR     = GLenum_ptr;
use GLMultiTexCoord2fPTR      = GLenum_GLfloat_GLfloat;
use GLMultiTexCoord2fvPTR     = GLenum_ptr;
use GLMultiTexCoord2iPTR      = GLenum_GLint_GLint;
use GLMultiTexCoord2ivPTR     = GLenum_ptr;
use GLMultiTexCoord2sPTR      = GLenum_GLshort_GLshort;
use GLMultiTexCoord2svPTR     = GLenum_ptr;
use GLMultiTexCoord3dPTR      = GLenum_GLdouble_GLdouble_GLdouble;
use GLMultiTexCoord3dvPTR     = GLenum_ptr;
use GLMultiTexCoord3fPTR      = GLenum_GLfloat_GLfloat_GLfloat;
use GLMultiTexCoord3fvPTR     = GLenum_ptr;
use GLMultiTexCoord3iPTR      = GLenum_GLint_GLint_GLint;
use GLMultiTexCoord3ivPTR     = GLenum_ptr;
use GLMultiTexCoord3sPTR      = GLenum_GLshort_GLshort_GLshort;
use GLMultiTexCoord3svPTR     = GLenum_ptr;
use GLMultiTexCoord4dPTR      = GLenum_GLdouble_GLdouble_GLdouble_GLdouble;
use GLMultiTexCoord4dvPTR     = GLenum_ptr;
use GLMultiTexCoord4fPTR      = GLenum_GLfloat_GLfloat_GLfloat_GLfloat;
use GLMultiTexCoord4fvPTR     = GLenum_ptr;
use GLMultiTexCoord4iPTR      = GLenum_GLint_GLint_GLint_GLint;
use GLMultiTexCoord4ivPTR     = GLenum_ptr;
use GLMultiTexCoord4sPTR      = GLenum_GLshort_GLshort_GLshort_GLshort;
use GLMultiTexCoord4svPTR     = GLenum_ptr;
use GLLoadTransposeMatrixfPTR = GLFUNC_ptr;
use GLLoadTransposeMatrixdPTR = GLFUNC_ptr;
use GLMultTransposeMatrixfPTR = GLFUNC_ptr;
use GLMultTransposeMatrixdPTR = GLFUNC_ptr;

// 1.4
use GLBlendFuncSeparatePTR     = GLenum_GLenum_GLenum_GLenum;
use GLMultiDrawArraysPTR       = GLenum_ptr_ptr_GLsizei;
use GLMultiDrawElementsPTR     = GLenum_ptr_GLenum_ptr_GLsizei;
use GLPointParameterfPTR       = GLenum_GLfloat;
use GLPointParameterfvPTR      = GLenum_ptr;
use GLPointParameteriPTR       = GLenum_GLint;
use GLPointParameterivPTR      = GLenum_ptr;
use GLFogCoordfPTR             = GLFUNC_GLfloat;
use GLFogCoordfvPTR            = GLFUNC_ptr;
use GLFogCoorddPTR             = GLFUNC_GLdouble;
use GLFogCoorddvPTR            = GLFUNC_ptr;
use GLFogCoordPointerPTR       = GLenum_GLsizei_ptr;
use GLSecondaryColor3bPTR      = GLbyte_GLbyte_GLbyte;
use GLSecondaryColor3bvPTR     = GLFUNC_ptr;
use GLSecondaryColor3dPTR      = GLdouble_GLdouble_GLdouble;
use GLSecondaryColor3dvPTR     = GLFUNC_ptr;
use GLSecondaryColor3fPTR      = GLfloat_GLfloat_GLfloat;
use GLSecondaryColor3fvPTR     = GLFUNC_ptr;
use GLSecondaryColor3iPTR      = GLint_GLint_GLint;
use GLSecondaryColor3ivPTR     = GLFUNC_ptr;
use GLSecondaryColor3sPTR      = GLshort_GLshort_GLshort;
use GLSecondaryColor3svPTR     = GLFUNC_ptr;
use GLSecondaryColor3ubPTR     = GLubyte_GLubyte_GLubyte;
use GLSecondaryColor3ubvPTR    = GLFUNC_ptr;
use GLSecondaryColor3uiPTR     = GLuint_GLuint_GLuint;
use GLSecondaryColor3uivPTR    = GLFUNC_ptr;
use GLSecondaryColor3usPTR     = GLushort_GLushort_GLushort;
use GLSecondaryColor3usvPTR    = GLFUNC_ptr;
use GLSecondaryColorPointerPTR = GLint_GLenum_GLsizei_ptr;
use GLWindowPos2dPTR           = GLdouble_GLdouble;
use GLWindowPos2dvPTR          = GLFUNC_ptr;
use GLWindowPos2fPTR           = GLfloat_GLfloat;
use GLWindowPos2fvPTR          = GLFUNC_ptr;
use GLWindowPos2iPTR           = GLint_GLint;
use GLWindowPos2ivPTR          = GLFUNC_ptr;
use GLWindowPos2sPTR           = GLshort_GLshort;
use GLWindowPos2svPTR          = GLFUNC_ptr;
use GLWindowPos3dPTR           = GLdouble_GLdouble_GLdouble;
use GLWindowPos3dvPTR          = GLFUNC_ptr;
use GLWindowPos3fPTR           = GLfloat_GLfloat_GLfloat;
use GLWindowPos3fvPTR          = GLFUNC_ptr;
use GLWindowPos3iPTR           = GLint_GLint_GLint;
use GLWindowPos3ivPTR          = GLFUNC_ptr;
use GLWindowPos3sPTR           = GLshort_GLshort_GLshort;
use GLWindowPos3svPTR          = GLFUNC_ptr;
use GLBlendColorPTR            = GLfloat_GLfloat_GLfloat_GLfloat;
use GLBlendEquationPTR         = GLFUNC_GLenum;
// 1.5
use GLGenQueriesPTR            = GLsizei_ptr;
use GLDeleteQueriesPTR         = GLsizei_ptr;
use GLIsQueryPTR               = GLFUNC_GLboolean__GLuint;
use GLBeginQueryPTR            = GLenum_GLuint;
use GLEndQueryPTR              = GLFUNC_GLenum;
use GLGetQueryivPTR            = GLenum_GLenum_ptr;
use GLGetQueryObjectivPTR      = GLuint_GLenum_ptr;
use GLGetQueryObjectuivPTR     = GLuint_GLenum_ptr;
use GLBindBufferPTR            = GLenum_GLuint;
use GLDeleteBuffersPTR         = GLsizei_ptr;
use GLGenBuffersPTR            = GLsizei_ptr;
use GLIsBufferPTR              = GLFUNC_GLboolean__GLuint;
use GLBufferDataPTR            = GLenum_GLsizeiptr_ptr_GLenum;
use GLBufferSubDataPTR         = GLenum_GLintptr_GLsizeiptr_ptr;
use GLGetBufferSubDataPTR      = GLenum_GLintptr_GLsizeiptr_ptr;
use GLMapBufferPTR             = ptr__GLenum_GLenum;
use GLUnmapBufferPTR           = GLFUNC_GLboolean__GLenum;
use GLGetBufferParameterivPTR  = GLenum_GLenum_ptr;
use GLGetBufferPointervPTR     = GLenum_GLenum_ptr;
// 2.0
use GLBlendEquationSeparatePTR = GLenum_GLenum;
use GLDrawBuffersPTR           = GLsizei_ptr;
use GLStencilOpSeparatePTR     = GLenum_GLenum_GLenum_GLenum;
use GLStencilFuncSeparatePTR   = GLenum_GLenum_GLint_GLuint;
use GLStencilMaskSeparatePTR   = GLenum_GLuint;
use GLAttachShaderPTR          = GLuint_GLuint;
use GLBindAttribLocationPTR    = GLuint_GLuint_string;
use GLCompileShaderPTR         = GLFUNC_GLuint;
use GLCreateProgramPTR         = GLFUNC_GLuint__VOID;
use GLCreateShaderPTR          = GLFUNC_GLuint__GLenum;
use GLDeleteProgramPTR         = GLFUNC_GLuint;
use GLDeleteShaderPTR          = GLFUNC_GLuint;
use GLDetachShaderPTR          = GLuint_GLuint;
use GLDisableVertexAttribArrayPTR = GLFUNC_GLuint;
use GLEnableVertexAttribArrayPTR  = GLFUNC_GLuint;
use GLGetActiveAttribPTR       = GLuint_GLuint_GLsizei_ptr_ptr_ptr_ptr;
use GLGetActiveUniformPTR      = GLuint_GLuint_GLsizei_ptr_ptr_ptr_ptr;
use GLGetAttachedShadersPTR    = GLuint_GLsizei_ptr_ptr;
use GLGetAttribLocationPTR     = GLint__GLuint_string;
use GLGetProgramivPTR          = GLuint_GLenum_ptr;
use GLGetProgramInfoLogPTR     = GLuint_GLsizei_ptr_ptr;
use GLGetShaderivPTR           = GLuint_GLenum_ptr;
use GLGetShaderInfoLogPTR      = GLuint_GLsizei_ptr_ptr;
use GLGetShaderSourcePTR       = GLuint_GLsizei_ptr_ptr;
use GLGetUniformLocationPTR    = GLint__GLuint_ptr;
use GLGetUniformfvPTR          = GLuint_GLint_ptr;
use GLGetUniformivPTR          = GLuint_GLint_ptr;
use GLGetVertexAttribdvPTR     = GLuint_GLenum_ptr;
use GLGetVertexAttribfvPTR     = GLuint_GLenum_ptr;
use GLGetVertexAttribivPTR     = GLuint_GLenum_ptr;
use GLGetVertexAttribPointervPTR = GLuint_GLenum_ptr;
use GLIsProgramPTR             = GLFUNC_GLboolean__GLuint;
use GLIsShaderPTR              = GLFUNC_GLboolean__GLuint;
use GLLinkProgramPTR           = GLFUNC_GLuint;
use GLShaderSourcePTR          = GLuint_GLsizei_ptr_ptr;
use GLUseProgramPTR            = GLFUNC_GLuint;
use GLUniform1fPTR             = GLint_GLfloat;
use GLUniform2fPTR             = GLint_GLfloat_GLfloat;
use GLUniform3fPTR             = GLint_GLfloat_GLfloat_GLfloat;
use GLUniform4fPTR             = GLint_GLfloat_GLfloat_GLfloat_GLfloat;
use GLUniform1iPTR             = GLint_GLint;
use GLUniform2iPTR             = GLint_GLint_GLint;
use GLUniform3iPTR             = GLint_GLint_GLint_GLint;
use GLUniform4iPTR             = GLint_GLint_GLint_GLint_GLint;
use GLUniform1fvPTR            = GLint_GLsizei_ptr;
use GLUniform2fvPTR            = GLint_GLsizei_ptr;
use GLUniform3fvPTR            = GLint_GLsizei_ptr;
use GLUniform4fvPTR            = GLint_GLsizei_ptr;
use GLUniform1ivPTR            = GLint_GLsizei_ptr;
use GLUniform2ivPTR            = GLint_GLsizei_ptr;
use GLUniform3ivPTR            = GLint_GLsizei_ptr;
use GLUniform4ivPTR            = GLint_GLsizei_ptr;
use GLUniformMatrix2fvPTR      = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix3fvPTR      = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix4fvPTR      = GLint_GLsizei_GLboolean_ptr;
use GLValidateProgramPTR       = GLFUNC_GLuint;
use GLVertexAttrib1dPTR        = GLuint_GLdouble;
use GLVertexAttrib1dvPTR       = GLuint_ptr;
use GLVertexAttrib1fPTR        = GLuint_GLfloat;
use GLVertexAttrib1fvPTR       = GLuint_ptr;
use GLVertexAttrib1sPTR        = GLuint_GLshort;
use GLVertexAttrib1svPTR       = GLuint_ptr;
use GLVertexAttrib2dPTR        = GLuint_GLdouble_GLdouble;
use GLVertexAttrib2dvPTR       = GLuint_ptr;
use GLVertexAttrib2fPTR        = GLuint_GLfloat_GLfloat;
use GLVertexAttrib2fvPTR       = GLuint_ptr;
use GLVertexAttrib2sPTR        = GLuint_GLshort_GLshort;
use GLVertexAttrib2svPTR       = GLuint_ptr;
use GLVertexAttrib3dPTR        = GLuint_GLdouble_GLdouble_GLdouble;
use GLVertexAttrib3dvPTR       = GLuint_ptr;
use GLVertexAttrib3fPTR        = GLuint_GLfloat_GLfloat_GLfloat;
use GLVertexAttrib3fvPTR       = GLuint_ptr;
use GLVertexAttrib3sPTR        = GLuint_GLshort_GLshort_GLshort;
use GLVertexAttrib3svPTR       = GLuint_ptr;
use GLVertexAttrib4NbvPTR      = GLuint_ptr;
use GLVertexAttrib4NivPTR      = GLuint_ptr;
use GLVertexAttrib4NsvPTR      = GLuint_ptr;
use GLVertexAttrib4NubPTR      = GLuint_GLubyte_GLubyte_GLubyte_GLubyte;
use GLVertexAttrib4NubvPTR     = GLuint_ptr;
use GLVertexAttrib4NuivPTR     = GLuint_ptr;
use GLVertexAttrib4NusvPTR     = GLuint_ptr;
use GLVertexAttrib4bvPTR       = GLuint_ptr;
use GLVertexAttrib4dPTR        = GLuint_GLdouble_GLdouble_GLdouble_GLdouble;
use GLVertexAttrib4dvPTR       = GLuint_ptr;
use GLVertexAttrib4fPTR        = GLuint_GLfloat_GLfloat_GLfloat_GLfloat;
use GLVertexAttrib4fvPTR       = GLuint_ptr;
use GLVertexAttrib4ivPTR       = GLuint_ptr;
use GLVertexAttrib4sPTR        = GLuint_GLshort_GLshort_GLshort_GLshort;
use GLVertexAttrib4svPTR       = GLuint_ptr;
use GLVertexAttrib4ubvPTR      = GLuint_ptr;
use GLVertexAttrib4uivPTR      = GLuint_ptr;
use GLVertexAttrib4usvPTR      = GLuint_ptr;
use GLVertexAttribPointerPTR   = GLuint_GLint_GLenum_GLboolean_GLsizei_ptr;
// 2.1
use GLUniformMatrix2x3fvPTR    = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix3x2fvPTR    = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix2x4fvPTR    = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix4x2fvPTR    = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix3x4fvPTR    = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix4x3fvPTR    = GLint_GLsizei_GLboolean_ptr;
// 3.0
use GLColorMaskiPTR            = GLuint_GLboolean_GLboolean_GLboolean_GLboolean;
use GLGetBooleani_vPTR         = GLenum_GLuint_ptr;
use GLGetIntegeri_vPTR         = GLenum_GLuint_ptr;
use GLEnableiPTR               = GLenum_GLuint;
use GLDisableiPTR              = GLenum_GLuint;
use GLIsEnablediPTR            = GLboolean__GLenum_GLuint;
use GLBeginTransformFeedbackPTR = GLFUNC_GLenum;
use GLEndTransformFeedbackPTR  = GLFUNC_VOID;
use GLBindBufferRangePTR       = GLenum_GLuint_GLuint_GLintptr_GLsizeiptr;
use GLBindBufferBasePTR        = GLenum_GLuint_GLuint;
use GLTransformFeedbackVaryingsPTR = GLuint_GLsizei_ptr_GLenum;
use GLGetTransformFeedbackVaryingPTR = GLuint_GLuint_GLsizei_ptr_ptr_ptr_ptr;
use GLClampColorPTR            = GLenum_GLenum;
use GLBeginConditionalRenderPTR = GLuint_GLenum;
use GLEndConditionalRenderPTR  = GLFUNC_VOID;
use GLVertexAttribIPointerPTR  = GLuint_GLint_GLenum_GLsizei_ptr;
use GLGetVertexAttribIivPTR    = GLuint_GLenum_ptr;
use GLGetVertexAttribIuivPTR   = GLuint_GLenum_ptr;
use GLVertexAttribI1iPTR       = GLuint_GLint;
use GLVertexAttribI2iPTR       = GLuint_GLint_GLint;
use GLVertexAttribI3iPTR       = GLuint_GLint_GLint_GLint;
use GLVertexAttribI4iPTR       = GLuint_GLint_GLint_GLint_GLint;
use GLVertexAttribI1uiPTR      = GLuint_GLuint;
use GLVertexAttribI2uiPTR      = GLuint_GLuint_GLuint;
use GLVertexAttribI3uiPTR      = GLuint_GLuint_GLuint_GLuint;
use GLVertexAttribI4uiPTR      = GLuint_GLuint_GLuint_GLuint_GLuint;
use GLVertexAttribI1ivPTR      = GLuint_ptr;
use GLVertexAttribI2ivPTR      = GLuint_ptr;
use GLVertexAttribI3ivPTR      = GLuint_ptr;
use GLVertexAttribI4ivPTR      = GLuint_ptr;
use GLVertexAttribI1uivPTR     = GLuint_ptr;
use GLVertexAttribI2uivPTR     = GLuint_ptr;
use GLVertexAttribI3uivPTR     = GLuint_ptr;
use GLVertexAttribI4uivPTR     = GLuint_ptr;
use GLVertexAttribI4bvPTR      = GLuint_ptr;
use GLVertexAttribI4svPTR      = GLuint_ptr;
use GLVertexAttribI4ubvPTR     = GLuint_ptr;
use GLVertexAttribI4usvPTR     = GLuint_ptr;
use GLGetUniformuivPTR         = GLuint_GLint_ptr;
use GLBindFragDataLocationPTR  = GLuint_GLuint_string;
use GLGetFragDataLocationPTR   = GLuint_string;
use GLUniform1uiPTR            = GLint_GLuint;
use GLUniform2uiPTR            = GLint_GLuint_GLuint;
use GLUniform3uiPTR            = GLint_GLuint_GLuint_GLuint;
use GLUniform4uiPTR            = GLint_GLuint_GLuint_GLuint_GLuint;
use GLUniform1uivPTR           = GLint_GLsizei_ptr;
use GLUniform2uivPTR           = GLint_GLsizei_ptr;
use GLUniform3uivPTR           = GLint_GLsizei_ptr;
use GLUniform4uivPTR           = GLint_GLsizei_ptr;
use GLTexParameterIivPTR       = GLenum_GLenum_ptr;
use GLTexParameterIuivPTR      = GLenum_GLenum_ptr;
use GLGetTexParameterIivPTR    = GLenum_GLenum_ptr; 
use GLGetTexParameterIuivPTR   = GLenum_GLenum_ptr;
use GLClearBufferivPTR         = GLenum_GLint_ptr;
use GLClearBufferuivPTR        = GLenum_GLint_ptr;
use GLClearBufferfvPTR         = GLenum_GLint_ptr;
use GLClearBufferfiPTR         = GLenum_GLint_GLfloat_GLint;
use GLGetStringiPTR            = ptr__GLenum_GLuint;
use GLIsRenderbufferPTR        = GLFUNC_GLboolean__GLuint;
use GLBindRenderbufferPTR      = GLenum_GLuint;
use GLDeleteRenderbuffersPTR   = GLsizei_ptr;
use GLGenRenderbuffersPTR      = GLsizei_ptr;
use GLRenderbufferStoragePTR   = GLenum_GLenum_GLsizei_GLsizei;
use GLGetRenderbufferParameterivPTR = GLenum_GLenum_ptr;
use GLIsFramebufferPTR         = GLFUNC_GLboolean__GLuint;
use GLBindFramebufferPTR       = GLenum_GLuint;
use GLDeleteFramebuffersPTR    = GLsizei_ptr;
use GLGenFramebuffersPTR       = GLsizei_ptr;
use GLCheckFramebufferStatusPTR = GLFUNC_GLenum__GLenum;
use GLFramebufferTexture1DPTR  = GLenum_GLenum_GLenum_GLuint_GLint;
use GLFramebufferTexture2DPTR  = GLenum_GLenum_GLenum_GLuint_GLint;
use GLFramebufferTexture3DPTR  = GLenum_GLenum_GLenum_GLuint_GLint_GLint;
use GLFramebufferRenderbufferPTR  = GLenum_GLenum_GLenum_GLuint;
use GLGetFramebufferAttachmentParameterivPTR = GLenum_GLenum_GLenum_ptr;
use GLGenerateMipmapPTR           = GLFUNC_GLenum;
use GLBlitFramebufferPTR          = GLint_GLint_GLint_GLint_GLint_GLint_GLint_GLint_GLbitfield_GLenum;
use GLRenderbufferStorageMultisamplePTR = GLenum_GLsizei_GLenum_GLsizei_GLsizei;
use GLFramebufferTextureLayerPTR  = GLenum_GLenum_GLuint_GLint_GLint;
use GLMapBufferRangePTR           = ptr__GLenum_GLintptr_GLsizeiptr_GLbitfield;
use GLFlushMappedBufferRangePTR   = GLenum_GLintptr_GLsizeiptr;
use GLBindVertexArrayPTR          = GLFUNC_GLuint;
use GLDeleteVertexArraysPTR       = GLsizei_ptr;
use GLGenVertexArraysPTR          = GLsizei_ptr;
use GLIsVertexArrayPTR            = GLFUNC_GLboolean__GLuint;
// 3.1
use GLDrawArraysInstancedPTR      = GLenum_GLint_GLsizei_GLsizei;
use GLDrawElementsInstancedPTR    = GLenum_GLsizei_GLenum_ptr_GLsizei;
use GLTexBufferPTR                = GLenum_GLenum_GLuint;
use GLPrimitiveRestartIndexPTR    = GLFUNC_GLuint;
use GLCopyBufferSubDataPTR        = GLenum_GLenum_GLintptr_GLintptr_GLsizeiptr;
use GLGetUniformIndicesPTR        = GLuint_GLsizei_ptr_ptr;
use GLGetActiveUniformsivPTR      = GLuint_GLsizei_ptr_GLenum_ptr;
use GLGetActiveUniformNamePTR     = GLuint_GLuint_GLsizei_ptr_ptr;
use GLGetUniformBlockIndexPTR     = GLuint_ptr;
use GLGetActiveUniformBlockivPTR  = GLuint_GLuint_GLenum_ptr;
use GLGetActiveUniformBlockNamePTR = GLuint_GLuint_GLsizei_ptr_ptr;
use GLUniformBlockBindingPTR      = GLuint_GLuint_GLuint;
// 3.2
use GLDrawElementsBaseVertexPTR   = GLenum_GLsizei_GLenum_ptr_GLint;
use GLDrawRangeElementsBaseVertexPTR = GLenum_GLuint_GLuint_GLsizei_GLenum_ptr_GLint;
use GLDrawElementsInstancedBaseVertexPTR = GLenum_GLsizei_GLenum_ptr_GLsizei_GLint;
use GLMultiDrawElementsBaseVertexPTR     = GLenum_ptr_GLenum_ptr_GLsizei_ptr;
use GLProvokingVertexPTR          = GLFUNC_GLenum;
use GLFenceSyncPTR                = GLsync__GLenum_GLbitfield;
use GLIsSyncPTR                   = GLFUNC_GLboolean__GLsync;
use GLDeleteSyncPTR               = GLFUNC_GLsync;
use GLClientWaitSyncPTR           = GLenum__GLsync_GLbitfield_GLuint64;
use GLWaitSyncPTR                 = GLsync_GLbitfield_GLuint64;
use GLGetInteger64vPTR            = GLenum_ptr;
use GLGetSyncivPTR                = GLsync_GLenum_GLsizei_ptr_ptr;
use GLGetInteger64i_vPTR          = GLenum_GLuint_ptr;
use GLGetBufferParameteri64vPTR   = GLenum_GLenum_ptr;
use GLFramebufferTexturePTR       = GLenum_GLenum_GLuint_GLint;
use GLTexImage2DMultisamplePTR    = GLenum_GLsizei_GLenum_GLsizei_GLsizei_GLboolean;
use GLTexImage3DMultisamplePTR    = GLenum_GLsizei_GLenum_GLsizei_GLsizei_GLsizei_GLboolean;
use GLGetMultisamplefvPTR         = GLenum_GLuint_ptr;
use GLSampleMaskiPTR              = GLuint_GLbitfield;
// 3.3
use GLBindFragDataLocationIndexedPTR = GLuint_GLuint_GLuint_string;
use GLGetFragDataIndexPTR         = GLint__GLuint_string;
use GLGenSamplersPTR              = GLsizei_ptr;
use GLDeleteSamplersPTR           = GLsizei_ptr;
use GLIsSamplerPTR                = GLboolean__GLuint;
use GLBindSamplerPTR              = GLuint_GLuint;
use GLSamplerParameteriPTR        = GLuint_GLenum_GLint;
use GLSamplerParameterivPTR       = GLuint_GLenum_ptr;
use GLSamplerParameterfPTR        = GLuint_GLenum_GLfloat;
use GLSamplerParameterfvPTR       = GLuint_GLenum_ptr;
use GLSamplerParameterIivPTR      = GLuint_GLenum_ptr;
use GLSamplerParameterIuivPTR     = GLuint_GLenum_ptr;
use GLGetSamplerParameterivPTR    = GLuint_GLenum_ptr;
use GLGetSamplerParameterIivPTR   = GLuint_GLenum_ptr;
use GLGetSamplerParameterfvPTR    = GLuint_GLenum_ptr;
use GLGetSamplerParameterIuivPTR  = GLuint_GLenum_ptr;
use GLQueryCounterPTR             = GLuint_GLenum;
use GLGetQueryObjecti64vPTR       = GLuint_GLenum_ptr;
use GLGetQueryObjectui64vPTR      = GLuint_GLenum_ptr;
use GLVertexAttribDivisorPTR      = GLuint_GLuint;
use GLVertexAttribP1uiPTR         = GLuint_GLenum_GLboolean_GLuint;
use GLVertexAttribP1uivPTR        = GLuint_GLenum_GLboolean_ptr;
use GLVertexAttribP2uiPTR         = GLuint_GLenum_GLboolean_GLuint;
use GLVertexAttribP2uivPTR        = GLuint_GLenum_GLboolean_ptr;
use GLVertexAttribP3uiPTR         = GLuint_GLenum_GLboolean_GLuint;
use GLVertexAttribP3uivPTR        = GLuint_GLenum_GLboolean_ptr;
use GLVertexAttribP4uiPTR         = GLuint_GLenum_GLboolean_GLuint;
use GLVertexAttribP4uivPTR        = GLuint_GLenum_GLboolean_ptr;
use GLVertexP2uiPTR               = GLenum_GLuint;
use GLVertexP2uivPTR              = GLenum_ptr;
use GLVertexP3uiPTR               = GLenum_GLuint;
use GLVertexP3uivPTR              = GLenum_ptr;
use GLVertexP4uiPTR               = GLenum_GLuint;
use GLVertexP4uivPTR              = GLenum_ptr;
use GLTexCoordP1uiPTR             = GLenum_GLuint;
use GLTexCoordP1uivPTR            = GLenum_ptr;
use GLTexCoordP2uiPTR             = GLenum_GLuint;
use GLTexCoordP2uivPTR            = GLenum_ptr;
use GLTexCoordP3uiPTR             = GLenum_GLuint;
use GLTexCoordP3uivPTR = GLenum_ptr;
use GLTexCoordP4uiPTR  = GLenum_GLuint;
use GLTexCoordP4uivPTR = GLenum_ptr;
use GLMultiTexCoordP1uiPTR = GLenum_GLenum_GLuint;
use GLMultiTexCoordP1uivPTR = GLenum_GLenum_ptr;
use GLMultiTexCoordP2uiPTR = GLenum_GLenum_GLuint;
use GLMultiTexCoordP2uivPTR = GLenum_GLenum_ptr;
use GLMultiTexCoordP3uiPTR = GLenum_GLenum_GLuint;
use GLMultiTexCoordP3uivPTR = GLenum_GLenum_ptr;
use GLMultiTexCoordP4uiPTR = GLenum_GLenum_GLuint;
use GLMultiTexCoordP4uivPTR = GLenum_GLenum_ptr;
use GLNormalP3uiPTR = GLenum_GLuint;
use GLNormalP3uivPTR = GLenum_ptr;
use GLColorP3uiPTR = GLenum_GLuint;
use GLColorP3uivPTR = GLenum_ptr;
use GLColorP4uiPTR = GLenum_GLuint;
use GLColorP4uivPTR = GLenum_ptr;
use GLSecondaryColorP3uiPTR = GLenum_GLuint;
use GLSecondaryColorP3uivPTR = GLenum_ptr;
// 4.0
use GLMinSampleShadingPTR = GLFUNC_GLfloat;
use GLBlendEquationiPTR = GLuint_GLenum;
use GLBlendEquationSeparateiPTR = GLuint_GLenum_GLenum;
use GLBlendFunciPTR = GLuint_GLenum_GLenum;
use GLBlendFuncSeparateiPTR = GLuint_GLenum_GLenum_GLenum_GLenum;
use GLDrawArraysIndirectPTR = GLenum_ptr;
use GLDrawElementsIndirectPTR = GLenum_GLenum_ptr;
use GLUniform1dPTR = GLint_GLdouble;
use GLUniform2dPTR = GLint_GLdouble_GLdouble;
use GLUniform3dPTR = GLint_GLdouble_GLdouble_GLdouble;
use GLUniform4dPTR = GLint_GLdouble_GLdouble_GLdouble_GLdouble;
use GLUniform1dvPTR = GLint_GLsizei_ptr;
use GLUniform2dvPTR = GLint_GLsizei_ptr;
use GLUniform3dvPTR = GLint_GLsizei_ptr;
use GLUniform4dvPTR = GLint_GLsizei_ptr;
use GLUniformMatrix2dvPTR = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix3dvPTR = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix4dvPTR = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix2x3dvPTR = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix2x4dvPTR = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix3x2dvPTR = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix3x4dvPTR = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix4x2dvPTR = GLint_GLsizei_GLboolean_ptr;
use GLUniformMatrix4x3dvPTR = GLint_GLsizei_GLboolean_ptr;
use GLGetUniformdvPTR = GLuint_GLint_ptr;
use GLGetSubroutineUniformLocationPTR = GLint__GLuint_GLenum_string;
use GLGetSubroutineIndexPTR = GLuint__GLuint_GLenum_string;
use GLGetActiveSubroutineUniformivPTR = GLuint_GLenum_GLuint_GLenum_ptr;
use GLGetActiveSubroutineUniformNamePTR = GLuint_GLenum_GLenum_GLenum_GLenum;
use GLGetActiveSubroutineNamePTR = GLuint_GLenum_GLuint_GLsizei_ptr_ptr;
use GLUniformSubroutinesuivPTR = GLenum_GLsizei_ptr;
use GLGetUniformSubroutineuivPTR = GLenum_GLint_ptr;
use GLGetProgramStageivPTR = GLuint_GLenum_GLenum_ptr;
use GLPatchParameteriPTR = GLenum_GLint;
use GLPatchParameterfvPTR = GLenum_ptr;
use GLBindTransformFeedbackPTR = GLenum_GLuint;
use GLDeleteTransformFeedbacksPTR = GLsizei_ptr;
use GLGenTransformFeedbacksPTR = GLsizei_ptr;
use GLIsTransformFeedbackPTR = GLboolean__GLuint;
use GLPauseTransformFeedbackPTR   = GLFUNC_VOID;
use GLResumeTransformFeedbackPTR = GLFUNC_VOID;
use GLDrawTransformFeedbackPTR = GLenum_GLuint;
use GLDrawTransformFeedbackStreamPTR = GLenum_GLuint_GLuint;
use GLBeginQueryIndexedPTR = GLenum_GLuint_GLuint;
use GLEndQueryIndexedPTR = GLenum_GLuint;
use GLGetQueryIndexedivPTR = GLenum_GLuint_GLenum_ptr;
// == ext ==
use GLMultiDrawArraysIndirectAMDPTR = GLenum_ptr_GLsizei_GLsizei;
use GLMultiDrawElementsIndirectAMDPTR = GLenum_GLenum_ptr_GLsizei_GLsizei;
use GLReleaseShaderCompilerPTR = GLFUNC_VOID;
use GLShaderBinaryPTR = GLsizei_ptr_GLenum_ptr_GLsizei;
use GLGetShaderPrecisionFormatPTR = GLenum_GLenum_ptr_ptr;
use GLDepthRangefPTR = GLfloat_GLfloat;
use GLClearDepthfPTR = GLFUNC_GLfloat;
use GLMemoryBarrierByRegionPTR = GLFUNC_GLbitfield;
use GLPrimitiveBoundingBoxARBPTR = GLfloat_GLfloat_GLfloat_GLfloat_GLfloat_GLfloat_GLfloat_GLfloat;
use GLDrawArraysInstancedBaseInstancePTR = GLenum_GLint_GLsizei_GLsizei_GLuint;
use GLDrawElementsInstancedBaseInstancePTR = GLenum_GLsizei_GLenum_ptr_GLsizei_GLuint;
use GLDrawElementsInstancedBaseVertexBaseInstancePTR = GLenum_GLsizei_GLenum_ptr_GLsizei_GLint_GLuint;
use GLBufferStoragePTR = GLenum_GLsizeiptr_ptr_GLbitfield;
use GLClearBufferDataPTR = GLenum_GLenum_GLenum_GLenum_ptr;
use GLClearBufferSubDataPTR = GLenum_GLenum_GLintptr_GLsizeiptr_GLenum_GLenum_ptr;
use GLClearTexImagePTR = GLuint_GLint_GLenum_GLenum_ptr;
use GLClearTexSubImagePTR = GLuint_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei_GLenum_GLenum_ptr;
use GLClipControlPTR = GLenum_GLenum;
use GLClampColorARBPTR = GLenum_GLenum;
use GLDispatchComputePTR = GLuint_GLuint_GLuint;
use GLDispatchComputeIndirectPTR = GLFUNC_GLintptr;
use GLDispatchComputeGroupSizeARBPTR = GLuint_GLuint_GLuint_GLuint_GLuint_GLuint;
use GLCopyImageSubDataPTR = GLuint_GLenum_GLint_GLint_GLint_GLint_GLuint_GLenum_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei;
use GLDebugMessageControlARBPTR = GLenum_GLenum_GLenum_GLsizei_ptr_GLboolean;
use GLDebugMessageInsertARBPTR = GLenum_GLenum_GLuint_GLenum_GLsizei_ptr;
use GLDebugMessageCallbackARBPTR = ptr_ptr;
use GLGetDebugMessageLogARBPTR = GLuint_GLsizei_ptr_ptr_ptr_ptr_ptr_ptr;
use GLCreateTransformFeedbacksPTR = GLsizei_ptr;
use GLTransformFeedbackBufferBasePTR = GLuint_GLuint_GLuint;
use GLTransformFeedbackBufferRangePTR = GLuint_GLuint_GLuint_GLintptr_GLsizeiptr;
use GLGetTransformFeedbackivPTR = GLuint_GLenum_ptr;
use GLGetTransformFeedbacki_vPTR = GLuint_GLenum_GLuint_ptr;
use GLGetTransformFeedbacki64_vPTR = GLuint_GLenum_GLuint_ptr;
use GLCreateBuffersPTR = GLsizei_ptr;
use GLNamedBufferStoragePTR = GLuint_GLsizeiptr_ptr_GLbitfield;
use GLNamedBufferDataPTR = GLuint_GLsizeiptr_ptr_GLenum;
use GLNamedBufferSubDataPTR = GLuint_GLintptr_GLsizeiptr_ptr;
use GLCopyNamedBufferSubDataPTR = GLuint_GLuint_GLintptr_GLintptr_GLsizeiptr;
use GLClearNamedBufferDataPTR = GLuint_GLenum_GLenum_GLenum_ptr;
use GLClearNamedBufferSubDataPTR = GLuint_GLenum_GLintptr_GLsizeiptr_GLenum_GLenum_ptr;
use GLMapNamedBufferPTR           = pointer__GLuint_GLenum;
use GLMapNamedBufferRangePTR = ptr__GLuint_GLintptr_GLsizeiptr_GLbitfield;
use GLUnmapNamedBufferPTR = GLboolean__GLuint;
use GLFlushMappedNamedBufferRangePTR = GLuint_GLintptr_GLsizeiptr;
use GLGetNamedBufferParameterivPTR = GLuint_GLenum_ptr;
use GLGetNamedBufferParameteri64vPTR = GLuint_GLenum_ptr;
use GLGetNamedBufferPointervPTR = GLuint_GLenum_ptr;
use GLGetNamedBufferSubDataPTR = GLuint_GLintptr_GLsizeiptr_ptr;
use GLCreateFramebuffersPTR = GLsizei_ptr;
use GLNamedFramebufferRenderbufferPTR = GLuint_GLenum_GLenum_GLuint;
use GLNamedFramebufferParameteriPTR = GLuint_GLenum_GLint;
use GLNamedFramebufferTexturePTR = GLuint_GLenum_GLuint_GLint;
use GLNamedFramebufferTextureLayerPTR = GLuint_GLenum_GLuint_GLint_GLint;
use GLNamedFramebufferDrawBufferPTR = GLuint_GLenum;
use GLNamedFramebufferDrawBuffersPTR = GLuint_GLsizei_ptr;
use GLNamedFramebufferReadBufferPTR = GLuint_GLenum;
use GLInvalidateNamedFramebufferDataPTR = GLuint_GLsizei_ptr;
use GLInvalidateNamedFramebufferSubDataPTR = GLuint_GLsizei_ptr_GLint_GLint_GLsizei_GLsizei;
use GLClearNamedFramebufferivPTR = GLuint_GLenum_GLint_ptr;
use GLClearNamedFramebufferuivPTR = GLuint_GLenum_GLint_ptr;
use GLClearNamedFramebufferfvPTR = GLuint_GLenum_GLint_ptr;
use GLClearNamedFramebufferfiPTR = GLuint_GLenum_GLint_GLfloat_GLint;
use GLBlitNamedFramebufferPTR = GLuint_GLuint_GLint_GLint_GLint_GLint_GLint_GLint_GLint_GLint_GLbitfield_GLenum;
use GLCheckNamedFramebufferStatusPTR = GLenum__GLuint_GLenum;
use GLGetNamedFramebufferParameterivPTR = GLuint_GLenum_ptr;
use GLGetNamedFramebufferAttachmentParameterivPTR = GLuint_GLenum_GLenum_ptr;
use GLCreateRenderbuffersPTR = GLsizei_ptr;
use GLNamedRenderbufferStoragePTR = GLuint_GLenum_GLsizei_GLsizei;
use GLNamedRenderbufferStorageMultisamplePTR = GLuint_GLsizei_GLenum_GLsizei_GLsizei;
use GLGetNamedRenderbufferParameterivPTR = GLuint_GLenum_ptr;
use GLCreateTexturesPTR = GLenum_GLsizei_ptr;
use GLTextureBufferPTR = GLuint_GLenum_GLuint;
use GLTextureBufferRangePTR = GLuint_GLenum_GLuint_GLintptr_GLsizeiptr;
use GLTextureStorage1DPTR = GLuint_GLsizei_GLenum_GLsizei;
use GLTextureStorage2DPTR = GLuint_GLsizei_GLenum_GLsizei_GLsizei;
use GLTextureStorage3DPTR = GLuint_GLsizei_GLenum_GLsizei_GLsizei_GLsizei;
use GLTextureStorage2DMultisamplePTR = GLuint_GLsizei_GLenum_GLsizei_GLsizei_GLboolean;
use GLTextureStorage3DMultisamplePTR = GLuint_GLsizei_GLenum_GLsizei_GLsizei_GLsizei_GLboolean;
use GLTextureSubImage1DPTR = GLuint_GLint_GLint_GLsizei_GLenum_GLenum_ptr;
use GLTextureSubImage2DPTR = GLuint_GLint_GLint_GLint_GLsizei_GLsizei_GLenum_GLenum_ptr;
use GLTextureSubImage3DPTR = GLuint_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei_GLenum_GLenum_ptr;
use GLCompressedTextureSubImage1DPTR = GLuint_GLint_GLint_GLsizei_GLenum_GLsizei_ptr;
use GLCompressedTextureSubImage2DPTR = GLuint_GLint_GLint_GLint_GLsizei_GLsizei_GLenum_GLsizei_ptr;
use GLCompressedTextureSubImage3DPTR = GLuint_GLint_GLint_GLint_GLint_GLsizei_GLsizei_GLsizei_GLenum_GLsizei_ptr;
use GLCopyTextureSubImage1DPTR = GLuint_GLint_GLint_GLint_GLint_GLsizei;
use GLCopyTextureSubImage2DPTR = GLuint_GLint_GLint_GLint_GLint_GLint_GLsizei_GLsizei;
use GLCopyTextureSubImage3DPTR = GLuint_GLint_GLint_GLint_GLint_GLint_GLint_GLsizei_GLsizei;

const string[] Prefixes = $[ "OpenGL ES-CM ", "OpenGL ES-CL ", "OpenGL ES " ];

public struct GLAD_Config
{
   int                  Major : rprop(major); 
   int                  Minor : rprop(minor); 
   
   int                  GLAD_GL_VERSION_1_0 : rprop;
   int                  GLAD_GL_VERSION_1_1 : rprop;
   int                  GLAD_GL_VERSION_1_2 : rprop;
   int                  GLAD_GL_VERSION_1_3 : rprop;
   int                  GLAD_GL_VERSION_1_4 : rprop;
   int                  GLAD_GL_VERSION_1_5 : rprop;
   int                  GLAD_GL_VERSION_2_0 : rprop;
   int                  GLAD_GL_VERSION_2_1 : rprop;
   int                  GLAD_GL_VERSION_3_0 : rprop;
   int                  GLAD_GL_VERSION_3_1 : rprop;
   int                  GLAD_GL_VERSION_3_2 : rprop;
   int                  GLAD_GL_VERSION_3_3 : rprop;
   int                  GLAD_GL_VERSION_4_0 : rprop;
   
   int                  GLAD_GL_AMD_multi_draw_indirect : rprop;
   int                  GLAD_GL_ARB_ES2_compatibility : rprop;
   int                  GLAD_GL_ARB_ES3_1_compatibility : rprop;
   int                  GLAD_GL_ARB_ES3_2_compatibility : rprop;
   int                  GLAD_GL_ARB_base_instance : rprop;
   int                  GLAD_GL_ARB_blend_func_extended : rprop;
   int                  GLAD_GL_ARB_buffer_storage : rprop;
   int                  GLAD_GL_ARB_clear_buffer_object : rprop;
   int                  GLAD_GL_ARB_clear_texture : rprop;
   int                  GLAD_GL_ARB_clip_control : rprop;
   int                  GLAD_GL_ARB_color_buffer_float : rprop;
   int                  GLAD_GL_ARB_compute_shader : rprop;
   int                  GLAD_GL_ARB_copy_buffer : rprop;
   int                  GLAD_GL_ARB_compute_variable_group_size : rprop;
   int                  GLAD_GL_ARB_copy_image : rprop;
   int                  GLAD_GL_ARB_debug_output : rprop;
   int                  GLAD_GL_ARB_direct_state_access : rprop;

   private int has_ext(string extensions, string ext)
   {
      int index := extensions.indexOf(0, ext);
      if (index >= 0) {
         int endIndex := index + ext.Length;
         if (endIndex == extensions.Length || extensions[endIndex] == $32) {
            ^ 1
         };
      };
      
      ^ 0
   }
      
   private init(string versions)
   {
      int prefixLen := Prefixes.Length;
      int start := 0;
      for (int i := 0; i < prefixLen; i++) {
         if (versions.startingWith(Prefixes[i])) {
            start := Prefixes[i].Length;
            
            :break;
         }
      };
      
      int dotPos := versions.indexOf(start, $46);
      major := versions.Substring(start, dotPos - start).toInt();
      start:= dotPos + 1;
      dotPos := versions.indexOf(start, $46);
      minor := versions.Substring(start, dotPos - start).toInt();
      
      GLAD_GL_VERSION_1_0 := ((major == 1 && minor >= 0) || major > 1) ? 1 : 0;
      GLAD_GL_VERSION_1_1 := ((major == 1 && minor >= 1) || major > 1) ? 1 : 0;
      GLAD_GL_VERSION_1_2 := ((major == 1 && minor >= 2) || major > 1) ? 1 : 0;
      GLAD_GL_VERSION_1_3 := ((major == 1 && minor >= 3) || major > 1) ? 1 : 0;
      GLAD_GL_VERSION_1_4 := ((major == 1 && minor >= 4) || major > 1) ? 1 : 0;
      GLAD_GL_VERSION_1_5 := ((major == 1 && minor >= 5) || major > 1) ? 1 : 0;
      GLAD_GL_VERSION_2_0 := ((major == 2 && minor >= 0) || major > 2) ? 1 : 0;
      GLAD_GL_VERSION_2_1 := ((major == 2 && minor >= 1) || major > 2) ? 1 : 0;
      GLAD_GL_VERSION_3_0 := ((major == 3 && minor >= 0) || major > 3) ? 1 : 0;
      GLAD_GL_VERSION_3_1 := ((major == 3 && minor >= 1) || major > 3) ? 1 : 0;
      GLAD_GL_VERSION_3_2 := ((major == 3 && minor >= 2) || major > 3) ? 1 : 0;
      GLAD_GL_VERSION_3_3 := ((major == 3 && minor >= 3) || major > 3) ? 1 : 0;
      GLAD_GL_VERSION_4_0 := ((major == 4 && minor >= 0) || major > 4) ? 1 : 0;      
   }
   
   private init_ext(string extensions)
   {
      GLAD_GL_AMD_multi_draw_indirect := has_ext(extensions, "GL_AMD_multi_draw_indirect");
      GLAD_GL_ARB_ES2_compatibility := has_ext(extensions, "GL_ARB_ES2_compatibility");
      GLAD_GL_ARB_ES3_1_compatibility := has_ext(extensions, "GL_ARB_ES3_1_compatibility");
      GLAD_GL_ARB_ES3_2_compatibility := has_ext(extensions, "GL_ARB_ES3_2_compatibility");
      GLAD_GL_ARB_base_instance := has_ext(extensions, "GL_ARB_base_instance");
      GLAD_GL_ARB_blend_func_extended := has_ext(extensions, "GL_ARB_blend_func_extended");
      GLAD_GL_ARB_buffer_storage := has_ext(extensions, "GL_ARB_buffer_storage");
      GLAD_GL_ARB_clear_buffer_object := has_ext(extensions, "GL_ARB_clear_buffer_object");
      GLAD_GL_ARB_clear_texture := has_ext(extensions, "GL_ARB_clear_texture");
      GLAD_GL_ARB_clip_control := has_ext(extensions, "GL_ARB_clip_control");
      GLAD_GL_ARB_color_buffer_float := has_ext(extensions, "GL_ARB_color_buffer_float");
      GLAD_GL_ARB_compute_shader := has_ext(extensions, "GL_ARB_compute_shader");
      GLAD_GL_ARB_compute_variable_group_size := has_ext(extensions, "GL_ARB_compute_variable_group_size");
      GLAD_GL_ARB_copy_buffer := has_ext(extensions, "GL_ARB_copy_buffer");
      GLAD_GL_ARB_copy_image := has_ext(extensions, "GL_ARB_copy_image");
      GLAD_GL_ARB_debug_output := has_ext(extensions, "GL_ARB_debug_output");
      GLAD_GL_ARB_direct_state_access := has_ext(extensions, "GL_ARB_direct_state_access");      
   }
   
   constructor load(string versions, string extensions)
   {
      init(versions);
      init_ext(extensions);
   }
}

public struct GLAD_Wrapper
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   GLCullFacePTR        glCullFace;
	GLFrontFacePTR       glFrontFace;
	GLHintPTR            glHint;
	GLLineWidthPTR       glLineWidth;
   GLPointSizePTR       glPointSize;
   GLPolygonModePTR     glPolygonMode;
   GLScissorPTR         glScissor;
   GLTexImage1DPTR      glTexImage1D;
   GLTexImage2DPTR      glTexImage2D;
   GLDrawBufferPTR      glDrawBuffer;
   GLClearPTR           glClear;
   GLClearColorPTR      glClearColor;
   GLClearStencilPTR    glClearStencil;
   GLClearDepthPTR      glClearDepth;
   GLStencilMaskPTR     glStencilMask;
   GLColorMaskPTR       glColorMask;
   GLDepthMaskPTR       glDepthMask;
   GLDisablePTR         glDisable;
   GLEnablePTR          glEnable;
   GLFinishPTR          glFinish;
   GLFlushPTR           glFlush;
   GLBlendFuncPTR       glBlendFunc;
   GLLogicOpPTR         glLogicOp;
   GLStencilFuncPTR     glStencilFunc;
   GLStencilOpPTR       glStencilOp;
   GLDepthFuncPTR       glDepthFunc;
   GLReadBufferPTR      glReadBuffer;
   GLReadPixelsPTR      glReadPixels;
   GLGetBooleanvPTR     glGetBooleanv;
   GLGetDoublevPTR      glGetDoublev;
   GLGetErrorPTR        glGetError;
   GLGetFloatvPTR       glGetFloatv;
   GLGetIntegervPTR     glGetIntegerv;
//   GLGetStringPTR       glGetString;
   GLGetTexImagePTR     glGetTexImage;
   GLIsEnabledPTR       glIsEnabled;
   GLDepthRangePTR      glDepthRange;
   GLViewportPTR        glViewport;
   GLNewListPTR         glNewList;
   GLEndListPTR         glEndList;
   GLCallListPTR        glCallList;
   GLCallListsPTR       glCallLists;
   GLDeleteListsPTR     glDeleteLists;
   GLGenListsPTR        glGenLists;
   GLListBasePTR        glListBase;
   GLBeginPTR           glBegin;
   GLBitmapPTR          glBitmap;
   GLEdgeFlagPTR        glEdgeFlag;
   GLEdgeFlagvPTR       glEdgeFlagv;
   GLEndPTR             glEnd;
   GLClipPlanePTR       glClipPlane;
   GLColorMaterialPTR   glColorMaterial;
   GLLineStipplePTR     glLineStipple;
   GLPolygonStipplePTR  glPolygonStipple;
   GLShadeModelPTR      glShadeModel;
   GLFeedbackBufferPTR  glFeedbackBuffer;
   GLSelectBufferPTR    glSelectBuffer;
   GLRenderModePTR      glRenderMode;
   GLInitNamesPTR       glInitNames;
   GLLoadNamePTR        glLoadName;
   GLPassThroughPTR     glPassThrough;
   GLPopNamePTR         glPopName;
   GLPushNamePTR        glPushName;
   GLClearAccumPTR      glClearAccum;
   GLClearIndexPTR      glClearIndex;
   GLIndexMaskPTR       glIndexMask;
   GLAccumPTR           glAccum;
   GLPopAttribPTR       glPopAttrib;
   GLPushAttribPTR      glPushAttrib;
   GLEvalMesh1PTR       glEvalMesh1;
   GLEvalPoint1PTR      glEvalPoint1;
   GLEvalMesh2PTR       glEvalMesh2;
   GLEvalPoint2PTR      glEvalPoint2;
   GLAlphaFuncPTR       glAlphaFunc;
   GLPixelZoomPTR       glPixelZoom;
   GLCopyPixelsPTR      glCopyPixels;
   GLDrawPixelsPTR      glDrawPixels;
   GLGetClipPlanePTR    glGetClipPlane;
   GLGetPolygonStipplePTR glGetPolygonStipple;
   GLIsListPTR          glIsList;
   GLFrustumPTR         glFrustum;
   GLLoadIdentityPTR    glLoadIdentity;
   GLMatrixModePTR      glMatrixMode;
   GLOrthoPTR           glOrtho;
   GLPopMatrixPTR       glPopMatrix;
   GLPushMatrixPTR      glPushMatrix;
   
   // 1.1 
   GLDrawArraysPTR      glDrawArrays;
   GLDrawElementsPTR    glDrawElements;
   GLGetPointervPTR     glGetPointerv;
   GLPolygonOffsetPTR   glPolygonOffset;
   GLCopyTexImage1DPTR  glCopyTexImage1D;
   GLCopyTexImage2DPTR  glCopyTexImage2D;
   GLCopyTexSubImage1DPTR glCopyTexSubImage1D;
   GLCopyTexSubImage2DPTR glCopyTexSubImage2D;
   GLTexSubImage1DPTR   glTexSubImage1D;
   GLTexSubImage2DPTR   glTexSubImage2D;
   GLBindTexturePTR     glBindTexture;
   GLDeleteTexturesPTR  glDeleteTextures;
   GLGenTexturesPTR     glGenTextures;
   GLIsTexturePTR       glIsTexture;
   GLArrayElementPTR    glArrayElement;
   GLColorPointerPTR    glColorPointer;
   GLDisableClientStatePTR glDisableClientState;
   GLEdgeFlagPointerPTR glEdgeFlagPointer;
   GLEnableClientStatePTR  glEnableClientState;
   GLIndexPointerPTR    glIndexPointer;
   GLInterleavedArraysPTR  glInterleavedArrays;
   GLNormalPointerPTR      glNormalPointer;
   GLTexCoordPointerPTR    glTexCoordPointer;
   GLVertexPointerPTR      glVertexPointer;
   GLAreTexturesResidentPTR glAreTexturesResident;
   GLPrioritizeTexturesPTR glPrioritizeTextures;
   
   // ?
   GLIndexubPTR            glIndexub;
   GLIndexubvPTR           glIndexubv;
   
   GLPopClientAttribPTR    glPopClientAttrib;
   GLPushClientAttribPTR   glPushClientAttrib;
   // 1.2
   GLDrawRangeElementsPTR  glDrawRangeElements;
   GLTexImage3DPTR         glTexImage3D;
   GLTexSubImage3DPTR      glTexSubImage3D;
   GLCopyTexSubImage3DPTR  glCopyTexSubImage3D;
   // 1.3
   GLActiveTexturePTR         glActiveTexture;
   GLSampleCoveragePTR        glSampleCoverage;
   GLCompressedTexImage3DPTR  glCompressedTexImage3D;
   GLCompressedTexImage2DPTR  glCompressedTexImage2D;
   GLCompressedTexImage1DPTR  glCompressedTexImage1D;
   GLCompressedTexSubImage3DPTR glCompressedTexSubImage3D;
   GLCompressedTexSubImage2DPTR glCompressedTexSubImage2D;
   GLCompressedTexSubImage1DPTR glCompressedTexSubImage1D;
   GLGetCompressedTexImagePTR glGetCompressedTexImage;
   GLClientActiveTexturePTR   glClientActiveTexture;  
   // 1.4
   GLBlendFuncSeparatePTR     glBlendFuncSeparate;
   GLMultiDrawArraysPTR       glMultiDrawArrays;
   GLMultiDrawElementsPTR     glMultiDrawElements;
      
   GLFogCoordPointerPTR       glFogCoordPointer;
      
   GLSecondaryColorPointerPTR    glSecondaryColorPointer;   
   GLBlendColorPTR            glBlendColor;
   GLBlendEquationPTR         glBlendEquation;
   // 1.5
   GLGenQueriesPTR            glGenQueries;
   GLDeleteQueriesPTR         glDeleteQueries;
   GLIsQueryPTR               glIsQuery;
   GLBeginQueryPTR            glBeginQuery;
   GLEndQueryPTR              glEndQuery;
   GLGetQueryivPTR            glGetQueryiv;
   GLBindBufferPTR            glBindBuffer; 
   GLDeleteBuffersPTR         glDeleteBuffers;
   GLGenBuffersPTR            glGenBuffers;
   GLIsBufferPTR              glIsBuffer;
   GLBufferDataPTR            glBufferData;
   GLBufferSubDataPTR         glBufferSubData;
   GLGetBufferSubDataPTR      glGetBufferSubData;
   GLMapBufferPTR             glMapBuffer;
   GLUnmapBufferPTR           glUnmapBuffer;
   GLGetBufferParameterivPTR  glGetBufferParameteriv;
   GLGetBufferPointervPTR     glGetBufferPointerv;
   // 2.0
   GLBlendEquationSeparatePTR glBlendEquationSeparate;
   GLDrawBuffersPTR           glDrawBuffers;
   GLStencilOpSeparatePTR     glStencilOpSeparate;
   GLStencilFuncSeparatePTR   glStencilFuncSeparate;
   GLStencilMaskSeparatePTR   glStencilMaskSeparate;
   GLAttachShaderPTR          glAttachShader;
   GLBindAttribLocationPTR    glBindAttribLocation;
   GLCompileShaderPTR         glCompileShader;
   GLCreateProgramPTR         glCreateProgram;
   GLCreateShaderPTR          glCreateShader;
   GLDeleteProgramPTR         glDeleteProgram;
   GLDeleteShaderPTR          glDeleteShader;
   GLDetachShaderPTR          glDetachShader;
   GLDisableVertexAttribArrayPTR glDisableVertexAttribArray;
   GLEnableVertexAttribArrayPTR  glEnableVertexAttribArray;
   GLGetActiveAttribPTR       glGetActiveAttrib;
   GLGetActiveUniformPTR      glGetActiveUniform;
   GLGetAttachedShadersPTR    glGetAttachedShaders;
   GLGetAttribLocationPTR     glGetAttribLocation;
   GLGetProgramivPTR          glGetProgramiv;
   GLGetProgramInfoLogPTR     glGetProgramInfoLog;
   GLGetShaderivPTR           glGetShaderiv;
   GLGetShaderInfoLogPTR      glGetShaderInfoLog;
   GLGetShaderSourcePTR       glGetShaderSource;      
   GLGetUniformLocationPTR    glGetUniformLocation;   
   GLGetVertexAttribPointervPTR glGetVertexAttribPointerv;
   GLIsProgramPTR             glIsProgram;
   GLIsShaderPTR              glIsShader;
   GLLinkProgramPTR           glLinkProgram;
   GLShaderSourcePTR          glShaderSource;
   GLUseProgramPTR            glUseProgram;
   GLValidateProgramPTR       glValidateProgram;   
   GLVertexAttribPointerPTR   glVertexAttribPointer;   
   GLVertexAttribP1uiPTR      glVertexAttribP1ui;
   GLVertexAttribP1uivPTR     glVertexAttribP1uiv;
   GLVertexAttribP2uiPTR      glVertexAttribP2ui;
   GLVertexAttribP2uivPTR     glVertexAttribP2uiv;
   GLVertexAttribP3uiPTR      glVertexAttribP3ui;
   GLVertexAttribP3uivPTR     glVertexAttribP3uiv;
   GLVertexAttribP4uiPTR      glVertexAttribP4ui;
   GLVertexAttribP4uivPTR     glVertexAttribP4uiv;
   // 2.1   
   // 3.0
   GLColorMaskiPTR            glColorMaski;
   GLGetBooleani_vPTR         glGetBooleani_v;
   GLGetIntegeri_vPTR         glGetIntegeri_v;
   GLEnableiPTR               glEnablei;
   GLDisableiPTR              glDisablei;
   GLIsEnablediPTR            glIsEnabledi;
   GLBeginTransformFeedbackPTR glBeginTransformFeedback;
   GLEndTransformFeedbackPTR  glEndTransformFeedback;
   GLBindBufferRangePTR       glBindBufferRange;
   GLBindBufferBasePTR        glBindBufferBase;
   GLTransformFeedbackVaryingsPTR glTransformFeedbackVaryings;
   GLGetTransformFeedbackVaryingPTR glGetTransformFeedbackVarying;
   GLClampColorPTR            glClampColor;   
   GLBeginConditionalRenderPTR glBeginConditionalRender;
   GLEndConditionalRenderPTR  glEndConditionalRender;
   GLVertexAttribIPointerPTR  glVertexAttribIPointer;
   GLGetVertexAttribIivPTR    glGetVertexAttribIiv;
   GLGetVertexAttribIuivPTR   glGetVertexAttribIuiv;   
   GLGetUniformuivPTR         glGetUniformuiv;
   GLBindFragDataLocationPTR  glBindFragDataLocation;
   GLGetFragDataLocationPTR   glGetFragDataLocation;   
   GLGetStringiPTR            glGetStringi;
   GLIsRenderbufferPTR        glIsRenderbuffer;
   GLBindRenderbufferPTR      glBindRenderbuffer;
   GLDeleteRenderbuffersPTR   glDeleteRenderbuffers;
   GLGenRenderbuffersPTR      glGenRenderbuffers;
   GLRenderbufferStoragePTR   glRenderbufferStorage;
   GLGetRenderbufferParameterivPTR glGetRenderbufferParameteriv;
   GLIsFramebufferPTR         glIsFramebuffer;
   GLBindFramebufferPTR       glBindFramebuffer;
   GLDeleteFramebuffersPTR    glDeleteFramebuffers;
   GLGenFramebuffersPTR       glGenFramebuffers;
   GLCheckFramebufferStatusPTR glCheckFramebufferStatus;
   GLFramebufferTexture1DPTR  glFramebufferTexture1D;
   GLFramebufferTexture2DPTR  glFramebufferTexture2D;
   GLFramebufferTexture3DPTR  glFramebufferTexture3D;
   GLFramebufferRenderbufferPTR  glFramebufferRenderbuffer;
   GLGetFramebufferAttachmentParameterivPTR glGetFramebufferAttachmentParameteriv;
   GLGenerateMipmapPTR           glGenerateMipmap;
   GLBlitFramebufferPTR          glBlitFramebuffer;
   GLRenderbufferStorageMultisamplePTR glRenderbufferStorageMultisample;
   GLFramebufferTextureLayerPTR  glFramebufferTextureLayer;
   GLMapBufferRangePTR           glMapBufferRange;
   GLFlushMappedBufferRangePTR   glFlushMappedBufferRange;
   GLBindVertexArrayPTR          glBindVertexArray;
   GLDeleteVertexArraysPTR       glDeleteVertexArrays;
   GLGenVertexArraysPTR          glGenVertexArrays;
   GLIsVertexArrayPTR            glIsVertexArray;
   GLTexCoordP1uiPTR             glTexCoordP1ui;
   GLTexCoordP1uivPTR            glTexCoordP1uiv;
   GLTexCoordP2uiPTR             glTexCoordP2ui;
   GLTexCoordP2uivPTR            glTexCoordP2uiv;
   GLTexCoordP3uiPTR             glTexCoordP3ui;
   GLTexCoordP3uivPTR            glTexCoordP3uiv;
   GLTexCoordP4uiPTR             glTexCoordP4ui;
   GLTexCoordP4uivPTR            glTexCoordP4uiv;   
   // 3.1
   GLDrawArraysInstancedPTR      glDrawArraysInstanced;
   GLDrawElementsInstancedPTR    glDrawElementsInstanced;
   GLTexBufferPTR                glTexBuffer;
   GLPrimitiveRestartIndexPTR    glPrimitiveRestartIndex;
   GLCopyBufferSubDataPTR        glCopyBufferSubData;
   GLGetUniformIndicesPTR        glGetUniformIndices;
   GLGetActiveUniformsivPTR      glGetActiveUniformsiv;
   GLGetActiveUniformNamePTR     glGetActiveUniformName;
   GLGetUniformBlockIndexPTR     glGetUniformBlockIndex;
   GLGetActiveUniformBlockivPTR  glGetActiveUniformBlockiv;
   GLGetActiveUniformBlockNamePTR glGetActiveUniformBlockName;
   GLUniformBlockBindingPTR      glUniformBlockBinding;
//   /*GLBindBufferRangePTR          glBindBufferRange;
//   GLGetIntegeri_vPTR            glGetIntegeri_v;*/
   // 3.2
   GLDrawElementsBaseVertexPTR   glDrawElementsBaseVertex;
   GLDrawRangeElementsBaseVertexPTR glDrawRangeElementsBaseVertex;
   GLDrawElementsInstancedBaseVertexPTR glDrawElementsInstancedBaseVertex;
   GLMultiDrawElementsBaseVertexPTR     glMultiDrawElementsBaseVertex;
   GLProvokingVertexPTR          glProvokingVertex;
   GLFenceSyncPTR                glFenceSync;
   GLIsSyncPTR                   glIsSync;
   GLDeleteSyncPTR               glDeleteSync;
   GLClientWaitSyncPTR           glClientWaitSync;
   GLWaitSyncPTR                 glWaitSync;
   GLGetInteger64vPTR            glGetInteger64v;
   GLGetSyncivPTR                glGetSynciv;
   GLGetInteger64i_vPTR          glGetInteger64i_v;
   GLGetBufferParameteri64vPTR   glGetBufferParameteri64v;
   GLFramebufferTexturePTR       glFramebufferTexture;
   GLTexImage2DMultisamplePTR    glTexImage2DMultisample;
   GLTexImage3DMultisamplePTR    glTexImage3DMultisample;
   GLGetMultisamplefvPTR         glGetMultisamplefv;
   GLSampleMaskiPTR              glSampleMaski;
   // 3.3
   GLBindFragDataLocationIndexedPTR glBindFragDataLocationIndexed;
   GLGetFragDataIndexPTR         glGetFragDataIndex;
   GLGenSamplersPTR              glGenSamplers;
   GLDeleteSamplersPTR           glDeleteSamplers;
   GLIsSamplerPTR                glIsSampler;
   GLBindSamplerPTR              glBindSampler;   
   GLQueryCounterPTR             glQueryCounter;   
   GLVertexAttribDivisorPTR      glVertexAttribDivisor;   
   GLVertexP2uiPTR               glVertexP2ui;
   GLVertexP2uivPTR              glVertexP2uiv;
   GLVertexP3uiPTR               glVertexP3ui;
   GLVertexP3uivPTR              glVertexP3uiv;
   GLVertexP4uiPTR               glVertexP4ui;
   GLVertexP4uivPTR              glVertexP4uiv;
   GLMultiTexCoordP1uiPTR        glMultiTexCoordP1ui;
   GLMultiTexCoordP1uivPTR       glMultiTexCoordP1uiv;
   GLMultiTexCoordP2uiPTR        glMultiTexCoordP2ui;
   GLMultiTexCoordP2uivPTR       glMultiTexCoordP2uiv;
   GLMultiTexCoordP3uiPTR        glMultiTexCoordP3ui;
   GLMultiTexCoordP3uivPTR       glMultiTexCoordP3uiv;
   GLMultiTexCoordP4uiPTR        glMultiTexCoordP4ui;
   GLMultiTexCoordP4uivPTR       glMultiTexCoordP4uiv;   
   GLNormalP3uiPTR               glNormalP3ui;
   GLNormalP3uivPTR              glNormalP3uiv;
   GLColorP3uiPTR                glColorP3ui;
   GLColorP3uivPTR               glColorP3uiv;
   GLColorP4uiPTR                glColorP4ui;
   GLColorP4uivPTR               glColorP4uiv;
   GLSecondaryColorP3uiPTR       glSecondaryColorP3ui;
   GLSecondaryColorP3uivPTR      glSecondaryColorP3uiv;   
   // 4.0
   GLMinSampleShadingPTR         glMinSampleShading;
   GLBlendEquationiPTR           glBlendEquationi;
   GLBlendEquationSeparateiPTR   glBlendEquationSeparatei;
   GLBlendFunciPTR               glBlendFunci;
   GLBlendFuncSeparateiPTR       glBlendFuncSeparatei;
   GLDrawArraysIndirectPTR       glDrawArraysIndirect;
   GLDrawElementsIndirectPTR     glDrawElementsIndirect;
   
   GLGetUniformdvPTR             glGetUniformdv;
   GLGetSubroutineUniformLocationPTR glGetSubroutineUniformLocation;
   GLGetSubroutineIndexPTR       glGetSubroutineIndex;
   GLGetActiveSubroutineUniformivPTR   glGetActiveSubroutineUniformiv;
   GLGetActiveSubroutineUniformNamePTR glGetActiveSubroutineUniformName;
   GLGetActiveSubroutineNamePTR        glGetActiveSubroutineName;
   
   GLBindTransformFeedbackPTR    glBindTransformFeedback;
   GLDeleteTransformFeedbacksPTR glDeleteTransformFeedbacks;
   GLGenTransformFeedbacksPTR    glGenTransformFeedbacks;
   GLIsTransformFeedbackPTR      glIsTransformFeedback;
   GLPauseTransformFeedbackPTR   glPauseTransformFeedback;
   GLResumeTransformFeedbackPTR  glResumeTransformFeedback;
   GLDrawTransformFeedbackPTR    glDrawTransformFeedback;
   GLDrawTransformFeedbackStreamPTR glDrawTransformFeedbackStream;
   GLBeginQueryIndexedPTR        glBeginQueryIndexed;
   
   // ext
   GLMultiDrawArraysIndirectAMDPTR     glMultiDrawArraysIndirectAMD;
   GLMultiDrawElementsIndirectAMDPTR   glMultiDrawElementsIndirectAMD;   
   GLReleaseShaderCompilerPTR    glReleaseShaderCompiler;
   GLShaderBinaryPTR             glShaderBinary;
   GLGetShaderPrecisionFormatPTR glGetShaderPrecisionFormat;
   GLDepthRangefPTR              glDepthRangef;
   GLClearDepthfPTR              glClearDepthf;
   GLMemoryBarrierByRegionPTR    glMemoryBarrierByRegion;
   GLPrimitiveBoundingBoxARBPTR  glPrimitiveBoundingBoxARB;   
   GLDrawArraysInstancedBaseInstancePTR               glDrawArraysInstancedBaseInstance;
   GLDrawElementsInstancedBaseInstancePTR             glDrawElementsInstancedBaseInstance;
   GLDrawElementsInstancedBaseVertexBaseInstancePTR   glDrawElementsInstancedBaseVertexBaseInstance;
   GLBufferStoragePTR            glBufferStorage;
   GLClearBufferDataPTR          glClearBufferData;
   GLClearBufferSubDataPTR       glClearBufferSubData;
   GLClearTexImagePTR            glClearTexImage;
   GLClearTexSubImagePTR         glClearTexSubImage;
   GLClipControlPTR              glClipControl;      
   GLClampColorARBPTR            glClampColorARB;
   GLDispatchComputePTR          glDispatchCompute;
   GLDispatchComputeIndirectPTR  glDispatchComputeIndirect;
   GLDispatchComputeGroupSizeARBPTR    glDispatchComputeGroupSizeARB;
   GLCopyImageSubDataPTR         glCopyImageSubData;   
   GLDebugMessageControlARBPTR   glDebugMessageControlARB;
   GLDebugMessageInsertARBPTR    glDebugMessageInsertARB;
   GLDebugMessageCallbackARBPTR  glDebugMessageCallbackARB;
   GLGetDebugMessageLogARBPTR    glGetDebugMessageLogARB;
   GLCreateTransformFeedbacksPTR       glCreateTransformFeedbacks;
   GLTransformFeedbackBufferBasePTR    glTransformFeedbackBufferBase;
   GLTransformFeedbackBufferRangePTR   glTransformFeedbackBufferRange;
   GLCreateBuffersPTR            glCreateBuffers;
   GLNamedBufferStoragePTR       glNamedBufferStorage;
   GLNamedBufferDataPTR          glNamedBufferData;
   GLNamedBufferSubDataPTR       glNamedBufferSubData;
   GLCopyNamedBufferSubDataPTR   glCopyNamedBufferSubData;
   GLClearNamedBufferDataPTR     glClearNamedBufferData;
   GLClearNamedBufferSubDataPTR  glClearNamedBufferSubData;
   GLMapNamedBufferPTR           glMapNamedBuffer;
   GLMapNamedBufferRangePTR      glMapNamedBufferRange;
   GLUnmapNamedBufferPTR         glUnmapNamedBuffer;
   GLFlushMappedNamedBufferRangePTR    glFlushMappedNamedBufferRange;   
   GLGetNamedBufferPointervPTR         glGetNamedBufferPointerv;
   GLGetNamedBufferSubDataPTR          glGetNamedBufferSubData;
   GLCreateFramebuffersPTR             glCreateFramebuffers;
   GLNamedFramebufferRenderbufferPTR   glNamedFramebufferRenderbuffer;
   GLNamedFramebufferParameteriPTR     glNamedFramebufferParameteri;
   GLNamedFramebufferTexturePTR        glNamedFramebufferTexture;
   GLNamedFramebufferTextureLayerPTR   glNamedFramebufferTextureLayer;
   GLNamedFramebufferDrawBufferPTR     glNamedFramebufferDrawBuffer;
   GLNamedFramebufferDrawBuffersPTR    glNamedFramebufferDrawBuffers;
   GLNamedFramebufferReadBufferPTR     glNamedFramebufferReadBuffer;
   GLInvalidateNamedFramebufferDataPTR glInvalidateNamedFramebufferData;
   GLInvalidateNamedFramebufferSubDataPTR glInvalidateNamedFramebufferSubData;   
   GLBlitNamedFramebufferPTR           glBlitNamedFramebuffer;
   GLCheckNamedFramebufferStatusPTR    glCheckNamedFramebufferStatus;
   GLGetNamedFramebufferParameterivPTR glGetNamedFramebufferParameteriv;
   GLGetNamedFramebufferAttachmentParameterivPTR   glGetNamedFramebufferAttachmentParameteriv;
   GLCreateRenderbuffersPTR            glCreateRenderbuffers;
   GLNamedRenderbufferStoragePTR       glNamedRenderbufferStorage;
   GLNamedRenderbufferStorageMultisamplePTR     glNamedRenderbufferStorageMultisample;
   GLGetNamedRenderbufferParameterivPTR         glGetNamedRenderbufferParameteriv;
   GLCreateTexturesPTR                 glCreateTextures;
   GLTextureBufferPTR                  glTextureBuffer;
   GLTextureBufferRangePTR             glTextureBufferRange;
   GLTextureStorage1DPTR               glTextureStorage1D;
   GLTextureStorage2DPTR               glTextureStorage2D;
   GLTextureStorage3DPTR               glTextureStorage3D;
   GLTextureStorage2DMultisamplePTR    glTextureStorage2DMultisample;
   GLTextureStorage3DMultisamplePTR    glTextureStorage3DMultisample;
   GLTextureSubImage1DPTR              glTextureSubImage1D;
   GLTextureSubImage2DPTR              glTextureSubImage2D;
   GLTextureSubImage3DPTR              glTextureSubImage3D;
   GLCompressedTextureSubImage1DPTR    glCompressedTextureSubImage1D;
   GLCompressedTextureSubImage2DPTR    glCompressedTextureSubImage2D;
   GLCompressedTextureSubImage3DPTR    glCompressedTextureSubImage3D;
   GLCopyTextureSubImage1DPTR          glCopyTextureSubImage1D;
   GLCopyTextureSubImage2DPTR          glCopyTextureSubImage2D;
   GLCopyTextureSubImage3DPTR          glCopyTextureSubImage3D;
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_direct_state_access == 0) {
         ^ self
      };
      glCreateTransformFeedbacks := GLCreateTransformFeedbacksPTR.load(get_proc, "glCreateTransformFeedbacks");
      glTransformFeedbackBufferBase := GLTransformFeedbackBufferBasePTR.load(get_proc, "glTransformFeedbackBufferBase");
      glTransformFeedbackBufferRange := GLTransformFeedbackBufferRangePTR.load(get_proc, "glTransformFeedbackBufferRange");
      glCreateBuffers := GLCreateBuffersPTR.load(get_proc, "glCreateBuffers");
      glNamedBufferStorage := GLNamedBufferStoragePTR.load(get_proc, "glNamedBufferStorage");
      glNamedBufferData := GLNamedBufferDataPTR.load(get_proc, "glNamedBufferData");
      glNamedBufferSubData := GLNamedBufferSubDataPTR.load(get_proc, "glNamedBufferSubData");
      glCopyNamedBufferSubData := GLCopyNamedBufferSubDataPTR.load(get_proc, "glCopyNamedBufferSubData");
      glClearNamedBufferData := GLClearNamedBufferDataPTR.load(get_proc, "glClearNamedBufferData");
      glClearNamedBufferSubData := GLClearNamedBufferSubDataPTR.load(get_proc, "glClearNamedBufferSubData");
      glMapNamedBuffer := GLMapNamedBufferPTR.load(get_proc, "glMapNamedBuffer");
      glMapNamedBufferRange := GLMapNamedBufferRangePTR.load(get_proc, "glMapNamedBufferRange");
      glUnmapNamedBuffer := GLUnmapNamedBufferPTR.load(get_proc, "glUnmapNamedBuffer");
      glFlushMappedNamedBufferRange := GLFlushMappedNamedBufferRangePTR.load(get_proc, "glFlushMappedNamedBufferRange");
      glGetNamedBufferPointerv := GLGetNamedBufferPointervPTR.load(get_proc, "glGetNamedBufferPointerv");
      glGetNamedBufferSubData := GLGetNamedBufferSubDataPTR.load(get_proc, "glGetNamedBufferSubData");
      glCreateFramebuffers := GLCreateFramebuffersPTR.load(get_proc, "glCreateFramebuffers");
      glNamedFramebufferRenderbuffer := GLNamedFramebufferRenderbufferPTR.load(get_proc, "glNamedFramebufferRenderbuffer");
      glNamedFramebufferParameteri := GLNamedFramebufferParameteriPTR.load(get_proc, "glNamedFramebufferParameteri");
      glNamedFramebufferTexture := GLNamedFramebufferTexturePTR.load(get_proc, "glNamedFramebufferTexture");
      glNamedFramebufferTextureLayer := GLNamedFramebufferTextureLayerPTR.load(get_proc, "glNamedFramebufferTextureLayer");
      glNamedFramebufferDrawBuffer := GLNamedFramebufferDrawBufferPTR.load(get_proc, "glNamedFramebufferDrawBuffer");
      glNamedFramebufferDrawBuffers := GLNamedFramebufferDrawBuffersPTR.load(get_proc, "glNamedFramebufferDrawBuffers");
      glNamedFramebufferReadBuffer := GLNamedFramebufferReadBufferPTR.load(get_proc, "glNamedFramebufferReadBuffer");
      glInvalidateNamedFramebufferData := GLInvalidateNamedFramebufferDataPTR.load(get_proc, "glInvalidateNamedFramebufferData");
      glInvalidateNamedFramebufferSubData := GLInvalidateNamedFramebufferSubDataPTR.load(get_proc, "glInvalidateNamedFramebufferSubData");
      glBlitNamedFramebuffer := GLBlitNamedFramebufferPTR.load(get_proc, "glBlitNamedFramebuffer");
      glCheckNamedFramebufferStatus := GLCheckNamedFramebufferStatusPTR.load(get_proc, "glCheckNamedFramebufferStatus");
      glGetNamedFramebufferParameteriv := GLGetNamedFramebufferParameterivPTR.load(get_proc, "glGetNamedFramebufferParameteriv");
      glGetNamedFramebufferAttachmentParameteriv := GLGetNamedFramebufferAttachmentParameterivPTR.load(get_proc, "glGetNamedFramebufferAttachmentParameteriv");
      glCreateRenderbuffers := GLCreateRenderbuffersPTR.load(get_proc, "glCreateRenderbuffers");
      glNamedRenderbufferStorage := GLNamedRenderbufferStoragePTR.load(get_proc, "glNamedRenderbufferStorage");
      glNamedRenderbufferStorageMultisample := GLNamedRenderbufferStorageMultisamplePTR.load(get_proc, "glNamedRenderbufferStorageMultisample");
      glGetNamedRenderbufferParameteriv := GLGetNamedRenderbufferParameterivPTR.load(get_proc, "glGetNamedRenderbufferParameteriv");
      glCreateTextures := GLCreateTexturesPTR.load(get_proc, "glCreateTextures");
      glTextureBuffer := GLTextureBufferPTR.load(get_proc, "glTextureBuffer");
      glTextureBufferRange := GLTextureBufferRangePTR.load(get_proc, "glTextureBufferRange");
      glTextureStorage1D := GLTextureStorage1DPTR.load(get_proc, "glTextureStorage1D");
      glTextureStorage2D := GLTextureStorage2DPTR.load(get_proc, "glTextureStorage2D");
      glTextureStorage3D := GLTextureStorage3DPTR.load(get_proc, "glTextureStorage3D");
      glTextureStorage2DMultisample := GLTextureStorage2DMultisamplePTR.load(get_proc, "glTextureStorage2DMultisample");
      glTextureStorage3DMultisample := GLTextureStorage3DMultisamplePTR.load(get_proc, "glTextureStorage3DMultisample");
      glTextureSubImage1D := GLTextureSubImage1DPTR.load(get_proc, "glTextureSubImage1D");
      glTextureSubImage2D := GLTextureSubImage2DPTR.load(get_proc, "glTextureSubImage2D");
      glTextureSubImage3D := GLTextureSubImage3DPTR.load(get_proc, "glTextureSubImage3D");
      glCompressedTextureSubImage1D := GLCompressedTextureSubImage1DPTR.load(get_proc, "glCompressedTextureSubImage1D");
      glCompressedTextureSubImage2D := GLCompressedTextureSubImage2DPTR.load(get_proc, "glCompressedTextureSubImage2D");
      glCompressedTextureSubImage3D := GLCompressedTextureSubImage3DPTR.load(get_proc, "glCompressedTextureSubImage3D");
      glCopyTextureSubImage1D := GLCopyTextureSubImage1DPTR.load(get_proc, "glCopyTextureSubImage1D");
      glCopyTextureSubImage2D := GLCopyTextureSubImage2DPTR.load(get_proc, "glCopyTextureSubImage2D");
      glCopyTextureSubImage3D := GLCopyTextureSubImage3DPTR.load(get_proc, "glCopyTextureSubImage3D");
      
/*      
      glTextureParameterf = (PFNGLTEXTUREPARAMETERFPROC)load("glTextureParameterf");
      glTextureParameterfv = (PFNGLTEXTUREPARAMETERFVPROC)load("glTextureParameterfv");
      glTextureParameteri = (PFNGLTEXTUREPARAMETERIPROC)load("glTextureParameteri");
      glTextureParameterIiv = (PFNGLTEXTUREPARAMETERIIVPROC)load("glTextureParameterIiv");
      glTextureParameterIuiv = (PFNGLTEXTUREPARAMETERIUIVPROC)load("glTextureParameterIuiv");
      glTextureParameteriv = (PFNGLTEXTUREPARAMETERIVPROC)load("glTextureParameteriv");
      glGenerateTextureMipmap = (PFNGLGENERATETEXTUREMIPMAPPROC)load("glGenerateTextureMipmap");
      glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC)load("glBindTextureUnit");
      glGetTextureImage = (PFNGLGETTEXTUREIMAGEPROC)load("glGetTextureImage");
      glGetCompressedTextureImage = (PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC)load("glGetCompressedTextureImage");
      glGetTextureLevelParameterfv = (PFNGLGETTEXTURELEVELPARAMETERFVPROC)load("glGetTextureLevelParameterfv");
      glGetTextureLevelParameteriv = (PFNGLGETTEXTURELEVELPARAMETERIVPROC)load("glGetTextureLevelParameteriv");
      glGetTextureParameterfv = (PFNGLGETTEXTUREPARAMETERFVPROC)load("glGetTextureParameterfv");
      glGetTextureParameterIiv = (PFNGLGETTEXTUREPARAMETERIIVPROC)load("glGetTextureParameterIiv");
      glGetTextureParameterIuiv = (PFNGLGETTEXTUREPARAMETERIUIVPROC)load("glGetTextureParameterIuiv");
      glGetTextureParameteriv = (PFNGLGETTEXTUREPARAMETERIVPROC)load("glGetTextureParameteriv");
      glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)load("glCreateVertexArrays");
      glDisableVertexArrayAttrib = (PFNGLDISABLEVERTEXARRAYATTRIBPROC)load("glDisableVertexArrayAttrib");
      glEnableVertexArrayAttrib = (PFNGLENABLEVERTEXARRAYATTRIBPROC)load("glEnableVertexArrayAttrib");
      glVertexArrayElementBuffer = (PFNGLVERTEXARRAYELEMENTBUFFERPROC)load("glVertexArrayElementBuffer");
      glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)load("glVertexArrayVertexBuffer");
      glVertexArrayVertexBuffers = (PFNGLVERTEXARRAYVERTEXBUFFERSPROC)load("glVertexArrayVertexBuffers");
      glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)load("glVertexArrayAttribBinding");
      glVertexArrayAttribFormat = (PFNGLVERTEXARRAYATTRIBFORMATPROC)load("glVertexArrayAttribFormat");
      glVertexArrayAttribIFormat = (PFNGLVERTEXARRAYATTRIBIFORMATPROC)load("glVertexArrayAttribIFormat");
      glVertexArrayAttribLFormat = (PFNGLVERTEXARRAYATTRIBLFORMATPROC)load("glVertexArrayAttribLFormat");
      glVertexArrayBindingDivisor = (PFNGLVERTEXARRAYBINDINGDIVISORPROC)load("glVertexArrayBindingDivisor");
      glGetVertexArrayiv = (PFNGLGETVERTEXARRAYIVPROC)load("glGetVertexArrayiv");
      glGetVertexArrayIndexediv = (PFNGLGETVERTEXARRAYINDEXEDIVPROC)load("glGetVertexArrayIndexediv");
      glGetVertexArrayIndexed64iv = (PFNGLGETVERTEXARRAYINDEXED64IVPROC)load("glGetVertexArrayIndexed64iv");
      glCreateSamplers = (PFNGLCREATESAMPLERSPROC)load("glCreateSamplers");
      glCreateProgramPipelines = (PFNGLCREATEPROGRAMPIPELINESPROC)load("glCreateProgramPipelines");
      glCreateQueries = (PFNGLCREATEQUERIESPROC)load("glCreateQueries");
      glGetQueryBufferObjecti64v = (PFNGLGETQUERYBUFFEROBJECTI64VPROC)load("glGetQueryBufferObjecti64v");
      glGetQueryBufferObjectiv = (PFNGLGETQUERYBUFFEROBJECTIVPROC)load("glGetQueryBufferObjectiv");
      glGetQueryBufferObjectui64v = (PFNGLGETQUERYBUFFEROBJECTUI64VPROC)load("glGetQueryBufferObjectui64v");
      glGetQueryBufferObjectuiv = (PFNGLGETQUERYBUFFEROBJECTUIVPROC)load("glGetQueryBufferObjectuiv");
*/      
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_0 == 0) {
         ^ self
      };
      
      glCullFace := GLCullFacePTR.loadDirectly(libGL, "glCullFace");
      glFrontFace := GLFrontFacePTR.loadDirectly(libGL, "glFrontFace");
      glHint := GLHintPTR.loadDirectly(libGL, "glHint");
      glLineWidth := GLLineWidthPTR.loadDirectly(libGL, "glLineWidth");
      
      glPointSize := GLPointSizePTR.loadDirectly(libGL, "glPointSize");
      glPolygonMode := GLPolygonModePTR.loadDirectly(libGL, "glPolygonMode");
      glScissor := GLScissorPTR.loadDirectly(libGL, "glScissor");
      glTexImage1D := GLTexImage1DPTR.loadDirectly(libGL, "glTexImage1D");
      glTexImage2D := GLTexImage2DPTR.loadDirectly(libGL, "glTexImage2D");
      glDrawBuffer := GLDrawBufferPTR.loadDirectly(libGL, "glDrawBuffer");
      glClear := GLClearPTR.loadDirectly(libGL, "glClear");
      glClearColor := GLClearColorPTR.loadDirectly(libGL, "glClearColor");
      glClearStencil := GLClearStencilPTR.loadDirectly(libGL, "glClearStencil");
      glClearDepth := GLClearDepthPTR.loadDirectly(libGL, "glClearDepth");
      glStencilMask := GLStencilMaskPTR.loadDirectly(libGL, "glStencilMask");
      glColorMask := GLColorMaskPTR.loadDirectly(libGL, "glColorMask");
      glDepthMask := GLDepthMaskPTR.loadDirectly(libGL, "glDepthMask");
      glDisable := GLDisablePTR.loadDirectly(libGL, "glDisable");
      glEnable := GLEnablePTR.loadDirectly(libGL, "glEnable");
      glFinish := GLFinishPTR.loadDirectly(libGL, "glFinish");
      glFlush := GLFlushPTR.loadDirectly(libGL, "glFlush");
      glBlendFunc := GLBlendFuncPTR.loadDirectly(libGL, "glBlendFunc");
      glLogicOp := GLLogicOpPTR.loadDirectly(libGL, "glLogicOp");
      glStencilFunc := GLStencilFuncPTR.loadDirectly(libGL, "glStencilFunc");
      glStencilOp := GLStencilOpPTR.loadDirectly(libGL, "glStencilOp");
      glDepthFunc := GLDepthFuncPTR.loadDirectly(libGL, "glDepthFunc");
      glReadBuffer := GLReadBufferPTR.loadDirectly(libGL, "glReadBuffer");
      glReadPixels := GLReadPixelsPTR.loadDirectly(libGL, "glReadPixels");
      glGetBooleanv := GLGetBooleanvPTR.loadDirectly(libGL, "glGetBooleanv");
      glGetDoublev := GLGetDoublevPTR.loadDirectly(libGL, "glGetDoublev");
      glGetError := GLGetErrorPTR.loadDirectly(libGL, "glGetError"); 
      glGetFloatv := GLGetFloatvPTR.loadDirectly(libGL, "glGetFloatv");
      glGetIntegerv := GLGetIntegervPTR.loadDirectly(libGL, "glGetIntegerv");
//   glGetString := GLGetStringPTR.loadDirectly(libGL, "glGetString");
      glGetTexImage := GLGetTexImagePTR.loadDirectly(libGL, "glGetTexImage");
      glIsEnabled := GLIsEnabledPTR.loadDirectly(libGL, "glIsEnabled");
      glDepthRange := GLDepthRangePTR.loadDirectly(libGL, "glDepthRange");
      glViewport := GLViewportPTR.loadDirectly(libGL, "glViewport");
      glNewList := GLNewListPTR.loadDirectly(libGL, "glNewList");
      glEndList := GLEndListPTR.loadDirectly(libGL, "glEndList");
      glCallList := GLCallListPTR.loadDirectly(libGL, "glCallList");
      glCallLists := GLCallListsPTR.loadDirectly(libGL, "glCallLists");
      glDeleteLists := GLDeleteListsPTR.loadDirectly(libGL, "glDeleteLists");
      glGenLists := GLGenListsPTR.loadDirectly(libGL, "glGenLists");
      glListBase := GLListBasePTR.loadDirectly(libGL, "glListBase");
      glBegin := GLBeginPTR.loadDirectly(libGL, "glBegin");
      glBitmap := GLBitmapPTR.loadDirectly(libGL, "glBitmap");
      glEdgeFlag := GLEdgeFlagPTR.loadDirectly(libGL, "glEdgeFlag");
      glEdgeFlagv := GLEdgeFlagvPTR.loadDirectly(libGL, "glEdgeFlagv");
      glEnd := GLEndPTR.loadDirectly(libGL, "glEnd");
      glClipPlane := GLClipPlanePTR.loadDirectly(libGL, "glClipPlane");
      glColorMaterial := GLColorMaterialPTR.loadDirectly(libGL, "glColorMaterial");   
      glLineStipple := GLLineStipplePTR.loadDirectly(libGL, "glLineStipple"); 
      glPolygonStipple := GLPolygonStipplePTR.loadDirectly(libGL, "glPolygonStipple");
      glShadeModel := GLShadeModelPTR.loadDirectly(libGL, "glShadeModel");
      glFeedbackBuffer := GLFeedbackBufferPTR.loadDirectly(libGL, "glFeedbackBuffer");
      glSelectBuffer := GLSelectBufferPTR.loadDirectly(libGL, "glSelectBuffer");
      glRenderMode := GLRenderModePTR.loadDirectly(libGL, "glRenderMode");
      glInitNames := GLInitNamesPTR.loadDirectly(libGL, "glInitNames");
      glLoadName := GLLoadNamePTR.loadDirectly(libGL, "glLoadName");
      glPassThrough := GLPassThroughPTR.loadDirectly(libGL, "glPassThrough");
      glPopName := GLPopNamePTR.loadDirectly(libGL, "glPopName");
      glPushName := GLPushNamePTR.loadDirectly(libGL, "glPushName"); 
      glClearAccum := GLClearAccumPTR.loadDirectly(libGL, "glClearAccum");
      glClearIndex := GLClearIndexPTR.loadDirectly(libGL, "glClearIndex");
      glIndexMask := GLIndexMaskPTR.loadDirectly(libGL, "glIndexMask");
      glAccum := GLAccumPTR.loadDirectly(libGL, "glAccum");
      glPopAttrib := GLPopAttribPTR.loadDirectly(libGL, "glPopAttrib");
      glPushAttrib := GLPushAttribPTR.loadDirectly(libGL, "glPushAttrib");
      glEvalMesh1 := GLEvalMesh1PTR.loadDirectly(libGL, "glEvalMesh1");
      glEvalPoint1 := GLEvalPoint1PTR.loadDirectly(libGL, "glEvalPoint1");
      glEvalMesh2 := GLEvalMesh2PTR.loadDirectly(libGL, "glEvalMesh2");
      glEvalPoint2 := GLEvalPoint2PTR.loadDirectly(libGL, "glEvalPoint2");
      glAlphaFunc := GLAlphaFuncPTR.loadDirectly(libGL, "glAlphaFunc");
      glPixelZoom := GLPixelZoomPTR.loadDirectly(libGL, "glPixelZoom"); 
      glCopyPixels := GLCopyPixelsPTR.loadDirectly(libGL, "glCopyPixels");
      glDrawPixels := GLDrawPixelsPTR.loadDirectly(libGL, "glDrawPixels");
      glGetClipPlane := GLGetClipPlanePTR.loadDirectly(libGL, "glGetClipPlane"); 
      glGetPolygonStipple := GLGetPolygonStipplePTR.loadDirectly(libGL, "glGetPolygonStipple");
      glIsList := GLIsListPTR.loadDirectly(libGL, "glIsList");
      glFrustum := GLFrustumPTR.loadDirectly(libGL, "glFrustum");
      glLoadIdentity := GLLoadIdentityPTR.loadDirectly(libGL, "glLoadIdentity");
      glMatrixMode := GLMatrixModePTR.loadDirectly(libGL, "glMatrixMode");
      glOrtho := GLOrthoPTR.loadDirectly(libGL, "glOrtho");
      glPopMatrix := GLPopMatrixPTR.loadDirectly(libGL, "glPopMatrix");
      glPushMatrix := GLPushMatrixPTR.loadDirectly(libGL, "glPushMatrix");
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_1 == 0) {
         ^ self
      };
      
      glDrawArrays := GLDrawArraysPTR.loadDirectly(libGL, "glDrawArrays");
      glDrawElements := GLDrawElementsPTR.loadDirectly(libGL, "glDrawElements");
      glGetPointerv := GLGetPointervPTR.loadDirectly(libGL, "glGetPointerv");
      glPolygonOffset := GLPolygonOffsetPTR.loadDirectly(libGL, "glPolygonOffset");
      glCopyTexImage1D := GLCopyTexImage1DPTR.loadDirectly(libGL, "glCopyTexImage1D");
      glCopyTexImage2D := GLCopyTexImage2DPTR.loadDirectly(libGL, "glCopyTexImage2D");
      glCopyTexSubImage1D := GLCopyTexSubImage1DPTR.loadDirectly(libGL, "glCopyTexSubImage1D");
      glCopyTexSubImage2D := GLCopyTexSubImage2DPTR.loadDirectly(libGL, "glCopyTexSubImage2D");      
      glTexSubImage1D := GLTexSubImage1DPTR.loadDirectly(libGL, "glTexSubImage1D");
      glTexSubImage2D := GLTexSubImage2DPTR.loadDirectly(libGL, "glTexSubImage2D");
      glBindTexture := GLBindTexturePTR.loadDirectly(libGL, "glBindTexture");
      glDeleteTextures := GLDeleteTexturesPTR.loadDirectly(libGL, "glDeleteTextures");
      glGenTextures := GLGenTexturesPTR.loadDirectly(libGL, "glGenTextures");
      glIsTexture := GLIsTexturePTR.loadDirectly(libGL, "glIsTexture");
      glArrayElement := GLArrayElementPTR.loadDirectly(libGL, "glArrayElement");
      glColorPointer := GLColorPointerPTR.loadDirectly(libGL, "glColorPointer");
      glDisableClientState := GLDisableClientStatePTR.loadDirectly(libGL, "glDisableClientState");
      glEdgeFlagPointer := GLEdgeFlagPointerPTR.loadDirectly(libGL, "glEdgeFlagPointer");
      glEnableClientState := GLEnableClientStatePTR.loadDirectly(libGL, "glEnableClientState");
      glIndexPointer := GLIndexPointerPTR.loadDirectly(libGL, "glIndexPointer");
      glInterleavedArrays := GLInterleavedArraysPTR.loadDirectly(libGL, "glInterleavedArrays");
      glNormalPointer := GLNormalPointerPTR.loadDirectly(libGL, "glNormalPointer");
      glTexCoordPointer := GLTexCoordPointerPTR.loadDirectly(libGL, "glTexCoordPointer");
      glVertexPointer := GLVertexPointerPTR.loadDirectly(libGL, "glVertexPointer");
      glAreTexturesResident := GLAreTexturesResidentPTR.loadDirectly(libGL, "glAreTexturesResident");
      glPrioritizeTextures := GLPrioritizeTexturesPTR.loadDirectly(libGL, "glPrioritizeTextures");
      glIndexub := GLIndexubPTR.loadDirectly(libGL, "glIndexub");
      glIndexubv := GLIndexubvPTR.loadDirectly(libGL, "glIndexubv");
      glPopClientAttrib := GLPopClientAttribPTR.loadDirectly(libGL, "glPopClientAttrib");
      glPushClientAttrib := GLPushClientAttribPTR.loadDirectly(libGL, "glPushClientAttrib");
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_2 == 0) {
         ^ self
      };
      
      glDrawRangeElements := GLDrawRangeElementsPTR.loadDirectly(libGL, "glDrawRangeElements");
      glTexImage3D := GLTexImage3DPTR.loadDirectly(libGL, "glTexImage3D");
      glTexSubImage3D := GLTexSubImage3DPTR.loadDirectly(libGL, "glTexSubImage3D");
      glCopyTexSubImage3D := GLCopyTexSubImage3DPTR.loadDirectly(libGL, "glCopyTexSubImage3D");
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_3 == 0) {
         ^ self
      };
      
      glActiveTexture := GLActiveTexturePTR.loadDirectly(libGL, "glActiveTexture");
      glSampleCoverage := GLSampleCoveragePTR.loadDirectly(libGL, "glSampleCoverage");
      glCompressedTexImage3D := GLCompressedTexImage3DPTR.loadDirectly(libGL, "glCompressedTexImage3D");
      glCompressedTexImage2D := GLCompressedTexImage2DPTR.loadDirectly(libGL, "glCompressedTexImage2D");
      glCompressedTexImage1D := GLCompressedTexImage1DPTR.loadDirectly(libGL, "glCompressedTexImage1D");
      glCompressedTexSubImage3D := GLCompressedTexSubImage3DPTR.loadDirectly(libGL, "glCompressedTexSubImage3D");
      glCompressedTexSubImage2D := GLCompressedTexSubImage2DPTR.loadDirectly(libGL, "glCompressedTexSubImage2D");
      glCompressedTexSubImage1D := GLCompressedTexSubImage1DPTR.loadDirectly(libGL, "glCompressedTexSubImage1D");
      glGetCompressedTexImage := GLGetCompressedTexImagePTR.loadDirectly(libGL, "glGetCompressedTexImage");
      glClientActiveTexture := GLClientActiveTexturePTR.loadDirectly(libGL, "glClientActiveTexture");      
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_4 == 0) {
         ^ self
      };

      glBlendFuncSeparate := GLBlendFuncSeparatePTR.loadDirectly(libGL, "glBlendFuncSeparate");
      glMultiDrawArrays := GLMultiDrawArraysPTR.loadDirectly(libGL, "glMultiDrawArrays");
      glMultiDrawElements := GLMultiDrawElementsPTR.loadDirectly(libGL, "glMultiDrawElements");
      glFogCoordPointer := GLFogCoordPointerPTR.loadDirectly(libGL, "glFogCoordPointer");
      glSecondaryColorPointer := GLSecondaryColorPointerPTR.loadDirectly(libGL, "glSecondaryColorPointer");
      glBlendColor := GLBlendColorPTR.loadDirectly(libGL, "glBlendColor");
      glBlendEquation := GLBlendEquationPTR.loadDirectly(libGL, "glBlendEquation");
   }            

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_5 == 0) {
         ^ self
      };
      
      glGenQueries := GLGenQueriesPTR.loadDirectly(libGL, "glGenQueries");
      glDeleteQueries := GLDeleteQueriesPTR.loadDirectly(libGL, "glDeleteQueries");
      glIsQuery := GLIsQueryPTR.loadDirectly(libGL, "glIsQuery");
      glBeginQuery := GLBeginQueryPTR.loadDirectly(libGL, "glBeginQuery");
      glEndQuery := GLEndQueryPTR.loadDirectly(libGL, "glEndQuery");
      glGetQueryiv := GLGetQueryivPTR.loadDirectly(libGL, "glGetQueryiv");
      glBindBuffer := GLBindBufferPTR.loadDirectly(libGL, "glBindBuffer");      
      glDeleteBuffers := GLDeleteBuffersPTR.loadDirectly(libGL, "glDeleteBuffers");
      glGenBuffers := GLGenBuffersPTR.loadDirectly(libGL, "glGenBuffers");
      glIsBuffer := GLIsBufferPTR.loadDirectly(libGL, "glIsBuffer");
      glBufferData := GLBufferDataPTR.loadDirectly(libGL, "glBufferData");
      glBufferSubData := GLBufferSubDataPTR.loadDirectly(libGL, "glBufferSubData");
      glGetBufferSubData := GLGetBufferSubDataPTR.loadDirectly(libGL, "glGetBufferSubData");
      glMapBuffer := GLMapBufferPTR.loadDirectly(libGL, "glMapBuffer");
      glUnmapBuffer := GLUnmapBufferPTR.loadDirectly(libGL, "glUnmapBuffer");
      glGetBufferParameteriv := GLGetBufferParameterivPTR.loadDirectly(libGL, "glGetBufferParameteriv");
      glGetBufferPointerv := GLGetBufferPointervPTR.loadDirectly(libGL, "glGetBufferPointerv");
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_0 == 0) {
         ^ self
      };

      glBlendEquationSeparate := GLBlendEquationSeparatePTR.loadDirectly(libGL, "glBlendEquationSeparate");
      glDrawBuffers := GLDrawBuffersPTR.loadDirectly(libGL, "glDrawBuffers");
      glStencilOpSeparate := GLStencilOpSeparatePTR.loadDirectly(libGL, "glStencilOpSeparate");
      glStencilFuncSeparate := GLStencilFuncSeparatePTR.loadDirectly(libGL, "glStencilFuncSeparate");
      glStencilMaskSeparate := GLStencilMaskSeparatePTR.loadDirectly(libGL, "glStencilMaskSeparate");
      glAttachShader := GLAttachShaderPTR.loadDirectly(libGL, "glAttachShader");
      glBindAttribLocation := GLBindAttribLocationPTR.loadDirectly(libGL, "glBindAttribLocation");
      glCompileShader := GLCompileShaderPTR.loadDirectly(libGL, "glCompileShader");
      glCreateProgram := GLCreateProgramPTR.loadDirectly(libGL, "glCreateProgram");
      glCreateShader := GLCreateShaderPTR.loadDirectly(libGL, "glCreateShader");
      glDeleteProgram := GLDeleteProgramPTR.loadDirectly(libGL, "glDeleteProgram");
      glDeleteShader := GLDeleteShaderPTR.loadDirectly(libGL, "glDeleteShader");
      glDetachShader := GLDetachShaderPTR.loadDirectly(libGL, "glDetachShader");
      glDisableVertexAttribArray := GLDisableVertexAttribArrayPTR.loadDirectly(libGL, "glDisableVertexAttribArray");
      glEnableVertexAttribArray := GLEnableVertexAttribArrayPTR.loadDirectly(libGL, "glEnableVertexAttribArray");
      glGetActiveAttrib := GLGetActiveAttribPTR.loadDirectly(libGL, "glGetActiveAttrib");
      glGetActiveUniform := GLGetActiveUniformPTR.loadDirectly(libGL, "glGetActiveUniform");
      glGetAttachedShaders := GLGetAttachedShadersPTR.loadDirectly(libGL, "glGetAttachedShaders");
      glGetAttribLocation := GLGetAttribLocationPTR.loadDirectly(libGL, "glGetAttribLocation");
      glGetProgramiv := GLGetProgramivPTR.loadDirectly(libGL, "glGetProgramiv");
      glGetProgramInfoLog := GLGetProgramInfoLogPTR.loadDirectly(libGL, "glGetProgramInfoLog");
      glGetShaderiv := GLGetShaderivPTR.loadDirectly(libGL, "glGetShaderiv");
      glGetShaderInfoLog := GLGetShaderInfoLogPTR.loadDirectly(libGL, "glGetShaderInfoLog");
      glGetShaderSource := GLGetShaderSourcePTR.loadDirectly(libGL, "glGetShaderSource");      
      glGetUniformLocation := GLGetUniformLocationPTR.loadDirectly(libGL, "glGetUniformLocation");
      glGetVertexAttribPointerv := GLGetVertexAttribPointervPTR.loadDirectly(libGL, "glGetVertexAttribPointerv");
      glIsProgram := GLIsProgramPTR.loadDirectly(libGL, "glIsProgram");
      glIsShader := GLIsShaderPTR.loadDirectly(libGL, "glIsShader");
      glLinkProgram := GLLinkProgramPTR.loadDirectly(libGL, "glLinkProgram");
      glShaderSource := GLShaderSourcePTR.loadDirectly(libGL, "glShaderSource");      
      glUseProgram := GLUseProgramPTR.loadDirectly(libGL, "glUseProgram");      
      glValidateProgram := GLValidateProgramPTR.loadDirectly(libGL, "glValidateProgram");
      glVertexAttribPointer := GLVertexAttribPointerPTR.loadDirectly(libGL, "glVertexAttribPointer");
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_0 == 0) {
         ^ self
      };
      
      glColorMaski := GLColorMaskiPTR.loadDirectly(libGL, "glColorMaski");
      glGetBooleani_v := GLGetBooleani_vPTR.loadDirectly(libGL, "glGetBooleani_v");
      glGetIntegeri_v := GLGetIntegeri_vPTR.loadDirectly(libGL, "glGetIntegeri_v");
      glEnablei := GLEnableiPTR.loadDirectly(libGL, "glEnablei");
      glDisablei := GLDisableiPTR.loadDirectly(libGL, "glDisablei");
      glIsEnabledi := GLIsEnablediPTR.loadDirectly(libGL, "glIsEnabledi");
      glBeginTransformFeedback := GLBeginTransformFeedbackPTR.loadDirectly(libGL, "glBeginTransformFeedback");
      glEndTransformFeedback := GLEndTransformFeedbackPTR.loadDirectly(libGL, "glEndTransformFeedback");
      glBindBufferRange := GLBindBufferRangePTR.loadDirectly(libGL, "glBindBufferRange");
      glBindBufferBase := GLBindBufferBasePTR.loadDirectly(libGL, "glBindBufferBase");
      glTransformFeedbackVaryings := GLTransformFeedbackVaryingsPTR.loadDirectly(libGL, "glTransformFeedbackVaryings");
      glGetTransformFeedbackVarying := GLGetTransformFeedbackVaryingPTR.loadDirectly(libGL, "glGetTransformFeedbackVarying");
      glClampColor := GLClampColorPTR.loadDirectly(libGL, "glClampColor");      
      glBeginConditionalRender := GLBeginConditionalRenderPTR.loadDirectly(libGL, "glBeginConditionalRender");
      glEndConditionalRender := GLEndConditionalRenderPTR.loadDirectly(libGL, "glEndConditionalRender");
      glVertexAttribIPointer := GLVertexAttribIPointerPTR.loadDirectly(libGL, "glVertexAttribIPointer");
      glGetVertexAttribIiv := GLGetVertexAttribIivPTR.loadDirectly(libGL, "glGetVertexAttribIiv");
      glGetVertexAttribIuiv := GLGetVertexAttribIuivPTR.loadDirectly(libGL, "glGetVertexAttribIuiv");
      glGetUniformuiv := GLGetUniformuivPTR.loadDirectly(libGL, "glGetUniformuiv");
      glBindFragDataLocation := GLBindFragDataLocationPTR.loadDirectly(libGL, "glBindFragDataLocation");
      glGetFragDataLocation := GLGetFragDataLocationPTR.loadDirectly(libGL, "glGetFragDataLocation");
      glGetStringi := GLGetStringiPTR.loadDirectly(libGL, "glGetStringi");
      glIsRenderbuffer := GLIsRenderbufferPTR.loadDirectly(libGL, "glIsRenderbuffer");
      glBindRenderbuffer := GLBindRenderbufferPTR.loadDirectly(libGL, "glBindRenderbuffer");      
      glDeleteRenderbuffers := GLDeleteRenderbuffersPTR.loadDirectly(libGL, "glDeleteRenderbuffers");
      glGenRenderbuffers := GLGenRenderbuffersPTR.loadDirectly(libGL, "glGenRenderbuffers");
      glRenderbufferStorage := GLRenderbufferStoragePTR.loadDirectly(libGL, "glRenderbufferStorage");
      glGetRenderbufferParameteriv := GLGetRenderbufferParameterivPTR.loadDirectly(libGL, "glGetRenderbufferParameteriv");
      glIsFramebuffer := GLIsFramebufferPTR.loadDirectly(libGL, "glIsFramebuffer");
      glBindFramebuffer := GLBindFramebufferPTR.loadDirectly(libGL, "glBindFramebuffer");
      glDeleteFramebuffers := GLDeleteFramebuffersPTR.loadDirectly(libGL, "glDeleteFramebuffers");
      glGenFramebuffers := GLGenFramebuffersPTR.loadDirectly(libGL, "glGenFramebuffers");
      glCheckFramebufferStatus  := GLCheckFramebufferStatusPTR.loadDirectly(libGL, "glCheckFramebufferStatus");
      glFramebufferTexture1D := GLFramebufferTexture1DPTR.loadDirectly(libGL, "glFramebufferTexture1D");
      glFramebufferTexture2D := GLFramebufferTexture2DPTR.loadDirectly(libGL, "glFramebufferTexture2D");
      glFramebufferTexture3D := GLFramebufferTexture3DPTR.loadDirectly(libGL, "glFramebufferTexture3D");
      glFramebufferRenderbuffer := GLFramebufferRenderbufferPTR.loadDirectly(libGL, "glFramebufferRenderbuffer");
      glGetFramebufferAttachmentParameteriv := GLGetFramebufferAttachmentParameterivPTR.loadDirectly(libGL, "glGetFramebufferAttachmentParameteriv");
      glGenerateMipmap := GLGenerateMipmapPTR.loadDirectly(libGL, "glGenerateMipmap");
      glBlitFramebuffer := GLBlitFramebufferPTR.loadDirectly(libGL, "glBlitFramebuffer");
      glRenderbufferStorageMultisample := GLRenderbufferStorageMultisamplePTR.loadDirectly(libGL, "glRenderbufferStorageMultisample");
      glFramebufferTextureLayer := GLFramebufferTextureLayerPTR.loadDirectly(libGL, "glFramebufferTextureLayer");
      glMapBufferRange := GLMapBufferRangePTR.loadDirectly(libGL, "glMapBufferRange");
      glFlushMappedBufferRange := GLFlushMappedBufferRangePTR.loadDirectly(libGL, "glFlushMappedBufferRange");
      glBindVertexArray := GLBindVertexArrayPTR.loadDirectly(libGL, "glBindVertexArray");
      glDeleteVertexArrays := GLDeleteVertexArraysPTR.loadDirectly(libGL, "glDeleteVertexArrays");
      glGenVertexArrays := GLGenVertexArraysPTR.loadDirectly(libGL, "glGenVertexArrays");
      glIsVertexArray := GLIsVertexArrayPTR.loadDirectly(libGL, "glIsVertexArray");
      glTexCoordP1ui := GLTexCoordP1uiPTR.loadDirectly(libGL, "glTexCoordP1ui");
      glTexCoordP1uiv := GLTexCoordP1uivPTR.loadDirectly(libGL, "glTexCoordP1uiv");
      glTexCoordP2ui := GLTexCoordP2uiPTR.loadDirectly(libGL, "glTexCoordP2ui");
      glTexCoordP2uiv := GLTexCoordP2uivPTR.loadDirectly(libGL, "glTexCoordP2uiv");
      glTexCoordP3ui := GLTexCoordP3uiPTR.loadDirectly(libGL, "glTexCoordP3ui");
      glTexCoordP3uiv := GLTexCoordP3uivPTR.loadDirectly(libGL, "glTexCoordP3uiv");
      glTexCoordP4ui := GLTexCoordP4uiPTR.loadDirectly(libGL, "glTexCoordP4ui");
      glTexCoordP4uiv := GLTexCoordP4uivPTR.loadDirectly(libGL, "glTexCoordP4uiv");      
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_1 == 0) {
         ^ self
      };
      
      glDrawArraysInstanced := GLDrawArraysInstancedPTR.loadDirectly(libGL, "glDrawArraysInstanced");
      glDrawElementsInstanced := GLDrawElementsInstancedPTR.loadDirectly(libGL, "glDrawElementsInstanced");
      glTexBuffer := GLTexBufferPTR.loadDirectly(libGL, "glTexBuffer");
      glPrimitiveRestartIndex := GLPrimitiveRestartIndexPTR.loadDirectly(libGL, "glPrimitiveRestartIndex");
      glCopyBufferSubData := GLCopyBufferSubDataPTR.loadDirectly(libGL, "glCopyBufferSubData");
      glGetUniformIndices := GLGetUniformIndicesPTR.loadDirectly(libGL, "glGetUniformIndices");
      glGetActiveUniformsiv := GLGetActiveUniformsivPTR.loadDirectly(libGL, "glGetActiveUniformsiv");
      glGetActiveUniformName := GLGetActiveUniformNamePTR.loadDirectly(libGL, "glGetActiveUniformName");
      glGetUniformBlockIndex := GLGetUniformBlockIndexPTR.loadDirectly(libGL, "glGetUniformBlockIndex");
      glGetActiveUniformBlockiv := GLGetActiveUniformBlockivPTR.loadDirectly(libGL, "glGetActiveUniformBlockiv");
      glGetActiveUniformBlockName := GLGetActiveUniformBlockNamePTR.loadDirectly(libGL, "glGetActiveUniformBlockName");
      glUniformBlockBinding := GLUniformBlockBindingPTR.loadDirectly(libGL, "glUniformBlockBinding");
      /*glBindBufferRange := GLBindBufferRangePTR.loadDirectly(libGL, "glBindBufferRange");
      glBindBufferBase := GLBindBufferBasePTR.loadDirectly(libGL, "glBindBufferBase");
      glGetIntegeri_v := GLGetIntegeri_vPTR.loadDirectly(libGL, "glGetIntegeri_v");*/      
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_2 == 0) {
         ^ self
      };
      glDrawElementsBaseVertex := GLDrawElementsBaseVertexPTR.loadDirectly(libGL, "glDrawElementsBaseVertex");
      glDrawRangeElementsBaseVertex := GLDrawRangeElementsBaseVertexPTR.loadDirectly(libGL, "glDrawRangeElementsBaseVertex");
      glDrawElementsInstancedBaseVertex := GLDrawElementsInstancedBaseVertexPTR.loadDirectly(libGL, "glDrawElementsInstancedBaseVertex");
      glMultiDrawElementsBaseVertex := GLMultiDrawElementsBaseVertexPTR.loadDirectly(libGL, "glMultiDrawElementsBaseVertex");
      glProvokingVertex := GLProvokingVertexPTR.loadDirectly(libGL, "glProvokingVertex");
      glFenceSync := GLFenceSyncPTR.loadDirectly(libGL, "glFenceSync");
      glIsSync := GLIsSyncPTR.loadDirectly(libGL, "glIsSync");
      glDeleteSync := GLDeleteSyncPTR.loadDirectly(libGL, "glDeleteSync");
      glClientWaitSync := GLClientWaitSyncPTR.loadDirectly(libGL, "glClientWaitSync");
      glWaitSync := GLWaitSyncPTR.loadDirectly(libGL, "glWaitSync");
      glGetInteger64v := GLGetInteger64vPTR.loadDirectly(libGL, "glGetInteger64v");
      glGetSynciv := GLGetSyncivPTR.loadDirectly(libGL, "glGetSynciv");
      glGetInteger64i_v := GLGetInteger64i_vPTR.loadDirectly(libGL, "glGetInteger64i_v");
      glGetBufferParameteri64v := GLGetBufferParameteri64vPTR.loadDirectly(libGL, "glGetBufferParameteri64v");
      glFramebufferTexture := GLFramebufferTexturePTR.loadDirectly(libGL, "glFramebufferTexture");
      glTexImage2DMultisample := GLTexImage2DMultisamplePTR.loadDirectly(libGL, "glTexImage2DMultisample");
      glTexImage3DMultisample := GLTexImage3DMultisamplePTR.loadDirectly(libGL, "glTexImage3DMultisample");
      glGetMultisamplefv := GLGetMultisamplefvPTR.loadDirectly(libGL, "glGetMultisamplefv");
      glSampleMaski := GLSampleMaskiPTR.loadDirectly(libGL, "glSampleMaski");
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_3 == 0) {
         ^ self
      };
      
      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.loadDirectly(libGL, "glBindFragDataLocationIndexed");
      glGetFragDataIndex := GLGetFragDataIndexPTR.loadDirectly(libGL, "glGetFragDataIndex");
      glGenSamplers := GLGenSamplersPTR.loadDirectly(libGL, "glGenSamplers");
      glDeleteSamplers := GLDeleteSamplersPTR.loadDirectly(libGL, "glDeleteSamplers");
      glIsSampler := GLIsSamplerPTR.loadDirectly(libGL, "glIsSampler");
      glBindSampler := GLBindSamplerPTR.loadDirectly(libGL, "glBindSampler");
      glQueryCounter := GLQueryCounterPTR.loadDirectly(libGL, "glQueryCounter");
      glVertexAttribDivisor := GLVertexAttribDivisorPTR.loadDirectly(libGL, "glVertexAttribDivisor");
      glVertexAttribP1ui := GLVertexAttribP1uiPTR.loadDirectly(libGL, "glVertexAttribP1ui");
      glVertexAttribP1uiv := GLVertexAttribP1uivPTR.loadDirectly(libGL, "glVertexAttribP1uiv");
      glVertexAttribP2ui := GLVertexAttribP2uiPTR.loadDirectly(libGL, "glVertexAttribP2ui");
      glVertexAttribP2uiv := GLVertexAttribP2uivPTR.loadDirectly(libGL, "glVertexAttribP2uiv");
      glVertexAttribP3ui := GLVertexAttribP3uiPTR.loadDirectly(libGL, "glVertexAttribP3ui");
      glVertexAttribP3uiv := GLVertexAttribP3uivPTR.loadDirectly(libGL, "glVertexAttribP3uiv");
      glVertexAttribP4ui := GLVertexAttribP4uiPTR.loadDirectly(libGL, "glVertexAttribP4ui");
      glVertexAttribP4uiv := GLVertexAttribP4uivPTR.loadDirectly(libGL, "glVertexAttribP4uiv");      
      glVertexP2ui := GLVertexP2uiPTR.loadDirectly(libGL, "glVertexP2ui");
      glVertexP2uiv := GLVertexP2uivPTR.loadDirectly(libGL, "glVertexP2uiv");
      glVertexP3ui := GLVertexP3uiPTR.loadDirectly(libGL, "glVertexP3ui");
      glVertexP3uiv := GLVertexP3uivPTR.loadDirectly(libGL, "glVertexP3uiv");
      glVertexP4ui := GLVertexP4uiPTR.loadDirectly(libGL, "glVertexP4ui");
      glVertexP4uiv := GLVertexP4uivPTR.loadDirectly(libGL, "glVertexP4uiv");
      glMultiTexCoordP1ui := GLMultiTexCoordP1uiPTR.loadDirectly(libGL, "glMultiTexCoordP1ui");
      glMultiTexCoordP1uiv := GLMultiTexCoordP1uivPTR.loadDirectly(libGL, "glMultiTexCoordP1uiv");
      glMultiTexCoordP2ui := GLMultiTexCoordP2uiPTR.loadDirectly(libGL, "glMultiTexCoordP2ui");
      glMultiTexCoordP2uiv := GLMultiTexCoordP2uivPTR.loadDirectly(libGL, "glMultiTexCoordP2uiv");
      glMultiTexCoordP3ui := GLMultiTexCoordP3uiPTR.loadDirectly(libGL, "glMultiTexCoordP3ui");
      glMultiTexCoordP3uiv := GLMultiTexCoordP3uivPTR.loadDirectly(libGL, "glMultiTexCoordP3uiv");
      glMultiTexCoordP4ui := GLMultiTexCoordP4uiPTR.loadDirectly(libGL, "glMultiTexCoordP4ui");
      glMultiTexCoordP4uiv := GLMultiTexCoordP4uivPTR.loadDirectly(libGL, "glMultiTexCoordP4uiv");
      glNormalP3ui := GLNormalP3uiPTR.loadDirectly(libGL, "glNormalP3ui");
      glNormalP3uiv := GLNormalP3uivPTR.loadDirectly(libGL, "glNormalP3uiv");
      glColorP3ui := GLColorP3uiPTR.loadDirectly(libGL, "glColorP3ui");
      glColorP3uiv := GLColorP3uivPTR.loadDirectly(libGL, "glColorP3uiv");
      glColorP4ui := GLColorP4uiPTR.loadDirectly(libGL, "glColorP4ui");
      glColorP4uiv := GLColorP4uivPTR.loadDirectly(libGL, "glColorP4uiv");
      glSecondaryColorP3ui := GLSecondaryColorP3uiPTR.loadDirectly(libGL, "glSecondaryColorP3ui");
      glSecondaryColorP3uiv := GLSecondaryColorP3uivPTR.loadDirectly(libGL, "glSecondaryColorP3uiv");      
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_4_0 == 0) {
         ^ self
      };
      glMinSampleShading := GLMinSampleShadingPTR.loadDirectly(libGL, "glMinSampleShading");
      glBlendEquationi := GLBlendEquationiPTR.loadDirectly(libGL, "glBlendEquationi");
      glBlendEquationSeparatei := GLBlendEquationSeparateiPTR.loadDirectly(libGL, "glBlendEquationSeparatei");
      glBlendFunci := GLBlendFunciPTR.loadDirectly(libGL, "glBlendFunci");
      glBlendFuncSeparatei := GLBlendFuncSeparateiPTR.loadDirectly(libGL, "glBlendFuncSeparatei");
      glDrawArraysIndirect := GLDrawArraysIndirectPTR.loadDirectly(libGL, "glDrawArraysIndirect");
      glDrawElementsIndirect := GLDrawElementsIndirectPTR.loadDirectly(libGL, "glDrawElementsIndirect");
      glGetUniformdv := GLGetUniformdvPTR.loadDirectly(libGL, "glGetUniformdv");
      glGetSubroutineUniformLocation := GLGetSubroutineUniformLocationPTR.loadDirectly(libGL, "glGetSubroutineUniformLocation");
      glGetSubroutineIndex := GLGetSubroutineIndexPTR.loadDirectly(libGL, "glGetSubroutineIndex");
      glGetActiveSubroutineUniformiv := GLGetActiveSubroutineUniformivPTR.loadDirectly(libGL, "glGetActiveSubroutineUniformiv");
      glGetActiveSubroutineUniformName := GLGetActiveSubroutineUniformNamePTR.loadDirectly(libGL, "glGetActiveSubroutineUniformName");
      glGetActiveSubroutineName := GLGetActiveSubroutineNamePTR.loadDirectly(libGL, "glGetActiveSubroutineName");
      glBindTransformFeedback := GLBindTransformFeedbackPTR.loadDirectly(libGL, "glBindTransformFeedback");
      glDeleteTransformFeedbacks := GLDeleteTransformFeedbacksPTR.loadDirectly(libGL, "glDeleteTransformFeedbacks");
      glGenTransformFeedbacks := GLGenTransformFeedbacksPTR.loadDirectly(libGL, "glGenTransformFeedbacks");
      glIsTransformFeedback := GLIsTransformFeedbackPTR.loadDirectly(libGL, "glIsTransformFeedback");
      glPauseTransformFeedback := GLPauseTransformFeedbackPTR.loadDirectly(libGL, "glPauseTransformFeedback");
      glResumeTransformFeedback := GLResumeTransformFeedbackPTR.loadDirectly(libGL, "glResumeTransformFeedback");
      glDrawTransformFeedback := GLDrawTransformFeedbackPTR.loadDirectly(libGL, "glDrawTransformFeedback");
      glDrawTransformFeedbackStream := GLDrawTransformFeedbackStreamPTR.loadDirectly(libGL, "glDrawTransformFeedbackStream");
      glBeginQueryIndexed := GLBeginQueryIndexedPTR.loadDirectly(libGL, "glBeginQueryIndexed");
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
         ^ self
      };
      glMultiDrawArraysIndirectAMD := GLMultiDrawArraysIndirectAMDPTR.load(get_proc, "glMultiDrawArraysIndirectAMD");
      glMultiDrawElementsIndirectAMD := GLMultiDrawElementsIndirectAMDPTR.load(get_proc, "glMultiDrawElementsIndirectAMD");
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
         ^ self
      };
      glReleaseShaderCompiler := GLReleaseShaderCompilerPTR.load(get_proc, "glReleaseShaderCompiler");
      glShaderBinary := GLShaderBinaryPTR.load(get_proc, "glShaderBinary");
      glGetShaderPrecisionFormat := GLGetShaderPrecisionFormatPTR.load(get_proc, "glGetShaderPrecisionFormat");
      glDepthRangef := GLDepthRangefPTR.load(get_proc, "glDepthRangef");
      glClearDepthf := GLClearDepthfPTR.load(get_proc, "glClearDepthf");
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
         ^ self
      };
      glMemoryBarrierByRegion := GLMemoryBarrierByRegionPTR.load(get_proc, "glMemoryBarrierByRegion");
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
         ^ self
      };
      glPrimitiveBoundingBoxARB := GLPrimitiveBoundingBoxARBPTR.load(get_proc, "glPrimitiveBoundingBoxARB");
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_base_instance == 0) {
         ^ self
      };
      glDrawArraysInstancedBaseInstance := GLDrawArraysInstancedBaseInstancePTR.load(get_proc, "glDrawArraysInstancedBaseInstance");
      glDrawElementsInstancedBaseInstance := GLDrawElementsInstancedBaseInstancePTR.load(get_proc, "glDrawElementsInstancedBaseInstance");
      glDrawElementsInstancedBaseVertexBaseInstance := GLDrawElementsInstancedBaseVertexBaseInstancePTR.load(get_proc, "glDrawElementsInstancedBaseVertexBaseInstance");
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
         ^ self
      };
      glBufferStorage := GLBufferStoragePTR.load(get_proc, "glBufferStorage");
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
         ^ self
      };
      glClearBufferData := GLClearBufferDataPTR.load(get_proc, "glClearBufferData");
      glClearBufferSubData := GLClearBufferSubDataPTR.load(get_proc, "glClearBufferSubData");      
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_clear_texture == 0) {
         ^ self
      };
      glClearTexImage := GLClearTexImagePTR.load(get_proc, "glClearTexImage");
      glClearTexSubImage := GLClearTexSubImagePTR.load(get_proc, "glClearTexSubImage");      
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_clip_control == 0) {
         ^ self
      };
      glClipControl := GLClipControlPTR.load(get_proc, "glClipControl");
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
         ^ self
      };
      glClampColorARB := GLClampColorARBPTR.load(get_proc, "glClampColorARB");
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
      if (config.GLAD_GL_ARB_compute_shader == 0) {
         ^ self
      };
      glDispatchCompute := GLDispatchComputePTR.load(get_proc, "glDispatchCompute");
      glDispatchComputeIndirect := GLDispatchComputeIndirectPTR.load(get_proc, "glDispatchComputeIndirect");
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
         ^ self
      };
      glDispatchComputeGroupSizeARB := GLDispatchComputeGroupSizeARBPTR.load(get_proc, "glDispatchComputeGroupSizeARB");
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_copy_image == 0) {
         ^ self
      };
      glCopyImageSubData := GLCopyImageSubDataPTR.load(get_proc, "glCopyImageSubData");
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_debug_output == 0) {
         ^ self
      };
      glDebugMessageControlARB := GLDebugMessageControlARBPTR.load(get_proc, "glDebugMessageControlARB");
      glDebugMessageInsertARB := GLDebugMessageInsertARBPTR.load(get_proc, "glDebugMessageInsertARB");
      glDebugMessageCallbackARB := GLDebugMessageCallbackARBPTR.load(get_proc, "glDebugMessageCallbackARB");
      glGetDebugMessageLogARB := GLGetDebugMessageLogARBPTR.load(get_proc, "glGetDebugMessageLogARB");
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
/*
   	load_GL_3DFX_tbuffer(load);
   	load_GL_AMD_debug_output(load);
   	load_GL_AMD_draw_buffers_blend(load);
   	load_GL_AMD_framebuffer_multisample_advanced(load);
   	load_GL_AMD_framebuffer_sample_positions(load);
   	load_GL_AMD_gpu_shader_int64(load);
   	load_GL_AMD_interleaved_elements(load);
 */
      load_GL_AMD_multi_draw_indirect(config);
/*	   load_GL_AMD_name_gen_delete(load);
   	load_GL_AMD_occlusion_query_event(load);
   	load_GL_AMD_performance_monitor(load);
   	load_GL_AMD_sample_positions(load);
   	load_GL_AMD_sparse_texture(load);
   	load_GL_AMD_stencil_operation_extended(load);
   	load_GL_AMD_vertex_shader_tessellator(load);
   	load_GL_APPLE_element_array(load);
   	load_GL_APPLE_fence(load);
   	load_GL_APPLE_flush_buffer_range(load);
   	load_GL_APPLE_object_purgeable(load);
   	load_GL_APPLE_texture_range(load);
   	load_GL_APPLE_vertex_array_object(load);
   	load_GL_APPLE_vertex_array_range(load);
   	load_GL_APPLE_vertex_program_evaluators(load);*/
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
/*      load_GL_ARB_bindless_texture(load);*/
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
/*      load_GL_ARB_cl_event(load);*/
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
/*      load_GL_ARB_draw_buffers(load);
      load_GL_ARB_draw_buffers_blend(load);
      load_GL_ARB_draw_elements_base_vertex(load);
      load_GL_ARB_draw_indirect(load);
      load_GL_ARB_draw_instanced(load);
      load_GL_ARB_fragment_program(load);
      load_GL_ARB_framebuffer_no_attachments(load);
      load_GL_ARB_framebuffer_object(load);
      load_GL_ARB_geometry_shader4(load);
      load_GL_ARB_get_program_binary(load);
      load_GL_ARB_get_texture_sub_image(load);
      load_GL_ARB_gl_spirv(load);
      load_GL_ARB_gpu_shader_fp64(load);
      load_GL_ARB_gpu_shader_int64(load);
      load_GL_ARB_imaging(load);
      load_GL_ARB_indirect_parameters(load);
      load_GL_ARB_instanced_arrays(load);
      load_GL_ARB_internalformat_query(load);
      load_GL_ARB_internalformat_query2(load);
      load_GL_ARB_invalidate_subdata(load);
      load_GL_ARB_map_buffer_range(load);
      load_GL_ARB_matrix_palette(load);
      load_GL_ARB_multi_bind(load);
      load_GL_ARB_multi_draw_indirect(load);
      load_GL_ARB_multisample(load);
      load_GL_ARB_multitexture(load);
      load_GL_ARB_occlusion_query(load);
      load_GL_ARB_parallel_shader_compile(load);
      load_GL_ARB_point_parameters(load);
      load_GL_ARB_polygon_offset_clamp(load);
      load_GL_ARB_program_interface_query(load);
      load_GL_ARB_provoking_vertex(load);
      load_GL_ARB_robustness(load);
      load_GL_ARB_sample_locations(load);
      load_GL_ARB_sample_shading(load);
      load_GL_ARB_sampler_objects(load);
      load_GL_ARB_separate_shader_objects(load);
      load_GL_ARB_shader_atomic_counters(load);
      load_GL_ARB_shader_image_load_store(load);
      load_GL_ARB_shader_objects(load);
      load_GL_ARB_shader_storage_buffer_object(load);
      load_GL_ARB_shader_subroutine(load);
      load_GL_ARB_shading_language_include(load);
      load_GL_ARB_sparse_buffer(load);
      load_GL_ARB_sparse_texture(load);
      load_GL_ARB_sync(load);
      load_GL_ARB_tessellation_shader(load);
      load_GL_ARB_texture_barrier(load);
      load_GL_ARB_texture_buffer_object(load);
      load_GL_ARB_texture_buffer_range(load);
      load_GL_ARB_texture_compression(load);
      load_GL_ARB_texture_multisample(load);
      load_GL_ARB_texture_storage(load);
      load_GL_ARB_texture_storage_multisample(load);
      load_GL_ARB_texture_view(load);
      load_GL_ARB_timer_query(load);
      load_GL_ARB_transform_feedback2(load);
      load_GL_ARB_transform_feedback3(load);
      load_GL_ARB_transform_feedback_instanced(load);
      load_GL_ARB_transpose_matrix(load);
      load_GL_ARB_uniform_buffer_object(load);
      load_GL_ARB_vertex_array_object(load);
      load_GL_ARB_vertex_attrib_64bit(load);
      load_GL_ARB_vertex_attrib_binding(load);
      load_GL_ARB_vertex_blend(load);
      load_GL_ARB_vertex_buffer_object(load);
      load_GL_ARB_vertex_program(load);
      load_GL_ARB_vertex_shader(load);
      load_GL_ARB_vertex_type_2_10_10_10_rev(load);
      load_GL_ARB_viewport_array(load);
      load_GL_ARB_window_pos(load);
      load_GL_ATI_draw_buffers(load);
      load_GL_ATI_element_array(load);
      load_GL_ATI_envmap_bumpmap(load);
      load_GL_ATI_fragment_shader(load);
      load_GL_ATI_map_object_buffer(load);
      load_GL_ATI_pn_triangles(load);
      load_GL_ATI_separate_stencil(load);
      load_GL_ATI_vertex_array_object(load);
      load_GL_ATI_vertex_attrib_array_object(load);
      load_GL_ATI_vertex_streams(load);
      load_GL_EXT_EGL_image_storage(load);
      load_GL_EXT_bindable_uniform(load);
      load_GL_EXT_blend_color(load);
      load_GL_EXT_blend_equation_separate(load);
      load_GL_EXT_blend_func_separate(load);
      load_GL_EXT_blend_minmax(load);
      load_GL_EXT_color_subtable(load);
      load_GL_EXT_compiled_vertex_array(load);
      load_GL_EXT_convolution(load);
      load_GL_EXT_coordinate_frame(load);
      load_GL_EXT_copy_texture(load);
      load_GL_EXT_cull_vertex(load);
      load_GL_EXT_debug_label(load);
      load_GL_EXT_debug_marker(load);
      load_GL_EXT_depth_bounds_test(load);
      load_GL_EXT_direct_state_access(load);
      load_GL_EXT_draw_buffers2(load);
      load_GL_EXT_draw_instanced(load);
      load_GL_EXT_draw_range_elements(load);
      load_GL_EXT_external_buffer(load);
      load_GL_EXT_fog_coord(load);
      load_GL_EXT_framebuffer_blit(load);
      load_GL_EXT_framebuffer_multisample(load);
      load_GL_EXT_framebuffer_object(load);
      load_GL_EXT_geometry_shader4(load);
      load_GL_EXT_gpu_program_parameters(load);
      load_GL_EXT_gpu_shader4(load);
      load_GL_EXT_histogram(load);
      load_GL_EXT_index_func(load);
      load_GL_EXT_index_material(load);
      load_GL_EXT_light_texture(load);
      load_GL_EXT_memory_object(load);
      load_GL_EXT_memory_object_fd(load);
      load_GL_EXT_memory_object_win32(load);
      load_GL_EXT_multi_draw_arrays(load);
      load_GL_EXT_multisample(load);
      load_GL_EXT_paletted_texture(load);
      load_GL_EXT_pixel_transform(load);
      load_GL_EXT_point_parameters(load);
      load_GL_EXT_polygon_offset(load);
      load_GL_EXT_polygon_offset_clamp(load);
      load_GL_EXT_provoking_vertex(load);
      load_GL_EXT_raster_multisample(load);
      load_GL_EXT_secondary_color(load);
      load_GL_EXT_semaphore(load);
      load_GL_EXT_semaphore_fd(load);
      load_GL_EXT_semaphore_win32(load);
      load_GL_EXT_separate_shader_objects(load);
      load_GL_EXT_shader_framebuffer_fetch_non_coherent(load);
      load_GL_EXT_shader_image_load_store(load);
      load_GL_EXT_stencil_clear_tag(load);
      load_GL_EXT_stencil_two_side(load);
      load_GL_EXT_subtexture(load);
      load_GL_EXT_texture3D(load);
      load_GL_EXT_texture_array(load);
      load_GL_EXT_texture_buffer_object(load);
      load_GL_EXT_texture_integer(load);
      load_GL_EXT_texture_object(load);
      load_GL_EXT_texture_perturb_normal(load);
      load_GL_EXT_timer_query(load);
      load_GL_EXT_transform_feedback(load);
      load_GL_EXT_vertex_array(load);
      load_GL_EXT_vertex_attrib_64bit(load);
      load_GL_EXT_vertex_shader(load);
      load_GL_EXT_vertex_weighting(load);
      load_GL_EXT_win32_keyed_mutex(load);
      load_GL_EXT_window_rectangles(load);
      load_GL_EXT_x11_sync_object(load);
      load_GL_GREMEDY_frame_terminator(load);
      load_GL_GREMEDY_string_marker(load);
      load_GL_HP_image_transform(load);
      load_GL_IBM_multimode_draw_arrays(load);
      load_GL_IBM_static_data(load);
      load_GL_IBM_vertex_array_lists(load);
      load_GL_INGR_blend_func_separate(load);
      load_GL_INTEL_framebuffer_CMAA(load);
      load_GL_INTEL_map_texture(load);
      load_GL_INTEL_parallel_arrays(load);
      load_GL_INTEL_performance_query(load);
      load_GL_KHR_blend_equation_advanced(load);
      load_GL_KHR_debug(load);
      load_GL_KHR_parallel_shader_compile(load);
      load_GL_KHR_robustness(load);
      load_GL_MESA_framebuffer_flip_y(load);
      load_GL_MESA_resize_buffers(load);
      load_GL_MESA_window_pos(load);
      load_GL_NVX_conditional_render(load);
      load_GL_NVX_gpu_multicast2(load);
      load_GL_NVX_linked_gpu_multicast(load);
      load_GL_NVX_progress_fence(load);
      load_GL_NV_alpha_to_coverage_dither_control(load);
      load_GL_NV_bindless_multi_draw_indirect(load);
      load_GL_NV_bindless_multi_draw_indirect_count(load);
      load_GL_NV_bindless_texture(load);
      load_GL_NV_blend_equation_advanced(load);
      load_GL_NV_clip_space_w_scaling(load);
      load_GL_NV_command_list(load);
      load_GL_NV_conditional_render(load);
      load_GL_NV_conservative_raster(load);
      load_GL_NV_conservative_raster_dilate(load);
      load_GL_NV_conservative_raster_pre_snap_triangles(load);
      load_GL_NV_copy_image(load);
      load_GL_NV_depth_buffer_float(load);
      load_GL_NV_draw_texture(load);
      load_GL_NV_draw_vulkan_image(load);
      load_GL_NV_evaluators(load);
      load_GL_NV_explicit_multisample(load);
      load_GL_NV_fence(load);
      load_GL_NV_fragment_coverage_to_color(load);
      load_GL_NV_fragment_program(load);
      load_GL_NV_framebuffer_mixed_samples(load);
      load_GL_NV_framebuffer_multisample_coverage(load);
      load_GL_NV_geometry_program4(load);
      load_GL_NV_gpu_multicast(load);
      load_GL_NV_gpu_program4(load);
      load_GL_NV_gpu_program5(load);
      load_GL_NV_gpu_shader5(load);
      load_GL_NV_half_float(load);
      load_GL_NV_internalformat_sample_query(load);
      load_GL_NV_memory_attachment(load);
      load_GL_NV_memory_object_sparse(load);
      load_GL_NV_mesh_shader(load);
      load_GL_NV_occlusion_query(load);
      load_GL_NV_parameter_buffer_object(load);
      load_GL_NV_path_rendering(load);
      load_GL_NV_pixel_data_range(load);
      load_GL_NV_point_sprite(load);
      load_GL_NV_present_video(load);
      load_GL_NV_primitive_restart(load);
      load_GL_NV_query_resource(load);
      load_GL_NV_query_resource_tag(load);
      load_GL_NV_register_combiners(load);
      load_GL_NV_register_combiners2(load);
      load_GL_NV_sample_locations(load);
      load_GL_NV_scissor_exclusive(load);
      load_GL_NV_shader_buffer_load(load);
      load_GL_NV_shading_rate_image(load);
      load_GL_NV_texture_barrier(load);
      load_GL_NV_texture_multisample(load);
      load_GL_NV_timeline_semaphore(load);
      load_GL_NV_transform_feedback(load);
      load_GL_NV_transform_feedback2(load);
      load_GL_NV_vdpau_interop(load);
      load_GL_NV_vdpau_interop2(load);
      load_GL_NV_vertex_array_range(load);
      load_GL_NV_vertex_attrib_integer_64bit(load);
      load_GL_NV_vertex_buffer_unified_memory(load);
      load_GL_NV_vertex_program(load);
      load_GL_NV_vertex_program4(load);
      load_GL_NV_video_capture(load);
      load_GL_NV_viewport_swizzle(load);
      load_GL_OES_byte_coordinates(load);
      load_GL_OES_fixed_point(load);
      load_GL_OES_query_matrix(load);
      load_GL_OES_single_precision(load);
      load_GL_OVR_multiview(load);
      load_GL_PGI_misc_hints(load);
      load_GL_SGIS_detail_texture(load);
      load_GL_SGIS_fog_function(load);
      load_GL_SGIS_multisample(load);
      load_GL_SGIS_pixel_texture(load);
      load_GL_SGIS_point_parameters(load);
      load_GL_SGIS_sharpen_texture(load);
      load_GL_SGIS_texture4D(load);
      load_GL_SGIS_texture_color_mask(load);
      load_GL_SGIS_texture_filter4(load);
      load_GL_SGIX_async(load);
      load_GL_SGIX_flush_raster(load);
      load_GL_SGIX_fragment_lighting(load);
      load_GL_SGIX_framezoom(load);
      load_GL_SGIX_igloo_interface(load);
      load_GL_SGIX_instruments(load);
      load_GL_SGIX_list_priority(load);
      load_GL_SGIX_pixel_texture(load);
      load_GL_SGIX_polynomial_ffd(load);
      load_GL_SGIX_reference_plane(load);
      load_GL_SGIX_sprite(load);
      load_GL_SGIX_tag_sample_buffer(load);
      load_GL_SGI_color_table(load);
      load_GL_SUNX_constant_data(load);
      load_GL_SUN_global_alpha(load);
      load_GL_SUN_mesh_array(load);
      load_GL_SUN_triangle_list(load);
      load_GL_SUN_vertex(load);
         
*/      
      ^ true
   }   
   
   initCanvas(GLADCanvas canvas)
   {
      canvas.initColorFns(glClearColor, glClear);
   }
      
   GLGetStringPTR GLGetString
      = glGetString;
}

public struct GLAD_WrapperD
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   GLColor3dPTR         glColor3d;
   GLColor3dvPTR        glColor3dv;
   GLColor4dPTR         glColor4d;
   GLColor4dvPTR        glColor4dv;
   GLIndexdPTR          glIndexd;
   GLIndexdvPTR         glIndexdv;
   GLNormal3dPTR        glNormal3d;
   GLNormal3dvPTR       glNormal3dv;
   GLRasterPos2dPTR     glRasterPos2d;
   GLRasterPos2dvPTR    glRasterPos2dv;
   GLRasterPos3dPTR     glRasterPos3d;
   GLRasterPos3dvPTR    glRasterPos3dv;
   GLRasterPos4dPTR     glRasterPos4d;
   GLRasterPos4dvPTR    glRasterPos4dv;
   GLRectdPTR           glRectd;
   GLRectdvPTR          glRectdv;
   GLTexCoord1dPTR      glTexCoord1d;
   GLTexCoord1dvPTR     glTexCoord1dv;
   GLTexCoord2dPTR      glTexCoord2d;
   GLTexCoord2dvPTR     glTexCoord2dv;
   GLTexCoord3dPTR      glTexCoord3d;
   GLTexCoord3dvPTR     glTexCoord3dv;
   GLTexCoord4dPTR      glTexCoord4d;
   GLTexCoord4dvPTR     glTexCoord4dv;
   GLVertex2dPTR        glVertex2d;
   GLVertex2dvPTR       glVertex2dv;
   GLVertex3dPTR        glVertex3d;
   GLVertex3dvPTR       glVertex3dv;
   GLVertex4dPTR        glVertex4d;
   GLVertex4dvPTR       glVertex4dv;
   GLTexGendPTR         glTexGend;
   GLTexGendvPTR        glTexGendv;
   GLMap1dPTR           glMap1d;
   GLMap2dPTR           glMap2d;
   GLMapGrid1dPTR       glMapGrid1d;
   GLMapGrid2dPTR       glMapGrid2d;
   GLEvalCoord1dPTR     glEvalCoord1d;
   GLEvalCoord1dvPTR    glEvalCoord1dv;
   GLEvalCoord2dPTR     glEvalCoord2d;
   GLEvalCoord2dvPTR    glEvalCoord2dv;
   GLGetMapdvPTR        glGetMapdv;
   GLGetTexGendvPTR     glGetTexGendv;
   GLLoadMatrixdPTR     glLoadMatrixd;
   GLMultMatrixdPTR     glMultMatrixd;
   GLRotatedPTR         glRotated;
   GLScaledPTR          glScaled;
   GLTranslatedPTR      glTranslated;
   // 1.3
   GLMultiTexCoord1dPTR       glMultiTexCoord1d;
   GLMultiTexCoord1dvPTR      glMultiTexCoord1dv;
   GLMultiTexCoord2dPTR       glMultiTexCoord2d;
   GLMultiTexCoord2dvPTR      glMultiTexCoord2dv;
   GLMultiTexCoord3dPTR       glMultiTexCoord3d;
   GLMultiTexCoord3dvPTR      glMultiTexCoord3dv;
   GLMultiTexCoord4dPTR       glMultiTexCoord4d;
   GLMultiTexCoord4dvPTR      glMultiTexCoord4dv;
   GLLoadTransposeMatrixdPTR  glLoadTransposeMatrixd;
   GLMultTransposeMatrixdPTR  glMultTransposeMatrixd;
   // 1.4
   GLFogCoorddPTR             glFogCoordd;
   GLFogCoorddvPTR            glFogCoorddv;
   GLSecondaryColor3dPTR      glSecondaryColor3d;
   GLSecondaryColor3dvPTR     glSecondaryColor3dv;
   GLWindowPos2dPTR           glWindowPos2d;
   GLWindowPos2dvPTR          glWindowPos2dv;
   GLWindowPos3dPTR           glWindowPos3d;
   GLWindowPos3dvPTR          glWindowPos3dv;
   // 2.0
   GLGetVertexAttribdvPTR     glGetVertexAttribdv;
   GLVertexAttrib1dPTR        glVertexAttrib1d;
   GLVertexAttrib1dvPTR       glVertexAttrib1dv;
   GLVertexAttrib2dPTR        glVertexAttrib2d;
   GLVertexAttrib2dvPTR       glVertexAttrib2dv;
   GLVertexAttrib3dPTR        glVertexAttrib3d;
   GLVertexAttrib3dvPTR       glVertexAttrib3dv;
   GLVertexAttrib4dPTR        glVertexAttrib4d;
   GLVertexAttrib4dvPTR       glVertexAttrib4dv;
   // 3.0
   // 3.3
   // 4.0
   GLUniform1dPTR                glUniform1d;
   GLUniform2dPTR                glUniform2d;
   GLUniform3dPTR                glUniform3d;
   GLUniform4dPTR                glUniform4d;
   GLUniform1dvPTR               glUniform1dv;
   GLUniform2dvPTR               glUniform2dv;
   GLUniform3dvPTR               glUniform3dv;
   GLUniform4dvPTR               glUniform4dv;
   GLUniformMatrix2dvPTR         glUniformMatrix2dv;
   GLUniformMatrix3dvPTR         glUniformMatrix3dv;
   GLUniformMatrix4dvPTR         glUniformMatrix4dv;
   GLUniformMatrix2x3dvPTR       glUniformMatrix2x3dv;
   GLUniformMatrix2x4dvPTR       glUniformMatrix2x4dv;
   GLUniformMatrix3x2dvPTR       glUniformMatrix3x2dv;
   GLUniformMatrix3x4dvPTR       glUniformMatrix3x4dv;
   GLUniformMatrix4x2dvPTR       glUniformMatrix4x2dv;
   GLUniformMatrix4x3dvPTR       glUniformMatrix4x3dv;   
   // ext
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_direct_state_access == 0) {
//         ^ self
//      };
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_0 == 0) {
         ^ self
      };
      
      glColor3d := GLColor3dPTR.loadDirectly(libGL, "glColor3d");
      glColor3dv := GLColor3dvPTR.loadDirectly(libGL, "glColor3dv");
      glColor4d := GLColor4dPTR.loadDirectly(libGL, "glColor4d");
      glColor4dv := GLColor4dvPTR.loadDirectly(libGL, "glColor4dv");
      glIndexd := GLIndexdPTR.loadDirectly(libGL, "glIndexd");
      glIndexdv := GLIndexdvPTR.loadDirectly(libGL, "glIndexdv");
      glNormal3d := GLNormal3dPTR.loadDirectly(libGL, "glNormal3d");
      glNormal3dv := GLNormal3dvPTR.loadDirectly(libGL, "glNormal3dv");
      glRasterPos2d := GLRasterPos2dPTR.loadDirectly(libGL, "glRasterPos2d");
      glRasterPos2dv := GLRasterPos2dvPTR.loadDirectly(libGL, "glRasterPos2dv");
      glRasterPos3d := GLRasterPos3dPTR.loadDirectly(libGL, "glRasterPos3d");
      glRasterPos3dv := GLRasterPos3dvPTR.loadDirectly(libGL, "glRasterPos3dv");
      glRasterPos4d := GLRasterPos4dPTR.loadDirectly(libGL, "glRasterPos4d");
      glRasterPos4dv := GLRasterPos4dvPTR.loadDirectly(libGL, "glRasterPos4dv");
      glRectd := GLRectdPTR.loadDirectly(libGL, "glRectd");
      glRectdv := GLRectdvPTR.loadDirectly(libGL, "glRectdv");
      glTexCoord1d := GLTexCoord1dPTR.loadDirectly(libGL, "glTexCoord1d");
      glTexCoord1dv := GLTexCoord1dvPTR.loadDirectly(libGL, "glTexCoord1dv");
      glTexCoord2d := GLTexCoord2dPTR.loadDirectly(libGL, "glTexCoord2d");
      glTexCoord2dv := GLTexCoord2dvPTR.loadDirectly(libGL, "glTexCoord2dv");
      glTexCoord3d := GLTexCoord3dPTR.loadDirectly(libGL, "glTexCoord3d");
      glTexCoord3dv := GLTexCoord3dvPTR.loadDirectly(libGL, "glTexCoord3dv");
      glTexCoord4d := GLTexCoord4dPTR.loadDirectly(libGL, "glTexCoord4d");
      glTexCoord4dv := GLTexCoord4dvPTR.loadDirectly(libGL, "glTexCoord4dv");
      glVertex2d := GLVertex2dPTR.loadDirectly(libGL, "glVertex2d");
      glVertex2dv := GLVertex2dvPTR.loadDirectly(libGL, "glVertex2dv");
      glVertex3d := GLVertex3dPTR.loadDirectly(libGL, "glVertex3d");
      glVertex3dv := GLVertex3dvPTR.loadDirectly(libGL, "glVertex3dv");
      glVertex4d := GLVertex4dPTR.loadDirectly(libGL, "glVertex4d");
      glVertex4dv := GLVertex4dvPTR.loadDirectly(libGL, "glVertex4dv");
      glTexGend := GLTexGendPTR.loadDirectly(libGL, "glTexGend");
      glTexGendv := GLTexGendvPTR.loadDirectly(libGL, "glTexGendv");
      glMap1d := GLMap1dPTR.loadDirectly(libGL, "glMap1d");
      glMap2d := GLMap2dPTR.loadDirectly(libGL, "glMap2d");
      glMapGrid1d := GLMapGrid1dPTR.loadDirectly(libGL, "glMapGrid1d");
      glMapGrid2d := GLMapGrid2dPTR.loadDirectly(libGL, "glMapGrid2d");
      glEvalCoord1d := GLEvalCoord1dPTR.loadDirectly(libGL, "glEvalCoord1d");
      glEvalCoord1dv := GLEvalCoord1dvPTR.loadDirectly(libGL, "glEvalCoord1dv");
      glEvalCoord2d := GLEvalCoord2dPTR.loadDirectly(libGL, "glEvalCoord2d");
      glEvalCoord2dv := GLEvalCoord2dvPTR.loadDirectly(libGL, "glEvalCoord2dv");
      glGetMapdv := GLGetMapdvPTR.loadDirectly(libGL, "glGetMapdv");
      glGetTexGendv := GLGetTexGendvPTR.loadDirectly(libGL, "glGetTexGendv");
      glLoadMatrixd := GLLoadMatrixdPTR.loadDirectly(libGL, "glLoadMatrixd");
      glMultMatrixd := GLMultMatrixdPTR.loadDirectly(libGL, "glMultMatrixd");
      glRotated := GLRotatedPTR.loadDirectly(libGL, "glRotated");
      glScaled := GLScaledPTR.loadDirectly(libGL, "glScaled");
      glTranslated := GLTranslatedPTR.loadDirectly(libGL, "glTranslated");
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_2 == 0) {
//         ^ self
//      };
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_3 == 0) {
         ^ self
      };
      
      glMultiTexCoord1d := GLMultiTexCoord1dPTR.loadDirectly(libGL, "glMultiTexCoord1d");
      glMultiTexCoord1dv := GLMultiTexCoord1dvPTR.loadDirectly(libGL, "glMultiTexCoord1dv");
      glMultiTexCoord2d := GLMultiTexCoord2dPTR.loadDirectly(libGL, "glMultiTexCoord2d");
      glMultiTexCoord2dv := GLMultiTexCoord2dvPTR.loadDirectly(libGL, "glMultiTexCoord2dv");
      glMultiTexCoord3d := GLMultiTexCoord3dPTR.loadDirectly(libGL, "glMultiTexCoord3d");
      glMultiTexCoord3dv := GLMultiTexCoord3dvPTR.loadDirectly(libGL, "glMultiTexCoord3dv");
      glMultiTexCoord4d := GLMultiTexCoord4dPTR.loadDirectly(libGL, "glMultiTexCoord4d");
      glMultiTexCoord4dv := GLMultiTexCoord4dvPTR.loadDirectly(libGL, "glMultiTexCoord4dv");
      glLoadTransposeMatrixd := GLLoadTransposeMatrixdPTR.loadDirectly(libGL, "glLoadTransposeMatrixd");
      glMultTransposeMatrixd := GLMultTransposeMatrixdPTR.loadDirectly(libGL, "glMultTransposeMatrixd");
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_4 == 0) {
         ^ self
      };

      glFogCoordd := GLFogCoorddPTR.loadDirectly(libGL, "glFogCoordd");
      glFogCoorddv := GLFogCoorddvPTR.loadDirectly(libGL, "glFogCoorddv");
      glSecondaryColor3d := GLSecondaryColor3dPTR.loadDirectly(libGL, "glSecondaryColor3d");
      glSecondaryColor3dv := GLSecondaryColor3dvPTR.loadDirectly(libGL, "glSecondaryColor3dv");
      glWindowPos2d := GLWindowPos2dPTR.loadDirectly(libGL, "glWindowPos2d");
      glWindowPos2dv := GLWindowPos2dvPTR.loadDirectly(libGL, "glWindowPos2dv");
      glWindowPos3d := GLWindowPos3dPTR.loadDirectly(libGL, "glWindowPos3d");
      glWindowPos3dv := GLWindowPos3dvPTR.loadDirectly(libGL, "glWindowPos3dv");
   }            

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_5 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_0 == 0) {
         ^ self
      };

      glGetVertexAttribdv := GLGetVertexAttribdvPTR.loadDirectly(libGL, "glGetVertexAttribdv");
      glVertexAttrib1d := GLVertexAttrib1dPTR.loadDirectly(libGL, "glVertexAttrib1d");
      glVertexAttrib1dv := GLVertexAttrib1dvPTR.loadDirectly(libGL, "glVertexAttrib1dv");
      glVertexAttrib2d := GLVertexAttrib2dPTR.loadDirectly(libGL, "glVertexAttrib2d");
      glVertexAttrib2dv := GLVertexAttrib2dvPTR.loadDirectly(libGL, "glVertexAttrib2dv");
      glVertexAttrib3d := GLVertexAttrib3dPTR.loadDirectly(libGL, "glVertexAttrib3d");
      glVertexAttrib3dv := GLVertexAttrib3dvPTR.loadDirectly(libGL, "glVertexAttrib3dv");
      glVertexAttrib4d := GLVertexAttrib4dPTR.loadDirectly(libGL, "glVertexAttrib4d");
      glVertexAttrib4dv := GLVertexAttrib4dvPTR.loadDirectly(libGL, "glVertexAttrib4dv");
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_2 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_3 == 0) {
//         ^ self
//      };      
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_4_0 == 0) {
         ^ self
      };
      glUniform1d := GLUniform1dPTR.loadDirectly(libGL, "glUniform1d");
      glUniform2d := GLUniform2dPTR.loadDirectly(libGL, "glUniform2d");
      glUniform3d := GLUniform3dPTR.loadDirectly(libGL, "glUniform3d");
      glUniform4d := GLUniform4dPTR.loadDirectly(libGL, "glUniform4d");
      glUniform1dv := GLUniform1dvPTR.loadDirectly(libGL, "glUniform1dv");
      glUniform2dv := GLUniform2dvPTR.loadDirectly(libGL, "glUniform2dv");
      glUniform3dv := GLUniform3dvPTR.loadDirectly(libGL, "glUniform3dv");
      glUniform4dv := GLUniform4dvPTR.loadDirectly(libGL, "glUniform4dv");
      glUniformMatrix2dv := GLUniformMatrix2dvPTR.loadDirectly(libGL, "glUniformMatrix2dv");
      glUniformMatrix3dv := GLUniformMatrix3dvPTR.loadDirectly(libGL, "glUniformMatrix3dv");
      glUniformMatrix4dv := GLUniformMatrix4dvPTR.loadDirectly(libGL, "glUniformMatrix4dv");
      glUniformMatrix2x3dv := GLUniformMatrix2x3dvPTR.loadDirectly(libGL, "glUniformMatrix2x3dv");
      glUniformMatrix2x4dv := GLUniformMatrix2x4dvPTR.loadDirectly(libGL, "glUniformMatrix2x4dv");
      glUniformMatrix3x2dv := GLUniformMatrix3x2dvPTR.loadDirectly(libGL, "glUniformMatrix3x2dv");
      glUniformMatrix3x4dv := GLUniformMatrix3x4dvPTR.loadDirectly(libGL, "glUniformMatrix3x4dv");
      glUniformMatrix4x2dv := GLUniformMatrix4x2dvPTR.loadDirectly(libGL, "glUniformMatrix4x2dv");
      glUniformMatrix4x3dv := GLUniformMatrix4x3dvPTR.loadDirectly(libGL, "glUniformMatrix4x3dv");
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
//      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_base_instance == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_texture == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clip_control == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
//      if (config.GLAD_GL_ARB_compute_shader == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_copy_image == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_debug_output == 0) {
//         ^ self
//      };
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
      load_GL_AMD_multi_draw_indirect(config);
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
 
      ^ true
   }      
}

public struct GLAD_WrapperF
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   GLTexParameterfPTR   glTexParameterf;
   GLTexParameterfvPTR  glTexParameterfv;
   GLPixelStorefPTR     glPixelStoref;
   GLGetTexParameterfvPTR        glGetTexParameterfv;
   GLGetTexLevelParameterfvPTR   glGetTexLevelParameterfv;
   GLColor3fPTR         glColor3f;
   GLColor3fvPTR        glColor3fv;
   GLColor4fPTR         glColor4f;
   GLColor4fvPTR        glColor4fv;
   GLIndexfPTR          glIndexf;
   GLIndexfvPTR         glIndexfv;
   GLNormal3fPTR        glNormal3f;
   GLNormal3fvPTR       glNormal3fv;
   GLRasterPos2fPTR     glRasterPos2f;
   GLRasterPos2fvPTR    glRasterPos2fv;
   GLRasterPos3fPTR     glRasterPos3f;
   GLRasterPos3fvPTR    glRasterPos3fv;
   GLRasterPos4fPTR     glRasterPos4f;
   GLRasterPos4fvPTR    glRasterPos4fv;
   GLRectfPTR           glRectf;
   GLRectfvPTR          glRectfv;
   GLTexCoord1fPTR      glTexCoord1f;
   GLTexCoord1fvPTR     glTexCoord1fv;
   GLTexCoord2fPTR      glTexCoord2f;
   GLTexCoord2fvPTR     glTexCoord2fv;
   GLTexCoord3fPTR      glTexCoord3f;
   GLTexCoord3fvPTR     glTexCoord3fv;
   GLTexCoord4fPTR      glTexCoord4f;
   GLTexCoord4fvPTR     glTexCoord4fv;
   GLVertex2fPTR        glVertex2f;
   GLVertex2fvPTR       glVertex2fv;
   GLVertex3fPTR        glVertex3f;
   GLVertex3fvPTR       glVertex3fv;
   GLVertex4fPTR        glVertex4f;
   GLVertex4fvPTR       glVertex4fv;
   GLFogfPTR            glFogf;
   GLFogfvPTR           glFogfv;
   GLLightfPTR          glLightf;
   GLLightfvPTR         glLightfv;
   GLLightModelfPTR     glLightModelf;
   GLLightModelfvPTR    glLightModelfv;
   GLMaterialfPTR       glMaterialf;
   GLMaterialfvPTR      glMaterialfv;   
   GLTexEnvfPTR         glTexEnvf;
   GLTexEnvfvPTR        glTexEnvfv;
   GLTexGenfPTR         glTexGenf;
   GLTexGenfvPTR        glTexGenfv;
   GLMap1fPTR           glMap1f;
   GLMap2fPTR           glMap2f;
   GLMapGrid1fPTR       glMapGrid1f;
   GLMapGrid2fPTR       glMapGrid2f;
   GLEvalCoord1fPTR     glEvalCoord1f;
   GLEvalCoord1fvPTR    glEvalCoord1fv;
   GLEvalCoord2fPTR     glEvalCoord2f;
   GLEvalCoord2fvPTR    glEvalCoord2fv;
   GLPixelTransferfPTR  glPixelTransferf;
   GLPixelMapfvPTR      glPixelMapfv;
   GLGetLightfvPTR      glGetLightfv;
   GLGetMapfvPTR        glGetMapfv;
   GLGetMaterialfvPTR   glGetMaterialfv;
   GLGetPixelMapfvPTR   glGetPixelMapfv;
   GLGetTexEnvfvPTR     glGetTexEnvfv;
   GLGetTexGenfvPTR     glGetTexGenfv;
   GLLoadMatrixfPTR     glLoadMatrixf;
   GLMultMatrixfPTR     glMultMatrixf;
   GLRotatefPTR         glRotatef;
   GLScalefPTR          glScalef;
   GLTranslatefPTR      glTranslatef;
   // 1.3
   GLMultiTexCoord1fPTR       glMultiTexCoord1f;
   GLMultiTexCoord1fvPTR      glMultiTexCoord1fv;
   GLMultiTexCoord2fPTR       glMultiTexCoord2f;
   GLMultiTexCoord2fvPTR      glMultiTexCoord2fv;
   GLMultiTexCoord3fPTR       glMultiTexCoord3f;
   GLMultiTexCoord3fvPTR      glMultiTexCoord3fv;
   GLMultiTexCoord4fPTR       glMultiTexCoord4f;
   GLMultiTexCoord4fvPTR      glMultiTexCoord4fv;
   GLLoadTransposeMatrixfPTR  glLoadTransposeMatrixf;
   GLMultTransposeMatrixfPTR  glMultTransposeMatrixf;   
   // 1.4
   GLPointParameterfPTR       glPointParameterf;
   GLPointParameterfvPTR      glPointParameterfv;
   GLFogCoordfPTR             glFogCoordf;
   GLFogCoordfvPTR            glFogCoordfv;
   GLSecondaryColor3fPTR      glSecondaryColor3f;
   GLSecondaryColor3fvPTR     glSecondaryColor3fv;
   GLWindowPos2fPTR           glWindowPos2f;
   GLWindowPos2fvPTR          glWindowPos2fv;
   GLWindowPos3fPTR           glWindowPos3f;
   GLWindowPos3fvPTR          glWindowPos3fv;   
   // 2.0
   GLGetUniformfvPTR          glGetUniformfv;
   GLGetVertexAttribfvPTR     glGetVertexAttribfv;
   GLUniform1fPTR             glUniform1f;
   GLUniform2fPTR             glUniform2f;
   GLUniform3fPTR             glUniform3f;
   GLUniform4fPTR             glUniform4f;
   GLUniform1fvPTR            glUniform1fv;
   GLUniform2fvPTR            glUniform2fv;
   GLUniform3fvPTR            glUniform3fv;
   GLUniform4fvPTR            glUniform4fv;
   GLVertexAttrib1fPTR        glVertexAttrib1f;
   GLVertexAttrib1fvPTR       glVertexAttrib1fv;
   GLVertexAttrib2fPTR        glVertexAttrib2f;
   GLVertexAttrib2fvPTR       glVertexAttrib2fv;
   GLVertexAttrib3fPTR        glVertexAttrib3f;
   GLVertexAttrib3fvPTR       glVertexAttrib3fv;
   GLVertexAttrib4fPTR        glVertexAttrib4f;
   GLVertexAttrib4fvPTR       glVertexAttrib4fv;
   GLUniformMatrix2fvPTR      glUniformMatrix2fv;
   GLUniformMatrix3fvPTR      glUniformMatrix3fv;
   GLUniformMatrix4fvPTR      glUniformMatrix4fv;         
   // 2.1
   GLUniformMatrix2x3fvPTR    glUniformMatrix2x3fv;
   GLUniformMatrix3x2fvPTR    glUniformMatrix3x2fv;
   GLUniformMatrix2x4fvPTR    glUniformMatrix2x4fv;
   GLUniformMatrix4x2fvPTR    glUniformMatrix4x2fv;
   GLUniformMatrix3x4fvPTR    glUniformMatrix3x4fv;
   GLUniformMatrix4x3fvPTR    glUniformMatrix4x3fv;             
   // 3.0
   GLClearBufferfvPTR         glClearBufferfv;
   GLClearBufferfiPTR         glClearBufferfi;
   // 3.3
   GLSamplerParameterfPTR        glSamplerParameterf;
   GLSamplerParameterfvPTR       glSamplerParameterfv;
   GLGetSamplerParameterfvPTR    glGetSamplerParameterfv;
   GLGetSamplerParameterIuivPTR  glGetSamplerParameterIuiv;
   // 4.0
   GLPatchParameterfvPTR         glPatchParameterfv;
   // ext
   GLClearNamedFramebufferfvPTR        glClearNamedFramebufferfv;
   GLClearNamedFramebufferfiPTR        glClearNamedFramebufferfi;
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_direct_state_access == 0) {
         ^ self
      };
      glClearNamedFramebufferfv := GLClearNamedFramebufferfvPTR.load(get_proc, "glClearNamedFramebufferfv");
      glClearNamedFramebufferfi := GLClearNamedFramebufferfiPTR.load(get_proc, "glClearNamedFramebufferfi");
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_0 == 0) {
         ^ self
      };
      
      glTexParameterf := GLTexParameterfPTR.loadDirectly(libGL, "glTexParameterf");
      glTexParameterfv := GLTexParameterfvPTR.loadDirectly(libGL, "glTexParameterfv");
      glPixelStoref := GLPixelStorefPTR.loadDirectly(libGL, "glPixelStoref");
      glGetTexParameterfv := GLGetTexParameterfvPTR.loadDirectly(libGL, "glGetTexParameterfv");
      glGetTexLevelParameterfv := GLGetTexLevelParameterfvPTR.loadDirectly(libGL, "glGetTexLevelParameterfv");
      glColor3f := GLColor3fPTR.loadDirectly(libGL, "glColor3f");
      glColor3fv := GLColor3fvPTR.loadDirectly(libGL, "glColor3fv");
      glColor4f := GLColor4fPTR.loadDirectly(libGL, "glColor4f");
      glColor4fv := GLColor4fvPTR.loadDirectly(libGL, "glColor4fv");
      glIndexf := GLIndexfPTR.loadDirectly(libGL, "glIndexf");
      glIndexfv := GLIndexfvPTR.loadDirectly(libGL, "glIndexfv");
      glNormal3f := GLNormal3fPTR.loadDirectly(libGL, "glNormal3f");
      glNormal3fv := GLNormal3fvPTR.loadDirectly(libGL, "glNormal3fv");
      glRasterPos2f := GLRasterPos2fPTR.loadDirectly(libGL, "glRasterPos2f");
      glRasterPos2fv := GLRasterPos2fvPTR.loadDirectly(libGL, "glRasterPos2fv");
      glRasterPos3f := GLRasterPos3fPTR.loadDirectly(libGL, "glRasterPos3f");
      glRasterPos3fv := GLRasterPos3fvPTR.loadDirectly(libGL, "glRasterPos3fv");
      glRasterPos4f := GLRasterPos4fPTR.loadDirectly(libGL, "glRasterPos4f");
      glRasterPos4fv := GLRasterPos4fvPTR.loadDirectly(libGL, "glRasterPos4fv");
      glRectf := GLRectfPTR.loadDirectly(libGL, "glRectf");
      glRectfv := GLRectfvPTR.loadDirectly(libGL, "glRectfv");
      glTexCoord1f := GLTexCoord1fPTR.loadDirectly(libGL, "glTexCoord1f");
      glTexCoord1fv := GLTexCoord1fvPTR.loadDirectly(libGL, "glTexCoord1fv");
      glTexCoord2f := GLTexCoord2fPTR.loadDirectly(libGL, "glTexCoord2f");
      glTexCoord2fv := GLTexCoord2fvPTR.loadDirectly(libGL, "glTexCoord2fv");
      glTexCoord3f := GLTexCoord3fPTR.loadDirectly(libGL, "glTexCoord3f");
      glTexCoord3fv := GLTexCoord3fvPTR.loadDirectly(libGL, "glTexCoord3fv");
      glTexCoord4f := GLTexCoord4fPTR.loadDirectly(libGL, "glTexCoord4f");
      glTexCoord4fv := GLTexCoord4fvPTR.loadDirectly(libGL, "glTexCoord4fv");
      glVertex2f := GLVertex2fPTR.loadDirectly(libGL, "glVertex2f");
      glVertex2fv := GLVertex2fvPTR.loadDirectly(libGL, "glVertex2fv");
      glVertex3f := GLVertex3fPTR.loadDirectly(libGL, "glVertex3f");
      glVertex3fv := GLVertex3fvPTR.loadDirectly(libGL, "glVertex3fv");
      glVertex4f := GLVertex4fPTR.loadDirectly(libGL, "glVertex4f");
      glVertex4fv := GLVertex4fvPTR.loadDirectly(libGL, "glVertex4fv");
      glFogf := GLFogfPTR.loadDirectly(libGL, "glFogf");
      glFogfv := GLFogfvPTR.loadDirectly(libGL, "glFogfv");
      glLightf := GLLightfPTR.loadDirectly(libGL, "glLightf");
      glLightfv := GLLightfvPTR.loadDirectly(libGL, "glLightfv");
      glLightModelf := GLLightModelfPTR.loadDirectly(libGL, "glLightModelf");
      glLightModelfv := GLLightModelfvPTR.loadDirectly(libGL, "glLightModelfv");
      glMaterialf := GLMaterialfPTR.loadDirectly(libGL, "glMaterialf");
      glMaterialfv := GLMaterialfvPTR.loadDirectly(libGL, "glMaterialfv");
      glTexEnvf := GLTexEnvfPTR.loadDirectly(libGL, "glTexEnvf");
      glTexEnvfv := GLTexEnvfvPTR.loadDirectly(libGL, "glTexEnvfv");
      glTexGenf := GLTexGenfPTR.loadDirectly(libGL, "glTexGenf");
      glTexGenfv := GLTexGenfvPTR.loadDirectly(libGL, "glTexGenfv");
      glMap1f := GLMap1fPTR.loadDirectly(libGL, "glMap1f");
      glMap2f := GLMap2fPTR.loadDirectly(libGL, "glMap2f");
      glMapGrid1f := GLMapGrid1fPTR.loadDirectly(libGL, "glMapGrid1f");
      glMapGrid2f := GLMapGrid2fPTR.loadDirectly(libGL, "glMapGrid2f"); 
      glEvalCoord1f := GLEvalCoord1fPTR.loadDirectly(libGL, "glEvalCoord1f");
      glEvalCoord1fv := GLEvalCoord1fvPTR.loadDirectly(libGL, "glEvalCoord1fv");
      glEvalCoord2f := GLEvalCoord2fPTR.loadDirectly(libGL, "glEvalCoord2f");
      glEvalCoord2fv := GLEvalCoord2fvPTR.loadDirectly(libGL, "glEvalCoord2fv");
      glPixelTransferf := GLPixelTransferfPTR.loadDirectly(libGL, "glPixelTransferf");
      glPixelMapfv := GLPixelMapfvPTR.loadDirectly(libGL, "glPixelMapfv");
      glGetLightfv := GLGetLightfvPTR.loadDirectly(libGL, "glGetLightfv");
      glGetMapfv := GLGetMapfvPTR.loadDirectly(libGL, "glGetMapfv");
      glGetMaterialfv := GLGetMaterialfvPTR.loadDirectly(libGL, "glGetMaterialfv");
      glGetPixelMapfv := GLGetPixelMapfvPTR.loadDirectly(libGL, "glGetPixelMapfv");
      glGetTexEnvfv := GLGetTexEnvfvPTR.loadDirectly(libGL, "glGetTexEnvfv");
      glGetTexGenfv := GLGetTexGenfvPTR.loadDirectly(libGL, "glGetTexGenfv");
      glLoadMatrixf := GLLoadMatrixfPTR.loadDirectly(libGL, "glLoadMatrixf");
      glMultMatrixf := GLMultMatrixfPTR.loadDirectly(libGL, "glMultMatrixf");
      glRotatef := GLRotatefPTR.loadDirectly(libGL, "glRotatef");
      glScalef := GLScalefPTR.loadDirectly(libGL, "glScalef");
      glTranslatef := GLTranslatefPTR.loadDirectly(libGL, "glTranslatef");
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_2 == 0) {
//         ^ self
//      };
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_3 == 0) {
         ^ self
      };
      
      glMultiTexCoord1f := GLMultiTexCoord1fPTR.loadDirectly(libGL, "glMultiTexCoord1f");
      glMultiTexCoord1fv := GLMultiTexCoord1fvPTR.loadDirectly(libGL, "glMultiTexCoord1fv");
      glMultiTexCoord2f := GLMultiTexCoord2fPTR.loadDirectly(libGL, "glMultiTexCoord2f");
      glMultiTexCoord2fv := GLMultiTexCoord2fvPTR.loadDirectly(libGL, "glMultiTexCoord2fv");
      glMultiTexCoord3f := GLMultiTexCoord3fPTR.loadDirectly(libGL, "glMultiTexCoord3f");
      glMultiTexCoord3fv := GLMultiTexCoord3fvPTR.loadDirectly(libGL, "glMultiTexCoord3fv");
      glMultiTexCoord4f := GLMultiTexCoord4fPTR.loadDirectly(libGL, "glMultiTexCoord4f");
      glMultiTexCoord4fv := GLMultiTexCoord4fvPTR.loadDirectly(libGL, "glMultiTexCoord4fv");
      glLoadTransposeMatrixf := GLLoadTransposeMatrixfPTR.loadDirectly(libGL, "glLoadTransposeMatrixf");
      glMultTransposeMatrixf := GLMultTransposeMatrixfPTR.loadDirectly(libGL, "glMultTransposeMatrixf");
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_4 == 0) {
         ^ self
      };

      glPointParameterf := GLPointParameterfPTR.loadDirectly(libGL, "glPointParameterf");
      glPointParameterfv := GLPointParameterfvPTR.loadDirectly(libGL, "glPointParameterfv");
      glFogCoordf := GLFogCoordfPTR.loadDirectly(libGL, "glFogCoordf");
      glFogCoordfv := GLFogCoordfvPTR.loadDirectly(libGL, "glFogCoordfv");
      glSecondaryColor3f := GLSecondaryColor3fPTR.loadDirectly(libGL, "glSecondaryColor3f");
      glSecondaryColor3fv := GLSecondaryColor3fvPTR.loadDirectly(libGL, "glSecondaryColor3fv");
      glWindowPos2f := GLWindowPos2fPTR.loadDirectly(libGL, "glWindowPos2f");
      glWindowPos2fv := GLWindowPos2fvPTR.loadDirectly(libGL, "glWindowPos2fv");
      glWindowPos3f := GLWindowPos3fPTR.loadDirectly(libGL, "glWindowPos3f");
      glWindowPos3fv := GLWindowPos3fvPTR.loadDirectly(libGL, "glWindowPos3fv");
   }            

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_5 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_0 == 0) {
         ^ self
      };

      glGetUniformfv := GLGetUniformfvPTR.loadDirectly(libGL, "glGetUniformfv");
      glGetVertexAttribfv := GLGetVertexAttribfvPTR.loadDirectly(libGL, "glGetVertexAttribfv");
      glUniform1f := GLUniform1fPTR.loadDirectly(libGL, "glUniform1f");
      glUniform2f := GLUniform2fPTR.loadDirectly(libGL, "glUniform2f");
      glUniform3f := GLUniform3fPTR.loadDirectly(libGL, "glUniform3f");
      glUniform4f := GLUniform4fPTR.loadDirectly(libGL, "glUniform4f");
      glUniform1fv := GLUniform1fvPTR.loadDirectly(libGL, "glUniform1fv");
      glUniform2fv := GLUniform2fvPTR.loadDirectly(libGL, "glUniform2fv");
      glUniform3fv := GLUniform3fvPTR.loadDirectly(libGL, "glUniform3fv");
      glUniform4fv := GLUniform4fvPTR.loadDirectly(libGL, "glUniform4fv");
      glVertexAttrib1f := GLVertexAttrib1fPTR.loadDirectly(libGL, "glVertexAttrib1f");
      glVertexAttrib1fv := GLVertexAttrib1fvPTR.loadDirectly(libGL, "glVertexAttrib1fv");
      glVertexAttrib2f := GLVertexAttrib2fPTR.loadDirectly(libGL, "glVertexAttrib2f");
      glVertexAttrib2fv := GLVertexAttrib2fvPTR.loadDirectly(libGL, "glVertexAttrib2fv");
      glVertexAttrib3f := GLVertexAttrib3fPTR.loadDirectly(libGL, "glVertexAttrib3f");
      glVertexAttrib3fv := GLVertexAttrib3fvPTR.loadDirectly(libGL, "glVertexAttrib3fv");
      glVertexAttrib4f := GLVertexAttrib4fPTR.loadDirectly(libGL, "glVertexAttrib4f");
      glVertexAttrib4fv := GLVertexAttrib4fvPTR.loadDirectly(libGL, "glVertexAttrib4fv");
      glUniformMatrix2fv := GLUniformMatrix2fvPTR.loadDirectly(libGL, "glUniformMatrix2fv");
      glUniformMatrix3fv := GLUniformMatrix3fvPTR.loadDirectly(libGL, "glUniformMatrix3fv");
      glUniformMatrix4fv := GLUniformMatrix4fvPTR.loadDirectly(libGL, "glUniformMatrix4fv");      
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_1 == 0) {
         ^ self
      };
      glUniformMatrix2x3fv := GLUniformMatrix2x3fvPTR.loadDirectly(libGL, "glUniformMatrix2x3fv");
      glUniformMatrix3x2fv := GLUniformMatrix3x2fvPTR.loadDirectly(libGL, "glUniformMatrix3x2fv");
      glUniformMatrix2x4fv := GLUniformMatrix2x4fvPTR.loadDirectly(libGL, "glUniformMatrix2x4fv");
      glUniformMatrix4x2fv := GLUniformMatrix4x2fvPTR.loadDirectly(libGL, "glUniformMatrix4x2fv");
      glUniformMatrix3x4fv := GLUniformMatrix3x4fvPTR.loadDirectly(libGL, "glUniformMatrix3x4fv");
      glUniformMatrix4x3fv := GLUniformMatrix4x3fvPTR.loadDirectly(libGL, "glUniformMatrix4x3fv");      
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_0 == 0) {
         ^ self
      };
      
      glClearBufferfv := GLClearBufferfvPTR.loadDirectly(libGL, "glClearBufferfv");
      glClearBufferfi := GLClearBufferfiPTR.loadDirectly(libGL, "glClearBufferfi");
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_2 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_3 == 0) {
         ^ self
      };
      
      glSamplerParameterf := GLSamplerParameterfPTR.loadDirectly(libGL, "glSamplerParameterf");
      glSamplerParameterfv := GLSamplerParameterfvPTR.loadDirectly(libGL, "glSamplerParameterfv");
      glGetSamplerParameterfv := GLGetSamplerParameterfvPTR.loadDirectly(libGL, "glGetSamplerParameterfv");
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_4_0 == 0) {
         ^ self
      };
      glPatchParameterfv := GLPatchParameterfvPTR.loadDirectly(libGL, "glPatchParameterfv");
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
//      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_base_instance == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_texture == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clip_control == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
//      if (config.GLAD_GL_ARB_compute_shader == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_copy_image == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_debug_output == 0) {
//         ^ self
//      };
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
      load_GL_AMD_multi_draw_indirect(config);
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
 
      ^ true
   }      
}

public struct GLAD_WrapperI
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   GLTexParameteriPTR   glTexParameteri;
   GLTexParameterivPTR  glTexParameteriv;
   GLPixelStoreiPTR     glPixelStorei;
   GLGetTexParameterivPTR        glGetTexParameteriv;
   GLGetTexLevelParameterivPTR   glGetTexLevelParameteriv;
   GLColor3iPTR         glColor3i;
   GLColor3ivPTR        glColor3iv;
   GLColor4iPTR         glColor4i;
   GLColor4ivPTR        glColor4iv;
   GLIndexiPTR          glIndexi;
   GLIndexivPTR         glIndexiv;
   GLNormal3iPTR        glNormal3i;
   GLNormal3ivPTR       glNormal3iv;
   GLRasterPos2iPTR     glRasterPos2i;
   GLRasterPos2ivPTR    glRasterPos2iv;
   GLRasterPos3iPTR     glRasterPos3i;
   GLRasterPos3ivPTR    glRasterPos3iv;
   GLRasterPos4iPTR     glRasterPos4i;
   GLRasterPos4ivPTR    glRasterPos4iv;
   GLRectiPTR           glRecti;
   GLRectivPTR          glRectiv;
   GLTexCoord1iPTR      glTexCoord1i;
   GLTexCoord1ivPTR     glTexCoord1iv;
   GLTexCoord2iPTR      glTexCoord2i;
   GLTexCoord2ivPTR     glTexCoord2iv;
   GLTexCoord3iPTR      glTexCoord3i;
   GLTexCoord3ivPTR     glTexCoord3iv;
   GLTexCoord4iPTR      glTexCoord4i;
   GLTexCoord4ivPTR     glTexCoord4iv;
   GLVertex2iPTR        glVertex2i;
   GLVertex2ivPTR       glVertex2iv;
   GLVertex3iPTR        glVertex3i;
   GLVertex3ivPTR       glVertex3iv;
   GLVertex4iPTR        glVertex4i;
   GLVertex4ivPTR       glVertex4iv;
   GLFogiPTR            glFogi;
   GLFogivPTR           glFogiv;
   GLLightiPTR          glLighti;
   GLLightivPTR         glLightiv;
   GLLightModeliPTR     glLightModeli;
   GLLightModelivPTR    glLightModeliv;   
   GLMaterialiPTR       glMateriali;
   GLMaterialivPTR      glMaterialiv;
   GLTexEnviPTR         glTexEnvi;
   GLTexEnvivPTR        glTexEnviv;
   GLTexGeniPTR         glTexGeni;
   GLTexGenivPTR        glTexGeniv;
   GLPixelTransferiPTR  glPixelTransferi;
   GLGetLightivPTR      glGetLightiv;
   GLGetMapivPTR        glGetMapiv;
   GLGetMaterialivPTR   glGetMaterialiv;
   GLGetTexEnvivPTR     glGetTexEnviv;
   GLGetTexGenivPTR     glGetTexGeniv;
   // 1.3
   GLMultiTexCoord1iPTR       glMultiTexCoord1i;
   GLMultiTexCoord1ivPTR      glMultiTexCoord1iv;
   GLMultiTexCoord2iPTR       glMultiTexCoord2i;
   GLMultiTexCoord2ivPTR      glMultiTexCoord2iv;
   GLMultiTexCoord3iPTR       glMultiTexCoord3i;
   GLMultiTexCoord3ivPTR      glMultiTexCoord3iv;
   GLMultiTexCoord4iPTR       glMultiTexCoord4i;
   GLMultiTexCoord4ivPTR      glMultiTexCoord4iv;
   // 1.4
   GLPointParameteriPTR       glPointParameteri;
   GLPointParameterivPTR      glPointParameteriv;
   GLSecondaryColor3iPTR      glSecondaryColor3i;
   GLSecondaryColor3ivPTR     glSecondaryColor3iv;
   GLWindowPos2iPTR           glWindowPos2i;
   GLWindowPos2ivPTR          glWindowPos2iv;
   GLWindowPos3iPTR           glWindowPos3i;
   GLWindowPos3ivPTR          glWindowPos3iv;   
   // 2.0
   GLGetUniformivPTR          glGetUniformiv;
   GLGetVertexAttribivPTR     glGetVertexAttribiv;
   GLUniform1iPTR             glUniform1i;
   GLUniform2iPTR             glUniform2i;
   GLUniform3iPTR             glUniform3i;
   GLUniform4iPTR             glUniform4i;
   GLUniform1ivPTR            glUniform1iv;
   GLUniform2ivPTR            glUniform2iv;
   GLUniform3ivPTR            glUniform3iv;
   GLUniform4ivPTR            glUniform4iv;
   GLVertexAttrib4NivPTR      glVertexAttrib4Niv;
   GLVertexAttrib4ivPTR       glVertexAttrib4iv;
   // 2.1
   // 3.0
   GLVertexAttribI1iPTR       glVertexAttribI1i;
   GLVertexAttribI2iPTR       glVertexAttribI2i;
   GLVertexAttribI3iPTR       glVertexAttribI3i;
   GLVertexAttribI4iPTR       glVertexAttribI4i;
   GLVertexAttribI1ivPTR      glVertexAttribI1iv;
   GLVertexAttribI2ivPTR      glVertexAttribI2iv;
   GLVertexAttribI3ivPTR      glVertexAttribI3iv;
   GLVertexAttribI4ivPTR      glVertexAttribI4iv;
   GLTexParameterIivPTR       glTexParameterIiv;
   GLGetTexParameterIivPTR    glGetTexParameterIiv;
   GLClearBufferivPTR         glClearBufferiv;
   // 3.3
   GLSamplerParameteriPTR        glSamplerParameteri;
   GLSamplerParameterivPTR       glSamplerParameteriv;
   GLSamplerParameterIivPTR      glSamplerParameterIiv;
   GLGetSamplerParameterivPTR    glGetSamplerParameteriv;
   GLGetSamplerParameterIivPTR   glGetSamplerParameterIiv;
   GLGetQueryObjectivPTR         glGetQueryObjectiv;
   // 4.0
   GLGetProgramStageivPTR        glGetProgramStageiv;
   GLPatchParameteriPTR          glPatchParameteri;
   GLEndQueryIndexedPTR          glEndQueryIndexed;
   GLGetQueryIndexedivPTR        glGetQueryIndexediv;
   // ext
   GLGetTransformFeedbackivPTR         glGetTransformFeedbackiv;
   GLGetNamedBufferParameterivPTR      glGetNamedBufferParameteriv;
   GLGetTransformFeedbacki_vPTR        glGetTransformFeedbacki_v;
   GLClearNamedFramebufferivPTR        glClearNamedFramebufferiv;
   GLClearNamedFramebufferfiPTR        glClearNamedFramebufferfi;
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_direct_state_access == 0) {
         ^ self
      };
      glGetTransformFeedbackiv := GLGetTransformFeedbackivPTR.load(get_proc, "glGetTransformFeedbackiv");
      glGetTransformFeedbacki_v := GLGetTransformFeedbacki_vPTR.load(get_proc, "glGetTransformFeedbacki_v");
      glGetNamedBufferParameteriv := GLGetNamedBufferParameterivPTR.load(get_proc, "glGetNamedBufferParameteriv");
      glClearNamedFramebufferiv := GLClearNamedFramebufferivPTR.load(get_proc, "glClearNamedFramebufferiv");
      glClearNamedFramebufferfi := GLClearNamedFramebufferfiPTR.load(get_proc, "glClearNamedFramebufferfi");
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_0 == 0) {
         ^ self
      };
      
      glTexParameteri := GLTexParameteriPTR.loadDirectly(libGL, "glTexParameteri");
      glTexParameteriv := GLTexParameterivPTR.loadDirectly(libGL, "glTexParameteriv");
      glPixelStorei := GLPixelStoreiPTR.loadDirectly(libGL, "glPixelStorei");
      glGetTexParameteriv := GLGetTexParameterivPTR.loadDirectly(libGL, "glGetTexParameteriv");
      glGetTexLevelParameteriv := GLGetTexLevelParameterivPTR.loadDirectly(libGL, "glGetTexLevelParameteriv");
      glColor3i := GLColor3iPTR.loadDirectly(libGL, "glColor3i");
      glColor3iv := GLColor3ivPTR.loadDirectly(libGL, "glColor3iv");
      glColor4i := GLColor4iPTR.loadDirectly(libGL, "glColor4i");
      glColor4iv := GLColor4ivPTR.loadDirectly(libGL, "glColor4iv");
      glIndexi := GLIndexiPTR.loadDirectly(libGL, "glIndexi");
      glIndexiv := GLIndexivPTR.loadDirectly(libGL, "glIndexiv");
      glNormal3i := GLNormal3iPTR.loadDirectly(libGL, "glNormal3i");
      glNormal3iv := GLNormal3ivPTR.loadDirectly(libGL, "glNormal3iv");
      glRasterPos2i := GLRasterPos2iPTR.loadDirectly(libGL, "glRasterPos2i");
      glRasterPos2iv := GLRasterPos2ivPTR.loadDirectly(libGL, "glRasterPos2iv");
      glRasterPos3i := GLRasterPos3iPTR.loadDirectly(libGL, "glRasterPos3i");
      glRasterPos3iv := GLRasterPos3ivPTR.loadDirectly(libGL, "glRasterPos3iv");
      glRasterPos4i := GLRasterPos4iPTR.loadDirectly(libGL, "glRasterPos4i");
      glRasterPos4iv := GLRasterPos4ivPTR.loadDirectly(libGL, "glRasterPos4iv");
      glRecti := GLRectiPTR.loadDirectly(libGL, "glRecti");
      glRectiv := GLRectivPTR.loadDirectly(libGL, "glRectiv");
      glTexCoord1i := GLTexCoord1iPTR.loadDirectly(libGL, "glTexCoord1i");
      glTexCoord1iv := GLTexCoord1ivPTR.loadDirectly(libGL, "glTexCoord1iv");
      glTexCoord2i := GLTexCoord2iPTR.loadDirectly(libGL, "glTexCoord2i");
      glTexCoord2iv := GLTexCoord2ivPTR.loadDirectly(libGL, "glTexCoord2iv");
      glTexCoord3i := GLTexCoord3iPTR.loadDirectly(libGL, "glTexCoord3i");
      glTexCoord3iv := GLTexCoord3ivPTR.loadDirectly(libGL, "glTexCoord3iv");
      glTexCoord4i := GLTexCoord4iPTR.loadDirectly(libGL, "glTexCoord4i");
      glTexCoord4iv := GLTexCoord4ivPTR.loadDirectly(libGL, "glTexCoord4iv");
      glVertex2i := GLVertex2iPTR.loadDirectly(libGL, "glVertex2i");
      glVertex2iv := GLVertex2ivPTR.loadDirectly(libGL, "glVertex2iv");
      glVertex3i := GLVertex3iPTR.loadDirectly(libGL, "glVertex3i");
      glVertex3iv := GLVertex3ivPTR.loadDirectly(libGL, "glVertex3iv");
      glVertex4i := GLVertex4iPTR.loadDirectly(libGL, "glVertex4i");
      glVertex4iv := GLVertex4ivPTR.loadDirectly(libGL, "glVertex4iv");
      glFogi := GLFogiPTR.loadDirectly(libGL, "glFogi");
      glFogiv := GLFogivPTR.loadDirectly(libGL, "glFogiv"); 
      glLighti := GLLightiPTR.loadDirectly(libGL, "glLighti");
      glLightiv := GLLightivPTR.loadDirectly(libGL, "glLightiv");
      glLightModeli := GLLightModeliPTR.loadDirectly(libGL, "glLightModeli");
      glLightModeliv := GLLightModelivPTR.loadDirectly(libGL, "glLightModeliv"); 
      glMateriali := GLMaterialiPTR.loadDirectly(libGL, "glMateriali");
      glMaterialiv := GLMaterialivPTR.loadDirectly(libGL, "glMaterialiv");
      glTexEnvi := GLTexEnviPTR.loadDirectly(libGL, "glTexEnvi");
      glTexEnviv := GLTexEnvivPTR.loadDirectly(libGL, "glTexEnviv"); 
      glTexGeni := GLTexGeniPTR.loadDirectly(libGL, "glTexGeni");
      glTexGeniv := GLTexGenivPTR.loadDirectly(libGL, "glTexGeniv");
      glPixelTransferi := GLPixelTransferiPTR.loadDirectly(libGL, "glPixelTransferi");
      glGetLightiv := GLGetLightivPTR.loadDirectly(libGL, "glGetLightiv");
      glGetMapiv := GLGetMapivPTR.loadDirectly(libGL, "glGetMapiv");
      glGetMaterialiv := GLGetMaterialivPTR.loadDirectly(libGL, "glGetMaterialiv");
      glGetTexEnviv := GLGetTexEnvivPTR.loadDirectly(libGL, "glGetTexEnviv");
      glGetTexGeniv := GLGetTexGenivPTR.loadDirectly(libGL, "glGetTexGeniv");
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_2 == 0) {
//         ^ self
//      };
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_3 == 0) {
         ^ self
      };
      
      glMultiTexCoord1i := GLMultiTexCoord1iPTR.loadDirectly(libGL, "glMultiTexCoord1i");
      glMultiTexCoord1iv := GLMultiTexCoord1ivPTR.loadDirectly(libGL, "glMultiTexCoord1iv");
      glMultiTexCoord2i := GLMultiTexCoord2iPTR.loadDirectly(libGL, "glMultiTexCoord2i");
      glMultiTexCoord2iv := GLMultiTexCoord2ivPTR.loadDirectly(libGL, "glMultiTexCoord2iv");
      glMultiTexCoord3i := GLMultiTexCoord3iPTR.loadDirectly(libGL, "glMultiTexCoord3i");
      glMultiTexCoord3iv := GLMultiTexCoord3ivPTR.loadDirectly(libGL, "glMultiTexCoord3iv");
      glMultiTexCoord4i := GLMultiTexCoord4iPTR.loadDirectly(libGL, "glMultiTexCoord4i");
      glMultiTexCoord4iv := GLMultiTexCoord4ivPTR.loadDirectly(libGL, "glMultiTexCoord4iv");
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_4 == 0) {
         ^ self
      };

      glPointParameteri := GLPointParameteriPTR.loadDirectly(libGL, "glPointParameteri");
      glPointParameteriv := GLPointParameterivPTR.loadDirectly(libGL, "glPointParameteriv");      
      glSecondaryColor3i := GLSecondaryColor3iPTR.loadDirectly(libGL, "glSecondaryColor3i");
      glSecondaryColor3iv := GLSecondaryColor3ivPTR.loadDirectly(libGL, "glSecondaryColor3iv");
      glWindowPos2i := GLWindowPos2iPTR.loadDirectly(libGL, "glWindowPos2i");
      glWindowPos2iv := GLWindowPos2ivPTR.loadDirectly(libGL, "glWindowPos2iv");
      glWindowPos3i := GLWindowPos3iPTR.loadDirectly(libGL, "glWindowPos3i");
      glWindowPos3iv := GLWindowPos3ivPTR.loadDirectly(libGL, "glWindowPos3iv");
   }            

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_5 == 0) {
         ^ self
      };
      glGetQueryObjectiv := GLGetQueryObjectivPTR.loadDirectly(libGL, "glGetQueryObjectiv");
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_0 == 0) {
         ^ self
      };

      glGetUniformiv := GLGetUniformivPTR.loadDirectly(libGL, "glGetUniformiv");
      glGetVertexAttribiv := GLGetVertexAttribivPTR.loadDirectly(libGL, "glGetVertexAttribiv");
      glUniform1i := GLUniform1iPTR.loadDirectly(libGL, "glUniform1i");
      glUniform2i := GLUniform2iPTR.loadDirectly(libGL, "glUniform2i");
      glUniform3i := GLUniform3iPTR.loadDirectly(libGL, "glUniform3i");
      glUniform4i := GLUniform4iPTR.loadDirectly(libGL, "glUniform4i");
      glUniform1iv := GLUniform1ivPTR.loadDirectly(libGL, "glUniform1iv");
      glUniform2iv := GLUniform2ivPTR.loadDirectly(libGL, "glUniform2iv");
      glUniform3iv := GLUniform3ivPTR.loadDirectly(libGL, "glUniform3iv");
      glUniform4iv := GLUniform4ivPTR.loadDirectly(libGL, "glUniform4iv");
      glVertexAttrib4Niv := GLVertexAttrib4NivPTR.loadDirectly(libGL, "glVertexAttrib4Niv");
      glVertexAttrib4iv := GLVertexAttrib4ivPTR.loadDirectly(libGL, "glVertexAttrib4iv");
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_0 == 0) {
         ^ self
      };
      
      glVertexAttribI1i := GLVertexAttribI1iPTR.loadDirectly(libGL, "glVertexAttribI1i");
      glVertexAttribI2i := GLVertexAttribI2iPTR.loadDirectly(libGL, "glVertexAttribI2i");
      glVertexAttribI3i := GLVertexAttribI3iPTR.loadDirectly(libGL, "glVertexAttribI3i");
      glVertexAttribI4i := GLVertexAttribI4iPTR.loadDirectly(libGL, "glVertexAttribI4i");
      glVertexAttribI1iv := GLVertexAttribI1ivPTR.loadDirectly(libGL, "glVertexAttribI1iv");
      glVertexAttribI2iv := GLVertexAttribI2ivPTR.loadDirectly(libGL, "glVertexAttribI2iv");
      glVertexAttribI3iv := GLVertexAttribI3ivPTR.loadDirectly(libGL, "glVertexAttribI3iv");
      glVertexAttribI4iv := GLVertexAttribI4ivPTR.loadDirectly(libGL, "glVertexAttribI4iv");
      glTexParameterIiv := GLTexParameterIivPTR.loadDirectly(libGL, "glTexParameterIiv");
      glGetTexParameterIiv := GLGetTexParameterIivPTR.loadDirectly(libGL, "glGetTexParameterIiv");
      glClearBufferiv := GLClearBufferivPTR.loadDirectly(libGL, "glClearBufferiv");
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_2 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_3 == 0) {
         ^ self
      };
      
      glSamplerParameteri := GLSamplerParameteriPTR.loadDirectly(libGL, "glSamplerParameteri");
      glSamplerParameteriv := GLSamplerParameterivPTR.loadDirectly(libGL, "glSamplerParameteriv");
      glSamplerParameterIiv := GLSamplerParameterIivPTR.loadDirectly(libGL, "glSamplerParameterIiv");
      glGetSamplerParameteriv := GLGetSamplerParameterivPTR.loadDirectly(libGL, "glGetSamplerParameteriv");
      glGetSamplerParameterIiv := GLGetSamplerParameterIivPTR.loadDirectly(libGL, "glGetSamplerParameterIiv");
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_4_0 == 0) {
         ^ self
      };
      glGetProgramStageiv := GLGetProgramStageivPTR.loadDirectly(libGL, "glGetProgramStageiv");
      glPatchParameteri := GLPatchParameteriPTR.loadDirectly(libGL, "glPatchParameteri");
      glEndQueryIndexed := GLEndQueryIndexedPTR.loadDirectly(libGL, "glEndQueryIndexed");
      glGetQueryIndexediv := GLGetQueryIndexedivPTR.loadDirectly(libGL, "glGetQueryIndexediv");
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
//      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_base_instance == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_texture == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clip_control == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
//      if (config.GLAD_GL_ARB_compute_shader == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_copy_image == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_debug_output == 0) {
//         ^ self
//      };
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
      load_GL_AMD_multi_draw_indirect(config);
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
 
      ^ true
   }      
}

public struct GLAD_WrapperUI
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   GLColor3uiPTR        glColor3ui;
   GLColor3uivPTR       glColor3uiv;
   GLColor4uiPTR        glColor4ui;
   GLColor4uivPTR       glColor4uiv;
   GLPixelMapuivPTR     glPixelMapuiv;
   GLGetPixelMapuivPTR  glGetPixelMapuiv;
   // 1.3
   // 1.4
   GLSecondaryColor3uiPTR     glSecondaryColor3ui;
   GLSecondaryColor3uivPTR    glSecondaryColor3uiv;
   // 2.0
   GLUniform1uiPTR            glUniform1ui;
   GLUniform2uiPTR            glUniform2ui;
   GLUniform3uiPTR            glUniform3ui;
   GLUniform4uiPTR            glUniform4ui;
   GLVertexAttrib4NuivPTR     glVertexAttrib4Nuiv;
   GLVertexAttrib4uivPTR      glVertexAttrib4uiv;
   // 2.1
   // 3.0
   GLVertexAttribI1uiPTR      glVertexAttribI1ui;
   GLVertexAttribI2uiPTR      glVertexAttribI2ui;
   GLVertexAttribI3uiPTR      glVertexAttribI3ui;
   GLVertexAttribI4uiPTR      glVertexAttribI4ui;
   GLVertexAttribI1uivPTR     glVertexAttribI1uiv;
   GLVertexAttribI2uivPTR     glVertexAttribI2uiv;
   GLVertexAttribI3uivPTR     glVertexAttribI3uiv;
   GLVertexAttribI4uivPTR     glVertexAttribI4uiv;
   GLUniform1uivPTR              glUniform1uiv;
   GLUniform2uivPTR              glUniform2uiv;
   GLUniform3uivPTR              glUniform3uiv;
   GLUniform4uivPTR              glUniform4uiv;
   GLTexParameterIuivPTR      glTexParameterIuiv;
   GLGetTexParameterIuivPTR   glGetTexParameterIuiv;
   GLClearBufferuivPTR        glClearBufferuiv;
   // 3.3
   GLSamplerParameterIuivPTR     glSamplerParameterIuiv;
   GLGetSamplerParameterIuivPTR  glGetSamplerParameterIuiv;
   GLGetQueryObjectuivPTR        glGetQueryObjectuiv;   
   // 4.0
   GLUniformSubroutinesuivPTR    glUniformSubroutinesuiv;
   GLGetUniformSubroutineuivPTR  glGetUniformSubroutineuiv;
   // ext
   GLClearNamedFramebufferuivPTR       glClearNamedFramebufferuiv;
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_direct_state_access == 0) {
         ^ self
      };
      glClearNamedFramebufferuiv := GLClearNamedFramebufferuivPTR.load(get_proc, "glClearNamedFramebufferuiv");
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_0 == 0) {
         ^ self
      };
      
      glColor3ui := GLColor3uiPTR.loadDirectly(libGL, "glColor3ui");
      glColor3uiv := GLColor3uivPTR.loadDirectly(libGL, "glColor3uiv");
      glColor4ui := GLColor4uiPTR.loadDirectly(libGL, "glColor4ui");
      glColor4uiv := GLColor4uivPTR.loadDirectly(libGL, "glColor4uiv");
      glPixelMapuiv := GLPixelMapuivPTR.loadDirectly(libGL, "glPixelMapuiv");
      glGetPixelMapuiv := GLGetPixelMapuivPTR.loadDirectly(libGL, "glGetPixelMapuiv");
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_2 == 0) {
//         ^ self
//      };
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_3 == 0) {
//         ^ self
//      };
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_4 == 0) {
         ^ self
      };

      glSecondaryColor3ui := GLSecondaryColor3uiPTR.loadDirectly(libGL, "glSecondaryColor3ui");
      glSecondaryColor3uiv := GLSecondaryColor3uivPTR.loadDirectly(libGL, "glSecondaryColor3uiv");
   }            

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_5 == 0) {
         ^ self
      };
      glGetQueryObjectuiv := GLGetQueryObjectuivPTR.loadDirectly(libGL, "glGetQueryObjectuiv");      
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_0 == 0) {
         ^ self
      };

      glVertexAttrib4Nuiv := GLVertexAttrib4NuivPTR.loadDirectly(libGL, "glVertexAttrib4Nuiv");
      glVertexAttrib4uiv := GLVertexAttrib4uivPTR.loadDirectly(libGL, "glVertexAttrib4uiv");
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_0 == 0) {
         ^ self
      };
      
      glVertexAttribI1ui := GLVertexAttribI1uiPTR.loadDirectly(libGL, "glVertexAttribI1ui");
      glVertexAttribI2ui := GLVertexAttribI2uiPTR.loadDirectly(libGL, "glVertexAttribI2ui");
      glVertexAttribI3ui := GLVertexAttribI3uiPTR.loadDirectly(libGL, "glVertexAttribI3ui");
      glVertexAttribI4ui := GLVertexAttribI4uiPTR.loadDirectly(libGL, "glVertexAttribI4ui");
      glVertexAttribI1uiv := GLVertexAttribI1uivPTR.loadDirectly(libGL, "glVertexAttribI1uiv");
      glVertexAttribI2uiv := GLVertexAttribI2uivPTR.loadDirectly(libGL, "glVertexAttribI2uiv");
      glVertexAttribI3uiv := GLVertexAttribI3uivPTR.loadDirectly(libGL, "glVertexAttribI3uiv");
      glVertexAttribI4uiv := GLVertexAttribI4uivPTR.loadDirectly(libGL, "glVertexAttribI4uiv");
      glTexParameterIuiv := GLTexParameterIuivPTR.loadDirectly(libGL, "glTexParameterIuiv");
      glGetTexParameterIuiv := GLGetTexParameterIuivPTR.loadDirectly(libGL, "glGetTexParameterIuiv");
      glClearBufferuiv := GLClearBufferuivPTR.loadDirectly(libGL, "glClearBufferuiv");
      glUniform1ui := GLUniform1uiPTR.loadDirectly(libGL, "glUniform1ui");
      glUniform2ui := GLUniform2uiPTR.loadDirectly(libGL, "glUniform2ui");
      glUniform3ui := GLUniform3uiPTR.loadDirectly(libGL, "glUniform3ui");
      glUniform4ui := GLUniform4uiPTR.loadDirectly(libGL, "glUniform4ui");
      glUniform1uiv := GLUniform1uivPTR.loadDirectly(libGL, "glUniform1uiv");
      glUniform2uiv := GLUniform2uivPTR.loadDirectly(libGL, "glUniform2uiv");
      glUniform3uiv := GLUniform3uivPTR.loadDirectly(libGL, "glUniform3uiv");
      glUniform4uiv := GLUniform4uivPTR.loadDirectly(libGL, "glUniform4uiv");
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_2 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_3 == 0) {
         ^ self
      };
      
      glSamplerParameterIuiv := GLSamplerParameterIuivPTR.loadDirectly(libGL, "glSamplerParameterIuiv");
      glGetSamplerParameterIuiv := GLGetSamplerParameterIuivPTR.loadDirectly(libGL, "glGetSamplerParameterIuiv");
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_4_0 == 0) {
         ^ self
      };
      glGetUniformSubroutineuiv := GLGetUniformSubroutineuivPTR.loadDirectly(libGL, "glGetUniformSubroutineuiv");
      glUniformSubroutinesuiv := GLUniformSubroutinesuivPTR.loadDirectly(libGL, "glUniformSubroutinesuiv");
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
//      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_base_instance == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_texture == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clip_control == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
//      if (config.GLAD_GL_ARB_compute_shader == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_copy_image == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_debug_output == 0) {
//         ^ self
//      };
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
      load_GL_AMD_multi_draw_indirect(config);
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
 
      ^ true
   }      
}

public struct GLAD_WrapperS
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   GLColor3sPTR         glColor3s;
   GLColor3svPTR        glColor3sv;
   GLColor4sPTR         glColor4s;
   GLColor4svPTR        glColor4sv;
   GLIndexsPTR          glIndexs;
   GLIndexsvPTR         glIndexsv;
   GLNormal3sPTR        glNormal3s;
   GLNormal3svPTR       glNormal3sv;
   GLRasterPos2sPTR     glRasterPos2s;
   GLRasterPos2svPTR    glRasterPos2sv;
   GLRasterPos3sPTR     glRasterPos3s;
   GLRasterPos3svPTR    glRasterPos3sv;   
   GLRasterPos4sPTR     glRasterPos4s;
   GLRasterPos4svPTR    glRasterPos4sv;
   GLRectsPTR           glRects;
   GLRectsvPTR          glRectsv;
   GLTexCoord1sPTR      glTexCoord1s;
   GLTexCoord1svPTR     glTexCoord1sv;
   GLTexCoord2sPTR      glTexCoord2s;
   GLTexCoord2svPTR     glTexCoord2sv;
   GLTexCoord3sPTR      glTexCoord3s;
   GLTexCoord3svPTR     glTexCoord3sv;
   GLTexCoord4sPTR      glTexCoord4s;
   GLTexCoord4svPTR     glTexCoord4sv;
   GLVertex2sPTR        glVertex2s;
   GLVertex2svPTR       glVertex2sv;
   GLVertex3sPTR        glVertex3s;
   GLVertex3svPTR       glVertex3sv;
   GLVertex4sPTR        glVertex4s;
   GLVertex4svPTR       glVertex4sv;
   // 1.3
   GLMultiTexCoord1sPTR       glMultiTexCoord1s;
   GLMultiTexCoord1svPTR      glMultiTexCoord1sv;
   GLMultiTexCoord2sPTR       glMultiTexCoord2s;
   GLMultiTexCoord2svPTR      glMultiTexCoord2sv;
   GLMultiTexCoord3sPTR       glMultiTexCoord3s;
   GLMultiTexCoord3svPTR      glMultiTexCoord3sv;
   GLMultiTexCoord4sPTR       glMultiTexCoord4s;
   GLMultiTexCoord4svPTR      glMultiTexCoord4sv;
   // 1.4
   GLSecondaryColor3sPTR      glSecondaryColor3s;
   GLSecondaryColor3svPTR     glSecondaryColor3sv;
   GLWindowPos2sPTR           glWindowPos2s;
   GLWindowPos2svPTR          glWindowPos2sv;
   GLWindowPos3sPTR           glWindowPos3s;
   GLWindowPos3svPTR          glWindowPos3sv;
   // 2.0
   GLVertexAttrib1sPTR        glVertexAttrib1s;
   GLVertexAttrib1svPTR       glVertexAttrib1sv;
   GLVertexAttrib2sPTR        glVertexAttrib2s;
   GLVertexAttrib2svPTR       glVertexAttrib2sv;
   GLVertexAttrib3sPTR        glVertexAttrib3s;
   GLVertexAttrib3svPTR       glVertexAttrib3sv;
   GLVertexAttrib4NsvPTR      glVertexAttrib4Nsv;
   GLVertexAttrib4sPTR        glVertexAttrib4s;
   GLVertexAttrib4svPTR       glVertexAttrib4sv;
   // 2.1
   // 3.0
   GLVertexAttribI4svPTR      glVertexAttribI4sv;
   // 3.3
   // 4.0
   // ext
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_direct_state_access == 0) {
//         ^ self
//      };
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_0 == 0) {
         ^ self
      };
      
      glColor3s := GLColor3sPTR.loadDirectly(libGL, "glColor3s"); 
      glColor3sv := GLColor3svPTR.loadDirectly(libGL, "glColor3sv");
      glColor4s := GLColor4sPTR.loadDirectly(libGL, "glColor4s");
      glColor4sv := GLColor4svPTR.loadDirectly(libGL, "glColor4sv");
      glIndexs := GLIndexsPTR.loadDirectly(libGL, "glIndexs");
      glIndexsv := GLIndexsvPTR.loadDirectly(libGL, "glIndexsv");
      glNormal3s := GLNormal3sPTR.loadDirectly(libGL, "glNormal3s");
      glNormal3sv := GLNormal3svPTR.loadDirectly(libGL, "glNormal3sv");
      glRasterPos2s := GLRasterPos2sPTR.loadDirectly(libGL, "glRasterPos2s");
      glRasterPos2sv := GLRasterPos2svPTR.loadDirectly(libGL, "glRasterPos2sv"); 
      glRasterPos3s := GLRasterPos3sPTR.loadDirectly(libGL, "glRasterPos3s");
      glRasterPos3sv := GLRasterPos3svPTR.loadDirectly(libGL, "glRasterPos3sv");
      glRasterPos4s := GLRasterPos4sPTR.loadDirectly(libGL, "glRasterPos4s");
      glRasterPos4sv := GLRasterPos4svPTR.loadDirectly(libGL, "glRasterPos4sv"); 
      glRects := GLRectsPTR.loadDirectly(libGL, "glRects");
      glRectsv := GLRectsvPTR.loadDirectly(libGL, "glRectsv"); 
      glTexCoord1s := GLTexCoord1sPTR.loadDirectly(libGL, "glTexCoord1s");
      glTexCoord1sv := GLTexCoord1svPTR.loadDirectly(libGL, "glTexCoord1sv");
      glTexCoord2s := GLTexCoord2sPTR.loadDirectly(libGL, "glTexCoord2s");
      glTexCoord2sv := GLTexCoord2svPTR.loadDirectly(libGL, "glTexCoord2sv"); 
      glTexCoord3s := GLTexCoord3sPTR.loadDirectly(libGL, "glTexCoord3s");
      glTexCoord3sv := GLTexCoord3svPTR.loadDirectly(libGL, "glTexCoord3sv"); 
      glTexCoord4s := GLTexCoord4sPTR.loadDirectly(libGL, "glTexCoord4s");
      glTexCoord4sv := GLTexCoord4svPTR.loadDirectly(libGL, "glTexCoord4sv");
      glVertex2s := GLVertex2sPTR.loadDirectly(libGL, "glVertex2s");
      glVertex2sv := GLVertex2svPTR.loadDirectly(libGL, "glVertex2sv");
      glVertex3s := GLVertex3sPTR.loadDirectly(libGL, "glVertex3s");
      glVertex3sv := GLVertex3svPTR.loadDirectly(libGL, "glVertex3sv");
      glVertex4s := GLVertex4sPTR.loadDirectly(libGL, "glVertex4s");
      glVertex4sv := GLVertex4svPTR.loadDirectly(libGL, "glVertex4sv");
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_2 == 0) {
//         ^ self
//      };
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_3 == 0) {
         ^ self
      };
      
      glMultiTexCoord1s := GLMultiTexCoord1sPTR.loadDirectly(libGL, "glMultiTexCoord1s");
      glMultiTexCoord1sv := GLMultiTexCoord1svPTR.loadDirectly(libGL, "glMultiTexCoord1sv");
      glMultiTexCoord2s := GLMultiTexCoord2sPTR.loadDirectly(libGL, "glMultiTexCoord2s");
      glMultiTexCoord2sv := GLMultiTexCoord2svPTR.loadDirectly(libGL, "glMultiTexCoord2sv");
      glMultiTexCoord3s := GLMultiTexCoord3sPTR.loadDirectly(libGL, "glMultiTexCoord3s");
      glMultiTexCoord3sv := GLMultiTexCoord3svPTR.loadDirectly(libGL, "glMultiTexCoord3sv");
      glMultiTexCoord4s := GLMultiTexCoord4sPTR.loadDirectly(libGL, "glMultiTexCoord4s");
      glMultiTexCoord4sv := GLMultiTexCoord4svPTR.loadDirectly(libGL, "glMultiTexCoord4sv");
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_4 == 0) {
         ^ self
      };

      glSecondaryColor3s := GLSecondaryColor3sPTR.loadDirectly(libGL, "glSecondaryColor3s");
      glSecondaryColor3sv := GLSecondaryColor3svPTR.loadDirectly(libGL, "glSecondaryColor3sv");
      glWindowPos2s := GLWindowPos2sPTR.loadDirectly(libGL, "glWindowPos2s");
      glWindowPos2sv := GLWindowPos2svPTR.loadDirectly(libGL, "glWindowPos2sv");
      glWindowPos3s := GLWindowPos3sPTR.loadDirectly(libGL, "glWindowPos3s");
      glWindowPos3sv := GLWindowPos3svPTR.loadDirectly(libGL, "glWindowPos3sv");
   }            

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_5 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_0 == 0) {
         ^ self
      };

      glVertexAttrib1s := GLVertexAttrib1sPTR.loadDirectly(libGL, "glVertexAttrib1s");
      glVertexAttrib1sv := GLVertexAttrib1svPTR.loadDirectly(libGL, "glVertexAttrib1sv");
      glVertexAttrib2s := GLVertexAttrib2sPTR.loadDirectly(libGL, "glVertexAttrib2s");
      glVertexAttrib2sv := GLVertexAttrib2svPTR.loadDirectly(libGL, "glVertexAttrib2sv");
      glVertexAttrib3s := GLVertexAttrib3sPTR.loadDirectly(libGL, "glVertexAttrib3s");
      glVertexAttrib3sv := GLVertexAttrib3svPTR.loadDirectly(libGL, "glVertexAttrib3sv");
      glVertexAttrib4s := GLVertexAttrib4sPTR.loadDirectly(libGL, "glVertexAttrib4s");
      glVertexAttrib4sv := GLVertexAttrib4svPTR.loadDirectly(libGL, "glVertexAttrib4sv");
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_0 == 0) {
         ^ self
      };
      
      glVertexAttribI4sv := GLVertexAttribI4svPTR.loadDirectly(libGL, "glVertexAttribI4sv");
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_2 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_3 == 0) {
//         ^ self
//      };      
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_4_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
//      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_base_instance == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_texture == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clip_control == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
//      if (config.GLAD_GL_ARB_compute_shader == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_copy_image == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_debug_output == 0) {
//         ^ self
//      };
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
      load_GL_AMD_multi_draw_indirect(config);
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
 
      ^ true
   }      
}

public struct GLAD_WrapperUS
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   GLColor3usPTR        glColor3us;
   GLColor3usvPTR       glColor3usv;
   GLColor4usPTR        glColor4us;
   GLColor4usvPTR       glColor4usv;
   GLPixelMapusvPTR     glPixelMapusv;
   GLGetPixelMapusvPTR  glGetPixelMapusv;
   // 1.3
   // 1.4
   GLSecondaryColor3usPTR     glSecondaryColor3us;
   GLSecondaryColor3usvPTR    glSecondaryColor3usv;
   // 2.0
   GLVertexAttrib4NusvPTR     glVertexAttrib4Nusv;
   GLVertexAttrib4usvPTR      glVertexAttrib4usv;
   // 2.1
   // 3.0
   GLVertexAttribI4usvPTR     glVertexAttribI4usv;
   // 3.3
   // 4.0
   // ext
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_direct_state_access == 0) {
//         ^ self
//      };
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_0 == 0) {
         ^ self
      };
      
      glColor3us := GLColor3usPTR.loadDirectly(libGL, "glColor3us");
      glColor3usv := GLColor3usvPTR.loadDirectly(libGL, "glColor3usv");   
      glColor4us := GLColor4usPTR.loadDirectly(libGL, "glColor4us");
      glColor4usv := GLColor4usvPTR.loadDirectly(libGL, "glColor4usv");
      glPixelMapusv := GLPixelMapusvPTR.loadDirectly(libGL, "glPixelMapusv");
      glGetPixelMapusv := GLGetPixelMapusvPTR.loadDirectly(libGL, "glGetPixelMapusv");
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_2 == 0) {
//         ^ self
//      };
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_3 == 0) {
//         ^ self
//      };
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_4 == 0) {
         ^ self
      };

      glSecondaryColor3us := GLSecondaryColor3usPTR.loadDirectly(libGL, "glSecondaryColor3us");
      glSecondaryColor3usv := GLSecondaryColor3usvPTR.loadDirectly(libGL, "glSecondaryColor3usv");
   }            

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_5 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_0 == 0) {
         ^ self
      };

      glVertexAttrib4Nusv := GLVertexAttrib4NusvPTR.loadDirectly(libGL, "glVertexAttrib4Nusv");
      glVertexAttrib4usv := GLVertexAttrib4usvPTR.loadDirectly(libGL, "glVertexAttrib4usv");
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_0 == 0) {
         ^ self
      };
      
      glVertexAttribI4usv := GLVertexAttribI4usvPTR.loadDirectly(libGL, "glVertexAttribI4usv");      
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_2 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_3 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_4_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
//      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_base_instance == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_texture == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clip_control == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
//      if (config.GLAD_GL_ARB_compute_shader == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_copy_image == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_debug_output == 0) {
//         ^ self
//      };
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
      load_GL_AMD_multi_draw_indirect(config);
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
 
      ^ true
   }      
}

public struct GLAD_WrapperB
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   GLColor3bPTR         glColor3b;
   GLColor3bvPTR        glColor3bv;
   GLColor4bPTR         glColor4b;
   GLColor4bvPTR        glColor4bv;
   GLNormal3bPTR        glNormal3b;
   GLNormal3bvPTR       glNormal3bv;
   // 1.3
   // 1.4
   GLSecondaryColor3bPTR      glSecondaryColor3b;
   GLSecondaryColor3bvPTR     glSecondaryColor3bv;
   // 2.0
   GLVertexAttrib4NbvPTR      glVertexAttrib4Nbv;
   GLVertexAttrib4bvPTR       glVertexAttrib4bv;
   // 2.1
   // 3.0
   GLVertexAttribI4bvPTR      glVertexAttribI4bv;
   // 3.3
   // 4.0
   // ext
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_direct_state_access == 0) {
//         ^ self
//      };
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_0 == 0) {
         ^ self
      };
      
      glColor3b := GLColor3bPTR.loadDirectly(libGL, "glColor3b");
      glColor3bv := GLColor3bvPTR.loadDirectly(libGL, "glColor3bv");
      glColor4b := GLColor4bPTR.loadDirectly(libGL, "glColor4b");
      glColor4bv := GLColor4bvPTR.loadDirectly(libGL, "glColor4bv");
      glNormal3b := GLNormal3bPTR.loadDirectly(libGL, "glNormal3b");
      glNormal3bv := GLNormal3bvPTR.loadDirectly(libGL, "glNormal3bv");
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_2 == 0) {
//         ^ self
//      };
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_3 == 0) {
//         ^ self
//      };
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_4 == 0) {
         ^ self
      };

      glSecondaryColor3b := GLSecondaryColor3bPTR.loadDirectly(libGL, "glSecondaryColor3b");
      glSecondaryColor3bv := GLSecondaryColor3bvPTR.loadDirectly(libGL, "glSecondaryColor3bv");
   }            

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_5 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_0 == 0) {
         ^ self
      };

      glVertexAttrib4Nbv := GLVertexAttrib4NbvPTR.loadDirectly(libGL, "glVertexAttrib4Nbv");
      glVertexAttrib4bv := GLVertexAttrib4bvPTR.loadDirectly(libGL, "glVertexAttrib4bv");
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_0 == 0) {
         ^ self
      };
      
      glVertexAttribI4bv := GLVertexAttribI4bvPTR.loadDirectly(libGL, "glVertexAttribI4bv");
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_2 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_3 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_4_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
//      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_base_instance == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_texture == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clip_control == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
//      if (config.GLAD_GL_ARB_compute_shader == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_copy_image == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_debug_output == 0) {
//         ^ self
//      };
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
      load_GL_AMD_multi_draw_indirect(config);
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
 
      ^ true
   }      
}

public struct GLAD_WrapperUB
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   GLColor3ubPTR        glColor3ub;
   GLColor3ubvPTR       glColor3ubv;
   GLColor4ubPTR        glColor4ub;
   GLColor4ubvPTR       glColor4ubv;
   // 1.3
   // 1.4
   GLSecondaryColor3ubPTR     glSecondaryColor3ub;
   GLSecondaryColor3ubvPTR    glSecondaryColor3ubv;
   // 2.0
   GLVertexAttrib4NubPTR      glVertexAttrib4Nub;
   GLVertexAttrib4NubvPTR     glVertexAttrib4Nubv;
   GLVertexAttrib4ubvPTR      glVertexAttrib4ubv;
   // 2.1
   // 3.0
   GLVertexAttribI4ubvPTR     glVertexAttribI4ubv;
   // 3.3
   // 4.0
   // ext
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_direct_state_access == 0) {
//         ^ self
//      };
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_0 == 0) {
         ^ self
      };
      
      glColor3ub := GLColor3ubPTR.loadDirectly(libGL, "glColor3ub");
      glColor3ubv := GLColor3ubvPTR.loadDirectly(libGL, "glColor3ubv");
      glColor4ub := GLColor4ubPTR.loadDirectly(libGL, "glColor4ub");
      glColor4ubv := GLColor4ubvPTR.loadDirectly(libGL, "glColor4ubv");
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_2 == 0) {
//         ^ self
//      };
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_3 == 0) {
//         ^ self
//      };
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_1_4 == 0) {
         ^ self
      };

      glSecondaryColor3ub := GLSecondaryColor3ubPTR.loadDirectly(libGL, "glSecondaryColor3ub");
      glSecondaryColor3ubv := GLSecondaryColor3ubvPTR.loadDirectly(libGL, "glSecondaryColor3ubv");
   }            

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_5 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_0 == 0) {
         ^ self
      };

      glVertexAttrib4Nub := GLVertexAttrib4NubPTR.loadDirectly(libGL, "glVertexAttrib4Nub");
      glVertexAttrib4Nubv := GLVertexAttrib4NubvPTR.loadDirectly(libGL, "glVertexAttrib4Nubv");
      glVertexAttrib4ubv := GLVertexAttrib4ubvPTR.loadDirectly(libGL, "glVertexAttrib4ubv");
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_0 == 0) {
         ^ self
      };
      
      glVertexAttribI4ubv := GLVertexAttribI4ubvPTR.loadDirectly(libGL, "glVertexAttribI4ubv");
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_2 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_3 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_4_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
//      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_base_instance == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_texture == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clip_control == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
//      if (config.GLAD_GL_ARB_compute_shader == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_copy_image == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_debug_output == 0) {
//         ^ self
//      };
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
      load_GL_AMD_multi_draw_indirect(config);
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
 
      ^ true
   }      
}


public struct GLAD_WrapperI64
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   // 1.3
   // 1.4
   // 2.0
   // 3.0
   // 3.3
   GLGetQueryObjecti64vPTR       glGetQueryObjecti64v;
   // 4.0
   // ext
   GLGetTransformFeedbacki64_vPTR      glGetTransformFeedbacki64_v;
   GLGetNamedBufferParameteri64vPTR    glGetNamedBufferParameteri64v;
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
      if (config.GLAD_GL_ARB_direct_state_access == 0) {
         ^ self
      };
      glGetTransformFeedbacki64_v := GLGetTransformFeedbacki64_vPTR.load(get_proc, "glGetTransformFeedbacki64_v");
      glGetNamedBufferParameteri64v := GLGetNamedBufferParameteri64vPTR.load(get_proc, "glGetNamedBufferParameteri64v");
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_2 == 0) {
//         ^ self
//      };
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_3 == 0) {
//         ^ self
//      };
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_4 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_5 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_2_0 == 0) {
         ^ self
      };
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_2 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_3 == 0) {
         ^ self
      };
      
      glGetQueryObjecti64v := GLGetQueryObjecti64vPTR.loadDirectly(libGL, "glGetQueryObjecti64v");
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_4_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
//      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_base_instance == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_texture == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clip_control == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
//      if (config.GLAD_GL_ARB_compute_shader == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_copy_image == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_debug_output == 0) {
//         ^ self
//      };
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
      load_GL_AMD_multi_draw_indirect(config);
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
 
      ^ true
   }      
}

public struct GLAD_WrapperUI64
{
   handle libGL;
   
   // opengl functions
   WGLGetProcAddress    get_proc;   
   
   GLGetStringPTR       glGetString;
      
   // 1_0
   // 1.3
   // 1.4
   // 2.0
   // 2.1
   // 3.0
   // 3.3
   GLGetQueryObjectui64vPTR      glGetQueryObjectui64v;
   // 4.0
   // ext
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }
   
   private load_GL_ARB_direct_state_access(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_direct_state_access == 0) {
//         ^ self
//      };
   }   

   private load_GL_VERSION_1_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_1_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_2 == 0) {
//         ^ self
//      };
   }
      
   private load_GL_VERSION_1_3(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_3 == 0) {
//         ^ self
//      };
   }      

   private load_GL_VERSION_1_4(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_4 == 0) {
//         ^ self
//      };
   }            

   private load_GL_VERSION_1_5(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_1_5 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_2_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_2_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_2_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_1(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_1 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_2(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_3_2 == 0) {
//         ^ self
//      };
   }

   private load_GL_VERSION_3_3(GLAD_Config config)
   {
      if (config.GLAD_GL_VERSION_3_3 == 0) {
         ^ self
      };
      
      glGetQueryObjectui64v := GLGetQueryObjectui64vPTR.loadDirectly(libGL, "glGetQueryObjectui64v");
   }

   private load_GL_VERSION_4_0(GLAD_Config config)
   {
//      if (config.GLAD_GL_VERSION_4_0 == 0) {
//         ^ self
//      };
   }

   private load_GL_AMD_multi_draw_indirect(GLAD_Config config)
   {
//      if (config.GLAD_GL_AMD_multi_draw_indirect == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES2_compatibility == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_ES3_1_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_1_compatibility == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_ES3_2_compatibility(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_ES3_2_compatibility == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_base_instance(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_base_instance == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_blend_func_extended(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
//      glBindFragDataLocationIndexed := GLBindFragDataLocationIndexedPTR.load(get_proc, "glBindFragDataLocationIndexed");
//      glGetFragDataIndex := GLGetFragDataIndexPTR.load(get_proc, "glGetFragDataIndex");      
   }
   
   private load_GL_ARB_buffer_storage(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_blend_func_extended == 0) {
//         ^ self
//      };
   }   

   private load_GL_ARB_clear_buffer_object(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_buffer_object == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clear_texture(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clear_texture == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_clip_control(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_clip_control == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_color_buffer_float(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_color_buffer_float == 0) {
//         ^ self
//      };
   }
   
   private load_GL_ARB_compute_shader(GLAD_Config config)   
   {
//      if (config.GLAD_GL_ARB_compute_shader == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_compute_variable_group_size(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_compute_variable_group_size == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_copy_buffer(GLAD_Config config)
   {
//      if (GLAD_GL_ARB_copy_buffer == 0) {
//         ^ self
//      };
//      glCopyBufferSubData := GLCopyBufferSubDataPTR.load(get_proc, "glCopyBufferSubData");
   }
   
   private load_GL_ARB_copy_image(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_copy_image == 0) {
//         ^ self
//      };
   }

   private load_GL_ARB_debug_output(GLAD_Config config)
   {
//      if (config.GLAD_GL_ARB_debug_output == 0) {
//         ^ self
//      };
   }

   bool load(GLAD_Config config/*, IOpenGLEngine engine*/)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
   	load_GL_VERSION_1_0(config);
      load_GL_VERSION_1_1(config);
      load_GL_VERSION_1_2(config);
      load_GL_VERSION_1_3(config);
      load_GL_VERSION_1_4(config);
      load_GL_VERSION_1_5(config);
      load_GL_VERSION_2_0(config);
      load_GL_VERSION_2_1(config);
      load_GL_VERSION_3_0(config);
      load_GL_VERSION_3_1(config);
      load_GL_VERSION_3_2(config);
      load_GL_VERSION_3_3(config);
      load_GL_VERSION_4_0(config);
      
      load_GL_AMD_multi_draw_indirect(config);
      load_GL_ARB_ES2_compatibility(config);
      load_GL_ARB_ES3_1_compatibility(config);
      load_GL_ARB_ES3_2_compatibility(config);
      load_GL_ARB_base_instance(config);
      load_GL_ARB_blend_func_extended(config);
      load_GL_ARB_buffer_storage(config);
      load_GL_ARB_clear_buffer_object(config);
      load_GL_ARB_clear_texture(config);
      load_GL_ARB_clip_control(config);
      load_GL_ARB_color_buffer_float(config);
      load_GL_ARB_compute_shader(config);
      load_GL_ARB_compute_variable_group_size(config);
      load_GL_ARB_copy_buffer(config);
      load_GL_ARB_copy_image(config);
      load_GL_ARB_debug_output(config);
      load_GL_ARB_direct_state_access(config);
 
      ^ true
   }      
}

public struct GLADCanvas : IGLCanvas
{
   GLClearColorPTR      glClearColor;
   GLClearPTR           glClear;
   
   indexed internal initColorFns(GLClearColorPTR glClearColor, GLClearPTR glClear)
   {
      this glClearColor := glClearColor;
      this glClear := glClear;
   }
   
   clear(OVec4 color)
   {
      glClearColor(color.x, color.y, color.z, color.w);
      glClear(GL_COLOR_BUFFER_BIT);
   }
}

public sealed class GLADLibrary
{
   int          _major; 
   int          _minor;
   GLAD_Wrapper _core;
   
   constructor()
   {
      _core := $();
   }

   IGLCanvas newCanvas()
   {
      GLADCanvas canvas := $();
      
      _core.initCanvas(canvas);
      
      ^ canvas
   }

   private string? loadVersionStr()
   {
      auto glGetString := _core.GLGetString;

      pointer verPtr := glGetString(GL_VERSION);
      string versions := *verPtr;

      ^ versions;
   }
      
   private string? loadExtensionsStr()
   {
      auto glGetString := _core.GLGetString;

      pointer ptr := glGetString(GL_NUM_EXTENSIONS);
      if (ptr.isUnassigned()) {
         ^ nil
      };
      
      ^ ptr.getStringUnsafe();
   }
      
   (int, int) getVersion()
   {
      ^ (_major, _minor)
   }
      
   bool load()
   {
      string? versions := loadVersionStr();
      if:nil(versions) {
         ^ false
      };
      
      string? extensions := loadExtensionsStr();
      if:nil(extensions) {
         ^ false
      };

      GLAD_Config config := GLAD_Config.load(versions!, extensions!);

      if:not(_core.load(config)) {
         ^ false
      };

      _major := config.Major;
      _minor := config.Minor;

      ^ true
   }
   
}
