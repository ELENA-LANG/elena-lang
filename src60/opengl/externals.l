import system'drawing;
import extensions;

use extern OPENGL : importing("OPENGL32");

public const struct HGLRC
{
   embeddable handle _handle;
   
   bool isUnassigned()
      = _handle.isUnassigned();
   
   constructor(handle handle)
   {
      _handle := handle
   }
   
   makeCurrent(HDC dc)
   {
      int retVal := OPENGL.wglMakeCurrent(dc, _handle);
   }
}

use GLenum = system'IntNumber;
use GLint = system'IntNumber;
use GLfloat = /*system'FloatNumber*/system'IntNumber;
use GLsizei = system'IntNumber;
use GLboolean = system'IntNumber;
use GLdouble = system'RealNumber;
use GLuint = system'UIntNumber;
use GLbyte = system'SByteNumber;
use GLshort = system'ShortNumber;
use GLushort = system'UShortNumber;
use GLubyte = system'ByteNumber;

internal struct WGLGetProcAddress      : extern_winapi_func<pointer>(string name);

internal struct GLGetStringPTR         : extern_winapi_func<pointer>(int name);

internal struct GLCullFacePTR          : extern_winapi_func(int mode);
internal struct GLFrontFacePTR         : extern_winapi_func(int mode);
internal struct GLHintPTR              : extern_winapi_func(int target, int mode);
internal struct GLLineWidthPTR         : extern_winapi_func(/*GLfloat*/int width);
internal struct GLPointSizePTR         : extern_winapi_func(/*GLfloat*/int width);
internal struct GLPolygonModePTR       : extern_winapi_func(int face, int mode);
internal struct GLScissorPTR           : extern_winapi_func(int x, int y, int width, int height);
internal struct GLTexParameterfPTR     : extern_winapi_func(int target, int pname, /*GLfloat*/int param);
internal struct GLTexParameterfvPTR    : extern_winapi_func(int target, int pname, pointer params);
internal struct GLTexParameteriPTR     : extern_winapi_func(int target, int pname, int param);
internal struct GLTexParameterivPTR    : extern_winapi_func(int target, int pname, pointer params);
internal struct GLTexImage1DPTR        : extern_winapi_func(int target, int level, int internalformat, int width, int border, int format, int type, pointer pixels);
internal struct GLTexImage2DPTR        : extern_winapi_func(int target, int level, int internalformat, int width, int height, int border, int format, int type, pointer pixels);
internal struct GLDrawBufferPTR        : extern_winapi_func(int buf);
internal struct GLClearPTR             : extern_winapi_func(int mask);
internal struct GLClearColorPTR        : extern_winapi_func(/*GLfloat*/int red, /*GLfloat*/int green, /*GLfloat*/int blue, /*GLfloat*/int alpha);
internal struct GLClearStencilPTR      : extern_winapi_func(real depth);
internal struct GLClearDepthPTR        : extern_winapi_func(uint mask);
internal struct GLStencilMaskPTR       : extern_winapi_func(byte red, byte green, byte blue, byte alpha);
internal struct GLColorMaskPTR         : extern_winapi_func(byte flag);
internal struct GLDepthMaskPTR         : extern_winapi_func(int cap);
internal struct GLDisablePTR           : extern_winapi_func(int cap);
internal struct GLEnablePTR            : extern_winapi_func(int cap);
internal struct GLFinishPTR            : extern_winapi_func();
internal struct GLFlushPTR             : extern_winapi_func();
internal struct GLBlendFuncPTR         : extern_winapi_func(int opcode);
internal struct GLLogicOpPTR           : extern_winapi_func(int /*func*/fnc, int ref, uint mask);
internal struct GLStencilFuncPTR       : extern_winapi_func(int fail, int zfail, int zpass);
internal struct GLStencilOpPTR         : extern_winapi_func(int /*func*/fnc);
internal struct GLDepthFuncPTR         : extern_winapi_func(int pname, /*GLfloat*/int param);
internal struct GLPixelStorefPTR       : extern_winapi_func(GLenum pname, GLfloat param);
internal struct GLPixelStoreiPTR       : extern_winapi_func(GLenum pname, GLint param);
internal struct GLReadBufferPTR        : extern_winapi_func(GLenum src);
internal struct GLReadPixelsPTR        : extern_winapi_func(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, pointer pixels);
internal struct GLGetBooleanvPTR       : extern_winapi_func(GLenum pname, /*GLboolean * */pointer data);
internal struct GLGetDoublevPTR        : extern_winapi_func(GLenum pname, /*GLdouble * */pointer data);
internal struct GLGetErrorPTR          : extern_winapi_func();
internal struct GLGetFloatvPTR         : extern_winapi_func(GLenum pname, /*GLfloat * */pointer data);
internal struct GLGetIntegervPTR       : extern_winapi_func(GLenum pname, /*GLint * */ pointer data);
//internal struct GLGetStringPTR         : extern_winapi_func<pointer>(GLenum name);
internal struct GLGetTexImagePTR       : extern_winapi_func(GLenum target, GLint level, GLenum format, GLenum type, /*void * */pointer pixels);
internal struct GLGetTexParameterfvPTR        : extern_winapi_func(GLenum target, GLenum pname, /*GLfloat * */ pointer params);
internal struct GLGetTexParameterivPTR        : extern_winapi_func(GLenum target, GLenum pname, /*GLint * */ pointer params);
internal struct GLGetTexLevelParameterfvPTR   : extern_winapi_func(GLenum target, GLint level, GLenum pname, /*GLfloat * */pointer params);
internal struct GLGetTexLevelParameterivPTR   : extern_winapi_func(GLenum target, GLint level, GLenum pname, /*GLint * */ pointer params);
internal struct GLIsEnabledPTR         : extern_winapi_func<GLboolean>(GLenum cap);
internal struct GLDepthRangePTR        : extern_winapi_func(GLdouble n, GLdouble f);
internal struct GLViewportPTR          : extern_winapi_func(GLint x, GLint y, GLsizei width, GLsizei height);
internal struct GLNewListPTR           : extern_winapi_func(GLuint list, GLenum mode);
internal struct GLEndListPTR           : extern_winapi_func();
internal struct GLCallListPTR          : extern_winapi_func(GLuint list);
internal struct GLCallListsPTR         : extern_winapi_func(GLsizei n, GLenum type, /*const void * */pointer lists);
internal struct GLDeleteListsPTR       : extern_winapi_func(GLuint list, GLsizei range);
internal struct GLGenListsPTR          : extern_winapi_func(GLsizei range);
internal struct GLListBasePTR          : extern_winapi_func(GLuint base);
internal struct GLBeginPTR             : extern_winapi_func(GLenum mode);
internal struct GLBitmapPTR            : extern_winapi_func(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, /*const GLubyte * */pointer bitmap);
internal struct GLColor3bPTR           : extern_winapi_func(GLbyte red, GLbyte green, GLbyte blue);
internal struct GLColor3bvPTR          : extern_winapi_func(/*const GLbyte * */pointer v);
internal struct GLColor3dPTR           : extern_winapi_func(GLdouble red, GLdouble green, GLdouble blue);
internal struct GLColor3dvPTR          : extern_winapi_func(/*const GLdouble * */pointer v);
internal struct GLColor3fPTR           : extern_winapi_func(GLfloat red, GLfloat green, GLfloat blue);
internal struct GLColor3fvPTR          : extern_winapi_func(/*const GLfloat * */pointer v);
internal struct GLColor3iPTR           : extern_winapi_func(GLint red, GLint green, GLint blue);
internal struct GLColor3ivPTR          : extern_winapi_func(/*const GLint * */pointer v);
internal struct GLColor3sPTR           : extern_winapi_func(GLshort red, GLshort green, GLshort blue);
internal struct GLColor3svPTR        : extern_winapi_func(/*const GLshort * */pointer v);
internal struct GLColor3ubPTR        : extern_winapi_func(GLubyte red, GLubyte green, GLubyte blue);
internal struct GLColor3ubvPTR       : extern_winapi_func(/*const GLubyte * */pointer v);
internal struct GLColor3uiPTR        : extern_winapi_func(GLuint red, GLuint green, GLuint blue);
internal struct GLColor3uivPTR       : extern_winapi_func(/*const GLuint * */pointer v);
internal struct GLColor3usPTR        : extern_winapi_func(GLushort red, GLushort green, GLushort blue);
internal struct GLColor3usvPTR       : extern_winapi_func(/*const GLushort * */pointer v);
internal struct GLColor4bPTR           : extern_winapi_func(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
internal struct GLColor4bvPTR          : extern_winapi_func(/*const GLbyte * */pointer v);
internal struct GLColor4dPTR           : extern_winapi_func(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
internal struct GLColor4dvPTR          : extern_winapi_func(/*const GLdouble * */pointer v);
internal struct GLColor4fPTR           : extern_winapi_func(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
internal struct GLColor4fvPTR          : extern_winapi_func(/*const GLfloat * */pointer v);
internal struct GLColor4iPTR           : extern_winapi_func(GLint red, GLint green, GLint blue, GLint alpha);
internal struct GLColor4ivPTR          : extern_winapi_func(/*const GLint * */pointer v);
internal struct GLColor4sPTR           : extern_winapi_func(GLshort red, GLshort green, GLshort blue, GLshort alpha);
internal struct GLColor4svPTR        : extern_winapi_func(/*const GLshort * */pointer v);
internal struct GLColor4ubPTR        : extern_winapi_func(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
internal struct GLColor4ubvPTR       : extern_winapi_func(/*const GLubyte * */pointer v);
internal struct GLColor4uiPTR        : extern_winapi_func(GLuint red, GLuint green, GLuint blue, GLuint alpha);
internal struct GLColor4uivPTR       : extern_winapi_func(/*const GLuint * */pointer v);
internal struct GLColor4usPTR        : extern_winapi_func(GLushort red, GLushort green, GLushort blue, GLushort alpha);
internal struct GLColor4usvPTR       : extern_winapi_func(/*const GLushort * */pointer v);
internal struct GLEdgeFlagPTR        : extern_winapi_func(GLboolean flag);
internal struct GLEdgeFlagvPTR       : extern_winapi_func(/*const GLboolean * */pointer flag);
internal struct GLEndPTR             : extern_winapi_func();
internal struct GLIndexdPTR          : extern_winapi_func(GLdouble c);
internal struct GLIndexdvPTR         : extern_winapi_func(/*const GLdouble * */pointer c);
internal struct GLIndexfPTR          : extern_winapi_func(GLfloat c);
internal struct GLIndexfvPTR         : extern_winapi_func(/*const GLfloat * */pointer c);
internal struct GLIndexiPTR          : extern_winapi_func(GLint c);
internal struct GLIndexivPTR         : extern_winapi_func(/*const GLint * */pointer c);
internal struct GLIndexsPTR          : extern_winapi_func(GLshort c);
internal struct GLIndexsvPTR         : extern_winapi_func(/*const GLshort * */pointer c);
internal struct GLNormal3bPTR        : extern_winapi_func(GLbyte nx, GLbyte ny, GLbyte nz);
internal struct GLNormal3bvPTR       : extern_winapi_func(/*const GLbyte * */pointer v);
internal struct GLNormal3dPTR        : extern_winapi_func(GLdouble nx, GLdouble ny, GLdouble nz);
internal struct GLNormal3dvPTR       : extern_winapi_func(/*const GLdouble * */pointer v);
internal struct GLNormal3fPTR        : extern_winapi_func(GLfloat nx, GLfloat ny, GLfloat nz);
internal struct GLNormal3fvPTR       : extern_winapi_func(/*const GLfloat * */pointer v);
internal struct GLNormal3iPTR        : extern_winapi_func(GLint nx, GLint ny, GLint nz);
internal struct GLNormal3ivPTR       : extern_winapi_func(/*const GLint * */pointer v);
internal struct GLNormal3sPTR        : extern_winapi_func(GLshort nx, GLshort ny, GLshort nz);
internal struct GLNormal3svPTR       : extern_winapi_func(/*const GLshort * */pointer v);
internal struct GLRasterPos2dPTR     : extern_winapi_func(GLdouble x, GLdouble y);
internal struct GLRasterPos2dvPTR    : extern_winapi_func(/*const GLdouble * */pointer v);
internal struct GLRasterPos2fPTR     : extern_winapi_func(GLfloat x, GLfloat y);
internal struct GLRasterPos2fvPTR    : extern_winapi_func(/*const GLfloat * */pointer v);
internal struct GLRasterPos2iPTR     : extern_winapi_func(GLint x, GLint y);
internal struct GLRasterPos2ivPTR    : extern_winapi_func(/*const GLint * */pointer v);
internal struct GLRasterPos2sPTR     : extern_winapi_func(GLshort x, GLshort y);
internal struct GLRasterPos2svPTR    : extern_winapi_func(/*const GLshort * */pointer v);
internal struct GLRasterPos3dPTR     : extern_winapi_func(GLdouble x, GLdouble y, GLdouble z);
internal struct GLRasterPos3dvPTR    : extern_winapi_func(/*const GLdouble * */pointer v);
internal struct GLRasterPos3fPTR     : extern_winapi_func(GLfloat x, GLfloat y, GLfloat z);
internal struct GLRasterPos3fvPTR    : extern_winapi_func(/*const GLfloat * */pointer v);
internal struct GLRasterPos3iPTR     : extern_winapi_func(GLint x, GLint y, GLint z);
internal struct GLRasterPos3ivPTR    : extern_winapi_func(/*const GLint * */pointer v);
internal struct GLRasterPos3sPTR     : extern_winapi_func(GLshort x, GLshort y, GLshort z);
internal struct GLRasterPos3svPTR    : extern_winapi_func(/*const GLshort * */pointer v);
internal struct GLRasterPos4dPTR     : extern_winapi_func(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
internal struct GLRasterPos4dvPTR    : extern_winapi_func(/*const GLdouble * */pointer v);
internal struct GLRasterPos4fPTR     : extern_winapi_func(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
internal struct GLRasterPos4fvPTR    : extern_winapi_func(/*const GLfloat * */pointer v);
internal struct GLRasterPos4iPTR     : extern_winapi_func(GLint x, GLint y, GLint z, GLint w);
internal struct GLRasterPos4ivPTR    : extern_winapi_func(/*const GLint * */pointer v);
internal struct GLRasterPos4sPTR     : extern_winapi_func(GLshort x, GLshort y, GLshort z, GLshort w);
internal struct GLRasterPos4svPTR    : extern_winapi_func(/*const GLshort * */pointer v);
internal struct GLRectdPTR           : extern_winapi_func(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
internal struct GLRectdvPTR          : extern_winapi_func(/*const GLdouble * */pointer v1, /*const GLdouble * */pointer v2);
internal struct GLRectfPTR           : extern_winapi_func(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
internal struct GLRectfvPTR          : extern_winapi_func(/*const GLfloat * */pointer v1, /*const GLfloat * */pointer v2);
internal struct GLRectiPTR           : extern_winapi_func(GLint x1, GLint y1, GLint x2, GLint y2);
internal struct GLRectivPTR          : extern_winapi_func(/*const GLint * */pointer v1, /*const GLint * */pointer v2);
internal struct GLRectsPTR           : extern_winapi_func(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
internal struct GLRectsvPTR          : extern_winapi_func(/*const GLshort * */pointer v1, /*const GLshort * */pointer v2);

const string[] Prefixes = $[ "OpenGL ES-CM ", "OpenGL ES-CL ", "OpenGL ES " ];

// --- GLContext ---

public struct GLContext
{
   handle libGL;
   
   // opengl functions
//   WGLGetProcAddress    get_proc;
   
   GLGetStringPTR       glGetString;
   
   int                  GLAD_GL_VERSION_1_0;
   int                  GLAD_GL_VERSION_1_1;
   int                  GLAD_GL_VERSION_1_2;
   int                  GLAD_GL_VERSION_1_3;
   int                  GLAD_GL_VERSION_1_4;
   int                  GLAD_GL_VERSION_1_5;
   int                  GLAD_GL_VERSION_2_0;
   int                  GLAD_GL_VERSION_2_1;
   int                  GLAD_GL_VERSION_3_0;
   int                  GLAD_GL_VERSION_3_1;
   int                  GLAD_GL_VERSION_3_2;
   int                  GLAD_GL_VERSION_3_3;
   int                  GLAD_GL_VERSION_4_0;
   
   // 1_0
   GLCullFacePTR        glCullFace;
	GLFrontFacePTR       glFrontFace;
	GLHintPTR            glHint;
	GLLineWidthPTR       glLineWidth;
   GLPointSizePTR       glPointSize;
   GLPolygonModePTR     glPolygonMode;
   GLScissorPTR         glScissor;
   GLTexParameterfPTR   glTexParameterf;
   GLTexParameterfvPTR  glTexParameterfv;
   GLTexParameteriPTR   glTexParameteri;
   GLTexParameterivPTR  glTexParameteriv;
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
   GLPixelStorefPTR     glPixelStoref;
   GLPixelStoreiPTR     glPixelStorei;
   GLReadBufferPTR      glReadBuffer;
   GLReadPixelsPTR      glReadPixels;
   GLGetBooleanvPTR     glGetBooleanv;
   GLGetDoublevPTR      glGetDoublev;
   GLGetErrorPTR        glGetError;
   GLGetFloatvPTR       glGetFloatv;
   GLGetIntegervPTR     glGetIntegerv;
//   GLGetStringPTR       glGetString;
   GLGetTexImagePTR     glGetTexImage;
   GLGetTexParameterfvPTR        glGetTexParameterfv;
   GLGetTexParameterivPTR        glGetTexParameteriv;
   GLGetTexLevelParameterfvPTR   glGetTexLevelParameterfv;
   GLGetTexLevelParameterivPTR   glGetTexLevelParameteriv;
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
   GLColor3bPTR         glColor3b;
   GLColor3bvPTR        glColor3bv;
   GLColor3dPTR         glColor3d;
   GLColor3dvPTR        glColor3dv;
   GLColor3fPTR         glColor3f;
   GLColor3fvPTR        glColor3fv;
   GLColor3iPTR         glColor3i;
   GLColor3ivPTR        glColor3iv;
   GLColor3sPTR         glColor3s;
   GLColor3svPTR        glColor3sv;
   GLColor3ubPTR        glColor3ub;
   GLColor3ubvPTR       glColor3ubv;
   GLColor3uiPTR        glColor3ui;
   GLColor3uivPTR       glColor3uiv;
   GLColor3usPTR        glColor3us;
   GLColor3usvPTR       glColor3usv;
   GLColor4bPTR         glColor4b;
   GLColor4bvPTR        glColor4bv;
   GLColor4dPTR         glColor4d;
   GLColor4dvPTR        glColor4dv;
   GLColor4fPTR         glColor4f;
   GLColor4fvPTR        glColor4fv;
   GLColor4iPTR         glColor4i;
   GLColor4ivPTR        glColor4iv;
   GLColor4sPTR         glColor4s;
   GLColor4svPTR        glColor4sv;
   GLColor4ubPTR        glColor4ub;
   GLColor4ubvPTR       glColor4ubv;
   GLColor4uiPTR        glColor4ui;
   GLColor4uivPTR       glColor4uiv;
   GLColor4usPTR        glColor4us;
   GLColor4usvPTR       glColor4usv;
   GLEdgeFlagPTR        glEdgeFlag;
   GLEdgeFlagvPTR       glEdgeFlagv;
   GLEndPTR             glEnd;
   GLIndexdPTR          glIndexd;
   GLIndexdvPTR         glIndexdv;
   GLIndexfPTR          glIndexf;
   GLIndexfvPTR         glIndexfv;
   GLIndexiPTR          glIndexi;
   GLIndexivPTR         glIndexiv;
   GLIndexsPTR          glIndexs;
   GLIndexsvPTR         glIndexsv;      
   GLNormal3bPTR        glNormal3b;
   GLNormal3bvPTR       glNormal3bv;
   GLNormal3dPTR        glNormal3d;
   GLNormal3dvPTR       glNormal3dv;
   GLNormal3fPTR        glNormal3f;
   GLNormal3fvPTR       glNormal3fv;
   GLNormal3iPTR        glNormal3i;
   GLNormal3ivPTR       glNormal3iv;
   GLNormal3sPTR        glNormal3s;
   GLNormal3svPTR       glNormal3sv;
   GLRasterPos2dPTR     glRasterPos2d;
   GLRasterPos2dvPTR    glRasterPos2dv;
   GLRasterPos2fPTR     glRasterPos2f;
   GLRasterPos2fvPTR    glRasterPos2fv;
   GLRasterPos2iPTR     glRasterPos2i;
   GLRasterPos2ivPTR    glRasterPos2iv;
   GLRasterPos2sPTR     glRasterPos2s;
   GLRasterPos2svPTR    glRasterPos2sv;
   GLRasterPos3dPTR     glRasterPos3d;
   GLRasterPos3dvPTR    glRasterPos3dv;
   GLRasterPos3fPTR     glRasterPos3f;
   GLRasterPos3fvPTR    glRasterPos3fv;
   GLRasterPos3iPTR     glRasterPos3i;
   GLRasterPos3ivPTR    glRasterPos3iv;
   GLRasterPos3sPTR     glRasterPos3s;
   GLRasterPos3svPTR    glRasterPos3sv;   
   GLRasterPos4dPTR     glRasterPos4d;
   GLRasterPos4dvPTR    glRasterPos4dv;
   GLRasterPos4fPTR     glRasterPos4f;
   GLRasterPos4fvPTR    glRasterPos4fv;
   GLRasterPos4iPTR     glRasterPos4i;
   GLRasterPos4ivPTR    glRasterPos4iv;
   GLRasterPos4sPTR     glRasterPos4s;
   GLRasterPos4svPTR    glRasterPos4sv;
                                                                        
   GLRectdPTR           glRectd;
   GLRectdvPTR          glRectdv;
   GLRectfPTR           glRectf;
   GLRectfvPTR          glRectfv;
   GLRectiPTR           glRecti;
   GLRectivPTR          glRectiv;
   GLRectsPTR           glRects;
   GLRectsvPTR          glRectsv;
                                                                                                                                                                                                                        
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      
//      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
      glGetString := GLGetStringPTR.load(libGL, "glGetString");
   }

   private find_coreGL(IOpenGLEngine engine)
   {
      pointer verPtr := glGetString(GL_VERSION);
      string versions := *verPtr;
      
      int prefixLen := Prefixes.Length;
      int start := 0;
      for (int i := 0; i < prefixLen; i++) {
         if (versions.startingWith(Prefixes[i])) {
            start := Prefixes[i].Length;
            
            :break;
         }
      };
      
      int dotPos := versions.indexOf(start, $46);
      int major := versions.Substring(start, dotPos - start).toInt();
      start:= dotPos + 1;
      dotPos := versions.indexOf(start, $46);
      int minor := versions.Substring(start, dotPos - start).toInt();
      
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
/*      if (GLVersion.major > 4 || (GLVersion.major >= 4 && GLVersion.minor >= 0)) {
      	max_loaded_major = 4;
      	max_loaded_minor = 0;
      }*/
      
      engine.setVersion(major, minor);
   }

   private load_GL_VERSION_1_0()
   {
      if (GLAD_GL_VERSION_1_0 == 0) {
         ^ self
      };
      
   glCullFace := GLCullFacePTR.load(libGL, "glCullFace");
   glFrontFace := GLFrontFacePTR.load(libGL, "glFrontFace");
   glHint := GLHintPTR.load(libGL, "glHint");
   glLineWidth := GLLineWidthPTR.load(libGL, "glLineWidth");
   
   glPointSize := GLPointSizePTR.load(libGL, "glPointSize");
   glPolygonMode := GLPolygonModePTR.load(libGL, "glPolygonMode");
   glScissor := GLScissorPTR.load(libGL, "glScissor");
   glTexParameterf := GLTexParameterfPTR.load(libGL, "glTexParameterf");
   glTexParameterfv := GLTexParameterfvPTR.load(libGL, "glTexParameterfv");
   glTexParameteri := GLTexParameteriPTR.load(libGL, "glTexParameteri");
   glTexParameteriv := GLTexParameterivPTR.load(libGL, "glTexParameteriv");
   glTexImage1D := GLTexImage1DPTR.load(libGL, "glTexImage1D");
   glTexImage2D := GLTexImage2DPTR.load(libGL, "glTexImage2D");
   glDrawBuffer := GLDrawBufferPTR.load(libGL, "glDrawBuffer");
   glClear := GLClearPTR.load(libGL, "glClear");
   glClearColor := GLClearColorPTR.load(libGL, "glClearColor");
   glClearStencil := GLClearStencilPTR.load(libGL, "glClearStencil");
   glClearDepth := GLClearDepthPTR.load(libGL, "glClearDepth");
   glStencilMask := GLStencilMaskPTR.load(libGL, "glStencilMask");
   glColorMask := GLColorMaskPTR.load(libGL, "glColorMask");
   glDepthMask := GLDepthMaskPTR.load(libGL, "glDepthMask");
   glDisable := GLDisablePTR.load(libGL, "glDisable");
   glEnable := GLEnablePTR.load(libGL, "glEnable");
   glFinish := GLFinishPTR.load(libGL, "glFinish");
   glFlush := GLFlushPTR.load(libGL, "glFlush");
   glBlendFunc := GLBlendFuncPTR.load(libGL, "glBlendFunc");
   glLogicOp := GLLogicOpPTR.load(libGL, "glLogicOp");
   glStencilFunc := GLStencilFuncPTR.load(libGL, "glStencilFunc");
   glStencilOp := GLStencilOpPTR.load(libGL, "glStencilOp");
   glDepthFunc := GLDepthFuncPTR.load(libGL, "glDepthFunc");
   glPixelStoref := GLPixelStorefPTR.load(libGL, "glPixelStoref");
   glPixelStorei := GLPixelStoreiPTR.load(libGL, "glPixelStorei");
   glReadBuffer := GLReadBufferPTR.load(libGL, "glReadBuffer");
   glReadPixels := GLReadPixelsPTR.load(libGL, "glReadPixels");
   glGetBooleanv := GLGetBooleanvPTR.load(libGL, "glGetBooleanv");
   glGetDoublev := GLGetDoublevPTR.load(libGL, "glGetDoublev");
   glGetError := GLGetErrorPTR.load(libGL, "glGetError"); 
   glGetFloatv := GLGetFloatvPTR.load(libGL, "glGetFloatv");
   glGetIntegerv := GLGetIntegervPTR.load(libGL, "glGetIntegerv");
//   glGetString := GLGetStringPTR.load(libGL, "glGetString");
   glGetTexImage := GLGetTexImagePTR.load(libGL, "glGetTexImage");
   glGetTexParameterfv := GLGetTexParameterfvPTR.load(libGL, "glGetTexParameterfv");
   glGetTexParameteriv := GLGetTexParameterivPTR.load(libGL, "glGetTexParameteriv");
   glGetTexLevelParameterfv := GLGetTexLevelParameterfvPTR.load(libGL, "glGetTexLevelParameterfv");
   glGetTexLevelParameteriv := GLGetTexLevelParameterivPTR.load(libGL, "glGetTexLevelParameteriv");
   glIsEnabled := GLIsEnabledPTR.load(libGL, "glIsEnabled");
   glDepthRange := GLDepthRangePTR.load(libGL, "glDepthRange");
   glViewport := GLViewportPTR.load(libGL, "glViewport");
   glNewList := GLNewListPTR.load(libGL, "glNewList");
   glEndList := GLEndListPTR.load(libGL, "glEndList");
   glCallList := GLCallListPTR.load(libGL, "glCallList");
   glCallLists := GLCallListsPTR.load(libGL, "glCallLists");
   glDeleteLists := GLDeleteListsPTR.load(libGL, "glDeleteLists");
   glGenLists := GLGenListsPTR.load(libGL, "glGenLists");
   glListBase := GLListBasePTR.load(libGL, "glListBase");
   glBegin := GLBeginPTR.load(libGL, "glBegin");
   glBitmap := GLBitmapPTR.load(libGL, "glBitmap");
   glColor3b := GLColor3bPTR.load(libGL, "glColor3b");
   glColor3bv := GLColor3bvPTR.load(libGL, "glColor3bv");
   glColor3d := GLColor3dPTR.load(libGL, "glColor3d");
   glColor3dv := GLColor3dvPTR.load(libGL, "glColor3dv");
   glColor3f := GLColor3fPTR.load(libGL, "glColor3f");
   glColor3fv := GLColor3fvPTR.load(libGL, "glColor3fv");
   glColor3i := GLColor3iPTR.load(libGL, "glColor3i");
   glColor3iv := GLColor3ivPTR.load(libGL, "glColor3iv");
   glColor3s := GLColor3sPTR.load(libGL, "glColor3s"); 
   glColor3sv := GLColor3svPTR.load(libGL, "glColor3sv");
   glColor3ub := GLColor3ubPTR.load(libGL, "glColor3ub");
   glColor3ubv := GLColor3ubvPTR.load(libGL, "glColor3ubv");
   glColor3ui := GLColor3uiPTR.load(libGL, "glColor3ui");
   glColor3uiv := GLColor3uivPTR.load(libGL, "glColor3uiv");
   glColor3us := GLColor3usPTR.load(libGL, "glColor3us");
   glColor3usv := GLColor3usvPTR.load(libGL, "glColor3usv");   
   glColor4b := GLColor4bPTR.load(libGL, "glColor4b");
   glColor4bv := GLColor4bvPTR.load(libGL, "glColor4bv");
   glColor4d := GLColor4dPTR.load(libGL, "glColor4d");
   glColor4dv := GLColor4dvPTR.load(libGL, "glColor4dv");
   glColor4f := GLColor4fPTR.load(libGL, "glColor4f");
   glColor4fv := GLColor4fvPTR.load(libGL, "glColor4fv");
   glColor4i := GLColor4iPTR.load(libGL, "glColor4i");
   glColor4iv := GLColor4ivPTR.load(libGL, "glColor4iv");
   glColor4s := GLColor4sPTR.load(libGL, "glColor4s");
   glColor4sv := GLColor4svPTR.load(libGL, "glColor4sv");
   glColor4ub := GLColor4ubPTR.load(libGL, "glColor4ub");
   glColor4ubv := GLColor4ubvPTR.load(libGL, "glColor4ubv");
   glColor4ui := GLColor4uiPTR.load(libGL, "glColor4ui");
   glColor4uiv := GLColor4uivPTR.load(libGL, "glColor4uiv");
   glColor4us := GLColor4usPTR.load(libGL, "glColor4us");
   glColor4usv := GLColor4usvPTR.load(libGL, "glColor4usv");
   glEdgeFlag := GLEdgeFlagPTR.load(libGL, "glEdgeFlag");
   glEdgeFlagv := GLEdgeFlagvPTR.load(libGL, "glEdgeFlagv");
   glEnd := GLEndPTR.load(libGL, "glEnd");
   glIndexd := GLIndexdPTR.load(libGL, "glIndexd");
   glIndexdv := GLIndexdvPTR.load(libGL, "glIndexdv");
   glIndexf := GLIndexfPTR.load(libGL, "glIndexf");
   glIndexfv := GLIndexfvPTR.load(libGL, "glIndexfv");
   glIndexi := GLIndexiPTR.load(libGL, "glIndexi");
   glIndexiv := GLIndexivPTR.load(libGL, "glIndexiv");
   glIndexs := GLIndexsPTR.load(libGL, "glIndexs");
   glIndexsv := GLIndexsvPTR.load(libGL, "glIndexsv");
   glNormal3b := GLNormal3bPTR.load(libGL, "glNormal3b");
   glNormal3bv := GLNormal3bvPTR.load(libGL, "glNormal3bv");
   glNormal3d := GLNormal3dPTR.load(libGL, "glNormal3d");
   glNormal3dv := GLNormal3dvPTR.load(libGL, "glNormal3dv");
   glNormal3f := GLNormal3fPTR.load(libGL, "glNormal3f");
   glNormal3fv := GLNormal3fvPTR.load(libGL, "glNormal3fv");
   glNormal3i := GLNormal3iPTR.load(libGL, "glNormal3i");
   glNormal3iv := GLNormal3ivPTR.load(libGL, "glNormal3iv");
   glNormal3s := GLNormal3sPTR.load(libGL, "glNormal3s");
   glNormal3sv := GLNormal3svPTR.load(libGL, "glNormal3sv");
   glRasterPos2d := GLRasterPos2dPTR.load(libGL, "glRasterPos2d");
   glRasterPos2dv := GLRasterPos2dvPTR.load(libGL, "glRasterPos2dv");
   glRasterPos2f := GLRasterPos2fPTR.load(libGL, "glRasterPos2f");
   glRasterPos2fv := GLRasterPos2fvPTR.load(libGL, "glRasterPos2fv");
   glRasterPos2i := GLRasterPos2iPTR.load(libGL, "glRasterPos2i");
   glRasterPos2iv := GLRasterPos2ivPTR.load(libGL, "glRasterPos2iv");
   glRasterPos2s := GLRasterPos2sPTR.load(libGL, "glRasterPos2s");
   glRasterPos2sv := GLRasterPos2svPTR.load(libGL, "glRasterPos2sv"); 
   glRasterPos3d := GLRasterPos3dPTR.load(libGL, "glRasterPos3d");
   glRasterPos3dv := GLRasterPos3dvPTR.load(libGL, "glRasterPos3dv");
   glRasterPos3f := GLRasterPos3fPTR.load(libGL, "glRasterPos3f");
   glRasterPos3fv := GLRasterPos3fvPTR.load(libGL, "glRasterPos3fv");
   glRasterPos3i := GLRasterPos3iPTR.load(libGL, "glRasterPos3i");
   glRasterPos3iv := GLRasterPos3ivPTR.load(libGL, "glRasterPos3iv");
   glRasterPos3s := GLRasterPos3sPTR.load(libGL, "glRasterPos3s");
   glRasterPos3sv := GLRasterPos3svPTR.load(libGL, "glRasterPos3sv");
   glRasterPos4d := GLRasterPos4dPTR.load(libGL, "glRasterPos4d");
   glRasterPos4dv := GLRasterPos4dvPTR.load(libGL, "glRasterPos4dv");
   glRasterPos4f := GLRasterPos4fPTR.load(libGL, "glRasterPos4f");
   glRasterPos4fv := GLRasterPos4fvPTR.load(libGL, "glRasterPos4fv");
   glRasterPos4i := GLRasterPos4iPTR.load(libGL, "glRasterPos4i");
   glRasterPos4iv := GLRasterPos4ivPTR.load(libGL, "glRasterPos4iv");
   glRasterPos4s := GLRasterPos4sPTR.load(libGL, "glRasterPos4s");
   glRasterPos4sv := GLRasterPos4svPTR.load(libGL, "glRasterPos4sv"); 
   glRectd := GLRectdPTR.load(libGL, "glRectd");
   glRectdv := GLRectdvPTR.load(libGL, "glRectdv");
   glRectf := GLRectfPTR.load(libGL, "glRectf");
   glRectfv := GLRectfvPTR.load(libGL, "glRectfv");
   glRecti := GLRectiPTR.load(libGL, "glRecti");
   glRectiv := GLRectivPTR.load(libGL, "glRectiv");
   glRects := GLRectsPTR.load(libGL, "glRects");
   glRectsv := GLRectsvPTR.load(libGL, "glRectsv");
/* 
	glad_glTexCoord1d = (PFNGLTEXCOORD1DPROC)load("glTexCoord1d");
	glad_glTexCoord1dv = (PFNGLTEXCOORD1DVPROC)load("glTexCoord1dv");
	glad_glTexCoord1f = (PFNGLTEXCOORD1FPROC)load("glTexCoord1f");
	glad_glTexCoord1fv = (PFNGLTEXCOORD1FVPROC)load("glTexCoord1fv");
	glad_glTexCoord1i = (PFNGLTEXCOORD1IPROC)load("glTexCoord1i");
	glad_glTexCoord1iv = (PFNGLTEXCOORD1IVPROC)load("glTexCoord1iv");
	glad_glTexCoord1s = (PFNGLTEXCOORD1SPROC)load("glTexCoord1s");
	glad_glTexCoord1sv = (PFNGLTEXCOORD1SVPROC)load("glTexCoord1sv");
	glad_glTexCoord2d = (PFNGLTEXCOORD2DPROC)load("glTexCoord2d");
	glad_glTexCoord2dv = (PFNGLTEXCOORD2DVPROC)load("glTexCoord2dv");
	glad_glTexCoord2f = (PFNGLTEXCOORD2FPROC)load("glTexCoord2f");
	glad_glTexCoord2fv = (PFNGLTEXCOORD2FVPROC)load("glTexCoord2fv");
	glad_glTexCoord2i = (PFNGLTEXCOORD2IPROC)load("glTexCoord2i");
	glad_glTexCoord2iv = (PFNGLTEXCOORD2IVPROC)load("glTexCoord2iv");
	glad_glTexCoord2s = (PFNGLTEXCOORD2SPROC)load("glTexCoord2s");
	glad_glTexCoord2sv = (PFNGLTEXCOORD2SVPROC)load("glTexCoord2sv");
	glad_glTexCoord3d = (PFNGLTEXCOORD3DPROC)load("glTexCoord3d");
	glad_glTexCoord3dv = (PFNGLTEXCOORD3DVPROC)load("glTexCoord3dv");
	glad_glTexCoord3f = (PFNGLTEXCOORD3FPROC)load("glTexCoord3f");
	glad_glTexCoord3fv = (PFNGLTEXCOORD3FVPROC)load("glTexCoord3fv");
	glad_glTexCoord3i = (PFNGLTEXCOORD3IPROC)load("glTexCoord3i");
	glad_glTexCoord3iv = (PFNGLTEXCOORD3IVPROC)load("glTexCoord3iv");
	glad_glTexCoord3s = (PFNGLTEXCOORD3SPROC)load("glTexCoord3s");
	glad_glTexCoord3sv = (PFNGLTEXCOORD3SVPROC)load("glTexCoord3sv");
	glad_glTexCoord4d = (PFNGLTEXCOORD4DPROC)load("glTexCoord4d");
	glad_glTexCoord4dv = (PFNGLTEXCOORD4DVPROC)load("glTexCoord4dv");
	glad_glTexCoord4f = (PFNGLTEXCOORD4FPROC)load("glTexCoord4f");
	glad_glTexCoord4fv = (PFNGLTEXCOORD4FVPROC)load("glTexCoord4fv");
	glad_glTexCoord4i = (PFNGLTEXCOORD4IPROC)load("glTexCoord4i");
	glad_glTexCoord4iv = (PFNGLTEXCOORD4IVPROC)load("glTexCoord4iv");
	glad_glTexCoord4s = (PFNGLTEXCOORD4SPROC)load("glTexCoord4s");
	glad_glTexCoord4sv = (PFNGLTEXCOORD4SVPROC)load("glTexCoord4sv");
	glad_glVertex2d = (PFNGLVERTEX2DPROC)load("glVertex2d");
	glad_glVertex2dv = (PFNGLVERTEX2DVPROC)load("glVertex2dv");
	glad_glVertex2f = (PFNGLVERTEX2FPROC)load("glVertex2f");
	glad_glVertex2fv = (PFNGLVERTEX2FVPROC)load("glVertex2fv");
	glad_glVertex2i = (PFNGLVERTEX2IPROC)load("glVertex2i");
	glad_glVertex2iv = (PFNGLVERTEX2IVPROC)load("glVertex2iv");
	glad_glVertex2s = (PFNGLVERTEX2SPROC)load("glVertex2s");
	glad_glVertex2sv = (PFNGLVERTEX2SVPROC)load("glVertex2sv");
	glad_glVertex3d = (PFNGLVERTEX3DPROC)load("glVertex3d");
	glad_glVertex3dv = (PFNGLVERTEX3DVPROC)load("glVertex3dv");
	glad_glVertex3f = (PFNGLVERTEX3FPROC)load("glVertex3f");
	glad_glVertex3fv = (PFNGLVERTEX3FVPROC)load("glVertex3fv");
	glad_glVertex3i = (PFNGLVERTEX3IPROC)load("glVertex3i");
	glad_glVertex3iv = (PFNGLVERTEX3IVPROC)load("glVertex3iv");
	glad_glVertex3s = (PFNGLVERTEX3SPROC)load("glVertex3s");
	glad_glVertex3sv = (PFNGLVERTEX3SVPROC)load("glVertex3sv");
	glad_glVertex4d = (PFNGLVERTEX4DPROC)load("glVertex4d");
	glad_glVertex4dv = (PFNGLVERTEX4DVPROC)load("glVertex4dv");
	glad_glVertex4f = (PFNGLVERTEX4FPROC)load("glVertex4f");
	glad_glVertex4fv = (PFNGLVERTEX4FVPROC)load("glVertex4fv");
	glad_glVertex4i = (PFNGLVERTEX4IPROC)load("glVertex4i");
	glad_glVertex4iv = (PFNGLVERTEX4IVPROC)load("glVertex4iv");
	glad_glVertex4s = (PFNGLVERTEX4SPROC)load("glVertex4s");
	glad_glVertex4sv = (PFNGLVERTEX4SVPROC)load("glVertex4sv");
	glad_glClipPlane = (PFNGLCLIPPLANEPROC)load("glClipPlane");
	glad_glColorMaterial = (PFNGLCOLORMATERIALPROC)load("glColorMaterial");
	glad_glFogf = (PFNGLFOGFPROC)load("glFogf");
	glad_glFogfv = (PFNGLFOGFVPROC)load("glFogfv");
	glad_glFogi = (PFNGLFOGIPROC)load("glFogi");
	glad_glFogiv = (PFNGLFOGIVPROC)load("glFogiv");
	glad_glLightf = (PFNGLLIGHTFPROC)load("glLightf");
	glad_glLightfv = (PFNGLLIGHTFVPROC)load("glLightfv");
	glad_glLighti = (PFNGLLIGHTIPROC)load("glLighti");
	glad_glLightiv = (PFNGLLIGHTIVPROC)load("glLightiv");
	glad_glLightModelf = (PFNGLLIGHTMODELFPROC)load("glLightModelf");
	glad_glLightModelfv = (PFNGLLIGHTMODELFVPROC)load("glLightModelfv");
	glad_glLightModeli = (PFNGLLIGHTMODELIPROC)load("glLightModeli");
	glad_glLightModeliv = (PFNGLLIGHTMODELIVPROC)load("glLightModeliv");
	glad_glLineStipple = (PFNGLLINESTIPPLEPROC)load("glLineStipple");
	glad_glMaterialf = (PFNGLMATERIALFPROC)load("glMaterialf");
	glad_glMaterialfv = (PFNGLMATERIALFVPROC)load("glMaterialfv");
	glad_glMateriali = (PFNGLMATERIALIPROC)load("glMateriali");
	glad_glMaterialiv = (PFNGLMATERIALIVPROC)load("glMaterialiv");
	glad_glPolygonStipple = (PFNGLPOLYGONSTIPPLEPROC)load("glPolygonStipple");
	glad_glShadeModel = (PFNGLSHADEMODELPROC)load("glShadeModel");
	glad_glTexEnvf = (PFNGLTEXENVFPROC)load("glTexEnvf");
	glad_glTexEnvfv = (PFNGLTEXENVFVPROC)load("glTexEnvfv");
	glad_glTexEnvi = (PFNGLTEXENVIPROC)load("glTexEnvi");
	glad_glTexEnviv = (PFNGLTEXENVIVPROC)load("glTexEnviv");
	glad_glTexGend = (PFNGLTEXGENDPROC)load("glTexGend");
	glad_glTexGendv = (PFNGLTEXGENDVPROC)load("glTexGendv");
	glad_glTexGenf = (PFNGLTEXGENFPROC)load("glTexGenf");
	glad_glTexGenfv = (PFNGLTEXGENFVPROC)load("glTexGenfv");
	glad_glTexGeni = (PFNGLTEXGENIPROC)load("glTexGeni");
	glad_glTexGeniv = (PFNGLTEXGENIVPROC)load("glTexGeniv");
	glad_glFeedbackBuffer = (PFNGLFEEDBACKBUFFERPROC)load("glFeedbackBuffer");
	glad_glSelectBuffer = (PFNGLSELECTBUFFERPROC)load("glSelectBuffer");
	glad_glRenderMode = (PFNGLRENDERMODEPROC)load("glRenderMode");
	glad_glInitNames = (PFNGLINITNAMESPROC)load("glInitNames");
	glad_glLoadName = (PFNGLLOADNAMEPROC)load("glLoadName");
	glad_glPassThrough = (PFNGLPASSTHROUGHPROC)load("glPassThrough");
	glad_glPopName = (PFNGLPOPNAMEPROC)load("glPopName");
	glad_glPushName = (PFNGLPUSHNAMEPROC)load("glPushName");
	glad_glClearAccum = (PFNGLCLEARACCUMPROC)load("glClearAccum");
	glad_glClearIndex = (PFNGLCLEARINDEXPROC)load("glClearIndex");
	glad_glIndexMask = (PFNGLINDEXMASKPROC)load("glIndexMask");
	glad_glAccum = (PFNGLACCUMPROC)load("glAccum");
	glad_glPopAttrib = (PFNGLPOPATTRIBPROC)load("glPopAttrib");
	glad_glPushAttrib = (PFNGLPUSHATTRIBPROC)load("glPushAttrib");
	glad_glMap1d = (PFNGLMAP1DPROC)load("glMap1d");
	glad_glMap1f = (PFNGLMAP1FPROC)load("glMap1f");
	glad_glMap2d = (PFNGLMAP2DPROC)load("glMap2d");
	glad_glMap2f = (PFNGLMAP2FPROC)load("glMap2f");
	glad_glMapGrid1d = (PFNGLMAPGRID1DPROC)load("glMapGrid1d");
	glad_glMapGrid1f = (PFNGLMAPGRID1FPROC)load("glMapGrid1f");
	glad_glMapGrid2d = (PFNGLMAPGRID2DPROC)load("glMapGrid2d");
	glad_glMapGrid2f = (PFNGLMAPGRID2FPROC)load("glMapGrid2f");
	glad_glEvalCoord1d = (PFNGLEVALCOORD1DPROC)load("glEvalCoord1d");
	glad_glEvalCoord1dv = (PFNGLEVALCOORD1DVPROC)load("glEvalCoord1dv");
	glad_glEvalCoord1f = (PFNGLEVALCOORD1FPROC)load("glEvalCoord1f");
	glad_glEvalCoord1fv = (PFNGLEVALCOORD1FVPROC)load("glEvalCoord1fv");
	glad_glEvalCoord2d = (PFNGLEVALCOORD2DPROC)load("glEvalCoord2d");
	glad_glEvalCoord2dv = (PFNGLEVALCOORD2DVPROC)load("glEvalCoord2dv");
	glad_glEvalCoord2f = (PFNGLEVALCOORD2FPROC)load("glEvalCoord2f");
	glad_glEvalCoord2fv = (PFNGLEVALCOORD2FVPROC)load("glEvalCoord2fv");
	glad_glEvalMesh1 = (PFNGLEVALMESH1PROC)load("glEvalMesh1");
	glad_glEvalPoint1 = (PFNGLEVALPOINT1PROC)load("glEvalPoint1");
	glad_glEvalMesh2 = (PFNGLEVALMESH2PROC)load("glEvalMesh2");
	glad_glEvalPoint2 = (PFNGLEVALPOINT2PROC)load("glEvalPoint2");
	glad_glAlphaFunc = (PFNGLALPHAFUNCPROC)load("glAlphaFunc");
	glad_glPixelZoom = (PFNGLPIXELZOOMPROC)load("glPixelZoom");
	glad_glPixelTransferf = (PFNGLPIXELTRANSFERFPROC)load("glPixelTransferf");
	glad_glPixelTransferi = (PFNGLPIXELTRANSFERIPROC)load("glPixelTransferi");
	glad_glPixelMapfv = (PFNGLPIXELMAPFVPROC)load("glPixelMapfv");
	glad_glPixelMapuiv = (PFNGLPIXELMAPUIVPROC)load("glPixelMapuiv");
	glad_glPixelMapusv = (PFNGLPIXELMAPUSVPROC)load("glPixelMapusv");
	glad_glCopyPixels = (PFNGLCOPYPIXELSPROC)load("glCopyPixels");
	glad_glDrawPixels = (PFNGLDRAWPIXELSPROC)load("glDrawPixels");
	glad_glGetClipPlane = (PFNGLGETCLIPPLANEPROC)load("glGetClipPlane");
	glad_glGetLightfv = (PFNGLGETLIGHTFVPROC)load("glGetLightfv");
	glad_glGetLightiv = (PFNGLGETLIGHTIVPROC)load("glGetLightiv");
	glad_glGetMapdv = (PFNGLGETMAPDVPROC)load("glGetMapdv");
	glad_glGetMapfv = (PFNGLGETMAPFVPROC)load("glGetMapfv");
	glad_glGetMapiv = (PFNGLGETMAPIVPROC)load("glGetMapiv");
	glad_glGetMaterialfv = (PFNGLGETMATERIALFVPROC)load("glGetMaterialfv");
	glad_glGetMaterialiv = (PFNGLGETMATERIALIVPROC)load("glGetMaterialiv");
	glad_glGetPixelMapfv = (PFNGLGETPIXELMAPFVPROC)load("glGetPixelMapfv");
	glad_glGetPixelMapuiv = (PFNGLGETPIXELMAPUIVPROC)load("glGetPixelMapuiv");
	glad_glGetPixelMapusv = (PFNGLGETPIXELMAPUSVPROC)load("glGetPixelMapusv");
	glad_glGetPolygonStipple = (PFNGLGETPOLYGONSTIPPLEPROC)load("glGetPolygonStipple");
	glad_glGetTexEnvfv = (PFNGLGETTEXENVFVPROC)load("glGetTexEnvfv");
	glad_glGetTexEnviv = (PFNGLGETTEXENVIVPROC)load("glGetTexEnviv");
	glad_glGetTexGendv = (PFNGLGETTEXGENDVPROC)load("glGetTexGendv");
	glad_glGetTexGenfv = (PFNGLGETTEXGENFVPROC)load("glGetTexGenfv");
	glad_glGetTexGeniv = (PFNGLGETTEXGENIVPROC)load("glGetTexGeniv");
	glad_glIsList = (PFNGLISLISTPROC)load("glIsList");
	glad_glFrustum = (PFNGLFRUSTUMPROC)load("glFrustum");
	glad_glLoadIdentity = (PFNGLLOADIDENTITYPROC)load("glLoadIdentity");
	glad_glLoadMatrixf = (PFNGLLOADMATRIXFPROC)load("glLoadMatrixf");
	glad_glLoadMatrixd = (PFNGLLOADMATRIXDPROC)load("glLoadMatrixd");
	glad_glMatrixMode = (PFNGLMATRIXMODEPROC)load("glMatrixMode");
	glad_glMultMatrixf = (PFNGLMULTMATRIXFPROC)load("glMultMatrixf");
	glad_glMultMatrixd = (PFNGLMULTMATRIXDPROC)load("glMultMatrixd");
	glad_glOrtho = (PFNGLORTHOPROC)load("glOrtho");
	glad_glPopMatrix = (PFNGLPOPMATRIXPROC)load("glPopMatrix");
	glad_glPushMatrix = (PFNGLPUSHMATRIXPROC)load("glPushMatrix");
	glad_glRotated = (PFNGLROTATEDPROC)load("glRotated");
	glad_glRotatef = (PFNGLROTATEFPROC)load("glRotatef");
	glad_glScaled = (PFNGLSCALEDPROC)load("glScaled");
	glad_glScalef = (PFNGLSCALEFPROC)load("glScalef");
	glad_glTranslated = (PFNGLTRANSLATEDPROC)load("glTranslated");
	glad_glTranslatef = (PFNGLTRANSLATEFPROC)load("glTranslatef");

*/      
   }

   bool load(IOpenGLEngine engine)
   {
      if (glGetString.isUnassigned()) {
         ^ false
      };
      if (glGetString(GL_VERSION).isUnassigned()) {
         ^ false
      };
      
      find_coreGL(engine);
   	load_GL_VERSION_1_0();
/*      load_GL_VERSION_1_1(load);
      load_GL_VERSION_1_2(load);
      load_GL_VERSION_1_3(load);
      load_GL_VERSION_1_4(load);
      load_GL_VERSION_1_5(load);
      load_GL_VERSION_2_0(load);
      load_GL_VERSION_2_1(load);
      load_GL_VERSION_3_0(load);
      load_GL_VERSION_3_1(load);
      load_GL_VERSION_3_2(load);
      load_GL_VERSION_3_3(load);
      load_GL_VERSION_4_0(load);*/
      
      ^ true
   }
}
