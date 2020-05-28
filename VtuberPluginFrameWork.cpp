/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/
#include "VtuberPluginFrameWork.hpp"
#include "VtuberFrameWork.hpp"

namespace{
	struct Vtuber_data {
		obs_source_t *source;
		gs_texture_t *tex;
		int model_count;
		const char **modelFileName;
	};
}

const char * VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberGetName(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "Live 2D";
}

void * VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberCreate(obs_data_t *settings,
							   obs_source_t *source)
{
	int width = obs_data_get_int(settings, "width");
	int height = obs_data_get_int(settings, "height");
	double x = obs_data_get_double(settings, "x");
	double y = obs_data_get_double(settings, "y");
	double scale = obs_data_get_double(settings, "scale");

	Vtuber_data *spv = (Vtuber_data *)malloc(sizeof(Vtuber_data));
	VtuberFrameWork::InitVtuber(width, height);
	VtuberFrameWork::SetScale(scale);
	VtuberFrameWork::SetX(x);
	VtuberFrameWork::SetY(y);

	spv->source = source;
	spv->model_count = VtuberFrameWork::ModelNum();
	spv->modelFileName = VtuberFrameWork::GetModelFileName();

	obs_enter_graphics();

	spv->tex = gs_texture_create(width, height, GS_RGBA, 1, NULL,
				     GS_DYNAMIC);	
	obs_leave_graphics();

	return spv;
}

void VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberDestroy(void *data)
{
	VtuberFrameWork::UinitVtuber();

	Vtuber_data *spv = (Vtuber_data *)data;

	gs_texture_destroy(spv->tex);

	delete spv;
}

void VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberRender(
	void *data,
	gs_effect_t *effect)
{
	Vtuber_data *spv = (Vtuber_data *)data;

	int width, height;
	width = VtuberFrameWork::GetWidth();
	height = VtuberFrameWork::GetHeight();

	obs_enter_graphics();
	gs_texture_t *tex =gs_texture_create(width, height, GS_RGBA, 1, NULL, GS_DYNAMIC);	
	obs_leave_graphics();

	uint8_t *ptr;
	uint32_t linesize;
	if (gs_texture_map(tex, &ptr, &linesize))
		VtuberFrameWork::ReanderVtuber((char*) ptr);
	gs_texture_unmap(tex);
	
	obs_source_draw(tex, 0, 0, 0, 0, true);

	gs_texture_destroy(tex);
}

uint32_t VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberWidth(void *data)
{
	return VtuberFrameWork::GetWidth();
}

uint32_t VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberHeight(void *data)
{
	return VtuberFrameWork::GetHeight();
}

static void fill_vtuber_model_list(obs_property_t *p, void *data)
{
	Vtuber_data *vtb = (Vtuber_data *)data;
	int size = vtb->model_count;
	if (size>0)
	for (int i = 0; i < size; i++) {
		obs_property_list_add_string(p, vtb->modelFileName[i],vtb->modelFileName[i]);
	}
}

static bool vtuber_model_callback(obs_properties_t *props,
				  obs_property_t *property,
				  obs_data_t *settings){
	Vtuber_data *vtb = (Vtuber_data *)props;

	

	return true;
};

obs_properties_t * VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberGetProperties(void *data)
{

	Vtuber_data *vtb = (Vtuber_data *)data;

	obs_properties_t *ppts = obs_properties_create();
	obs_properties_set_param(ppts, vtb, NULL);

	obs_property_t *p;
	int width, height;
	width = VtuberFrameWork::GetWidth();
	height = VtuberFrameWork::GetHeight();

	p = obs_properties_add_list(ppts, "priority","Model",
				    OBS_COMBO_TYPE_LIST,
				    OBS_COMBO_FORMAT_STRING);
	fill_vtuber_model_list(p, vtb);
	
	obs_properties_add_int(ppts, "width", "Width", 128, 1280, 32);
	obs_properties_add_int(ppts, "height", "Height", 128, 1280, 32);
	obs_properties_add_float_slider(ppts,"scale","Scale",0.5,10.0,0.1);
	obs_properties_add_float_slider(ppts, "x", "x", -4.0, 4.0, 1);
	obs_properties_add_float_slider(ppts, "y", "y", -4.0, 4.0, 1);

	obs_property_set_modified_callback(p, vtuber_model_callback);

	return ppts;
}

void VtuberPluginFrameWork::VtuberPluginFrameWork::Vtuber_update(
	void *data, obs_data_t *settings)
{
	Vtuber_data *vtb = (Vtuber_data *)data;

	const char *vtb_str = obs_data_get_string(settings, "priority");
	double vscale = obs_data_get_double(settings, "scale");
	double x = obs_data_get_int(settings, "x");
	double y = obs_data_get_int(settings, "y");
	int width = obs_data_get_int(settings,"width");
	int height = obs_data_get_int(settings, "height");

	VtuberFrameWork::SetX(x);
	VtuberFrameWork::SetY(y);
	VtuberFrameWork::SetScale(vscale);
	VtuberFrameWork::Resize(width, height);

	int size = vtb->model_count;
	if (size>0)
	for (int i = 0; i < size; i++) {
		if (strcmp(vtb_str, vtb->modelFileName[i]) == 0) {
			VtuberFrameWork::ChangeModel(i);
			break;
		}
	}

}

void VtuberPluginFrameWork::VtuberPluginFrameWork::Vtuber_defaults(
	obs_data_t *settings)
{
	obs_data_set_default_int(settings, "width", 960);
	obs_data_set_default_int(settings, "height", 960);
	obs_data_set_default_double(settings, "x", 0);
	obs_data_set_default_double(settings, "y", 0);
	obs_data_set_default_double(settings, "scale", 2.0);
}
