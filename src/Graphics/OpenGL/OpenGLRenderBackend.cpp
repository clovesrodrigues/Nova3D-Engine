#include "OpenGLRenderBackend.hpp"
#include <Nova3D/Core/Logger.hpp>
#include <Nova3D/Platform/IWindow.hpp>
#include <SDL.h>
#if defined(__ANDROID__)
#include <GLES3/gl3.h>
#else
#include <SDL_opengl.h>
#endif

namespace nova3d::graphics {
static unsigned int compileStage(unsigned int type, const std::string& src){ auto id=glCreateShader(type); const char* c=src.c_str(); glShaderSource(id,1,&c,nullptr); glCompileShader(id); return id; }
OpenGLShader::~OpenGLShader(){ if(m_program) glDeleteProgram(m_program);} 
bool OpenGLShader::compile(const std::string& vs,const std::string& fs){ auto v=compileStage(GL_VERTEX_SHADER,vs); auto f=compileStage(GL_FRAGMENT_SHADER,fs); m_program=glCreateProgram(); glAttachShader(m_program,v); glAttachShader(m_program,f); glLinkProgram(m_program); glDeleteShader(v); glDeleteShader(f); return m_program!=0;}
void OpenGLShader::bind(){ glUseProgram(m_program);} void OpenGLShader::unbind(){ glUseProgram(0);} bool OpenGLShader::setUniformFloat(const std::string& n,float v){auto l=glGetUniformLocation(m_program,n.c_str()); if(l<0)return false; glUniform1f(l,v); return true;} bool OpenGLShader::setUniformVec4(const std::string& n,float x,float y,float z,float w){auto l=glGetUniformLocation(m_program,n.c_str()); if(l<0)return false; glUniform4f(l,x,y,z,w); return true;}
OpenGLTexture::~OpenGLTexture(){ if(m_id) glDeleteTextures(1,&m_id);} bool OpenGLTexture::upload(const void* p,int w,int h,int c){ if(!m_id) glGenTextures(1,&m_id); m_width=w; m_height=h; bind(0); GLenum fmt=(c==4)?GL_RGBA:GL_RGB; glTexImage2D(GL_TEXTURE_2D,0,fmt,w,h,0,fmt,GL_UNSIGNED_BYTE,p); return true;} void OpenGLTexture::bind(std::uint32_t s){ glActiveTexture(GL_TEXTURE0+s); glBindTexture(GL_TEXTURE_2D,m_id);} void OpenGLTexture::unbind(){ glBindTexture(GL_TEXTURE_2D,0);} void OpenGLTexture::setFilter(TextureFilter min,TextureFilter mag){ glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,min==TextureFilter::Linear?GL_LINEAR:GL_NEAREST); glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,mag==TextureFilter::Linear?GL_LINEAR:GL_NEAREST);} void OpenGLTexture::setWrap(TextureWrap u,TextureWrap v){ glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,u==TextureWrap::Repeat?GL_REPEAT:GL_CLAMP_TO_EDGE); glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,v==TextureWrap::Repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);} 
OpenGLVertexBuffer::~OpenGLVertexBuffer(){ if(m_id) glDeleteBuffers(1,&m_id); if(m_vao) glDeleteVertexArrays(1,&m_vao);} bool OpenGLVertexBuffer::upload(const void* d,std::size_t b){ if(!m_id) glGenBuffers(1,&m_id); if(!m_vao) glGenVertexArrays(1,&m_vao); glBindVertexArray(m_vao); glBindBuffer(GL_ARRAY_BUFFER,m_id); glBufferData(GL_ARRAY_BUFFER,(GLsizeiptr)b,d,GL_STATIC_DRAW); glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0); glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float))); return true;} void OpenGLVertexBuffer::bind(){ glBindVertexArray(m_vao); glBindBuffer(GL_ARRAY_BUFFER,m_id);} void OpenGLVertexBuffer::unbind(){ glBindVertexArray(0); glBindBuffer(GL_ARRAY_BUFFER,0);} 
OpenGLIndexBuffer::~OpenGLIndexBuffer(){ if(m_id) glDeleteBuffers(1,&m_id);} bool OpenGLIndexBuffer::upload(const std::uint32_t* d,std::size_t c){ if(!m_id) glGenBuffers(1,&m_id); m_count=c; glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_id); glBufferData(GL_ELEMENT_ARRAY_BUFFER,(GLsizeiptr)(sizeof(std::uint32_t)*c),d,GL_STATIC_DRAW); return true;} void OpenGLIndexBuffer::bind(){ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_id);} void OpenGLIndexBuffer::unbind(){ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);} 
void OpenGLVideoDriver::beginFrame(){} void OpenGLVideoDriver::endFrame(){} void OpenGLVideoDriver::clear(const Color& c,bool d){ glClearColor(c.r,c.g,c.b,c.a); glClear(d?(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT):GL_COLOR_BUFFER_BIT);} void OpenGLVideoDriver::setViewport(const Viewport& v){ glViewport(v.x,v.y,v.width,v.height);} void OpenGLVideoDriver::setRenderState(const RenderState& s){ s.depthTest?glEnable(GL_DEPTH_TEST):glDisable(GL_DEPTH_TEST); glDepthMask(s.depthWrite?GL_TRUE:GL_FALSE); s.culling?glEnable(GL_CULL_FACE):glDisable(GL_CULL_FACE); s.wireframe?glPolygonMode(GL_FRONT_AND_BACK,GL_LINE):glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); s.blending?glEnable(GL_BLEND):glDisable(GL_BLEND);} void OpenGLVideoDriver::drawIndexed(PrimitiveType,std::size_t c){ glDrawElements(GL_TRIANGLES,(GLsizei)c,GL_UNSIGNED_INT,nullptr);} std::shared_ptr<IShader> OpenGLVideoDriver::createShader(){ return std::make_shared<OpenGLShader>(); } std::shared_ptr<ITexture> OpenGLVideoDriver::createTexture(){ return std::make_shared<OpenGLTexture>(); } std::shared_ptr<IVertexBuffer> OpenGLVideoDriver::createVertexBuffer(){ return std::make_shared<OpenGLVertexBuffer>(); } std::shared_ptr<IIndexBuffer> OpenGLVideoDriver::createIndexBuffer(){ return std::make_shared<OpenGLIndexBuffer>(); } std::shared_ptr<IMaterial> OpenGLVideoDriver::createMaterial(){ return std::make_shared<BasicMaterial>(); }

bool OpenGLRenderContextDesktop::initialize(platform::IWindow& window) { m_window=&window;
#if defined(__ANDROID__)
SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3); SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
#else
SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3); SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
#endif
SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1); SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24); m_context=SDL_GL_CreateContext(window.sdlWindow()); if(!m_context){ core::Logger::error("SDL_GL_CreateContext failed"); return false;} SDL_GL_SetSwapInterval(1); int w=0,h=0; window.getSize(w,h); resize(w,h); return true; }
void OpenGLRenderContextDesktop::shutdown(){ if(m_context){ SDL_GL_DeleteContext(m_context); m_context=nullptr; }}
void OpenGLRenderContextDesktop::beginFrame(){ m_videoDriver.beginFrame(); m_videoDriver.clear({0.1F,0.15F,0.2F,1.0F}, true);} void OpenGLRenderContextDesktop::endFrame(){ m_videoDriver.endFrame(); if(m_window) m_window->swapBuffers(); }
void OpenGLRenderContextDesktop::resize(int width, int height){ m_videoDriver.setViewport({0,0,width,height}); }

} // namespace
