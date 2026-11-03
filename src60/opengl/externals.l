import system'drawing;

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

// --- WGLContext ---

internal struct WGLGetProcAddress               : extern_winapi_func<pointer>(string name);

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

internal struct WGLGetExtensionsStringARBPTR    : wgl_func<pointer>(HDC dc);
internal struct WGLGetExtensionsStringEXTPTR    : wgl_func<pointer>();
internal struct WGLSetStereoEmitterState3DLPTR           : wgl_func<uint>(uint maxCount, pointer ids);

internal struct WGLGetGPUIDsAMDPTR                       : wgl_func<uint>(uint id, pointer ids);
internal struct WGLGetGPUInfoAMDPTR                      : wgl_func<int>(uint id, int property, uint dataType, uint size, pointer data);
internal struct WGLGetContextGPUIDAMDPTR                 : wgl_func<uint>(HGLRC hglrc);
internal struct WGLCreateAssociatedContextAMDPTR         : wgl_func<HGLRC>(uint id);
internal struct WGLCreateAssociatedContextAttribsAMDPTR  : wgl_func<HGLRC>(uint id, HGLRC hShareContext, pointer attribList);
internal struct WGLDeleteAssociatedContextAMDPTR         : wgl_func<int>(HGLRC hglrc);
internal struct WGLMakeAssociatedContextCurrentAMDPTR    : wgl_func<int>(HGLRC hglrc);
internal struct WGLGetCurrentAssociatedContextAMDPTR     : wgl_func<HGLRC>();
internal struct WGLBlitContextFramebufferAMDPTR          : wgl_func<int>(HGLRC dstCtx, int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, int mask, int filter);

internal struct WGLCreateBufferRegionARBPTR              : wgl_func<handle>(HDC hDC, int iLayerPlane, uint uType);
internal struct WGLDeleteBufferRegionARBPTR              : wgl_func<int>(handle hRegion);
internal struct WGLSaveBufferRegionARBPTR                : wgl_func<int>(handle hRegion, int x, int y, int width, int height);
internal struct WGLRestoreBufferRegionARBPTR             : wgl_func<int>(handle hRegion, int x, int y, int width, int height, int xSrc, int ySrc);

internal struct WGLCreateContextAttribsARBPTR            : wgl_func<int>(HDC hDC, HGLRC hShareContext, pointer attribList);

internal struct WGLMakeContextCurrentARBPTR              : wgl_func<int>(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);
internal struct WGLGetCurrentReadDCARBPTR                : wgl_func<HDC>();

internal struct WGLCreatePbufferARBPTR                   : wgl_func<pointer>(HDC hDC, int iPixelFormat, int iWidth, int iHeight, pointer piAttribList);
internal struct WGLGetPbufferDCARBPTR                    : wgl_func<HDC>(pointer hPbuffer);
internal struct WGLReleasePbufferDCARBPTR                : wgl_func<int>(pointer hPbuffer, HDC hDC);
internal struct WGLDestroyPbufferARBPTR                  : wgl_func<int>(pointer hPbuffer);
internal struct WGLQueryPbufferARBPTR                    : wgl_func<HDC>(pointer hPbuffer, int iAttribute, pointer piValue);

internal struct WGLGetPixelFormatAttribivARBPTR          : wgl_func<int>(HDC hdc, int iPixelFormat, int iLayerPlane, uint nAttributes, pointer piAttributes, pointer piValues);
internal struct WGLGetPixelFormatAttribfvARBPTR          : wgl_func<int>(HDC hdc, int iPixelFormat, int iLayerPlane, uint nAttributes, pointer piAttributes, pointer pfValues);
internal struct WGLChoosePixelFormatARBPTR               : wgl_func<int>(HDC hdc, pointer piAttribIList, pointer pfAttribFList, uint nMaxFormats, pointer piFormats, pointer nNumFormats);

internal struct WGLBindTexImageARBPTR                    : wgl_func<int>(pointer hPbuffer, int iBuffer);
internal struct WGLReleaseTexImageARBPTR                 : wgl_func<int>(pointer hPbuffer, int iBuffer);
internal struct WGLSetPbufferAttribARBPTR                : wgl_func<int>(pointer hPbuffer, pointer piAttribList);

internal struct WGLCreateDisplayColorTableEXTPTR         : wgl_func<byte>(short id);
internal struct WGLLoadDisplayColorTableEXTPTR           : wgl_func<byte>(pointer table, int length);
internal struct WGLBindDisplayColorTableEXTPTR           : wgl_func<byte>(short id);
internal struct WGLDestroyDisplayColorTableEXTPTR        : wgl_func<int>(short id);

internal struct WGLMakeContextCurrentEXTPTR              : wgl_func<int>(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);
internal struct WGLGetCurrentReadDCEXTPTR                : wgl_func<HDC>();

internal struct WGLCreatePbufferEXTPTR                   : wgl_func<pointer>(HDC hDC, int iPixelFormat, int iWidth, int iHeight, pointer piAttribList);
internal struct WGLGetPbufferDCEXTPTR                    : wgl_func<HDC>(pointer hPbuffer);
internal struct WGLReleasePbufferDCEXTPTR 	            : wgl_func<int>(pointer hPbuffer, HDC hDC);
internal struct WGLDestroyPbufferEXTPTR                  : wgl_func<int>(pointer hPbuffer);
internal struct WGLQueryPbufferEXTPTR                    : wgl_func<int>(pointer hPbuffer, int iAttribute, pointer piValue);

internal struct WGLGetPixelFormatAttribivEXTPTR          : wgl_func<int>(HDC hdc, int iPixelFormat, int iLayerPlane, uint nAttributes, pointer piAttributes, pointer piValues);
internal struct WGLGetPixelFormatAttribfvEXTPTR          : wgl_func<int>(HDC hdc, int iPixelFormat, int iLayerPlane, uint nAttributes, pointer piAttributes, pointer pfValues);
internal struct WGLChoosePixelFormatEXTPTR               : wgl_func<int>(HDC hdc, pointer piAttribIList, pointer pfAttribFList, uint nMaxFormats, pointer piFormats, pointer nNumFormats);

internal struct WGLSwapIntervalEXTPTR                    : wgl_func<int>(int interval);
internal struct WGLGetSwapIntervalEXTPTR                 : wgl_func<int>();

internal struct WGLGetDigitalVideoParametersI3DPTR       : wgl_func<int>(HDC hDC, int iAttribute, pointer piValue);
internal struct WGLSetDigitalVideoParametersI3DPTR       : wgl_func<int>(HDC hDC, int iAttribute, pointer piValue);

internal struct WGLGetGammaTableParametersI3DPTR         : wgl_func<int>(HDC hDC, int iAttribute, pointer piValue);
internal struct WGLSetGammaTableParametersI3DPTR         : wgl_func<int>(HDC hDC, int iAttribute, pointer piValue);
internal struct WGLGetGammaTableI3DPTR                   : wgl_func<int>(HDC hDC, int iEntries, pointer puRed, pointer puGreen, pointer puBlue);
internal struct WGLSetGammaTableI3DPTR                   : wgl_func<int>(HDC hDC, int iEntries, pointer puRed, pointer puGreen, pointer puBlue);

internal struct WGLEnableGenlockI3DPTR                   : wgl_func<int>(HDC hDC);
internal struct WGLDisableGenlockI3DPTR                  : wgl_func<int>(HDC hDC);
internal struct WGLIsEnabledGenlockI3DPTR                : wgl_func<int>(HDC hDC, pointer pFlag);
internal struct WGLGenlockSourceI3DPTR                   : wgl_func<int>(HDC hDC, uint uSource);
internal struct WGLGetGenlockSourceI3DPTR                : wgl_func<int>(HDC hDC, pointer uSource);
internal struct WGLGenlockSourceEdgeI3DPTR               : wgl_func<int>(HDC hDC, uint uEdge);
internal struct WGLGetGenlockSourceEdgeI3DPTR            : wgl_func<int>(HDC hDC, pointer uEdge);
internal struct WGLGenlockSampleRateI3DPTR               : wgl_func<int>(HDC hDC, uint uRate);
internal struct WGLGetGenlockSampleRateI3DPTR            : wgl_func<int>(HDC hDC, pointer uRate);
internal struct WGLGenlockSourceDelayI3DPTR              : wgl_func<int>(HDC hDC, uint uDelay);
internal struct WGLGetGenlockSourceDelayI3DPTR           : wgl_func<int>(HDC hDC, pointer uDelay);
internal struct WGLQueryGenlockMaxSourceDelayI3DPTR      : wgl_func<int>(HDC hDC, pointer uMaxLineDelay, pointer uMaxPixelDelay);

internal struct WGLCreateImageBufferI3DPTR               : wgl_func<pointer>(HDC hDC, int dwSize, uint uFlags);
internal struct WGLDestroyImageBufferI3DPTR              : wgl_func<int>(HDC hDC, pointer pAddress);
internal struct WGLAssociateImageBufferEventsI3DPTR      : wgl_func<int>(HDC hDC, pointer pEvent, pointer pAddress, pointer pSize, uint count);
internal struct WGLReleaseImageBufferEventsI3DPTR        : wgl_func<int>(HDC hDC, pointer pAddress, uint count);

internal struct WGLEnableFrameLockI3DPTR                 : wgl_func<int>();
internal struct WGLDisableFrameLockI3DPTR                : wgl_func<int>();
internal struct WGLIsEnabledFrameLockI3DPTR              : wgl_func<int>(pointer pFlag);
internal struct WGLQueryFrameLockMasterI3DPTR            : wgl_func<int>(pointer pFlag);

internal struct WGLGetFrameUsageI3DPTR                   : wgl_func<int>(pointer pUsage);
internal struct WGLBeginFrameTrackingI3DPTR              : wgl_func<int>();
internal struct WGLEndFrameTrackingI3DPTR                : wgl_func<int>();
internal struct WGLQueryFrameTrackingI3DPTR              : wgl_func<int>(pointer pFrameCount, pointer pMissedFrames, pointer pLastMissedUsage);

internal struct WGLDXSetResourceShareHandleNVPTR         : wgl_func<int>(pointer dxObject, handle shareHandle);
internal struct WGLDXOpenDeviceNVPTR                     : wgl_func<handle>(pointer dxDevice);
internal struct WGLDXCloseDeviceNVPTR                    : wgl_func<int>(handle hDevice);
internal struct WGLDXRegisterObjectNVPTR                 : wgl_func<handle>(handle hDevice, pointer dxObject, uint name, int type, int access);
internal struct WGLDXUnregisterObjectNVPTR               : wgl_func<int>(handle hDevice, handle hObject);
internal struct WGLDXObjectAccessNVPTR                   : wgl_func<int>(handle hObject, int access);
internal struct WGLDXLockObjectsNVPTR                    : wgl_func<int>(handle hDevice, int count, pointer hObjects);
internal struct WGLDXUnlockObjectsNVPTR                  : wgl_func<int>(handle hDevice, int count, pointer hObjects);

internal struct WGLCopyImageSubDataNVPTR                 : wgl_func<int>(HGLRC hSrcRC, uint srcName, int srcTarget, int srcLevel, int srcX, int srcY, int srcZ, HGLRC hDstRC, uint dstName, int dstTarget, int dstLevel, int dstX, int dstY, int dstZ, int width, int height, int depth);

internal struct WGLDelayBeforeSwapNVPTR                  : wgl_func<int>(HDC hDC, /*float*/int seconds);

internal struct WGLEnumGpusNVPTR                         : wgl_func<int>(uint iGpuIndex, pointer phGpu);
internal struct WGLEnumGpuDevicesNVPTR                   : wgl_func<int>(pointer hGpu, uint iDeviceIndex, pointer lpGpuDevice);
internal struct WGLCreateAffinityDCNVPTR                 : wgl_func<HDC>(pointer phGpuList);
internal struct WGLEnumGpusFromAffinityDCNVPTR           : wgl_func<int>(HDC hAffinityDC, uint iGpuIndex, pointer hGpu);
internal struct WGLDeleteDCNVPTR                         : wgl_func<int>(HDC hdc);

internal struct WGLEnumerateVideoDevicesNVPTR            : wgl_func<int>(HDC hDc, pointer phDeviceList);
internal struct WGLBindVideoDeviceNVPTR                  : wgl_func<int>(HDC hDc, int uVideoSlot, pointer hVideoDevice, pointer piAttribList);
internal struct WGLQueryCurrentContextNVPTR              : wgl_func<int>(int iAttribute, pointer piValue);

internal struct WGLJoinSwapGroupNVPTR                    : wgl_func<int>(HDC hDC, uint group);
internal struct WGLBindSwapBarrierNVPTR                  : wgl_func<int>(uint group, uint barrier);
internal struct WGLQuerySwapGroupNVPTR                   : wgl_func<int>(HDC hDC, pointer group, pointer barrier);
internal struct WGLQueryMaxSwapGroupsNVPTR               : wgl_func<int>(HDC hDC, pointer maxGroups, pointer maxBarriers);
internal struct WGLQueryFrameCountNVPTR                  : wgl_func<int>(HDC hDC, pointer count);
internal struct WGLResetFrameCountNVPTR                  : wgl_func<int>(HDC hDC);

internal struct WGLAllocateMemoryNVPTR                   : wgl_func<pointer>(int size, int/*GLfloat*/ readfreq, /*GLfloat*/int writefreq, /*GLfloat*/int priority);
internal struct WGLFreeMemoryNVPTR                       : wgl_func<int>(pointer pointer);

internal struct WGLBindVideoCaptureDeviceNVPTR           : wgl_func<int>(uint uVideoSlot, pointer hDevice);
internal struct WGLEnumerateVideoCaptureDevicesNVPTR     : wgl_func<int>(HDC hDc, pointer phDeviceList);
internal struct WGLLockVideoCaptureDeviceNVPTR           : wgl_func<int>(HDC hDc, pointer hDevice);
internal struct WGLQueryVideoCaptureDeviceNVPTR          : wgl_func<int>(HDC hDc, pointer hDevice, int iAttribute, pointer piValue);
internal struct WGLReleaseVideoCaptureDeviceNVPTR        : wgl_func<int>(HDC hDc, pointer hDevice);

internal struct WGLGetVideoDeviceNVPTR                   : wgl_func<int>(HDC hDC, int numDevices, pointer hVideoDevice);
internal struct WGLReleaseVideoDeviceNVPTR               : wgl_func<int>(pointer hVideoDevice);
internal struct WGLBindVideoImageNVPTR                   : wgl_func<int>(pointer hVideoDevice, pointer hPbuffer, int iVideoBuffer);
internal struct WGLReleaseVideoImageNVPTR                : wgl_func<int>(pointer hPbuffer, int iVideoBuffer);
internal struct WGLSendPbufferToVideoNVPTR               : wgl_func<int>(pointer hPbuffer, int iBufferType, pointer pulCounterPbuffer, int bBlock);
internal struct WGLGetVideoInfoNVPTR                     : wgl_func<int>(pointer hpVideoDevice, pointer pulCounterOutputPbuffer, pointer pulCounterOutputVideo);

internal struct WGLGetSyncValuesOMLPTR                   : wgl_func<int>(HDC hdc, pointer ust, pointer msc, pointer sbc);
internal struct WGLGetMscRateOMLPTR                      : wgl_func<int>(HDC hdc, pointer numerator, pointer denominator);
internal struct WGLSwapBuffersMscOMLPTR                  : wgl_func<long>(HDC hdc, long target_msc, long divisor, long remainder);
internal struct WGLSwapLayerBuffersMscOMLPTR             : wgl_func<long>(HDC hdc, int fuPlanes, long target_msc, long divisor, long remainder);
internal struct WGLWaitForMscOMLPTR                      : wgl_func<int>(HDC hdc, long target_msc, long divisor, long remainder, pointer ust, pointer msc, pointer sbc);
internal struct WGLWaitForSbcOMLPTR                      : wgl_func<int>(HDC hdc, long target_sbc, pointer ust, pointer msc, pointer sbc);

public struct WGLContext
{
   HDC    wglDC; 
   // opengl functions
   WGLGetProcAddress                         get_proc;
   // opengl extensions
   WGLGetExtensionsStringARBPTR              wglGetExtensionsStringARB;
   WGLGetExtensionsStringEXTPTR              wglGetExtensionsStringEXT; 
   WGLSetStereoEmitterState3DLPTR            wglSetStereoEmitterState3DL;

	WGLGetGPUIDsAMDPTR                        wglGetGPUIDsAMD;
	WGLGetGPUInfoAMDPTR                       wglGetGPUInfoAMD; 
	WGLGetContextGPUIDAMDPTR                  wglGetContextGPUIDAMD; 
	WGLCreateAssociatedContextAMDPTR          wglCreateAssociatedContextAMD;
	WGLCreateAssociatedContextAttribsAMDPTR   wglCreateAssociatedContextAttribsAMD;
	WGLDeleteAssociatedContextAMDPTR          wglDeleteAssociatedContextAMD; 
	WGLMakeAssociatedContextCurrentAMDPTR     wglMakeAssociatedContextCurrentAMD;
   WGLGetCurrentAssociatedContextAMDPTR      wglGetCurrentAssociatedContextAMD;
	WGLBlitContextFramebufferAMDPTR           wglBlitContextFramebufferAMD;
   WGLCreateBufferRegionARBPTR               wglCreateBufferRegionARB;
   WGLDeleteBufferRegionARBPTR               wglDeleteBufferRegionARB;
   WGLSaveBufferRegionARBPTR                 wglSaveBufferRegionARB;
   WGLRestoreBufferRegionARBPTR              wglRestoreBufferRegionARB;
   WGLCreateContextAttribsARBPTR             wglCreateContextAttribsARB;   
   WGLMakeContextCurrentARBPTR               wglMakeContextCurrentARB;
   WGLGetCurrentReadDCARBPTR                 wglGetCurrentReadDCARB;      
   WGLCreatePbufferARBPTR                    wglCreatePbufferARB;
   WGLGetPbufferDCARBPTR                     wglGetPbufferDCARB;
   WGLReleasePbufferDCARBPTR                 wglReleasePbufferDCARB;
   WGLDestroyPbufferARBPTR                   wglDestroyPbufferARB;
   WGLQueryPbufferARBPTR                     wglQueryPbufferARB;             
	WGLGetPixelFormatAttribivARBPTR           wglGetPixelFormatAttribivARB;
	WGLGetPixelFormatAttribfvARBPTR           wglGetPixelFormatAttribfvARB;
	WGLChoosePixelFormatARBPTR                wglChoosePixelFormatARB;       
	WGLBindTexImageARBPTR                     wglBindTexImageARB;
 	WGLReleaseTexImageARBPTR                  wglReleaseTexImageARB;
	WGLSetPbufferAttribARBPTR                 wglSetPbufferAttribARB;
	WGLCreateDisplayColorTableEXTPTR          wglCreateDisplayColorTableEXT;
	WGLLoadDisplayColorTableEXTPTR            wglLoadDisplayColorTableEXT;
	WGLBindDisplayColorTableEXTPTR            wglBindDisplayColorTableEXT;
	WGLDestroyDisplayColorTableEXTPTR         wglDestroyDisplayColorTableEXT;
	WGLMakeContextCurrentEXTPTR               wglMakeContextCurrentEXT;
	WGLGetCurrentReadDCEXTPTR                 wglGetCurrentReadDCEXT;                        
	WGLCreatePbufferEXTPTR                    wglCreatePbufferEXT;
   WGLGetPbufferDCEXTPTR                     wglGetPbufferDCEXT;
   WGLReleasePbufferDCEXTPTR 	               wglReleasePbufferDCEXT;
	WGLDestroyPbufferEXTPTR                   wglDestroyPbufferEXT;
	WGLQueryPbufferEXTPTR                     wglQueryPbufferEXT;                        
	WGLGetPixelFormatAttribivEXTPTR           wglGetPixelFormatAttribivEXT;
	WGLGetPixelFormatAttribfvEXTPTR           wglGetPixelFormatAttribfvEXT;
	WGLChoosePixelFormatEXTPTR                wglChoosePixelFormatEXT;
	WGLSwapIntervalEXTPTR                     wglSwapIntervalEXT;
	WGLGetSwapIntervalEXTPTR                  wglGetSwapIntervalEXT;
	WGLGetDigitalVideoParametersI3DPTR        wglGetDigitalVideoParametersI3D;
	WGLSetDigitalVideoParametersI3DPTR        wglSetDigitalVideoParametersI3D;
	WGLGetGammaTableParametersI3DPTR          wglGetGammaTableParametersI3D;
	WGLSetGammaTableParametersI3DPTR          wglSetGammaTableParametersI3D;
	WGLGetGammaTableI3DPTR                    wglGetGammaTableI3D;
	WGLSetGammaTableI3DPTR                    wglSetGammaTableI3D;
	WGLEnableGenlockI3DPTR                    wglEnableGenlockI3D;
	WGLDisableGenlockI3DPTR                   wglDisableGenlockI3D;
	WGLIsEnabledGenlockI3DPTR                 wglIsEnabledGenlockI3D;
	WGLGenlockSourceI3DPTR                    wglGenlockSourceI3D;
	WGLGetGenlockSourceI3DPTR                 wglGetGenlockSourceI3D;
	WGLGenlockSourceEdgeI3DPTR                wglGenlockSourceEdgeI3D;
	WGLGetGenlockSourceEdgeI3DPTR             wglGetGenlockSourceEdgeI3D;
	WGLGenlockSampleRateI3DPTR                wglGenlockSampleRateI3D;
	WGLGetGenlockSampleRateI3DPTR             wglGetGenlockSampleRateI3D;
	WGLGenlockSourceDelayI3DPTR               wglGenlockSourceDelayI3D;
	WGLGetGenlockSourceDelayI3DPTR            wglGetGenlockSourceDelayI3D;
	WGLQueryGenlockMaxSourceDelayI3DPTR       wglQueryGenlockMaxSourceDelayI3D;
	WGLCreateImageBufferI3DPTR                wglCreateImageBufferI3D;
	WGLDestroyImageBufferI3DPTR               wglDestroyImageBufferI3D;
	WGLAssociateImageBufferEventsI3DPTR       wglAssociateImageBufferEventsI3D;
	WGLReleaseImageBufferEventsI3DPTR         wglReleaseImageBufferEventsI3D;
	WGLEnableFrameLockI3DPTR                  wglEnableFrameLockI3D;
	WGLDisableFrameLockI3DPTR                 wglDisableFrameLockI3D;
	WGLIsEnabledFrameLockI3DPTR               wglIsEnabledFrameLockI3D;
	WGLQueryFrameLockMasterI3DPTR             wglQueryFrameLockMasterI3D;
	WGLGetFrameUsageI3DPTR                    wglGetFrameUsageI3D;
	WGLBeginFrameTrackingI3DPTR               wglBeginFrameTrackingI3D;
	WGLEndFrameTrackingI3DPTR                 wglEndFrameTrackingI3D;
	WGLQueryFrameTrackingI3DPTR               wglQueryFrameTrackingI3D;
	WGLDXSetResourceShareHandleNVPTR          wglDXSetResourceShareHandleNV;
	WGLDXOpenDeviceNVPTR                      wglDXOpenDeviceNV;
	WGLDXCloseDeviceNVPTR                     wglDXCloseDeviceNV;
	WGLDXRegisterObjectNVPTR                  wglDXRegisterObjectNV;
	WGLDXUnregisterObjectNVPTR                wglDXUnregisterObjectNV;
	WGLDXObjectAccessNVPTR                    wglDXObjectAccessNV;
	WGLDXLockObjectsNVPTR                     wglDXLockObjectsNV;
	WGLDXUnlockObjectsNVPTR                   wglDXUnlockObjectsNV; 
    WGLCopyImageSubDataNVPTR                  wglCopyImageSubDataNV;
	WGLEnumGpusNVPTR                          wglEnumGpusNV;
	WGLEnumGpuDevicesNVPTR                    wglEnumGpuDevicesNV;
	WGLCreateAffinityDCNVPTR                  wglCreateAffinityDCNV;
	WGLEnumGpusFromAffinityDCNVPTR            wglEnumGpusFromAffinityDCNV;
	WGLDeleteDCNVPTR                          wglDeleteDCNV; 
    WGLDelayBeforeSwapNVPTR                   wglDelayBeforeSwapNV;
	WGLEnumerateVideoDevicesNVPTR             wglEnumerateVideoDevicesNV;
	WGLBindVideoDeviceNVPTR                   wglBindVideoDeviceNV;
	WGLQueryCurrentContextNVPTR               wglQueryCurrentContextNV;
	WGLJoinSwapGroupNVPTR                     wglJoinSwapGroupNV;
	WGLBindSwapBarrierNVPTR                   wglBindSwapBarrierNV;
	WGLQuerySwapGroupNVPTR                    wglQuerySwapGroupNV;
	WGLQueryMaxSwapGroupsNVPTR                wglQueryMaxSwapGroupsNV;
	WGLQueryFrameCountNVPTR                   wglQueryFrameCountNV;
	WGLResetFrameCountNVPTR                   wglResetFrameCountNV;
	WGLAllocateMemoryNVPTR                    wglAllocateMemoryNV;
	WGLFreeMemoryNVPTR                        wglFreeMemoryNV;
	WGLBindVideoCaptureDeviceNVPTR            wglBindVideoCaptureDeviceNV;
	WGLEnumerateVideoCaptureDevicesNVPTR      wglEnumerateVideoCaptureDevicesNV;
	WGLLockVideoCaptureDeviceNVPTR            wglLockVideoCaptureDeviceNV;
	WGLQueryVideoCaptureDeviceNVPTR           wglQueryVideoCaptureDeviceNV;
	WGLReleaseVideoCaptureDeviceNVPTR         wglReleaseVideoCaptureDeviceNV;
	WGLGetVideoDeviceNVPTR                    wglGetVideoDeviceNV;
	WGLReleaseVideoDeviceNVPTR                wglReleaseVideoDeviceNV;
	WGLBindVideoImageNVPTR                    wglBindVideoImageNV;
	WGLReleaseVideoImageNVPTR                 wglReleaseVideoImageNV;
	WGLSendPbufferToVideoNVPTR                wglSendPbufferToVideoNV;
	WGLGetVideoInfoNVPTR                      wglGetVideoInfoNV;
	WGLGetSyncValuesOMLPTR                    wglGetSyncValuesOML;
	WGLGetMscRateOMLPTR                       wglGetMscRateOML;
	WGLSwapBuffersMscOMLPTR                   wglSwapBuffersMscOML;
	WGLSwapLayerBuffersMscOMLPTR              wglSwapLayerBuffersMscOML;
	WGLWaitForMscOMLPTR                       wglWaitForMscOML;
	WGLWaitForSbcOMLPTR                       wglWaitForSbcOML;

   // flags
   int WGL_3DFX_multisample;
   int WGL_3DL_stereo_control;
   int WGL_AMD_gpu_association;
   int WGL_ARB_buffer_region;
   int WGL_ARB_context_flush_control;
   int WGL_ARB_create_context;
   int WGL_ARB_create_context_no_error;
   int WGL_ARB_create_context_profile;
   int WGL_ARB_create_context_robustness;
//   int WGL_ARB_extensions_string;
   int WGL_ARB_framebuffer_sRGB;
   int WGL_ARB_make_current_read;
   int WGL_ARB_multisample;
   int WGL_ARB_pbuffer;
   int WGL_ARB_pixel_format;
   int WGL_ARB_pixel_format_float;
   int WGL_ARB_render_texture;
   int WGL_ARB_robustness_application_isolation;
   int WGL_ARB_robustness_share_group_isolation;
   int WGL_ATI_pixel_format_float;
   int WGL_ATI_render_texture_rectangle;
   int WGL_EXT_colorspace;
   int WGL_EXT_create_context_es2_profile;
   int WGL_EXT_create_context_es_profile;
   int WGL_EXT_depth_float;
   int WGL_EXT_display_color_table;
   int WGL_EXT_extensions_string;
   int WGL_EXT_framebuffer_sRGB;
   int WGL_EXT_make_current_read;
   int WGL_EXT_multisample;
   int WGL_EXT_pbuffer;
   int WGL_EXT_pixel_format;
   int WGL_EXT_pixel_format_packed_float;
   int WGL_EXT_swap_control;
   int WGL_EXT_swap_control_tear;
   int WGL_I3D_digital_video_control;
   int WGL_I3D_gamma;
   int WGL_I3D_genlock;
   int WGL_I3D_image_buffer;
   int WGL_I3D_swap_frame_lock;
   int WGL_I3D_swap_frame_usage;
   int WGL_NV_DX_interop;
   int WGL_NV_DX_interop2;
   int WGL_NV_copy_image;
   int WGL_NV_delay_before_swap;
   int WGL_NV_float_buffer;
   int WGL_NV_gpu_affinity;
   int WGL_NV_multigpu_context;
   int WGL_NV_multisample_coverage;
   int WGL_NV_present_video;
   int WGL_NV_render_depth_texture;
   int WGL_NV_render_texture_rectangle;
   int WGL_NV_swap_group;
   int WGL_NV_vertex_array_range;
   int WGL_NV_video_capture;
   int WGL_NV_video_output;
   int WGL_OML_sync_control;
   
   constructor()
   {
      handle libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      get_proc := WGLGetProcAddress.load(libGL, "wglGetProcAddress");
   }
   
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
   
   private string? loadExtensionsStr()
   {
      pointer ptr := wglGetExtensionsStringARB(wglDC);
      if (ptr.isUnassigned()) {
         ^ nil
      };
      
      ^ ptr.getStringUnsafe();
   }
   
   private checkExtensions(string extensions)
   {
      WGL_3DFX_multisample := has_ext(extensions, "WGL_3DFX_multisample");
      WGL_3DL_stereo_control := has_ext(extensions, "WGL_3DL_stereo_control");      
      
      WGL_AMD_gpu_association := has_ext(extensions, "WGL_AMD_gpu_association");
      WGL_ARB_buffer_region := has_ext(extensions, "WGL_ARB_buffer_region");
      WGL_ARB_context_flush_control := has_ext(extensions, "WGL_ARB_context_flush_control");
      WGL_ARB_create_context := has_ext(extensions, "WGL_ARB_create_context");
      WGL_ARB_create_context_no_error := has_ext(extensions, "WGL_ARB_create_context_no_error");
      WGL_ARB_create_context_profile := has_ext(extensions, "WGL_ARB_create_context_profile");
      WGL_ARB_create_context_robustness := has_ext(extensions, "WGL_ARB_create_context_robustness");
//      WGL_ARB_extensions_string := has_ext(extensions, "WGL_ARB_extensions_string");
      WGL_ARB_framebuffer_sRGB := has_ext(extensions, "WGL_ARB_framebuffer_sRGB");
      WGL_ARB_make_current_read := has_ext(extensions, "WGL_ARB_make_current_read");
      WGL_ARB_multisample := has_ext(extensions, "WGL_ARB_multisample");
      WGL_ARB_pbuffer := has_ext(extensions, "WGL_ARB_pbuffer");
      WGL_ARB_pixel_format := has_ext(extensions, "WGL_ARB_pixel_format");
      WGL_ARB_pixel_format_float := has_ext(extensions, "WGL_ARB_pixel_format_float");
      WGL_ARB_render_texture := has_ext(extensions, "WGL_ARB_render_texture");
      WGL_ARB_robustness_application_isolation := has_ext(extensions, "WGL_ARB_robustness_application_isolation");
      WGL_ARB_robustness_share_group_isolation := has_ext(extensions, "WGL_ARB_robustness_share_group_isolation");
      WGL_ATI_pixel_format_float := has_ext(extensions, "WGL_ATI_pixel_format_float");
      WGL_ATI_render_texture_rectangle := has_ext(extensions, "WGL_ATI_render_texture_rectangle");
      WGL_EXT_colorspace := has_ext(extensions, "WGL_EXT_colorspace");
      WGL_EXT_create_context_es2_profile := has_ext(extensions, "WGL_EXT_create_context_es2_profile");
      WGL_EXT_create_context_es_profile := has_ext(extensions, "WGL_EXT_create_context_es_profile");
      WGL_EXT_depth_float := has_ext(extensions, "WGL_EXT_depth_float");
      WGL_EXT_display_color_table := has_ext(extensions, "WGL_EXT_display_color_table");
      WGL_EXT_extensions_string := has_ext(extensions, "WGL_EXT_extensions_string");
      WGL_EXT_framebuffer_sRGB := has_ext(extensions, "WGL_EXT_framebuffer_sRGB");
      WGL_EXT_make_current_read := has_ext(extensions, "WGL_EXT_make_current_read");
      WGL_EXT_multisample := has_ext(extensions, "WGL_EXT_multisample");
      WGL_EXT_pbuffer := has_ext(extensions, "WGL_EXT_pbuffer");
      WGL_EXT_pixel_format := has_ext(extensions, "WGL_EXT_pixel_format");
      WGL_EXT_pixel_format_packed_float := has_ext(extensions, "WGL_EXT_pixel_format_packed_float");
      WGL_EXT_swap_control := has_ext(extensions, "WGL_EXT_swap_control");
      WGL_EXT_swap_control_tear := has_ext(extensions, "WGL_EXT_swap_control_tear");
      WGL_I3D_digital_video_control := has_ext(extensions, "WGL_I3D_digital_video_control");
      WGL_I3D_gamma := has_ext(extensions, "WGL_I3D_gamma");
      WGL_I3D_genlock := has_ext(extensions, "WGL_I3D_genlock");
      WGL_I3D_image_buffer := has_ext(extensions, "WGL_I3D_image_buffer");
      WGL_I3D_swap_frame_lock := has_ext(extensions, "WGL_I3D_swap_frame_lock");
      WGL_I3D_swap_frame_usage := has_ext(extensions, "WGL_I3D_swap_frame_usage");
      WGL_NV_DX_interop := has_ext(extensions, "WGL_NV_DX_interop");
      WGL_NV_DX_interop2 := has_ext(extensions, "WGL_NV_DX_interop2");
      WGL_NV_copy_image := has_ext(extensions, "WGL_NV_copy_image");
      WGL_NV_delay_before_swap := has_ext(extensions, "WGL_NV_delay_before_swap");
      WGL_NV_float_buffer := has_ext(extensions, "WGL_NV_float_buffer");
      WGL_NV_gpu_affinity := has_ext(extensions, "WGL_NV_gpu_affinity");
      WGL_NV_multigpu_context := has_ext(extensions, "WGL_NV_multigpu_context");
      WGL_NV_multisample_coverage := has_ext(extensions, "WGL_NV_multisample_coverage");
      WGL_NV_present_video := has_ext(extensions, "WGL_NV_present_video");
      WGL_NV_render_depth_texture := has_ext(extensions, "WGL_NV_render_depth_texture");
      WGL_NV_render_texture_rectangle := has_ext(extensions, "WGL_NV_render_texture_rectangle");
      WGL_NV_swap_group := has_ext(extensions, "WGL_NV_swap_group");
      WGL_NV_vertex_array_range := has_ext(extensions, "WGL_NV_vertex_array_range");
      WGL_NV_video_capture := has_ext(extensions, "WGL_NV_video_capture");
      WGL_NV_video_output := has_ext(extensions, "WGL_NV_video_output");
      WGL_OML_sync_control := has_ext(extensions, "WGL_OML_sync_control");      
   }
   
   private loadExtensions(string extensions)
   {
      checkExtensions(extensions);
      
      if (WGL_3DL_stereo_control == 1) {
         wglSetStereoEmitterState3DL := WGLSetStereoEmitterState3DLPTR.load(get_proc, "wglSetStereoEmitterState3DL");
      };
   	if (WGL_AMD_gpu_association == 1) {
      	wglGetGPUIDsAMD := WGLGetGPUIDsAMDPTR.load(get_proc, "wglGetGPUIDsAMD");
      	wglGetGPUInfoAMD := WGLGetGPUInfoAMDPTR.load(get_proc, "wglGetGPUInfoAMD");
      	wglGetContextGPUIDAMD := WGLGetContextGPUIDAMDPTR.load(get_proc, "wglGetContextGPUIDAMD");
      	wglCreateAssociatedContextAMD := WGLCreateAssociatedContextAMDPTR.load(get_proc, "wglCreateAssociatedContextAMD");
      	wglCreateAssociatedContextAttribsAMD := WGLCreateAssociatedContextAttribsAMDPTR.load(get_proc, "wglCreateAssociatedContextAttribsAMD");
      	wglDeleteAssociatedContextAMD := WGLDeleteAssociatedContextAMDPTR.load(get_proc, "wglDeleteAssociatedContextAMD");
      	wglMakeAssociatedContextCurrentAMD := WGLMakeAssociatedContextCurrentAMDPTR.load(get_proc, "wglMakeAssociatedContextCurrentAMD");
      	wglGetCurrentAssociatedContextAMD := WGLGetCurrentAssociatedContextAMDPTR.load(get_proc, "wglGetCurrentAssociatedContextAMD");
      	wglBlitContextFramebufferAMD := WGLBlitContextFramebufferAMDPTR.load(get_proc, "wglBlitContextFramebufferAMD");       
      };
   	if (WGL_ARB_buffer_region == 1) {
      	wglCreateBufferRegionARB := WGLCreateBufferRegionARBPTR.load(get_proc, "wglCreateBufferRegionARB");
      	wglDeleteBufferRegionARB := WGLDeleteBufferRegionARBPTR.load(get_proc, "wglDeleteBufferRegionARB");
      	wglSaveBufferRegionARB := WGLSaveBufferRegionARBPTR.load(get_proc, "wglSaveBufferRegionARB");
      	wglRestoreBufferRegionARB := WGLRestoreBufferRegionARBPTR.load(get_proc, "wglRestoreBufferRegionARB");       
      };
   	if (WGL_ARB_create_context == 1) {
         wglCreateContextAttribsARB := WGLCreateContextAttribsARBPTR.load(get_proc, "wglCreateContextAttribsARB");
      };
/*   	if (WGL_ARB_extensions_string == 1) {
         glad_wglGetExtensionsStringARB := WGLCreateContextAttribsARBPTR.load(get_proc, "wglGetExtensionsStringARB");
      };*/
   	if (WGL_ARB_make_current_read == 1) {
         wglMakeContextCurrentARB := WGLMakeContextCurrentARBPTR.load(get_proc, "wglMakeContextCurrentARB");
         wglGetCurrentReadDCARB := WGLGetCurrentReadDCARBPTR.load(get_proc, "wglGetCurrentReadDCARB");       
      };
   	if (WGL_ARB_pbuffer == 1) {
         wglCreatePbufferARB := WGLCreatePbufferARBPTR.load(get_proc, "wglCreatePbufferARB");
         wglGetPbufferDCARB := WGLGetPbufferDCARBPTR.load(get_proc, "wglGetPbufferDCARB");
         wglReleasePbufferDCARB := WGLReleasePbufferDCARBPTR.load(get_proc, "wglReleasePbufferDCARB");
         wglDestroyPbufferARB := WGLDestroyPbufferARBPTR.load(get_proc, "wglDestroyPbufferARB");
         wglQueryPbufferARB := WGLQueryPbufferARBPTR.load(get_proc, "wglQueryPbufferARB");       
      };
   	if (WGL_ARB_pixel_format == 1) {
      	wglGetPixelFormatAttribivARB := WGLGetPixelFormatAttribivARBPTR.load(get_proc, "wglGetPixelFormatAttribivARB");
      	wglGetPixelFormatAttribfvARB := WGLGetPixelFormatAttribfvARBPTR.load(get_proc, "wglGetPixelFormatAttribfvARB");
      	wglChoosePixelFormatARB := WGLChoosePixelFormatARBPTR.load(get_proc, "wglChoosePixelFormatARB");       
      };
   	if (WGL_ARB_render_texture == 1) {
         wglBindTexImageARB := WGLBindTexImageARBPTR.load(get_proc, "wglBindTexImageARB");
         wglReleaseTexImageARB := WGLReleaseTexImageARBPTR.load(get_proc, "wglReleaseTexImageARB");
         wglSetPbufferAttribARB := WGLSetPbufferAttribARBPTR.load(get_proc, "wglSetPbufferAttribARB");       
      };
   	if (WGL_EXT_display_color_table == 1) {
	      wglCreateDisplayColorTableEXT := WGLCreateDisplayColorTableEXTPTR.load(get_proc, "wglCreateDisplayColorTableEXT");
	      wglLoadDisplayColorTableEXT := WGLLoadDisplayColorTableEXTPTR.load(get_proc, "wglLoadDisplayColorTableEXT");
	      wglBindDisplayColorTableEXT := WGLBindDisplayColorTableEXTPTR.load(get_proc, "wglBindDisplayColorTableEXT");
	      wglDestroyDisplayColorTableEXT := WGLDestroyDisplayColorTableEXTPTR.load(get_proc, "wglDestroyDisplayColorTableEXT");
      };
   	if (WGL_EXT_extensions_string == 1) {
         wglGetExtensionsStringEXT := WGLGetExtensionsStringEXTPTR.load(get_proc, "wglGetExtensionsStringEXT");       
      };
   	if (WGL_EXT_make_current_read == 1) {
      	wglMakeContextCurrentEXT := WGLMakeContextCurrentEXTPTR.load(get_proc, "wglMakeContextCurrentEXT");
      	wglGetCurrentReadDCEXT := WGLGetCurrentReadDCEXTPTR.load(get_proc, "wglGetCurrentReadDCEXT");       
      };
   	if (WGL_EXT_pbuffer == 1) {
      	wglCreatePbufferEXT := WGLCreatePbufferEXTPTR.load(get_proc, "wglCreatePbufferEXT");
      	wglGetPbufferDCEXT := WGLGetPbufferDCEXTPTR.load(get_proc, "wglGetPbufferDCEXT");
      	wglReleasePbufferDCEXT := WGLReleasePbufferDCEXTPTR.load(get_proc, "wglReleasePbufferDCEXT");
      	wglDestroyPbufferEXT := WGLDestroyPbufferEXTPTR.load(get_proc, "wglDestroyPbufferEXT");
      	wglQueryPbufferEXT := WGLQueryPbufferEXTPTR.load(get_proc, "wglQueryPbufferEXT");       
      };
   	if (WGL_EXT_pixel_format == 1) {
      	wglGetPixelFormatAttribivEXT := WGLGetPixelFormatAttribivEXTPTR.load(get_proc, "wglGetPixelFormatAttribivEXT");
      	wglGetPixelFormatAttribfvEXT := WGLGetPixelFormatAttribfvEXTPTR.load(get_proc, "wglGetPixelFormatAttribfvEXT");
      	wglChoosePixelFormatEXT := WGLChoosePixelFormatEXTPTR.load(get_proc, "wglChoosePixelFormatEXT");       
      };
   	if (WGL_EXT_swap_control == 1) {
      	wglSwapIntervalEXT := WGLSwapIntervalEXTPTR.load(get_proc, "wglSwapIntervalEXT");
      	wglGetSwapIntervalEXT := WGLGetSwapIntervalEXTPTR.load(get_proc, "wglGetSwapIntervalEXT");       
      };
   	if (WGL_I3D_digital_video_control == 1) {
      	wglGetDigitalVideoParametersI3D := WGLGetDigitalVideoParametersI3DPTR.load(get_proc, "wglGetDigitalVideoParametersI3D");
      	wglSetDigitalVideoParametersI3D := WGLSetDigitalVideoParametersI3DPTR.load(get_proc, "wglSetDigitalVideoParametersI3D");       
      };
   	if (WGL_I3D_gamma == 1) {
      	wglGetGammaTableParametersI3D := WGLGetGammaTableParametersI3DPTR.load(get_proc, "wglGetGammaTableParametersI3D");
      	wglSetGammaTableParametersI3D := WGLSetGammaTableParametersI3DPTR.load(get_proc, "wglSetGammaTableParametersI3D");
      	wglGetGammaTableI3D := WGLGetGammaTableI3DPTR.load(get_proc, "wglGetGammaTableI3D");
      	wglSetGammaTableI3D := WGLSetGammaTableI3DPTR.load(get_proc, "wglSetGammaTableI3D");       
      };
   	if (WGL_I3D_genlock == 1) {
      	wglEnableGenlockI3D := WGLEnableGenlockI3DPTR.load(get_proc, "wglEnableGenlockI3D");
      	wglDisableGenlockI3D := WGLDisableGenlockI3DPTR.load(get_proc, "wglDisableGenlockI3D");
      	wglIsEnabledGenlockI3D := WGLIsEnabledGenlockI3DPTR.load(get_proc, "wglIsEnabledGenlockI3D");
      	wglGenlockSourceI3D := WGLGenlockSourceI3DPTR.load(get_proc, "wglGenlockSourceI3D");
      	wglGetGenlockSourceI3D := WGLGetGenlockSourceI3DPTR.load(get_proc, "wglGetGenlockSourceI3D");
      	wglGenlockSourceEdgeI3D := WGLGenlockSourceEdgeI3DPTR.load(get_proc, "wglGenlockSourceEdgeI3D");
      	wglGetGenlockSourceEdgeI3D := WGLGetGenlockSourceEdgeI3DPTR.load(get_proc, "wglGetGenlockSourceEdgeI3D");
      	wglGenlockSampleRateI3D := WGLGenlockSampleRateI3DPTR.load(get_proc, "wglGenlockSampleRateI3D");
      	wglGetGenlockSampleRateI3D := WGLGetGenlockSampleRateI3DPTR.load(get_proc, "wglGetGenlockSampleRateI3D");
      	wglGenlockSourceDelayI3D := WGLGenlockSourceDelayI3DPTR.load(get_proc, "wglGenlockSourceDelayI3D");
      	wglGetGenlockSourceDelayI3D := WGLGetGenlockSourceDelayI3DPTR.load(get_proc, "wglGetGenlockSourceDelayI3D");
      	wglQueryGenlockMaxSourceDelayI3D := WGLQueryGenlockMaxSourceDelayI3DPTR.load(get_proc, "wglQueryGenlockMaxSourceDelayI3D");       
      };
   	if (WGL_I3D_image_buffer == 1) {
      	wglCreateImageBufferI3D := WGLCreateImageBufferI3DPTR.load(get_proc, "wglCreateImageBufferI3D");
      	wglDestroyImageBufferI3D := WGLDestroyImageBufferI3DPTR.load(get_proc, "wglDestroyImageBufferI3D");
      	wglAssociateImageBufferEventsI3D := WGLAssociateImageBufferEventsI3DPTR.load(get_proc, "wglAssociateImageBufferEventsI3D");
      	wglReleaseImageBufferEventsI3D := WGLReleaseImageBufferEventsI3DPTR.load(get_proc, "wglReleaseImageBufferEventsI3D");       
      };
   	if (WGL_I3D_swap_frame_lock == 1) {
      	wglEnableFrameLockI3D := WGLEnableFrameLockI3DPTR.load(get_proc, "wglEnableFrameLockI3D");
      	wglDisableFrameLockI3D := WGLDisableFrameLockI3DPTR.load(get_proc, "wglDisableFrameLockI3D");
      	wglIsEnabledFrameLockI3D := WGLIsEnabledFrameLockI3DPTR.load(get_proc, "wglIsEnabledFrameLockI3D");
      	wglQueryFrameLockMasterI3D := WGLQueryFrameLockMasterI3DPTR.load(get_proc, "wglQueryFrameLockMasterI3D");       
      };
   	if (WGL_I3D_swap_frame_usage == 1) {
	      wglGetFrameUsageI3D := WGLGetFrameUsageI3DPTR.load(get_proc, "wglGetFrameUsageI3D");
	      wglBeginFrameTrackingI3D := WGLBeginFrameTrackingI3DPTR.load(get_proc, "wglBeginFrameTrackingI3D");
	      wglEndFrameTrackingI3D := WGLEndFrameTrackingI3DPTR.load(get_proc, "wglEndFrameTrackingI3D");
	      wglQueryFrameTrackingI3D := WGLQueryFrameTrackingI3DPTR.load(get_proc, "wglQueryFrameTrackingI3D");      
      };
   	if (WGL_NV_DX_interop == 1) {
      	wglDXSetResourceShareHandleNV := WGLDXSetResourceShareHandleNVPTR.load(get_proc, "wglDXSetResourceShareHandleNV");
      	wglDXOpenDeviceNV := WGLDXOpenDeviceNVPTR.load(get_proc, "wglDXOpenDeviceNV");
      	wglDXCloseDeviceNV := WGLDXCloseDeviceNVPTR.load(get_proc, "wglDXCloseDeviceNV");
      	wglDXRegisterObjectNV := WGLDXRegisterObjectNVPTR.load(get_proc, "wglDXRegisterObjectNV");
      	wglDXUnregisterObjectNV := WGLDXUnregisterObjectNVPTR.load(get_proc, "wglDXUnregisterObjectNV");
      	wglDXObjectAccessNV := WGLDXObjectAccessNVPTR.load(get_proc, "wglDXObjectAccessNV");
      	wglDXLockObjectsNV := WGLDXLockObjectsNVPTR.load(get_proc, "wglDXLockObjectsNV");
      	wglDXUnlockObjectsNV := WGLDXUnlockObjectsNVPTR.load(get_proc, "wglDXUnlockObjectsNV");
      };
   	if (WGL_NV_copy_image == 1) {
         wglCopyImageSubDataNV := WGLCopyImageSubDataNVPTR.load(get_proc, "wglCopyImageSubDataNV");
      };
   	if (WGL_NV_delay_before_swap == 1) {
         wglDelayBeforeSwapNV := WGLDelayBeforeSwapNVPTR.load(get_proc, "wglDelayBeforeSwapNV");      
      };
   	if (WGL_NV_gpu_affinity == 1) {
      	wglEnumGpusNV := WGLEnumGpusNVPTR.load(get_proc, "wglEnumGpusNV");
      	wglEnumGpuDevicesNV := WGLEnumGpuDevicesNVPTR.load(get_proc, "wglEnumGpuDevicesNV");
      	wglCreateAffinityDCNV := WGLCreateAffinityDCNVPTR.load(get_proc, "wglCreateAffinityDCNV");
      	wglEnumGpusFromAffinityDCNV := WGLEnumGpusFromAffinityDCNVPTR.load(get_proc, "wglEnumGpusFromAffinityDCNV");
      	wglDeleteDCNV := WGLDeleteDCNVPTR.load(get_proc, "wglDeleteDCNV");
      };
   	if (WGL_NV_present_video == 1) {
	   wglEnumerateVideoDevicesNV := WGLEnumerateVideoDevicesNVPTR.load(get_proc, "wglEnumerateVideoDevicesNV");
	   wglBindVideoDeviceNV := WGLBindVideoDeviceNVPTR.load(get_proc, "wglBindVideoDeviceNV");
	   wglQueryCurrentContextNV := WGLQueryCurrentContextNVPTR.load(get_proc, "wglQueryCurrentContextNV");       
      };
   	if (WGL_NV_swap_group == 1) {
	   wglJoinSwapGroupNV := WGLJoinSwapGroupNVPTR.load(get_proc, "wglJoinSwapGroupNV");
	   wglBindSwapBarrierNV := WGLBindSwapBarrierNVPTR.load(get_proc, "wglBindSwapBarrierNV");
	   wglQuerySwapGroupNV := WGLQuerySwapGroupNVPTR.load(get_proc, "wglQuerySwapGroupNV");
	   wglQueryMaxSwapGroupsNV := WGLQueryMaxSwapGroupsNVPTR.load(get_proc, "wglQueryMaxSwapGroupsNV");
	   wglQueryFrameCountNV := WGLQueryFrameCountNVPTR.load(get_proc, "wglQueryFrameCountNV");
	   wglResetFrameCountNV := WGLResetFrameCountNVPTR.load(get_proc, "wglResetFrameCountNV");       
      };
   	if (WGL_NV_vertex_array_range == 1) {
	    wglAllocateMemoryNV := WGLAllocateMemoryNVPTR.load(get_proc, "wglAllocateMemoryNV");
	    wglFreeMemoryNV := WGLFreeMemoryNVPTR.load(get_proc, "wglFreeMemoryNV");       
      };
   	if (WGL_NV_video_capture == 1) {
	   wglBindVideoCaptureDeviceNV := WGLBindVideoCaptureDeviceNVPTR.load(get_proc, "wglBindVideoCaptureDeviceNV");
	   wglEnumerateVideoCaptureDevicesNV := WGLEnumerateVideoCaptureDevicesNVPTR.load(get_proc, "wglEnumerateVideoCaptureDevicesNV");
	   wglLockVideoCaptureDeviceNV := WGLLockVideoCaptureDeviceNVPTR.load(get_proc, "wglLockVideoCaptureDeviceNV");
	   wglQueryVideoCaptureDeviceNV := WGLQueryVideoCaptureDeviceNVPTR.load(get_proc, "wglQueryVideoCaptureDeviceNV");
	   wglReleaseVideoCaptureDeviceNV := WGLReleaseVideoCaptureDeviceNVPTR.load(get_proc, "wglReleaseVideoCaptureDeviceNV");       
      };
   	if (WGL_NV_video_output == 1) {
	   wglGetVideoDeviceNV := WGLGetVideoDeviceNVPTR.load(get_proc, "wglGetVideoDeviceNV");
	   wglReleaseVideoDeviceNV := WGLReleaseVideoDeviceNVPTR.load(get_proc, "wglReleaseVideoDeviceNV");
	   wglBindVideoImageNV := WGLBindVideoImageNVPTR.load(get_proc, "wglBindVideoImageNV");
	   wglReleaseVideoImageNV := WGLReleaseVideoImageNVPTR.load(get_proc, "wglReleaseVideoImageNV");
	   wglSendPbufferToVideoNV := WGLSendPbufferToVideoNVPTR.load(get_proc, "wglSendPbufferToVideoNV");
	   wglGetVideoInfoNV := WGLGetVideoInfoNVPTR.load(get_proc, "wglGetVideoInfoNV");       
      };
   	if (WGL_OML_sync_control == 1) {
	   wglGetSyncValuesOML := WGLGetSyncValuesOMLPTR.load(get_proc, "wglGetSyncValuesOML");
	   wglGetMscRateOML := WGLGetMscRateOMLPTR.load(get_proc, "wglGetMscRateOML");
	   wglSwapBuffersMscOML := WGLSwapBuffersMscOMLPTR.load(get_proc, "wglSwapBuffersMscOML");
	   wglSwapLayerBuffersMscOML := WGLSwapLayerBuffersMscOMLPTR.load(get_proc, "wglSwapLayerBuffersMscOML");
	   wglWaitForMscOML := WGLWaitForMscOMLPTR.load(get_proc, "wglWaitForMscOML");
	   wglWaitForSbcOML := WGLWaitForSbcOMLPTR.load(get_proc, "wglWaitForSbcOML");       
      };
   }
   
   bool load(HDC dc)
   {
      if (get_proc.isUnassigned()) {
         ^ false
      };
      
      wglGetExtensionsStringARB := WGLGetExtensionsStringARBPTR.load(get_proc, "wglGetExtensionsStringARB");      
      if (wglGetExtensionsStringARB.isUnassigned()) {
         ^ false
      };
      
      wglDC := dc;
      
      string? extensions := loadExtensionsStr();
      if:not:nil(extensions) {
         loadExtensions(extensions);
      };
      
      ^ true
   }
}

// --- GLContext ---

public struct GLContext
{
   handle libGL;
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
   }

   bool load()
   {
      ^ true
   }
}

// --- WGLWrapper ---

internal singleton WGLWrapper
{
   HGLRC createContext(HDC dc)
   {
      handle h := OPENGL.wglCreateContext(dc);
      
      ^ h
   }
}