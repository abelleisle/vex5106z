#ifndef MAIN_H_
#define MAIN_H_

#include <API.h>
#include <zephyr.h>

#ifdef __cplusplus
extern "C" {
#endif

void autonomous();
void initializeIO();
void initialize();
void operatorControl();

#ifdef __cplusplus
}
#endif

#endif // MAIN_H_
