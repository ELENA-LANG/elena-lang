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

// --- extern_winapi_func ---

public template wgl_func<T>(params member)::
{
   __winapi_fn_ptr func;

   constructor load(WGLGetProcAddress get_proc, string func_name) 
   {
      func := get_proc(func_name);
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

internal struct WGLGetProcAddress      : extern_winapi_func<pointer>(string name);

internal struct GLGetStringPTR         : extern_winapi_func<pointer>(int name);

internal struct GLCullFacePTR          : extern_winapi_func<int>(int mode);
internal struct GLFrontFacePTR         : extern_winapi_func<int>(int mode);
internal struct GLHintPTR              : extern_winapi_func<int>(int target, int mode);
internal struct GLLineWidthPTR         : extern_winapi_func<int>(/*GLfloat*/int width);

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
   
   GLCullFacePTR        glCullFace;
	GLFrontFacePTR       glFrontFace;
	GLHintPTR            glHint;
	GLLineWidthPTR       glLineWidth;
   
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
 /*
	glad_glPointSize = (PFNGLPOINTSIZEPROC)load("glPointSize");
	glad_glPolygonMode = (PFNGLPOLYGONMODEPROC)load("glPolygonMode");
	glad_glScissor = (PFNGLSCISSORPROC)load("glScissor");
	glad_glTexParameterf = (PFNGLTEXPARAMETERFPROC)load("glTexParameterf");
	glad_glTexParameterfv = (PFNGLTEXPARAMETERFVPROC)load("glTexParameterfv");
	glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC)load("glTexParameteri");
	glad_glTexParameteriv = (PFNGLTEXPARAMETERIVPROC)load("glTexParameteriv");
	glad_glTexImage1D = (PFNGLTEXIMAGE1DPROC)load("glTexImage1D");
	glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC)load("glTexImage2D");
	glad_glDrawBuffer = (PFNGLDRAWBUFFERPROC)load("glDrawBuffer");
	glad_glClear = (PFNGLCLEARPROC)load("glClear");
	glad_glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor");
	glad_glClearStencil = (PFNGLCLEARSTENCILPROC)load("glClearStencil");
	glad_glClearDepth = (PFNGLCLEARDEPTHPROC)load("glClearDepth");
	glad_glStencilMask = (PFNGLSTENCILMASKPROC)load("glStencilMask");
	glad_glColorMask = (PFNGLCOLORMASKPROC)load("glColorMask");
	glad_glDepthMask = (PFNGLDEPTHMASKPROC)load("glDepthMask");
	glad_glDisable = (PFNGLDISABLEPROC)load("glDisable");
	glad_glEnable = (PFNGLENABLEPROC)load("glEnable");
	glad_glFinish = (PFNGLFINISHPROC)load("glFinish");
	glad_glFlush = (PFNGLFLUSHPROC)load("glFlush");
	glad_glBlendFunc = (PFNGLBLENDFUNCPROC)load("glBlendFunc");
	glad_glLogicOp = (PFNGLLOGICOPPROC)load("glLogicOp");
	glad_glStencilFunc = (PFNGLSTENCILFUNCPROC)load("glStencilFunc");
	glad_glStencilOp = (PFNGLSTENCILOPPROC)load("glStencilOp");
	glad_glDepthFunc = (PFNGLDEPTHFUNCPROC)load("glDepthFunc");
	glad_glPixelStoref = (PFNGLPIXELSTOREFPROC)load("glPixelStoref");
	glad_glPixelStorei = (PFNGLPIXELSTOREIPROC)load("glPixelStorei");
	glad_glReadBuffer = (PFNGLREADBUFFERPROC)load("glReadBuffer");
	glad_glReadPixels = (PFNGLREADPIXELSPROC)load("glReadPixels");
	glad_glGetBooleanv = (PFNGLGETBOOLEANVPROC)load("glGetBooleanv");
	glad_glGetDoublev = (PFNGLGETDOUBLEVPROC)load("glGetDoublev");
	glad_glGetError = (PFNGLGETERRORPROC)load("glGetError");
	glad_glGetFloatv = (PFNGLGETFLOATVPROC)load("glGetFloatv");
	glad_glGetIntegerv = (PFNGLGETINTEGERVPROC)load("glGetIntegerv");
	glad_glGetString = (PFNGLGETSTRINGPROC)load("glGetString");
	glad_glGetTexImage = (PFNGLGETTEXIMAGEPROC)load("glGetTexImage");
	glad_glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC)load("glGetTexParameterfv");
	glad_glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC)load("glGetTexParameteriv");
	glad_glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC)load("glGetTexLevelParameterfv");
	glad_glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC)load("glGetTexLevelParameteriv");
	glad_glIsEnabled = (PFNGLISENABLEDPROC)load("glIsEnabled");
	glad_glDepthRange = (PFNGLDEPTHRANGEPROC)load("glDepthRange");
	glad_glViewport = (PFNGLVIEWPORTPROC)load("glViewport");
	glad_glNewList = (PFNGLNEWLISTPROC)load("glNewList");
	glad_glEndList = (PFNGLENDLISTPROC)load("glEndList");
	glad_glCallList = (PFNGLCALLLISTPROC)load("glCallList");
	glad_glCallLists = (PFNGLCALLLISTSPROC)load("glCallLists");
	glad_glDeleteLists = (PFNGLDELETELISTSPROC)load("glDeleteLists");
	glad_glGenLists = (PFNGLGENLISTSPROC)load("glGenLists");
	glad_glListBase = (PFNGLLISTBASEPROC)load("glListBase");
	glad_glBegin = (PFNGLBEGINPROC)load("glBegin");
	glad_glBitmap = (PFNGLBITMAPPROC)load("glBitmap");
	glad_glColor3b = (PFNGLCOLOR3BPROC)load("glColor3b");
	glad_glColor3bv = (PFNGLCOLOR3BVPROC)load("glColor3bv");
	glad_glColor3d = (PFNGLCOLOR3DPROC)load("glColor3d");
	glad_glColor3dv = (PFNGLCOLOR3DVPROC)load("glColor3dv");
	glad_glColor3f = (PFNGLCOLOR3FPROC)load("glColor3f");
	glad_glColor3fv = (PFNGLCOLOR3FVPROC)load("glColor3fv");
	glad_glColor3i = (PFNGLCOLOR3IPROC)load("glColor3i");
	glad_glColor3iv = (PFNGLCOLOR3IVPROC)load("glColor3iv");
	glad_glColor3s = (PFNGLCOLOR3SPROC)load("glColor3s");
	glad_glColor3sv = (PFNGLCOLOR3SVPROC)load("glColor3sv");
	glad_glColor3ub = (PFNGLCOLOR3UBPROC)load("glColor3ub");
	glad_glColor3ubv = (PFNGLCOLOR3UBVPROC)load("glColor3ubv");
	glad_glColor3ui = (PFNGLCOLOR3UIPROC)load("glColor3ui");
	glad_glColor3uiv = (PFNGLCOLOR3UIVPROC)load("glColor3uiv");
	glad_glColor3us = (PFNGLCOLOR3USPROC)load("glColor3us");
	glad_glColor3usv = (PFNGLCOLOR3USVPROC)load("glColor3usv");
	glad_glColor4b = (PFNGLCOLOR4BPROC)load("glColor4b");
	glad_glColor4bv = (PFNGLCOLOR4BVPROC)load("glColor4bv");
	glad_glColor4d = (PFNGLCOLOR4DPROC)load("glColor4d");
	glad_glColor4dv = (PFNGLCOLOR4DVPROC)load("glColor4dv");
	glad_glColor4f = (PFNGLCOLOR4FPROC)load("glColor4f");
	glad_glColor4fv = (PFNGLCOLOR4FVPROC)load("glColor4fv");
	glad_glColor4i = (PFNGLCOLOR4IPROC)load("glColor4i");
	glad_glColor4iv = (PFNGLCOLOR4IVPROC)load("glColor4iv");
	glad_glColor4s = (PFNGLCOLOR4SPROC)load("glColor4s");
	glad_glColor4sv = (PFNGLCOLOR4SVPROC)load("glColor4sv");
	glad_glColor4ub = (PFNGLCOLOR4UBPROC)load("glColor4ub");
	glad_glColor4ubv = (PFNGLCOLOR4UBVPROC)load("glColor4ubv");
	glad_glColor4ui = (PFNGLCOLOR4UIPROC)load("glColor4ui");
	glad_glColor4uiv = (PFNGLCOLOR4UIVPROC)load("glColor4uiv");
	glad_glColor4us = (PFNGLCOLOR4USPROC)load("glColor4us");
	glad_glColor4usv = (PFNGLCOLOR4USVPROC)load("glColor4usv");
	glad_glEdgeFlag = (PFNGLEDGEFLAGPROC)load("glEdgeFlag");
	glad_glEdgeFlagv = (PFNGLEDGEFLAGVPROC)load("glEdgeFlagv");
	glad_glEnd = (PFNGLENDPROC)load("glEnd");
	glad_glIndexd = (PFNGLINDEXDPROC)load("glIndexd");
	glad_glIndexdv = (PFNGLINDEXDVPROC)load("glIndexdv");
	glad_glIndexf = (PFNGLINDEXFPROC)load("glIndexf");
	glad_glIndexfv = (PFNGLINDEXFVPROC)load("glIndexfv");
	glad_glIndexi = (PFNGLINDEXIPROC)load("glIndexi");
	glad_glIndexiv = (PFNGLINDEXIVPROC)load("glIndexiv");
	glad_glIndexs = (PFNGLINDEXSPROC)load("glIndexs");
	glad_glIndexsv = (PFNGLINDEXSVPROC)load("glIndexsv");
	glad_glNormal3b = (PFNGLNORMAL3BPROC)load("glNormal3b");
	glad_glNormal3bv = (PFNGLNORMAL3BVPROC)load("glNormal3bv");
	glad_glNormal3d = (PFNGLNORMAL3DPROC)load("glNormal3d");
	glad_glNormal3dv = (PFNGLNORMAL3DVPROC)load("glNormal3dv");
	glad_glNormal3f = (PFNGLNORMAL3FPROC)load("glNormal3f");
	glad_glNormal3fv = (PFNGLNORMAL3FVPROC)load("glNormal3fv");
	glad_glNormal3i = (PFNGLNORMAL3IPROC)load("glNormal3i");
	glad_glNormal3iv = (PFNGLNORMAL3IVPROC)load("glNormal3iv");
	glad_glNormal3s = (PFNGLNORMAL3SPROC)load("glNormal3s");
	glad_glNormal3sv = (PFNGLNORMAL3SVPROC)load("glNormal3sv");
	glad_glRasterPos2d = (PFNGLRASTERPOS2DPROC)load("glRasterPos2d");
	glad_glRasterPos2dv = (PFNGLRASTERPOS2DVPROC)load("glRasterPos2dv");
	glad_glRasterPos2f = (PFNGLRASTERPOS2FPROC)load("glRasterPos2f");
	glad_glRasterPos2fv = (PFNGLRASTERPOS2FVPROC)load("glRasterPos2fv");
	glad_glRasterPos2i = (PFNGLRASTERPOS2IPROC)load("glRasterPos2i");
	glad_glRasterPos2iv = (PFNGLRASTERPOS2IVPROC)load("glRasterPos2iv");
	glad_glRasterPos2s = (PFNGLRASTERPOS2SPROC)load("glRasterPos2s");
	glad_glRasterPos2sv = (PFNGLRASTERPOS2SVPROC)load("glRasterPos2sv");
	glad_glRasterPos3d = (PFNGLRASTERPOS3DPROC)load("glRasterPos3d");
	glad_glRasterPos3dv = (PFNGLRASTERPOS3DVPROC)load("glRasterPos3dv");
	glad_glRasterPos3f = (PFNGLRASTERPOS3FPROC)load("glRasterPos3f");
	glad_glRasterPos3fv = (PFNGLRASTERPOS3FVPROC)load("glRasterPos3fv");
	glad_glRasterPos3i = (PFNGLRASTERPOS3IPROC)load("glRasterPos3i");
	glad_glRasterPos3iv = (PFNGLRASTERPOS3IVPROC)load("glRasterPos3iv");
	glad_glRasterPos3s = (PFNGLRASTERPOS3SPROC)load("glRasterPos3s");
	glad_glRasterPos3sv = (PFNGLRASTERPOS3SVPROC)load("glRasterPos3sv");
	glad_glRasterPos4d = (PFNGLRASTERPOS4DPROC)load("glRasterPos4d");
	glad_glRasterPos4dv = (PFNGLRASTERPOS4DVPROC)load("glRasterPos4dv");
	glad_glRasterPos4f = (PFNGLRASTERPOS4FPROC)load("glRasterPos4f");
	glad_glRasterPos4fv = (PFNGLRASTERPOS4FVPROC)load("glRasterPos4fv");
	glad_glRasterPos4i = (PFNGLRASTERPOS4IPROC)load("glRasterPos4i");
	glad_glRasterPos4iv = (PFNGLRASTERPOS4IVPROC)load("glRasterPos4iv");
	glad_glRasterPos4s = (PFNGLRASTERPOS4SPROC)load("glRasterPos4s");
	glad_glRasterPos4sv = (PFNGLRASTERPOS4SVPROC)load("glRasterPos4sv");
	glad_glRectd = (PFNGLRECTDPROC)load("glRectd");
	glad_glRectdv = (PFNGLRECTDVPROC)load("glRectdv");
	glad_glRectf = (PFNGLRECTFPROC)load("glRectf");
	glad_glRectfv = (PFNGLRECTFVPROC)load("glRectfv");
	glad_glRecti = (PFNGLRECTIPROC)load("glRecti");
	glad_glRectiv = (PFNGLRECTIVPROC)load("glRectiv");
	glad_glRects = (PFNGLRECTSPROC)load("glRects");
	glad_glRectsv = (PFNGLRECTSVPROC)load("glRectsv");
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
