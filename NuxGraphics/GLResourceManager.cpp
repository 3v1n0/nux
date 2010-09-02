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

NAMESPACE_BEGIN_OGL

extern PixelFormatInfo GPixelFormats[];

IMPLEMENT_ROOT_OBJECT_TYPE(NResource);
IMPLEMENT_ROOT_OBJECT_TYPE(NGLResource);

NResource::NResource()
:   m_ResourceIndex(INL_INVALID_INDEX)
{
#define GET_UNIQUE_RESOURCE_INDEX INL_GLOBAL_OBJECT_INSTANCE(NUniqueIndex)
    m_ResourceIndex = GET_UNIQUE_RESOURCE_INDEX.GetUniqueIndex();
#undef GET_UNIQUE_RESOURCE_INDEX
}

unsigned int NResource::GetResourceIndex() const
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

NGLResource::NGLResource(NResourceSet* ResourceManager)
:   Set(ResourceManager)
,   Cached(0)
,   NumRefs(0)
,   ResourceType(NULL)
,   Size(0)
,   UpdateHint(RUH_Static)
{
    PrevResource = NULL;
    NextResource = Set->FirstResource;
    Set->FirstResource = this;

    if(NextResource)
    {
        NextResource->PrevResource = this;
    }
}

NGLResource::~NGLResource()
{
    if(PrevResource)
    {
        PrevResource->NextResource = NextResource;
    }
    else
    {
        Set->FirstResource = NextResource;
    }

    if(NextResource)
    {
        NextResource->PrevResource = PrevResource;
    }

    if(Cached)
    {
        Set->FlushResource(this);
    }
}

void NResourceCache::InitializeResourceFactories()
{   
    // Define the factory pair NResource - NGLResource
#define INL_DEFINE_RESOURCE_FACTORY_PAIR(SourceTypeName, ResourceTypeName) \
    static TGLResourceFactory<SourceTypeName, ResourceTypeName> Factory##SourceTypeName(&SourceTypeName::StaticObjectType); \
    GetResourceFactories().push_back(&Factory##SourceTypeName);

    INL_DEFINE_RESOURCE_FACTORY_PAIR(NTexture2D, NGLTexture2D);
    INL_DEFINE_RESOURCE_FACTORY_PAIR(NRectangleTexture, NGLRectangleTexture);
    INL_DEFINE_RESOURCE_FACTORY_PAIR(NTextureCube, NGLTextureCube);
    INL_DEFINE_RESOURCE_FACTORY_PAIR(NTextureVolume, NGLTextureVolume);
    INL_DEFINE_RESOURCE_FACTORY_PAIR(NAnimatedTexture, NGLAnimatedTexture);
    INL_DEFINE_RESOURCE_FACTORY_PAIR(NVertexBuffer, NGLVertexBuffer);
    INL_DEFINE_RESOURCE_FACTORY_PAIR(NIndexBuffer, NGLIndexBuffer);
    INL_DEFINE_RESOURCE_FACTORY_PAIR(NVertexDeclaration, NGLVertexDeclaration);
    INL_DEFINE_RESOURCE_FACTORY_PAIR(NStaticMesh, NGLStaticMesh);

#undef INL_DEFINE_RESOURCE_FACTORY_PAIR

    // Define the factory updater for and NResource
#define INL_DEFINE_RESOURCE_UPDATER(SourceTypeName) \
    static NResourceUpdater Updater##SourceTypeName(&SourceTypeName::StaticObjectType); \
    GetResourceUpdaters().push_back(&Updater##SourceTypeName);

    INL_DEFINE_RESOURCE_UPDATER(NTexture2D);	
    INL_DEFINE_RESOURCE_UPDATER(NRectangleTexture);	
    INL_DEFINE_RESOURCE_UPDATER(NTextureCube);	
    INL_DEFINE_RESOURCE_UPDATER(NTextureVolume);	
    INL_DEFINE_RESOURCE_UPDATER(NAnimatedTexture);	
    INL_DEFINE_RESOURCE_UPDATER(NVertexBuffer);	
    INL_DEFINE_RESOURCE_UPDATER(NIndexBuffer);	
    INL_DEFINE_RESOURCE_UPDATER(NVertexDeclaration);	
    INL_DEFINE_RESOURCE_UPDATER(NStaticMesh);	

#undef INL_DEFINE_RESOURCE_UPDATER
}

TRefGL< NGLResource > NResourceCache::GetCachedResource(NResource* Source)
{
    // check to see if it already exists
    TRefGL< NGLResource >	CachedResource = TResourceCache<int,NGLResource>::FindCachedResourceById(Source->GetResourceIndex());
    if(CachedResource.IsNull())
    {
        // iterate over the list of factory types
        for (t_u32 i = 0; i < GetResourceFactories().size(); ++i)
        {
            NResourceFactory* ResourceFactory = GetResourceFactories()[i];
            // check if the factory is valid for the source resource type
            if (ResourceFactory->BuildsThisResource(Source))
            {
                // cache the device resource
                CachedResource = ResourceFactory->BuildResource(this, Source);
                break;
            }
        }
        // make sure the result is valid
        if (CachedResource.IsNull())
        {
            nuxError(TEXT("Cannot cache resource type %s"), Source->Type().GetName());
        }
        else
        {
            // Get resource textual description
            CachedResource->Description		= Source->GetResourceName();
            // Get resource type
            CachedResource->ResourceType	= &(Source->Type());
            // add it to the pool of cached resources
            AddCachedResource(Source->GetResourceIndex(), CachedResource);
        }		
    }
    return CachedResource;
}

bool NResourceCache::IsCachedResource(NResource* Source)
{
    TRefGL< NGLResource > CachedResource = TResourceCache< int, NGLResource >::FindCachedResourceById(Source->GetResourceIndex());
    return (CachedResource.IsValid());
}

void UpdateResource(NResource* Source)
{

}

NAMESPACE_END_OGL
