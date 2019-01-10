/*
 *  *********************************************
 *  Usage : replace the main() as following 
 *  *********************************************
 *
// defined "_useMyMain01" inside the following h file
#include    "ap_main_patch01.h"

int16 main(int16 param)
{
#ifdef _useMyMain01
    _mPatch01();
}
int16 main_fake(int16 param)
{
#endif
*
*/

#ifndef     _useMyMain01
#define     _useMyMain01
void _mPatch01(void) {
    while ( 1 ) {
        ClearWatchDog();
    }
} // _mPatch01
#endif
