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


#ifndef GLRESOURCEMANAGER_H
#define GLRESOURCEMANAGER_H

NAMESPACE_BEGIN_OGL

// class NTexture;
// class NTexture2D;
// class NRectangleTexture;
// class NTextureCube;
// class NTextureVolume;
// class NAnimatedTexture;
// 
// class NVertexBuffer;
// class NIndexBuffer;
// 
// class NGLTexture2D;
// class NGLRectangleTexture;
// class NGLTextureCube;
// class NGLTextureVolume;
// class NGLAnimatedTexture;

//     NResource           NGLResource
//         |                   |
//     NTexture            NGLTexture
//         |                   |
//     NTexture2D          NGLTexture2D
// 
// 
//     NResourceFactory
//         |
//     NGLResourceFactory
//         |
//     TGLResourceFactory<class NTexture2D, class NGLTexture2D>
// 
// 
//     NResourceSet
//         |
//     TResourceCache<class IdType(int), class ResourceType(NGLResource)>
//         |
//     NResourceCache: TResourceCache<int, NGLResource>
//
//

//! Device independent base resource.
class NResource: public NRefCount
{
    DECLARE_ROOT_OBJECT_TYPE(NResource);
public:
    NResource();
    virtual NString GetResourceName() { return NString(TEXT("")); }
    unsigned int GetResourceIndex() const;
private:
    unsigned int m_ResourceIndex;
};

class NGLResource;

class NResourceSet
{
public:
    NResourceSet();
    virtual ~NResourceSet(){};

protected:
    NGLResource* FirstResource;

    // Flush
    virtual void Flush() {}

    // FreeResource - Called when a potentially cached resource has been freed.
    virtual void FreeResource(NResource* Resource) {}

    // FlushResource - Removes a resource from the set.
    virtual void FlushResource(NGLResource* Resource) {}

    friend class NGLResource;
};

enum EResourceUpdateHint
{
    RUH_Static,				// The resource is updated once, at creation time.
    RUH_CacheableDynamic,	// The resource is updated occasionally, but not every frame.
    RUH_Dynamic				// The resource changes every frame.
};

class NGLResource: public NRefCount
{
    DECLARE_ROOT_OBJECT_TYPE(NGLResource);

public:
    NGLResource(NResourceSet* InSet);
    virtual ~NGLResource();

   virtual NString GetSubTypeDescription() { return NString(TEXT("Resource")); }
    virtual NString GetTypeDescription() { return NString(TEXT("Resource")); }

    //! Returns the size in bytes of the resource.
    /*! Returns the size in bytes of the resource.
        @return Size of the resource in bytes.
    */
    virtual unsigned int GetSize() const { return Size; }

    //! Returns the size of the max LOD of the resource.
    /*!
        Returns the size of the max LOD of the resource. For a texture, this is the size of mipmap 0.
        Texture resource overwrite this function.
        @return Size of the resource in bytes.
    */
    virtual unsigned int GetMaxLodSize() const { return Size; }

    /*! 
        Updates the resource.
    */
    virtual bool UpdateResource(NResource* Resource) = 0;

protected:
    NResourceSet*       Set;
    bool                Cached;
    unsigned int        NumRefs;
    NString             Description;
    NObjectType*        ResourceType;

    unsigned int        Size;
    EResourceUpdateHint UpdateHint;

    NGLResource* PrevResource;
    NGLResource* NextResource;

    template<class IdType, class ResourceType>
    friend class TResourceCache;
    friend class NResourceCache;
};


//! Device independent resource factory.
class NResourceFactory
{
public:
    NResourceFactory(NObjectType* Type)
        :   m_ResourceType(Type)
    {}

      // Returns the resource type for this factory
      const NObjectType& Type() const
      { 
          return *m_ResourceType; 
      }

      /*! 
        Returns true if the given NResource is created by this factory.
        @param  Resource - the resource in question.
      */
      bool BuildsThisResource(NResource* Resource) 
      { 
          return Resource->Type().IsObjectType(Type());
      }

      virtual NGLResource*	BuildResource(NResourceSet* ResourceManager, NResource* Resource) 
      { 
          return NULL; 
      }
private:
    //! Type associated with this factory class.
    NObjectType* m_ResourceType;
};

template <class T, class D>
class TGLResourceFactory : public NResourceFactory
{
public:	
    /*!
        Constructor.
        @param  type - resource class type to associate w/ this factory.
    */
    TGLResourceFactory(NObjectType* Type)
        :   NResourceFactory(Type)
    {}

    virtual	~TGLResourceFactory(void) 
    {}

    //! Create a new resource.
    /*! Create a new resource for the given NResource.
        @param  ResourceManager The resource manager.
        @param  Resource        Resource to build and cache.
        @return The built resource.
    */
    virtual NGLResource*	BuildResource(NResourceSet* ResourceManager, NResource* Resource) 
    { 
        return new D(ResourceManager, (T*)Resource); 
    }
};


//! Device independent resource updater.
class NResourceUpdater
{
public:
    NResourceUpdater(NObjectType* Type)
        :   m_ResourceType(Type)
    {}

    //! Returns the resource type for this factory.
    const NObjectType& Type() const 
    { 
        return *m_ResourceType; 
    }

    /*! 
        Returns true if the given NResource can be updated by this factory.
        @param  Resource    The resource in question.
    */
    bool UpdatesThisResource(NResource* Resource) 
    { 
        return Resource->Type().IsObjectType(Type());
    }

    virtual bool UpdateResource(TRefGL< NGLResource > DeviceResource, NResource* Resource) const
    { 
        return DeviceResource->UpdateResource(Resource); 
    }

private:
    //! Type associated with this factory class.
    NObjectType*	m_ResourceType;
};

template<typename IdType, typename ResourceType>
class TResourceCache: public NResourceSet
{
public:
    std::map< IdType, TRefGL< ResourceType > > ResourceMap;
    
    // Resource factory instances for each NResource/NGLResource pair.
    std::vector<NResourceFactory*> ResourceFactories;

    // Resource updater instances for each NResource type.
    std::vector<NResourceUpdater*> ResourceUpdaters;


    TResourceCache()
        :   NResourceSet()
    {}

    void Flush()
    {
        // See the language FAQ 35.18 at http://www.parashift.com/c++-faq-lite/templates.html  for why the "typename".
        typename std::map< IdType, TRefGL< ResourceType > >::iterator It;
        for(It = ResourceMap.begin(); It != ResourceMap.end(); It++)
        {
            TRefGL< ResourceType >	CachedResource = (*It).second;
            CachedResource->Cached = 0;
        }
        // Erases all elements from the map.
        ResourceMap.clear();
    }

    void AddCachedResource(const IdType& Id, TRefGL< ResourceType > Resource)
    {
        typedef std::map< IdType, TRefGL< ResourceType > >  MapType; 
        ResourceMap.insert(typename MapType::value_type(Id, Resource));
        Resource->Cached = 1;
    }

    TRefGL< ResourceType > FindCachedResourceById(const IdType& Id)
    {
        typedef std::map< IdType, TRefGL< ResourceType > >  MapType; 
        typename MapType::iterator it = ResourceMap.find(Id);
        if(it != ResourceMap.end())
            return (*it).second;
        return 0;
    }

    void FlushResourceId(const IdType& Id)
    {
        TRefGL< ResourceType >	CachedResource = 0;

        typedef std::map< IdType, TRefGL< ResourceType > >  MapType; 
        typename MapType::iterator it = ResourceMap.find(Id);
        if(it != ResourceMap.end())
            CachedResource = (*it).second;
        if(CachedResource.IsValid())
        {
            ResourceMap.erase(it);
            CachedResource->Cached = 0; // Make sure that if the following line deletes the resource, it doesn't try to remove itself from the TDynamicMap we're iterating over.
        }
    }

    virtual void FlushResource(NGLResource* Resource)
    {
        typedef std::map< IdType, TRefGL< ResourceType > >  MapType; 
        typename MapType::iterator it;
        for(it = ResourceMap.begin(); it != ResourceMap.end(); it++)
        {
            TRefGL< ResourceType >	CachedResource = (*it).second;
            if(CachedResource == Resource)
            {
                ResourceMap.erase(it);
                CachedResource->Cached = 0; // Make sure that if the following line deletes the resource, it doesn't try to remove itself from the TDynamicMap we're iterating over.
                return;
            }
        }
    }

    // Register NGLResource with the corresponding NResource 
    virtual void InitializeResourceFactories() = 0;

    std::vector<NResourceFactory*>&	GetResourceFactories(void) { return(ResourceFactories); }
    std::vector<NResourceUpdater*>&	GetResourceUpdaters(void) { return(ResourceUpdaters); }
};

class NResourceCache: public TResourceCache<int, NGLResource>
{
public:
    NResourceCache()
        :   TResourceCache<int, NGLResource>()
    {}

    TRefGL< NGLResource > GetCachedResource(NResource* Source);
    bool         IsCachedResource(NResource* Source);

    virtual void InitializeResourceFactories();
    virtual void FreeResource(NResource* Resource)
    {
        FlushResourceId(Resource->GetResourceIndex());
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////

NAMESPACE_END_OGL

#endif // GLRESOURCEMANAGER_H

