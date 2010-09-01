#include "NKernel.h"
#include "Size.h"

NAMESPACE_BEGIN

Size::Size(t_int32 w, t_int32 h)
{ 
	width_  = w; 
	height_ = h;
}

//----------------------------------------------------------------------------
Size::~Size()
{

}

//----------------------------------------------------------------------------
Size::Size(const Size& s)
{
	width_ = s.width_;
	height_ = s.height_;
}

//----------------------------------------------------------------------------
Size& Size::operator =(const Size& s)
{
	width_ = s.width_;
	height_ = s.height_;
	return *this;
}

t_bool Size::operator== (const Size& s) const
{
	if((width_ == s.width_) && (height_ == s.height_))
	{
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
t_bool Size::operator!= (const Size& s) const
{
	if((width_ == s.width_) && (height_ == s.height_))
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------
Size Size::operator+ (const Size& s) const
{
    Size size;
    size.width_ = width_ + size.width_;
    size.height_ = height_ + size.height_;
    return size;
}

//----------------------------------------------------------------------------
Size Size::operator- (const Size& s) const
{
    Size size;
    size.width_ = width_ - size.width_;
    size.height_ = height_ - size.height_;
    return size;
}

//----------------------------------------------------------------------------
t_int32 Size::GetWidth() const
{
	return width_;
}

//----------------------------------------------------------------------------
t_int32 Size::GetHeight() const
{
	return height_;
}

//----------------------------------------------------------------------------
void Size::SetWidth(t_int32 w)
{
	width_ = w;
}

//----------------------------------------------------------------------------
void Size::SetHeight(t_int32 h)
{
	height_ = h;
}

//----------------------------------------------------------------------------

NAMESPACE_END
