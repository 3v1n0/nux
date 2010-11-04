CLEANFILES = 
DISTCLEANFILES =
EXTRA_DIST =

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
  $(MAINTAINER_CFLAGS)

libnux_graphics_@NUX_API_VERSION@_la_LIBADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxImage/libnux-image-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxMesh/libnux-mesh-@NUX_API_VERSION@.la \
  $(NUX_GRAPHICS_LIBS)

libnux_graphics_@NUX_API_VERSION@_la_LDFLAGS = \
  $(NUX_LT_LDFLAGS)

source_h = \
  $(srcdir)/FontTexture.h \
  $(srcdir)/FontRenderer.h \
  $(srcdir)/GfxEventsX11.h \
  $(srcdir)/GfxSetupX11.h \
  $(srcdir)/Gfx_Events.h \
  $(srcdir)/Gfx_Interface.h \
  $(srcdir)/GLDeviceFactory.h \
  $(srcdir)/GLDeviceFrameBufferObject.h \
  $(srcdir)/GLDeviceObjects.h \
  $(srcdir)/GLError.h \
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
  $(srcdir)/IniFile.h \
  $(srcdir)/IOpenGLAnimatedTexture.h \
  $(srcdir)/IOpenGLAsmShader.h \
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
  $(srcdir)/OpenGLDefinitions.h \
  $(srcdir)/OpenGLEngine.h \
  $(srcdir)/UIColorTheme.h \
  $(srcdir)/VirtualKeyCodesX11.h \
  $(srcdir)/GlobalGraphicsInitializer.h \
  $(srcdir)/RunTimeStats.h \
  $(srcdir)/RenderingPipe.h \
  $(srcdir)/RenderingPipeAsm.h \
  $(srcdir)/RenderingPipeGLSL.h \
  $(srcdir)/NuxGraphics.h \
  $(srcdir)/NuxGraphicsObject.h \
  $(srcdir)/NuxGraphicsResources.h \
  $(srcdir)/XInputWindow.h

source_cpp = \
  $(srcdir)/UIColorTheme.cpp \
  $(srcdir)/IOpenGLRectangleTexture.cpp \
  $(srcdir)/IOpenGLVertexDeclaration.cpp \
  $(srcdir)/GLDeviceFrameBufferObject.cpp \
  $(srcdir)/GLSh_ColorPicker.cpp \
  $(srcdir)/RenderingPipeGLSL.cpp \
  $(srcdir)/IOpenGLVolumeTexture.cpp \
  $(srcdir)/IOpenGLIndexBuffer.cpp \
  $(srcdir)/GLShaderParameter.cpp \
  $(srcdir)/OpenGLEngine.cpp \
  $(srcdir)/GLResource.cpp \
  $(srcdir)/GLDeviceObjects.cpp \
  $(srcdir)/GLResourceManager.cpp \
  $(srcdir)/GLPBuffer.cpp \
  $(srcdir)/IOpenGLCubeTexture.cpp \
  $(srcdir)/GlobalGraphicsInitializer.cpp \
  $(srcdir)/IOpenGLAnimatedTexture.cpp \
  $(srcdir)/GLRenderStates.cpp \
  $(srcdir)/RenderingPipe.cpp \
  $(srcdir)/IniFile.cpp \
  $(srcdir)/FontTexture.cpp \
  $(srcdir)/FontRenderer.cpp \
  $(srcdir)/GLTextureResourceManager.cpp \
  $(srcdir)/GLTextureStates.cpp \
  $(srcdir)/IOpenGLVolume.cpp \
  $(srcdir)/IOpenGLBaseTexture.cpp \
  $(srcdir)/RenderingPipeAsm.cpp \
  $(srcdir)/GLWindowManager.cpp \
  $(srcdir)/GLSh_DrawFunction.cpp \
  $(srcdir)/GLSh_Fill.cpp \
  $(srcdir)/IOpenGLAsmShader.cpp \
  $(srcdir)/GLDeviceFactory2.cpp \
  $(srcdir)/IOpenGLTexture2D.cpp \
  $(srcdir)/GLVertexResourceManager.cpp \
  $(srcdir)/IOpenGLFrameBufferObject.cpp \
  $(srcdir)/GLShader2.cpp \
  $(srcdir)/GLTemplatePrimitiveBuffer.cpp \
  $(srcdir)/IOpenGLSurface.cpp \
  $(srcdir)/GLRenderingAPI.cpp \
  $(srcdir)/GLDeviceFactory.cpp \
  $(srcdir)/GLError.cpp \
  $(srcdir)/RunTimeStats.cpp \
  $(srcdir)/IOpenGLVertexBuffer.cpp \
  $(srcdir)/GfxSetupX11.cpp \
  $(srcdir)/IOpenGLQuery.cpp \
  $(srcdir)/GLShader.cpp \
  $(srcdir)/IOpenGLGLSLShader.cpp \
  $(srcdir)/GLTimer.cpp \
  $(srcdir)/IOpenGLPixelBufferOject.cpp \
  $(srcdir)/NuxGraphics.cpp \
  $(srcdir)/NuxGraphicsObject.cpp \
  $(srcdir)/NuxGraphicsResources.cpp \
  $(srcdir)/XInputWindow.cpp

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
