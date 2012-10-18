CLEANFILES = 
DISTCLEANFILES =
EXTRA_DIST = $(srcdir)/GraphicsDisplayWin.cpp \
             $(srcdir)/GraphicsDisplayWin.h

lib_LTLIBRARIES = \
  libnux-graphics-@NUX_API_VERSION@.la

libnux_graphics_@NUX_API_VERSION@_la_CPPFLAGS= \
  -I$(srcdir) \
  -I$(top_srcdir) \
  -DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
  -DDATADIR=\""$(datadir)"\" \
  -DPKGDATADIR=\""$(pkgdatadir)/@NUX_API_VERSION@"\" \
  -DG_LOG_DOMAIN=\"NuxGraphics\" \
  $(GCC_FLAGS) \
  $(NUX_GRAPHICS_CFLAGS) \
  $(MAINTAINER_CFLAGS) \
  $(GEIS_CFLAGS)

if USE_X11
libnux_graphics_@NUX_API_VERSION@_la_LIBADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(NUX_GRAPHICS_LIBS) \
  $(GEIS_LIBS)
else
libnux_graphics_@NUX_API_VERSION@_la_LIBADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(NUX_GRAPHICS_LIBS) \
  -L/usr/lib/arm-linux-gnueabi \
  -L/usr/lib/arm-linux-gnueabi/libhybris-egl \
  -lis \
  -lsf \
  -lhybris_ics \
  -lEGL \
  -lGLESv2
endif

libnux_graphics_@NUX_API_VERSION@_la_LDFLAGS = \
  $(NUX_LT_LDFLAGS)

source_h = \
  $(srcdir)/BitmapFormats.h \
  $(srcdir)/CairoGraphics.h \
  $(srcdir)/Events.h \
  $(srcdir)/FontTexture.h \
  $(srcdir)/FontRenderer.h \
  $(srcdir)/GdkGraphics.h \
  $(srcdir)/Gfx_Interface.h \
  $(srcdir)/GLDeviceFrameBufferObject.h \
  $(srcdir)/GLDeviceObjects.h \
  $(srcdir)/GLError.h \
  $(srcdir)/GlobalGraphicsInitializer.h \
  $(srcdir)/GLPBuffer.h \
  $(srcdir)/GLRenderStates.h \
  $(srcdir)/GLResource.h \
  $(srcdir)/GLResourceManager.h \
  $(srcdir)/GLShader.h \
  $(srcdir)/GLShaderParameter.h \
  $(srcdir)/GLSh_ColorPicker.h \
  $(srcdir)/GLSh_DrawFunction.h \
  $(srcdir)/GLSh_Fill.h \
  $(srcdir)/GLTemplatePrimitiveBuffer.h \
  $(srcdir)/GLTextureResourceManager.h \
  $(srcdir)/GLTextureStates.h \
  $(srcdir)/GLThread.h \
  $(srcdir)/GLTimer.h \
  $(srcdir)/GLVertexResourceManager.h \
  $(srcdir)/GLWindowManager.h \
  $(srcdir)/GpuDevice.h \
  $(srcdir)/GraphicsDisplay.h \
  $(srcdir)/GraphicsEngine.h \
  $(srcdir)/MeshData.h \
  $(srcdir)/MeshFileLoader-OBJ.h \
  $(srcdir)/ImageSurface.h \
  $(srcdir)/IOpenGLAnimatedTexture.h \
  $(srcdir)/IOpenGLBaseTexture.h \
  $(srcdir)/IOpenGLCubeTexture.h \
  $(srcdir)/IOpenGLFrameBufferObject.h \
  $(srcdir)/IOpenGLGLSLShader.h \
  $(srcdir)/IOpenGLIndexBuffer.h \
  $(srcdir)/IOpenGLPixelBufferOject.h \
  $(srcdir)/IOpenGLQuery.h \
  $(srcdir)/IOpenGLRectangleTexture.h \
  $(srcdir)/IOpenGLResource.h \
  $(srcdir)/IOpenGLSurface.h \
  $(srcdir)/IOpenGLTexture2D.h \
  $(srcdir)/IOpenGLVertexBuffer.h \
  $(srcdir)/IOpenGLVertexDeclaration.h \
  $(srcdir)/IOpenGLVolume.h \
  $(srcdir)/IOpenGLVolumeTexture.h \
  $(srcdir)/NuxGraphics.h \
  $(srcdir)/NuxGraphicsObject.h \
  $(srcdir)/NuxGraphicsResources.h \
  $(srcdir)/OpenGLDefinitions.h \
  $(srcdir)/OpenGLMapping.h \
  $(srcdir)/RenderingPipe.h \
  $(srcdir)/RenderingPipeGLSL.h \
  $(srcdir)/RenderingPipeTextureBlendShaderSource.h \
  $(srcdir)/RunTimeStats.h \
  $(srcdir)/VirtualKeyCodesX11.h

if USE_X11
source_h += \
  $(srcdir)/GraphicsDisplayX11.h \
  $(srcdir)/IOpenGLAsmShader.h \
  $(srcdir)/RenderingPipeAsm.h \
  $(srcdir)/XInputWindow.h
else
source_h += \
  $(srcdir)/SystemEventSource.h
endif

if HAVE_GEIS
source_h += \
  $(srcdir)/GestureEvent.h
endif

source_cpp = \
  $(srcdir)/BitmapFormats.cpp \
  $(srcdir)/CairoGraphics.cpp \
  $(srcdir)/Events.cpp \
  $(srcdir)/FontRenderer.cpp \
  $(srcdir)/FontTexture.cpp \
  $(srcdir)/GdkGraphics.cpp \
  $(srcdir)/GLDeviceFrameBufferObject.cpp \
  $(srcdir)/GLDeviceObjects.cpp \
  $(srcdir)/GLError.cpp \
  $(srcdir)/GlobalGraphicsInitializer.cpp \
  $(srcdir)/GLPBuffer.cpp \
  $(srcdir)/GLRenderStates.cpp \
  $(srcdir)/GLResource.cpp \
  $(srcdir)/GLResourceManager.cpp \
  $(srcdir)/GLShader.cpp \
  $(srcdir)/GLShaderParameter.cpp \
  $(srcdir)/GLSh_ColorPicker.cpp \
  $(srcdir)/GLSh_DrawFunction.cpp \
  $(srcdir)/GLSh_Fill.cpp \
  $(srcdir)/GLTemplatePrimitiveBuffer.cpp \
  $(srcdir)/GLTextureResourceManager.cpp \
  $(srcdir)/GLTextureStates.cpp \
  $(srcdir)/GLTimer.cpp \
  $(srcdir)/GLVertexResourceManager.cpp \
  $(srcdir)/GLWindowManager.cpp \
  $(srcdir)/GpuDevice.cpp \
  $(srcdir)/GpuDeviceShader.cpp \
  $(srcdir)/GpuDeviceTexture.cpp \
  $(srcdir)/GpuDeviceVertex.cpp \
  $(srcdir)/GraphicsEngine.cpp \
  $(srcdir)/MeshData.cpp \
  $(srcdir)/MeshFileLoader-OBJ.cpp \
  $(srcdir)/ImageSurface.cpp \
  $(srcdir)/IOpenGLAnimatedTexture.cpp \
  $(srcdir)/IOpenGLBaseTexture.cpp \
  $(srcdir)/IOpenGLCubeTexture.cpp \
  $(srcdir)/IOpenGLFrameBufferObject.cpp \
  $(srcdir)/IOpenGLGLSLShader.cpp \
  $(srcdir)/IOpenGLIndexBuffer.cpp \
  $(srcdir)/IOpenGLPixelBufferOject.cpp \
  $(srcdir)/IOpenGLQuery.cpp \
  $(srcdir)/IOpenGLRectangleTexture.cpp \
  $(srcdir)/IOpenGLSurface.cpp \
  $(srcdir)/IOpenGLTexture2D.cpp \
  $(srcdir)/IOpenGLVertexBuffer.cpp \
  $(srcdir)/IOpenGLVertexDeclaration.cpp \
  $(srcdir)/IOpenGLVolume.cpp \
  $(srcdir)/IOpenGLVolumeTexture.cpp \
  $(srcdir)/NuxGraphics.cpp \
  $(srcdir)/NuxGraphicsObject.cpp \
  $(srcdir)/NuxGraphicsResources.cpp \
  $(srcdir)/RenderingPipe.cpp \
  $(srcdir)/RenderingPipeGLSL.cpp \
  $(srcdir)/RenderingPipeTextureBlend.cpp \
  $(srcdir)/GLRenderingAPI.cpp \
  $(srcdir)/RunTimeStats.cpp


if USE_X11
source_cpp += \
  $(srcdir)/GraphicsDisplayX11.cpp \
  $(srcdir)/IOpenGLAsmShader.cpp \
  $(srcdir)/RenderingPipeAsm.cpp \
  $(srcdir)/XInputWindow.cpp
else
source_cpp += \
  $(srcdir)/SystemEventSource.cpp
endif

if HAVE_GEIS
source_cpp += \
  $(srcdir)/GestureEvent.cpp
endif


libnux_graphics_@NUX_API_VERSION@_la_SOURCES = \
  $(source_cpp) \
  $(source_h)

nux_graphicsdir = $(includedir)/Nux-@NUX_API_VERSION@/NuxGraphics
nux_graphics_HEADERS = \
  $(source_h)

nux-graphics-@NUX_API_VERSION@.pc: nux-graphics.pc
	$(AM_V_GEN) cp -f nux-graphics.pc nux-graphics-@NUX_API_VERSION@.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = nux-graphics-@NUX_API_VERSION@.pc

CLEANFILES += nux-graphics-@NUX_API_VERSION@.pc
DISTCLEANFILES += nux-graphics.pc
EXTRA_DIST += nux-graphics.pc.in
