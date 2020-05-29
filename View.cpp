/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

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

View::View():
    _programId(0), _renderTarget(SelectTarget_None)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    // デバイス座標からスクリーン座標に変換するための
    _deviceToScreen = new CubismMatrix44();

    // 画面の表示の拡大縮小や移動の変換を行う行列
    _viewMatrix = new CubismViewMatrix();
}

View::~View()
{
    _renderBuffer.DestroyOffscreenFrame();
    delete _viewMatrix;
    delete _deviceToScreen;
}

void View::Initialize()
{
    int width, height;

    width = VtuberDelegate::GetInstance()->getBufferWidth();
    height = VtuberDelegate::GetInstance()->getBufferHeight();

    if(width==0 || height==0)
    {
        return;
    }

    float ratio = static_cast<float>(height) / static_cast<float>(width);
    float left = ViewLogicalLeft;
    float right = ViewLogicalRight;
    float bottom = -ratio;
    float top = ratio;

    _viewMatrix->SetScreenRect(left, right, bottom, top); // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端

    float screenW = fabsf(left - right);
    _deviceToScreen->LoadIdentity(); // サイズが変わった際などリセット必須
    _deviceToScreen->ScaleRelative(screenW / width, -screenW / width);
    _deviceToScreen->TranslateRelative(-width * 0.5f, -height * 0.5f);

    // 表示範囲の設定
    _viewMatrix->SetMaxScale(ViewMaxScale); // 限界拡大率
    _viewMatrix->SetMinScale(ViewMinScale); // 限界縮小率

    // 表示できる最大範囲
    _viewMatrix->SetMaxScreenRect(
        ViewLogicalMaxLeft,
        ViewLogicalMaxRight,
        ViewLogicalMaxBottom,
        ViewLogicalMaxTop
    );
}

void View::Render()
{
    LAppLive2DManager* Live2DManager = LAppLive2DManager::GetInstance();

    int width, height;
    width = VtuberDelegate::GetInstance()->getBufferWidth();
    height = VtuberDelegate::GetInstance()->getBufferHeight();
    double x, y, scale;
    x = VtuberDelegate::GetInstance()->GetX();
    y = VtuberDelegate::GetInstance()->GetY();
    scale = VtuberDelegate::GetInstance()->getScale();
    _viewMatrix->Scale(scale, scale * static_cast<float>(width) /static_cast<float>(height));
    _viewMatrix->Translate(x, y);

    Live2DManager->OnUpdate();
   
}

float View::TransformViewX(float deviceX) const
{
    float screenX = _deviceToScreen->TransformX(deviceX);
    return _viewMatrix->InvertTransformX(screenX); 
}

float View::TransformViewY(float deviceY) const
{
    float screenY = _deviceToScreen->TransformY(deviceY);
    return _viewMatrix->InvertTransformY(screenY); 
}

float View::TransformScreenX(float deviceX) const
{
    return _deviceToScreen->TransformX(deviceX);
}

float View::TransformScreenY(float deviceY) const
{
    return _deviceToScreen->TransformY(deviceY);
}

void View::PreModelDraw(LAppModel& refModel)
{
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2* useTarget = NULL;

    if (_renderTarget != SelectTarget_None)
    {
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {
            int width, height;
	    width = VtuberDelegate::GetInstance()->getBufferWidth();
	    height = VtuberDelegate::GetInstance()->getBufferHeight();
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

Csm::CubismViewMatrix * View::GetViewMatrix()
{
	return _viewMatrix;
}

