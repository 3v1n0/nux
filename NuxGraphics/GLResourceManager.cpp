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


#include "GLResource.h"
#include "IOpenGLResource.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"

namespace nux
{

  extern PixelFormatInfo GPixelFormats[];

  NUX_IMPLEMENT_OBJECT_TYPE (ResourceData);
  NUX_IMPLEMENT_OBJECT_TYPE (CachedResourceData);

  ResourceData::ResourceData()
    :   m_ResourceIndex (NUX_INVALID_INDEX)
  {
#define GET_UNIQUE_RESOURCE_INDEX NUX_GLOBAL_OBJECT_INSTANCE(UniqueIndex)
    m_ResourceIndex = GET_UNIQUE_RESOURCE_INDEX.GetUniqueIndex();
#undef GET_UNIQUE_RESOURCE_INDEX
  }

  int ResourceData::GetResourceIndex() const
  {
    return m_ResourceIndex;
  }

  NResourceSet::NResourceSet()
    : FirstResource ( NULL )
  {
    if ( 1 )
    {

    }
  }

  CachedResourceData::CachedResourceData (NResourceSet *ResourceManager)
    :   Set (ResourceManager)
    ,   Cached (0)
    ,   NumRefs (0)
    ,   ResourceType (NULL)
    ,   Size (0)
    ,   UpdateHint (RUH_Static)
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

    if (Cached)
    {
      Set->FlushResource (this);
    }
  }

  void NResourceCache::InitializeResourceFactories()
  {
    // Define the factory pair ResourceData - CachedResourceData
#define NUX_DEFINE_RESOURCE_FACTORY_PAIR(SourceTypeName, ResourceTypeName) \
    static TGLResourceFactory<SourceTypeName, ResourceTypeName> Factory##SourceTypeName(&SourceTypeName::StaticObjectType); \
    GetResourceFactories().push_back(&Factory##SourceTypeName);

    NUX_DEFINE_RESOURCE_FACTORY_PAIR (Texture2D, CachedTexture2D);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR (TextureRectangle, CachedTextureRectangle);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR (TextureCube, CachedTextureCube);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR (TextureVolume, CachedTextureVolume);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR (TextureFrameAnimation, CachedTextureFrameAnimation);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR (NVertexBuffer, NGLVertexBuffer);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR (NIndexBuffer, NGLIndexBuffer);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR (NVertexDeclaration, NGLVertexDeclaration);
    NUX_DEFINE_RESOURCE_FACTORY_PAIR (NStaticMesh, NGLStaticMesh);

#undef NUX_DEFINE_RESOURCE_FACTORY_PAIR

    // Define the factory updater for and ResourceData
#define NUX_DEFINE_RESOURCE_UPDATER(SourceTypeName) \
    static NResourceUpdater Updater##SourceTypeName(&SourceTypeName::StaticObjectType); \
    GetResourceUpdaters().push_back(&Updater##SourceTypeName);

    NUX_DEFINE_RESOURCE_UPDATER (Texture2D);
    NUX_DEFINE_RESOURCE_UPDATER (TextureRectangle);
    NUX_DEFINE_RESOURCE_UPDATER (TextureCube);
    NUX_DEFINE_RESOURCE_UPDATER (TextureVolume);
    NUX_DEFINE_RESOURCE_UPDATER (TextureFrameAnimation);
    NUX_DEFINE_RESOURCE_UPDATER (NVertexBuffer);
    NUX_DEFINE_RESOURCE_UPDATER (NIndexBuffer);
    NUX_DEFINE_RESOURCE_UPDATER (NVertexDeclaration);
    NUX_DEFINE_RESOURCE_UPDATER (NStaticMesh);

#undef NUX_DEFINE_RESOURCE_UPDATER
  }

  IntrusiveSP<CachedResourceData> NResourceCache::GetCachedResource (ResourceData *Source)
  {
    // check to see if it already exists
    IntrusiveSP<CachedResourceData> CachedResource = TResourceCache<int, CachedResourceData>::FindCachedResourceById (Source->GetResourceIndex());

    if (CachedResource.IsNull() )
    {
      // iterate over the list of factory types
      for (t_u32 i = 0; i < GetResourceFactories().size(); ++i)
      {
        NResourceFactory *ResourceFactory = GetResourceFactories() [i];

        // check if the factory is valid for the source resource type
        if (ResourceFactory->BuildsThisResource (Source) )
        {
          // cache the device resource
          CachedResourceData* ptr = ResourceFactory->BuildResource (this, Source);
          CachedResource = IntrusiveSP<CachedResourceData> (ptr);
          ptr->UnReference();
          break;
        }
      }

      // make sure the result is valid
      if (CachedResource.IsNull() )
      {
        nuxError (TEXT ("Cannot cache resource type %s"), Source->Type().GetName() );
      }
      else
      {
        // Get resource type
        CachedResource->ResourceType	= & (Source->Type() );
        // add it to the pool of cached resources
        AddCachedResource (Source->GetResourceIndex(), CachedResource);
      }
    }

    return CachedResource;
  }

  bool NResourceCache::IsCachedResource (ResourceData *Source)
  {
    IntrusiveSP<CachedResourceData> CachedResource = TResourceCache< int, CachedResourceData >::FindCachedResourceById (Source->GetResourceIndex() );
    return (CachedResource.IsValid() );
  }

}
