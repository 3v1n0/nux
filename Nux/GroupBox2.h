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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef GROUPBOX2_H
#define GROUPBOX2_H

namespace nux
{

  class Layout;

  /*!
      A stretch factor of 0 or 1 for the GroupBox has no effect because the GroupBox is designed to tightly adjust
      to the size of its composition layout. This unlike the Panel widget for instance who does not force itself to
      adjust to the size of its composition layout.
  */
  class GroupBox2 : public View
  {
  public:
    GroupBox2(const char *Caption = "", NUX_FILE_LINE_PROTO);
    ~GroupBox2();

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    virtual bool SetLayout(Layout *layout);
    void SetCaption(const char *Caption);
  
  protected:
    virtual bool AcceptKeyNavFocus();
  private:
    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement(long LayoutResult);
    virtual void ComputeContentPosition(float offsetX, float offsetY);

    BasicView   *m_CaptionArea;
    Layout     *m_layout;

    static int CAPTION_X_MARGIN;
    static int X_MARGIN;
    static int Y_MARGIN;

    static Color GROUPBOX2_HEADER_BASE_COLOR;
    static Color GROUPBOX2_HEADER_TEXT_COLOR;
    static int TOP_HEADER_HEIGHT;
  };
}

#endif // GroupBox2_H
