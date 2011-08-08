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


#ifndef RGBAPROPERTY_H
#define RGBAPROPERTY_H

namespace nux
{

  class ToggleButton;
  class ColorGradientPropertyItem;

  // TODO: Tim Penhey 2011-05-12
  // Both this class and RGBPropertyItem should share a base class
  // as many of the color calculations are common to the two.
  class RGBAPropertyItem: public SectionProperty
  {
    NODE_XML_CLASS_MEMBER (RGBAPropertyItem);
    NODE_SKIP_CHILD (true);
    NODE_SIG_RECEIVER (RecvPropertyChange, ColorGradient *);

  public:
    RGBAPropertyItem (const TCHAR *name, float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 1.0f);
    virtual ~RGBAPropertyItem();

    virtual long ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter, RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color (0x0) );
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
      return color_.red;
    }
    double GetGreen()   const
    {
      return color_.green;
    }
    double GetBlue()    const
    {
      return color_.blue;
    }
    double GetAlpha()   const
    {
      return color_.alpha;
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

    void SetColorModel (color::Model cm);
    void SetColorFormat (color::Format cf);

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

    Color color_;
    ToggleButton *m_ColorModel;
    ToggleButton *m_ColorFormat;
    color::Model color_model_;
    color::Format color_format_;
  };

}

#endif // RGBAPROPERTY_H


