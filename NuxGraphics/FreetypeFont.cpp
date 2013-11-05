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


#include "FreetypeFont.h"
#include "agg_rendering_buffer.h"
#include "GLTextureManager.h"

//#include "OpenGL/GLsdk/GL/gl.h"
//#include "OpenGL/GLsdk/GL/glprocs.h"
//#include <GL/glu.h>
#include <GL/glew.h>
#include "GfxServer.h"
#include "OpenGLEngine/GLCheck.h"
#include "OpenGLEngine/OpenGLEngine.h"

FontMgr *FontMgr::pinstance = 0;

int   Dot_GlyphIndex;

void GLDrawRectangle (int xmin, int ymin, int xmax, int ymax)
{
  glColor3f (1.0f, 1.0f, 1.0f);
  glDisable (GL_TEXTURE_2D);
  glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
  glBegin (GL_QUADS);
  glVertex3i (xmin,  ymin, 1);
  glVertex3i (xmax,  ymin, 1);
  glVertex3i (xmax,  ymax, 1);
  glVertex3i (xmin,  ymax, 1);
  glEnd();
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  glEnable (GL_TEXTURE_2D);
}

FontMgr::FontMgr()
{
  m_font_list = new list <FontFamily *>;
  current_font = NULL;
  current_style = NULL;
}

FontMgr *FontMgr::Instance()
{
  if (pinstance == 0)
    pinstance = new FontMgr();

  return pinstance;
}

bool FontMgr::load_font (const char *font_filename)
{
  FT_Library   library;   // handle to library
  FT_Face      face;      // handle to face object
  FT_Error     error;

  error = FT_Init_FreeType ( &library );

  if (error )
  {
    return false;
  }

  error = FT_New_Face ( library,
                        font_filename,
                        0,
                        &face );

  if ( error == FT_Err_Unknown_File_Format )
  {
    // the font file could be opened and read, but it appears
    // that its font format is unsupported
  }
  else if ( error )
  {
    // another error code means that the font file could not;
    // be opened or read, or simply that it is broken...;

    return false;
  }

  int font_bitmap_width, font_bitmap_height;
  font_bitmap_width = 0;
  font_bitmap_height = 18;

  error = FT_Set_Pixel_Sizes (
            face,                   // handle to face object
            font_bitmap_width,                      // pixel_width
            font_bitmap_height );   // pixel_height

  if (error)
  {
    return false;
  }


  FontStyle *fs = new FontStyle();
  FontFamily *ff = new FontFamily();

  ff->font_name = strdup (face->family_name);
  ff->m_face = face;
  ff->style_list = new list <FontStyle *>;

  fs->style_name = strdup (face->style_name);

  set_current_font (face->family_name, face->style_name);

  // check if the font is already loaded
  if (find_font ((const char*)face->family_name, (const char*)face->style_name) )
    return true;		// the font is already loaded

  fs->charmap = new CharData[face->num_glyphs];
  fs->ascent = face->ascender;
  fs->descent = face->descender;
  fs->texture_width  = font_bitmap_width;
  fs->texture_height = font_bitmap_height;
  fs->line_spacing = face->ascender >> 6;
  fs->num_glyph = face->num_glyphs;

  int char_num;
  unsigned char *imgp, *imp;

  for (char_num = 0; char_num < face->num_glyphs; char_num++)
  {
    FT_UInt glyph_index = FT_Get_Char_Index (face, char_num);
    error = FT_Load_Glyph (face, glyph_index, FT_LOAD_DEFAULT);

    if (error) continue;

    error = FT_Render_Glyph (face->glyph, ft_render_mode_normal);

    if (error) continue;

    /*error = FT_Load_Char( face, char_num, FT_LOAD_RENDER );*/

    unsigned int PO2w = NextPowerOfTwo (face->glyph->bitmap.width);
    unsigned int PO2h = NextPowerOfTwo (face->glyph->bitmap.rows);

    if ( ( (glyph_index == 0) && (char_num == 0) ) || (glyph_index != 0) )
    {
      imgp = (unsigned char *) calloc (PO2h * PO2w * 4, sizeof (unsigned char) );
      imp = imgp;

      //cout << "rows = " << face->glyph->bitmap.rows << endl;
      //cout << "width = " << face->glyph->bitmap.width << endl;
      //cout << "pitch = " << face->glyph->bitmap.pitch << endl;

      int i, j;
      int position = 0;

      for ( j = 0; j < face->glyph->bitmap.rows; j++)
      {
        for ( i = 0; i < face->glyph->bitmap.width; i++)
        {
          //if(imp < imgp + font_bitmap_height*font_bitmap_width*4) // sometimes a char has more rows than the texture
          {
            * (imp + 4 * i + 0) = * (face->glyph->bitmap.buffer + j * face->glyph->bitmap.pitch + i);
            // *(imp + 4*i+1) = *(face->glyph->bitmap.buffer + j*face->glyph->bitmap.pitch +i);
            // *(imp + 4*i+2) = *(face->glyph->bitmap.buffer + j*face->glyph->bitmap.pitch +i);

            /* if(*(imp + 4*i+0) == 0)
                        *(imp + 4*i+3) = 0x0;

            if(*(imp + 4*i+0) == 255)
              *(imp + 4*i+3) = 0xff;
            else
              *(imp + 4*i+3) = *(imp + 4*i+0); */

            //*(imp + 4*i+0) = 0;
            * (imp + 4 * i + 1) = * (imp + 4 * i + 0);
            * (imp + 4 * i + 2) = * (imp + 4 * i + 0);
            * (imp + 4 * i + 3) = * (imp + 4 * i + 0);
          }
        }

        //imp += 4*font_bitmap_width;
        imp += 4 * PO2w;
        // position += 4*font_bitmap_width;
      }

      //ImageFormat *image = new ImageFormat();
      //image->allocate(BITFMT_GRAY8, font_bitmap_width, font_bitmap_height);

      /*image->width = font_bitmap_width;
      image->Height() = font_bitmap_height;
      image->bpp = 32;
	        image->data = imgp;*/

      //unsigned char* buffer = new unsigned char[font_bitmap_width * font_bitmap_height];
      //agg::rendering_buffer rbuf(buffer, font_bitmap_width, font_bitmap_height, font_bitmap_height*4);

      //TextureMgr* texturemgr = TextureMgr::Instance();

      GLuint texname;
      glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
      CHECKGL(glGenTextures (1, &texname));
      glBindTexture (GL_TEXTURE_2D, texname);

      /*	float *border_color = new float[4];
      *(border_color) = 0;
      *(border_color+1) = 1.0f;
      *(border_color+2) = 0;
      *(border_color+3) = 1.0f;
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
      glTexEnvfv(GL_TEXTURE_2D, GL_TEXTURE_ENV_COLOR, border_color);
      */
      fs->charmap[char_num].gltexid = texname;
      fs->charmap[char_num].glTexWidth = PO2w;
      fs->charmap[char_num].glTexHeight = PO2h;
      glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, PO2w, PO2h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgp);

      // fs->charmap[char_num].gltexid = texturemgr->load_image(image);
    }
    else
      fs->charmap[char_num].gltexid = fs->charmap[0].gltexid;  // the missing glyph

    fs->charmap[char_num].hadvance  = face->glyph->metrics.horiAdvance >> 6; // 1/64th of a pixel
    fs->charmap[char_num].vadvance  = face->glyph->metrics.vertAdvance >> 6;
    fs->charmap[char_num].hbearingX = face->glyph->metrics.horiBearingX >> 6;
    fs->charmap[char_num].hbearingY = face->glyph->metrics.horiBearingY >> 6;
    fs->charmap[char_num].vbearingX = face->glyph->metrics.vertBearingX >> 6;
    fs->charmap[char_num].vbearingY = face->glyph->metrics.vertBearingY >> 6;
    fs->charmap[char_num].gwidth    = face->glyph->metrics.width >> 6;
    fs->charmap[char_num].gheight   = face->glyph->metrics.height >> 6;

    fs->charmap[char_num].bitmap_left_bearing = face->glyph->bitmap_left;
    fs->charmap[char_num].bitmap_top_bearing = face->glyph->bitmap_top;
    fs->charmap[char_num].bitmap_width = face->glyph->bitmap.width;
    fs->charmap[char_num].bitmap_height = face->glyph->bitmap.rows;
    fs->charmap[char_num].glyph_index = glyph_index;

  }



  ff->style_list->push_back (fs);
  m_font_list->push_back (ff);
  return true;
}

int FontMgr::get_character_texture_id (const char *font_name, int encoding, CharData *cd)
{
  list <FontFamily *>::iterator iter;

  for (iter = m_font_list->begin(); iter != m_font_list->end(); ++iter)
  {
    if (strcmp ( (*iter)->font_name, font_name) == 0)
    {
      cd->bitmap_width = (* (*iter)->style_list->begin() )->charmap[encoding].bitmap_width;
      cd->bitmap_height = (* (*iter)->style_list->begin() )->charmap[encoding].bitmap_height;
      cd->bitmap_left_bearing = (* (*iter)->style_list->begin() )->charmap[encoding].bitmap_left_bearing;
      cd->bitmap_top_bearing = (* (*iter)->style_list->begin() )->charmap[encoding].bitmap_top_bearing;
      cd->hadvance  = (* (*iter)->style_list->begin() )->charmap[encoding].hadvance;

      cd->glTexWidth  = (* (*iter)->style_list->begin() )->charmap[encoding].glTexWidth;
      cd->glTexHeight  = (* (*iter)->style_list->begin() )->charmap[encoding].glTexHeight;


      return ( (* (*iter)->style_list->begin() )->charmap[encoding].gltexid);
    }
  }

  return 0;
}

int FontMgr::get_character_texture_id (int encoding, CharData *cd)
{
  FontStyle *fs;
  fs = get_font_style (current_font, current_style);

  if (fs != NULL)
  {
    cd->bitmap_width = fs->charmap[encoding].bitmap_width;
    cd->bitmap_height = fs->charmap[encoding].bitmap_height;
    cd->bitmap_left_bearing = fs->charmap[encoding].bitmap_left_bearing;
    cd->bitmap_top_bearing = fs->charmap[encoding].bitmap_top_bearing;
    cd->hadvance  = fs->charmap[encoding].hadvance;
    cd->gwidth = fs->charmap[encoding].gwidth;
    cd->gheight = fs->charmap[encoding].gheight;

    cd->glTexWidth  = fs->charmap[encoding].glTexWidth;
    cd->glTexHeight  = fs->charmap[encoding].glTexHeight;

    return fs->charmap[encoding].gltexid;
  }

  return 0;
}

void FontMgr::get_style_texture_size (const char *font_name, int *width, int *height)
{
  list <FontFamily *>::iterator iter;

  for (iter = m_font_list->begin(); iter != m_font_list->end(); ++iter)
  {
    if (strcmp ( (*iter)->font_name, font_name) == 0)
    {
      *width = (* (*iter)->style_list->begin() )->texture_width;
      *height = (* (*iter)->style_list->begin() )->texture_height;
      return;
    }
  }
}

void FontMgr::get_style_texture_size (int *width, int *height)
{
  FontStyle *fs;
  fs = get_font_style (current_font, current_style);

  if (fs != NULL)
  {
    *width = fs->texture_width;
    *height = fs->texture_height;
    return;
  }
}

void FontMgr::get_style_line_spacing (const char *font_name, int *line_spacing)
{
  list <FontFamily *>::iterator iter;

  for (iter = m_font_list->begin(); iter != m_font_list->end(); ++iter)
  {
    if (strcmp ( (*iter)->font_name, font_name) == 0)
    {
      *line_spacing = (* (*iter)->style_list->begin() )->line_spacing;
    }
  }
}

void FontMgr::get_style_line_spacing (int *line_spacing)
{
  FontStyle *fs;
  fs = get_font_style (current_font, current_style);

  if (fs != NULL)
  {
    *line_spacing = fs->line_spacing;
    return;
  }
}

unsigned int FontMgr::get_null_character_texture_id (const char *font_name)
{
  list <FontFamily *>::iterator iter;

  for (iter = m_font_list->begin(); iter != m_font_list->end(); ++iter)
  {
    if (strcmp ( (*iter)->font_name, font_name) == 0)
    {
      return ( (* (*iter)->style_list->begin() )->charmap[0].gltexid);
    }
  }

  return 0;
}

unsigned int FontMgr::get_null_character_texture_id()
{
  FontStyle *fs;
  fs = get_font_style (current_font, current_style);

  if (fs != NULL)
  {
    return fs->charmap[0].gltexid;
  }

  return 0;
}

bool FontMgr::find_font (const char *font_name, const char *style_name)
{
  list <FontFamily *>::iterator iter;

  for (iter = m_font_list->begin(); iter != m_font_list->end(); ++iter)
  {
    if (strcmp ( (*iter)->font_name, font_name) == 0)
    {
      return true;
    }
  }

  return false;
}

FontFamily *FontMgr::get_font_family (const char *font_name)
{
  list <FontFamily *>::iterator iter;

  for (iter = m_font_list->begin(); iter != m_font_list->end(); ++iter)
  {
    if (strcmp ( (*iter)->font_name, font_name) == 0)
    {
      return (*iter);
    }
  }

  return NULL;
}

FontStyle *FontMgr::get_font_style (char *font_name, char *style_name)
{
  list <FontFamily *>::iterator iter;
  list <FontStyle *>::iterator iter1;

  for (iter = m_font_list->begin(); iter != m_font_list->end(); ++iter)
  {
    if (strcmp ( (*iter)->font_name, font_name) == 0)
    {
      for (iter1 = ( (*iter)->style_list)->begin(); iter1 != ( (*iter)->style_list)->end(); ++iter1)
        if (strcmp ( (*iter1)->style_name, style_name) == 0)
        {
          return (*iter1);
        }
    }
  }

  return NULL;
}

void FontMgr::set_current_font (char *font_name, char *style_name)
{
  if (current_font != NULL)
    free ( current_font);

  if (current_style != NULL)
    free (current_style);

  current_font = strdup (font_name);
  current_style = strdup (style_name);

}

FontMgr::~FontMgr()
{


}

void enable_drawing_in_restricted_area (int xmin, int ymin, int xmax, int ymax)
{
  glScissor (xmin, ymax, xmax - xmin, ymax - ymin);
  glEnable (GL_SCISSOR_TEST);
}

void disable_drawing_in_restricted_area()
{
  glDisable (GL_SCISSOR_TEST);
}


void FontMgr::draw_text (int rect_xmin, int rect_ymin, int rect_xmax, int rect_ymax, const char *text, const char *sh_name)
{
  int num_cararcter;
  int width, height, bitmap_x, bitmap_y;
  unsigned int ascii_code;
  int line_spacing;
  int x_cursor_position;
  int y_cursor_position;

  if ( (rect_xmin == 0) && (rect_xmax == 0) && (rect_ymin == 0) && (rect_ymax == 0) ) // the drawing area is invalid
    return;

//    if(num_cararcter == 0)
  //      return;  // NULL string; return

  //enable_drawing_in_restricted_area(rect_xmin, rect_ymin, rect_xmax, rect_ymax); // enable scissoring // FIXME (correct with the size of the Window 480-ymin, 480-ymax)

  glColor3f (1.0f, 1.0f, 1.0f);
  glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
  glBegin (GL_QUADS);
  glVertex3i (rect_xmin,  rect_ymin, 1);
  glVertex3i (rect_xmax,  rect_ymin, 1);
  glVertex3i (rect_xmax,  rect_ymax, 1);
  glVertex3i (rect_xmin,  rect_ymax, 1);
  glEnd();
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);


  get_style_texture_size (&width, &height);
  get_style_line_spacing (&line_spacing);

  x_cursor_position = rect_xmin;
  y_cursor_position = rect_ymin + line_spacing;

  num_cararcter = strlen (text);

  glEnable (GL_TEXTURE_2D);

  for (int i = 0; i < num_cararcter; i++)
  {
    ascii_code = (unsigned int) text[i];

    CharData cd;
    int texture_id = get_character_texture_id (ascii_code, &cd);
    glBindTexture (GL_TEXTURE_2D, texture_id /*TextureMgr::Instance()->glid(texture_id)*/);

    if (ascii_code == 13) // return caractere
    {
      x_cursor_position = rect_xmin;
      y_cursor_position += height + 2; // line_spacing;
    }

    if (texture_id != get_null_character_texture_id() )
    {
      glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      if (x_cursor_position + cd.bitmap_left_bearing + cd.gwidth > rect_xmax - 5) // 5 pixel margin at the end of the line
      {
        x_cursor_position = rect_xmin;
        y_cursor_position += height + 2; // line_spacing;
      }

      bitmap_x = x_cursor_position + cd.bitmap_left_bearing;
      bitmap_y = y_cursor_position - cd.bitmap_top_bearing;

      glColor3f (1.0f, 0.0f, 1.0f);
      glBegin (GL_QUADS);
      glTexCoord2f (0.0f, 0.0f);
      glVertex3i (bitmap_x,  bitmap_y, 1);
      glTexCoord2f (0.0f, 1.0f);
      glVertex3i (bitmap_x,  bitmap_y + height, 1);
      glTexCoord2f (1.0f, 1.0f);
      glVertex3i (bitmap_x + width,  bitmap_y + height, 1);
      glTexCoord2f (1.0f, 0.0f);
      glVertex3i (bitmap_x + width,  bitmap_y, 1);
      glEnd();

      x_cursor_position += cd.hadvance;

      if (x_cursor_position > rect_xmax)
      {
        x_cursor_position = rect_xmin;
        y_cursor_position += height + 2; // line_spacing;
      }
    }
  }

  glDisable (GL_TEXTURE_2D);
  //disable_drawing_in_restricted_area(); // disable scissoring
}



// We start by packing the information related to a single glyph image into a single structure instead of parallel arrays.
// We thus define the following structure type:

typedef struct  TGlyph_
{
  FT_UInt    index;  // glyph index
  FT_Vector  pos;    // glyph origin on the baseline
  FT_Glyph   glyph;  // glyph image

} TGlyphStruct, *PGlyphStruct;

TGlyphStruct    gGlyphs[1000];  // glyphs table
int             gNumGlyphs = 0;
// We also translate each glyph image directly after it is loaded to its position on the baseline at load time.
// As we will see, this as several advantages. Our glyph sequence loader thus becomes:

void FontMgr::ComputeGlyphString (int x, int y, const char *text)
{
  //FT_GlyphSlot  slot = face->glyph;  // a small shortcut
  FT_Bool       use_kerning;
  FT_UInt       previous;
  int           pen_x, pen_y, n;
  PGlyphStruct        ptrGlyph;               // current glyph in table
  FT_Error     error;

  int num_chars = strlen (text);

  //... create face object ...
  FT_Face      face = get_font_family (current_font)->m_face;

  pen_x = x;   // start at (0,0)
  pen_y = y;

  gNumGlyphs  = 0;
  use_kerning = FT_HAS_KERNING ( face );
  previous    = 0;

  ptrGlyph = gGlyphs;

  for ( n = 0; n < num_chars; n++ )
  {
    //ptrGlyph->index = FT_Get_Char_Index( face, text[n] );
    ptrGlyph->index = get_font_style (current_font, current_style)->charmap[text[n]].glyph_index;

    if ( use_kerning && previous && ptrGlyph->index )
    {
      FT_Vector  delta;
      FT_Get_Kerning ( face, previous, ptrGlyph->index, FT_KERNING_DEFAULT, &delta );

      pen_x += delta.x >> 6;
    }

    // store current pen position
    ptrGlyph->pos.x = pen_x;
    ptrGlyph->pos.y = pen_y;

//        error = FT_Load_Glyph(face, ptrGlyph->index, FT_LOAD_NO_BITMAP);
//        if ( error ) continue;
//        error = FT_Get_Glyph(face->glyph, &ptrGlyph->glyph);
//        if ( error ) continue;

    pen_x   += get_font_style (current_font, current_style)->charmap[text[n]].hadvance;
    previous = ptrGlyph->index;

    // discard glyph image (bitmap or not)
    FT_Done_Glyph ( gGlyphs[n].glyph );
    // increment number of glyphs
    ptrGlyph++;
  }

  // count number of glyphs loaded
  gNumGlyphs = ptrGlyph - gGlyphs;
}

unsigned int FontMgr::GetStringWidth (const std::string &text)
{
  StringBBox stringBBox;
  ComputeStringBBox (stringBBox, text.c_str() );
  return stringBBox.width;
}

unsigned int FontMgr::GetStringHeight (const std::string &text)
{
  StringBBox stringBBox;
  ComputeStringBBox (stringBBox, text.c_str() );
  return stringBBox.height;
}

void  FontMgr::ComputeStringBBox (StringBBox &stringBBox, const char *text)
{
  int numCharacter = strlen (text);
  FT_Face face = get_font_family (current_font)->m_face;
  FT_Bool use_kerning = FT_HAS_KERNING ( face );
  int current_glyph_index = 0;
  int previous_glyph_index = 0;

  int width = 0;
  int max_ybearing = 0;
  int max_height = 0;
  int max_downline = 0;


  // for each glyph image, compute its bounding box,
  // translate it, and grow the string bbox
  for ( int n = 0; n < numCharacter; n++ )
  {
    // set the character were interested in
    CharData cd = get_font_style (current_font, current_style)->charmap[text[n]];
    current_glyph_index = cd.glyph_index;

    if (use_kerning && previous_glyph_index && cd.glyph_index)
    {
      FT_Vector delta;
      FT_Get_Kerning ( face, previous_glyph_index, current_glyph_index, FT_KERNING_DEFAULT, &delta );

      width += delta.x >> 6;
    }

    width += cd.hadvance;

    if (cd.hbearingY > max_ybearing)
      max_ybearing = cd.hbearingY;

    if (cd.bitmap_height - cd.hbearingY > max_downline)
      max_downline = cd.bitmap_height - cd.hbearingY;

    if (cd.bitmap_height > max_height)
      max_height = cd.bitmap_height;

    previous_glyph_index = current_glyph_index;
  }

  stringBBox.x = 0;
  stringBBox.y = 0;
  stringBBox.width = width;
  stringBBox.height = max_ybearing + max_downline;
  stringBBox.ybearing = max_ybearing;
  stringBBox.downline = max_downline;
}

void FontMgr::PositionString (const PageBBox &pageBBox, StringBBox &strBBox, TextAlignment alignment)
{

  int x, y;
  int xmin, ymin, xmax, ymax;
  xmin = pageBBox.xmin + pageBBox.x_margin;
  xmax = pageBBox.xmax - pageBBox.x_margin;
  ymin = pageBBox.ymin + pageBBox.y_margin;
  ymax = pageBBox.ymax - pageBBox.y_margin;

  switch (alignment)
  {
    case eAlignTextCenter:
    default:
      x = xmin + ( (float) (xmax - xmin) - (float) (strBBox.width) ) / 2.0f;
      y = ymin + ( (float) (ymax - ymin) - (float) (strBBox.height) ) / 2.0f + strBBox.ybearing;
      break;

    case eAlignTextLeft:
      x = xmin;
      y = ymin + ( (float) (ymax - ymin) - (float) (strBBox.height) ) / 2.0f + strBBox.ybearing;
      break;

    case eAlignTextRight:
      x = xmin + ( (float) (xmax - xmin) - (float) (strBBox.width) );
      y = ymin + ( (float) (ymax - ymin) - (float) (strBBox.height) ) / 2.0f + strBBox.ybearing;
      break;
  }

  strBBox.x = x;
  strBBox.y = y;
}

void FontMgr::RenderGlyph (const PageBBox &pageSize, const char *text, TextAlignment alignment)
{
  FT_Vector  start;
  StringBBox stringBBox;

  // get bbox of original glyph sequence
  /*ComputeStringBBox(stringBBox, text);
  PositionString(pageSize, stringBBox, alignment);
  ComputeGlyphString(stringBBox.x, stringBBox.y, text);*/

  // compute string dimensions in integer pixels
  //int string_width  = stringBBox.width;
  //int string_height = stringBBox.height;

  // set up start position in 26.6 cartesian space
  //start.x = ( ( target_width  - string_width  ) / 2 ) * 64;
  //start.y = ( ( target_height - string_height ) / 2 ) * 64;

  float angle = 0;
  //FT_Matrix matrix;
  // set up transform (a rotation here)
  /*matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
  matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
  matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
  matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );*/

  glEnable (GL_TEXTURE_2D);
  int bmpX;
  int bmpY;
  int bmpWidth;
  int bmpHeight;
  float u;
  float v;

//    int window_height = GfxServer::get_server()->getWindowHeight();
//    glScissor(pageSize.xmin, window_height - pageSize.ymax,
//        pageSize.xmax - pageSize.xmin, pageSize.ymax - pageSize.ymin);
  gOpenGLEngine.PushClippingRectangle (inl::Rect (pageSize.xmin, pageSize.ymin, pageSize.xmax - pageSize.xmin, pageSize.ymax - pageSize.ymin) );
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for ( int n = 0; n < gNumGlyphs; n++ )
  {
    unsigned int ascii_code = (unsigned int) text[n];
    FT_Face      face = get_font_family (current_font)->m_face;

    int gltexid = get_font_style ((char*)face->family_name, (char*)face->style_name)->charmap[ascii_code].gltexid;
    CharData cd = get_font_style ((char*)face->family_name, (char*)face->style_name)->charmap[ascii_code];
    glBindTexture (GL_TEXTURE_2D, gltexid);

    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // Set text Color here
    glColor4f (0.0f, 0.0f, 0.0f, 1);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (gltexid != get_null_character_texture_id() )
    {
      bmpX = gGlyphs[n].pos.x + cd.bitmap_left_bearing;
      bmpY = gGlyphs[n].pos.y - cd.bitmap_top_bearing;
      bmpWidth = cd.bitmap_width;
      bmpHeight = cd.bitmap_height;

      if (cd.glTexWidth != 0 && cd.glTexHeight != 0)
      {
        u = float (cd.bitmap_width) / cd.glTexWidth;
        v = float (cd.bitmap_height) / cd.glTexHeight;
      }
      else
      {
        u = 1;
        v = 1;
      }


      glBegin (GL_QUADS);
      glMultiTexCoord2fARB (GL_TEXTURE0_ARB, 0.0f, 0.0f);
      glVertex3i (bmpX           , bmpY, 1);
      glMultiTexCoord2fARB (GL_TEXTURE0_ARB, 0.0f, v);
      glVertex3i (bmpX           , bmpY + bmpHeight, 1);
      glMultiTexCoord2fARB (GL_TEXTURE0_ARB, u, v);
      glVertex3i (bmpX + bmpWidth, bmpY + bmpHeight, 1);
      glMultiTexCoord2fARB (GL_TEXTURE0_ARB, u, 0.0f);
      glVertex3i (bmpX + bmpWidth, bmpY, 1);
      glEnd();
    }
  }

//    FT_Face      face = get_font_family(current_font)->m_face;
//    unsigned int ascii_code = (unsigned int) text[strlen(text)-1];
//    CharData last_cd = get_font_style(face->family_name, face->style_name)->charmap[ascii_code];
//    CharData dot_cd = get_font_style(face->family_name, face->style_name)->charmap['%'];
//    int dot_gltexid = get_font_style(face->family_name, face->style_name)->charmap['%'].gltexid;
//
//    if(dot_cd.glTexWidth != 0 && dot_cd.glTexHeight != 0)
//    {
//        // This first check made sure that the GL texture for '.' has been loaded.
//        // Position of first dot
//        int posX = gGlyphs[gNumGlyphs-1].pos.x + last_cd.hadvance;
//        int posY = gGlyphs[gNumGlyphs-1].pos.y;// - dot_cd.bitmap_top_bearing;
//
//        bmpWidth = dot_cd.bitmap_width;
//        bmpHeight = dot_cd.bitmap_height;
//        u = float(dot_cd.bitmap_width) / dot_cd.glTexWidth;
//        v = float(dot_cd.bitmap_height) / dot_cd.glTexHeight;
//
//        glBindTexture(GL_TEXTURE_2D, dot_gltexid);
//
//        int page_right_border_posX = pageSize.xmin + pageSize.xmax - pageSize.xmin;
//        while(posX + dot_cd.hadvance < page_right_border_posX)
//        {
//            bmpX = posX + dot_cd.bitmap_left_bearing;
//            bmpY = posY - dot_cd.bitmap_top_bearing;
//
//            glBegin(GL_QUADS);
//            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);   glVertex3i(bmpX           , bmpY, 1);
//            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, v);      glVertex3i(bmpX           , bmpY + bmpHeight, 1);
//            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u, v);         glVertex3i(bmpX + bmpWidth, bmpY + bmpHeight, 1);
//            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u, 0.0f);      glVertex3i(bmpX + bmpWidth, bmpY, 1);
//            glEnd();
//            posX += dot_cd.hadvance;
//        }
//    }
//    GLDrawRectangle(
//        stringBBox.x,
//        stringBBox.y - stringBBox.ybearing,
//        stringBBox.x + stringBBox.width,
//        stringBBox.y - stringBBox.downline);

  glDisable (GL_BLEND);
  glDisable (GL_TEXTURE_2D);
  gOpenGLEngine.PopClippingRectangle();
}

void FontMgr::RenderTextLineEdit (const PageBBox &pageSize, const std::string &Str, unsigned int CursorPosition, int offset)
{
  FT_Vector  start;
  StringBBox stringBBox;

  // get bbox of original glyph sequence
  ComputeStringBBox (stringBBox, Str.c_str() );
  PositionString (pageSize, stringBBox, eAlignTextLeft);
  ComputeGlyphString (stringBBox.x + offset, stringBBox.y, Str.c_str() );
  RenderGlyph (pageSize, Str.c_str(), eAlignTextLeft);

  // Render Cursor
  std::string temp = Str.substr (0, CursorPosition);
  int w = GetStringWidth (temp);
  int h = GetStringHeight (temp);

  glDisable (GL_TEXTURE_2D);
  glColor4f (1, 0, 0, 1);
  glBegin (GL_LINES);
  glVertex3i (pageSize.xmin + w + offset, pageSize.ymin, 1);
  glVertex3i (pageSize.xmin + w + offset, pageSize.ymax, 1);
  glEnd();
}

void FontMgr::RenderTextLineStatic (const PageBBox &pageSize, const std::string &Str, TextAlignment alignment, char trailingchar)
{
  return;
  FT_Vector  start;
  StringBBox stringBBox;

  // get bbox of original glyph sequence
  ComputeStringBBox (stringBBox, Str.c_str() );
  PositionString (pageSize, stringBBox, alignment);
  ComputeGlyphString (stringBBox.x, stringBBox.y, Str.c_str() );
  RenderGlyph (pageSize, Str.c_str(), alignment);

  if (trailingchar == 0)
    return;

  const char *text = Str.c_str();
  FT_Face      face = get_font_family (current_font)->m_face;
  unsigned int ascii_code = (unsigned int) text[strlen (text)-1];
  CharData last_cd = get_font_style ((char*)face->family_name, (char*)face->style_name)->charmap[ascii_code];
  CharData dot_cd = get_font_style ((char*)face->family_name, (char*)face->style_name)->charmap[trailingchar];
  int dot_gltexid = get_font_style ((char*)face->family_name, (char*)face->style_name)->charmap[trailingchar].gltexid;

  glEnable (GL_TEXTURE_2D);

  if (dot_cd.glTexWidth != 0 && dot_cd.glTexHeight != 0)
  {
    // This first check made sure that the GL texture for '.' has been loaded.
    // Position of first dot
    int posX = gGlyphs[gNumGlyphs-1].pos.x + last_cd.hadvance;
    int posY = gGlyphs[gNumGlyphs-1].pos.y; // - dot_cd.bitmap_top_bearing;

    int bmpWidth = dot_cd.bitmap_width;
    int bmpHeight = dot_cd.bitmap_height;
    float u = float (dot_cd.bitmap_width) / dot_cd.glTexWidth;
    float v = float (dot_cd.bitmap_height) / dot_cd.glTexHeight;

//        glBindTexture(GL_TEXTURE_2D, dot_gltexid);
//        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//        // Set text Color here
//        glColor4f(0.0f, 0.0f, 0.0f, 1);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glEnable(GL_BLEND);
//        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int page_right_border_posX = pageSize.xmin + pageSize.xmax - pageSize.xmin;

    while (posX + dot_cd.hadvance < page_right_border_posX)
    {
      int bmpX = posX + dot_cd.bitmap_left_bearing;
      int bmpY = posY - dot_cd.bitmap_top_bearing;

      glBegin (GL_QUADS);
      glMultiTexCoord2fARB (GL_TEXTURE0_ARB, 0.0f, 0.0f);
      glVertex3i (bmpX           , bmpY, 1);
      glMultiTexCoord2fARB (GL_TEXTURE0_ARB, 0.0f, v);
      glVertex3i (bmpX           , bmpY + bmpHeight, 1);
      glMultiTexCoord2fARB (GL_TEXTURE0_ARB, u, v);
      glVertex3i (bmpX + bmpWidth, bmpY + bmpHeight, 1);
      glMultiTexCoord2fARB (GL_TEXTURE0_ARB, u, 0.0f);
      glVertex3i (bmpX + bmpWidth, bmpY, 1);
      glEnd();

      posX += dot_cd.hadvance;
    }
  }

  glDisable (GL_BLEND);
  glDisable (GL_TEXTURE_2D);
}

