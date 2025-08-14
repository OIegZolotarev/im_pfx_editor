/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#include "application.h"
#include "mathlib.h"
#include "gl_texture.h"
#include "gl_backend.h"

#include <locale>
#include <stb_image.h>
#include "img_indexed_from_memory.h"

void *AsynchTextureLoadTask::LoadTextureFileData(GLTexture *texture)
{
    FileData *pData = Application::GetFileSystem()->LoadFile(texture->Name());

    if (!pData)
        return nullptr;

    // Load from file
    int image_width  = 0;
    int image_height = 0;
    int comps        = 0;

    stbi_set_flip_vertically_on_load(true);
    void *pixels = stbi_load_from_memory(pData->Data(), (int)pData->Length(), &image_width, &image_height, &comps, 4);

    texture->SetWidth(image_width);
    texture->SetHeight(image_height);

    pData->UnRef();

    // SDL_Delay(100);

    return pixels;
}

AsynchTextureLoadTask::AsynchTextureLoadTask(const char *setDescription)
{
    if (setDescription != nullptr)
        m_strDescription = setDescription;
    else
        m_strDescription = "Loading textures...";
}

AsynchTextureLoadTask::~AsynchTextureLoadTask()
{
    assert(m_qScheduledTextures.empty());
}

GLTexture *AsynchTextureLoadTask::ScheduleTexture(const char *fileName)
{
    GLTexture *texture = new GLTexture(fileName, TextureSource::CommonImage, 0);

    m_qScheduledTextures.push(texture);
    m_nTotalSteps = m_qScheduledTextures.size();

    return texture;
}

ITaskStepResult *AsynchTextureLoadTask::ExecuteStep(LoaderThread *loaderThread)
{
    if (m_qScheduledTextures.empty())
        return new ITaskStepResult(TaskStepResultType::FinishedSuccesfully);

    GLTexture *texture = m_qScheduledTextures.front();
    m_qScheduledTextures.pop();

    m_nPerformedSteps++;

    void *pixels = LoadTextureFileData(texture);

    return new AsynchTextureLoadResult(texture, pixels);
}

void AsynchTextureLoadTask::OnCompletion()
{
    (void)0;
}

AsynchTextureLoadResult::AsynchTextureLoadResult(GLTexture *texture, void *pixels)
    : ITaskStepResult(TaskStepResultType::StepPerformed)
{
    m_pPixels  = pixels;
    m_pTexture = texture;

    m_strElementDescription = texture->Name();
}

AsynchTextureLoadResult::~AsynchTextureLoadResult()
{
    if (m_pPixels)
        stbi_image_free(m_pPixels);
}

void AsynchTextureLoadResult::ExecuteOnCompletion()
{
    // TODO: emo textures
    if (!m_pPixels)
    {
        return;
    }

    m_pTexture->UploadPixels(m_pPixels, GL_RGBA, GL_RGBA);
}

bool AsynchTextureLoadResult::NeedEndCallback()
{
    return true;
}

bool GLTexture::IsLoaded() const
{
    return m_bLoaded;
}

void GLTexture::SetLoaded(bool val)
{
    m_bLoaded = val;
}

int GLTexture::Width() const
{
    return m_iWidth;
}

void GLTexture::SetWidth(int val)
{
    m_iWidth = val;
}

int GLTexture::Height() const
{
    return m_iHeight;
}

void GLTexture::SetHeight(int val)
{
    m_iHeight = val;
}

int GLTexture::SpriteSheetWidth() const
{
    return m_iSpriteSheetWidth;
}

void GLTexture::SetSpriteSheetWidth(int val)
{
    m_iSpriteSheetWidth = val;
}

int GLTexture::SpriteSheetHeight() const
{
    return m_iSpriteSheetHeight;
}

void GLTexture::SetSpriteSheetHeight(int val)
{
    m_iSpriteSheetHeight = val;
}

GLuint GLTexture::GLTextureNum(size_t frame) const
{
    if (m_uiGLTexnum)
        return m_uiGLTexnum[frame];
    return 0;
}

void GLTexture::SetGLTextureNum(GLuint val, size_t frame)
{
    m_uiGLTexnum[frame] = val;
}

void GLTexture::UploadRawTexture(RawTexture *pTexture)
{
    GenerateGLHandle();
    int currentFrame = -1;

    for (auto &rawImage : pTexture->Items())
    {
        if (rawImage->frameIndex != currentFrame)
        {
            currentFrame = (int)rawImage->frameIndex;
            Bind(currentFrame);

            // TODO: adjust this
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, rawImage->mipLevel, rawImage->glInternalFormat, rawImage->width, rawImage->height,
                     0, rawImage->glFormat, GL_UNSIGNED_BYTE, rawImage->data);

        if (!m_iWidth)
        {
            m_iWidth  = rawImage->width;
            m_iHeight = rawImage->height;
        }

        m_bLoaded = true;
    }
}

void GLTexture::UploadPixels(void *pixels, GLint internalFormat, GLenum format)
{
    GenerateGLHandle();
    Bind(0);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non
    // power-of-two textures glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_REPEAT); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // thread-safe
    // g_vecGLTextures.push_back(m_pTexture);

    // stbi_image_free(m_pPixels);
}

GLTexture::GLTexture(const char *fileName, TextureSource source, bool fallback, int nFrames /*= 1*/)
{
    m_strFileName = fileName;
    m_Source      = source;
    m_Fallback    = fallback;
    m_NumFrames   = nFrames;

    static size_t nIndex = 0;
    m_nIndex             = ++nIndex;
}

GLTexture::~GLTexture()
{
    if (m_uiGLTexnum)
    {
        delete m_uiGLTexnum;
        glDeleteTextures(m_NumFrames, m_uiGLTexnum);
    }
}

void GLTexture::GenerateGLHandle()
{
    // assert(!m_uiGLTexnum);

    // TODO: handle texture reloading
    if (!m_uiGLTexnum)
    {
        m_uiGLTexnum = new GLuint[m_NumFrames];
        glGenTextures(m_NumFrames, m_uiGLTexnum);
    }
}

void GLTexture::Bind(int unit, size_t frame /*= 0*/)
{
    GLBackend::BindTexture(unit, m_uiGLTexnum[frame]);
}

void GLTexture::Ref()
{
    m_NumReferences++;
}

void GLTexture::Unref()
{
    m_NumReferences--;
}

bool GLTexture::HasAlpha()
{
    return m_iFlags & FL_HAS_ALPHA_CHANNEL;
}

size_t GLTexture::NumRef()
{
    return m_NumReferences;
}

std::string &GLTexture::Name()
{
    return m_strFileName;
}

TextureSource TextureManager::DetermineTextureSourceFromFileName(const char *fileName)
{
    auto fs  = FileSystem::Instance();
    auto ext = fs->ExtractFileExtension(fileName);
    
    // TODO: fixme
    //boost::algorithm::to_lower(ext);

    struct sLookupTable
    {
        const char *  ext;
        TextureSource fmt;
    };

    static sLookupTable lookup[] = {
        {"spr", TextureSource::GoldSourceSprite},
        {"tga", TextureSource::CommonImage},
        {"png", TextureSource::CommonImage},
        {"jpg", TextureSource::CommonImage},
    };

    for (auto it : lookup)
    {
        if (!strcmp(it.ext, ext.c_str()))
            return it.fmt;
    }

    // Can't determine format by extension, will try with file header
    //
    return TextureSource::GuessByItself;
}

TextureSource TextureManager::DetermineTextureSourceFileSignature(void *pixels, size_t length)
{
    if (length < 8)
        return TextureSource::Unknown;

    // Fallback to common image - in hopes stbi will do the job
    return TextureSource::CommonImage;
}

TextureManager::~TextureManager()
{
    for (auto &it : m_lstTexturesPool)
    {
        delete it;
    }
}

// GoldSource::WADTexturePool* TextureManager::RegisterWAD(const char *fileName, bool shared)
// {
//     IFileHandle *fd = FileSystem::Instance()->OpenFileHandle(fileName);

//     GoldSource::WADTexturePool *pool = new GoldSource::WADTexturePool(fd, shared);
//     m_lstWADSPool.push_back(pool);

//     return pool;
// }

// void TextureManager::UnregisterWAD(const char *fileName)
// {
//     m_lstWADSPool.remove_if([&](GoldSource::WADTexturePool *wt) {
//         if (strcmp(fileName, wt->FileName()))
//         {
//             return true;
//         }

//         return false;
//     });
// }

// void TextureManager::UnregisterWAD(GoldSource::WADTexturePool *pWad)
// {
//     m_lstWADSPool.remove_if([&](GoldSource::WADTexturePool *wt) {
//          return pWad == wt;
//     });
// }

GLTexture *TextureManager::LoadTextureSynch(const char *  fileName,
                                            TextureSource source /*= TextureSource::GuessByItself*/)
{
    GLTexture *pResult = new GLTexture(fileName, source, false);

    if (source == TextureSource::GuessByItself)
        source = DetermineTextureSourceFromFileName(fileName);

    RawTexture *rawTexture = LoadRawTexture(fileName, source);

    if (rawTexture)
    {
        pResult->UploadRawTexture(rawTexture);
        delete rawTexture;
    }
    else
    {
        Instance()->MakeFallbackTexture(pResult, FallbackTexture::EmoCheckerboard);
        return pResult;
    }

    return pResult;
}

GLTexture *TextureManager::LoadTextureSynch(void *data, size_t len, const char *name,
                                            TextureSource source /*= TextureSource::GuessByItself*/)
{
    GLTexture *pResult = new GLTexture(name, source, false);

    Instance()->m_lstTexturesPool.push_back(pResult);

    RawTexture *rawTexture = LoadRawTexture(data, len, source);

    if (!rawTexture)
    {
        Instance()->MakeFallbackTexture(pResult, FallbackTexture::EmoCheckerboard);
        return pResult;
    }
    else
    {
        pResult->UploadRawTexture(rawTexture);
        delete rawTexture;
    }

    return pResult;
}

RawTexture *TextureManager::LoadRawTexture(const char *  fileName,
                                           TextureSource source /*= TextureSource::GuessByItself*/)
{
    auto fs = FileSystem::Instance();
    auto fd = fs->LoadFile(fileName);

    if (!fd)
    {
        return nullptr;
    }

    if (source == TextureSource::GuessByItself)
        source = DetermineTextureSourceFromFileName(fileName);

    RawTexture *pResult = LoadRawTexture(fd->Data(), fd->Length(), source);
    fd->UnRef();

    return pResult;
}

RawTexture *TextureManager::LoadRawTexture(void *data, size_t length,
                                           TextureSource source /*= TextureSource::GuessByItself*/)
{
    RawTexture *pResult = nullptr;

    if (source == TextureSource::GuessByItself)
        source = DetermineTextureSourceFileSignature(data, length);

    switch (source)
    {
    case TextureSource::CommonImage:
        pResult = DecodeCommonImage(data, length);
        break;
    case TextureSource::GoldSourceMipTexture:
        pResult = DecodeGoldsourceMiptex(data, length);
        break;
    case TextureSource::GoldSourceWadFile:
        BREAKPOINT();
        break;
    case TextureSource::GoldSourceSprite:
        pResult = DecodeGoldsourceSprite(data, length);
        break;
    case TextureSource::IndexedFrommemory:
        pResult = ((IndexedFromMemoryImage*)data)->MakeRawTexture();
        break;
    case TextureSource::GuessByItself:
        break;
    case TextureSource::Unknown:
        break;
    default:
        break;
    }

    return pResult;
}

GLTexture *TextureManager::LoadTextureAsynch(const char *fileName, TextureSource source /*= TextureSource::GuessByItself*/)
{
    GLTexture *destTexture = new GLTexture(fileName, source, false);
    Instance()->m_lstTexturesPool.push_back(destTexture);

    AsynchLoadTextureTask *pNewItem = new AsynchLoadTextureTask(fileName, nullptr, 0, source, destTexture);
    pNewItem->Schedule();

    return destTexture;
}

GLTexture *TextureManager::LoadTextureAsynch(void *data, size_t len, const char *name,
                                      TextureSource source /*= TextureSource::GuessByItself*/)
{
    GLTexture *destTexture = new GLTexture(name, source, false);
    Instance()->m_lstTexturesPool.push_back(destTexture);

    AsynchLoadTextureTask *pNewItem = new AsynchLoadTextureTask(name, data, len, source, destTexture);
    pNewItem->Schedule();

    return destTexture;
}

void TextureManager::PurgeTextures()
{
    size_t nPurged = 0;
    m_lstTexturesPool.remove_if([&](GLTexture *texture) {
        if (!texture->NumRef())
        {
            nPurged++;
            delete texture;
            return true;
        }
        return false;
    });

    Con_Printf("TextureManager::PurgeTextures(): purged %zd textures\n", nPurged);
}

GLTexture *TextureManager::LoadWADTextureSynch(char *name)
{
    // GLTexture *pResult   = new GLTexture(name, TextureSource::GoldSourceWadFile, true);
    // bool       bFallback = true;

    // for (auto &wad : Instance()->m_lstWADSPool)
    // {
    //     auto mip = wad->LoadMipTex(name);

    //     if (!mip)
    //         continue;

    //     RawTexture *texture = LoadRawTexture((void *)mip, 0, TextureSource::GoldSourceMipTexture);
    //     pResult->UploadRawTexture(texture);

    //     bFallback = false;
    //     break;
    // }

    // if (bFallback)
    //     Instance()->MakeFallbackTexture(pResult, FallbackTexture::EmoCheckerboard);

    // return pResult;

    return nullptr;
}

GLTexture * TextureManager::LoadWADTextureAsynch(const char *name)
{
    return nullptr;

    // GLTexture *pResult   = new GLTexture(name, TextureSource::GoldSourceWadFile, true);
    

    // for (auto &wad : Instance()->m_lstWADSPool)
    // {
    //     auto mip = wad->LoadMipTex(name);

    //     if (!mip)
    //         continue;

    //     AsynchLoadTextureTask *pTask =
    //         new AsynchLoadTextureTask(name, (void *)mip, 0, TextureSource::GoldSourceMipTexture, pResult);

    //     pTask->Schedule();

    //     return pResult;
    // }
        
    // Instance()->MakeFallbackTexture(pResult, FallbackTexture::EmoCheckerboard);
    // return pResult;
}

GLTexture *TextureManager::GetWhiteTexture()
{
    return Instance()->m_pWhiteTexture;
}

GLTexture *TextureManager::GetFallbackTexture()
{
    return Instance()->m_pEmoTexture;
}

void TextureManager::DestroyTexture(GLTexture *m_pTexture)
{
    m_lstTexturesPool.remove_if([&](GLTexture *pTexture) {

        if (pTexture == m_pTexture)
        {
            delete pTexture;
            return true;
        }

        return false;
    });
}

RawTexture *TextureManager::DecodeCommonImage(const void *data, size_t length)
{
    // Load from file
    int image_width  = 0;
    int image_height = 0;
    int comps        = 0;

    stbi_set_flip_vertically_on_load(true);
    void *pixels = stbi_load_from_memory((byte *)data, length, &image_width, &image_height, &comps, 4);

    RawTexture *pResult = new RawTexture;

    rawimage_t *pFrame = new rawimage_t(image_width, image_height, 4);

    memcpy(pFrame->data, pixels, pFrame->dataLength);

    stbi_image_free(pixels);
    pResult->AddRawFrame(pFrame);

    return pResult;
}

RawTexture *TextureManager::DecodeGoldsourceMiptex(const void *data, size_t length)
{
    return nullptr;
    // GoldSource::miptex_t *texture = (GoldSource::miptex_t *)data;
    // RawTexture *          pResult = new RawTexture;

    // int pixelSize = texture->name[0] == '{' ? 4 : 3;
    // int size      = texture->width * texture->height;
    // int datasize  = size + (size / 4) + (size / 16) + (size / 64);

    // color24_t *pal = (color24_t *)(((byte *)data) + texture->offsets[0] + datasize + 2);

    // for (int i = 0; i < 4; i++)
    // {
    //     byte *pixels = (byte *)data + texture->offsets[i];

    //     size_t mipWidth  = texture->width >> i;
    //     size_t mipHeight = texture->height >> i;

    //     size_t      numPixels = mipWidth * mipHeight;
    //     rawimage_t *pFrame    = new rawimage_t(mipWidth, mipHeight, pixelSize);

    //     pFrame->mipLevel = i;

    //     // Convert pixels from indexed to RGB(A)

    //     if (pixelSize == 4)
    //     {
    //         color32_t *ptr = (color32_t *)pFrame->data;

    //         for (size_t j = 0; j < numPixels; j++)
    //         {
    //             color24_t *palEntry = &pal[pixels[j]];

    //             if (palEntry->b == 255 && palEntry->g == 0 && palEntry->r == 0)
    //             {
    //                 ptr->r = ptr->g = ptr->b = ptr->a = 0;
    //             }
    //             else
    //             {
    //                 ptr->r = palEntry->r;
    //                 ptr->g = palEntry->g;
    //                 ptr->b = palEntry->b;
    //                 ptr->a = 255;
    //             }

    //             ptr++;
    //         }
    //     }
    //     else
    //     {
    //         color24_t *ptr = (color24_t *)pFrame->data;

    //         for (size_t j = 0; j < numPixels; j++)
    //         {
    //             *ptr = pal[pixels[j]];
    //             ptr++;
    //         }
    //     }

    //     pResult->AddRawFrame(pFrame);
    // }

    // return pResult;
}

RawTexture *TextureManager::DecodeGoldsourceSprite(const void *pixels, size_t length)
{
    return ::DecodeGoldSourceSpite((byte *)pixels, length);
}

TextureManager::TextureManager()
{
}

void TextureManager::OnGLInit()
{
    m_pEmoTexture   = LoadTextureSynch("res/textures/default/emo.png");
    m_pWhiteTexture = LoadTextureSynch("res/textures/default/white.png");
}

void TextureManager::MakeFallbackTexture(GLTexture *pResult, FallbackTexture fallbackTexture)
{
    Con_Printf("TextureManager(): failed to load %s, falling back to default texture\n", pResult->Name().c_str());

    GLTexture *pFallback = nullptr;

    switch (fallbackTexture)
    {
    case FallbackTexture::EmoCheckerboard:
        pFallback = m_pEmoTexture;
        break;
    case FallbackTexture::White:
        pFallback = m_pWhiteTexture;
        break;
    default:
        pFallback = m_pEmoTexture;
        break;
    }

    if (!pFallback)
    {
        Con_Printf("TextureManager(): Can't fallback for texture %s - no falback texture avaible!!!\n",
                   pResult->Name().c_str());
        return;
    }

    pResult->m_uiGLTexnum = pFallback->m_uiGLTexnum;
    pResult->m_iWidth     = pFallback->m_iWidth;
    pResult->m_iHeight    = pFallback->m_iHeight;
}

RawTexture::RawTexture()
{
}

RawTexture::~RawTexture()
{
    for (auto &it : m_lstFrames)
        delete it;

    m_lstFrames.clear();
}

void RawTexture::AddRawFrame(rawimage_t *frame)
{
    m_lstFrames.push_back(frame);
}

size_t RawTexture::NumFrames()
{
    return m_lstFrames.size();
}

std::list<rawimage_t *> &RawTexture::Items()
{
    return m_lstFrames;
}

rawimage_s::rawimage_s(size_t _width, size_t _height, size_t components)
{
    width  = _width;
    height = _height;

    dataLength = width * height * components;
    data       = new byte[dataLength];

    switch (components)
    {
    case 3:
        glFormat         = GL_RGB;
        glInternalFormat = GL_RGB;
        break;
    case 4:
        glFormat         = GL_RGBA;
        glInternalFormat = GL_RGBA;
        break;
    }
}

rawimage_s::~rawimage_s()
{
    delete data;
}

AsynchLoadTextureTask::AsynchLoadTextureTask(const char *fileName, void *data, size_t length, TextureSource source,
                                             GLTexture *destTexture)
{
    m_strFileName  = fileName;
    m_Source       = source;
    m_DataLength   = length;
    m_pData        = data;
    m_pDestTexture = destTexture;
}

AsynchLoadTextureTask::~AsynchLoadTextureTask()
{
}

RawTexture *AsynchLoadTextureTask::GetRawTexture()
{
    return m_pRawTexture;
}

ITaskStepResult *AsynchLoadTextureTask::ExecuteStep(LoaderThread *loaderThread)
{
    if (m_Source == TextureSource::GuessByItself)
    {
        if (m_pData)
            m_Source = TextureManager::DetermineTextureSourceFileSignature(m_pData, m_DataLength);
        else
            m_Source = TextureManager::DetermineTextureSourceFromFileName(m_strFileName.c_str());
    }

    if (m_pData)
        m_pRawTexture = TextureManager::LoadRawTexture(m_pData, m_DataLength, m_Source);
    else
        m_pRawTexture = TextureManager::LoadRawTexture(m_strFileName.c_str(), m_Source);

    ITaskStepResult *taskResult = nullptr;

    if (m_pRawTexture)
    {
        taskResult = new LoadSuccesedResult(this);
    }
    else
    {
        TextureManager::Instance()->MakeFallbackTexture(m_pDestTexture, FallbackTexture::EmoCheckerboard);
        taskResult = new LoadFailedResult();      
    }

    return taskResult;
}

void AsynchLoadTextureTask::OnCompletion()
{
}

 AsynchLoadTextureTask::LoadSuccesedResult::LoadSuccesedResult(AsynchLoadTextureTask *task)
    : ITaskStepResult(TaskStepResultType::FinishedSuccesfully)
{
    m_pPixels      = task->m_pRawTexture;
    m_pDestTexture = task->m_pDestTexture;
}

bool AsynchLoadTextureTask::LoadSuccesedResult::NeedEndCallback()
{
    return true;
}

void AsynchLoadTextureTask::LoadSuccesedResult::ExecuteOnCompletion()
{
    m_pDestTexture->UploadRawTexture(m_pPixels);
    delete m_pPixels;
}

 AsynchLoadTextureTask::LoadFailedResult::LoadFailedResult() : ITaskStepResult(TaskStepResultType::FinishedWithFailure)
{
}

bool AsynchLoadTextureTask::LoadFailedResult::NeedEndCallback()
{
    return false;
}
