#include <obs-module.h>
#include "VtuberPluginFrameWork.hpp"

OBS_DECLARE_MODULE()

MODULE_EXPORT const char *obs_module_description(void)
{
	return "Vtuber";
}

bool obs_module_load(void)
{
	obs_source_info Vtuber_video{
	};
	Vtuber_video.id = "Vtuber";
	Vtuber_video.type = OBS_SOURCE_TYPE_INPUT;
	Vtuber_video.output_flags = OBS_SOURCE_VIDEO;
	Vtuber_video.get_name =
		VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberGetName;
	Vtuber_video.create =
		VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberCreate;
	Vtuber_video.destroy =
		VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberDestroy;
	Vtuber_video.video_render =
		VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberRender;
	Vtuber_video.get_width =
		VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberWidth;
	Vtuber_video.get_height =
		VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberHeight;
	Vtuber_video.get_properties =
		VtuberPluginFrameWork::VtuberPluginFrameWork::VtuberGetProperties;
	Vtuber_video.update =
		VtuberPluginFrameWork::VtuberPluginFrameWork::Vtuber_update;
	Vtuber_video.get_defaults =
		VtuberPluginFrameWork::VtuberPluginFrameWork::Vtuber_defaults;

	obs_register_source(&Vtuber_video);

	return true;
}

#ifdef _WIN32
void obs_module_unload(void)
{

}
#endif
