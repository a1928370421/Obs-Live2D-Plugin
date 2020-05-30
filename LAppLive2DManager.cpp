/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppLive2DManager.hpp"
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Rendering/CubismRenderer.hpp>
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "VtuberDelegate.hpp"
#include "LAppModel.hpp"
#include "View.hpp"

//#define USE_RENDER_TARGET

using namespace Csm;
using namespace LAppDefine;
using namespace std;

namespace {
    LAppLive2DManager* s_instance = NULL;

    void FinishedMotion(ACubismMotion* self)
    {
        LAppPal::PrintLog("Motion Finished: %x", self);
    }
}

LAppLive2DManager* LAppLive2DManager::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new LAppLive2DManager();
    }

    return s_instance;
}

void LAppLive2DManager::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

LAppLive2DManager::LAppLive2DManager()
{
	ChangeScene(0,0);
}

LAppLive2DManager::~LAppLive2DManager()
{
	for (size_t i = 0; i < 16; i++) {
		ReleaseAllModel(i);
	}
    
}

void LAppLive2DManager::ReleaseAllModel(Csm::csmUint16 id)
{
    for (csmUint32 i = 0; i < _modeldata[id]._models.GetSize(); i++)
    {
		delete _modeldata[id]._models[i];
    }
    _modeldata[id]._models.Clear();
}

void LAppLive2DManager::ReleaseModel(Csm::csmUint16 id) {
	for (csmUint32 i = 0; i < _modeldata[id]._models.GetSize();
	     i++) {
		delete _modeldata[id]._models[i];
	}
	_modeldata[id]._models.Clear();
}

LAppModel *LAppLive2DManager::GetModel(Csm::csmUint16 id) const
{
    if (0 < _modeldata[id]._models.GetSize())
    {
	return _modeldata[id]._models[0];
    }
    return NULL;
}

void LAppLive2DManager::OnUpdate(Csm::csmUint16 id) const
{    
    CubismMatrix44 projection;
    CubismMatrix44 *_viewMatrix =VtuberDelegate::GetInstance()->GetView()->GetViewMatrix(id);

    if (_viewMatrix != NULL)
    {
	projection.MultiplyByMatrix(_viewMatrix);	
    }

        LAppModel* model = GetModel(id);

	if (model) {
		VtuberDelegate::GetInstance()->GetView()->PreModelDraw(*model,id);
	
		model->SetRandomMotion(VtuberDelegate::GetInstance()->GetRandomMotion(id));
		model->SetDelayTime(VtuberDelegate::GetInstance()->GetDelayTime(id));
		model->Update();
		model->Draw(projection);

		VtuberDelegate::GetInstance()->GetView()->PostModelDraw(*model);
	}
}

void LAppLive2DManager::ChangeScene(Csm::csmInt32 index, Csm::csmInt16 _id)
{
    _modeldata[_id]._sceneIndex = index;
    if (DebugLogEnable)
    {
        //LAppPal::PrintLog("[APP]model index: %d", _sceneIndex);
    }
    const char **ModelDir = VtuberDelegate::GetInstance()->GetModelsName();
    // ModelDir[]に保持したディレクトリ名から
    // model3.jsonのパスを決定する.
    // ディレクトリ名とmodel3.jsonの名前を一致させておくこと.
    std::string model = ModelDir[index];
    std::string modelPath = ResourcesPath + model + "/";
    std::string modelJsonName = ModelDir[index];
    modelJsonName += ".model3.json";

    //delete _models[id];
    ReleaseAllModel(_id);
    _modeldata[_id]._models.PushBack(new LAppModel());
    _modeldata[_id]._models[0]->LoadAssets(modelPath.c_str(),modelJsonName.c_str());

    /*
     * モデル半透明表示を行うサンプルを提示する。
     * ここでUSE_RENDER_TARGET、USE_MODEL_RENDER_TARGETが定義されている場合
     * 別のレンダリングターゲットにモデルを描画し、描画結果をテクスチャとして別のスプライトに張り付ける。
     */
    {
#if defined(USE_RENDER_TARGET)
        // Viewの持つターゲットに描画を行う場合、こちらを選択
        View::SelectTarget useRenderTarget = View::SelectTarget_ViewFrameBuffer;
#elif defined(USE_MODEL_RENDER_TARGET)
        // 各LAppModelの持つターゲットに描画を行う場合、こちらを選択
        View::SelectTarget useRenderTarget = View::SelectTarget_ModelFrameBuffer;
#else
        // デフォルトのメインフレームバッファへレンダリングする(通常)
        View::SelectTarget useRenderTarget = View::SelectTarget_None;
#endif

#if defined(USE_RENDER_TARGET) || defined(USE_MODEL_RENDER_TARGET)
        // モデル個別にαを付けるサンプルとして、もう1体モデルを作成し、少し位置をずらす
        _models.PushBack(new LAppModel());
        _models[1]->LoadAssets(modelPath.c_str(), modelJsonName.c_str());
        _models[1]->GetModelMatrix()->TranslateX(0.2f);
#endif

        VtuberDelegate::GetInstance()->GetView()->SwitchRenderingTarget(
		useRenderTarget);

        // 別レンダリング先を選択した際の背景クリア色
        float clearColor[3] = { 1.0f, 1.0f, 1.0f };
	VtuberDelegate::GetInstance()->GetView()->SetRenderTargetClearColor(
		clearColor[0], clearColor[1], clearColor[2]);
    }
}


