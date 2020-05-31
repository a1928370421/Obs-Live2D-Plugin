
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include "CubismFramework.hpp"
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>

#define MAXVIEWDATA 1024

class LAppModel;

/**
* @brief 描画クラス
*/
struct ViewData {
	Csm::CubismViewMatrix *_viewMatrix;
	Csm::CubismMatrix44 *_deviceToScreen;
};

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

    void Initialize(int id);

    void Release(int id);

    void Render(int id);

    float TransformViewX(float deviceX,int id) const;

    float TransformViewY(float deviceY,int id) const;

    float TransformScreenX(float deviceX, int id) const;

    float TransformScreenY(float deviceY, int id) const;

    void PreModelDraw(LAppModel& refModel,int id);

    void PostModelDraw(LAppModel& refModel);

    void SwitchRenderingTarget(SelectTarget targetType);

    void SetRenderTargetClearColor(float r, float g, float b);

     Csm::CubismViewMatrix *GetViewMatrix(int id);

     Csm::CubismMatrix44 *GetDeviceToScreenMatrix(int id);

private:
    ViewData _viewData[MAXVIEWDATA];

    GLuint _programId;                       ///< シェーダID

    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 _renderBuffer;   ///< モードによってはCubismモデル結果をこっちにレンダリング
    SelectTarget _renderTarget;     ///< レンダリング先の選択肢
    float _clearColor[4];           ///< レンダリングターゲットのクリアカラー
};
