CLEANFILES = 
DISTCLEANFILES =
EXTRA_DIST =

AM_CPPFLAGS = \
  -I$(srcdir) \
  -I$(top_srcdir) \
  -DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
  -DDATADIR=\""$(datadir)"\" \
  -DG_LOG_DOMAIN=\"NuxGraphics\"

lib_LTLIBRARIES = \
  libnux-graphics-@NUX_API_VERSION@.la

libnux_graphics_@NUX_API_VERSION@_la_CPPFLAGS= \
  $(GCC_FLAGS) \
  $(NUX_GRAPHICS_CFLAGS) \
  $(MAINTAINER_CFLAGS)

libnux_graphics_@NUX_API_VERSION@_la_LIBADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxCore/libnux-image-@NUX_API_VERSION@.la \
  $(NUX_GRAPHICS_LIBS)

libnux_graphics_@NUX_API_VERSION@_la_LDFLAGS = \
  $(NUX_LT_LDFLAGS)

source_h = \
  $(srcdir)/GlobalGraphicsInitializer.h \
  $(srcdir)/OpenGLDefinitions.h \
  $(srcdir)/IOpenGLVertexBuffer.h \
  $(srcdir)/IOpenGLBaseTexture.h \
  $(srcdir)/GLRenderStates.h \
  $(srcdir)/VirtualKeyCodes.h \
  $(srcdir)/IOpenGLTexture2D.h \
  $(srcdir)/UIColorTheme.h \
  $(srcdir)/IOpenGLFrameBufferObject.h \
  $(srcdir)/FreetypeFont.h \
  $(srcdir)/IOpenGLVolumeTexture.h \
  $(srcdir)/GLShaderParameter.h \
  $(srcdir)/GLShader.h \
  $(srcdir)/IOpenGLIndexBuffer.h \
  $(srcdir)/IOpenGLAnimatedTexture.h \
  $(srcdir)/RenderingPipeAsm.h \
  $(srcdir)/GLWindowManager.h \
  $(srcdir)/GLResourceManager.h \
  $(srcdir)/IOpenGLQuery.h \
  $(srcdir)/IOpenGLVertexDeclaration.h \
  $(srcdir)/GLTimer.h \
  $(srcdir)/GLTextureResourceManager.h \
  $(srcdir)/GLDeviceFrameBufferObject.h \
  $(srcdir)/FontTexture.h \
  $(srcdir)/IOpenGLSurface.h \
  $(srcdir)/GLDeviceFactory.h \
  $(srcdir)/RunTimeStats.h \
  $(srcdir)/IOpenGLRectangleTexture.h \
  $(srcdir)/IOpenGLGLSLShader.h \
  $(srcdir)/GLSh_DrawFunction.h \
  $(srcdir)/GLSh_ColorPicker.h \
  $(srcdir)/GfxEventsX11.h \
  $(srcdir)/VirtualKeyCodesX11.h \
  $(srcdir)/GLError.h \
  $(srcdir)/Gfx_OpenGL.h \
  $(srcdir)/GLTextureStates.h \
  $(srcdir)/GLVertexResourceManager.h \
  $(srcdir)/OpenGLEngine.h \
  $(srcdir)/RenderingPipeGLSL.h \
  $(srcdir)/GLThread.h \
  $(srcdir)/GLTemplatePrimitiveBuffer.h \
  $(srcdir)/GfxSetupX11.h \
  $(srcdir)/GLResource.h \
  $(srcdir)/IOpenGLPixelBufferOject.h \
  $(srcdir)/Gfx_Interface.h \
  $(srcdir)/IOpenGLAsmShader.h \
  $(srcdir)/GLSh_Fill.h \
  $(srcdir)/GLPBuffer.h \
  $(srcdir)/IOpenGLResource.h \
  $(srcdir)/Gfx_Events.h \
  $(srcdir)/RenderingPipe.h \
  $(srcdir)/GLDeviceObjects.h \
  $(srcdir)/IniFile.h \
  $(srcdir)/IOpenGLCubeTexture.h \
  $(srcdir)/IOpenGLVolume.h

source_cpp = \
  $(srcdir)/UIColorTheme.cpp \
  $(srcdir)/IOpenGLRectangleTexture.cpp \
  $(srcdir)/IOpenGLVertexDeclaration.cpp \
  $(srcdir)/GLDeviceFrameBufferObject.cpp \
  $(srcdir)/GLSh_ColorPicker.cpp \
  $(srcdir)/RenderingPipeGLSL.cpp \
  $(srcdir)/IOpenGLVolumeTexture.cpp \
  $(srcdir)/FreetypeFont.cpp \
  $(srcdir)/IOpenGLIndexBuffer.cpp \
  $(srcdir)/GLShaderParameter.cpp \
  $(srcdir)/OpenGLEngine.cpp \
  $(srcdir)/GLResource.cpp \
  $(srcdir)/GLDeviceObjects.cpp \
  $(srcdir)/GLResourceManager.cpp \
  $(srcdir)/IOpenGLCubeTexture.cpp \
  $(srcdir)/GlobalGraphicsInitializer.cpp \
  $(srcdir)/IOpenGLAnimatedTexture.cpp \
  $(srcdir)/GLRenderStates.cpp \
  $(srcdir)/RenderingPipe.cpp \
  $(srcdir)/IniFile.cpp \
  $(srcdir)/FontTexture.cpp \
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
  $(srcdir)/GLPBuffer.cpp \
  $(srcdir)/Gfx_OpenGL.cpp \
  $(srcdir)/GLShader.cpp \
  $(srcdir)/IOpenGLGLSLShader.cpp \
  $(srcdir)/GLTimer.cpp \
  $(srcdir)/IOpenGLPixelBufferOject.cpp

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
