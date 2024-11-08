#pragma once


#include <string>
#include <map>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

	void _sg_initialize(int w, int h, const std::map<std::string, std::pair<size_t, std::vector<unsigned char>>>& fontMap);

	void _sg_shutdown();


	void _sg_render(int width, int height);

#ifdef __cplusplus
}
#endif