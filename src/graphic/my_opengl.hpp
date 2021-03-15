

// clang-format off
#include <glad/glad.h>
#include <iostream>

namespace{
    const char* glGetErrorString(GLenum error)
{
    switch (error)
    {
    case GL_NO_ERROR:          return "No Error";
    case GL_INVALID_ENUM:      return "Invalid Enum";
    case GL_INVALID_VALUE:     return "Invalid Value";
    case GL_INVALID_OPERATION: return "Invalid Operation";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid Framebuffer Operation";
    case GL_OUT_OF_MEMORY:     return "Out of Memory";
    case GL_STACK_UNDERFLOW:   return "Stack Underflow";
    case GL_STACK_OVERFLOW:    return "Stack Overflow";
    default:                   return "Unknown Error";
    }
}

bool _glCheckErrors(const char *filename, int line)
{
    GLenum err;
    bool error = false;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("OpenGL Error: %s (%d) [%u] %s\n", filename, line, err, glGetErrorString(err));
        error = true;
        /* exit(-1); */
    }
    return error;
}
#define glCheckErrors() _glCheckErrors(__FILE__, __LINE__)
}
namespace OpenGL {
    class RenderBuffer
    {

    public:
        enum class Target
        {
            RENDERBUFFER = GL_RENDERBUFFER,
        };
        RenderBuffer()
        {
            glGenRenderbuffers(1, &_id);
            glCheckErrors();
            _currentTarget =  Target::RENDERBUFFER;
        }
        void Bind(Target target)
        {
            glBindRenderbuffer(static_cast<int>(target), _id);
            glCheckErrors();
            _currentTarget = target;
        }
          void Bind()
        {
            glBindRenderbuffer(static_cast<int>(_currentTarget), _id);
            glCheckErrors();
        }
        void UnBind()
        {
            glBindRenderbuffer(static_cast<int>(_currentTarget), 0);
            glCheckErrors();
        }
        void DepthStencil(const int w, const int h)
        {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT , w, h); 
            glCheckErrors();
        }
        void DepthStencilMultisample(const int w, const int h)
        {
            glRenderbufferStorageMultisample (static_cast<int>(_currentTarget),0, GL_DEPTH_COMPONENT16, w, h);
            glCheckErrors();
        }
        Target &GetTarget()
        {
            return _currentTarget;
        }
        ~RenderBuffer()
        {
            glDeleteRenderbuffers(1, &_id);
        }
        operator int() const { return _id; }

    private:
        GLuint _id;
        Target _currentTarget = Target::RENDERBUFFER;
    };
    class Texture
    {
    public:
        enum class Format
        {
            RGB = GL_RGB,
            DEPTH_STENCIL = GL_DEPTH_STENCIL
        };
        enum class Target
        {
            TEXTURE_2D = GL_TEXTURE_2D,
            TEXTURE_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE
        };
        enum class Params
        {
            MINIFYING_MIN = GL_TEXTURE_MIN_FILTER,
            MINIFYING_MAX = GL_TEXTURE_MAG_FILTER

        };
        enum class MinifyingValue
        {
            NEAREST = GL_NEAREST,
            LINEAR = GL_LINEAR,
        };

    public:
        Texture()
        {
            glGenTextures(1, &_id);
            glCheckErrors();
            _currentTarget = Target::TEXTURE_2D;
        }
        Texture(const Target target)
        {
            glGenTextures(1, &_id);
            glCheckErrors();            
            _currentTarget = target;
        }
        ~Texture()
        {
            glDeleteTextures(1, &_id);
            glCheckErrors();
        }
        void Bind(const Target target)
        {
            glBindTexture(static_cast<int>(target), _id);
            glCheckErrors();
            _currentTarget = target;
        }
        void Bind()
        {
            glBindTexture(static_cast<int>(_currentTarget), _id);
            glCheckErrors();
        }
        void UnBind()
        {
            glBindTexture(static_cast<int>(_currentTarget), 0);
            glCheckErrors();
        }
        void RGB(const int w, const int h)
        {
            glTexImage2D(static_cast<int>(_currentTarget), 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glCheckErrors();
        }
        void RGBA(const int w, const int h)
        {
            glTexImage2D(static_cast<int>(_currentTarget), 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glCheckErrors();
        }
        void Multisample(const int w, const int h) {
            glTexImage2DMultisample(static_cast<int>(_currentTarget), 1, GL_RGBA,
                         w, h, NULL);
            glCheckErrors();
        }
        void Minifying(const MinifyingValue value)
        {
            glTexParameteri(static_cast<int>(_currentTarget), static_cast<int>(Params::MINIFYING_MAX), static_cast<int>(value));
            glCheckErrors();
            glTexParameteri(static_cast<int>(_currentTarget), static_cast<int>(Params::MINIFYING_MIN), static_cast<int>(value));
            glCheckErrors();
        }
        const Target &GetTarget()
        {
            return _currentTarget;
        }
        operator int() const { return _id; }

    private:
        unsigned int _id = 0;
        Target _currentTarget;
    };
    class Framebuffer
    {
    public:
        enum class Status
        {
            IMCOMPLETE = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
            COMPLETE = GL_FRAMEBUFFER_COMPLETE,
            FRAMEBUFFER_UNDEFINED  = GL_FRAMEBUFFER_UNDEFINED ,
            FRAMEBUFFER_INCOMPLETE_ATTACHMENT = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT ,
            FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
        };
        enum class Attachment
        {
            COLOR = GL_COLOR_ATTACHMENT0,
            DEPTH_STENCIL = GL_DEPTH_ATTACHMENT
        };
        enum class Target
        {
            ALL = GL_FRAMEBUFFER,
            DRAW = GL_DRAW_FRAMEBUFFER,
            READ = GL_READ_FRAMEBUFFER
        };
        Framebuffer()
        {
            glGenFramebuffers(1, &_id);
            _currentTarget = Target::ALL;
        }

        void Bind(const Target target)
        {
            glBindFramebuffer(static_cast<int>(target), _id);
            glCheckErrors();
            _currentTarget = target;
        }
        void Bind()
        {
            glBindFramebuffer(static_cast<int>(_currentTarget), _id);
            glCheckErrors();
        }
        void UnBind()
        {
            glBindFramebuffer(static_cast<int>(_currentTarget), 0);
            glCheckErrors();        
        }

        void Attach(Texture &texture, Attachment attachement = Attachment::COLOR)
        {
            switch (texture.GetTarget())
            {
            case Texture::Target::TEXTURE_2D_MULTISAMPLE:
            case Texture::Target::TEXTURE_2D:
                glFramebufferTexture2D(static_cast<int>(_currentTarget), static_cast<int>(attachement), static_cast<int>(texture.GetTarget()), static_cast<int>(texture), 0);
                glCheckErrors();
                break;
            default:
                break;
            }
            CheckStatus();
        }
        void Attach(RenderBuffer &renderBuffer, Attachment attachement = Attachment::DEPTH_STENCIL)
        {
            switch (renderBuffer.GetTarget())
            {
            case RenderBuffer::Target::RENDERBUFFER:
                glFramebufferRenderbuffer(static_cast<int>(_currentTarget), static_cast<int>(attachement), GL_RENDERBUFFER, renderBuffer);
                glCheckErrors();
                break;

            default:
                break;
            }
            CheckStatus();
        }
        bool IsComplete()
        {
            if (static_cast<int>(_status) == 0)
            {
                CheckStatus();
            }
            return _status == Status::COMPLETE;
        }
        ~Framebuffer()
        {
            glDeleteFramebuffers(1, &_id);
            glCheckErrors();
        }

    private:
        unsigned int _id;
        Target _currentTarget = Target::ALL;
        Status _status;

        void CheckStatus()
        {
            _status = Status(static_cast<int>(glCheckFramebufferStatus(static_cast<int>(_currentTarget))));
        }
    };
    class PostProcessing
    {
    protected:
        Framebuffer &_frameBuffer;
        RenderBuffer &_renderBuffer;
        Texture &_textureColor;
        virtual void init_default_texture(int screenWidth, int screenHeight) {
            _textureColor.RGB(screenWidth, screenHeight);
            _textureColor.Minifying(Texture::MinifyingValue::LINEAR);
        }
        virtual  void init_default_depth_stencil(int screenWidth, int screenHeight) {
            _renderBuffer.Bind();
            _renderBuffer.DepthStencilMultisample(screenWidth, screenHeight);
        }
        void attach_color_and_stencil_buffer(int screenWidth, int screenHeight) {

            _frameBuffer.Attach(_renderBuffer); 
            _frameBuffer.Attach(_textureColor);

            GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, DrawBuffers);
            glCheckErrors();
            if (!_frameBuffer.IsComplete())
            {
                std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
                exit(-1);
            }
        }
    private:
       
        unsigned int _quadVAO;
        void init_framebuffer() {
            float quadVertices[] = {// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                                    // positions   // texCoords
                                    -1.0f, 1.0f, 0.0f, 1.0f,
                                    -1.0f, -1.0f, 0.0f, 0.0f,
                                    1.0f, -1.0f, 1.0f, 0.0f,

                                    -1.0f, 1.0f, 0.0f, 1.0f,
                                    1.0f, -1.0f, 1.0f, 0.0f,
                                    1.0f, 1.0f, 1.0f, 1.0f};
            unsigned int quadVBO;
            glGenVertexArrays(1, &_quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(_quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
            glCheckErrors();
        }


    public:
        PostProcessing(int screenWidth, int screenHeight) : _frameBuffer(* new Framebuffer()), _renderBuffer(* new RenderBuffer()),_textureColor( * new Texture())  
        {
             _frameBuffer.Bind();
            init_framebuffer();
            _textureColor.Bind();
            init_default_texture(screenWidth,screenHeight);
            init_default_depth_stencil(screenWidth,screenHeight);
            attach_color_and_stencil_buffer(screenWidth,screenHeight);
            _frameBuffer.UnBind();
        }
        PostProcessing(int screenWidth, int screenHeight,Texture &texture) : _frameBuffer(* new Framebuffer()), _renderBuffer(* new  RenderBuffer()),_textureColor(texture) 
        {

            init_framebuffer();
        }
        virtual void Copy(Framebuffer &other,int w, int h) {
              
            other.Bind(OpenGL::Framebuffer::Target::READ);
            _frameBuffer.Bind(OpenGL::Framebuffer::Target::DRAW);
   /*          glDrawBuffer(GL_BACK); */
            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR ); 
            glCheckErrors();
            other.UnBind();
            _frameBuffer.UnBind();
        }
        void Capture()
        {
   /*          glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 */
            _frameBuffer.Bind(OpenGL::Framebuffer::Target::DRAW);
/* 
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); */
        }

        void Bind() {
            _frameBuffer.Bind();
        }
        void Bind(const Framebuffer::Target target) {
            _frameBuffer.Bind(target);
        }
        void Render(unsigned int program, Texture &texture) {
            glUseProgram(program);
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(program, "screenTexture"), 0);
            texture.Bind();
            glBindVertexArray(_quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);
        }
        void Render(unsigned int program,bool unbind)
        {
            // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
            if(unbind) {
                _frameBuffer.UnBind();  
                glDisable(GL_DEPTH_TEST);
                // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                // clear all relevant buffers
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            glUseProgram(program);
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(program, "screenTexture"), 0);
            _textureColor.Bind();
            glBindVertexArray(_quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);
        }
        Texture &GetTexture() {
            return _textureColor;
        }
        Framebuffer &GetFramebuffer() {
            return _frameBuffer;
        }
    };
    class  MultiSamplePostProcessing: public PostProcessing {
        protected:
            void init_default_texture(int screenWidth, int screenHeight) override {
                _textureColor.Multisample(screenWidth, screenHeight);
              /*   _textureColor.Minifying(Texture::MinifyingValue::LINEAR);  */
            }
            void init_default_depth_stencil(int screenWidth, int screenHeight) override {
                _renderBuffer.Bind();
                _renderBuffer.DepthStencilMultisample(screenWidth, screenHeight);
            }
        public:
            MultiSamplePostProcessing(int screenWidth, int screenHeight) : 
            PostProcessing(
                screenWidth,
                screenHeight, 
               * new Texture(Texture::Target::TEXTURE_2D_MULTISAMPLE)
            )
            {
                _frameBuffer.Bind();
                _textureColor.Bind();
                init_default_texture(screenWidth,screenHeight);
                init_default_depth_stencil(screenWidth,screenHeight);
                attach_color_and_stencil_buffer(screenWidth,screenHeight);
                _frameBuffer.UnBind();
             
            }
            void Copy(Framebuffer &other,int w, int h) override {   
                other.Bind(OpenGL::Framebuffer::Target::READ);
                _frameBuffer.Bind(OpenGL::Framebuffer::Target::DRAW);
                glDrawBuffer(GL_BACK);
                glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST ); 
                glCheckErrors();
                other.UnBind();
                _frameBuffer.UnBind();
            }
           
    };
}
