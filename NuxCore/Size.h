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

