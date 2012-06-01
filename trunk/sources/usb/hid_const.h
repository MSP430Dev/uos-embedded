#ifndef __HID_CONST_H__
#define __HID_CONST_H__

#define USB_HID_INTERFACE_CLASS     3
#define USB_HID_NONBOOT_SUBCLASS    0
#define USB_HID_BOOT_SUBCLASS       1
#define USB_HID_PROT_NONE           0
#define USB_HID_PROT_KEYBOARD       1
#define USB_HID_PROT_MOUSE          2

//
// HID report types
//
#define USB_DESC_TYPE_HID       0x21
#define USB_DESC_TYPE_REPORT    0x22
#define USB_DESC_TYPE_PHYSICAL  0x23

//
// HID specific requests
//
#define USB_HID_GET_REPORT      0x01
#define USB_HID_GET_IDLE        0x02
#define USB_HID_GET_PROTOCOL    0x03
#define USB_HID_SET_REPORT      0x09
#define USB_HID_SET_IDLE        0x0A
#define USB_HID_SET_PROTOCOL    0x0B

//
// HID report types
//
#define USB_HID_RPT_IN          0x01
#define USB_HID_RPT_OUT         0x02
#define USB_HID_RPT_FEATURE     0x03

//
// Generic macro for HID item header
//
#define HID_ITEM_HEADER(bTag,bType,bSize) (((bTag) << 4) | ((bType) << 2) | (bSize))

//
// Valid HID items types (for internal use; user should look at HID_INPUT and other defines)
//
#define HID_TYPE_MAIN           0
#define HID_TYPE_GLOBAL         1
#define HID_TYPE_LOCAL          2

//
// Valid HID items tags (for internal use; user should look at HID_INPUT and other defines)
//

// For MAIN items
#define HID_TAG_INPUT           0x8
#define HID_TAG_OUTPUT          0x9
#define HID_TAG_FEATURE         0xB
#define HID_TAG_COLLECTION      0xA
#define HID_TAG_END_COLLECTION  0xC

// For GLOBAL items
#define HID_TAG_USAGE_PAGE          0x0
#define HID_TAG_LOGICAL_MINIMUM     0x1
#define HID_TAG_LOGICAL_MAXIMUM     0x2
#define HID_TAG_PHYSICAL_MINIMUM    0x3
#define HID_TAG_PHYSICAL_MAXIMUM    0x4
#define HID_TAG_UNIT_EXPONENT       0x5
#define HID_TAG_UNIT                0x6
#define HID_TAG_REPORT_SIZE         0x7
#define HID_TAG_REPORT_ID           0x8
#define HID_TAG_REPORT_COUNT        0x9
#define HID_TAG_PUSH                0xA
#define HID_TAG_POP                 0xB

// For LOCAL items
#define HID_TAG_USAGE               0x0
#define HID_TAG_USAGE_MINIMUM       0x1
#define HID_TAG_USAGE_MAXIMUM       0x2
#define HID_TAG_DESIGNATOR_INDEX    0x3
#define HID_TAG_DESIGNATOR_MINIMUM  0x4
#define HID_TAG_DESIGNATOR_MAXIMUM  0x5
#define HID_TAG_STRING_INDEX        0x7
#define HID_TAG_STRING_MINIMUM      0x8
#define HID_TAG_STRING_MAXIMUM      0x9
#define HID_TAG_DELIMITER           0xA

//
// Item attributes
//

// INPUT, OUTPUT, FEATURE item data
#define HID_DATA                0x00
#define HID_CONSTANT            0x01
#define HID_ARRAY               0x00
#define HID_VARIABLE            0x02
#define HID_ABSOLUTE            0x00
#define HID_RELATIVE            0x04
#define HID_NO_WRAP             0x00
#define HID_WRAP                0x08
#define HID_LINEAR              0x00
#define HID_NON_LINEAR          0x10
#define HID_PREFERRED_STATE     0x00
#define HID_NO_PREFERRED        0x20
#define HID_NO_NULL             0x00
#define HID_NULL_STATE          0x40
#define HID_NON_VOLATILE        0x00
#define HID_VOLATILE            0x80

// COLLECTION item data
#define HID_PHYSICAL            0x00
#define HID_APPLICATION         0x01
#define HID_LOGICAL             0x02
#define HID_REPORT              0x03
#define HID_NAMED_ARRAY         0x04
#define HID_USAGE_SWITCH        0x05
#define HID_USAGE_MODIFIER      0x06

//
// Defines for use in applications
//

// MAIN items
#define HID_INPUT(attr)      HID_ITEM_HEADER(HID_TAG_INPUT,HID_TYPE_MAIN,1), (attr)
#define HID_INPUT_BB(attr)   HID_ITEM_HEADER(HID_TAG_INPUT,HID_TYPE_MAIN,2), (attr), 0x1
#define HID_OUTPUT(attr)     HID_ITEM_HEADER(HID_TAG_OUTPUT,HID_TYPE_MAIN,1), (attr)
#define HID_OUTPUT_BB(attr)  HID_ITEM_HEADER(HID_TAG_OUTPUT,HID_TYPE_MAIN,2), (attr), 0x1
#define HID_FEATURE(attr)    HID_ITEM_HEADER(HID_TAG_FEATURE,HID_TYPE_MAIN,1), (attr)
#define HID_FEATURE_BB(attr) HID_ITEM_HEADER(HID_TAG_FEATURE,HID_TYPE_MAIN,2), (attr), 0x1
#define HID_COLLECTION(type) HID_ITEM_HEADER(HID_TAG_COLLECTION,HID_TYPE_MAIN,1), (type)
#define HID_END_COLLECTION   HID_ITEM_HEADER(HID_TAG_END_COLLECTION,HID_TYPE_MAIN,0)

// GLOBAL items
#define HID_USAGE_PAGE_1B(page)        HID_ITEM_HEADER(HID_TAG_USAGE_PAGE,HID_TYPE_GLOBAL,1),\
                                       ((page) & 0xFF)
#define HID_USAGE_PAGE_2B(page)        HID_ITEM_HEADER(HID_TAG_USAGE_PAGE,HID_TYPE_GLOBAL,2),\
                                       ((page) & 0xFF), (((page) >> 8) & 0xFF)
#define HID_LOGICAL_MINIMUM_1B(value)  HID_ITEM_HEADER(HID_TAG_LOGICAL_MINIMUM,HID_TYPE_GLOBAL,1), \
                                       ((value) & 0xFF)
#define HID_LOGICAL_MINIMUM_2B(value)  HID_ITEM_HEADER(HID_TAG_LOGICAL_MINIMUM,HID_TYPE_GLOBAL,2), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_LOGICAL_MINIMUM_4B(value)  HID_ITEM_HEADER(HID_TAG_LOGICAL_MINIMUM,HID_TYPE_GLOBAL,3), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                       (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_LOGICAL_MAXIMUM_1B(value)  HID_ITEM_HEADER(HID_TAG_LOGICAL_MAXIMUM,HID_TYPE_GLOBAL,1), \
                                       ((value) & 0xFF)
#define HID_LOGICAL_MAXIMUM_2B(value)  HID_ITEM_HEADER(HID_TAG_LOGICAL_MAXIMUM,HID_TYPE_GLOBAL,2), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_LOGICAL_MAXIMUM_4B(value)  HID_ITEM_HEADER(HID_TAG_LOGICAL_MAXIMUM,HID_TYPE_GLOBAL,3), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                       (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_PHYSICAL_MINIMUM_1B(value) HID_ITEM_HEADER(HID_TAG_PHYSICAL_MINIMUM,HID_TYPE_GLOBAL,1), \
                                       ((value) & 0xFF)
#define HID_PHYSICAL_MINIMUM_2B(value) HID_ITEM_HEADER(HID_TAG_PHYSICAL_MINIMUM,HID_TYPE_GLOBAL,2), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_PHYSICAL_MINIMUM_4B(value) HID_ITEM_HEADER(HID_TAG_PHYSICAL_MINIMUM,HID_TYPE_GLOBAL,3), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                       (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_PHYSICAL_MAXIMUM_1B(value) HID_ITEM_HEADER(HID_TAG_PHYSICAL_MAXIMUM,HID_TYPE_GLOBAL,1), \
                                       ((value) & 0xFF)
#define HID_PHYSICAL_MAXIMUM_2B(value) HID_ITEM_HEADER(HID_TAG_PHYSICAL_MAXIMUM,HID_TYPE_GLOBAL,2), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_PHYSICAL_MAXIMUM_4B(value) HID_ITEM_HEADER(HID_TAG_PHYSICAL_MAXIMUM,HID_TYPE_GLOBAL,3), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                       (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_UNIT_EXPONENT(value)       HID_ITEM_HEADER(HID_TAG_UNIT_EXPONENT,HID_TYPE_GLOBAL,1), (value)
#define HID_UNIT(value)                HID_ITEM_HEADER(HID_TAG_UNIT,HID_TYPE_GLOBAL,3), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                       (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_REPORT_SIZE(value)         HID_ITEM_HEADER(HID_TAG_REPORT_SIZE,HID_TYPE_GLOBAL,1), (value)
#define HID_REPORT_ID(value)           HID_ITEM_HEADER(HID_TAG_REPORT_ID,HID_TYPE_GLOBAL,1), (value)
#define HID_REPORT_COUNT(value)        HID_ITEM_HEADER(HID_TAG_REPORT_COUNT,HID_TYPE_GLOBAL,1), (value)
#define HID_PUSH                       HID_ITEM_HEADER(HID_TAG_PUSH,HID_TYPE_GLOBAL,0)
#define HID_POP                        HID_ITEM_HEADER(HID_TAG_POP,HID_TYPE_GLOBAL,0)

// LOCAL items
#define HID_USAGE_1B(value)            HID_ITEM_HEADER(HID_TAG_USAGE,HID_TYPE_LOCAL,1),\
                                       ((value) & 0xFF)
#define HID_USAGE_2B(value)            HID_ITEM_HEADER(HID_TAG_USAGE,HID_TYPE_LOCAL,2),\
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_USAGE_MINIMUM(value)       HID_ITEM_HEADER(HID_TAG_USAGE_MINIMUM,HID_TYPE_LOCAL,1), \
                                       ((value) & 0xFF)
#define HID_USAGE_MAXIMUM(value)       HID_ITEM_HEADER(HID_TAG_USAGE_MAXIMUM,HID_TYPE_LOCAL,1), \
                                       ((value) & 0xFF)
#define HID_DESIGNATOR_INDEX_1B(value) HID_ITEM_HEADER(HID_TAG_DESIGNATOR_INDEX,HID_TYPE_LOCAL,1), \
                                       ((value) & 0xFF)
#define HID_DESIGNATOR_INDEX_2B(value) HID_ITEM_HEADER(HID_TAG_DESIGNATOR_INDEX,HID_TYPE_LOCAL,2), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_DESIGNATOR_INDEX_4B(value) HID_ITEM_HEADER(HID_TAG_DESIGNATOR_INDEX,HID_TYPE_LOCAL,3), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                       (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_DESIGNATOR_MINIMUM_1B(value) HID_ITEM_HEADER(HID_TAG_DESIGNATOR_MINIMUM,HID_TYPE_LOCAL,1), \
                                         ((value) & 0xFF)
#define HID_DESIGNATOR_MINIMUM_2B(value) HID_ITEM_HEADER(HID_TAG_DESIGNATOR_MINIMUM,HID_TYPE_LOCAL,2), \
                                         ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_DESIGNATOR_MINIMUM_4B(value) HID_ITEM_HEADER(HID_TAG_DESIGNATOR_MINIMUM,HID_TYPE_LOCAL,3), \
                                         ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                         (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_DESIGNATOR_MAXIMUM_1B(value) HID_ITEM_HEADER(HID_TAG_DESIGNATOR_MAXIMUM,HID_TYPE_LOCAL,1), \
                                         ((value) & 0xFF)
#define HID_DESIGNATOR_MAXIMUM_2B(value) HID_ITEM_HEADER(HID_TAG_DESIGNATOR_MAXIMUM,HID_TYPE_LOCAL,2), \
                                         ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_DESIGNATOR_MAXIMUM_4B(value) HID_ITEM_HEADER(HID_TAG_DESIGNATOR_MAXIMUM,HID_TYPE_LOCAL,3), \
                                         ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                         (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_STRING_INDEX_1B(value)     HID_ITEM_HEADER(HID_TAG_STRING_INDEX,HID_TYPE_LOCAL,1), \
                                       ((value) & 0xFF)
#define HID_STRING_INDEX_2B(value)     HID_ITEM_HEADER(HID_TAG_STRING_INDEX,HID_TYPE_LOCAL,2), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_STRING_INDEX_4B(value)     HID_ITEM_HEADER(HID_TAG_STRING_INDEX,HID_TYPE_LOCAL,3), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                       (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_STRING_MINIMUM_1B(value)   HID_ITEM_HEADER(HID_TAG_STRING_MINIMUM,HID_TYPE_LOCAL,1), \
                                       ((value) & 0xFF)
#define HID_STRING_MINIMUM_2B(value)   HID_ITEM_HEADER(HID_TAG_STRING_MINIMUM,HID_TYPE_LOCAL,2), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_STRING_MINIMUM_4B(value)   HID_ITEM_HEADER(HID_TAG_STRING_MINIMUM,HID_TYPE_LOCAL,3), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                       (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_STRING_MAXIMUM_1B(value)   HID_ITEM_HEADER(HID_TAG_STRING_MAXIMUM,HID_TYPE_LOCAL,1), \
                                       ((value) & 0xFF)
#define HID_STRING_MAXIMUM_2B(value)   HID_ITEM_HEADER(HID_TAG_STRING_MAXIMUM,HID_TYPE_LOCAL,2), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF)
#define HID_STRING_MAXIMUM_4B(value)   HID_ITEM_HEADER(HID_TAG_STRING_MAXIMUM,HID_TYPE_LOCAL,3), \
                                       ((value) & 0xFF), (((value) >> 8) & 0xFF), \
                                       (((value) >> 16) & 0xFF), (((value) >> 24) & 0xFF)
#define HID_DELIMITER(value)           HID_ITEM_HEADER(HID_TAG_DELIMITER,HID_TYPE_LOCAL,1), \
                                       ((value) & 1)
                                       
                                       
//
// HID Usage Pages
//
#define HUP_GENERIC_DESKTOP             0x01
#define HUP_SIMULATION                  0x02
#define HUP_VR                          0x03
#define HUP_SPORT                       0x04
#define HUP_GAME                        0x05
#define HUP_GENERIC_DEVICE              0x06
#define HUP_KEYBOARD                    0x07
#define HUP_LEDS                        0x08
#define HUP_BUTTONS                     0x09
#define HUP_ORDINAL                     0x0A
#define HUP_TELEPHONY                   0x0B
#define HUP_CONSUMER                    0x0C
#define HUP_DIGITIGER                   0x0D
#define HUP_PID                         0x0F
#define HUP_UNICODE                     0x10
#define HUP_ALPHANUMERIC_DISPLAY        0x14
#define HUP_MEDICAL_INSTRUMENTS         0x40
#define HUP_MONITOR1                    0x80
#define HUP_MONITOR2                    0x81
#define HUP_MONITOR3                    0x82
#define HUP_MONITOR4                    0x83
#define HUP_POWER1                      0x84
#define HUP_POWER2                      0x85
#define HUP_POWER3                      0x86
#define HUP_POWER4                      0x87
#define HUP_BAR_CODE_SCANNER            0x8C
#define HUP_SCALE                       0x8D
#define HUP_MSR                         0x8E
#define HUP_CAMERA                      0x90
#define HUP_ARCADE                      0x91

//
// HID Usages
//

// Generic Desktop Page
#define HU_POINTER                      0x01
#define HU_MOUSE                        0x02
#define HU_JOYSTICK                     0x04
#define HU_GAMEPAD                      0x05
#define HU_KEYBOARD                     0x06
#define HU_KEYPAD                       0x07
#define HU_MULTIAXES                    0x08
#define HU_X                            0x30
#define HU_Y                            0x31
#define HU_Z                            0x32
#define HU_RX                           0x33
#define HU_RY                           0x34
#define HU_RZ                           0x35
#define HU_SLIDER                       0x36
#define HU_DIAL                         0x37
#define HU_WHEEL                        0x38
#define HU_HAT_SWITCH                   0x39
#define HU_COUNTED_BUFFER               0x3A
#define HU_BYTE_COUNT                   0x3B
#define HU_MOTION_WAKEUP                0x3C
#define HU_START                        0x3D
#define HU_SELECT                       0x3E
#define HU_VX                           0x40
#define HU_VY                           0x41
#define HU_VZ                           0x42
#define HU_VBRX                         0x43
#define HU_VBRY                         0x44
#define HU_VBRZ                         0x45
#define HU_VNO                          0x46
#define HU_FEATURE_NOTIFICATION         0x47
#define HU_SYSTEM_CONTROL               0x80
#define HU_SYSTEM_POWER_DOWN            0x81
#define HU_SYSTEM_SLEEP                 0x82
#define HU_SYSTEM_WAKEUP                0x83
#define HU_SYSTEM_CONTEXT_MENU          0x84
#define HU_SYSTEM_MAIN_MENU             0x85
#define HU_SYSTEM_APP_MENU              0x86
#define HU_SYSTEM_MENU_HELP             0x87
#define HU_SYSTEM_MENU_EXIT             0x88
#define HU_SYSTEM_MENU_SELECT           0x89
#define HU_SYSTEM_MENU_RIGHT            0x8A
#define HU_SYSTEM_MENU_LEFT             0x8B
#define HU_SYSTEM_MENU_UP               0x8C
#define HU_SYSTEM_MENU_DOWN             0x8D
#define HU_SYSTEM_COLD_RESTART          0x8E
#define HU_SYSTEM_WARM_RESTART          0x8F
#define HU_D_PAD_UP                     0x90
#define HU_D_PAD_DOWN                   0x91
#define HU_D_PAD_RIGHT                  0x92
#define HU_D_PAD_LEFT                   0x93
#define HU_SYSTEM_DOCK                  0xA0
#define HU_SYSTEM_UNDOCK                0xA1
#define HU_SYSTEM_SETUP                 0xA2
#define HU_SYSTEM_BREAK                 0xA3
#define HU_SYSTEM_DEBUGGER_BREAK        0xA4
#define HU_APPLICATION_BREAK            0xA5
#define HU_APPLICATION_DEBUGGER_BREAK   0xA6
#define HU_SYSTEM_SPEAKER_MUTE          0xA7
#define HU_SYSTEM_HIBERNATE             0xA8
#define HU_SYSTEM_DISPLAY_INVERT        0xB0
#define HU_SYSTEM_DISPLAY_INTERNAL      0xB1
#define HU_SYSTEM_DISPLAY_EXTERNAL      0xB2
#define HU_SYSTEM_DISPLAY_BOTH          0xB3
#define HU_SYSTEM_DISPLAY_DUAL          0xB4
#define HU_SYSTEM_DISPLAY_TOGGLE        0xB5
#define HU_SYSTEM_DISPLAY_SWAP          0xB6
#define HU_SYSTEM_DISPLAY_LCD_AUTOSCALE 0xB7

// Simulation Controls Page
#define HU_FLIGHT_SIMULATION            0x01
#define HU_AUTOMOBILE_SIMULATION        0x02
#define HU_TANK_SIMULATION              0x03
#define HU_SPACESHIP_SIMULATION         0x04
#define HU_SUBMARINE_SIMULATION         0x05
#define HU_SAILING_SIMULATION           0x06
#define HU_MOTORCYCLE_SIMULATION        0x07
#define HU_SPORTS_SIMULATION            0x08
#define HU_AIRPLANE_SIMULATION          0x09
#define HU_HELICOPTER_SIMULATION        0x0A
#define HU_MAGIC_CARPET_SIMULATION      0x0B
#define HU_BICYCLE_SIMULATION           0x0C
#define HU_FLIGHT_CONTROL_STICK         0x20
#define HU_FLIGHT_STICK                 0x21
#define HU_CYCLIC_CONTROL               0x22
#define HU_CYCLIC_TRIM                  0x23
#define HU_FLIGHT_YOKE                  0x24
#define HU_TRACK_CONTROL                0x25
#define HU_AILERON                      0xB0
#define HU_AILERON_TRIM                 0xB1
#define HU_ANTI_TORQUE_CONTROL          0xB2
#define HU_AUTOPILOT_ENABLE             0xB3
#define HU_CHAFF_ENABLE                 0xB4
#define HU_COLLECTIVE_CONTROL           0xB5
#define HU_DIVE_BRAKE                   0xB6
#define HU_ELECTRONIC_COUNTERMEASURES   0xB7
#define HU_ELEVATOR                     0xB8
#define HU_ELEVATOR_TRIM                0xB9
#define HU_RUDDER                       0xBA
#define HU_THROTTLE                     0xBB
#define HU_FLIGHT_COMMUNICATIONS        0xBC
#define HU_FLARE_RELEASE                0xBD
#define HU_LANDING_GEAR                 0xBE
#define HU_TOE_BRAKE                    0xBF
#define HU_TRIGGER                      0xC0
#define HU_WEAPONS_ARM                  0xC1
#define HU_WEAPONS_SELECT               0xC2
#define HU_WING_FLAPS                   0xC3
#define HU_ACCELERATOR                  0xC4
#define HU_BRAKE                        0xC5
#define HU_CLUTCH                       0xC6
#define HU_SHIFTER                      0xC7
#define HU_STEERING                     0xC8
#define HU_TURRET_DIRECTION             0xC9
#define HU_BARREL_ELEVATION             0xCA
#define HU_DIVE_PLANE                   0xCB
#define HU_BALLAST                      0xCC
#define HU_BICYCLE_CRANK                0xCD
#define HU_HANDLE_BARS                  0xCE
#define HU_FRONT_BRAKE                  0xCF
#define HU_REAR_BRAKE                   0xD0

// VR Controls Page
#define HU_BELT                         0x01
#define HU_BODY_SUIT                    0x02
#define HU_FLEXOR                       0x03
#define HU_GLOVE                        0x04
#define HU_HEAD_TRACKER                 0x05
#define HU_HEAD_MOUNTED_DISPLAY         0x06
#define HU_HAND_TRACKER                 0x07
#define HU_OCULOMETER                   0x08
#define HU_VEST                         0x09
#define HU_ANIMATRONIC_DEVICE           0x0A
#define HU_STEREO_ENABLE                0x20
#define HU_DISPLAY_ENABLE               0x21

// Sport Controls Page
#define HU_BASEBALL_BAT                 0x01
#define HU_GOLF_CLUB                    0x02
#define HU_ROWING_MACHINE               0x03
#define HU_TREADMILL                    0x04
#define HU_OAR                          0x30
#define HU_SLOPE                        0x31
#define HU_RATE                         0x32
#define HU_STICK_SPEED                  0x33
#define HU_STICK_FACE_ANGLE             0x34
#define HU_STICK_HEEL_TOE               0x35
#define HU_STICK_FOLLOW_THROUGH         0x36
#define HU_STICK_TEMPO                  0x37
#define HU_STICK_TYPE                   0x38
#define HU_STICK_HEIGHT                 0x39
#define HU_PUTTER                       0x50
#define HU_1_IRON                       0x51
#define HU_2_IRON                       0x52
#define HU_3_IRON                       0x53
#define HU_4_IRON                       0x54
#define HU_5_IRON                       0x55
#define HU_6_IRON                       0x56
#define HU_7_IRON                       0x57
#define HU_8_IRON                       0x58
#define HU_9_IRON                       0x59
#define HU_10_IRON                      0x5A
#define HU_11_IRON                      0x5B
#define HU_SAND_WEDGE                   0x5C
#define HU_LOFT_WEDGE                   0x5D
#define HU_POWER_WEDGE                  0x5E
#define HU_1_WOOD                       0x5F
#define HU_3_WOOD                       0x60
#define HU_5_WOOD                       0x61
#define HU_7_WOOD                       0x62
#define HU_9_WOOD                       0x63

// Game Controls Page
#define HU_3D_GAME_CONTROLLER           0x01
#define HU_PINBALL_DEVICE               0x02
#define HU_GUN_DEVICE                   0x03
#define HU_POINT_OF_VIEW                0x20
#define HU_TURN_RIGHT_LEFT              0x21
#define HU_PITCH_FORWARD_BACKWARD       0x22
#define HU_ROLL_RIGHT_LEFT              0x23
#define HU_MOVE_RIGHT_LEFT              0x24
#define HU_MOVE_FORWARD_BACKWARD        0x25
#define HU_MOVE_UP_DOWN                 0x26
#define HU_LEAN_RIGHT_LEFT              0x27
#define HU_LEAN_FORWARD_BACKWARD        0x28
#define HU_HEIGHT_OF_POV                0x29
#define HU_FLIPPER                      0x2A
#define HU_SECONDARY_FLIPPER            0x2B
#define HU_BUMP                         0x2C
#define HU_NEW_GAME                     0x2D
#define HU_SHOOT_BALL                   0x2E
#define HU_PLAYER                       0x2F
#define HU_GUN_BOLT                     0x30
#define HU_GUN_CLIP                     0x31
#define HU_GUN_SELECTOR                 0x32
#define HU_GUN_SINGLE_SHOT              0x33
#define HU_GUN_BURST                    0x34
#define HU_GUN_AUTOMATIC                0x35
#define HU_GUN_SAFETY                   0x36
#define HU_GAMEPAD_FIRE_JUMP            0x37
#define HU_GAMEPAD_TRIGGER              0x39

// Generic Device Controls Page
#define HU_BATTERY_STRENGHT             0x20
#define HU_WIRELESS_CHANNEL             0x21
#define HU_WIRELESS_ID                  0x22

// Keyboard/Keypad Page
#define HU_KBD_ERROR_ROLL_OVER          0x01
#define HU_KBD_POST_FAIL                0x02
#define HU_KBD_ERROR_UNDEFINED          0x03
#define HU_KBD_A                        0x04
#define HU_KBD_B                        0x05
#define HU_KBD_C                        0x06
#define HU_KBD_D                        0x07
#define HU_KBD_E                        0x08
#define HU_KBD_F                        0x09
#define HU_KBD_G                        0x0A
#define HU_KBD_H                        0x0B
#define HU_KBD_I                        0x0C
#define HU_KBD_J                        0x0D
#define HU_KBD_K                        0x0E
#define HU_KBD_L                        0x0F
#define HU_KBD_M                        0x10
#define HU_KBD_N                        0x11
#define HU_KBD_O                        0x12
#define HU_KBD_P                        0x13
#define HU_KBD_Q                        0x14
#define HU_KBD_R                        0x15
#define HU_KBD_S                        0x16
#define HU_KBD_T                        0x17
#define HU_KBD_U                        0x18
#define HU_KBD_V                        0x19
#define HU_KBD_W                        0x1A
#define HU_KBD_X                        0x1B
#define HU_KBD_Y                        0x1C
#define HU_KBD_Z                        0x1D
#define HU_KBD_1                        0x1E
#define HU_KBD_2                        0x1F
#define HU_KBD_3                        0x20
#define HU_KBD_4                        0x21
#define HU_KBD_5                        0x22
#define HU_KBD_6                        0x23
#define HU_KBD_7                        0x24
#define HU_KBD_8                        0x25
#define HU_KBD_9                        0x26
#define HU_KBD_0                        0x27
#define HU_KBD_ENTER                    0x28
#define HU_KBD_ESCAPE                   0x29
#define HU_KBD_DELETE                   0x2A
#define HU_KBD_TAB                      0x2B
#define HU_KBD_SPACEBAR                 0x2C
#define HU_KBD_MINUS                    0x2D
#define HU_KBD_EQUAL                    0x2E
#define HU_KBD_SQ_BRACKET_OPEN          0x2F
#define HU_KBD_SQ_BRACKET_CLOSE         0x30
#define HU_KBD_BACKSLASH                0x31
#define HU_KBD_NON_US_NUMBER            0x32


#endif // __HID_CONST_H__
