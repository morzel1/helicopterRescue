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
####################

!main

	load r100 = [P_GROUND_DIST]
	
	bra [!main]

