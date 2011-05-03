/*
 * Copyright 2011 Inalogic® Inc.
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
 * Authored by: Travis Watkins <travis.watkins@linaro.org>
 *
 */

// A series of defines to map GL functions to GLES functions

#ifndef OPENGLMAPPING_H
#define OPENGLMAPPING_H

#define glUniform1iARB glUniform1i

#define glUniform1fARB glUniform1f
#define glUniform2fARB glUniform2f
#define glUniform3fARB glUniform3f
#define glUniform4fARB glUniform4f

#define glUniform1fvARB glUniform1fv
#define glUniform2fvARB glUniform2fv
#define glUniform3fvARB glUniform3fv
#define glUniform4fvARB glUniform4fv

#define glUniformMatrix2fvARB glUniformMatrix2fv
#define glUniformMatrix3fvARB glUniformMatrix3fv
#define glUniformMatrix4fvARB glUniformMatrix4fv

#endif // OPENGLMAPPING_H

