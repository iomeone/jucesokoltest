#pragma once


enum EEventType
{
	e_key_unknow = 0,
	e_key_down = 1,
	e_key_up = 2,
	e_mouse_down = 3,
	e_mouse_up = 4,
	e_mouse_scroll =5,
	e_mouse_move = 6,
};


enum EMouseButtonType
{
	e_mouse_none = 0,
	e_mouse_left = 1,
	e_mouse_middle = 2,
	e_mouse_right = 3,
};



struct TKeyEvent
{
	EEventType type= e_key_unknow;
	int key_code;
	EMouseButtonType mouse_button = e_mouse_none; 
	float mouse_dx = 0.0;
	float mouse_dy = 0.0;
	float scroll_y = 0.0;
 
};