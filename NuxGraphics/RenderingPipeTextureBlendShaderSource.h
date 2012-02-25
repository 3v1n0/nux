#ifndef __RENDERING_PIPE_TEXTURE_BLEND_SHADER_SOURCE
#define __RENDERING_PIPE_TEXTURE_BLEND_SHADER_SOURCE
  // For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
  // other attributes. Otherwise you get a bug on NVidia! Why is that???
  // [Update]: it seems that the vertex attributes have to be declared in alphabetic order in the shader. It does not matter if the vertex
  // attribute is declared first in the alphabetic order.

  // For some other strange reason, on Intel GMA, the order in which attributes are used in the vertex shaders, is the order used to associated them with a
  // and attribute location. One has to make sure that the vertex attribute get index 0. So use the vertex attribute first. All of this does not make any sense.
  // Need more info from driver developers.

  static const char * const BlendNormalShader = "                                          \n\
vec3 BlendNormal(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  return L;                                                                                   \n\
}";

  static const char * const BlendLightenShader = "                                         \n\
vec3 BlendLighten(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  // This mode is commutative                                                                 \n\
  vec3 V = max(L, B);                                                                         \n\
  return V;                                                                                   \n\
}";

  static const char * const BlendDarkenShader = "                                          \n\
vec3 BlendDarken(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  vec3 V = min(L, B);                                                                         \n\
  return V;                                                                                   \n\
}";

  static const char * const BlendMultiplyShader = "                                        \n\
vec3 BlendMultiply(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                  \n\
{                                                                                             \n\
  return(B * L);                                                                              \n\
}";

  static const char * const BlendAverageShader = "                                         \n\
vec3 BlendAverage(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  // This mode is commutative                                                                 \n\
  return((B + L) / 2.0);                                                                      \n\
}";

  // *** Additive Modes ***
  static const char * const BlendAddShader = "                                             \n\
vec3 BlendAdd(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                       \n\
{                                                                                             \n\
  return min(B + L, vec3(1.0));                                                               \n\
}";

  static const char * const BlendSubstractShader = "                                       \n\
vec3 BlendSubstract(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                 \n\
{                                                                                             \n\
  return max(B + L - vec3(1.0), vec3(0.0));                                                   \n\
}";

// *** Difference Modes ***
static const char * const BlendDifferenceShader = "                                      \n\
vec3 BlendDifference(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                \n\
{                                                                                             \n\
  return abs(B - L);                                                                          \n\
}";

static const char * const BlendNegationShader = "                                        \n\
vec3 BlendNegation(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                  \n\
{                                                                                             \n\
  return(vec3(1.0) - abs(vec3(1.0) - B - L));                                                 \n\
}";

static const char * const BlendExclusionShader = "                                       \n\
vec3 BlendExclusion(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                 \n\
{                                                                                             \n\
  return(B + L - 2.0 * B * L);                                                                \n\
}";

static const char * const BlendScreenShader = "                                          \n\
vec3 BlendScreen(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  return(1.0 - (1.0 - B) * (1.0 - L));                                                        \n\
}";

static const char * const BlendOverlayShader = "                                         \n\
float _BlendOverlay(float B /*background layer*/, float L /*foreground layer*/)               \n\
{                                                                                             \n\
  if (L < 0.5)                                                                                \n\
    return(2.0 * B * L);                                                                      \n\
  else                                                                                        \n\
    return(1.0 - 2.0 * (1.0 - B) * (1.0 - L));                                                \n\
}                                                                                             \n\
vec3 BlendOverlay(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  return vec3(_BlendOverlay(B.r,L.r), _BlendOverlay(B.g ,L.g), _BlendOverlay(B.b,L.b));  \n\
}";

static const char * const BlendSoftLightShader = "                                             \n\
float _BlendSoftLight(float B /*background layer*/, float L /*foreground layer*/)                   \n\
{                                                                                                   \n\
  if (L < 0.5)                                                                                      \n\
    return(2.0 * B * L + B * B * (1.0 - 2.0 * L);                                                   \n\
  else                                                                                              \n\
    return sqrt(B) * (2.0 * L - 1.0) + 2.0 * B * (1.0 - L);                                         \n\
}                                                                                                   \n\
vec3 BlendSoftLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                       \n\
{                                                                                                   \n\
  return vec3(_BlendSoftLight(B.r, L.r), _BlendSoftLight(B.g, L.g), _BlendSoftLight(B.b, L.b));  \n\
}";

  static const char * const BlendHardLightShader = "                                             \n\
float _BlendHardLight(float B /*background layer*/, float L /*foreground layer*/)                   \n\
{                                                                                                   \n\
  if (L < 0.5)                                                                                      \n\
    return(2.0 * B * L);                                                                            \n\
  else                                                                                              \n\
    return(1.0 - 2.0 * (1.0 - B) * (1.0 - L));                                                      \n\
}                                                                                                   \n\
vec3 BlendHardLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                       \n\
{                                                                                                   \n\
  return vec3(_BlendHardLight(B.r, L.r), _BlendHardLight(B.g, L.g), _BlendHardLight(B.b, L.b));  \n\
}";

  static const char * const BlendColorDodgeShader = "                                              \n\
float _BlendColorDodge(float B /*background layer*/, float L /*foreground layer*/)                    \n\
{                                                                                                     \n\
  if (L == 1.0)                                                                                       \n\
    return 1.0;                                                                                       \n\
  else                                                                                                \n\
    return min(B / (1.0 - L), 1.0);                                                                   \n\
}                                                                                                     \n\
vec3 BlendColorDodge(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                        \n\
{                                                                                                     \n\
  return vec3(_BlendColorDodge(B.r, L.r), _BlendColorDodge(B.g, L.g), _BlendColorDodge(B.b, L.b)); \n\
}";

  static const char * const BlendLinearDodgeShader = "                                                 \n\
float _BlendLinearDodge(float B /*background layer*/, float L /*foreground layer*/)                       \n\
{                                                                                                         \n\
  return min(B + L, 1.0);                                                                                 \n\
}                                                                                                         \n\
vec3 BlendLinearDodge(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  return vec3(_BlendLinearDodge(B.r, L.r), _BlendLinearDodge(B.g, L.g), _BlendLinearDodge(B.b, L.b));  \n\
}";

  static const char * const BlendColorBurnShader = "                                                   \n\
float _BlendColorBurn(float B /*background layer*/, float L /*foreground layer*/)                         \n\
{                                                                                                         \n\
  if (L == 0.0)                                                                                           \n\
    return 0.0;                                                                                           \n\
  else                                                                                                    \n\
    return max(1.0 - ((1.0 - B) / L), 0.0);                                                               \n\
}                                                                                                         \n\
vec3 BlendColorBurn(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                             \n\
{                                                                                                         \n\
  return vec3(_BlendColorBurn(B.r, L.r), _BlendColorBurn(B.g, L.g), _BlendColorBurn(B.b, L.b));        \n\
}";

  static const char * const BlendLinearBurnShader = "                                                  \n\
float _BlendLinearBurn(float B /*background layer*/, float L /*foreground layer*/)                        \n\
{                                                                                                         \n\
  return max(B + L - 1.0, 0.0);                                                                           \n\
}                                                                                                         \n\
vec3 BlendLinearBurn(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                            \n\
{                                                                                                         \n\
  return vec3(_BlendLinearBurn(B.r, L.r), _BlendLinearBurn(B.g, L.g), _BlendLinearBurn(B.b, L.b));     \n\
}";

  static const char * const BlendLinearLightShader = "                                                 \n\
float _BlendLinearLight(float B /*background layer*/, float L /*foreground layer*/)                       \n\
{                                                                                                         \n\
  if (L < 0.5)                                                                                            \n\
    return _BlendLinearBurn(B, (2.0 * L));                                                                \n\
  else                                                                                                    \n\
    return _BlendLinearDodge(B, (2.0 * (L - 0.5)));                                                       \n\
}                                                                                                         \n\
vec3 BlendLinearLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  return vec3(_BlendLinearLight(B.r, L.r), _BlendLinearLight(B.g, L.g), _BlendLinearLight(B.b, L.b));  \n\
}";

  static const char * const BlendVividLightShader = "                                                  \n\
float _BlendVividLight(float B /*background layer*/, float L /*foreground layer*/)                        \n\
{                                                                                                         \n\
  if (L < 0.5)                                                                                            \n\
    return _BlendColorBurn(B, (2.0 * L));                                                                 \n\
  else                                                                                                    \n\
    return _BlendColorDodge(B, (2.0 * (L - 0.5))));                                                       \n\
}                                                                                                         \n\
vec3 BlendVividLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                            \n\
{                                                                                                         \n\
  return vec3(_BlendVividLight(B.r, L.r), _BlendVividLight(B.g, L.g), _BlendVividLight(B.b, L.b));        \n\
}";

static const char * const BlendPinLightShader = "                                                    \n\
float _BlendPinLight(float B /*background layer*/, float L /*foreground layer*/)                          \n\
{                                                                                                         \n\
  if (L < 0.5)                                                                                            \n\
    return _BlendDarken(B, (2.0 * L));                                                                    \n\
  else                                                                                                    \n\
    return _BlendLighten(B, (2.0 * (L - 0.5))));                                                          \n\
}                                                                                                         \n\
vec3 BlendPinLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                              \n\
{                                                                                                         \n\
  return vec3(_BlendPinLight(B.r, L.r), _BlendPinLight(B.g, L.g), _BlendPinLight(B.b, L.b));              \n\
}";

static const char * const BlendHardMixShader = "                                                     \n\
float _BlendHardMix(float B /*background layer*/, float L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  if (_BlendVividLight(B, L) < 0.5)                                                                       \n\
    return 0.0;                                                                                           \n\
  else                                                                                                    \n\
    return 1.0;                                                                                           \n\
}                                                                                                         \n\
vec3 BlendHardMix(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return vec3(_BlendHardMix(B.r, L.r), _BlendHardMix(B.g, L.g), _BlendHardMix(B.b, L.b));                 \n\
}";

static const char * const BlendReflectShader = "                                                     \n\
float _BlendReflect(float B /*background layer*/, float L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  if (L == 1.0)                                                                                           \n\
    return 1.0;                                                                                           \n\
  else                                                                                                    \n\
    return min(B * B / (1.0 - L), 1.0);                                                                   \n\
}                                                                                                         \n\
vec3 BlendReflect(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return vec3(_BlendReflect(B.r, L.r), _BlendReflect(B.g, L.g), _BlendReflect(B.b, L.b));                 \n\
}";

static const char * const BlendGlowShader = "                                                        \n\
float _BlendGlow(float B /*background layer*/, float L /*foreground layer*/)                              \n\
{                                                                                                         \n\
  if (L == 1.0)                                                                                           \n\
    return 1.0;                                                                                           \n\
  else                                                                                                    \n\
    return min(L * L / (1.0 - B), 1.0);                                                                   \n\
}                                                                                                         \n\
vec3 BlendGlow(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                                  \n\
{                                                                                                         \n\
  return vec3(_BlendGlow(B.r, L.r), _BlendGlow(B.g, L.g), _BlendGlow(B.b, L.b));                          \n\
}";

static const char * const BlendPhoenixShader = "                                                     \n\
vec3 BlendPhoenix(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return min(B, L) - max(B, L) + vec3(1.0);                                                               \n\
}";

static const char * const BlendOpacityShader = "                                                     \n\
vec3 BlendOpacity(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return O * L + (1 - O) * B;                                                                             \n\
}";

#ifndef NUX_OPENGLES_20
#define VertexShaderHeader "#version 110 \n"
#else
#define VertexShaderHeader
#endif

#ifndef NUX_OPENGLES_20
#define FragmentShaderHeader "#version 110 \n"
#else
#define FragmentShaderHeader "precision mediump float; \n"
#endif


#endif
