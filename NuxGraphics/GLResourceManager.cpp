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


#include "GLResource.h"
#include "IOpenGLResource.h"
#include "GLResourceManager.h"
#include "GraphicsEngine.h"
#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"

namespace nux
{

  extern PixelFormatInfo GPixelFormats[];

  NUX_IMPLEMENT_OBJECT_TYPE(ResourceData);
  NUX_IMPLEMENT_OBJECT_TYPE(CachedResourceData);

  ResourceData::ResourceData(NUX_FILE_LINE_DECL)
    : Object(true, NUX_FILE_LINE_PARAM)
    , m_ResourceIndex(NUX_INVALID_INDEX)
  {
#define GET_UNIQUE_RESOURCE_INDEX NUX_GLOBAL_OBJECT_INSTANCE(UniqueIndex)
    m_ResourceIndex = GET_UNIQUE_RESOURCE_INDEX.GetUniqueIndex();
#undef GET_UNIQUE_RESOURCE_INDEX
  }

  ResourceData::~ResourceData()
  {
    // If the windows thread is deleted before any texture type resource, the
    // graphics display will not exist.
    GraphicsDisplay* display = GetGraphicsDisplay();
    if (display)
      display->GetGraphicsEngine()->FlushCachedResourceData(this);
  }

  int ResourceData::GetResourceIndex() const
  {
    return m_ResourceIndex;
  }

  NResourceSet::NResourceSet()
    : FirstResource( NULL )
  {
    if ( 1 )
    {

    }
  }

  CachedResourceData::CachedResourceData(NResourceSet *ResourceManager)
    :   Set(ResourceManager)
    ,   _cached(0)
    ,   NumRefs(0)
    ,   ResourceType(NULL)
    ,   Size(0)
    ,   UpdateHint(RUH_Static)
  {
    PrevResource = NULL;
    NextResource = Set->FirstResource;
    Set->FirstResource = this;

    if (NextResource)
    {
      NextResource->PrevResource = this;
    }
  }

  CachedResourceData::~CachedResourceData()
  {
    if (PrevResource)
    {
      PrevResource->NextResource = NextResource;
    }
    else
    {
      Set->FirstResource = NextResource;
    }

    if (NextResource)
    {
      NextResource->PrevResource = PrevResource;
    }

    if (_cached)
    {
      Set->FlushResource(this);
      _cached = false;
    }
  }

  void NResourceCache::InitializeResourceFactories()
  {
    // Define the factory pair ResourceData - CachedResourceData
#define NUX_DEFINE_RESOURCE_FACTORY_PAIR(SourceTypeName, ResourceTypeName) \
    static TGLResourceFactory<SourceTypeName, ResourceTypeName> Factory##SourceTypeName(&SourceTypeName::StaticObjectType); \
    GetResourceFactories().push_back(&Factory##SourceTypeName);

    NUX_DEFINE_RESOURCE_FACTORY_PAIR(Texture2D, CachedTexture2D);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR(TextureRectangle, CachedTextureRectangle);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR(TextureCube, CachedTextureCube);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR(TextureVolume, CachedTextureVolume);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR(TextureFrameAnimation, CachedTextureFrameAnimation);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR(VertexBuffer, CachedVertexBuffer);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR(IndexBuffer, CachedIndexBuffer);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR(VertexDeclaration, CachedVertexDeclaration);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR(MeshBuffer, CachedMeshBuffer);

#undef NUX_DEFINE_RESOURCE_FACTORY_PAIR

    // Define the factory updater for and ResourceData
#define NUX_DEFINE_RESOURCE_UPDATER(SourceTypeName) \
    static NResourceUpdater Updater##SourceTypeName(&SourceTypeName::StaticObjectType); \
    GetResourceUpdaters().push_back(&Updater##SourceTypeName);

    NUX_DEFINE_RESOURCE_UPDATER(Texture2D);
    NUX_DEFINE_RESOURCE_UPDATER(TextureRectangle);
    NUX_DEFINE_RESOURCE_UPDATER(TextureCube);
    NUX_DEFINE_RESOURCE_UPDATER(TextureVolume);
    NUX_DEFINE_RESOURCE_UPDATER(TextureFrameAnimation);
    NUX_DEFINE_RESOURCE_UPDATER(VertexBuffer);
    NUX_DEFINE_RESOURCE_UPDATER(IndexBuffer);
    NUX_DEFINE_RESOURCE_UPDATER(VertexDeclaration);
    NUX_DEFINE_RESOURCE_UPDATER(MeshBuffer);

#undef NUX_DEFINE_RESOURCE_UPDATER
  }

  ObjectPtr<CachedResourceData> NResourceCache::GetCachedResource(ResourceData *Source)
  {
    NUX_RETURN_VALUE_IF_NULL(Source, ObjectPtr<CachedResourceData>(0));
    
    // check to see if it already exists
    ObjectPtr<CachedResourceData> CachedResource = TResourceCache<int, CachedResourceData>::FindCachedResourceById(Source->GetResourceIndex());

    if (CachedResource.IsNull())
    {
      // iterate over the list of factory types
      for (unsigned int i = 0; i < GetResourceFactories().size(); ++i)
      {
        NResourceFactory *ResourceFactory = GetResourceFactories() [i];

        // check if the factory is valid for the source resource type
        if (ResourceFactory->BuildsThisResource(Source))
        {
          // cache the device resource
          CachedResourceData* ptr = ResourceFactory->BuildResource(this, Source);
          CachedResource = ObjectPtr<CachedResourceData> (ptr);
          ptr->UnReference();
          break;
        }
      }

      // make sure the result is valid
      if (CachedResource.IsNull())
      {
        nuxError("Cannot cache resource type %s", Source->Type().name );
      }
      else
      {
        // Get resource type
        CachedResource->ResourceType	= & (Source->Type());
        // add it to the pool of cached resources
        AddCachedResource(Source->GetResourceIndex(), CachedResource);
      }
    }

    return CachedResource;
  }

  bool NResourceCache::IsCachedResource(ResourceData *Source)
  {
    ObjectPtr<CachedResourceData> CachedResource = TResourceCache< int, CachedResourceData >::FindCachedResourceById(Source->GetResourceIndex());
    return(CachedResource.IsValid());
  }

}
