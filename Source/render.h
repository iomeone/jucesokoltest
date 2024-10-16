#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	void _sg_initialize();

	void _sg_shutdown();


	void _sg_render(int width, int height);

#ifdef __cplusplus
}
#endif