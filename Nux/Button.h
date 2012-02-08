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


  //! A Button with styling(image + text)
  /*!
      The Button class has a non persistent active state. It returns to a normal state after a mouse click.
      For a Button is a persistent active state, use the ToggleButton class. This widget is also known as a PushButton.
  */
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
    virtual ~Button();

    //! Emitted when the button is clicked.
    sigc::signal<void, Button*> click;

    //! Emitted when the active state changes.
    /*!
        Emitted when the active state changes, as a result of a mouse click or an API call.\n
        \sa Activate, Deactivate.
    */
    sigc::signal<void, Button*> state_change;

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
        This Button make a copy of the \a image argument if it is not NULL. The source image should be destroyed after if it is no longer needed.\n
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

    //! Set the padding of the internal layout.
    /*!
        Set the padding of the internal layout.\n
        The default value is 2 for top, right, bottom and left padding.

        @param top Top padding.
        @param right Right padding.
        @param bottom Bottom padding.
        @param left Left padding.
    */
    void SetLayoutPadding(int top, int right, int bottom, int left);

    //! Add a clipping rectangle inside the button.
    /*!
        Add an internal clipping rectangle inside the button. This will clip the rendering of anything 
        inside the button. The default value is 2 for top, right, bottom and left clipping.

        @param top Top clipping.
        @param right Right clipping.
        @param bottom Bottom clipping.
        @param left Left clipping.
    */
    void SetButtonClipRegion(int top_clip, int right_clip, int bottom_clip, int left_clip);

    //! Set the distribution of elements inside the layout.
    /*!
        Set how elements are distributed in the layout. The distribution takes effect only if the element don't 
        use all the space available in the layout. In that case, they may be distributed toward the start, middle 
        or end of the layout.

        @param distribution Distribution of elements inside the layout.
    */
    void SetDistribution(Distribution distribution);

    //! Set the order of the image and label inside the layout.
    /*!
        Set the order of the image and label in the layout. Either the image comes first or the label. This is true in 
        an horizontal or vertical layout configuration.

        @param item_order Order of the image and label inside the layout.
    */
    void SetItemOrder(ItemOrder item_order);

    //! Set the layout type.
    /*!
        Set the layout type. With an horizontal layout type, the image and the label are arranged horizontally, while with a 
        vertical layout type, they are arranged vertically. You may call SetItemOrder to choose if the image or the label 
        come first in an horizontal(from left to right) or vertical(top to bottom) configuration.

        @param layout_type The layout type.
    */
    void SetLayoutType(LayoutType layout_type);

    //! Set the space between the image and the label.
    /*!
        Set the space between the image and the label.

        @param space_between_items The space between the image and the label inside the layout.
    */
    void SetSpaceBetweenItems(int space_between_items);

    //! Activate the button.
    /*!
        Activate the button. If this object is a Button, then it has no persistent state and the function does nothing.
    */
    void Activate();

    //! Deactivate the button.
    /*!
        Deactivate the button. If this object is a Button, then it has no persistent state and the function does nothing.
    */
    void Deactivate();

    virtual void SetLabelFontSize(int point);

  protected:
    LayoutType layout_type_;    //!< The button layout type.
    ItemOrder item_order_;      //!< Ordering of the text and image.
    Distribution distribution_; //!< Distribution of the image and text inside the Button layout.
    int space_between_items_;   //!< Space between the Button image and text.
    bool persistent_active_state_; //!< The button's persistent state flag.

    int layout_left_padding_;
    int layout_right_padding_;
    int layout_top_padding_;
    int layout_bottom_padding_;

    int left_clip_;
    int right_clip_;
    int top_clip_;
    int bottom_clip_;

    bool SetLabelProperty(std::string &value, std::string const &str);

    void Initialize(const std::string &str, TextureArea* texture_area);

    void BuildLayout(const std::string &str, TextureArea* texture_area);

    TextureArea *image_;
    Size image_minimum_size_;
    Size image_maximum_size_;

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
//     virtual void PreLayoutManagement();
//     virtual long PostLayoutManagement(long LayoutResult);
    virtual long ComputeContentSize();
  };

  //typedef Button ToggleButton;
}

#endif // BUTTON_H
