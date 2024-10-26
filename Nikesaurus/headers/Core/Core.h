/*****************************************************************//**
 * \file   Core.h
 * \brief  Nikesaurus core file
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

 /*****************************************************************//**
 * DLL Export & Import Defines
 *********************************************************************/
#ifdef NIKESAURUS_PLATFORM_WINDOWS
	#ifdef NIKE_BUILD_DLL
		#define NIKESAURUS_API __declspec(dllexport)
	#else
		#define NIKESAURUS_API __declspec(dllimport)
	#endif
#else
#endif

/*****************************************************************//**
* INPUT STATES
*********************************************************************/
#define NIKE_TRUE                   1
#define NIKE_FALSE                  0
#define NIKE_RELEASE                0
#define NIKE_PRESS                  1
#define NIKE_REPEAT                 2

/*****************************************************************//**
* WINDOW MODES
*********************************************************************/
/*
* !!!QUERY ONLY
*/
#define NIKE_FOCUSED                0x00020001 
//This queries for focus status.
#define NIKE_ICONIFIED              0x00020002 
//This queries whether the window is minimized (iconified) or not.
#define NIKE_MAXIMIZED              0x00020008
//This queries whether the window is maximized.
#define NIKE_VISIBLE                0x00020004 
//This queries whether the window is currently visible
#define NIKE_HOVERED                0x0002000B
//This queries whether the mouse cursor is hovering over the window.

/*
* !!!QUERY & SET
*/
#define NIKE_RESIZABLE              0x00020003 
//Controls whether the window is resizable by the user. It can be queried as an attribute after the window is created.
#define NIKE_DECORATED              0x00020005
//Controls whether the window should have decorations(such as a border, close button, etc.).Can be set before creating the window or queried afterward.
#define NIKE_AUTO_ICONIFY           0x00020006
//Controls whether the window will be automatically minimized (iconified) when it loses focus while in fullscreen mode.
#define NIKE_FLOATING               0x00020007
//This hint sets the window to be "always on top," meaning it will float above other windows.

/*****************************************************************//**
* INPUT MODES
*********************************************************************/
#define NIKE_CURSOR                 0x00033001 //MODE
#define NIKE_CURSOR_NORMAL          0x00034001 //VALUE
#define NIKE_CURSOR_HIDDEN          0x00034002 //VALUE
#define NIKE_CURSOR_DISABLED        0x00034003 //VALUE
#define NIKE_CURSOR_CAPTURED        0x00034004 //VALUE

/*
* If the mode is `NIKE_STICKY_KEYS`, the value must be either `NIKE_TRUE` to
* enable sticky keys, or `NIKE_FALSE` to disable it.If sticky keys are
* enabled, a key press will ensure that @ref glfwGetKey returns `NIKE_PRESS`
* the next time it is called even if the key had been released before the
* call.This is useful when you are only interested in whether keys have been
* pressed but not when or in which order.
*/
#define NIKE_STICKY_KEYS            0x00033002

/*
* If the mode is `NIKE_STICKY_MOUSE_BUTTONS`, the value must be either
* `NIKE_TRUE` to enable sticky mouse buttons, or `NIKE_FALSE` to disable it.
* If sticky mouse buttons are enabled, a mouse button press will ensure that
* @ref glfwGetMouseButton returns `NIKE_PRESS` the next time it is called even
* if the mouse button had been released before the call.This is useful when
* you are only interested in whether mouse buttons have been pressed but not
*when or in which order.
*/
#define NIKE_STICKY_MOUSE_BUTTONS   0x00033003

/*
* If the mode is `NIKE_LOCK_KEY_MODS`, the value must be either `NIKE_TRUE` to
* enable lock key modifier bits, or `NIKE_FALSE` to disable them.If enabled,
* callbacks that receive modifier bits will also have the @ref
* NIKE_MOD_CAPS_LOCK bit set when the event was generated with Caps Lock on,
* and the @ref NIKE_MOD_NUM_LOCK bit when Num Lock was on.
*/
#define NIKE_LOCK_KEY_MODS          0x00033004

/*
* If the mode is `NIKE_RAW_MOUSE_MOTION`, the value must be either `NIKE_TRUE`
* to enable raw(unscaled and unaccelerated) mouse motion when the cursor is
* disabled, or `NIKE_FALSE` to disable it.If raw motion is not supported,
* attempting to set this will emit @ref NIKE_FEATURE_UNAVAILABLE.Call @ref
* glfwRawMouseMotionSupported to check for support.
*/
#define NIKE_RAW_MOUSE_MOTION       0x00033005

/*****************************************************************//**
* KEYBOARD KEY CODES
*********************************************************************/
#define NIKE_KEY_SPACE              32
#define NIKE_KEY_APOSTROPHE         39  /* ' */
#define NIKE_KEY_COMMA              44  /* , */
#define NIKE_KEY_MINUS              45  /* - */
#define NIKE_KEY_PERIOD             46  /* . */
#define NIKE_KEY_SLASH              47  /* / */
#define NIKE_KEY_0                  48
#define NIKE_KEY_1                  49
#define NIKE_KEY_2                  50
#define NIKE_KEY_3                  51
#define NIKE_KEY_4                  52
#define NIKE_KEY_5                  53
#define NIKE_KEY_6                  54
#define NIKE_KEY_7                  55
#define NIKE_KEY_8                  56
#define NIKE_KEY_9                  57
#define NIKE_KEY_SEMICOLON          59  /* ; */
#define NIKE_KEY_EQUAL              61  /* = */
#define NIKE_KEY_A                  65
#define NIKE_KEY_B                  66
#define NIKE_KEY_C                  67
#define NIKE_KEY_D                  68
#define NIKE_KEY_E                  69
#define NIKE_KEY_F                  70
#define NIKE_KEY_G                  71
#define NIKE_KEY_H                  72
#define NIKE_KEY_I                  73
#define NIKE_KEY_J                  74
#define NIKE_KEY_K                  75
#define NIKE_KEY_L                  76
#define NIKE_KEY_M                  77
#define NIKE_KEY_N                  78
#define NIKE_KEY_O                  79
#define NIKE_KEY_P                  80
#define NIKE_KEY_Q                  81
#define NIKE_KEY_R                  82
#define NIKE_KEY_S                  83
#define NIKE_KEY_T                  84
#define NIKE_KEY_U                  85
#define NIKE_KEY_V                  86
#define NIKE_KEY_W                  87
#define NIKE_KEY_X                  88
#define NIKE_KEY_Y                  89
#define NIKE_KEY_Z                  90
#define NIKE_KEY_LEFT_BRACKET       91  /* [ */
#define NIKE_KEY_BACKSLASH          92  /* \ */
#define NIKE_KEY_RIGHT_BRACKET      93  /* ] */
#define NIKE_KEY_GRAVE_ACCENT       96  /* ` */
#define NIKE_KEY_WORLD_1            161 /* non-US #1 */
#define NIKE_KEY_WORLD_2            162 /* non-US #2 */
#define NIKE_KEY_ESCAPE             256
#define NIKE_KEY_ENTER              257
#define NIKE_KEY_TAB                258
#define NIKE_KEY_BACKSPACE          259
#define NIKE_KEY_INSERT             260
#define NIKE_KEY_DELETE             261
#define NIKE_KEY_RIGHT              262
#define NIKE_KEY_LEFT               263
#define NIKE_KEY_DOWN               264
#define NIKE_KEY_UP                 265
#define NIKE_KEY_PAGE_UP            266
#define NIKE_KEY_PAGE_DOWN          267
#define NIKE_KEY_HOME               268
#define NIKE_KEY_END                269
#define NIKE_KEY_CAPS_LOCK          280
#define NIKE_KEY_SCROLL_LOCK        281
#define NIKE_KEY_NUM_LOCK           282
#define NIKE_KEY_PRINT_SCREEN       283
#define NIKE_KEY_PAUSE              284
#define NIKE_KEY_F1                 290
#define NIKE_KEY_F2                 291
#define NIKE_KEY_F3                 292
#define NIKE_KEY_F4                 293
#define NIKE_KEY_F5                 294
#define NIKE_KEY_F6                 295
#define NIKE_KEY_F7                 296
#define NIKE_KEY_F8                 297
#define NIKE_KEY_F9                 298
#define NIKE_KEY_F10                299
#define NIKE_KEY_F11                300
#define NIKE_KEY_F12                301
#define NIKE_KEY_F13                302
#define NIKE_KEY_F14                303
#define NIKE_KEY_F15                304
#define NIKE_KEY_F16                305
#define NIKE_KEY_F17                306
#define NIKE_KEY_F18                307
#define NIKE_KEY_F19                308
#define NIKE_KEY_F20                309
#define NIKE_KEY_F21                310
#define NIKE_KEY_F22                311
#define NIKE_KEY_F23                312
#define NIKE_KEY_F24                313
#define NIKE_KEY_F25                314
#define NIKE_KEY_KP_0               320
#define NIKE_KEY_KP_1               321
#define NIKE_KEY_KP_2               322
#define NIKE_KEY_KP_3               323
#define NIKE_KEY_KP_4               324
#define NIKE_KEY_KP_5               325
#define NIKE_KEY_KP_6               326
#define NIKE_KEY_KP_7               327
#define NIKE_KEY_KP_8               328
#define NIKE_KEY_KP_9               329
#define NIKE_KEY_KP_DECIMAL         330
#define NIKE_KEY_KP_DIVIDE          331
#define NIKE_KEY_KP_MULTIPLY        332
#define NIKE_KEY_KP_SUBTRACT        333
#define NIKE_KEY_KP_ADD             334
#define NIKE_KEY_KP_ENTER           335
#define NIKE_KEY_KP_EQUAL           336
#define NIKE_KEY_LEFT_SHIFT         340
#define NIKE_KEY_LEFT_CONTROL       341
#define NIKE_KEY_LEFT_ALT           342
#define NIKE_KEY_LEFT_SUPER         343
#define NIKE_KEY_RIGHT_SHIFT        344
#define NIKE_KEY_RIGHT_CONTROL      345
#define NIKE_KEY_RIGHT_ALT          346
#define NIKE_KEY_RIGHT_SUPER        347
#define NIKE_KEY_MENU               348
#define NIKE_KEY_LAST               NIKE_KEY_MENU

/*****************************************************************//**
* MOUSE BUTTON CODES
*********************************************************************/
#define NIKE_MOUSE_BUTTON_1         0
#define NIKE_MOUSE_BUTTON_2         1
#define NIKE_MOUSE_BUTTON_3         2
#define NIKE_MOUSE_BUTTON_4         3
#define NIKE_MOUSE_BUTTON_5         4
#define NIKE_MOUSE_BUTTON_6         5
#define NIKE_MOUSE_BUTTON_7         6
#define NIKE_MOUSE_BUTTON_8         7
#define NIKE_MOUSE_BUTTON_LAST      NIKE_MOUSE_BUTTON_8
#define NIKE_MOUSE_BUTTON_LEFT      NIKE_MOUSE_BUTTON_1
#define NIKE_MOUSE_BUTTON_RIGHT     NIKE_MOUSE_BUTTON_2
#define NIKE_MOUSE_BUTTON_MIDDLE    NIKE_MOUSE_BUTTON_3

/*****************************************************************//**
* AUDIO DEFINES TYPES
*********************************************************************/
typedef unsigned int NIKE_AUDIO_TIMEUNIT;
#define NIKE_AUDIO_TIMEUNIT_MS                            0x00000001
#define NIKE_AUDIO_TIMEUNIT_PCM                           0x00000002
#define NIKE_AUDIO_TIMEUNIT_PCMBYTES                      0x00000004
#define NIKE_AUDIO_TIMEUNIT_RAWBYTES                      0x00000008
#define NIKE_AUDIO_TIMEUNIT_PCMFRACTION                   0x00000010
#define NIKE_AUDIO_TIMEUNIT_MODORDER                      0x00000100
#define NIKE_AUDIO_TIMEUNIT_MODROW                        0x00000200
#define NIKE_AUDIO_TIMEUNIT_MODPATTERN                    0x00000400

typedef unsigned int NIKE_AUDIO_MODE;
#define NIKE_AUDIO_DEFAULT                                0x00000000
#define NIKE_AUDIO_LOOP_OFF                               0x00000001
#define NIKE_AUDIO_LOOP_NORMAL                            0x00000002
#define NIKE_AUDIO_LOOP_BIDI                              0x00000004
#define NIKE_AUDIO_2D                                     0x00000008
#define NIKE_AUDIO_3D                                     0x00000010
#define NIKE_AUDIO_CREATESTREAM                           0x00000080
#define NIKE_AUDIO_CREATESAMPLE                           0x00000100
#define NIKE_AUDIO_CREATECOMPRESSEDSAMPLE                 0x00000200
#define NIKE_AUDIO_OPENUSER                               0x00000400
#define NIKE_AUDIO_OPENMEMORY                             0x00000800
#define NIKE_AUDIO_OPENMEMORY_POINT                       0x10000000
#define NIKE_AUDIO_OPENRAW                                0x00001000
#define NIKE_AUDIO_OPENONLY                               0x00002000
#define NIKE_AUDIO_ACCURATETIME                           0x00004000
#define NIKE_AUDIO_MPEGSEARCH                             0x00008000
#define NIKE_AUDIO_NONBLOCKING                            0x00010000
#define NIKE_AUDIO_UNIQUE                                 0x00020000 //! Sets audio to be unique, disabling the ability to play this audio in multiple channels
#define NIKE_AUDIO_3D_HEADRELATIVE                        0x00040000
#define NIKE_AUDIO_3D_WORLDRELATIVE                       0x00080000
#define NIKE_AUDIO_3D_INVERSEROLLOFF                      0x00100000
#define NIKE_AUDIO_3D_LINEARROLLOFF                       0x00200000
#define NIKE_AUDIO_3D_LINEARSQUAREROLLOFF                 0x00400000
#define NIKE_AUDIO_3D_INVERSETAPEREDROLLOFF               0x00800000
#define NIKE_AUDIO_3D_CUSTOMROLLOFF                       0x04000000
#define NIKE_AUDIO_3D_IGNOREGEOMETRY                      0x40000000
#define NIKE_AUDIO_IGNORETAGS                             0x02000000
#define NIKE_AUDIO_LOWMEM                                 0x08000000
#define NIKE_AUDIO_VIRTUAL_PLAYFROMSTART                  0x80000000

/*****************************************************************//**
* ENTITY & COMPONENT DEFINE TYPES
*********************************************************************/
#include <bitset>

namespace NIKESAURUS {
    namespace Entity {
        //Entity Type
        using Type = uint16_t;

        //Max number of entities to be created at a single point
        const Type MAX = 3000;
    }

    namespace Component {
        //Component Signature Type
        using Type = uint8_t;

        //Max components to be stored in a signature
        const Type MAX = INT8_MAX;

        //Nested components signature
        using Signature = std::bitset<MAX>;
    }
}
