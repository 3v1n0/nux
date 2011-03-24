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
// > --cflags --libs gl x11 libpci`

#include <pci/pci.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#undef GLX_GLXEXT_PROTOTYPES
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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
  unsigned short id;
};

// Blacklists of GPUs for Compiz.
struct PciDevice gpu_blacklist[] = {
  { 0x8086, 0x3577 },  // Intel : 82830M/MG
  { 0x8086, 0x2562 }   // Intel : 82845G aka Poulsbo
};

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
      fprintf (stdout,
               "GL vertex program:        %s\n"
               "GL fragment program:      %s\n"
               "GL vertex buffer object:  %s\n"
               "GL framebuffer object:    %s\n"
               "GL version is 1.4+:       %s\n"
               "\n"
               "Unity supported:          %s\n",
               flags & FLAG_GL_ARB_VERTEX_PROGRAM ? yes : no,
               flags & FLAG_GL_ARB_FRAGMENT_PROGRAM ? yes : no,
               flags & FLAG_GL_ARB_VERTEX_BUFFER_OBJECT ? yes : no,
               flags & MASK_GL_FRAMEBUFFER_OBJECT ? yes : no,
               (major >= 2 || (major == 1 && minor >= 4)) ? yes : no,
               result == 0 ? yes : no);
    } else {
      fprintf (stdout, "\nCompiz supported:         %s\n",
               result == 0 ? yes : no);
    }
  } else {
    fprintf (stdout, "Error: %s\n", error);
  }
}

int main (int argc, char* argv[]) {
  int indirect = 0, print = 0, compiz = 0;
  char* error = NULL;
  unsigned int flags = 0;
  char* display_name = NULL;
  int screen;
  Window root;
  XVisualInfo *vinfos;
  Display* display;
  GLXContext context;
  char *vendor, *renderer, *version;
  int result, major, minor;

  // Basic command-line parsing.
  for (int i = 1; i < argc; i++) {
    if (((strncmp (argv[i], "-d", 2) == 0) ||
         (strncmp (argv[i], "--display", 9) == 0)) &&
        (i + 1 < argc)) {
      display_name = argv[i + 1];
      i++;
    } else if ((strncmp (argv[i], "-i", 2) == 0) ||
               (strncmp (argv[i], "--indirect", 10) == 0)) {
      indirect = 1;
    } else if ((strncmp (argv[i], "-p", 2) == 0) ||
               (strncmp (argv[i], "--print", 7) == 0)) {
      print = 1;
    } else if ((strncmp (argv[i], "-c", 2) == 0) ||
               (strncmp (argv[i], "--compiz", 8) == 0)) {
      compiz = 1;
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

  // Open a X11 connection and get the root window.
  display = XOpenDisplay (display_name);
  if (display == NULL) {
    error = "unable to open display";
    result = 1;
    goto abort;
  }
  screen = DefaultScreen (display);
  root = XRootWindow (display, screen);

  // Retrieve root window visual.
  XWindowAttributes attr;
  XVisualInfo vinfo_template;
  int nr_vinfos;
  if (XGetWindowAttributes (display, root, &attr) == 0) {
    error = "unable to get root window attributes";
    result = 1;
    goto abort;
  }
  vinfo_template.visualid = XVisualIDFromVisual (attr.visual);
  vinfos = XGetVisualInfo (display, VisualIDMask, &vinfo_template, &nr_vinfos);
  if (nr_vinfos == 0) {
    error = "unable to get visual informations for default visual";
    result = 1;
    goto abort;
  }

  // Check root window visual capabilities.
  int value;
  glXGetConfig (display, vinfos, GLX_USE_GL, &value);
  if (value == 0) {
    error = "OpenGL rendering is not supported by the root visual";
    result = 1;
    goto abort;
  }
  glXGetConfig (display, vinfos, GLX_DOUBLEBUFFER, &value);
  if (value == 0) {
    error = "color buffers of the root visual are not double-buffered";
    result = 1;
    goto abort;
  }

  // Create and map the OpenGL context to the root window and get the strings.
  context = glXCreateContext (display, vinfos, NULL, !indirect);
  if (context == NULL) {
    error = "unable to create the OpenGL context";
    result = 1;
    goto abort;
  }
  glXMakeCurrent (display, root, context);
  vendor = (char*) glGetString (GL_VENDOR);
  renderer = (char*) glGetString (GL_RENDERER);
  version = (char*) glGetString (GL_VERSION);

  // Fill flags with the supported GLX extensions.
  const char* glx_extensions = glXQueryExtensionsString (display, screen);
  const struct { const char* name; const unsigned int flag; } glx_extension[] = {
    { "GLX_SGIX_fbconfig", FLAG_GLX_SGIX_FBCONFIG },
    { "GLX_EXT_texture_from_pixmap", FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP },
    { NULL, 0 }
  };
  for (int i = 0; glx_extension[i].name != NULL; i++)
    if (is_extension_supported (glx_extensions, glx_extension[i].name) == 1)
      flags |= glx_extension[i].flag;
  if (flags & FLAG_GLX_SGIX_FBCONFIG) {
    if (glXGetProcAddressARB ("glXQueryDrawable") == NULL ||
        glXGetProcAddressARB ("glXGetFBConfigs") == NULL ||
        glXGetProcAddressARB ("glXGetFBConfigAttrib") == NULL ||
        glXGetProcAddressARB ("glXCreatePixmap") == NULL ||
        glXGetProcAddressARB ("glXDestroyPixmap") == NULL) {
      flags &= ~FLAG_GLX_SGIX_FBCONFIG;
    }
  }
  if (flags & FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP) {
    if (glXGetProcAddressARB ("glXBindTexImageEXT") == NULL ||
        glXGetProcAddressARB ("glXReleaseTexImageEXT") == NULL) {
      flags &= ~FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP;
    }
  }

  // Fill flags with the supported OpenGL extensions.
  const char* gl_extensions = glGetString (GL_EXTENSIONS);
  if (gl_extensions == NULL) {
    error = "invalid OpenGL extensions string";
    result = 1;
    goto abort;
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
    { NULL, 0 }
  };
  for (int i = 0; gl_extension[i].name != NULL; i++)
    if (is_extension_supported (gl_extensions, gl_extension[i].name) == 1)
      flags |= gl_extension[i].flag;

  // Check for software rendering.
  if (renderer != NULL &&
      (strncmp (renderer, "Software Rasterizer", 19) == 0 ||
       strncmp (renderer, "Mesa X11", 8) == 0)) {
    flags |= FLAG_SOFTWARE_RENDERING;
  }

  // FIXME(loicm): Compiz does a last check to test whether there's a fbconfig
  //     available for the default depth or not.

  // Scan the PCI devices searching for blacklisted GPUs.
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
          dev->device_id == gpu_blacklist[i].id) {
        flags |= FLAG_BLACKLISTED;
      }
    }
    dev = dev->next;
  }
  pci_cleanup (access);

  if (compiz == 0) {
    // Unity compatibility checks.
    get_opengl_version (version, &major, &minor);
    if ((major >= 2 || (major == 1 && minor >= 4)) &&
        (flags & FLAG_GLX_SGIX_FBCONFIG) &&
        (flags & FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP) &&
        (flags & MASK_GL_NON_POWER_OF_TWO) &&
        (flags & FLAG_GL_ARB_VERTEX_PROGRAM) &&
        (flags & FLAG_GL_ARB_FRAGMENT_PROGRAM) &&
        (flags & FLAG_GL_ARB_VERTEX_BUFFER_OBJECT) &&
        (flags & MASK_GL_FRAMEBUFFER_OBJECT) &&
        (~flags & FLAG_SOFTWARE_RENDERING) &&
        (~flags & FLAG_BLACKLISTED)) {
      result = 0;
    } else {
      result = 1;
    }
  } else {
    // Compiz compatibility checks.
    if ((flags & FLAG_GLX_SGIX_FBCONFIG) &&
        (flags & FLAG_GLX_EXT_TEXTURE_FROM_PIXMAP) &&
        (flags & MASK_GL_NON_POWER_OF_TWO) &&
        (~flags & FLAG_SOFTWARE_RENDERING) &&
        (~flags & FLAG_BLACKLISTED)) {
      result = 0;
    } else {
      result = 1;
    }
  }

abort:

  if (print == 1) {
    print_report (vendor, renderer, version, major, minor, flags, compiz,
                  result, error);
  }

  if (vinfos != NULL)
    XFree (vinfos);
  if (context != NULL)
    glXDestroyContext (display, context);
  if (display != NULL)
    XCloseDisplay (display);

  return result;
}
