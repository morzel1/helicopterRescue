/*
 * ports implementation - functions
 * for gpio port handling 
 * William Blair 
 * 03/08/17 */

#include "ports.h"

/*
 * clock_ports - sets the values on output
 * pins for the miniat based on current
 * values in the simulation
 * William Blair
 * 03/08/17
 * */
void clock_ports(miniat *m, double vY, int thrustY, int max_vel)
{
	/* initialize the port union (.u) */
	static m_wyde port_a = { .u = 0 };
	
	/* test the required values */
	unsigned int moving = (vY != 0);
	
	unsigned int accel  = (thrustY != 0 && // if thrust is being applied and the helicopter isn't at its max velocity
	                      ((vY != (-1*max_vel)) && (vY != max_vel)) );
	
	/* set each bit of the port */
	port_a.bits.bit0 = moving;// moving
	port_a.bits.bit1 = accel;// accelerating
	
	/* apply the port to the miniat */
	miniat_pins_set_gpio_port(m, m_gpio_id_A, port_a);
	
	return;
}
