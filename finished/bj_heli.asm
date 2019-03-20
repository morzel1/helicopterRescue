# Heli Rescue 3 assembly file
# modified to work with our system

# note - helicopter does NOT fall automatically
# assumes a 'hovering' state if thrust is 0

# peripheral constants

.const P_THRUST 0x4000       # assuming -100...100
.const P_Y 0x4001            # from 0-768
.const P_Y_VELOCITY 0x4002   # negative means falling
.const P_LIVES 0x4003

.const P_VICTIM_DIST 0x4010   # offset to nearest
.const P_VICTIM_Y 0x4011      # y offset

.const P_GROUND_DIST 0x4020     # dist to ground obstacle in path, or <0
.const P_GROUND_HEIGHT 0x4021   # y offset from heli-bottom to ground top

.const P_CEIL_DIST 0x4030     # dist to ceil obstacle in path, or <0
.const P_CEIL_HEIGHT 0x4031   # y offset from heli-top to ceiling bottom

####################
# REGISTERS
#
# r100: y
# r101: y_target
# r102: thrust
# r103: y_vel
####################

	movi r100 = 100
	movi r101 = -100
	movi r102 = 0

	movi r2 = 10
	movi r3 = -10

!main
	load r1 = [P_VICTIM_Y]
	bral [!check_slow], r1 < r2
	stor [P_THRUST] = r1
	bra  [!main]
	
!check_slow
	bral [!main], r1 < r3
	stor [P_THRUST] = r102
	bra  [!main]

!inf
	bra   [!inf]
