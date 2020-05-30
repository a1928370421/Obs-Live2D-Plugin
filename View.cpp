#include "View.hpp"
#include <math.h>
#include <string>
#include "LAppPal.hpp"
#include "VtuberDelegate.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "LAppDefine.hpp"
#include "LAppModel.hpp"

using namespace std;
using namespace LAppDefine;

View::View() : _programId(0), _renderTarget(SelectTarget_None)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;
}

View::~View()
{
    _renderBuffer.DestroyOffscreenFrame();
}

void View::Initialize(int id)
{
    int width, height;

    width = VtuberDelegate::GetInstance()->getBufferWidth(id);
    height = VtuberDelegate::GetInstance()->getBufferHeight(id);

    if(width==0 || height==0)
    {
        return;
    }

    float ratio = static_cast<float>(height) / static_cast<float>(width);
    float left = ViewLogicalLeft;
    float right = ViewLogicalRight;
    float bottom = -ratio;
    float top = ratio;

    _viewData[id]._viewMatrix = new CubismViewMatrix();
    _viewData[id]._deviceToScreen = new CubismMatrix44();

    _viewData[id]._viewMatrix->SetScreenRect(
	    left, right, bottom,
	    top); // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端

    float screenW = fabsf(left - right);
    _viewData[id]._deviceToScreen->LoadIdentity(); // サイズが変わった際などリセット必須
    _viewData[id]._deviceToScreen->ScaleRelative(screenW / width,
						 -screenW / width);
    _viewData[id]._deviceToScreen->TranslateRelative(-width * 0.5f,
						     -height * 0.5f);

    // 表示範囲の設定
    _viewData[id]._viewMatrix->SetMaxScale(ViewMaxScale); // 限界拡大率
    _viewData[id]._viewMatrix->SetMinScale(ViewMinScale); // 限界縮小率

    // 表示できる最大範囲
    _viewData[id]._viewMatrix->SetMaxScreenRect(
        ViewLogicalMaxLeft,
        ViewLogicalMaxRight,
        ViewLogicalMaxBottom,
        ViewLogicalMaxTop
    );
}

void View::Release(int id) {
	delete _viewData[id]._viewMatrix;
	delete _viewData[id]._deviceToScreen;
}

void View::Render(int id)
{
    LAppLive2DManager* Live2DManager = LAppLive2DManager::GetInstance();
    int width, height;
    width = VtuberDelegate::GetInstance()->getBufferWidth(id);
    height = VtuberDelegate::GetInstance()->getBufferHeight(id);
    double x, y, scale;
    x = VtuberDelegate::GetInstance()->GetX(id);
    y = VtuberDelegate::GetInstance()->GetY(id);
    scale = VtuberDelegate::GetInstance()->getScale(id);

    _viewData[id]._viewMatrix->Scale(scale, scale * static_cast<float>(width) /static_cast<float>(height));
    _viewData[id]._viewMatrix->Translate(x, y);

    Live2DManager->OnUpdate(id);
   
}

float View::TransformViewX(float deviceX,int id) const
{
	float screenX = _viewData[id]._deviceToScreen->TransformX(deviceX);
	return _viewData[id]._viewMatrix->InvertTransformX(screenX);
}

float View::TransformViewY(float deviceY,int id) const
{
	float screenY = _viewData[id]._deviceToScreen->TransformY(deviceY);
	return _viewData[id]._viewMatrix->InvertTransformY(screenY);
}

float View::TransformScreenX(float deviceX,int id) const
{
	return _viewData[id]._deviceToScreen->TransformX(deviceX);
}

float View::TransformScreenY(float deviceY,int id) const
{
	return _viewData[id]._deviceToScreen->TransformY(deviceY);
}

void View::PreModelDraw(LAppModel& refModel,int id)
{
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2* useTarget = NULL;

    if (_renderTarget != SelectTarget_None)
    {
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {
            int width, height;
	    width = VtuberDelegate::GetInstance()->getBufferWidth(id);
	    height = VtuberDelegate::GetInstance()->getBufferHeight(id);
            if (width != 0 && height != 0)
            {
                
                useTarget->CreateOffscreenFrame(static_cast<csmUint32>(width), static_cast<csmUint32>(height));
            }
        }

        useTarget->BeginDraw();
        useTarget->Clear(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]); 
    } else
    {
	    glClearColor(_clearColor[0], _clearColor[1], _clearColor[2],_clearColor[3]);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void View::PostModelDraw(LAppModel& refModel)
{
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2* useTarget = NULL;

    if (_renderTarget != SelectTarget_None)
    {
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();
     
        useTarget->EndDraw();
    } 
}

void View::SwitchRenderingTarget(SelectTarget targetType)
{
    _renderTarget = targetType;
}

void View::SetRenderTargetClearColor(float r, float g, float b)
{
    _clearColor[0] = r;
    _clearColor[1] = g;
    _clearColor[2] = b;
}

Csm::CubismViewMatrix * View::GetViewMatrix(int id)
{
	return _viewData[id]._viewMatrix;
}

