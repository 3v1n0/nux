#ifndef SIZE_H
#define SIZE_H

NAMESPACE_BEGIN

class Size
{
public:
	Size(t_int32 w = 0, t_int32 h = 0);
	~Size();
	Size(const Size& s);

	Size& operator = (const Size& s);
	t_bool operator == (const Size& s) const;
	t_bool operator != (const Size& s) const;
    Size operator + (const Size& p) const;
    Size operator - (const Size& p) const;


	t_int32 GetWidth() const;
	t_int32 GetHeight() const;
	void SetWidth(t_int32 w);
	void SetHeight(t_int32 h);

private:
	t_int32 width_;
	t_int32 height_;
};

NAMESPACE_END

#endif // SIZE_H

