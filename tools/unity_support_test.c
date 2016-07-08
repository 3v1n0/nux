// Copyright © 2011 Canonical Ltd
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License version 3 as published by the
// Free Software Foundation. This program is distributed in the hope that it
// will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details. You should have received a copy of the GNU
// General Public License along with this program. If not, see
// <http://www.gnu.org/licenses/>.
//
// Authored by:
//     Loïc Molinari <loic.molinari@canonical.com>
//     Jay Taoko <jay.taoko@canonical.com>

// unity_support_test checks for Unity support on a X11 display. All the checks
// are based on what's done in Compiz (core/plugins/opengl/src/screen.cpp).
//
// $ gcc -std=c99 unity_support_test.c -o unity_support_test `pkg-config
// > --cflags --libs gl x11 libpci xcomposite xdamage`

#include <pci/pci.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#ifndef NUX_OPENGLES_20
#include <GL/gl.h>
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#undef GLX_GLXEXT_PROTOTYPES
#else
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef NUX_OPENGLES_20
typedef GLXContext NUXContext;
#else
typedef EGLConfig NUXContext;
#endif

// Enables colored console output at build time.
#define COLORED_OUTPUT 1

// Gets the number of elements in an array.
#define ARRAY_SIZE(array) (sizeof (array) / sizeof ((array)[0]))

enum {
  // Extension flags.
  FLAG_GLX_SGIX_FBCONFIG           = (1 << 0),
  FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP = (1 << 1),
  FLAG_GL_ARB_NON_POWER_OF_TWO     = (1 << 2),
  FLAG_GL_NV_TEXTURE_RECTANGLE     = (1 << 3),
  FLAG_GL_EXT_TEXTURE_RECTANGLE    = (1 << 4),
  FLAG_GL_ARB_TEXTURE_RECTANGLE    = (1 << 5),
  FLAG_GL_ARB_VERTEX_PROGRAM       = (1 << 6),
  FLAG_GL_ARB_FRAGMENT_PROGRAM     = (1 << 7),
  FLAG_GL_ARB_VERTEX_BUFFER_OBJECT = (1 << 8),
  FLAG_GL_EXT_FRAMEBUFFER_OBJECT   = (1 << 9),
  FLAG_GL_ARB_FRAMEBUFFER_OBJECT   = (1 << 10),
  FLAG_SOFTWARE_RENDERING          = (1 << 11),
  FLAG_BLACKLISTED                 = (1 << 12),
  FLAG_GL_OES_EGL_IMAGE            = (1 << 13),
  FLAG_EGL_KHR_IMAGE_PIXMAP        = (1 << 14),

  // Extension masks.
  MASK_GL_NON_POWER_OF_TWO   = (FLAG_GL_ARB_NON_POWER_OF_TWO
                                | FLAG_GL_NV_TEXTURE_RECTANGLE
                                | FLAG_GL_EXT_TEXTURE_RECTANGLE
                                | FLAG_GL_ARB_TEXTURE_RECTANGLE),
  MASK_GL_FRAMEBUFFER_OBJECT = (FLAG_GL_EXT_FRAMEBUFFER_OBJECT
                                | FLAG_GL_ARB_FRAMEBUFFER_OBJECT)
};

// PCI device identity.
struct PciDevice {
  unsigned short vendor;
  unsigned short device;
};

// Blacklists of GPUs for Compiz.
struct PciDevice gpu_blacklist[] = {
  { 0x8086, 0x3577 },  // Intel : 82830M/MG
  { 0x8086, 0x2562 },  // Intel : 82845G aka Poulsbo
  { 0x1002, 0x4c57 },  // ATI : Radeon Mobility 7500
  { 0x10de, 0x0322 },  // nVidia: GeForce FX 5200
  { 0x10de, 0x0326 },  // nVidia: GeForce FX 5500
  { 0x10de, 0x0240 },  // nVidia: GeForce 6150
  { 0x10de, 0x01d3 },  // nVidia: GeForce Go 7300 SE / 7200 GS
  { 0x10de, 0x01d7 },  // nVidia: GeForce Go 7300 / Quadro NVS 110M
  { 0x10de, 0x01d8 }   // nVidia: GeForce Go 7400
};

typedef struct _TestResults {
  char *vendor, *renderer, *version;
  int result, major, minor;
  int indirect;
  int compiz;
  unsigned int flags;
  char	       *error;
} TestResults;

// Checks whether an extension is supported by the GLX/OpenGL implementation
// given the extension name and the list of supported extensions.
static int is_extension_supported (const char* extensions,
                                   const char* extension) {
  if (extensions != NULL && extension != NULL) {
    const size_t len = strlen (extension);
    char* p = (char*) extensions;
    char* end = p + strlen (p);
    while (p < end) {
      const size_t size = strcspn (p, " ");
      if (len == size && strncmp (extension, p, size) == 0)
        return 1;
      p += size + 1;
    }
  }
  return 0;
}

// Gets the OpenGL version number given the string.
static void get_opengl_version (const char *version, int* major, int* minor) {
  int tmp = 0, i;

  if (!version)
  {
    *major = 0;
    *minor = 0;
    return;
  }

  for (i = 0; isdigit (version[i]); i++)
    tmp = tmp * 10 + (version[i] - 48);
  if (version[i++] == '.') {
    *major = tmp;
    *minor = (version[i] - 48);
  } else {
    *major = 0;
    *minor = 0;
  }
}

static void print_help () {
  fprintf (stdout,
           "Check for Unity support on a X11 display.\n"
           "\n"
           "Usage:\n"
           "  unity-support-test [ options ]\n"
           "    -d, --display name: Specify the X11 display\n"
           "    -i, --indirect:     Force an indirect rendering context\n"
           "    -p, --print:        Print detection results on stdout\n"
           "    -c, --compiz:       Only check for Compiz support\n"
           "    -f  --force-check   Force checking already performed tests\n"
           "    -h, --help:         Show help\n");
}

static void print_report (const char* vendor, const char* renderer,
                          const char* version, int major, int minor,
                          unsigned int flags, int compiz, int result,
                          const char* error) {
  if (error == NULL) {
#if COLORED_OUTPUT == 1
    const char* yes = "\033[32;01myes\033[00m";
    const char* no = "\033[31;01mno\033[00m";
#else
    const char* yes = "yes";
    const char* no = "no";
#endif
    fprintf (stdout,
             "OpenGL vendor string:   %s\n"
             "OpenGL renderer string: %s\n"
             "OpenGL version string:  %s\n"
             "\n"
             "Not software rendered:    %s\n"
             "Not blacklisted:          %s\n"
             "GLX fbconfig:             %s\n"
             "GLX texture from pixmap:  %s\n"
             "GL npot or rect textures: %s\n",
             vendor, renderer, version,
             flags & FLAG_SOFTWARE_RENDERING ? no : yes,
             flags & FLAG_BLACKLISTED ? no : yes,
             flags & FLAG_GLX_SGIX_FBCONFIG ? yes : no,
             flags & FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP ? yes : no,
             flags & MASK_GL_NON_POWER_OF_TWO ? yes : no);
    if (compiz == 0) {
#ifndef NUX_OPENGLES_20
      fprintf (stdout,
               "GL vertex program:        %s\n"
               "GL fragment program:      %s\n"
               "GL vertex buffer object:  %s\n"
               "GL framebuffer object:    %s\n"
               "GL version is 1.4+:       %s\n"
               "\n"
               "Unity 3D supported:       %s\n",
               flags & FLAG_GL_ARB_VERTEX_PROGRAM ? yes : no,
               flags & FLAG_GL_ARB_FRAGMENT_PROGRAM ? yes : no,
               flags & FLAG_GL_ARB_VERTEX_BUFFER_OBJECT ? yes : no,
               flags & MASK_GL_FRAMEBUFFER_OBJECT ? yes : no,
               (major >= 2 || (major == 1 && minor >= 4)) ? yes : no,
               result == 0 ? yes : no);
#else
      fprintf (stdout,
               "GL OES EGL image:         %s\n"
               "EGL KHR image pixmap:     %s\n"
               "EGL version is 1.4+:      %s\n"
               "\n"
               "Unity supported:          %s\n",
               flags & FLAG_GL_OES_EGL_IMAGE ? yes : no,
               flags & FLAG_EGL_KHR_IMAGE_PIXMAP ? yes : no,
               (major >= 2 || (major == 1 && minor >= 4)) ? yes : no,
               result == 0 ? yes : no);
#endif
    } else {
      fprintf (stdout, "\nCompiz supported:         %s\n",
               result == 0 ? yes : no);
    }
  } else {
    fprintf (stdout, "Error: %s\n", error);
  }
}

static int check_root_visual (Display     *display,
			      unsigned int screen,
			      Window      root,
			      NUXContext  *context,
			      XVisualInfo **vinfos,
			      TestResults  *results)
{
  // Retrieve root window visual.
  XWindowAttributes attr;
  XVisualInfo vinfo_template;
  int nr_vinfos;
  if (XGetWindowAttributes (display, root, &attr) == 0) {
    results->error = strdup ("unable to get root window attributes");
    results->result = 1;
    return 0;
  }
  vinfo_template.visualid = XVisualIDFromVisual (attr.visual);
  *vinfos = XGetVisualInfo (display, VisualIDMask, &vinfo_template, &nr_vinfos);
  if (nr_vinfos == 0) {
    results->error = strdup ("unable to get visual informations for default visual");
    results->result = 1;
    return 0;
  }

  return 1;
}

static int check_xcomposite (Display     *display,
			      unsigned int screen,
			      Window      root,
			      NUXContext  *context,
			      XVisualInfo **vinfos,
			      TestResults  *results)
{
// Check for XComposite
  int composite_major, composite_minor;
  int composite_tmp;

  if (!XQueryExtension (display, COMPOSITE_NAME, &composite_tmp, &composite_tmp, &composite_tmp))
  {
    results->error = strdup ("no composite extension");
    results->result = 1;
    return 0;
  }

  XCompositeQueryVersion (display, &composite_major, &composite_minor);

  if (composite_major == 0 && composite_minor < 2)
  {
    results->error = strdup ("old composite extension");
    results->result = 1;
    return 0;
  }

  return 1;
}

static int check_damage_extension (Display     *display,
			            unsigned int screen,
				    Window      root,
				    NUXContext  *context,
			   	    XVisualInfo **vinfos,
			   	    TestResults  *results)
{
  int damage_tmp;

  if (!XDamageQueryExtension (display, &damage_tmp, &damage_tmp))
  {
    results->error = strdup ("no damage extension");
    results->result = 1;
    return 0;
  }

  return 1;
}

static int check_fixes_extension (Display     *display,
			           unsigned int screen,
			     	   Window      root,
			      	   NUXContext  *context,
				   XVisualInfo **vinfos,
				   TestResults  *results)
{
  int fixes_tmp;

  if (!XFixesQueryExtension (display, &fixes_tmp, &fixes_tmp))
  {
    results->error = strdup ("no fixes extension");
    results->result = 1;
    return 0;
  }

  return 1;
}

#ifdef NUX_OPENGLES_20

static int check_egl_config (Display     *display,
			      unsigned int screen,
			      Window      root,
			      NUXContext  *context,
			      XVisualInfo **vinfos,
			      TestResults  *results)
{
  EGLint attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_RED_SIZE, 1,
    EGL_GREEN_SIZE, 1,
    EGL_BLUE_SIZE, 1,
    EGL_NONE
  };
  EGLint ctx_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };

  XVisualInfo *visInfo, visTemplate;
  XSetWindowAttributes attr;
  Window win;
  int num_visuals;
  unsigned long mask;
  const int width = 400, height = 300;
  const char* name = "Unity Support Test";
  EGLDisplay egl_dpy;
  EGLSurface egl_surf;
  EGLContext egl_ctx;
  EGLConfig config;
  EGLint num_configs;
  EGLint vid;

  egl_dpy = eglGetDisplay(display);

  if (!eglChooseConfig (egl_dpy, attribs, &config, 1, &num_configs)) {
    results->error = strdup ("OpenGLES: couldn't get an EGL visual config");
    results->result = 1;
    return 0;
  }

  if (num_configs <= 0) {
    results->error = strdup ("OpenGLES: no valid config found (!num_configs)");
    results->result = 1;
    return 0;
  }

  if (!eglGetConfigAttrib (egl_dpy, config, EGL_NATIVE_VISUAL_ID, &vid)) {
    results->error = strdup ("OpenGLES: eglGetConfigAttrib() failed");
    results->result = 1;
    return 0;
  }

  /* The X window visual must match the EGL config */
  visTemplate.visualid = vid;
  visInfo = XGetVisualInfo (display, VisualIDMask, &visTemplate, &num_visuals);
  if (!visInfo) {
    results->error = strdup ("OpenGLES: unable to get a matching X visual");
    results->result = 1;
    return 0;
  }

  /* window attributes */
  attr.background_pixel = 0;
  attr.border_pixel = 0;
  attr.colormap = XCreateColormap (display, root, visInfo->visual, AllocNone);
  attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
  mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

  win = XCreateWindow (display, root, 0, 0, width, height,
                       0, visInfo->depth, InputOutput,
                       visInfo->visual, mask, &attr);

  /* set hints and properties */
  {
    XSizeHints sizehints;
    sizehints.x = 0;
    sizehints.y = 0;
    sizehints.width  = width;
    sizehints.height = height;
    sizehints.flags = USSize | USPosition;
    XSetNormalHints (display, win, &sizehints);
    XSetStandardProperties (display, win, name, name,
                            None, (char **)NULL, 0, &sizehints);
  }

  eglBindAPI (EGL_OPENGL_ES_API);

  egl_surf = eglCreateWindowSurface (egl_dpy, config, win, NULL);
  if (egl_surf == EGL_NO_SURFACE) {
    results->error = strdup ("OpenGLES: eglCreateWindowSurface failed");
    results->result = 1;
    return 0;
  }

  context = eglCreateContext (egl_dpy, config, EGL_NO_CONTEXT, ctx_attribs);
  if (context == EGL_NO_CONTEXT) {
    results->error = strdup ("OpenGLES: eglCreateContext failed");
    results->result = 1;
    return 0;
  }

  if (!eglMakeCurrent (egl_dpy, egl_surf, egl_surf, context)) {
    results->error = strdup ("OpenGLES: eglMakeCurrent() failed");
    results->result = 1;
    return 0;
  }

  XFree(visInfo);
  eglDestroySurface (egl_dpy, egl_surf);
  XDestroyWindow (display, win);

  return 1;
}

#else

static int check_glx_config (Display     *display,
			      unsigned int screen,
			      Window      root,
			      NUXContext  *context,
			      XVisualInfo **vinfos,
			      TestResults  *results)
{
  // Check root window visual capabilities.
  int value;
  glXGetConfig (display, *vinfos, GLX_USE_GL, &value);
  if (value == 0) {
    results->error = strdup ("OpenGL rendering is not supported by the root visual");
    results->result = 1;
    return 0;
  }

  return 1;
}

static int check_colorbuffers (Display     *display,
			        unsigned int screen,
				Window      root,
				NUXContext  *context,
				XVisualInfo **vinfos,
				TestResults  *results)
{
  int value;
  glXGetConfig (display,*vinfos, GLX_DOUBLEBUFFER, &value);
  if (value == 0) {
    results->error = strdup ("color buffers of the root visual are not double-buffered");
    results->result = 1;
    return 0;
  }

  return 1;
}
#endif

static int check_context (Display     *display,
			   unsigned int screen,
			   Window      root,
			   NUXContext  *context,
			   XVisualInfo **vinfos,
			   TestResults  *results)
{
#ifndef NUX_OPENGLES_20
  // Create and map the OpenGL context to the root window and get the strings.
  *context = glXCreateContext (display, *vinfos, NULL, !results->indirect);
  if (*context == NULL) {
    results->error = strdup ("unable to create the OpenGL context");
    results->result = 1;
    return 0;
  }
  glXMakeCurrent (display, root, *context);
#endif

  results->vendor = (char*) glGetString (GL_VENDOR);
  results->renderer = (char*) glGetString (GL_RENDERER);
  results->version = (char*) glGetString (GL_VERSION);

  return 1;
}

static int check_extensions (Display     *display,
			     unsigned int screen,
			     Window      root,
			     NUXContext  *context,
			     XVisualInfo **vinfos,
			     TestResults  *results)
{
  // Fill flags with the supported OpenGL extensions.
  const char* gl_extensions = (char*)glGetString (GL_EXTENSIONS);
  if (gl_extensions == NULL) {
    results->error = strdup ("invalid OpenGL extensions string");
    results->result = 1;
    return 0;
  }
  const struct { const char* name; const unsigned int flag; } gl_extension[] = {
    { "GL_ARB_texture_non_power_of_two", FLAG_GL_ARB_NON_POWER_OF_TWO },
    { "GL_NV_texture_rectangle", FLAG_GL_NV_TEXTURE_RECTANGLE },
    { "GL_EXT_texture_rectangle", FLAG_GL_EXT_TEXTURE_RECTANGLE },
    { "GL_ARB_texture_rectangle", FLAG_GL_ARB_TEXTURE_RECTANGLE },
    { "GL_ARB_vertex_program", FLAG_GL_ARB_VERTEX_PROGRAM },
    { "GL_ARB_fragment_program", FLAG_GL_ARB_FRAGMENT_PROGRAM },
    { "GL_ARB_vertex_buffer_object", FLAG_GL_ARB_VERTEX_BUFFER_OBJECT },
    { "GL_EXT_framebuffer_object", FLAG_GL_EXT_FRAMEBUFFER_OBJECT },
    { "GL_ARB_framebuffer_object", FLAG_GL_ARB_FRAMEBUFFER_OBJECT },
    { "GL_OES_EGL_image", FLAG_GL_OES_EGL_IMAGE },
    { NULL, 0 }
  };
  for (int i = 0; gl_extension[i].name != NULL; i++)
    if (is_extension_supported (gl_extensions, gl_extension[i].name) == 1)
      results->flags |= gl_extension[i].flag;

#ifndef NUX_OPENGLES_20
  // Fill results->flags with the supported GLX extensions.
  const char* glx_extensions = glXQueryExtensionsString (display, screen);
  const struct { const char* name; const unsigned int flag; } glx_extension[] = {
    { "GLX_SGIX_fbconfig", FLAG_GLX_SGIX_FBCONFIG },
    { "GLX_EXT_texture_from_pixmap", FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP },
    { NULL, 0 }
  };
  for (int i = 0; glx_extension[i].name != NULL; i++)
    if (is_extension_supported (glx_extensions, glx_extension[i].name) == 1)
      results->flags |= glx_extension[i].flag;
  if (results->flags & FLAG_GLX_SGIX_FBCONFIG) {
    if (glXGetProcAddressARB ((unsigned char*)"glXQueryDrawable") == NULL ||
        glXGetProcAddressARB ((unsigned char*)"glXGetFBConfigs") == NULL ||
        glXGetProcAddressARB ((unsigned char*)"glXGetFBConfigAttrib") == NULL ||
        glXGetProcAddressARB ((unsigned char*)"glXCreatePixmap") == NULL ||
        glXGetProcAddressARB ((unsigned char*)"glXDestroyPixmap") == NULL) {
      results->flags &= ~FLAG_GLX_SGIX_FBCONFIG;
    }
  }
  if (results->flags & FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP) {
    if (glXGetProcAddressARB ((unsigned char*)"glXBindTexImageEXT") == NULL ||
        glXGetProcAddressARB ((unsigned char*)"glXReleaseTexImageEXT") == NULL) {
      results->flags &= ~FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP;
    }
  }
#else
  EGLDisplay egl_dpy = eglGetDisplay(display);
  const char* egl_extensions = eglQueryString (egl_dpy, EGL_EXTENSIONS);
  const struct { const char* name; const unsigned int flag; } egl_extension[] = {
    { "EGL_KHR_image_pixmap", FLAG_EGL_KHR_IMAGE_PIXMAP },
    { NULL, 0 }
  };
  for (int i = 0; egl_extension[i].name != NULL; i++)
    if (is_extension_supported (egl_extensions, egl_extension[i].name) == 1)
      results->flags |= egl_extension[i].flag;
#endif

  return 1;
}

static int check_blacklist (Display     *display,
			    unsigned int screen,
			    Window      root,
			    NUXContext  *context,
			    XVisualInfo **vinfos,
			    TestResults  *results)
{
  // Check for software rendering.
  if (results->renderer != NULL &&
      (strncmp (results->renderer, "Software Rasterizer", 19) == 0 ||
       strncmp (results->renderer, "Mesa X11", 8) == 0 ||
       strstr (results->renderer, "llvmpipe") ||
       strstr (results->renderer, "on softpipe"))) {
    results->flags |= FLAG_SOFTWARE_RENDERING;
  }

  // jaytaoko: Balcklist the Geforce FX cards
  if (results->renderer != NULL) {
    if (strstr (results->renderer, "GeForce FX") ||
        (getenv("UNITY_LOW_GFX_MODE") != NULL && atoi(getenv("UNITY_LOW_GFX_MODE")) == 1)) {
      results->flags |= FLAG_BLACKLISTED;
    }
  }

  // FIXME(loicm): Compiz does a last check to test whether there's a fbconfig
  //     available for the default depth or not.

  // Scan the PCI devices searching for blacklisted GPUs.
// FIXME: pci or not is not actually related with PCI, it's just that if pci_init
// fails it exit directly :-(
#ifndef NUX_OPENGLES_20
  const int gpu_blacklist_size = ARRAY_SIZE (gpu_blacklist);
  struct pci_access* access;
  struct pci_dev* dev;
  access = pci_alloc ();
  pci_init (access);
  pci_scan_bus (access);
  dev = access->devices;
  while (dev != NULL) {
    pci_fill_info (dev, PCI_FILL_IDENT);
    for (int i = 0; i < gpu_blacklist_size; i++) {
      if (dev->vendor_id == gpu_blacklist[i].vendor &&
          dev->device_id == gpu_blacklist[i].device) {
        results->flags |= FLAG_BLACKLISTED;
      }
    }
    dev = dev->next;
  }
  pci_cleanup (access);
#endif

  return 1;
}

int (*tests[]) (Display     *display,
		      unsigned int screen,
		      Window      root,
		      NUXContext  *context,
		      XVisualInfo **vinfos,
		      TestResults  *results) = {
  check_root_visual,
  check_xcomposite,
  check_damage_extension,
  check_fixes_extension,
#ifndef NUX_OPENGLES_20
  check_glx_config,
  check_colorbuffers,
#else
  check_egl_config,
#endif
  check_context,
  check_extensions,
  check_blacklist
};

#ifndef NUX_OPENGLES_20
const unsigned int c_num_tests = 9;
#else
const unsigned int c_num_tests = 8;
#endif

int main (int argc, char* argv[]) {
  char         *display_name = NULL;
  int          screen;
  unsigned int print = 0;
  Window       root;
  XVisualInfo  *vinfos = NULL;
  Display      *display = NULL;
  NUXContext   context = NULL;
  TestResults  results;
#ifdef NUX_OPENGLES_20
  EGLDisplay   egl_dpy;
#endif
  char         resultfilename[30];
  int          resultfile;
  int          forcecheck = 0;

  memset(&results, 0, sizeof(TestResults));

  // Basic command-line parsing.
  for (int i = 1; i < argc; i++) {
    if (((strncmp (argv[i], "-d", 2) == 0) ||
         (strncmp (argv[i], "--display", 9) == 0)) &&
        (i + 1 < argc)) {
      display_name = argv[i + 1];
      i++;
    } else if ((strncmp (argv[i], "-i", 2) == 0) ||
               (strncmp (argv[i], "--indirect", 10) == 0)) {
      results.indirect = 1;
    } else if ((strncmp (argv[i], "-p", 2) == 0) ||
               (strncmp (argv[i], "--print", 7) == 0)) {
      print = 1;
    } else if ((strncmp (argv[i], "-c", 2) == 0) ||
               (strncmp (argv[i], "--compiz", 8) == 0)) {
      results.compiz = 1;
    } else if ((strncmp (argv[i], "-f", 2) == 0) ||
               (strncmp (argv[i], "--force-check", 13) == 0)) {
      forcecheck = 1;
    } else if ((strncmp (argv[i], "-h", 2) == 0) ||
               (strncmp (argv[i], "--help", 6) == 0)) {
      print_help ();
      return 2;
    } else {
      fprintf (stderr, "Error: unknown command-line option `%s'\n\n", argv[i]);
      print_help ();
      return 2;
    }
  }

  // can skip some tests if not forced
  if (!forcecheck && !print) {
      if (access("/tmp/unity_support_test.0", F_OK) == 0) {
          return 0;
      }
      if (getenv ("UNITY_FORCE_START")) {
          fprintf (stdout, "Warning: UNITY_FORCE_START enabled, no check for unity or compiz support.\n");
          return 0;
      }
  }

  // Open a X11 connection and get the root window.
  display = XOpenDisplay (display_name);
  
#ifndef NUX_OPENGLES_20  
  // Before doing anything with GLX, check that it is supported on the system.
  Bool glx_supported = False;
  int dummy0, dummy1;
  if (display)
    glx_supported = glXQueryExtension(display, &dummy0, &dummy1);

#endif

  if (!display) {
    results.error = strdup ("unable to open display");
    // exit with 5, to tell "it's not an error we should cache"
    results.result = 5;
  }
#ifndef NUX_OPENGLES_20
  else if (!glx_supported) {
    results.error = strdup ("GLX is not available on the system");
    // exit with 5, to tell "it's not an error we should cache"
    results.result = 5;
  }
#endif
  else
  {
    screen = DefaultScreen (display);
    root = XRootWindow (display, screen);

#ifndef NUX_OPENGLES_20
    // Do the tests, if one of them fails break out of the loop

    for (unsigned int i = 0; i < c_num_tests; i++)
      if (!(*tests[i]) (display, screen, root, &context, &vinfos, &results))
        break;

    if (results.compiz == 0) {
      // Unity compatibility checks.
      get_opengl_version (results.version, &results.major, &results.minor);
      if ((results.major >= 2 || (results.major == 1 && results.minor >= 4)) &&
          (results.flags & FLAG_GLX_SGIX_FBCONFIG) &&
          (results.flags & FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP) &&
          (results.flags & MASK_GL_NON_POWER_OF_TWO) &&
          (results.flags & FLAG_GL_ARB_VERTEX_PROGRAM) &&
          (results.flags & FLAG_GL_ARB_FRAGMENT_PROGRAM) &&
          (results.flags & FLAG_GL_ARB_VERTEX_BUFFER_OBJECT) &&
          (results.flags & MASK_GL_FRAMEBUFFER_OBJECT) &&
          (~results.flags & FLAG_SOFTWARE_RENDERING) &&
          (~results.flags & FLAG_BLACKLISTED)) {
        results.result = 0;
      } else {
        results.result = 1;
      }
    } else {
      // Compiz compatibility checks.
      if ((results.flags & FLAG_GLX_SGIX_FBCONFIG) &&
          (results.flags & FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP) &&
          (results.flags & MASK_GL_NON_POWER_OF_TWO) &&
          (~results.flags & FLAG_SOFTWARE_RENDERING) &&
          (~results.flags & FLAG_BLACKLISTED)) {
        results.result = 0;
      } else {
        results.result = 1;
      }
    }
#else
    egl_dpy = eglGetDisplay(display);
    if (!eglInitialize (egl_dpy, &results.major, &results.minor)) {
      results.error = strdup ("OpenGLES: eglInitialize() failed");
      results.result = 1;
    } else {
      // Do the tests, if one of them fails break out of the loop
      for (unsigned int i = 0; i < c_num_tests; i++)
        if (!(*tests[i]) (display, screen, root, &context, &vinfos, &results))
          break;

      if (results.compiz == 0) {
        // Unity compatibility checks.
        if ((results.major >= 2 || (results.major == 1 && results.minor >= 4)) &&
            (results.flags & FLAG_GL_OES_EGL_IMAGE) &&
            (results.flags & FLAG_EGL_KHR_IMAGE_PIXMAP) &&
            (~results.flags & FLAG_SOFTWARE_RENDERING) &&
            (~results.flags & FLAG_BLACKLISTED)) {
          results.result = 0;
        } else {
          results.result = 1;
        }
      } else {
        // Compiz compatibility checks.
        if ((results.flags & FLAG_GL_OES_EGL_IMAGE) &&
            (results.flags & FLAG_EGL_KHR_IMAGE_PIXMAP) &&
            (~results.flags & FLAG_SOFTWARE_RENDERING) &&
            (~results.flags & FLAG_BLACKLISTED)) {
          results.result = 0;
        } else {
          results.result = 1;
        }
      }
    }
#endif
  }

  if (print == 1) {
    print_report (results.vendor,  results.renderer,
		  results.version, results.major,
		  results.minor,   results.flags,
		  results.compiz,  results.result, results.error);
  }

  if (vinfos != NULL)
    XFree (vinfos);
#ifndef NUX_OPENGLES_20
  if (context != NULL)
    glXDestroyContext (display, context);
#else
  if (context == EGL_NO_CONTEXT)
    eglDestroyContext (egl_dpy, context);
  if (egl_dpy)
    eglTerminate (egl_dpy);
#endif
  if (display != NULL)
    XCloseDisplay (display);
  if (results.error != NULL)
    free (results.error);

  // drop result file
  if (results.result != 5) {
    snprintf(resultfilename, sizeof(resultfilename), "/tmp/unity_support_test.%i", results.result);
    resultfile = open(resultfilename, O_CREAT|O_WRONLY|O_EXCL, 0666);
    if (resultfile > 0)
      close(resultfile);
  }

  return results.result;
}
