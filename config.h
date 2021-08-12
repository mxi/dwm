/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int gappx     = 16;       /* gaps between windows */
static const unsigned int snap      = 16;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */

static const char font_mono[]       = "Noto Mono:size=14:antialias=true:autohint=true";
static const char font_sans[]       = "Noto Sans:size=14:antialias=true:autohint=true";
static const char font_serif[]      = "Noto Serif:size=14:antialias=true:autohint=true";
static const char font_awesome[]    = "Font Awesome 5 Free:size=14:antialias=true:autohint=true";
static const char *fonts[]          = { 
	font_mono,
	font_sans,
	font_serif,
	font_awesome,
};

static const char clr_black[]       = "#000000";
static const char clr_gray []       = "#808080";
static const char clr_white[]       = "#ffffff";
static const char clr_red  []       = "#ff6060";
static const char clr_green[]       = "#60ff60";
static const char clr_blue []       = "#6060ff";

enum { 
	SchemeNorm, 
	SchemeCol1, 
	SchemeCol2, 
	SchemeCol3, 
	SchemeCol4,
	SchemeCol5, 
	SchemeCol6, 
	SchemeSel 
}; /* color schemes */

static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { clr_gray , clr_black, clr_black    },
	[SchemeCol1] = { clr_white, clr_black, clr_black    }, /* WHT = white */
	[SchemeCol2] = { clr_red  , clr_black, clr_black    }, /* RED = red */
	[SchemeCol3] = { clr_green, clr_black, clr_black    }, /* GRN = green */
	[SchemeCol4] = { clr_blue , clr_black, clr_black    }, /* BLU = blue */
	[SchemeSel]  = { clr_white, clr_black, clr_white    },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", 
	"-m",  dmenumon , 
	"-fn", font_mono,
	"-nb", clr_black, 
	"-nf", clr_gray , 
	"-sb", clr_black, 
	"-sf", clr_white,
	NULL,
};

static const char *termcmd[]  = { "st", 
	NULL,
};

static const char *incr_brightness[] = { "brightness-mod.sh",
	"1",
	NULL,
};

static const char *decr_brightness[] = { "brightness-mod.sh",
	"-1",
	NULL,
};

static const char *mute_source[] = { "pactl",
	"set-source-mute",
	"@DEFAULT_SOURCE@",
	"toggle",
	NULL,
};

static const char *mute_sink[] = { "pactl",
	"set-sink-mute",
	"@DEFAULT_SINK@",
	"toggle",
	NULL,
};

static const char *incr_sink[] = { "pactl",
	"set-sink-volume",
	"@DEFAULT_SINK@",
	"+1%",
	NULL,
};

static const char *decr_sink[] = { "pactl",
	"set-sink-volume",
	"@DEFAULT_SINK@",
	"-1%",
	NULL,
};


/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

static Key keys[] = {
	/* modifier              key                 function        argument */
	/* spawners */
	{ MODKEY|ShiftMask, XK_Return,               spawn,          {.v = termcmd } },
	{ MODKEY,           XK_p,                    spawn,          {.v = dmenucmd } },
	{ 0,                XF86XK_MonBrightnessUp,  spawn,          {.v = incr_brightness } },
	{ 0,                XF86XK_MonBrightnessDown,spawn,          {.v = decr_brightness } },
	{ 0,                XK_F7,                   spawn,          {.v = mute_source } },
	{ 0,                XF86XK_AudioMute,        spawn,          {.v = mute_sink } },
	{ 0,                XF86XK_AudioRaiseVolume, spawn,          {.v = incr_sink } },
	{ 0,                XF86XK_AudioLowerVolume, spawn,          {.v = decr_sink } },

	/* spawners */
	{ MODKEY,           XK_b,                    togglebar,      {0} },
	{ MODKEY,           XK_j,                    focusstack,     {.i = +1 } },
	{ MODKEY,           XK_k,                    focusstack,     {.i = -1 } },
	{ MODKEY,           XK_i,                    incnmaster,     {.i = +1 } },
	{ MODKEY,           XK_d,                    incnmaster,     {.i = -1 } },
	{ MODKEY,           XK_h,                    setmfact,       {.f = -0.05} },
	{ MODKEY,           XK_l,                    setmfact,       {.f = +0.05} },
	{ MODKEY,           XK_Return,               zoom,           {0} },
	{ MODKEY,           XK_Tab,                  view,           {0} },
	{ MODKEY|ShiftMask, XK_c,                    killclient,     {0} },
	{ MODKEY,           XK_t,                    setlayout,      {.v = &layouts[0]} },
	{ MODKEY,           XK_f,                    setlayout,      {.v = &layouts[1]} },
	{ MODKEY,           XK_m,                    setlayout,      {.v = &layouts[2]} },
	{ MODKEY,           XK_space,                setlayout,      {0} },
	{ MODKEY|ShiftMask, XK_space,                togglefloating, {0} },
	{ MODKEY,           XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask, XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,           XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,           XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask, XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask, XK_period,               tagmon,         {.i = +1 } },
	{ MODKEY,           XK_minus,                setgaps,        {.i = -snap } },
	{ MODKEY,           XK_equal,                setgaps,        {.i = +snap } },
	{ MODKEY|ShiftMask, XK_equal,                setgaps,        {.i = 0  } },
	{ MODKEY|ShiftMask, XK_q,                    quit,           {0} },
	TAGKEYS(            XK_1,                                    0)
	TAGKEYS(            XK_2,                                    1)
	TAGKEYS(            XK_3,                                    2)
	TAGKEYS(            XK_4,                                    3)
	TAGKEYS(            XK_5,                                    4)
	TAGKEYS(            XK_6,                                    5)
	TAGKEYS(            XK_7,                                    6)
	TAGKEYS(            XK_8,                                    7)
	TAGKEYS(            XK_9,                                    8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

