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


#ifndef GLRESOURCEMANAGER_H
#define GLRESOURCEMANAGER_H

namespace nux
{

// class BaseTexture;
// class Texture2D;
// class TextureRectangle;
// class TextureCube;
// class TextureVolume;
// class TextureFrameAnimation;
//
// class NVertexBuffer;
// class NIndexBuffer;
//
// class CachedTexture2D;
// class CachedTextureRectangle;
// class CachedTextureCube;
// class CachedTextureVolume;
// class CachedTextureFrameAnimation;

//     ResourceData           CachedResourceData
//         |                   |
//     BaseTexture            CachedBaseTexture
//         |                   |
//     Texture2D              CachedTexture2D
//
//
//     NResourceFactory
//         |
//     NGLResourceFactory
//         |
//     TGLResourceFactory<class Texture2D, class CachedTexture2D>
//
//
//     NResourceSet
//         |
//     TResourceCache<class IdType(int), class ResourceType(CachedResourceData)>
//         |
//     NResourceCache: TResourceCache<int, CachedResourceData>
//
//

  //! Base class for all types of resources.
  class ResourceData: public Object
  {
    NUX_DECLARE_OBJECT_TYPE(ResourceData, Object);
  public:
    ResourceData(NUX_FILE_LINE_PROTO);
    virtual ~ResourceData();
    int GetResourceIndex() const;

  private:
    int m_ResourceIndex;
  };

  class CachedResourceData;

  class NResourceSet
  {
  public:
    NResourceSet();
    virtual ~NResourceSet() {};

  protected:
    CachedResourceData *FirstResource;

    // Flush
    virtual void Flush() {}

    // FreeResource - Called when a potentially cached resource has been freed.
    virtual void FreeResource(ResourceData * /* Resource */) {}

    // FlushResource - Removes a resource from the set.
    virtual void FlushResource(CachedResourceData * /* Resource */) {}

    friend class CachedResourceData;
  };

  enum EResourceUpdateHint
  {
    RUH_Static,				// The resource is updated once, at creation time.
    RUH_CacheableDynamic,	// The resource is updated occasionally, but not every frame.
    RUH_Dynamic				// The resource changes every frame.
  };

  class CachedResourceData: public Object
  {
    NUX_DECLARE_OBJECT_TYPE(CachedResourceData, Object);

  public:
    CachedResourceData(NResourceSet *InSet);
    virtual ~CachedResourceData();

    //! Returns the size in bytes of the resource.
    /*! Returns the size in bytes of the resource.
        @return Size of the resource in bytes.
    */
    virtual unsigned int GetSize() const
    {
      return Size;
    }

    //! Returns the size of the max LOD of the resource.
    /*!
        Returns the size of the max LOD of the resource. For a texture, this is the size of mipmap 0.
        Texture resource overwrite this function.
        @return Size of the resource in bytes.
    */
    virtual unsigned int GetMaxLodSize() const
    {
      return Size;
    }

    /*!
        Updates the resource.
    */
    virtual bool UpdateResource(ResourceData *Resource) = 0;

  protected:
    NResourceSet       *Set;
    bool                _cached;
    unsigned int        NumRefs;
    NObjectType        *ResourceType;

    unsigned int        Size;
    EResourceUpdateHint UpdateHint;

    CachedResourceData *PrevResource;
    CachedResourceData *NextResource;

    template<class IdType, class ResourceType>
    friend class TResourceCache;
    friend class NResourceCache;
  };


//! Device independent resource factory.
  class NResourceFactory
  {
  public:
    NResourceFactory(NObjectType *Type)
      :   m_ResourceType(Type)
    {}
    virtual ~NResourceFactory() {}

    // Returns the resource type for this factory
    const NObjectType &Type() const
    {
      return *m_ResourceType;
    }

    /*!
      Returns true if the given ResourceData is created by this factory.
      @param  Resource - the resource in question.
    */
    bool BuildsThisResource(ResourceData *Resource)
    {
      return Resource->Type().IsObjectType(Type());
    }

    virtual CachedResourceData *BuildResource(NResourceSet * /* ResourceManager */, ResourceData * /* Resource */)
    {
      return NULL;
    }

  private:
    //! Type associated with this factory class.
    NObjectType *m_ResourceType;
  };

  template <typename T, typename U>
  class TGLResourceFactory : public NResourceFactory
  {
  public:
    /*!
        Constructor.
        @param  type - resource class type to associate w/ this factory.
    */
    TGLResourceFactory(NObjectType *Type)
      :   NResourceFactory(Type)
    {}

    virtual	~TGLResourceFactory(void)
    {}

    //! Create a new resource.
    /*! Create a new resource for the given ResourceData.
        @param  ResourceManager The resource manager.
        @param  Resource        Resource to build and cache.
        @return The built resource.
    */
    virtual CachedResourceData *BuildResource(NResourceSet *ResourceManager, ResourceData *Resource)
    {
      return new U(ResourceManager, (T *)Resource);
    }
  };


//! Device independent resource updater.
  class NResourceUpdater
  {
  public:
    NResourceUpdater(NObjectType *Type)
      :   m_ResourceType(Type)
    {}

    //! Returns the resource type for this factory.
    const NObjectType &Type() const
    {
      return *m_ResourceType;
    }

    /*!
        Returns true if the given ResourceData can be updated by this factory.
        @param  Resource    The resource in question.
    */
    bool UpdatesThisResource(ResourceData *Resource)
    {
      return Resource->Type().IsObjectType(Type());
    }

    virtual bool UpdateResource(ObjectPtr< CachedResourceData > DeviceResource, ResourceData *Resource) const
    {
      return DeviceResource->UpdateResource(Resource);
    }

  private:
    //! Type associated with this factory class.
    NObjectType	*m_ResourceType;
  };

  template<typename IdType, typename ResourceType>
  class TResourceCache: public NResourceSet
  {
  public:
    std::map< IdType, ObjectPtr< ResourceType > > ResourceMap;

    // Resource factory instances for each ResourceData/CachedResourceData pair.
    std::vector<NResourceFactory *> ResourceFactories;

    // Resource updater instances for each ResourceData type.
    std::vector<NResourceUpdater *> ResourceUpdaters;


    TResourceCache()
      :   NResourceSet()
    {}

    void Flush()
    {
      // See the language FAQ 35.18 at http://www.parashift.com/c++-faq-lite/templates.html  for why the "typename".
      typename std::map< IdType, ObjectPtr< ResourceType > >::iterator It;

      for (It = ResourceMap.begin(); It != ResourceMap.end(); It++)
      {
//         ObjectPtr< ResourceType >	CachedResource = (*It).second;
//         CachedResource->_cached = 0;
//         CachedResource.Release();
        (*It).second->_cached = 0;
        (*It).second.Release();
      }

      // Erases all elements from the map.
      ResourceMap.clear();
    }

    void AddCachedResource(const IdType &Id, ObjectPtr< ResourceType > Resource)
    {
      typedef std::map< IdType, ObjectPtr< ResourceType > >  MapType;
      ResourceMap.insert(typename MapType::value_type(Id, Resource));
      Resource->_cached = 1;
    }

    ObjectPtr<ResourceType> FindCachedResourceById(const IdType &Id)
    {
      typedef std::map< IdType, ObjectPtr< ResourceType > >  MapType;
      typename MapType::iterator it = ResourceMap.find(Id);

      if (it != ResourceMap.end())
        return(*it).second;

      return ObjectPtr<ResourceType> (0);
    }

    /*!
        Remove a cached resource from the cache. The cached resource may still have references to it.
        The resource internal flag "_cached" is set to false.
    */
    void FlushResourceId(const IdType &Id)
    {
      ObjectPtr<ResourceType>	CachedResource(0);

      typedef std::map<IdType, ObjectPtr<ResourceType> >  MapType;
      typename MapType::iterator it = ResourceMap.find(Id);

      if (it != ResourceMap.end())
        CachedResource = (*it).second;

      if (CachedResource.IsValid())
      {
        ResourceMap.erase(it);
        CachedResource->_cached = false;
      }
    }

    virtual void FlushResource(CachedResourceData *Resource)
    {
      typedef std::map< IdType, ObjectPtr< ResourceType > >  MapType;
      typename MapType::iterator it;

      for (it = ResourceMap.begin(); it != ResourceMap.end(); it++)
      {
        ObjectPtr< ResourceType >	CachedResource = (*it).second;

        if (CachedResource == Resource)
        {
          ResourceMap.erase(it);
          CachedResource->_cached = 0; // Make sure that if the following line deletes the resource, it doesn't try to remove itself from the TDynamicMap we're iterating over.
          return;
        }
      }
    }

    // Register CachedResourceData with the corresponding ResourceData
    virtual void InitializeResourceFactories() = 0;

    std::vector<NResourceFactory *>&	GetResourceFactories(void)
    {
      return(ResourceFactories);
    }
    std::vector<NResourceUpdater *>&	GetResourceUpdaters(void)
    {
      return(ResourceUpdaters);
    }
  };

  class NResourceCache: public TResourceCache<int, CachedResourceData>
  {
  public:
    NResourceCache()
      :   TResourceCache<int, CachedResourceData>()
    {}

    ObjectPtr< CachedResourceData > GetCachedResource(ResourceData *Source);
    bool         IsCachedResource(ResourceData *Source);

    virtual void InitializeResourceFactories();
//     virtual void FreeResource(ResourceData *Resource)
//     {
//       FlushResourceId(Resource->GetResourceIndex());
//     }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif // GLRESOURCEMANAGER_H

