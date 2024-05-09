/* See LICENSE file for copyright and license details. */

#include "gaplessgrid.c"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int gappx     = 6;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int user_bh            = 4;        /* 2 is the default spacing around the bar's font */
static const int focusonwheel       = 0;
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "Hack-14";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_lgreen[]      = "#aaff00";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_lgreen  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating    canfocus    monitor */
	{ "ripdrag",  NULL,       NULL,       0,            1,		  1,          -1 },
	{ "Gimp",     NULL,       NULL,       0,            1,		  1,          -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,		  1,          -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle }, /* full screen */
	{ "[G]",      gaplessgrid }, /* grid layout */
};

/* key definitions */
#define MODKEY Mod4Mask
#define AltMask Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \ 
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} }, 

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
#define STATUSBAR "dwmblocks"

static const int dmenudesktop = 1; /* 1 means dmenu will use only desktop files from [/usr/share/applications/] */

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-c", "-i", "-l", "30", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, "-p", "Run:", NULL };
static const char *flameshotcmd[] = { "flameshot", "gui", NULL };
#define TERMINAL_ENVVAR "TERMINAL"
#define BROWSER_ENVVAR "BROWSER"

static const Key keys[] = {
	/* modifier                     key        function        argument */
	// COMMON APPS
	{ MODKEY,                       XK_x,      spawn,          {.v = dmenucmd } }, // app launcher
	{ MODKEY,	                XK_t,      spawn,          {.v = termcmd } }, // terminal
	{ MODKEY,	                XK_b,      spawn,          {.v = browsercmd } }, // browser
	{ MODKEY,	                XK_s,      spawn,          {.v = flameshotcmd } }, // screenshot tool
	{ MODKEY,			XK_f,      spawn,          {.v = rangercmd } }, // file manager
	// WINDOW NAVIGATION
	{ MODKEY,                       XK_grave,  focusstack,     {.i = +1 } }, // cycle window focus
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } }, // switch focus to window down
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } }, // switch focus to window up
	{ MODKEY,	                XK_q,      killclient,     {0} }, // quit app
	{ MODKEY|ShiftMask,             XK_q,      quitwmprompt,   {0} }, // DWM session exit prompt
	{ MODKEY|ControlMask|ShiftMask, XK_q,      shutdownprompt, {0} }, // shutdown the system
	// WINDOW LAYOUT
	{ MODKEY,                       XK_Return, zoom,           {0} }, // make selected the master window
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } }, // add master window slot
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } }, // subtract master window slot
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} }, // increase master width
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} }, // decrease master width
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} }, // hide top bar
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[0]} }, // tiling
	{ MODKEY|ShiftMask,		XK_f,      setlayout,      {.v = &layouts[1]} }, // floating
	{ MODKEY|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[2]} }, // maximised
	{ MODKEY|ShiftMask,             XK_g,      setlayout,      {.v = &layouts[3]} }, // grid
	{ MODKEY|ShiftMask,             XK_space,  setlayout,      {0} }, // switch between last two layouts
	// WINDOW MODES
	{ MODKEY|ControlMask,           XK_f,      togglefloating, {0} }, // make window floating
	{ MODKEY|AltMask,               XK_f,      togglefullscr,  {0} }, // make window full screen
	// TAG NAVIGATION
	{ MODKEY,                       XK_Tab,    view,           {0} }, // switch between last two tags
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } }, // view all tags
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } }, // stick selected to all tags
	// MONITOR NAVIGATION
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } }, // switch focus to monitor up
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } }, // switch focus to monitor down
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } }, // move window to monitor up
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } }, // move window to monitor down
	// TAG KEYS
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1 } },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2 } },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3 } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

