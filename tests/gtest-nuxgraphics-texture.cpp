#include <string>
#include <gmock/gmock.h>
#include <glib.h>

#include "Nux/Nux.h"
#include "Nux/HLayout.h"

#include "NuxGraphics/NuxGraphics.h"
#include "NuxGraphics/GLTextureResourceManager.h"
#include "Nux/CairoWrapper.h"


using namespace testing;
using namespace nux;

namespace {

class TestTextures : public ::testing::Test
{
public:
  virtual void SetUp()
  {
    g_unsetenv("NUX_FALLBACK_TEXTURE");
    nux::NuxInitialize(0);
    wnd_thread.reset(nux::CreateNuxWindow("nux::TestTextures", 300, 200, nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL));
  }

  std::unique_ptr<nux::WindowThread> wnd_thread;
};

TEST_F(TestTextures, TestTextureSizes)
{
  int max_size = GetGraphicsDisplay()->GetGpuDevice()->GetGpuInfo().GetMaxTextureSize();
  int size = max_size/4;

  nux::ObjectPtr<BaseTexture> texture;

  while (size <= max_size * 4)
  {
    BitmapFormat bitmap_format = BITFMT_B8G8R8A8;
    NBitmapData* bitmap = new NTextureData (bitmap_format,
                                10,
                                size,
                                1);

    texture.Adopt(GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture());
    GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource(texture.GetPointer());

    texture->Update(bitmap, true);

    delete bitmap;
    size*=2;
  }
}

TEST_F(TestTextures, FallbackTexture2DFromFile)
{
  ASSERT_THAT(CreateTexture2DFromFile(nullptr, -1, false), IsNull());
  g_setenv("NUX_FALLBACK_TEXTURE", "TRUE", TRUE);
  ASSERT_THAT(CreateTexture2DFromFile(nullptr, -1, false), NotNull());
}

TEST_F(TestTextures, FallbackTexture2DFromPixbuf)
{
  ASSERT_THAT(CreateTexture2DFromPixbuf(nullptr, false), IsNull());
  g_setenv("NUX_FALLBACK_TEXTURE", "TRUE", TRUE);
  ASSERT_THAT(CreateTexture2DFromPixbuf(nullptr, false), NotNull());
}

TEST_F(TestTextures, FallbackTextureFromPixbuf)
{
  ASSERT_THAT(CreateTextureFromPixbuf(nullptr), IsNull());
  g_setenv("NUX_FALLBACK_TEXTURE", "TRUE", TRUE);
  ASSERT_THAT(CreateTextureFromPixbuf(nullptr), NotNull());
}

TEST_F(TestTextures, FallbackTextureFromFile)
{
  ASSERT_THAT(CreateTextureFromFile(nullptr), IsNull());
  g_setenv("NUX_FALLBACK_TEXTURE", "TRUE", TRUE);
  ASSERT_THAT(CreateTextureFromFile(nullptr), NotNull());
}

TEST_F(TestTextures, FallbackTextureFromBitmapData)
{
  ASSERT_THAT(CreateTextureFromBitmapData(nullptr), IsNull());
  g_setenv("NUX_FALLBACK_TEXTURE", "TRUE", TRUE);
  ASSERT_THAT(CreateTextureFromBitmapData(nullptr), NotNull());
}

TEST_F(TestTextures, FallbackTextureLoadFromFile)
{
  ASSERT_THAT(LoadTextureFromFile(std::string()), IsNull());
  g_setenv("NUX_FALLBACK_TEXTURE", "TRUE", TRUE);
  ASSERT_THAT(LoadTextureFromFile(std::string()), NotNull());
}

}