/*
 * ports header - function declarations
 * for gpio port handling 
 * William Blair 
 * 03/08/17 */

#include <miniat.h>
 
#ifndef _PORTS_H_INCLUDED_
#define _PORTS_H_INCLUDED_

void clock_ports(miniat *m, double vY, int thrustY, int max_vel); // updates the gpio pins

#endif

