/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef DBM_FONTMGR
#define DBM_FONTMGR

#if defined WIN32
#include <windows.h>	// Header File For Windows
#include <vector>
#include <list>
#include <algorithm>
//	#include <gl\gl.h>	// Header File For The OpenGL32 Library
//	#include <gl\glu.h>	// Header File For The GLu32 Library
//	#include <gl\glaux.h>
using namespace std;
#else
#include <vector.h>
#include <algo.h>
//	#include <GL/gl.h>
//	#include <GL/glx.h>
//	#include <GL/glu.h>
#endif



/*#include "fmt_image.h"
#include "fmt_tga.h"
#include "fmt_bmp.h"
#include "dbm_texturemgr.h"*/

#include <ft2build.h>
#include <freetype/fttypes.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include FT_FREETYPE_H


typedef int FontID;

typedef enum _TextAlignment
{
  eAlignTextNone       = 0,
  eAlignTextLeft       = 1,
  eAlignTextRight      = 2,
  eAlignTextCenter     = 3,

} TextAlignment;

class StringBBox
{
public:
  StringBBox()
  {
    x = 0;
    y = 0;
    width = 0;
    height = 0;
    ybearing = 0;
    downline = 0;
  };
  ~StringBBox() {};

  int x;
  int y;
  int width;
  int height;
  int ybearing; // max ybearing of the string
  int downline; // max downline of the string (max space below the baseline)
};

class PageBBox
{
public:
  PageBBox()
  {
    xmin = 0;
    ymin = 0;
    xmax = 0;
    ymax = 0;
    x_margin = 0;
    y_margin = 0;
  };
  ~PageBBox() {};
  int xmin;
  int ymin;
  int xmax;
  int ymax;
  int x_margin;
  int y_margin;
};

typedef struct _CharData
{
  int hbearingX;
  int hbearingY;
  int vbearingX;
  int vbearingY;
  int vadvance;
  int hadvance;
  int gwidth;
  int gheight;

  struct
  {
    int xmin, xmax, ymin, ymax;
  } bbox;

  int bitmap_left_bearing;
  int bitmap_top_bearing;
  int bitmap_width;
  int bitmap_height;

  int glyph_index;
  int gltexid;
  int glTexWidth;
  int glTexHeight;

} CharData;

typedef struct _FontStyle
{
  char *style_name;
  CharData *charmap;
  int num_glyph;
  int ascent;
  int descent;
  int texture_width;
  int texture_height;
  int line_spacing;
} FontStyle;

typedef struct _FontFamily
{
  char *font_name;
  FT_Face m_face;
  //int num_glyph;
  list <FontStyle *> *style_list;
} FontFamily;

class FontMgr
{
private:
  char *current_font;
  char *current_style;

  FontMgr();
  static FontMgr *pinstance;
public:
  list <FontFamily *> *m_font_list;

  static FontMgr *Instance();

  int nombre;

  ~FontMgr();

  bool load_font (const char *font_name);
  int get_character_texture_id (const char *font_name, int encoding, CharData *cd);
  int get_character_texture_id (int encoding, CharData *cd);
  void get_style_texture_size (const char *font_name, int *width, int *height);
  void get_style_texture_size (int *width, int *height);
  void get_style_line_spacing (const char *font_name, int *line_spacing);
  void get_style_line_spacing (int *line_spacing);
  unsigned int get_null_character_texture_id (const char *font_name);
  unsigned int get_null_character_texture_id();
  bool find_font (const char *font_name, const char *style_name);

  FontFamily *get_font_family (const char *font_name);
  FontStyle *get_font_style (char *font_name, char *style_name);

  void set_current_font (char *font_name, char *style_name);
  void draw_text (int rect_xmin, int rect_ymin, int rect_xmax, int rect_ymax, const char *text, const char *sh_name);

  void ComputeGlyphString (int x, int y, const char *text);
  //unsigned int GetStringWidth(const char* text);
  //unsigned int GetStringHeight(const char* text);
  unsigned int GetStringWidth (const std::string &text);
  unsigned int GetStringHeight (const std::string &text);
  void ComputeStringBBox (StringBBox &stringBBox, const char *text);
  void PositionString (const PageBBox &, StringBBox &, TextAlignment align = eAlignTextCenter);

  //void RenderGlyph(int targer_width, int target_height);
  void RenderGlyph (const PageBBox &pageSize, const char *text, TextAlignment align = eAlignTextCenter);
  void RenderString (const PageBBox &pageSize, const std::string &Str, TextAlignment alignment,  unsigned int CursorPosition, int offset = 0);
  void RenderTextLineEdit (const PageBBox &pageSize, const std::string &Str, unsigned int CursorPosition, int offset = 0);
  void RenderTextLineStatic (const PageBBox &pageSize, const std::string &Str, TextAlignment alignment, char trailingchar = 0);

};


#endif
