#ifndef MAIN_H_
#define MAIN_H_

#include <API.h>

#ifdef __cplusplus
extern "C" {
#endif

extern Gyro gyro;
extern unsigned int imeCount;

void initializeIO();
void initialize();
void autonomous();
void operatorControl();

#ifdef __cplusplus
}
#endif

#endif // MAIN_H_
