CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST = GraphicsDisplayWin.cpp \
             GraphicsDisplayWin.h

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
  $(GEIS_CFLAGS) \
  $(COVERAGE_CFLAGS)

libnux_graphics_@NUX_API_VERSION@_la_LIBADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(NUX_GRAPHICS_LIBS) \
  $(GEIS_LIBS)

libnux_graphics_@NUX_API_VERSION@_la_LDFLAGS = \
  $(NUX_LT_LDFLAGS) \
  $(COVERAGE_LDFLAGS)

source_h = \
  BitmapFormats.h \
  CairoGraphics.h \
  Events.h \
  FontTexture.h \
  FontRenderer.h \
  GdkGraphics.h \
  Gfx_Interface.h \
  GLDeviceFrameBufferObject.h \
  GLDeviceObjects.h \
  GLError.h \
  GlobalGraphicsInitializer.h \
  GLPBuffer.h \
  GLRenderStates.h \
  GLResource.h \
  GLResourceManager.h \
  GLShader.h \
  GLShaderParameter.h \
  GLSh_ColorPicker.h \
  GLSh_DrawFunction.h \
  GLSh_Fill.h \
  GLTemplatePrimitiveBuffer.h \
  GLTextureResourceManager.h \
  GLTextureStates.h \
  GLThread.h \
  GLTimer.h \
  GLVertexResourceManager.h \
  GLWindowManager.h \
  GpuDevice.h \
  GraphicsDisplay.h \
  GraphicsEngine.h \
  MeshData.h \
  MeshFileLoader-OBJ.h \
  ImageSurface.h \
  IOpenGLAnimatedTexture.h \
  IOpenGLBaseTexture.h \
  IOpenGLCubeTexture.h \
  IOpenGLFrameBufferObject.h \
  IOpenGLGLSLShader.h \
  IOpenGLIndexBuffer.h \
  IOpenGLPixelBufferOject.h \
  IOpenGLQuery.h \
  IOpenGLRectangleTexture.h \
  IOpenGLResource.h \
  IOpenGLSurface.h \
  IOpenGLTexture2D.h \
  IOpenGLVertexBuffer.h \
  IOpenGLVertexDeclaration.h \
  IOpenGLVolume.h \
  IOpenGLVolumeTexture.h \
  NuxGraphics.h \
  NuxGraphicsObject.h \
  NuxGraphicsResources.h \
  OpenGLDefinitions.h \
  OpenGLMapping.h \
  RenderingPipe.h \
  RenderingPipeGLSL.h \
  RenderingPipeTextureBlendShaderSource.h \
  RunTimeStats.h

if USE_X11
source_h += \
  GraphicsDisplayX11.h \
  VirtualKeyCodesX11.h \
  XInputWindow.h
endif

if !NUX_OPENGLES_20
source_h += \
  IOpenGLAsmShader.h \
  RenderingPipeAsm.h
endif

if HAVE_GEIS
source_h += \
  GestureEvent.h
endif

source_cpp = \
  BitmapFormats.cpp \
  CairoGraphics.cpp \
  Events.cpp \
  FontRenderer.cpp \
  FontTexture.cpp \
  GdkGraphics.cpp \
  GLDeviceFrameBufferObject.cpp \
  GLDeviceObjects.cpp \
  GLError.cpp \
  GlobalGraphicsInitializer.cpp \
  GLPBuffer.cpp \
  GLRenderStates.cpp \
  GLResource.cpp \
  GLResourceManager.cpp \
  GLShader.cpp \
  GLShaderParameter.cpp \
  GLSh_ColorPicker.cpp \
  GLSh_DrawFunction.cpp \
  GLSh_Fill.cpp \
  GLTemplatePrimitiveBuffer.cpp \
  GLTextureResourceManager.cpp \
  GLTextureStates.cpp \
  GLTimer.cpp \
  GLVertexResourceManager.cpp \
  GLWindowManager.cpp \
  GpuDevice.cpp \
  GpuDeviceShader.cpp \
  GpuDeviceTexture.cpp \
  GpuDeviceVertex.cpp \
  GraphicsEngine.cpp \
  MeshData.cpp \
  MeshFileLoader-OBJ.cpp \
  ImageSurface.cpp \
  IOpenGLAnimatedTexture.cpp \
  IOpenGLBaseTexture.cpp \
  IOpenGLCubeTexture.cpp \
  IOpenGLFrameBufferObject.cpp \
  IOpenGLGLSLShader.cpp \
  IOpenGLIndexBuffer.cpp \
  IOpenGLPixelBufferOject.cpp \
  IOpenGLQuery.cpp \
  IOpenGLRectangleTexture.cpp \
  IOpenGLSurface.cpp \
  IOpenGLTexture2D.cpp \
  IOpenGLVertexBuffer.cpp \
  IOpenGLVertexDeclaration.cpp \
  IOpenGLVolume.cpp \
  IOpenGLVolumeTexture.cpp \
  NuxGraphics.cpp \
  NuxGraphicsObject.cpp \
  NuxGraphicsResources.cpp \
  RenderingPipe.cpp \
  RenderingPipeGLSL.cpp \
  RenderingPipeTextureBlend.cpp \
  GLRenderingAPI.cpp \
  RunTimeStats.cpp

if USE_X11
source_cpp += \
  GraphicsDisplayX11.cpp \
  XInputWindow.cpp
endif

if !NUX_OPENGLES_20
source_cpp += \
  IOpenGLAsmShader.cpp \
  RenderingPipeAsm.cpp
endif

if HAVE_GEIS
source_cpp += \
  GestureEvent.cpp
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

unused_src = \
 FreetypeFont.cpp \
 FreetypeFont.h \
 GdiImageLoader.cpp \
 GdiImageLoader.h \
 GfxEventsX11.h \
 IOpenGLCgShader.cpp \
 IOpenGLCgShader.h \
 Readme.txt \
 VirtualKeyCodes.h


CLEANFILES += nux-graphics-@NUX_API_VERSION@.pc
DISTCLEANFILES += nux-graphics.pc
EXTRA_DIST += nux-graphics.pc.in ${unused_src}
