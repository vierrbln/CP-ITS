/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2010. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/
#include <userint.h>
#ifdef __cplusplus
  extern "C"
  {
  #endif 
  /* Panels and Controls: */
  #define ADJUSTMENT                       1
  #define ADJUSTMENT_INDICATOR             2
  #define ADJUSTMENT_OK                    3       /* callback function: AdjustmentCallback */
  #define ADJUSTMENT_UNIT                  4
  #define ADJUSTMENT_LL                    5
  #define ADJUSTMENT_UL                    6
  #define ADJUSTMENT_BACKGROUND            7
  #define ADJUSTMENT_TEXT                  8
  #define DEBUG                            2
  #define DEBUG_TAB                        2       /* callback function: Tab */
  #define DEBUG_CLEAR                      3       /* callback function: ClearData */
  #define DEBUG_CPK                        4
  #define SINGLESTEP                       3
  #define SINGLESTEP_TERMINATE             2
  #define SINGLESTEP_CONTINUE              3
  #define SINGLESTEP_REPEAT                4
  #define SINGLESTEP_TEXTMSG               5
  /* tab page panel controls */
  #define ANALOG_GREEN                     2
  #define ANALOG_RED                       3
  #define ANALOG_UNIT                      4
  #define ANALOG_RESULT                    5
  #define ANALOG_ANALOG                    6
  #define ANALOG_VALUE                     7
  #define ANALOG_STRING                    8
  /* tab page panel controls */
  #define GRAPH_GRAPH                      2
  /* tab page panel controls */
  #define HISTOGRAM_HISTOGRAMM             2
  /* Menu Bars, Menus, and Menu Items: */
  #define MENUBAR                          1
  #define MENUBAR_SETTINGS                 2
  #define MENUBAR_SETTINGS_ALWAYSONTOP     3       /* callback function: AlwaysOnTop */
  #define MENUBAR_SETTINGS_SHOWCPK         4       /* callback function: ShowCpkValues */
  #define MENUBAR_SETTINGS_HISTO           5
  #define MENUBAR_SETTINGS_HISTO_SUBMENU   6
  #define MENUBAR_SETTINGS_HISTO_I10       7       /* callback function: ChangeHistogramInterval */
  #define MENUBAR_SETTINGS_HISTO_I20       8       /* callback function: ChangeHistogramInterval */
  #define MENUBAR_SETTINGS_HISTO_I30       9       /* callback function: ChangeHistogramInterval */
  #define MENUBAR_SETTINGS_HISTO_I40       10      /* callback function: ChangeHistogramInterval */
  #define MENUBAR_SETTINGS_HISTO_I50       11      /* callback function: ChangeHistogramInterval */
  #define MENUBAR_UTILITIES                12
  #define MENUBAR_UTILITIES_CLEARDATA      13      /* callback function: ClearDataMenu */
  /* Callback Prototypes: */
  int CVICALLBACK AdjustmentCallback(int panel, int control, int event, void* callbackData, int eventData1, int eventData2);
  void CVICALLBACK AlwaysOnTop(int menubar, int menuItem, void* callbackData, int panel);
  void CVICALLBACK ChangeHistogramInterval(int menubar, int menuItem, void* callbackData, int panel);
  int CVICALLBACK ClearData(int panel, int control, int event, void* callbackData, int eventData1, int eventData2);
  void CVICALLBACK ClearDataMenu(int menubar, int menuItem, void* callbackData, int panel);
  void CVICALLBACK ShowCpkValues(int menubar, int menuItem, void* callbackData, int panel);
  int CVICALLBACK Tab(int panel, int control, int event, void* callbackData, int eventData1, int eventData2);
  #ifdef __cplusplus
  }
#endif
