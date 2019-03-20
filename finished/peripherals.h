/*
 * peripherals header file, adapted
 * from Confer's robots miniat example 
 * William Blair
 * 03/06/17 */

#include <vector>

#include <miniat.h>
#include "Sprite.h"

#ifndef _PERIPHERALS_H_INCLUDED_
#define _PERIPHERALS_H_INCLUDED_
 
/* port defines */
#define P_THRUST       (0x4000)
#define P_Y            (0x4001)
#define P_Y_VELOCITY   (0x4002)
#define P_LIVES        (0x4003)

#define P_VICTIM_DIST  (0x4010)
#define P_VICTIM_Y     (0x4011)

#define P_GROUND_DIST    (0x4020)
#define P_GROUND_HEIGHT  (0x4021)

#define P_CEIL_DIST      (0x4030)
#define P_CEIL_HEIGHT    (0x4031)

/* function declarations */
bool clock_peripherals(miniat *m, Sprite &heli, int &thrustY, double &vY,
                       unsigned int &lives,
/*                       Sprite &v1, Sprite &v2, Sprite &v3, Sprite &v4, Sprite &v5, Sprite &v6, Sprite &v7, Sprite &v8, Sprite &v9, Sprite &v10,*/
                       std::vector<Sprite> &victims,
                       Sprite *objarray, int objarray_length); // handles the miniat bus

int get_vic_dist_x(Sprite &heli, std::vector<Sprite> &victims ); // returns the number of pixels horizontally to the closest victim
int get_vic_dist_y(Sprite &heli, std::vector<Sprite> &victims ); // returns the number of pixels vertically to the closes victim

int get_ground_dist_x(Sprite &heli, Sprite *objarray, int objarray_length);
int get_ceil_dist_x(Sprite &heli, Sprite *objarray, int objarray_length);

int get_obj_ceil_y(Sprite &heli, Sprite *objarray, int objarray_length);
int get_obj_ground_y(Sprite &heli, Sprite *objarray, int objarray_length);
 
#endif
 
