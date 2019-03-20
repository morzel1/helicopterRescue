/*
 * peripherals implementation file, adapted
 * from Confer's robots miniat example 
 * William Blair
 * 03/06/17 */

#include "peripherals.h"

#include <iostream>
#include <vector>

/* defines from main.cpp */
const int S_WIDTH  = 1024; // window dimensions
const int S_HEIGHT = 768;

/*
 * William Blair 
 * 03/06/17
 * clock_peripherals - updates the miniat bus each cycle */
bool clock_peripherals(miniat *m, Sprite &heli, int &thrustY, double &vY,
                       unsigned int &lives,
/*                       Sprite &v1, Sprite &v2, Sprite &v3, Sprite &v4, Sprite &v5, Sprite &v6, Sprite &v7, Sprite &v8, Sprite &v9, Sprite &v10,*/
					   std::vector<Sprite> &victims,
                       Sprite *objarray, int objarray_length)
{
	m_bus bus; // an object of the current miniat's peripheral bus
	
	/* make sure miniat isn't null when given */
	if(!m){
		std::cerr << "MiniAT NULL this clock cycle!\n";
		return false;
	}
	
	/* get the current miniat bus */
	bus = miniat_pins_bus_get(m);
	
	/* check if there has been an acknowledgement request */
	if(bus.req == M_HIGH && bus.ack == M_LOW)
	{
		bool handled = true; // to show that yep, we've handled the request
		if(bus.rW == M_HIGH)
		{
			/* store data in the different peripherals here */
			switch(bus.address)
			{
				case P_THRUST: // in our case only the thrust is writable
					std::cout << "\nSTORed to the P_THRUST peripheral\n";
					
					/* test if the new thrust is valid */
					if( (signed)bus.data < -100 ){
						std::cerr << "Invalid Thrust: " << (signed)bus.data << ", Must be between [-100, 100]\n";
						std::cerr << "Setting to -100\n";
						thrustY = -1 * -100;
					}
					else if( (signed)bus.data > 100 ){
						std::cerr << "Invalid Thrust: " << (signed)bus.data << ", Must be between [-100, 100]\n";
						std::cerr << "Setting to 100\n";
						thrustY = -1 * 100;
					}
					else{
						/* negated to compensate for confer's assembly 
						 * this makes positive thrust make the helicopter go up instead of down */
						thrustY = -1 * (int)((signed)bus.data);
						//thrustY = (int)((signed)bus.data);
						std::cout << "Address = 0x" << std::hex << bus.address <<
						std::dec << "\tData = " << (signed int)bus.data << std::endl;
					}
					break;
				default:
					std::cerr << "Invalid STOR address: " << bus.address << std::endl;
					
					/* Maybe set handled here to false but confers example doesn't have it built in */
					
					break;
			}
		}
		else
		{
			/* LOAD data from peripherals */
			switch(bus.address)
			{
				/* read the current y thrust */
				case P_THRUST:
					std::cout << "\nLOADed from P_THRUST\n";
					/* invert to give illusion of reversed up and down (since sdl up/down is reverse) */
					bus.data = thrustY;
					std::cout << "Address = 0x" << std::hex << bus.address <<
					std::dec << "\tData = " << (signed int)bus.data << std::endl;
					break;
					
				/* read the current helicopter y position */
				case P_Y:
					std::cout << "\nLOADed from P_Y\n";
					//commented to adapt to confer's code
					bus.data = S_HEIGHT - heli.getY();
					//bus.data = heli.getY();
					std::cout << "Address = 0x" << std::hex << bus.address <<
					std::dec << "\tData = " << bus.data << std::endl;
					break;
				/* read the current helicopter y velocity */
				case P_Y_VELOCITY:
					std::cout << "\nLOADed from P_Y_VELOCITY\n";
					// commenting to change to negative to follow confer's assembly
					//bus.data = vY;
					bus.data = -1* vY;
					std::cout << "Address = 0x" << std::hex << bus.address <<
					std::dec << "\tData = " << (signed)bus.data << std::endl;
					break;
				
				/* read the current number of lives the player has */
				case P_LIVES:
					std::cout << "\nLOADed from P_LIVES\n";
					bus.data = lives;
					std::cout << "Address = 0x" << std::hex << bus.address <<
					std::dec << "\tData = " << bus.data << std::endl;
					break;
					
				/* TODO - make me less horribly inefficient! */
				case P_VICTIM_DIST:
					std::cout << "\nLOADed from P_VICTIM_DIST\n";
					bus.data = get_vic_dist_x(heli, victims);
					std::cout << "Address = 0x" << std::hex << bus.address <<
					std::dec << "\tData = " << (signed)bus.data << std::endl;
					break;
				case P_VICTIM_Y:
					std::cout << "\nLOADed from P_VICY\n";
					bus.data = get_vic_dist_y(heli, victims);
					std::cout << "Address = 0x" << std::hex << bus.address <<
					std::dec << "\tData = " << (signed)bus.data << std::endl;
					break;

				case P_GROUND_DIST:
					std::cout << "\nLOADed from P_GROUNDIS\n";
					bus.data = get_ground_dist_x(heli, objarray, objarray_length);
					std::cout << "Address = 0x" << std::hex << bus.address <<
					std::dec << "\tData = " << (signed)bus.data << std::endl;
					break;
				case P_GROUND_HEIGHT:
					std::cout << "\nLOADed from P_GROUND_H\n";
					bus.data = get_obj_ground_y(heli, objarray, objarray_length);
					std::cout << "Address = 0x" << std::hex << bus.address <<
					std::dec << "\tData = " << (signed)bus.data << std::endl;
					break;
				case P_CEIL_DIST:
					std::cout << "\nLOADed from P_CEIL_DIST\n";
					bus.data = get_ceil_dist_x(heli, objarray, objarray_length);
					std::cout << "Address = 0x" << std::hex << bus.address <<
					std::dec << "\tData = " << (signed)bus.data << std::endl;
					break;
				case P_CEIL_HEIGHT:
					std::cout << "\nLOADed from P_CEIL_HEIGHT\n";
					bus.data = get_obj_ceil_y(heli, objarray, objarray_length);
					std::cout << "Address = 0x" << std::hex << bus.address <<
					std::dec << "\tData = " << (signed)bus.data << std::endl;
					break;
				default:
					std::cerr << "Invalid address to read from: " << bus.address << std::endl;
					break;
			}
		}
		
		/* if no errors during handling, set the acknowledge pin to HIGH */
		if(handled){
			bus.ack = M_HIGH;
		}
	}
	else if(bus.ack){
		/* this is where if you want to add delay to some peripherals 
		 * you would do that here */
		switch(bus.address)
		{
			default:
				bus.ack = M_LOW;
				break;
		}
	}
	
	/* finally write the new bus to the miniat */
	miniat_pins_bus_set(m, bus);
	
	return true;
}

/* 
 * William Blair
 * 03/27/17
 * get_vic_dist_x
 * 
 * atrociously inefficient check of which victim is closest
 * */
int get_vic_dist_x(Sprite &heli, std::vector<Sprite> &victims) // returns the number of pixels horizontally to the closest victim
{
	/* add them all to a vector for ease of access */
	/*std::vector<int> victims_x;
	victims_x.push_back(v1.getX());
	victims_x.push_back(v2.getX());
	victims_x.push_back(v3.getX());
	victims_x.push_back(v4.getX());
	victims_x.push_back(v5.getX());
	victims_x.push_back(v6.getX());
	victims_x.push_back(v7.getX());
	victims_x.push_back(v8.getX());
	victims_x.push_back(v9.getX());
	victims_x.push_back(v10.getX());
	*/
	int shortest_distance = 1000000; // the current shortest distance to a victim - updated in the for loop below
	                                 // arbitrarily large number to make sure the first victim checked will always be closer
	
	/* loops through each victim's x coordinate */
	for(int i=0; i<victims.size(); i++)
	{
		if( victims[i].getX() > (heli.getX() + heli.getW()) ) // make sure the victim is in front of the helicopter
		{
			if( victims[i].getX() - (heli.getX() + heli.getW()) < shortest_distance ){ // if the distance between them is smaller than the current smallest, 
				shortest_distance = victims[i].getX() - (heli.getX() + heli.getW());   // update the current smallest to the distance between them
			}
		}
	}

	return shortest_distance; // return the distance which is the shortest
}

/*
 * William Blair
 * 03/29/17
 * get_vic_dist_y
 * 
 * returns the number of pixels from either the top or the bottom of the helicopter to the nearest victim
 * if the victim is avove the helicopter the top is used, if the victim is below the helicopter the bottom is used
 * 
 * */
int get_vic_dist_y(Sprite &heli, std::vector<Sprite> &victims) // returns the number of pixels vertically to the closes victim
{
	/* first we need to figure out which victim is the closest horizontally 
	 * copied from get_vic_dist_x */
	 /* add them all to a vector for ease of access */
	/*std::vector<int> victims_x;
	victims_x.push_back(v1.getX());
	victims_x.push_back(v2.getX());
	victims_x.push_back(v3.getX());
	victims_x.push_back(v4.getX());
	victims_x.push_back(v5.getX());
	victims_x.push_back(v6.getX());
	victims_x.push_back(v7.getX());
	victims_x.push_back(v8.getX());
	victims_x.push_back(v9.getX());
	victims_x.push_back(v10.getX());
	*/
	int shortest_x = 0; // the position in the vector of the closest victim horizontally
	int shortest_x_dist = 1000000; // the distance between the helicopter and the closest victim horizontally
	int vic_height = victims[0].getH();    // get the height of the victims - to be used with difference between the helicopter top and the bottom of the victim
	
	/* loops through each victim's x coordinate */
	for(int i=0; i<victims.size(); i++)
	{
		if( victims[i].getX() > (heli.getX() + heli.getW()) ) // make sure the victim is in front of the helicopter
		{
			if( victims[i].getX() - (heli.getX() + heli.getW()) < shortest_x_dist ){ // if the distance between them is smaller than the current smallest, 
				shortest_x_dist = victims[i].getX() - (heli.getX() + heli.getW());   // update the current smallest to the distance between them
				shortest_x = i;                                                 // update the current smallest distance position in the vector
			}
		}
	}

	/* add all of the y coordinates of the victims to a vector for ease of access */
	/*std::vector<int> victims_y;
	victims_y.push_back(v1.getY());
	victims_y.push_back(v2.getY());
	victims_y.push_back(v3.getY());
	victims_y.push_back(v4.getY());
	victims_y.push_back(v5.getY());
	victims_y.push_back(v6.getY());
	victims_y.push_back(v7.getY());
	victims_y.push_back(v8.getY());
	victims_y.push_back(v9.getY());
	victims_y.push_back(v10.getY());
	*/
	int shortest_dist_y = 1000000; // the current shortest distance to a victim - updated in the for loop below
	                                 // arbitrarily large number to make sure the first victim checked will always be closer
	
	/* now take the victim that is closest horizontally and return the 
	 * vertical distance between them */
	if( victims[shortest_x].getY() > heli.getY() ) // if the victim is below the top of the helicopter
	{
		shortest_dist_y = -1*(victims[shortest_x].getY() - (heli.getY() + heli.getH())); // the distance between the top of the victim and the bottom of the helicopter
	}
	else                                      // if the victim is above the top of the helicopter or equal to
	{
		//shortest_dist_y = (victims_y[shortest_x] + vic_height) - heli.getY(); // should be a negative number
		shortest_dist_y = heli.getY() - (victims[shortest_x].getY() + vic_height); // should be a negative number
	}
	
	return shortest_dist_y;
}

/*
 * William Blair
 * 03/29/17
 * get_obj_dist_x
 * 
 * returns the distance to the closest ground
 * building in front of the helicopter
 * 
 * if there isn't one on screen or the helicopter
 * isn't in line to hit it, returns -1 */
int get_ground_dist_x(Sprite &heli, Sprite *objarray, int objarray_length)
{
	int shortest_distance = 1000000; // the current shortest distance to a victim - updated in the for loop below
	                                 // arbitrarily large number to make sure the first victim checked will always be closer
	int shortest_array_pos = -1;
	
	/* find the closest ground building x wise to the 
	 * helicopter */
	for(int i=0; i<objarray_length; i++)
	{
		if( (objarray+i)->getX() > 0 && (objarray+i)->getX() < 1024 && // make sure the building is on screen
		    (objarray+i)->getY() > heli.getH() && // make sure the building is at least lower than the possible distance the helicopter could fit through between the top of the window and the building
		    (objarray+i)->getX() > heli.getX() + heli.getW()   // make sure the helicopter hasn't started to pass the building
		  )
		{
			if( (objarray+i)->getX() - (heli.getX() + heli.getW()) < shortest_distance &&
			    (objarray+i)->getX() - (heli.getX() + heli.getW()) > 0 ){ // if the distance between them is smaller than the current smallest, 
				shortest_distance = objarray[i].getX() - (heli.getX() + heli.getW());   // update the current smallest to the distance between them
			}
		}
	}
	
	/* if the helicopter isn't in line to hit the closest building, return -1 */
	if( shortest_array_pos != -1 && heli.getY() + heli.getH() < (objarray+shortest_array_pos)->getY() ){
		shortest_distance = -1;
	}
	if( shortest_distance == 1000000 ){
		shortest_distance = -1;
	}
	
	return shortest_distance;
}

/*
 * William Blair
 * 04/26/17
 * get_ceil_dist_x
 * 
 * returns the distance between the front of the helicopter 
 * and the closest ceiling bulding/object 
 * 
 * if the helicopter is underneath the building, it is ignored
 * and the next building in front of it is used
 * 
 * if the helicopter isn't in line to hit the nearest top
 * building, it returns -1 */
int get_ceil_dist_x(Sprite &heli, Sprite *objarray, int objarray_length)
{
	int shortest_distance = 1000000; // the current shortest distance to a victim - updated in the for loop below
	                                 // arbitrarily large number to make sure the first victim checked will always be closer
	int shortest_array_pos = -1;
	
	/* get the shortest horizontal distance to a ceiling building that's
	 * in front of the helicopter */
	for(int i=0; i<objarray_length; i++)
	{
		if( (objarray+i)->getY() <= 0 &&                                // make sure the building is a ceiling building
		    (objarray+i)->getX() > 0 && (objarray+i)->getX() < 1024 &&  // make sure the building is on the screen currently
		    (objarray+i)->getX() > heli.getX()+heli.getW() )            // make sure the building is in front of the helicopter
		{
			if( (objarray+i)->getX() - (heli.getX() + heli.getW()) < shortest_distance &&
			    (objarray+i)->getX() - (heli.getX() + heli.getW()) > 0 ){ // if the distance between them is smaller than the current smallest, 
				shortest_distance = objarray[i].getX() - (heli.getX() + heli.getW());   // update the current smallest to the distance between them
				shortest_array_pos = i;
			}
		}
	}
	
	/* make sure the helicopter is vertically in line to hit the building 
	 * if the top of the helicopter is farther down than the bottom of the
	 * screen, then it's not going to hit it so return -1*/
	//if( shortest_array_pos >= 0 && heli.getY() > (objarray+shortest_array_pos)->getY() + (objarray+shortest_array_pos)->getH()) {
	//	shortest_distance = -1;
	//}
	
	if( shortest_distance == 1000000 ) shortest_distance = -1;
	
	return shortest_distance;
}

/*
 * William Blair
 * 03/29/17
 * get_obj_ceil_y
 * 
 * returns the distance between the nearest ceiling
 * building and the bottom of the helicopter, if it's in line
 * to hit it
 * 
 * if it isn't in line to hit one, or if the closest
 * building is a ground building it returns -1
 * */
int get_obj_ceil_y(Sprite &heli, Sprite *objarray, int objarray_length)
{
	int closest_ground_build_x = get_ground_dist_x(heli, objarray, objarray_length);
	if( closest_ground_build_x == -1 ) closest_ground_build_x = 1000000;
	
	int shortest_distance = 1000000; // the current shortest distance to a victim - updated in the for loop below
	                                 // arbitrarily large number to make sure the first victim checked will always be closer
	int shortest_array_pos = -1;
	
	/* get the shortest horizontal distance to a ceiling building that's
	 * in front of the helicopter */
	for(int i=0; i<objarray_length; i++)
	{
		if( (objarray+i)->getY() <= 0 &&                                // make sure the building is a ceiling building
		    (objarray+i)->getX() > 0 && (objarray+i)->getX() < 1024 &&  // make sure the building is on the screen currently
		    (objarray+i)->getX() > heli.getX()+heli.getW() )            // make sure the building is in front of the helicopter
		{
			if( (objarray+i)->getX() - (heli.getX() + heli.getW()) < shortest_distance ){ // if the distance between them is smaller than the current smallest, 
				shortest_distance = objarray[i].getX() - (heli.getX() + heli.getW());   // update the current smallest to the distance between them
				shortest_array_pos = i;
			}
		}
	}
	
	/* if there is a closer ground building, return -1 */
	if( shortest_distance != 1000000 &&
	    closest_ground_build_x < shortest_distance ){
		return -1;
	}
	
	/* make sure the helicopter is vertically in line to hit the building 
	 * if the top of the helicopter is farther down than the bottom of the
	 * screen, then it's not going to hit it so return -1*/
	
	if( shortest_array_pos != -1 ){
		shortest_distance = ((objarray+shortest_array_pos)->getY() + (objarray+shortest_array_pos)->getH()) - heli.getY();
	}
	else if(shortest_distance == 1000000){
		shortest_distance = -1;
	}
	
	return shortest_distance;
}

/*
 * William Blair
 * CS 370
 * 04/26/17
 * 
 * get_obj_ground_y - returns the distance from the bottom of the
 * helicopter to the top of the closest ground building (-1 if
 * the helicopter won't hit the closest ground building, or if the
 * closest building is a ceiling building)
 * */
int get_obj_ground_y(Sprite &heli, Sprite *objarray, int objarray_length)
{
	int closest_ceil_build_x = get_ceil_dist_x(heli, objarray, objarray_length);
	if( closest_ceil_build_x == -1 ) closest_ceil_build_x = 1000000;
	
	int shortest_distance = 1000000; // the current shortest distance to a victim - updated in the for loop below
	                                 // arbitrarily large number to make sure the first victim checked will always be closer
	int shortest_array_pos = -1;
	
	/* find the closest ground building x wise to the 
	 * helicopter */
	for(int i=0; i<objarray_length; i++)
	{
		if( (objarray+i)->getX() > 0 && (objarray+i)->getX() < 1024 && // make sure the building is on screen
		    (objarray+i)->getY() > heli.getH() && // make sure the building is at least lower than the possible distance the helicopter could fit through between the top of the window and the building
		    (objarray+i)->getX() > heli.getX() + heli.getW()   // make sure the helicopter hasn't started to pass the building
		  )
		{
			if( (objarray+i)->getX() - (heli.getX() + heli.getW()) < shortest_distance ){ // if the distance between them is smaller than the current smallest, 
				shortest_distance = objarray[i].getX() - (heli.getX() + heli.getW());   // update the current smallest to the distance between them
				shortest_array_pos = i;
			}
		}
	}
	
	/* if there is a building closer x wise on the ceiling, return -1 */
	if( shortest_distance != -1 && shortest_distance != 1000000 && 
	    closest_ceil_build_x < shortest_distance ){
		return -1;
	}
	
	/* if the helicopter isn't in line to hit the closest building, return -1 */
	if( shortest_array_pos != -1 && heli.getY() + heli.getH() < (objarray+shortest_array_pos)->getY() ){
		shortest_distance = -1;
	}
	if( shortest_distance == 1000000 ){
		shortest_distance = -1;
	}
	
	if( shortest_array_pos != -1 && shortest_distance != -1){
		shortest_distance = (heli.getY() + heli.getH()) - (objarray+shortest_array_pos)->getY();
	}
	
	return shortest_distance;
}


