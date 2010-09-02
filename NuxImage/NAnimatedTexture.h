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


#ifndef NANIMATEDTEXTURE_H
#define NANIMATEDTEXTURE_H

NAMESPACE_BEGIN

NBitmapData* LoadAnimatedTextureFromXML(const char* XMLFile);
bool SaveAnimatedTextureFile(NBitmapData* pAnimatedTexture, const TCHAR* Filename);
NBitmapData* LoadAnimatedTextureFile(const TCHAR* Filename);

NAMESPACE_END

#endif // NANIMATEDTEXTURE_H
