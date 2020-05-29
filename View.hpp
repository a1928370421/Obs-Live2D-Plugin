/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include "CubismFramework.hpp"
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>

class LAppModel;

/**
* @brief 描画クラス
*/
class View
{
public:

    enum SelectTarget
    {
        SelectTarget_None,                ///< デフォルトのフレームバッファにレンダリング
        SelectTarget_ModelFrameBuffer,    ///< LAppModelが各自持つフレームバッファにレンダリング
        SelectTarget_ViewFrameBuffer,     ///< Viewの持つフレームバッファにレンダリング
    };

    View();

    ~View();

    void Initialize();

    void Render();

    float TransformViewX(float deviceX) const;

    float TransformViewY(float deviceY) const;

    float TransformScreenX(float deviceX) const;

    float TransformScreenY(float deviceY) const;

    void PreModelDraw(LAppModel& refModel);

    void PostModelDraw(LAppModel& refModel);

    void SwitchRenderingTarget(SelectTarget targetType);

    void SetRenderTargetClearColor(float r, float g, float b);

     Csm::CubismViewMatrix *GetViewMatrix();

private:
    Csm::CubismMatrix44* _deviceToScreen;    ///< デバイスからスクリーンへの行列
    Csm::CubismViewMatrix* _viewMatrix;      ///< viewMatrix
    GLuint _programId;                       ///< シェーダID

    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 _renderBuffer;   ///< モードによってはCubismモデル結果をこっちにレンダリング
    SelectTarget _renderTarget;     ///< レンダリング先の選択肢
    float _clearColor[4];           ///< レンダリングターゲットのクリアカラー
};
