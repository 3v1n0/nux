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


#ifndef BUTTON_H
#define BUTTON_H
#include "AbstractButton.h"

namespace nux
{
  class HLayout;
  class VLayout;
  class TextureArea;
  class StaticText;

  // Deprecated
  // Image position with regard to the text
  enum Position
  {
    NUX_POSITION_LEFT,    // image on left, text on the right
    NUX_POSITION_RIGHT,   // text on the left, image on right
    NUX_POSITION_TOP,     // image above text
    NUX_POSITION_BOTTOM   // image below text
  };

  // A Button that has Styling (image + text)
  class Button: public AbstractButton
  {
    NUX_DECLARE_OBJECT_TYPE(Button, View);
  public:

    enum LayoutType
    {
      HORIZONTAL,
      VERTICAL,
    };

    enum ItemOrder
    {
      IMAGE_FIRST,
      LABEL_FIRST,
    };

    enum Distribution
    {
      START_OF_LAYOUT,
      END_OF_LAYOUT,
      CENTER_OF_LAYOUT,
      SPREAD_OVER_LAYOUT,
    };

    Button(TextureArea *image, NUX_FILE_LINE_PROTO);
    Button(const std::string& button_label, NUX_FILE_LINE_PROTO);
    Button(const std::string& button_label, TextureArea *image, NUX_FILE_LINE_PROTO);
    Button(NUX_FILE_LINE_PROTO);
    ~Button();

    //! Set the label.
    /*!
        Set the label of this Button. If the \a label argument is an empty string, then the the Button label is destroyed,
        and the content of the Button is re-arranged accordingly.

        @param label The label of the Button.
    */
    void SetLabel(const std::string &button_label);

    //!Return the label of this Button.
    /*!
        Return the label of this Button.

        @return The Button label string.
    */
    std::string GetLabel() const;

        //! Set the image.
    /*!
        Set the image of this Button. If the \a image argument is NULL and this Button has an image, then the image is destroyed,
        and the content of the button is re-arranged accordingly.\n
        This Button make a copy of the \a image argument if it is not NULL. The source image may be destroyed after if it is no longer needed.\n
        The minimum and maximum size of the internal image are set to the minimum and maximum size of the \a image argument.
        \sa SetImageMinimumSize, SetImageMaximumSize.

        @param A TextureArea.
    */
    void SetImage(TextureArea *image);

    //! Get the image.
    /*!
        Returns a new TextureArea with the same content as the internal TextureArea of this Button.
        The returned TextureArea has a floating reference.\n
        If the Button doesn't have an image, the function returns NULL. 

        @return A TextureArea.
    */
    TextureArea* GetImage();

    //! Set the minimum size of the image in the Button.
    /*!
        Set the minimum size of the image in the Button.\n
        When setting an image into this Button with \a SetImage, the minimum size is set to the one of the \a image argument
        of \a SetImage.
        \sa SetImage.

        @param width Minimum width of the image.
        @param height Minimum height of the image.
    */
    void SetImageMinimumSize(int width, int height);
    
    //! Set the maximum size of the image in the Button.
    /*!
        Set the maximum size of the image in the Button.\n
        When setting an image into this Button with \a SetImage, the maximum size is set to the one of the \a image argument
        of \a SetImage.
        \sa SetImage.

        @param width Maximum width of the image.
        @param height Maximum height of the image.
    */
    void SetImageMaximumSize(int width, int height);

    //! Set the text color.
    /*!
        Set the text color.

        @param color the text color.
    */
    void SetTextColor(const Color &color);

    //! Get the text color.
    /*!
        Get the text color.

        @return The text color.
    */
    Color GetTextColor();

    void SetDistribution(Distribution distribution);
    void SetItemOrder(ItemOrder item_order);
    void SetLayoutType(LayoutType layout_type);
    void SetSpaceBetweenItems(int space_between_items);

    // Deprecated
    Property<Position> image_position;

  protected:
    std::string label_;

    LayoutType layout_type_;    //!< The button layout type.
    ItemOrder item_order_;      //!< Ordering of the text and image.
    Distribution distribution_; //!< Distribution of the image and text inside the Button layout.
    int space_between_items_;   //!< Space between the Button image and text.

    Color text_color_; //!< The text color.

    bool SetLabelProperty(std::string &value, std::string const &str);

    void Initialize(const std::string &str, TextureArea* texture_area);

    void OnStateChanged(int value);
    void OnLabelChanged(std::string value);
    void OnImageChanged(TextureArea *value);
    void OnImagePositionChanged(int value);
    
    void BuildLayout(const std::string &str, TextureArea* texture_area);

    StaticText *static_text_;
    TextureArea *image_;
    Size image_minimum_size_;
    Size image_maximum_size_;

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);
  };

  //typedef Button ToggleButton;
}

#endif // BUTTON_H
