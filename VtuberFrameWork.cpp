/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/
#include "VtuberFrameWork.hpp"
#include"VtuberDelegate.hpp"
#include<mutex>

namespace {
    static bool isLoad;

    static bool isEnd;

    static bool isInit;

    static int initCount = 0;

    static std::mutex mut;
}

void VtuberFrameWork::InitVtuber(int id)
{
    mut.lock();
    initCount++;
    if (initCount == 0) {
	isInit = false;
	isEnd = false;
    }   
    isLoad = VtuberDelegate::GetInstance()->LoadResource(id);
    mut.unlock();
}

void VtuberFrameWork::ReanderVtuber(int targatid, char *data, int bufferWidth,
				    int bufferheight)
{
    mut.lock();

    if (isLoad) {
    
    if (initCount == 0 && isEnd) {
        VtuberDelegate::GetInstance()->Release();
        isEnd = false;
	goto end;
    }
    if (initCount>0&&!isInit) {
	    isInit = true;
	    VtuberDelegate::GetInstance()->Initialize();
    }

    if (isInit)
	VtuberDelegate::GetInstance()->Reader(targatid, data, bufferWidth,bufferheight);

    }
    end:
    mut.unlock();
}

void VtuberFrameWork::UinitVtuber(int id)
{
    mut.lock();    
    initCount--;
    if (isLoad && initCount == 0) {
	isEnd == true;
	isInit = false;	
    }
    VtuberDelegate::GetInstance()->ReleaseResource(id);
    mut.unlock();
}

void VtuberFrameWork::UpData(int id,double _x, double _y, int width, int height,
			     double sc,double _delayTime, bool randomMotion,const char *modelName)
{
	mut.lock();

	VtuberDelegate::GetInstance()->SetX(_x,id);
	VtuberDelegate::GetInstance()->SetY(_y, id);
	VtuberDelegate::GetInstance()->Resize(width, height,id);
	VtuberDelegate::GetInstance()->setScale(sc, id);
	VtuberDelegate::GetInstance()->ChangeModel(modelName,id);
	VtuberDelegate::GetInstance()->SetDelayTime(_delayTime,id);
	VtuberDelegate::GetInstance()->SetRandomMotion(randomMotion,id);

	mut.unlock();
}

int VtuberFrameWork::GetWidth(int id)
{    
    return VtuberDelegate::GetInstance()->getBufferWidth(id);
}

int VtuberFrameWork::GetHeight(int id)
{
	return VtuberDelegate::GetInstance()->getBufferHeight(id);
}

const char** VtuberFrameWork::GetModelFileName()
{
	return VtuberDelegate::GetInstance()->GetModelsName();
}

int VtuberFrameWork::ModelNum()
{
	return VtuberDelegate::GetInstance()->ModelCount();
}

double VtuberFrameWork::GetScale(int id)
{
	return VtuberDelegate::GetInstance()->getScale(id);
}

double VtuberFrameWork::GetX(int id)
{
	return VtuberDelegate::GetInstance()->GetX(id);
}

double VtuberFrameWork::GetY(int id)
{
	return VtuberDelegate::GetInstance()->GetY(id);
}





