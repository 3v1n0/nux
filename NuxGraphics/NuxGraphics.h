#ifndef NUXGRAPHICS_H
#define NUXGRAPHICS_H

namespace nux {

    class FontTexture;
    class NResourceCache;

    class NuxGraphicsResources
    {
        NUX_DECLARE_GLOBAL_OBJECT(NuxGraphicsResources, NuxGraphicsGlobalSingletonInitializer);
    public:

        void InitializeResources();
        NString FindResourceLocation(const TCHAR* ResourceFileName, bool ErrorOnFail = false);
        NString FindUITextureLocation(const TCHAR* ResourceFileName, bool ErrorOnFail = false);
        NString FindShaderLocation(const TCHAR* ResourceFileName, bool ErrorOnFail = false);
        NString FindFontLocation(const TCHAR* ResourceFileName, bool ErrorOnFail = false);

        const std::vector<NString>& GetFontSearchPath() const {return m_FontSearchPath;}
        const std::vector<NString>& GetShaderSearchPath() const {return m_ShaderSearchPath;}
        const std::vector<NString>& GetUITextureSearchPath() const {return m_UITextureSearchPath;}
    
        IntrusiveSP<FontTexture> GetFont();
        IntrusiveSP<FontTexture> GetBoldFont();

        /*!
            Cache font textures into the provided resource cache.

        */
        void CacheFontTextures(NResourceCache& RsrcCache);

    private:
        IntrusiveSP<FontTexture> m_normal_font;    //!< The normal font renderer
        IntrusiveSP<FontTexture> m_bold_font;      //!< The bold font renderer

        std::vector<NString> m_FontSearchPath;
        std::vector<NString> m_ShaderSearchPath;
        std::vector<NString> m_UITextureSearchPath;
        FilePath m_ResourcePathLocation;
    };

    void NuxGraphicsInitialize();


} //NUX_NAMESPACE_END

#define GNuxGraphicsResources NUX_GLOBAL_OBJECT_INSTANCE(nux::NuxGraphicsResources)

#endif // NUXGRAPHICS_H
