

// clang-format off
#include <glad/glad.h>
#include <iostream>


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
        }
        void Bind(Target target = Target::RENDERBUFFER)
        {
            glBindRenderbuffer(static_cast<int>(target), _id);
            _currentTarget = target;
        }
        void UnBind()
        {
            glBindRenderbuffer(static_cast<int>(_currentTarget), 0);
        }
        void DepthStencil(const int w, const int h)
        {
            glRenderbufferStorage(static_cast<int>(_currentTarget), GL_DEPTH24_STENCIL8, w, h);
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
        unsigned int _id;
        Target _currentTarget;
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
        }
        ~Texture()
        {
            glDeleteTextures(1, &_id);
        }
        void Bind(const Target target = Target::TEXTURE_2D)
        {
            glBindTexture(static_cast<int>(target), _id);
            _currentTarget = target;
        }
        void UnBind()
        {
            glBindTexture(static_cast<int>(_currentTarget), 0);
        }
        void RGB(const int w, const int h)
        {
            glTexImage2D(static_cast<int>(_currentTarget), 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        }

        void Minifying(const MinifyingValue value)
        {
            glTexParameteri(static_cast<int>(_currentTarget), static_cast<int>(Params::MINIFYING_MAX), static_cast<int>(value));
            glTexParameteri(static_cast<int>(_currentTarget), static_cast<int>(Params::MINIFYING_MIN), static_cast<int>(value));
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
            COMPLETE = GL_FRAMEBUFFER_COMPLETE
        };
        enum class Attachment
        {
            COLOR = GL_COLOR_ATTACHMENT0,
            DEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT
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
        }

        void Bind(const Target target = Target::ALL)
        {
            glBindFramebuffer(static_cast<int>(target), _id);
            _currentTarget = target;
        }

        void UnBind()
        {
            glBindFramebuffer(static_cast<int>(_currentTarget), 0);
        }

        void Attach(Texture &texture, Attachment attachement = Attachment::COLOR)
        {
            switch (texture.GetTarget())
            {
            case Texture::Target::TEXTURE_2D:
                glFramebufferTexture2D(static_cast<int>(_currentTarget), static_cast<int>(attachement), static_cast<int>(texture.GetTarget()), static_cast<int>(texture), 0);
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
                glFramebufferRenderbuffer(static_cast<int>(_currentTarget), static_cast<int>(attachement), static_cast<int>(renderBuffer.GetTarget()), renderBuffer);
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
        }

    private:
        unsigned int _id;
        Target _currentTarget;
        Status _status;

        void CheckStatus()
        {
            std::cout << glCheckFramebufferStatus(static_cast<int>(_currentTarget)) << std::endl;
            _status = Status(static_cast<int>(glCheckFramebufferStatus(static_cast<int>(_currentTarget))));
        }
    };
    class PostProcessing
    {
    private:
        Framebuffer _frameBuffer;
        Texture _textureColor;
        unsigned int _quadVAO;

    public:
        PostProcessing(int screenWidth, int screenHeight)
        {
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

            _frameBuffer.Bind();

            _textureColor.Bind();
            _textureColor.RGB(screenWidth, screenHeight);
            _textureColor.Minifying(Texture::MinifyingValue::LINEAR);

            _frameBuffer.Attach(_textureColor);

            auto &stencilBuffer = RenderBuffer();
            stencilBuffer.Bind();
            stencilBuffer.DepthStencil(screenWidth, screenHeight);

            _frameBuffer.Attach(stencilBuffer);

            GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, DrawBuffers);

            if (!_frameBuffer.IsComplete())
            {
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            }
            _frameBuffer.UnBind();
        }

        void Capture()
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            _frameBuffer.Bind();
            // make sure we clear the framebuffer's content
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void Render(unsigned int program)
        {
            // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
            _frameBuffer.UnBind();
            // clear all relevant buffers
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(program);
            glDisable(GL_DEPTH_TEST);
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(program, "screenTexture"), 0);
            _textureColor.Bind();
            glBindVertexArray(_quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);
        }
    };
}
