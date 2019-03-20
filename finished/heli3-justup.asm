# peripheral constants

.const P_THRUST 0x4000
.const P_Y 0x4001 # assuming from 0--999 - BJ EDITED TO 768 for window height
.const P_Y_VELOCITY 0x4002 # assuming negative means falling
.const P_LIVES 0x4003

.const P_VICTIM_DIST 0x4010 # offset to nearest
.const P_VICTIM_Y 0x4011 # y offset

.const P_GROUND_DIST 0x4020 # dist to ground obstacle in path, or <0
.const P_GROUND_HEIGHT 0x4021 # y offset from heli-bottom to ground top

.const P_CEIL_DIST 0x4030 # dist to ceil obstacle in path, or <0
.const P_CEIL_HEIGHT 0x4031 # y offset from heli-top to ceiling bottom

#.const CENTER_Y 500 - BJ EDITED THIS OUT to fix for 768 window height
#.const  CENTER_Y 384
.const  TARGET_Y 300

####################
# REGISTERS
#
# r100: y
# r101: y_target
# r102: thrust
# r103: y_vel
# r104: vic_dist
# r105: victim_y
# r106: ground_dist
# r107: ceil_height
####################

movi r102 = 100
stor [P_THRUST] = r102

!main

# try to fly to the middle of the screen
	#movi r101 = CENTER_Y
	#movi r101 = TARGET_Y

	#movi r102 = 100
	#stor [P_THRUST] = r102
	# remove me afterwords
	load r106 = [P_GROUND_DIST]

	# go back to main for now
	bra [!main]

!check_height
	load r100 = [P_Y]
	#bral [!below_center], r100 < r101
	# bj edited above out for testing
	brag [!stop_inf], r100 > r101
# above center of screen
	load r103 = [P_Y_VELOCITY]
	brae [!check_height], r103 == r0
	brag [!not_so_fast], r103 > r0
# above center but falling
	add r102 = r102 + 6
	bra [!fly]

!not_so_fast
	#sub r102 = r102 - 7
	movi r102 = 1
	bra [!fly]

!below_center
	movi r102 = 100

!fly
	stor [P_THRUST] = r102
	bra [!check_height]

# bj added for testing
!stop_inf
	movi r102 = 0
	stor [P_THRUST] = r102
	nop
	nop
	nop
	# then goes to cycle heli

!cycle_heli
#cycle down
	movi r102 = -100
	stor [P_THRUST] = r102	
	load r104 = [P_VICTIM_DIST]
	load r105 = [P_VICTIM_Y]
	nop
	nop
#cycle_up
	movi r102 = 66
	stor [P_THRUST] = r102
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	bra [!cycle_heli]
	
!inf
	bra   [!inf]
