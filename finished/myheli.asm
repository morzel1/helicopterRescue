# peripheral constants

.const P_THRUST 0x4000
.const P_HELIY  0x4001
.const P_YVEL   0x4002
.const P_LIVES  0x4003

.const P_VICDIS 0x4010
.const P_VICY   0x4011

.const P_GROUNDIS 0x4020
.const P_GROUNDH  0x4021

#bj comented these out as the haven't been added yet
# port constants

#.const PORT_A 0x1B1A

#.const PA_BIT_MOVING 0  # input: 1 when robot moving, 0 otherwise
#.const PA_BIT_TURNING 1  # input: 1 when turn angle is changing, 0 otherwise
#.const PA_BIT_TAXIING 2  # input: 1 when moving and target speed unmet, 0 otherwise

# test setting the thrust
MOVI r1 = 100
STOR [P_THRUST] = r1 # try setting a value of 5

LOAD r1 = [P_THRUST] # test reading the helicopter's thrust value
LOAD r1 = [P_HELIY]  # test reading the helicopter's y position
LOAD r1 = [P_YVEL]   # test reading the helicopter's y velocity
LOAD r1 = [P_LIVES]  # test reading the number of lives left

!main
	

!inf
	bra   [!inf]
