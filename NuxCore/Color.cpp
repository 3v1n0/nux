#include "NKernel.h"
#include "Math/MathUtility.h"
#include <cmath>
#include "NSystemTypes.h"
#include "ColorFunctions.h"
#include "Color.h"


NAMESPACE_BEGIN

//Red colors
const Color Color::IndianRed           (0xCD/255.0f, 0x5C/255.0f, 0x5C/255.0f);// 	205     92      92
const Color Color::LightCoral          (0xF0/255.0f, 0x80/255.0f, 0x80/255.0f);// 	240     128     128
const Color Color::Salmon              (0xFA/255.0f, 0x80/255.0f, 0x72/255.0f);// 	250     128     114
const Color Color::DarkSalmon          (0xE9/255.0f, 0x96/255.0f, 0x7A/255.0f);// 	233     150     122
const Color Color::LightSalmon         (0xFF/255.0f, 0xA0/255.0f, 0x7A/255.0f);// 	255     160     122
const Color Color::Crimson             (0xDC/255.0f, 0x14/255.0f, 0x3C/255.0f);// 	220     20      60
const Color Color::Red                 (0xFF/255.0f, 0x00/255.0f, 0x00/255.0f);// 	255     0       0
const Color Color::FireBrick           (0xB2/255.0f, 0x22/255.0f, 0x22/255.0f);// 	178     34      34
const Color Color::DarkRed             (0x8B/255.0f, 0x00/255.0f, 0x00/255.0f);// 	139     0       0

//Pink colors
const Color Color::Pink                (0xFF/255.0f, 0xC0/255.0f, 0xCB/255.0f);// 	255     192     203
const Color Color::LightPink           (0xFF/255.0f, 0xB6/255.0f, 0xC1/255.0f);// 	255     182     193
const Color Color::HotPink             (0xFF/255.0f, 0x69/255.0f, 0xB4/255.0f);// 	255     105     180
const Color Color::DeepPink            (0xFF/255.0f, 0x14/255.0f, 0x93/255.0f);// 	255     20      147
const Color Color::MediumVioletRed     (0xC7/255.0f, 0x15/255.0f, 0x85/255.0f);// 	199     21      133
const Color Color::PaleVioletRed       (0xDB/255.0f, 0x70/255.0f, 0x93/255.0f);//    219     112     147

//Orange colors
//const Color Color::LightSalmon       (0xFF/255.0f, 0xA0/255.0f, 0x7A/255.0f);// 	255     160     122
const Color Color::Coral               (0xFF/255.0f, 0x7F/255.0f, 0x50/255.0f);//    255     127     80
const Color Color::Tomato              (0xFF/255.0f, 0x63/255.0f, 0x47/255.0f);//    255     99      71
const Color Color::OrangeRed           (0xFF/255.0f, 0x45/255.0f, 0x00/255.0f);//    255     69      0
const Color Color::DarkOrange          (0xFF/255.0f, 0x8C/255.0f, 0x00/255.0f);//    255     140     0
const Color Color::Orange              (0xFF/255.0f, 0xA5/255.0f, 0x00/255.0f);//    255     165     0

//Yellow colors
const Color Color::Gold                (0xFF/255.0f, 0xD7/255.0f, 0x00/255.0f);// 	255     215     0
const Color Color::Yellow              (0xFF/255.0f, 0xFF/255.0f, 0x00/255.0f);// 	255     255     0
const Color Color::LightYellow         (0xFF/255.0f, 0xFF/255.0f, 0xE0/255.0f);// 	255     255     224
const Color Color::LemonChiffon        (0xFF/255.0f, 0xFA/255.0f, 0xCD/255.0f);// 	255     250     205
const Color Color::LightGoldenrodYellow(0xFA/255.0f, 0xFA/255.0f, 0xD2/255.0f);// 	250     250     210
const Color Color::PapayaWhip          (0xFF/255.0f, 0xEF/255.0f, 0xD5/255.0f);// 	255     239     213
const Color Color::Moccasin            (0xFF/255.0f, 0xE4/255.0f, 0xB5/255.0f);// 	255     228     181
const Color Color::PeachPuff           (0xFF/255.0f, 0xDA/255.0f, 0xB9/255.0f);// 	255     218     185
const Color Color::PaleGoldenrod       (0xEE/255.0f, 0xE8/255.0f, 0xAA/255.0f);// 	238     232     170
const Color Color::Khaki               (0xF0/255.0f, 0xE6/255.0f, 0x8C/255.0f);// 	240     230     140
const Color Color::DarkKhaki           (0xBD/255.0f, 0xB7/255.0f, 0x6B/255.0f);// 	189     183     107

//Purple colors
const Color Color::Lavender            (0xE6/255.0f, 0xE6/255.0f, 0xFA/255.0f);// 	230     230     250
const Color Color::Thistle             (0xD8/255.0f, 0xBF/255.0f, 0xD8/255.0f);// 	216     191     216
const Color Color::Plum                (0xDD/255.0f, 0xA0/255.0f, 0xDD/255.0f);// 	221     160     221
const Color Color::Violet              (0xEE/255.0f, 0x82/255.0f, 0xEE/255.0f);// 	238     130     238
const Color Color::Orchid              (0xDA/255.0f, 0x70/255.0f, 0xD6/255.0f);// 	218     112     214
const Color Color::Fuchsia             (0xFF/255.0f, 0x00/255.0f, 0xFF/255.0f);// 	255     0       255
const Color Color::Magenta             (0xFF/255.0f, 0x00/255.0f, 0xFF/255.0f);// 	255     0       255
const Color Color::MediumOrchid        (0xBA/255.0f, 0x55/255.0f, 0xD3/255.0f);// 	186     85      211
const Color Color::MediumPurple        (0x93/255.0f, 0x70/255.0f, 0xDB/255.0f);// 	147     112     219
const Color Color::BlueViolet          (0x8A/255.0f, 0x2B/255.0f, 0xE2/255.0f);// 	138     43      226
const Color Color::DarkViolet          (0x94/255.0f, 0x00/255.0f, 0xD3/255.0f);// 	148     0       211
const Color Color::DarkOrchid          (0x99/255.0f, 0x32/255.0f, 0xCC/255.0f);// 	153     50      204
const Color Color::DarkMagenta         (0x8B/255.0f, 0x00/255.0f, 0x8B/255.0f);// 	139     0       139
const Color Color::Purple              (0x80/255.0f, 0x00/255.0f, 0x80/255.0f);// 	128     0       128
const Color Color::Indigo              (0x4B/255.0f, 0x00/255.0f, 0x82/255.0f);// 	75      0       130
const Color Color::SlateBlue           (0x6A/255.0f, 0x5A/255.0f, 0xCD/255.0f);// 	106     90      205
const Color Color::DarkSlateBlue       (0x48/255.0f, 0x3D/255.0f, 0x8B/255.0f);// 	72      61      139

//Green colors
const Color Color::GreenYellow         (0xAD/255.0f, 0xFF/255.0f, 0x2F/255.0f);// 	173     255     47
const Color Color::Chartreuse          (0x7F/255.0f, 0xFF/255.0f, 0x00/255.0f);// 	127     255     0
const Color Color::LawnGreen           (0x7C/255.0f, 0xFC/255.0f, 0x00/255.0f);// 	124     252     0
const Color Color::Lime                (0x00/255.0f, 0xFF/255.0f, 0x00/255.0f);// 	0       255     0
const Color Color::LimeGreen           (0x32/255.0f, 0xCD/255.0f, 0x32/255.0f);// 	50      205     50
const Color Color::PaleGreen           (0x98/255.0f, 0xFB/255.0f, 0x98/255.0f);// 	152     251     152
const Color Color::LightGreen          (0x90/255.0f, 0xEE/255.0f, 0x90/255.0f);// 	144     238     144
const Color Color::MediumSpringGreen   (0x00/255.0f, 0xFA/255.0f, 0x9A/255.0f);// 	0       250     154
const Color Color::SpringGreen         (0x00/255.0f, 0xFF/255.0f, 0x7F/255.0f);// 	0       255     127
const Color Color::MediumSeaGreen      (0x3C/255.0f, 0xB3/255.0f, 0x71/255.0f);// 	60      179     113
const Color Color::SeaGreen            (0x2E/255.0f, 0x8B/255.0f, 0x57/255.0f);// 	46      139     87
const Color Color::ForestGreen         (0x22/255.0f, 0x8B/255.0f, 0x22/255.0f);// 	34      139     34
const Color Color::Green               (0x00/255.0f, 0x80/255.0f, 0x00/255.0f);// 	0       128     0
const Color Color::DarkGreen           (0x00/255.0f, 0x64/255.0f, 0x00/255.0f);// 	0       100     0
const Color Color::YellowGreen         (0x9A/255.0f, 0xCD/255.0f, 0x32/255.0f);// 	154     205     50
const Color Color::OliveDrab           (0x6B/255.0f, 0x8E/255.0f, 0x23/255.0f);// 	107     142     35
const Color Color::Olive               (0x80/255.0f, 0x80/255.0f, 0x00/255.0f);// 	128     128     0
const Color Color::DarkOliveGreen      (0x55/255.0f, 0x6B/255.0f, 0x2F/255.0f);// 	85      107     47
const Color Color::MediumAquamarine    (0x66/255.0f, 0xCD/255.0f, 0xAA/255.0f);// 	102     205     170
const Color Color::DarkSeaGreen        (0x8F/255.0f, 0xBC/255.0f, 0x8F/255.0f);// 	143     188     143     // in .Net this is (0x8F, 0xBC, 0x8B)
const Color Color::LightSeaGreen       (0x20/255.0f, 0xB2/255.0f, 0xAA/255.0f);// 	32      178     170
const Color Color::DarkCyan            (0x00/255.0f, 0x8B/255.0f, 0x8B/255.0f);// 	0       139     139
const Color Color::Teal                (0x00/255.0f, 0x80/255.0f, 0x80/255.0f);// 	0       128     128

//Blue colors
const Color Color::Aqua                (0x00/255.0f, 0xFF/255.0f, 0xFF/255.0f);// 	0       255     255
const Color Color::Cyan                (0x00/255.0f, 0xFF/255.0f, 0xFF/255.0f);// 	0       255     255
const Color Color::LightCyan           (0xE0/255.0f, 0xFF/255.0f, 0xFF/255.0f);// 	224     255     255
const Color Color::PaleTurquoise       (0xAF/255.0f, 0xEE/255.0f, 0xEE/255.0f);// 	175     238     238
const Color Color::Aquamarine          (0x7F/255.0f, 0xFF/255.0f, 0xD4/255.0f);// 	127     255     212
const Color Color::Turquoise           (0x40/255.0f, 0xE0/255.0f, 0xD0/255.0f);// 	64      224     208
const Color Color::MediumTurquoise     (0x48/255.0f, 0xD1/255.0f, 0xCC/255.0f);// 	72      209     204
const Color Color::DarkTurquoise       (0x00/255.0f, 0xCE/255.0f, 0xD1/255.0f);// 	0       206     209
const Color Color::CadetBlue           (0x5F/255.0f, 0x9E/255.0f, 0xA0/255.0f);// 	95      158     160
const Color Color::SteelBlue           (0x46/255.0f, 0x82/255.0f, 0xB4/255.0f);// 	70      130     180
const Color Color::LightSteelBlue      (0xB0/255.0f, 0xC4/255.0f, 0xDE/255.0f);// 	176     196     222
const Color Color::PowderBlue          (0xB0/255.0f, 0xE0/255.0f, 0xE6/255.0f);// 	176     224     230
const Color Color::LightBlue           (0xAD/255.0f, 0xD8/255.0f, 0xE6/255.0f);// 	173     216     230
const Color Color::SkyBlue             (0x87/255.0f, 0xCE/255.0f, 0xEB/255.0f);// 	135     206     235
const Color Color::LightSkyBlue        (0x87/255.0f, 0xCE/255.0f, 0xFA/255.0f);// 	135     206     250
const Color Color::DeepSkyBlue         (0x00/255.0f, 0xBF/255.0f, 0xFF/255.0f);// 	0       191     255
const Color Color::DodgerBlue          (0x1E/255.0f, 0x90/255.0f, 0xFF/255.0f);// 	30      144     255
const Color Color::CornflowerBlue      (0x64/255.0f, 0x95/255.0f, 0xED/255.0f);// 	100     149     237
const Color Color::MediumSlateBlue     (0x7B/255.0f, 0x68/255.0f, 0xEE/255.0f);// 	123     104     238
const Color Color::RoyalBlue           (0x41/255.0f, 0x69/255.0f, 0xE1/255.0f);// 	65      105     225
const Color Color::Blue                (0x00/255.0f, 0x00/255.0f, 0xFF/255.0f);// 	0       0       255
const Color Color::MediumBlue          (0x00/255.0f, 0x00/255.0f, 0xCD/255.0f);// 	0       0       205
const Color Color::DarkBlue            (0x00/255.0f, 0x00/255.0f, 0x8B/255.0f);// 	0       0       139
const Color Color::Navy                (0x00/255.0f, 0x00/255.0f, 0x80/255.0f);// 	0       0       128
const Color Color::MidnightBlue        (0x19/255.0f, 0x19/255.0f, 0x70/255.0f);// 	25      25      112

//Brown colors
const Color Color::Cornsilk            (0xFF/255.0f, 0xF8/255.0f, 0xDC/255.0f);// 	255     248     220
const Color Color::BlanchedAlmond      (0xFF/255.0f, 0xEB/255.0f, 0xCD/255.0f);// 	255     235     205
const Color Color::Bisque              (0xFF/255.0f, 0xE4/255.0f, 0xC4/255.0f);// 	255     228     196
const Color Color::NavajoWhite         (0xFF/255.0f, 0xDE/255.0f, 0xAD/255.0f);// 	255     222     173
const Color Color::Wheat               (0xF5/255.0f, 0xDE/255.0f, 0xB3/255.0f);// 	245     222     179
const Color Color::BurlyWood           (0xDE/255.0f, 0xB8/255.0f, 0x87/255.0f);// 	222     184     135
const Color Color::Tan                 (0xD2/255.0f, 0xB4/255.0f, 0x8C/255.0f);// 	210     180     140
const Color Color::RosyBrown           (0xBC/255.0f, 0x8F/255.0f, 0x8F/255.0f);// 	188     143     143
const Color Color::SandyBrown          (0xF4/255.0f, 0xA4/255.0f, 0x60/255.0f);// 	244     164     96
const Color Color::Goldenrod           (0xDA/255.0f, 0xA5/255.0f, 0x20/255.0f);// 	218     165     32
const Color Color::DarkGoldenrod       (0xB8/255.0f, 0x86/255.0f, 0x0B/255.0f);// 	184     134     11
const Color Color::Peru                (0xCD/255.0f, 0x85/255.0f, 0x3F/255.0f);// 	205     133     63
const Color Color::Chocolate           (0xD2/255.0f, 0x69/255.0f, 0x1E/255.0f);// 	210     105     30
const Color Color::SaddleBrown         (0x8B/255.0f, 0x45/255.0f, 0x13/255.0f);// 	139     69      19
const Color Color::Sienna              (0xA0/255.0f, 0x52/255.0f, 0x2D/255.0f);// 	160     82      45
const Color Color::Brown               (0xA5/255.0f, 0x2A/255.0f, 0x2A/255.0f);// 	165     42      42
const Color Color::Maroon              (0x80/255.0f, 0x00/255.0f, 0x00/255.0f);// 	128     0       0

//White colors
const Color Color::White               (0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f);// 	255     255     255
const Color Color::Snow                (0xFF/255.0f, 0xFA/255.0f, 0xFA/255.0f);// 	255     250     250
const Color Color::Honeydew            (0xF0/255.0f, 0xFF/255.0f, 0xF0/255.0f);// 	240     255     240
const Color Color::MintCream           (0xF5/255.0f, 0xFF/255.0f, 0xFA/255.0f);// 	245     255     250
const Color Color::Azure               (0xF0/255.0f, 0xFF/255.0f, 0xFF/255.0f);// 	240     255     255
const Color Color::AliceBlue           (0xF0/255.0f, 0xF8/255.0f, 0xFF/255.0f);// 	240     248     255
const Color Color::GhostWhite          (0xF8/255.0f, 0xF8/255.0f, 0xFF/255.0f);// 	248     248     255
const Color Color::WhiteSmoke          (0xF5/255.0f, 0xF5/255.0f, 0xF5/255.0f);// 	245     245     245
const Color Color::Seashell            (0xFF/255.0f, 0xF5/255.0f, 0xEE/255.0f);// 	255     245     238
const Color Color::Beige               (0xF5/255.0f, 0xF5/255.0f, 0xDC/255.0f);// 	245     245     220
const Color Color::OldLace             (0xFD/255.0f, 0xF5/255.0f, 0xE6/255.0f);// 	253     245     230
const Color Color::FloralWhite         (0xFF/255.0f, 0xFA/255.0f, 0xF0/255.0f);// 	255     250     240
const Color Color::Ivory               (0xFF/255.0f, 0xFF/255.0f, 0xF0/255.0f);// 	255     255     240
const Color Color::AntiqueWhite        (0xFA/255.0f, 0xEB/255.0f, 0xD7/255.0f);// 	250     235     215
const Color Color::Linen               (0xFA/255.0f, 0xF0/255.0f, 0xE6/255.0f);// 	250     240     230
const Color Color::LavenderBlush       (0xFF/255.0f, 0xF0/255.0f, 0xF5/255.0f);// 	255     240     245
const Color Color::MistyRose           (0xFF/255.0f, 0xE4/255.0f, 0xE1/255.0f);// 	255     228     225

//Grey colors
const Color Color::Gainsboro           (0xDC/255.0f, 0xDC/255.0f, 0xDC/255.0f);// 	220     220     220
const Color Color::LightGrey           (0xD3/255.0f, 0xD3/255.0f, 0xD3/255.0f);// 	211     211     211
const Color Color::Silver              (0xC0/255.0f, 0xC0/255.0f, 0xC0/255.0f);// 	192     192     192
const Color Color::DarkGray            (0xA9/255.0f, 0xA9/255.0f, 0xA9/255.0f);// 	169     169     169
const Color Color::Gray                (0x80/255.0f, 0x80/255.0f, 0x80/255.0f);// 	128     128     128
const Color Color::DimGray             (0x69/255.0f, 0x69/255.0f, 0x69/255.0f);// 	105     105     105
const Color Color::LightSlateGray      (0x77/255.0f, 0x88/255.0f, 0x99/255.0f);// 	119     136     153
const Color Color::SlateGray           (0x70/255.0f, 0x80/255.0f, 0x90/255.0f);// 	112     128     144
const Color Color::DarkSlateGray       (0x2F/255.0f, 0x4F/255.0f, 0x4F/255.0f);// 	47      79      79
const Color Color::Black               (0x00/255.0f, 0x00/255.0f, 0x00/255.0f);// 	0       0       0

Color Color::Dummy = Color::Black;

Color::Color()
: red_(0.0), green_(0.0), blue_(0.0f), alpha_(1.0f)
{                                   
}

Color::~Color()
{
}
 
Color::Color(const Color& color)
{
  red_   = color.R();
  green_ = color.G();
  blue_  = color.B();
  alpha_ = color.A();  
}
   
Color::Color(unsigned int c)
{
    red_   = (t_float) (RGB_GET_RED(c))*(1.f/255.f);
    green_ = (t_float) (RGB_GET_GREEN(c))*(1.f/255.f);
    blue_  = (t_float) (RGB_GET_BLUE(c))*(1.f/255.f);
    alpha_ = (t_float) (RGBA_GET_ALPHA(c))*(1.f/255.f);
}

Color::Color(t_float r, t_float g, t_float b)
{
    red_ = r;
    green_ = g;
    blue_ = b;
    alpha_ = 1.0f;
}

Color::Color(t_float r, t_float g, t_float b, t_float a)
{
    red_ = r;
    green_ = g;
    blue_ = b;
    alpha_ = a;
}

Color* Color::Clone() const
{
    return new Color(*this);
}

Color& Color::operator= (const Color& color)
{
  red_   = color.R();
  green_ = color.G();
  blue_  = color.B();
  alpha_ = color.A(); 

  return *this;
}

bool Color::operator == (const Color& color) const
{
	if((red_ == color.R())
		&& (green_ == color.G())
		&& (blue_ == color.B())
		&& (alpha_ == color.A())
		)
	{
		return true;
	}
	return false;
}

bool Color::operator!= (const Color& color) const
{
	if((red_ == color.R())
		&& (green_ == color.G())
		&& (blue_ == color.B())
		&& (alpha_ == color.A())
		)
	{
	    return false;
	}
    return true;
}

void Color::SetRGB(t_float r, t_float g, t_float b)
{
    red_   = r;
    green_ = g;
    blue_  = b;
    alpha_ = 1.0f;
} 

void Color::SetRGBA(t_float r, t_float g, t_float b, t_float a)
{
    red_   = r;
    green_ = g;
    blue_  = b;
    alpha_ = a;
}

/*void Color::set_rgba(t_uint32 val)
{
  red_   = (t_float)RGBA_GET_RED(   val )*(1.f/255.f);
  green_ = (t_float)RGBA_GET_GREEN( val )*(1.f/255.f);
  blue_  = (t_float)RGBA_GET_BLUE(  val )*(1.f/255.f);
  alpha_ = (t_float)RGBA_GET_ALPHA( val )*(1.f/255.f);
}*/

void Color::ClampVal()
{
    red_ = Max<t_float>(0.f, Min(1.f, red_));
  green_ = Max<t_float>(0.f, Min(1.f, green_));
  blue_  = Max<t_float>(0.f, Min(1.f, blue_));
  alpha_ = Max<t_float>(0.f, Min(1.f, alpha_));
}

void Color::Saturate()
{
  red_   = Min<t_float>(1.f, red_);
  green_ = Min<t_float>(1.f, green_);
  blue_  = Min<t_float>(1.f, blue_);
  alpha_ = Min<t_float>(1.f, alpha_);
}

void Color::Complement()
{
  red_   = 1.0f - red_;
  green_ = 1.0f - green_;
  blue_  = 1.0f - blue_;
  alpha_ = 1.0f - alpha_;
}

Color Color::Luminance()
{
    float L = 0.30*red_ + 0.59*green_ + 0.11*blue_;
    return Color(L, L, L, 1.0f);
}

Color Color::OneMinusLuminance()
{
    float L = 1.0f - (0.30*red_ + 0.59*green_ + 0.11*blue_);
    return Color(L, L, L, 1.0f);
}

// void Color::ConvertTo(BitmapFormat format, char* data_bits, t_float round_offset, bool gamma_correct) const
// {
//     t_uint32 iR, iG, iB, iA;
//     t_float Rg, Gg, Bg; // Gamma Corrected values
//     t_float Ag;
// 
//     if(gamma_correct)
//     {
//         Rg = Do_Gamma_Correction(red_);
//         Gg = Do_Gamma_Correction(green_);
//         Bg = Do_Gamma_Correction(blue_);
//     }
//     else
//     {
//         Rg = red_;
//         Gg = green_;
//         Bg = blue_;
//     }
//       
//     Ag = alpha_;
//       
//     switch(format)
//     {
//         case BITFMT_R8G8B8A8:
//         *((t_byte*)data_bits+0) = (t_byte)((t_float)Ag * 255. + round_offset);
//         *((t_byte*)data_bits+1) = (t_byte)((t_float)Bg * 255. + round_offset);
//         *((t_byte*)data_bits+2) = (t_byte)((t_float)Gg * 255. + round_offset);
//         *((t_byte*)data_bits+3) = (t_byte)((t_float)Rg * 255. + round_offset);
//         break;
// 
//         case BITFMT_B8G8R8A8:
//         *((t_byte*)data_bits+0) = (t_byte)((t_float)Ag * 255. + round_offset);
//         *((t_byte*)data_bits+1) = (t_byte)((t_float)Rg * 255. + round_offset);
//         *((t_byte*)data_bits+2) = (t_byte)((t_float)Gg * 255. + round_offset);
//         *((t_byte*)data_bits+3) = (t_byte)((t_float)Bg * 255. + round_offset);
//         break;
// 
//         case BITFMT_A8R8G8B8:
//         *((t_byte*)data_bits+0) = (t_byte)((t_float)Bg * 255. + round_offset);
//         *((t_byte*)data_bits+1) = (t_byte)((t_float)Gg * 255. + round_offset);
//         *((t_byte*)data_bits+2) = (t_byte)((t_float)Rg * 255. + round_offset);
//         *((t_byte*)data_bits+3) = (t_byte)((t_float)Ag * 255. + round_offset);
//         break;
// 
//         case BITFMT_A8B8G8R8:
//         *((t_byte*)data_bits+0) = (t_byte)((t_float)Rg * 255. + round_offset);
//         *((t_byte*)data_bits+1) = (t_byte)((t_float)Gg * 255. + round_offset);
//         *((t_byte*)data_bits+2) = (t_byte)((t_float)Bg * 255. + round_offset);
//         *((t_byte*)data_bits+3) = (t_byte)((t_float)Ag * 255. + round_offset);
//         break;
// 
//         case BITFMT_X8R8G8B8:
//         *((t_byte*)data_bits+0) = 0x00;
//         *((t_byte*)data_bits+1) = (t_byte)((t_float)Bg * 255. + round_offset);
//         *((t_byte*)data_bits+2) = (t_byte)((t_float)Gg * 255. + round_offset);
//         *((t_byte*)data_bits+3) = (t_byte)((t_float)Rg * 255. + round_offset);
//         break;
// 
//         case BITFMT_X8B8G8R8:
//         *((t_byte*)data_bits+0) = 0x00;
//         *((t_byte*)data_bits+1) = (t_byte)((t_float)Rg * 255. + round_offset);
//         *((t_byte*)data_bits+2) = (t_byte)((t_float)Gg * 255. + round_offset);
//         *((t_byte*)data_bits+3) = (t_byte)((t_float)Bg * 255. + round_offset);
//         break;
//     ////////////////////////////////////////////////////////////////////////////////////////
//         case BITFMT_A2R10G10B10: 
//         iR = (t_uint32) ((t_float)Rg * 1023.f + round_offset);
//         iG = (t_uint32) ((t_float)Gg * 1023.f + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 1023.f + round_offset);
//         iA = (t_uint32) ((t_float)Ag * 3.f + round_offset);
//         *((t_uint32*)data_bits) = (iA<<(10+10+10)) | (iR<<(10+10)) | (iG<<10) | iB;
//         break;
//         
//         case BITFMT_A2B10G10R10: 
//         iR = (t_uint32) ((t_float)Rg * 1023.f + round_offset);
//         iG = (t_uint32) ((t_float)Gg * 1023.f + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 1023.f + round_offset);
//         iA = (t_uint32) ((t_float)Ag * 3.f + round_offset);
//         *((t_uint32*)data_bits) = (iA<<(10+10+10)) | (iB<<(10+10)) | (iG<<10) | iR;
//         break;
//           
//         case BITFMT_R10G10B10A2: 
//         iR = (t_uint32) ((t_float)Rg * 1023.f + round_offset);
//         iG = (t_uint32) ((t_float)Gg * 1023.f + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 1023.f + round_offset);
//         iA = (t_uint32) ((t_float)Ag * 3.f + round_offset);
//         *((t_uint32*)data_bits) = (iR<<(10+10+10)) | (iG<<(10+10)) | (iB<<10) | iA;
//         break;
// 
//         case BITFMT_B10G10R10A2: 
//         iR = (t_uint32) ((t_float)Rg * 1023.f + round_offset);
//         iG = (t_uint32) ((t_float)Gg * 1023.f + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 1023.f + round_offset);
//         iA = (t_uint32) ((t_float)Ag * 3.f + round_offset);
//         *((t_uint32*)data_bits) = (iB<<(10+10+10)) | (iG<<(10+10)) | (iR<<10) | iA;
//         break;
// 
//         case BITFMT_R8G8B8:
//         *((t_byte*)data_bits+0) = (t_byte)((t_float)Bg * 255. + round_offset);
//         *((t_byte*)data_bits+1) = (t_byte)((t_float)Gg * 255. + round_offset);
//         *((t_byte*)data_bits+2) = (t_byte)((t_float)Rg * 255. + round_offset);
//         break;
// 
//         case BITFMT_B8G8R8:
//         *((t_byte*)data_bits+0) = (t_byte)((t_float)Rg * 255. + round_offset);
//         *((t_byte*)data_bits+1) = (t_byte)((t_float)Gg * 255. + round_offset);
//         *((t_byte*)data_bits+2) = (t_byte)((t_float)Bg * 255. + round_offset);
//         break;
// 
//         case BITFMT_R5G6B5:
//         iR = (t_uint32) ((t_float)Rg * 31. + round_offset); // apply rounding bias then truncate
//         iG = (t_uint32) ((t_float)Gg * 63. + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 31. + round_offset);
//         *((t_uint16*)data_bits) = (t_uint16) ((iR<<11) | (iG<<5) | iB);
//         break;
// 
//         case BITFMT_X1R5G5B5:
//         iR = (t_uint32) ((t_float)Rg * 31. + round_offset);
//         iG = (t_uint32) ((t_float)Gg * 31. + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 31. + round_offset);
//         *((t_uint16*)data_bits) = (t_uint16) ((iR<<10) | (iG<<5) | iB);
//         break;
// 
//         case BITFMT_A1R5G5B5:
//         iA = (t_uint32) ((t_float)Ag *  1. + round_offset);
//         iR = (t_uint32) ((t_float)Rg * 31. + round_offset);
//         iG = (t_uint32) ((t_float)Gg * 31. + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 31. + round_offset);
//         *((t_uint16*)data_bits) = (t_uint16) ((iA<<15) | (iR<<10) | (iG<<5) | iB);
//         break;
// 
//         case BITFMT_A4R4G4B4:
//         iA = (t_uint32) ((t_float)Ag * 15. + round_offset);
//         iR = (t_uint32) ((t_float)Rg * 15. + round_offset);
//         iG = (t_uint32) ((t_float)Gg * 15. + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 15. + round_offset);
//         *((t_uint16*)data_bits) = (t_uint16) ((iA<<12) | (iR<<8) | (iG<<4) | iB);
//         break;
// 
//         case BITFMT_X4R4G4B4:
//         iR = (t_uint32) ((t_float)Rg * 15. + round_offset);
//         iG = (t_uint32) ((t_float)Gg * 15. + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 15. + round_offset);
//         *((t_uint16*)data_bits) = (t_uint16) ((0x00<<12) | (iR<<8) | (iG<<4) | iB);
//         break;
// 
//         case BITFMT_A8R3G3B2:
//         iA = (t_uint32) ((t_float)Ag * 255. + round_offset);
//         iR = (t_uint32) ((t_float)Rg * 7. + round_offset);
//         iG = (t_uint32) ((t_float)Gg * 7. + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 3. + round_offset);
//         *((t_uint16*)data_bits) = (t_uint16) ((iA<<8) | (iR<<5) | (iG<<2) | iB);
//         break;
// 
//         case BITFMT_R3G3B2:
//         iR = (t_uint32) ((t_float)Rg * 7. + round_offset);
//         iG = (t_uint32) ((t_float)Gg * 7. + round_offset);
//         iB = (t_uint32) ((t_float)Bg * 3. + round_offset);
//         *((t_byte*)data_bits) = (t_byte) ((iR<<5) | (iG<<2) | iB);
//         break;
// 
//         case BITFMT_RGBA16F:
//         case BITFMT_RGB32F:
//         case BITFMT_RGBA32F:
//         case BITFMT_D24S8:
//         case BITFMT_DXT1:
//         case BITFMT_DXT2:
//         case BITFMT_DXT3:
//         case BITFMT_DXT4:
//         case BITFMT_DXT5:
//         case BITFMT_A8:
//         case BITFMT_UNKNOWN:
//         case BITFMT_END_GFX_FORMATS:
//         default:
//             break;
// 
//         /*    case BITFMT_A16B16G16R16:
//         *((t_uint16*)data_bits+0) = (t_uint16)((t_float)Rg * 65535. + round_offset);
//         *((t_uint16*)data_bits+1) = (t_uint16)((t_float)Gg * 65535. + round_offset);
//         *((t_uint16*)data_bits+2) = (t_uint16)((t_float)Bg * 65535. + round_offset);
//         *((t_uint16*)data_bits+3) = (t_uint16)((t_float)A * 65535. + round_offset);
//         break;
//            
//         case BITFMT_R16F:
//         {
//         *((CFloat16*)data_bits+0) = Rg;
//         }
//         break;
//            
//         case BITFMT_G16R16F:
//         {
//         *((CFloat16*)data_bits+0) = Rg;
//         *((CFloat16*)data_bits+1) = Gg;
//         }
//         break;
//            
//         case BITFMT_A16B16G16R16F:
//         {
//         *((CFloat16*)data_bits+0) = Rg;
//         *((CFloat16*)data_bits+1) = Gg;
//         *((CFloat16*)data_bits+2) = Bg;
//         *((CFloat16*)data_bits+3) = A;
//         }
//         break;
//            
//         case BITFMT_R32F:
//         {
//         *((t_float*)data_bits+0) = Rg;
//         }
//         break;
//            
//         case BITFMT_G32R32F:
//         {
//         *((t_float*)data_bits+0) = Rg;
//         *((t_float*)data_bits+1) = Gg;
//         }
//         break;
//            
//         case BITFMT_A32B32G32R32F:
//         {
//         *((t_float*)data_bits+0) = Rg;
//         *((t_float*)data_bits+1) = Gg;
//         *((t_float*)data_bits+2) = Bg;
//         *((t_float*)data_bits+3) = A;
//         }
//         break;
//         */
//     }
// }
// 
// void Color::ConvertFrom(BitmapFormat format,
//                          const char* data_bits, 
//                          bool undo_gamma_correction // (gamma 2.2 -> gamma 1.0)
//                          )
// {
//     t_uint16 u16BITS;
//     t_byte u8BITS;
// 
//     t_float R, G, B, A;
// 
//     switch (format)
//     {
//         default:
//         case BITFMT_UNKNOWN: 
//         {
//             return;
//         }
//         break;
//           
//         case BITFMT_R8G8B8A8:
//         {
//         R = *(( t_byte*)data_bits+3)/255.f;
//         G = *(( t_byte*)data_bits+2)/255.f;
//         B = *(( t_byte*)data_bits+1)/255.f;
//         A = *(( t_byte*)data_bits+0)/255.f;
//         }
//         break;
// 
//         case BITFMT_B8G8R8A8:
//         {
//         R = *(( t_byte*)data_bits+1)/255.f;
//         G = *(( t_byte*)data_bits+2)/255.f;
//         B = *(( t_byte*)data_bits+3)/255.f;
//         A = *(( t_byte*)data_bits+0)/255.f;
//         }
//         break;
// 
// 
//         case BITFMT_A8R8G8B8:
//         {
//         R = *(( t_byte*)data_bits+2)/255.f;
//         G = *(( t_byte*)data_bits+1)/255.f;
//         B = *(( t_byte*)data_bits+0)/255.f;
//         A = *(( t_byte*)data_bits+3)/255.f;
//         }
//         break;
// 
//         case BITFMT_A8B8G8R8:
//         {
//         R = *(( t_byte*)data_bits+0)/255.f;
//         G = *(( t_byte*)data_bits+1)/255.f;
//         B = *(( t_byte*)data_bits+2)/255.f;
//         A = *(( t_byte*)data_bits+3)/255.f;
//         }
//         break;
// 
//         case BITFMT_X8R8G8B8:
//         {
//         R = *(( t_byte*)data_bits+2)/255.f;
//         G = *(( t_byte*)data_bits+1)/255.f;
//         B = *(( t_byte*)data_bits+0)/255.f;
//         A = 1.f;
//         }
//         break;
// 
//         case BITFMT_X8B8G8R8:
//         {
//         R = *(( t_byte*)data_bits+0)/255.f;
//         G = *(( t_byte*)data_bits+1)/255.f;
//         B = *(( t_byte*)data_bits+2)/255.f;
//         A = 1.f;
//         }
//         break;
// 
//     //////////////////////////////////////////////////////////////////////
//         case BITFMT_A2R10G10B10:
//         {
//         t_uint32 u32BITS = *((t_uint32*)data_bits);
//         B = ((u32BITS>>(0    ))  & 0x3FF)/1023.f;
//         G = ((u32BITS>>(10   ))  & 0x3FF)/1023.f;
//         R = ((u32BITS>>(10+10))  & 0x3FF)/1023.f;
//         A = ((u32BITS>>(10+10+10)) & 0x3)/3.f;
//         }
//         break;
// 
//         case BITFMT_A2B10G10R10:
//         {
//         t_uint32 u32BITS = *((t_uint32*)data_bits);
//         R = ((u32BITS>>(0    ))  & 0x3FF)/1023.f;
//         G = ((u32BITS>>(10   ))  & 0x3FF)/1023.f;
//         B = ((u32BITS>>(10+10))  & 0x3FF)/1023.f;
//         A = ((u32BITS>>(10+10+10)) & 0x3)/3.f;
//         }
//         break;
// 
//         case BITFMT_R10G10B10A2:
//         {
//         t_uint32 u32BITS = *((t_uint32*)data_bits);
//         A = ((u32BITS>>(0    ))  & 0x3FF)/1023.f;
//         B = ((u32BITS>>(10   ))  & 0x3FF)/1023.f;
//         G = ((u32BITS>>(10+10))  & 0x3FF)/1023.f;
//         R = ((u32BITS>>(10+10+10)) & 0x3)/3.f;
//         }
//         break;
// 
//         case BITFMT_B10G10R10A2:
//         {
//         t_uint32 u32BITS = *((t_uint32*)data_bits);
//         A = ((u32BITS>>(0    ))  & 0x3FF)/1023.f;
//         R = ((u32BITS>>(10   ))  & 0x3FF)/1023.f;
//         G = ((u32BITS>>(10+10))  & 0x3FF)/1023.f;
//         B = ((u32BITS>>(10+10+10)) & 0x3)/3.f;
//         }
//         break;
// 
//         case BITFMT_R8G8B8:
//         R = *((t_byte*)data_bits+2)/255.f;
//         G = *((t_byte*)data_bits+1)/255.f;
//         B = *((t_byte*)data_bits+0)/255.f;
//         A = 1.0f;
//         break;
// 
//         case BITFMT_B8G8R8:
//         R = *((t_byte*)data_bits+0)/255.f;
//         G = *((t_byte*)data_bits+1)/255.f;
//         B = *((t_byte*)data_bits+2)/255.f;
//         A = 1.0f;
//         break;
// 
//         case BITFMT_R5G6B5:
//         u16BITS = *((t_uint16*)data_bits);
//         R = ((u16BITS>>(6+5)) & 0x001F)/31.f;
//         G = ((u16BITS>>   5) & 0x003F)/63.f;
//         B = ((u16BITS      ) & 0x001F)/31.f;
//         A = 1.0f;
//         break;
// 
//         case BITFMT_X1R5G5B5:
//         u16BITS = *((t_uint16*)data_bits);
//         R = ((u16BITS>>(5+5)) & 0x001F)/31.f;
//         G = ((u16BITS>>   5) & 0x001F)/31.f;
//         B = ((u16BITS      ) & 0x001F)/31.f;
//         A = 1.0f;
//         break;
// 
//         case BITFMT_A1R5G5B5:
//         u16BITS = *((t_uint16*)data_bits);
//         R = ((u16BITS>>(5+5)) & 0x001F)/31.f;
//         G = ((u16BITS>>   5) & 0x001F)/31.f;
//         B = ((u16BITS      ) & 0x001F)/31.f;
//         A = ( u16BITS & 0x8000 ) ? 1.f : 0.f;
//         break;
// 
//         case BITFMT_A4R4G4B4:
//         u16BITS = *((t_uint16*)data_bits);
//         R = ((u16BITS>>  (4+4)) & 0x000F)/15.f;
//         G = ((u16BITS>>    (4)) & 0x000F)/15.f;
//         B = ((u16BITS         ) & 0x000F)/15.f;
//         A = ((u16BITS>>(4+4+4)) & 0x000F)/15.f;
//         break;
// 
//         case BITFMT_X4R4G4B4:
//         u16BITS = *((t_uint16*)data_bits);
//         R = ((u16BITS>>(4+4)) & 0x000F)/15.f;
//         G = ((u16BITS>>  (4)) & 0x000F)/15.f;
//         B = ((u16BITS       ) & 0x000F)/15.f;
//         A = 1.f;
//         break;
// 
//         case BITFMT_A8R3G3B2:
//         u16BITS = *((t_uint16*)data_bits);
//         R = ((u16BITS>>(3+2)) & 0x07)/7.f;
//         G = ((u16BITS>>   2) & 0x07)/7.f;
//         B = ((u16BITS      ) & 0x03)/3.f;
//         A = (t_byte)(0xff & (u16BITS >> 8))/255.f;
//         break;
//           
//         case BITFMT_R3G3B2:
//         u8BITS = *((t_byte*)data_bits);
//         R = ((u8BITS>>(3+2)) & 0x07)/7.f;
//         G = ((u8BITS>>   2) & 0x07)/7.f;
//         B = ((u8BITS      ) & 0x03)/3.f;
//         A = 1.0f;
//         break;
//     }
//       
//     if(undo_gamma_correction)
//     {
//         Undo_Gamma_Correction(R);
//         Undo_Gamma_Correction(G);
//         Undo_Gamma_Correction(B);
//         Undo_Gamma_Correction(A);
//     } 
//       
//     red_   = R;
//     green_ = G;
//     blue_  = B;
//     alpha_ = A;
// }

Color Color::RandomColor()
{
    return Color(RandomUInt(255)/255.0f, RandomUInt(255)/255.0f, RandomUInt(255)/255.0f, RandomUInt(255)/255.0f);
}

unsigned int Color::RandomColorINT()
{
    return (RandomUInt(255) << 24) | (RandomUInt(255) << 16) | (RandomUInt(255) << 8) | RandomUInt(255);
}

Color operator + (Color color0, Color color1)
{
    Color result;
    result.SetRed   (color0.R() + color1.R());
    result.SetGreen (color0.G() + color1.G());
    result.SetBlue  (color0.B() + color1.B());
    result.SetAlpha (color0.A() + color1.A());
    return result;
}

Color operator - (Color color0, Color color1)
{
    Color result;
    result.SetRed   (color0.R() - color1.R());
    result.SetGreen (color0.G() - color1.G());
    result.SetBlue  (color0.B() - color1.B());
    result.SetRed   (color0.A() - color1.A());
    return result;
}

Color operator + (float v, Color color)
{
    Color result;
    result.SetRed   (v + color.R());
    result.SetGreen (v + color.G());
    result.SetBlue  (v + color.B());
    result.SetAlpha (v + color.A());
    return result;
}

Color operator + (Color color, float v)
{
    return v + color;
}

Color operator - (float v, Color color)
{
    Color result;
    result.SetRed   (v - color.R());
    result.SetGreen (v - color.G());
    result.SetBlue  (v - color.B());
    result.SetAlpha (v - color.A());
    return result;
}

Color operator - (Color color, float v)
{
    Color result;
    result.SetRed   (color.R() - v);
    result.SetGreen (color.G() - v);
    result.SetBlue  (color.B() - v);
    result.SetAlpha (color.A() - v);
    return result;
}

Color operator * (float v, Color color)
{
    Color result;
    result.SetRed   (v * color.R());
    result.SetGreen (v * color.G());
    result.SetBlue  (v * color.B());
    result.SetAlpha (v * color.A());
    return result;
}

Color operator * (Color color, float v)
{
    return v * color;
}


// The Hue/Saturation/Value model was created by A. R. Smith in 1978. It is based on such intuitive color characteristics as tint, 
// shade and tone (or family, purety and intensity). The coordinate system is cylindrical, and the colors are defined inside a hexcone.
// The hue value H runs from 0 to 360º. The saturation S is the degree of strength or purity and is from 0 to 1. Purity is how much white
// is added to the color, so S=1 makes the purest color (no white). Brightness V also ranges from 0 to 1, where 0 is the black.
// There is no transformation matrix for RGB/HSV conversion, but the algorithm follows:

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)

void RGBtoHSV( float r, float g, float b, float &h, float &s, float &v )
{
    float min, max, delta;

    min = Min<t_float>( Min<t_float>(r, g), b );
    max = Max<t_float>( Max<t_float>(r, g), b );
    v = max;				// v

    delta = max - min;


    if( max != 0 )
    {
        s = delta / max;		// s
    }
    else 
    {
        // MAX = 0 (i.e. if v = 0), then s is undefined.  r = g = b = 0
        s = 0;
        h = -1;
        return;
    }

    if(delta == 0) // MAX = MIN (i.e. s = 0), then h is undefined. r = g = b
    {
        h = 0;
        s = 0;
        return;
    }

    if( r == max )
        h = ( g - b ) / delta;		// between yellow & magenta
    else if( g == max )
        h = 2 + ( b - r ) / delta;	// between cyan & yellow
    else
        h = 4 + ( r - g ) / delta;	// between magenta & cyan

    h *= 60;				// degrees
    if( h < 0 )
        h += 360;

    // convert h from [0, 360] to [0, 1]
    h = (h) / 360.0f;

}

void HSVtoRGB( float &r, float &g, float &b, float h, float s, float v )
{
    int i;
    float f, p, q, t;

    // convert h from [0, 1] to [0, 360]
    h = h * 360.0f;

    if( s == 0 ) 
    {
        // achromatic (grey)
        r = g = b = v;
        return;
    }

    h /= 60;			// sector 0 to 5
    i = (int)std::floor( h );
    f = h - i;			// factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );

    switch( i ) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        default:		// case 5:
            r = v;
            g = p;
            b = q;
            break;
    }
}

/////////////////////////
// HLS-RGB Conversions //
/////////////////////////

float HLStoRGB1(float rn1, float rn2, float huei)
{
    // Static method. Auxiliary to HLS2RGB().

    float hue = huei;
    if (hue > 360) hue = hue - 360;
    if (hue < 0)   hue = hue + 360;
    if (hue < 60 ) return rn1 + (rn2-rn1)*hue/60;
    if (hue < 180) return rn2;
    if (hue < 240) return rn1 + (rn2-rn1)*(240-hue)/60;
    return rn1;
}

void HLStoRGB(float &r, float &g, float &b, float hue, float light, float satur)
{
    // Static method to compute RGB from HLS. The l and s are between [0,1]
    // and h is between [0, 1]. The returned r,g,b triplet is between [0,1].
    hue *= 360.0f;

    float rh, rl, rs, rm1, rm2;
    rh = rl = rs = 0;
    if (hue   > 0) rh = hue;   if (rh > 360) rh = 360;
    if (light > 0) rl = light; if (rl > 1)   rl = 1;
    if (satur > 0) rs = satur; if (rs > 1)   rs = 1;

    if (rl <= 0.5f)
        rm2 = rl*(1.0f + rs);
    else
        rm2 = rl + rs - rl*rs;
    rm1 = 2.0f*rl - rm2;

    if (!rs) { r = rl; g = rl; b = rl; return; }
    r = HLStoRGB1(rm1, rm2, rh+120);
    g = HLStoRGB1(rm1, rm2, rh);
    b = HLStoRGB1(rm1, rm2, rh-120);
}

void HLStoRGBi(int h, int l, int s, int &r, int &g, int &b)
{
    // Static method to compute RGB from HLS. The h,l,s are between [0,255].
    // The returned r,g,b triplet is between [0,255].

    float hh, ll, ss;
    float rr, gg, bb;
    hh = ll = ss = 0;
    rr = gg = bb = 0;

    hh = float(h) * 360 / 255;
    ll = float(l) / 255;
    ss = float(s) / 255;

    HLStoRGB(hh, ll, ss, rr, gg, bb);

    r = (int) (rr * 255);
    g = (int) (gg * 255);
    b = (int) (bb * 255);
}

void RGBtoHLS(float rr, float gg, float bb,
              float &hue, float &light, float &satur)
{
    // Static method to compute HLS from RGB. The r,g,b triplet is between
    // [0,1], hue is between [0,1], light and satur are [0,1].

    float rnorm, gnorm, bnorm, minval, maxval, msum, mdiff, r, g, b;
    r = g = b = 0;
    if (rr > 0) r = rr; if (r > 1) r = 1;
    if (gg > 0) g = gg; if (g > 1) g = 1;
    if (bb > 0) b = bb; if (b > 1) b = 1;

    minval = r;
    if (g < minval) minval = g;
    if (b < minval) minval = b;
    maxval = r;
    if (g > maxval) maxval = g;
    if (b > maxval) maxval = b;

    rnorm = gnorm = bnorm = 0;
    mdiff = maxval - minval;
    msum  = maxval + minval;
    light = 0.5f * msum;
    if (maxval != minval) {
        rnorm = (maxval - r)/mdiff;
        gnorm = (maxval - g)/mdiff;
        bnorm = (maxval - b)/mdiff;
    } else {
        satur = hue = 0;
        return;
    }

    if (light < 0.5f)
        satur = mdiff/msum;
    else
        satur = mdiff/(2.0f - msum);

    if (r == maxval)
        hue = 60.0f * (6.0f + bnorm - gnorm);
    else if (g == maxval)
        hue = 60.0f * (2.0f + rnorm - bnorm);
    else
        hue = 60.0f * (4.0f + gnorm - rnorm);

    if (hue > 360)
        hue = hue - 360;

    hue = hue/360.0f;
}


void RGBtoHLSi(int r, int g, int b, int &h, int &l, int &s)
{
    // Static method to compute HLS from RGB. The r,g,b triplet is between
    // [0,255], hue, light and satur are between [0,255].

    float rr, gg, bb, hue, light, satur;

    rr = float(r) / 255;
    gg = float(g) / 255;
    bb = float(b) / 255;

    RGBtoHLS(rr, gg, bb, hue, light, satur);

    h = (int) (hue/360 * 255);
    l = (int) (light * 255);
    s = (int) (satur * 255);
}

NAMESPACE_END
