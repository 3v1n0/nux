/*
 * Copyright 2010-2012 Inalogic® Inc.
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
 *              Robert Carr <racarr@canonical.com>
 */

#ifndef RENDERING_PIPE_TEXTURE_BLEND_SHADER_SOURCE_H
#define RENDERING_PIPE_TEXTURE_BLEND_SHADER_SOURCE_H

namespace nux
{
static const std::string BlendNormalShader = "                                                \n\
vec3 BlendNormal(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  return L;                                                                                   \n\
}";

static const std::string BlendLightenShader = "                                               \n\
vec3 BlendLighten(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  // This mode is commutative                                                                 \n\
  vec3 V = max(L, B);                                                                         \n\
  return V;                                                                                   \n\
}";

static const std::string BlendDarkenShader = "                                                \n\
vec3 BlendDarken(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  vec3 V = min(L, B);                                                                         \n\
  return V;                                                                                   \n\
}";

static const std::string BlendMultiplyShader = "                                              \n\
vec3 BlendMultiply(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                  \n\
{                                                                                             \n\
  return(B * L);                                                                              \n\
}";

static const std::string BlendAverageShader = "                                               \n\
vec3 BlendAverage(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  // This mode is commutative                                                                 \n\
  return((B + L) / 2.0);                                                                      \n\
}";

  // *** Additive Modes ***
static const std::string BlendAddShader = "                                                   \n\
vec3 BlendAdd(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                       \n\
{                                                                                             \n\
  return min(B + L, vec3(1.0));                                                               \n\
}";

static const std::string BlendSubstractShader = "                                             \n\
vec3 BlendSubstract(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                 \n\
{                                                                                             \n\
  return max(B + L - vec3(1.0), vec3(0.0));                                                   \n\
}";

// *** Difference Modes ***
static const std::string BlendDifferenceShader = "                                            \n\
vec3 BlendDifference(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                \n\
{                                                                                             \n\
  return abs(B - L);                                                                          \n\
}";

static const std::string BlendNegationShader = "                                              \n\
vec3 BlendNegation(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                  \n\
{                                                                                             \n\
  return(vec3(1.0) - abs(vec3(1.0) - B - L));                                                 \n\
}";

static const std::string BlendExclusionShader = "                                             \n\
vec3 BlendExclusion(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                 \n\
{                                                                                             \n\
  return(B + L - 2.0 * B * L);                                                                \n\
}";

static const std::string BlendScreenShader = "                                                \n\
vec3 BlendScreen(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  return(1.0 - (1.0 - B) * (1.0 - L));                                                        \n\
}";

static const std::string BlendOverlayShader = "                                               \n\
float _BlendOverlay(float B /*background layer*/, float L /*foreground layer*/)               \n\
{                                                                                             \n\
  if (L < 0.5)                                                                                \n\
    return(2.0 * B * L);                                                                      \n\
  else                                                                                        \n\
    return(1.0 - 2.0 * (1.0 - B) * (1.0 - L));                                                \n\
}                                                                                             \n\
vec3 BlendOverlay(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  return vec3(_BlendOverlay(B.r,L.r), _BlendOverlay(B.g ,L.g), _BlendOverlay(B.b,L.b));       \n\
}";

static const std::string BlendSoftLightShader = "                                                   \n\
float _BlendSoftLight(float B /*background layer*/, float L /*foreground layer*/)                   \n\
{                                                                                                   \n\
  if (L < 0.5)                                                                                      \n\
    return(2.0 * B * L + B * B * (1.0 - 2.0 * L));                                                  \n\
  else                                                                                              \n\
    return sqrt(B) * (2.0 * L - 1.0) + 2.0 * B * (1.0 - L);                                         \n\
}                                                                                                   \n\
vec3 BlendSoftLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                       \n\
{                                                                                                   \n\
  return vec3(_BlendSoftLight(B.r, L.r), _BlendSoftLight(B.g, L.g), _BlendSoftLight(B.b, L.b));     \n\
}";

static const std::string BlendHardLightShader = "                                                   \n\
float _BlendHardLight(float B /*background layer*/, float L /*foreground layer*/)                   \n\
{                                                                                                   \n\
  if (L < 0.5)                                                                                      \n\
    return(2.0 * B * L);                                                                            \n\
  else                                                                                              \n\
    return(1.0 - 2.0 * (1.0 - B) * (1.0 - L));                                                      \n\
}                                                                                                   \n\
vec3 BlendHardLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                       \n\
{                                                                                                   \n\
  return vec3(_BlendHardLight(B.r, L.r), _BlendHardLight(B.g, L.g), _BlendHardLight(B.b, L.b));     \n\
}";

static const std::string BlendColorDodgeShader = "                                                    \n\
float _BlendColorDodge(float B /*background layer*/, float L /*foreground layer*/)                    \n\
{                                                                                                     \n\
  if (L == 1.0)                                                                                       \n\
    return 1.0;                                                                                       \n\
  else                                                                                                \n\
    return min(B / (1.0 - L), 1.0);                                                                   \n\
}                                                                                                     \n\
vec3 BlendColorDodge(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                        \n\
{                                                                                                     \n\
  return vec3(_BlendColorDodge(B.r, L.r), _BlendColorDodge(B.g, L.g), _BlendColorDodge(B.b, L.b));    \n\
}";

static const std::string BlendLinearDodgeShader = "                                                       \n\
float _BlendLinearDodge(float B /*background layer*/, float L /*foreground layer*/)                       \n\
{                                                                                                         \n\
  return min(B + L, 1.0);                                                                                 \n\
}                                                                                                         \n\
vec3 BlendLinearDodge(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  return vec3(_BlendLinearDodge(B.r, L.r), _BlendLinearDodge(B.g, L.g), _BlendLinearDodge(B.b, L.b));     \n\
}";

static const std::string BlendColorBurnShader = "                                                         \n\
float _BlendColorBurn(float B /*background layer*/, float L /*foreground layer*/)                         \n\
{                                                                                                         \n\
  if (L == 0.0)                                                                                           \n\
    return 0.0;                                                                                           \n\
  else                                                                                                    \n\
    return max(1.0 - ((1.0 - B) / L), 0.0);                                                               \n\
}                                                                                                         \n\
vec3 BlendColorBurn(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                             \n\
{                                                                                                         \n\
  return vec3(_BlendColorBurn(B.r, L.r), _BlendColorBurn(B.g, L.g), _BlendColorBurn(B.b, L.b));           \n\
}";

static const std::string BlendLinearBurnShader = "                                                        \n\
float _BlendLinearBurn(float B /*background layer*/, float L /*foreground layer*/)                        \n\
{                                                                                                         \n\
  return max(B + L - 1.0, 0.0);                                                                           \n\
}                                                                                                         \n\
vec3 BlendLinearBurn(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                            \n\
{                                                                                                         \n\
  return vec3(_BlendLinearBurn(B.r, L.r), _BlendLinearBurn(B.g, L.g), _BlendLinearBurn(B.b, L.b));        \n\
}";

static const std::string BlendLinearLightShader = BlendLinearBurnShader + BlendLinearDodgeShader +
"float _BlendLinearLight(float B /*background layer*/, float L /*foreground layer*/)                      \n\
{                                                                                                         \n\
  if (L < 0.5)                                                                                            \n\
    return _BlendLinearBurn(B, (2.0 * L));                                                                \n\
  else                                                                                                    \n\
    return _BlendLinearDodge(B, (2.0 * (L - 0.5)));                                                       \n\
}                                                                                                         \n\
vec3 BlendLinearLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  return vec3(_BlendLinearLight(B.r, L.r), _BlendLinearLight(B.g, L.g), _BlendLinearLight(B.b, L.b));     \n\
}";

static const std::string BlendVividLightShader = BlendColorBurnShader + BlendColorDodgeShader +
"float _BlendVividLight(float B /*background layer*/, float L /*foreground layer*/)                       \n\
{                                                                                                         \n\
  if (L < 0.5)                                                                                            \n\
    return _BlendColorBurn(B, (2.0 * L));                                                                 \n\
  else                                                                                                    \n\
    return _BlendColorDodge(B, (2.0 * (L - 0.5)));                                                        \n\
}                                                                                                         \n\
vec3 BlendVividLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                            \n\
{                                                                                                         \n\
  return vec3(_BlendVividLight(B.r, L.r), _BlendVividLight(B.g, L.g), _BlendVividLight(B.b, L.b));        \n\
}";

static const std::string BlendPinLightShader = "                                                          \n\
float _BlendPinLight(float B /*background layer*/, float L /*foreground layer*/)                          \n\
{                                                                                                         \n\
  if (L < 0.5)                                                                                            \n\
    return min(B, (2.0 * L));                                                                             \n\
  else                                                                                                    \n\
    return max(B, (2.0 * (L - 0.5)));                                                                     \n\
}                                                                                                         \n\
vec3 BlendPinLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                              \n\
{                                                                                                         \n\
  return vec3(_BlendPinLight(B.r, L.r), _BlendPinLight(B.g, L.g), _BlendPinLight(B.b, L.b));              \n\
}";

static const std::string BlendHardMixShader = BlendVividLightShader +
"float _BlendHardMix(float B /*background layer*/, float L /*foreground layer*/)                          \n\
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

static const std::string BlendReflectShader = "                                                           \n\
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

static const std::string BlendGlowShader = "                                                              \n\
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

static const std::string BlendPhoenixShader = "                                                           \n\
vec3 BlendPhoenix(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return min(B, L) - max(B, L) + vec3(1.0);                                                               \n\
}";

static const std::string BlendOpacityShader = "                                                           \n\
vec3 BlendOpacity(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return O * L + (1 - O) * B;                                                                             \n\
}";

}

#endif
