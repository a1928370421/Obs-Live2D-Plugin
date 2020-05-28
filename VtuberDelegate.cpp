/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/

#include "VtuberDelegate.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp" 
using namespace Csm;
using namespace std;
using namespace LAppDefine;

namespace {
VtuberDelegate *s_instance = NULL;

    static int VtuberCount = 0;
}

VtuberDelegate *VtuberDelegate::GetInstance()
{
    if (s_instance == NULL)
    {
		s_instance = new VtuberDelegate();
    }

    return s_instance;
}

void VtuberDelegate::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

bool VtuberDelegate::LoadResource(uint16_t windowWidth, uint16_t windowHeight)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	if (!LAppPal::IsPathExist(ResourcesPath)) {
		return false;
	}
	ModelFileCount = LAppPal::GetAllDirName(ResourcesPath, ModelFileName);
	return true;
}

void VtuberDelegate::ReleaseResource() {

}

bool VtuberDelegate::Initialize()
{   VtuberCount++;
    if (!isINIT) {
	isINIT = true;
    if (DebugLogEnable)
    {
        LAppPal::PrintLog("START");
    }

    // GLFWの初期化
    if (glfwInit() == GL_FALSE)
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLog("Can't initilize GLFW");
        }
        return GL_FALSE;
    }
    
    // Windowの生成_
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    _window = glfwCreateWindow(RenderTargetWidth, RenderTargetHeight, "vtuber", NULL,
			       NULL);
    if (_window == NULL)
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLog("Can't create GLFW window.");
        }
        glfwTerminate();
        return GL_FALSE;
    }

    glfwMakeContextCurrent(_window);
    if (glewInit() != GLEW_OK) {
        if (DebugLogEnable)
        {
            LAppPal::PrintLog("Can't initilize glew.");
        }
        glfwTerminate();
        return GL_FALSE;
    }

    creatFrameBuffer();

    //AppViewの初期化
    _view->Initialize();

    // Cubism SDK の初期化
    InitializeCubism();
    }

    return GL_TRUE;
}

void VtuberDelegate::Release()
{   
    if(VtuberCount>1)
    {
        VtuberCount--;
    }
    else 
    {
        isINIT = false;

	releaseFrameBuffer();

        glfwDestroyWindow(_window);

        glfwTerminate();

        delete _textureManager;
        delete _view;

        LAppLive2DManager::ReleaseInstance();

        CubismFramework::Dispose();

        CubismFramework::CleanUp();   

        ReleaseInstance();
    }  
}

void VtuberDelegate::creatFrameBuffer()
{    
    glGenFramebuffers(1, &fbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, fbuffer);

    // 生成纹理
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RenderTargetWidth,
		 RenderTargetHeight, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // 将它附加到当前绑定的帧缓冲对象
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LAppPal::PrintLog("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void VtuberDelegate::releaseFrameBuffer()
{

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbuffer);
}

void VtuberDelegate::Reader(char *buffer)
{
	if (Resizeflag) {
		Resizeflag = false;

		_view->Initialize();

		glViewport(0, 0, _windowWidth, _windowHeight);
	}
	LAppPal::UpdateTime();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0);
	
	//描画更新
	_view->Render();

	glReadPixels(0, 0, _windowWidth, _windowHeight, GL_RGBA,GL_UNSIGNED_BYTE,buffer);
}

int VtuberDelegate::ModelCount()
{
	return ModelFileCount;
}

const char **VtuberDelegate::GetModelsName()
{
	return (const char**)ModelFileName;
}

void VtuberDelegate::Resize(int width, int height) {
	Resizeflag = true;	
	_windowWidth = width;
	_windowHeight = height;
}

void VtuberDelegate::ChangeModel(int i)
{
    LAppLive2DManager::GetInstance()->ChangeScene(i);
}

VtuberDelegate::VtuberDelegate() :
    _cubismOption(),
    _window(NULL)
{
    _view = new LAppView();
    _textureManager = new LAppTextureManager();
    isINIT = false;
    Resizeflag = false;
    ModelFileCount = -1;
}

VtuberDelegate::~VtuberDelegate() {}

void VtuberDelegate::InitializeCubism()
{
    _cubismOption.LogFunction = LAppPal::PrintMessage;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    //Initialize cubism
    CubismFramework::Initialize();

    //load model
    LAppLive2DManager::GetInstance();

    //default proj
    CubismMatrix44 projection;

    LAppPal::UpdateTime();
}

GLuint VtuberDelegate::CreateShader()
{
    //バーテックスシェーダのコンパイル
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShader =
        "#version 120\n"
        "attribute vec3 position;"
        "attribute vec2 uv;"
        "varying vec2 vuv;"
        "void main(void){"
        "    gl_Position = vec4(position, 1.0);"
        "    vuv = uv;"
        "}";
    glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
    glCompileShader(vertexShaderId);
    if(!CheckShader(vertexShaderId))
    {
        return 0;
    }

    //フラグメントシェーダのコンパイル
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShader =
        "#version 120\n"
        "varying vec2 vuv;"
        "uniform sampler2D texture;"
        "uniform vec4 baseColor;"
        "void main(void){"
        "    gl_FragColor = texture2D(texture, vuv) * baseColor;"
        "}";
    glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
    glCompileShader(fragmentShaderId);
    if (!CheckShader(fragmentShaderId))
    {
        return 0;
    }

    //プログラムオブジェクトの作成
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // リンク
    glLinkProgram(programId);

    glUseProgram(programId);

    return programId;
}

int VtuberDelegate::getBufferWidth()
{
    return _windowWidth;
}

int VtuberDelegate::getBufferHeight()
{
    return _windowHeight;
}

double VtuberDelegate::getScale()
{
	return Scale;
}

void VtuberDelegate::setScale(double _sc) {
	Scale = _sc;
}

double VtuberDelegate::GetX()
{
	return viewPoint_x;
}

void VtuberDelegate::SetX(double _x)
{
	viewPoint_x = _x;
}

double VtuberDelegate::GetY()
{
	return viewPoint_y;
}

void VtuberDelegate::SetY(double _y)
{
	viewPoint_y = _y;
}

bool VtuberDelegate::CheckShader(GLuint shaderId)
{
    GLint status;
    GLint logLength;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar* log = reinterpret_cast<GLchar*>(CSM_MALLOC(logLength));
        glGetShaderInfoLog(shaderId, logLength, &logLength, log);
        CubismLogError("Shader compile log: %s", log);
        CSM_FREE(log);
    }

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glDeleteShader(shaderId);
        return false;
    }

    return true;
}
