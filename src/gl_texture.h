/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#pragma once

#include "common.h"
#include "opengl_shared.h"
#include "fs_core.h"

#include "loader_thread.h"
//#include "wad_textures.h"

// namespace GoldSource
// {
//     class WADTexturePool;
// };



enum class TextureSource
{
    CommonImage,
    GoldSourceMipTexture,
    GoldSourceWadFile,
    GoldSourceSprite,
    IndexedFrommemory,
    GuessByItself,
    FrameBufferColor,
    FrameBufferDepth,
    Unknown
};

typedef struct rawimage_s
{
    int frameIndex = 0;
    int mipLevel   = 0;

    byte * data       = nullptr;
    size_t dataLength = 0;

    int width  = 0;
    int height = 0;

    GLint glInternalFormat = 0;
    GLint glFormat         = 0;

    rawimage_s(size_t _width, size_t _height, size_t components);
    ~rawimage_s();

} rawimage_t;

class RawTexture
{
    std::list<rawimage_t *> m_lstFrames;

public:
    RawTexture();
    ~RawTexture();

    void   AddRawFrame(rawimage_t *frame);
    size_t NumFrames();

    std::list<rawimage_t *> &Items();
};

#define FL_HAS_ALPHA_CHANNEL (1 << 0)

class GLTexture
{
    friend class TextureManager;

public:
    GLTexture(const char *fileName, TextureSource source, bool fallback, int nFrames = 1);
    ~GLTexture();

    bool IsLoaded() const;
    void SetLoaded(bool val);

    int  Width() const;
    void SetWidth(int val);

    int  Height() const;
    void SetHeight(int val);

    int  SpriteSheetWidth() const;
    void SetSpriteSheetWidth(int val);

    int  SpriteSheetHeight() const;
    void SetSpriteSheetHeight(int val);

    GLuint GLTextureNum(size_t frame = 0) const;
    void   SetGLTextureNum(GLuint val, size_t frame = 0);

    void UploadRawTexture(RawTexture *pTexture);
    void UploadPixels(void *pixels, GLint internalFormat, GLenum format);

    void GenerateGLHandle();
    void Bind(int unit, size_t frame = 0);

    void Ref();
    void Unref();

    bool HasAlpha();

    size_t       NumRef();
    std::string &Name();

    size_t Index()
    {
        return m_nIndex;
    }

private:
    bool m_bLoaded = false;
    int  m_iWidth  = 0;
    int  m_iHeight = 0;

    int m_iSpriteSheetWidth  = 0;
    int m_iSpriteSheetHeight = 0;

    TextureSource m_Source   = TextureSource::Unknown;
    bool          m_Fallback = false;

    RawTexture *m_pRawTextureData;

    int     m_NumFrames  = 0;
    GLuint *m_uiGLTexnum = nullptr;

    size_t m_NumReferences = 0;

    int         m_iFlags      = 0;
    std::string m_strFileName = "";    

    size_t m_nIndex = 0;
};

#include "img_goldsource_spr.h"

// Asynchronouse loader
//
class AsynchTextureLoadResult : public ITaskStepResult
{
    void *     m_pPixels  = nullptr;
    GLTexture *m_pTexture = nullptr;

public:
    AsynchTextureLoadResult(GLTexture *texture, void *pixels);
    ~AsynchTextureLoadResult();

    void ExecuteOnCompletion() override;
    bool NeedEndCallback() override;
};

class AsynchTextureLoadTask : public ITask
{
    std::queue<GLTexture *> m_qScheduledTextures;
    void *                  LoadTextureFileData(GLTexture *texture);

public:
    AsynchTextureLoadTask(const char *setDescription = nullptr);
    ~AsynchTextureLoadTask();

    GLTexture *      ScheduleTexture(const char *fileName);
    ITaskStepResult *ExecuteStep(LoaderThread *loaderThread) override;
    void             OnCompletion() override;
};

/// Texture manager

enum class FallbackTexture
{
    EmoCheckerboard = 0,
    White,
};

class AsynchLoadTextureTask: public ITask
{
    RawTexture *m_pRawTexture = nullptr;

    std::string m_strFileName;
    void *      m_pData;
    size_t      m_DataLength;
    TextureSource m_Source;

    GLTexture *m_pDestTexture;
public:
    AsynchLoadTextureTask(const char *fileName, void *data, size_t length, TextureSource source, GLTexture * destTexture);
    ~AsynchLoadTextureTask();

    RawTexture * GetRawTexture();

    ITaskStepResult *ExecuteStep(LoaderThread *loaderThread) override;

    void OnCompletion() override;

    class LoadFailedResult: public ITaskStepResult
    {
    public:
        LoadFailedResult();

        virtual bool NeedEndCallback();
    };

    class LoadSuccesedResult:public ITaskStepResult
    {
        RawTexture *m_pPixels = nullptr;
        GLTexture * m_pDestTexture   = nullptr;
    public:
        LoadSuccesedResult(AsynchLoadTextureTask *task);

        bool NeedEndCallback() override;

        void ExecuteOnCompletion() override;
    };

};



class TextureManager
{
    friend class AsynchLoadTextureTask;

    // std::list<GoldSource::WADTexturePool *> m_lstWADSPool;
    std::list<GLTexture *>                  m_lstTexturesPool;

    static TextureSource DetermineTextureSourceFromFileName(const char *fileName);
    static TextureSource DetermineTextureSourceFileSignature(void *pixels, size_t length);

    GLTexture *m_pEmoTexture;
    GLTexture *m_pWhiteTexture;

    void MakeFallbackTexture(GLTexture *texture, FallbackTexture fallbackTexture);

    static RawTexture *LoadRawTexture(const char *fileName, TextureSource source = TextureSource::GuessByItself);
    static RawTexture *LoadRawTexture(void *pixels, size_t length, TextureSource source = TextureSource::GuessByItself);

    static RawTexture *DecodeCommonImage(const void *data, size_t length);
    static RawTexture *DecodeGoldsourceMiptex(const void *data, size_t length);
    static RawTexture *DecodeGoldsourceSprite(const void *data, size_t length);

    TextureManager();

public:
    ~TextureManager();

    void OnGLInit();

    static TextureManager *Instance()
    {
        static TextureManager *sInstance = new TextureManager;
        return sInstance;
    }

    // GoldSource::WADTexturePool* RegisterWAD(const char *fileName, bool shared);
    // void UnregisterWAD(const char *fileName);
    // void UnregisterWAD(GoldSource::WADTexturePool * pWad);

    static GLTexture *LoadTextureSynch(const char *fileName, TextureSource source = TextureSource::GuessByItself);
    static GLTexture *LoadTextureSynch(void *data, size_t len, const char *name, TextureSource source = TextureSource::GuessByItself);

    static GLTexture *LoadTextureAsynch(const char *fileName, TextureSource source = TextureSource::GuessByItself);
    static GLTexture *LoadTextureAsynch(void *data, size_t len, const char *name,
                                        TextureSource source = TextureSource::GuessByItself);


    void PurgeTextures();

    static GLTexture *LoadWADTextureSynch(char *name);
    static GLTexture *LoadWADTextureAsynch(const char *name);
    
    static GLTexture *GetWhiteTexture();
    static GLTexture *GetFallbackTexture();
    void              DestroyTexture(GLTexture *m_pTexture);
};
