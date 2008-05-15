#ifndef __DIRECTFB_STRINGS_H__
#define __DIRECTFB_STRINGS_H__

#define DirectFBPixelFormatNames(Identifier) struct DFBPixelFormatName { \
     DFBSurfacePixelFormat format; \
     const char *name; \
} Identifier[] = { \
     { DSPF_ARGB1555, "ARGB1555" }, \
     { DSPF_RGB16, "RGB16" }, \
     { DSPF_RGB24, "RGB24" }, \
     { DSPF_RGB32, "RGB32" }, \
     { DSPF_ARGB, "ARGB" }, \
     { DSPF_A8, "A8" }, \
     { DSPF_YUY2, "YUY2" }, \
     { DSPF_RGB332, "RGB332" }, \
     { DSPF_UYVY, "UYVY" }, \
     { DSPF_I420, "I420" }, \
     { DSPF_YV12, "YV12" }, \
     { DSPF_LUT8, "LUT8" }, \
     { DSPF_ALUT44, "ALUT44" }, \
     { DSPF_AiRGB, "AiRGB" }, \
     { DSPF_A1, "A1" }, \
     { DSPF_NV12, "NV12" }, \
     { DSPF_NV16, "NV16" }, \
     { DSPF_ARGB2554, "ARGB2554" }, \
     { DSPF_ARGB4444, "ARGB4444" }, \
     { DSPF_NV21, "NV21" }, \
     { DSPF_UNKNOWN, "UNKNOWN" } \
};

#define DirectFBInputDeviceTypeFlagsNames(Identifier) struct DFBInputDeviceTypeFlagsName { \
     DFBInputDeviceTypeFlags type; \
     const char *name; \
} Identifier[] = { \
     { DIDTF_KEYBOARD, "KEYBOARD" }, \
     { DIDTF_MOUSE, "MOUSE" }, \
     { DIDTF_JOYSTICK, "JOYSTICK" }, \
     { DIDTF_REMOTE, "REMOTE" }, \
     { DIDTF_VIRTUAL, "VIRTUAL" }, \
     { DIDTF_NONE, "NONE" } \
};

#define DirectFBInputDeviceCapabilitiesNames(Identifier) struct DFBInputDeviceCapabilitiesName { \
     DFBInputDeviceCapabilities capability; \
     const char *name; \
} Identifier[] = { \
     { DICAPS_KEYS, "KEYS" }, \
     { DICAPS_AXES, "AXES" }, \
     { DICAPS_BUTTONS, "BUTTONS" }, \
     { DICAPS_NONE, "NONE" } \
};

#define DirectFBDisplayLayerTypeFlagsNames(Identifier) struct DFBDisplayLayerTypeFlagsName { \
     DFBDisplayLayerTypeFlags type; \
     const char *name; \
} Identifier[] = { \
     { DLTF_GRAPHICS, "GRAPHICS" }, \
     { DLTF_VIDEO, "VIDEO" }, \
     { DLTF_STILL_PICTURE, "STILL_PICTURE" }, \
     { DLTF_BACKGROUND, "BACKGROUND" }, \
     { DLTF_NONE, "NONE" } \
};

#define DirectFBDisplayLayerCapabilitiesNames(Identifier) struct DFBDisplayLayerCapabilitiesName { \
     DFBDisplayLayerCapabilities capability; \
     const char *name; \
} Identifier[] = { \
     { DLCAPS_SURFACE, "SURFACE" }, \
     { DLCAPS_OPACITY, "OPACITY" }, \
     { DLCAPS_ALPHACHANNEL, "ALPHACHANNEL" }, \
     { DLCAPS_SCREEN_LOCATION, "SCREEN_LOCATION" }, \
     { DLCAPS_FLICKER_FILTERING, "FLICKER_FILTERING" }, \
     { DLCAPS_DEINTERLACING, "DEINTERLACING" }, \
     { DLCAPS_SRC_COLORKEY, "SRC_COLORKEY" }, \
     { DLCAPS_DST_COLORKEY, "DST_COLORKEY" }, \
     { DLCAPS_BRIGHTNESS, "BRIGHTNESS" }, \
     { DLCAPS_CONTRAST, "CONTRAST" }, \
     { DLCAPS_HUE, "HUE" }, \
     { DLCAPS_SATURATION, "SATURATION" }, \
     { DLCAPS_LEVELS, "LEVELS" }, \
     { DLCAPS_FIELD_PARITY, "FIELD_PARITY" }, \
     { DLCAPS_WINDOWS, "WINDOWS" }, \
     { DLCAPS_SOURCES, "SOURCES" }, \
     { DLCAPS_ALPHA_RAMP, "ALPHA_RAMP" }, \
     { DLCAPS_PREMULTIPLIED, "PREMULTIPLIED" }, \
     { DLCAPS_SCREEN_POSITION, "SCREEN_POSITION" }, \
     { DLCAPS_SCREEN_SIZE, "SCREEN_SIZE" }, \
     { DLCAPS_NONE, "NONE" } \
};

#define DirectFBDisplayLayerBufferModeNames(Identifier) struct DFBDisplayLayerBufferModeName { \
     DFBDisplayLayerBufferMode mode; \
     const char *name; \
} Identifier[] = { \
     { DLBM_FRONTONLY, "FRONTONLY" }, \
     { DLBM_BACKVIDEO, "BACKVIDEO" }, \
     { DLBM_BACKSYSTEM, "BACKSYSTEM" }, \
     { DLBM_TRIPLE, "TRIPLE" }, \
     { DLBM_WINDOWS, "WINDOWS" }, \
     { DLBM_UNKNOWN, "UNKNOWN" } \
};

#define DirectFBScreenCapabilitiesNames(Identifier) struct DFBScreenCapabilitiesName { \
     DFBScreenCapabilities capability; \
     const char *name; \
} Identifier[] = { \
     { DSCCAPS_VSYNC, "VSYNC" }, \
     { DSCCAPS_POWER_MANAGEMENT, "POWER_MANAGEMENT" }, \
     { DSCCAPS_MIXERS, "MIXERS" }, \
     { DSCCAPS_ENCODERS, "ENCODERS" }, \
     { DSCCAPS_OUTPUTS, "OUTPUTS" }, \
     { DSCCAPS_NONE, "NONE" } \
};

#define DirectFBScreenEncoderCapabilitiesNames(Identifier) struct DFBScreenEncoderCapabilitiesName { \
     DFBScreenEncoderCapabilities capability; \
     const char *name; \
} Identifier[] = { \
     { DSECAPS_TV_STANDARDS, "TV_STANDARDS" }, \
     { DSECAPS_TEST_PICTURE, "TEST_PICTURE" }, \
     { DSECAPS_MIXER_SEL, "MIXER_SEL" }, \
     { DSECAPS_OUT_SIGNALS, "OUT_SIGNALS" }, \
     { DSECAPS_SCANMODE, "SCANMODE" }, \
     { DSECAPS_BRIGHTNESS, "BRIGHTNESS" }, \
     { DSECAPS_CONTRAST, "CONTRAST" }, \
     { DSECAPS_HUE, "HUE" }, \
     { DSECAPS_SATURATION, "SATURATION" }, \
     { DSECAPS_NONE, "NONE" } \
};

#define DirectFBScreenEncoderTypeNames(Identifier) struct DFBScreenEncoderTypeName { \
     DFBScreenEncoderType type; \
     const char *name; \
} Identifier[] = { \
     { DSET_CRTC, "CRTC" }, \
     { DSET_TV, "TV" }, \
     { DSET_UNKNOWN, "UNKNOWN" } \
};

#define DirectFBScreenEncoderTVStandardsNames(Identifier) struct DFBScreenEncoderTVStandardsName { \
     DFBScreenEncoderTVStandards standard; \
     const char *name; \
} Identifier[] = { \
     { DSETV_PAL, "PAL" }, \
     { DSETV_NTSC, "NTSC" }, \
     { DSETV_SECAM, "SECAM" }, \
     { DSETV_UNKNOWN, "UNKNOWN" } \
};

#define DirectFBScreenOutputCapabilitiesNames(Identifier) struct DFBScreenOutputCapabilitiesName { \
     DFBScreenOutputCapabilities capability; \
     const char *name; \
} Identifier[] = { \
     { DSOCAPS_CONNECTORS, "CONNECTORS" }, \
     { DSOCAPS_ENCODER_SEL, "ENCODER_SEL" }, \
     { DSOCAPS_SIGNAL_SEL, "SIGNAL_SEL" }, \
     { DSOCAPS_CONNECTOR_SEL, "CONNECTOR_SEL" }, \
     { DSOCAPS_NONE, "NONE" } \
};

#define DirectFBScreenOutputConnectorsNames(Identifier) struct DFBScreenOutputConnectorsName { \
     DFBScreenOutputConnectors connector; \
     const char *name; \
} Identifier[] = { \
     { DSOC_VGA, "VGA" }, \
     { DSOC_SCART, "SCART" }, \
     { DSOC_YC, "YC" }, \
     { DSOC_CVBS, "CVBS" }, \
     { DSOC_UNKNOWN, "UNKNOWN" } \
};

#define DirectFBScreenOutputSignalsNames(Identifier) struct DFBScreenOutputSignalsName { \
     DFBScreenOutputSignals signal; \
     const char *name; \
} Identifier[] = { \
     { DSOS_VGA, "VGA" }, \
     { DSOS_YC, "YC" }, \
     { DSOS_CVBS, "CVBS" }, \
     { DSOS_RGB, "RGB" }, \
     { DSOS_YCBCR, "YCBCR" }, \
     { DSOS_NONE, "NONE" } \
};

#define DirectFBScreenMixerCapabilitiesNames(Identifier) struct DFBScreenMixerCapabilitiesName { \
     DFBScreenMixerCapabilities capability; \
     const char *name; \
} Identifier[] = { \
     { DSMCAPS_FULL, "FULL" }, \
     { DSMCAPS_SUB_LEVEL, "SUB_LEVEL" }, \
     { DSMCAPS_SUB_LAYERS, "SUB_LAYERS" }, \
     { DSMCAPS_BACKGROUND, "BACKGROUND" }, \
     { DSMCAPS_NONE, "NONE" } \
};

#define DirectFBScreenMixerTreeNames(Identifier) struct DFBScreenMixerTreeName { \
     DFBScreenMixerTree tree; \
     const char *name; \
} Identifier[] = { \
     { DSMT_FULL, "FULL" }, \
     { DSMT_SUB_LEVEL, "SUB_LEVEL" }, \
     { DSMT_SUB_LAYERS, "SUB_LAYERS" }, \
     { DSMT_UNKNOWN, "UNKNOWN" } \
};

#define DirectFBScreenEncoderTestPictureNames(Identifier) struct DFBScreenEncoderTestPictureName { \
     DFBScreenEncoderTestPicture test_picture; \
     const char *name; \
} Identifier[] = { \
     { DSETP_MULTI, "MULTI" }, \
     { DSETP_SINGLE, "SINGLE" }, \
     { DSETP_WHITE, "WHITE" }, \
     { DSETP_YELLOW, "YELLOW" }, \
     { DSETP_CYAN, "CYAN" }, \
     { DSETP_GREEN, "GREEN" }, \
     { DSETP_MAGENTA, "MAGENTA" }, \
     { DSETP_RED, "RED" }, \
     { DSETP_BLUE, "BLUE" }, \
     { DSETP_BLACK, "BLACK" }, \
     { DSETP_OFF, "OFF" } \
};

#define DirectFBScreenEncoderScanModeNames(Identifier) struct DFBScreenEncoderScanModeName { \
     DFBScreenEncoderScanMode scan_mode; \
     const char *name; \
} Identifier[] = { \
     { DSESM_INTERLACED, "INTERLACED" }, \
     { DSESM_PROGRESSIVE, "PROGRESSIVE" }, \
     { DSESM_UNKNOWN, "UNKNOWN" } \
};

#endif
