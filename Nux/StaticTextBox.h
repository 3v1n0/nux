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


#ifndef STATICTEXTBOX_H
#define STATICTEXTBOX_H

namespace nux
{

  class Validator;

  class StaticTextBox : public View //public InputArea
  {
    NUX_DECLARE_OBJECT_TYPE(StaticTextBox, View);
  public:
    StaticTextBox(const char *Caption, NUX_FILE_LINE_PROTO);
    ~StaticTextBox();
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    // API
    void SetText(const char *Caption);
    void SetText(const std::string &Caption);
    const char *GetText() const;
    unsigned int GetTextSize() const
    {
      return (unsigned int) m_Text.length();
    }

    //! Change the widget minimum width whenever the text is set.
    /*!
        Change the widget minimum width whenever the text is set.
        @param b If true, the widget minimum width is set to match the size of the text
    */
    void SetMinWidthMatchText(bool b);
    //! Return true if the widget with changes to match the text width.
    bool GetMinWidthMatchText() const;

    void SetTextColor(const Color &color)
    {
      m_TextColor = color;
    };
    Color GetTextColor() const
    {
      return m_TextColor;
    };

    void SetTextBackgroundColor(const Color &color)
    {
      m_BackgroundColor = color;
    }
    Color GetTextBackgroundColor() const
    {
      return m_BackgroundColor;
    }

    void SetTextAlignment(TextAlignment alignment)
    {
      m_TextAlignment = alignment;
    }
    TextAlignment GetTextAlignment()
    {
      return m_TextAlignment;
    }

    void SetDrawBackground(bool b)
    {
      m_bDrawBackground = b;
    }
    void SetBackground(AbstractPaintLayer *bkg);

    /////////////////
    //  SIGNALS    //
    /////////////////

    virtual void SetFont(ObjectPtr<FontTexture> Font);
  protected:
    virtual bool AcceptKeyNavFocus();
    void AdjustMinWidthToMatchText();

    std::string m_Text;
    Color m_TextColor;
    Color m_BackgroundColor;
    tstring m_temporary_caption;
    Validator *m_validator;
    TextAlignment m_TextAlignment;
    AbstractPaintLayer *m_Background;
    bool m_bMinimumWidthMatchText;
    bool m_bDrawBackground;
    //! If true, blend the characters alpha value with the destination and write the result to the destination buffer.
    bool m_WriteAlpha;

  public:
    virtual void SetGeometry(const Geometry &geo)
    {
      Area::SetGeometry(geo);
      ComputeContentSize();
    }
  };

}

#endif // STATICTEXTBOX_H
