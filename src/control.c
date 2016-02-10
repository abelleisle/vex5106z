#include <main.h>

void setEvent(Controller *c){
	c->left.stick.x = joystickGetAnalog(c->num, 4);
	c->left.stick.y = joystickGetAnalog(c->num, 3);

	c->right.stick.x = joystickGetAnalog(c->num, 1);
	c->right.stick.y = joystickGetAnalog(c->num, 2);

	/**
	 * BACK RIGHT BUTTONS
	 */

	if(c->right.back.d == KEY_UP)c->right.back.d = UP;
	else if(c->right.back.d == DOWN && !joystickGetDigital(c->num, 6, JOY_DOWN))c->right.back.d = KEY_UP;
	else c->right.back.d = joystickGetDigital(c->num, 6, JOY_DOWN);

	if(c->right.back.u == KEY_UP)c->right.back.u = UP;
	else if(c->right.back.u == DOWN && !joystickGetDigital(c->num, 6, JOY_UP))c->right.back.u = KEY_UP;
	else c->right.back.u = joystickGetDigital(c->num, 6, JOY_UP);


	/*
	 * BACK LEFT BUTTONS
	 */
	if(c->left.back.d == KEY_UP)c->left.back.d = UP;
	else if(c->left.back.d == DOWN && !joystickGetDigital(c->num, 5, JOY_DOWN))c->left.back.d = KEY_UP;
	else c->left.back.d = joystickGetDigital(c->num, 5, JOY_DOWN);

	if(c->left.back.u == KEY_UP)c->left.back.u = UP;
	else if(c->left.back.u == DOWN && !joystickGetDigital(c->num, 5, JOY_UP))c->left.back.u = KEY_UP;
	else c->left.back.u = joystickGetDigital(c->num, 5, JOY_UP);


	/*
	 * LEFT FRONT BUTTONS
	 */
	if(c->left.front.l == KEY_UP)c->left.front.l = UP;
	else if(c->left.front.l == DOWN && !joystickGetDigital(c->num, 7, JOY_LEFT))c->left.front.l = KEY_UP;
	else c->left.front.l = joystickGetDigital(c->num, 7, JOY_LEFT);

	if(c->left.front.r == KEY_UP)c->left.front.r = UP;
	else if(c->left.front.r == DOWN && !joystickGetDigital(c->num, 7, JOY_RIGHT))c->left.front.r = KEY_UP;
	else c->left.front.r = joystickGetDigital(c->num, 7, JOY_RIGHT);

	if(c->left.front.u == KEY_UP)c->left.front.u = UP;
	else if(c->left.front.u == DOWN && !joystickGetDigital(c->num, 7, JOY_UP))c->left.front.u = KEY_UP;
	else c->left.front.u = joystickGetDigital(c->num, 7, JOY_UP);

	if(c->left.front.d == KEY_UP)c->left.front.d = UP;
	else if(c->left.front.d == DOWN && !joystickGetDigital(c->num, 7, JOY_DOWN))c->left.front.d = KEY_UP;
	else c->left.front.d = joystickGetDigital(c->num, 7, JOY_DOWN);

	/*
	 * RIGHT FRONT BUTTONS
	 */
	if(c->right.front.d == KEY_UP)c->right.front.d = UP;
	else if(c->right.front.d == DOWN && !joystickGetDigital(c->num, 8, JOY_DOWN))c->right.front.d = KEY_UP;
	else c->right.front.d = joystickGetDigital(c->num, 8, JOY_DOWN);

	if(c->right.front.l == KEY_UP)c->right.front.l = UP;
	else if(c->right.front.l == DOWN && !joystickGetDigital(c->num, 8, JOY_LEFT))c->right.front.l = KEY_UP;
	else c->right.front.l = joystickGetDigital(c->num, 8, JOY_LEFT);

	if(c->right.front.u == KEY_UP)c->right.front.u = UP;
	else if(c->right.front.u == DOWN && !joystickGetDigital(c->num, 8, JOY_UP))c->right.front.u = KEY_UP;
	else c->right.front.u = joystickGetDigital(c->num, 8, JOY_UP);

	if(c->right.front.r == KEY_UP)c->right.front.r = UP;
	else if(c->right.front.r == DOWN && !joystickGetDigital(c->num, 8, JOY_RIGHT))c->right.front.r = KEY_UP;
	else c->right.front.r = joystickGetDigital(c->num, 8, JOY_RIGHT);
}
