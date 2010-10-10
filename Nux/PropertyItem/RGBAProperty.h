/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef RGBAPROPERTY_H
#define RGBAPROPERTY_H

namespace nux
{

  class Button;
  class ColorGradientPropertyItem;

  class RGBAPropertyItem: public SectionProperty
  {
    NODE_XML_CLASS_MEMBER (RGBAPropertyItem);
    NODE_SKIP_CHILD (true);
    NODE_SIG_RECEIVER (RecvPropertyChange, ColorGradient *);

  public:
    RGBAPropertyItem (const TCHAR *name, float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 1.0f);
    virtual ~RGBAPropertyItem();

    virtual long ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void DrawProperty (GraphicsContext &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter, RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color (0x0) );
    virtual void ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector);
    virtual int GetItemBestHeight();

    void SetRed (double red)
    {
      m_red->SetValue (red);
      UpdateStartToEndColors();
    }
    void SetGreen (double green)
    {
      m_green->SetValue (green);
      UpdateStartToEndColors();
    }
    void SetBlue (double blue)
    {
      m_blue->SetValue (blue);
      UpdateStartToEndColors();
    }
    void SetAlpha (double alpha)
    {
      m_alpha->SetValue (alpha);
      UpdateStartToEndColors();
    }

    double GetRed()     const
    {
      return m_Red;
    }
    double GetGreen()   const
    {
      return m_Green;
    }
    double GetBlue()    const
    {
      return m_Blue;
    }
    double GetAlpha()   const
    {
      return m_Alpha;
    }
    void SetColor (double red, double green, double blue, double alpha)
    {
      m_red->SetValue (red);
      m_green->SetValue (green);
      m_blue->SetValue (blue);
      m_alpha->SetValue (alpha);
      UpdateStartToEndColors();
    }

    void OnChangeColorModel();
    void OnChangeColorFormat();

    void SetColorModel (eColorModel cm);
    void SetColorFormat (Color::Format cf);

  private:
    void UpdateStartToEndColors();

    void RedChange (ColorGradient *slider);
    void GreenChange (ColorGradient *slider);
    void BlueChange (ColorGradient *slider);
    void AlphaChange (ColorGradient *slider);

    ColorGradientPropertyItem *m_red;
    ColorGradientPropertyItem *m_green;
    ColorGradientPropertyItem *m_blue;
    ColorGradientPropertyItem *m_alpha;

    float m_Red, m_Green, m_Blue, m_Alpha;
    Button *m_ColorModel;
    Button *m_ColorFormat;
    enum eColorModel m_color_model;
    Color::Format m_color_format;
  };

}

#endif // RGBAPROPERTY_H


