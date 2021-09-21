/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  BRIEFING                         1
#define  BRIEFING_CONTINUE                2       /* control type: command, callback function: (none) */
#define  BRIEFING_TEXTBOX                 3       /* control type: textBox, callback function: (none) */

#define  MEASCURVE                        2       /* callback function: MeasCurvePanelCallback */
#define  MEASCURVE_PRINT                  2       /* control type: command, callback function: PrintGraphWindow */
#define  MEASCURVE_CLOSE                  3       /* control type: command, callback function: CloseGraphWindow */
#define  MEASCURVE_HISTOGRAM              4       /* control type: graph, callback function: (none) */
#define  MEASCURVE_GRAPH                  5       /* control type: graph, callback function: (none) */
#define  MEASCURVE_HISTOSLIDER            6       /* control type: scale, callback function: ChangeHistogramBars */

#define  OP_SINGLE                        3       /* callback function: OpSingleCallback */
#define  OP_SINGLE_ACTUALTESTSTEP         2       /* control type: textMsg, callback function: (none) */
#define  OP_SINGLE_RUN_TEST               3       /* control type: command, callback function: RunTest */
#define  OP_SINGLE_SERIAL                 4       /* control type: string, callback function: (none) */
#define  OP_SINGLE_ACTUALTESTSTEP_DESC    5       /* control type: textMsg, callback function: (none) */
#define  OP_SINGLE_TESTSTATUS_MSG         6       /* control type: textMsg, callback function: (none) */
#define  OP_SINGLE_DECORATION             7       /* control type: deco, callback function: (none) */
#define  OP_SINGLE_MSA_INDICATOR          8       /* control type: LED, callback function: MSAIndicatorCallBack */
#define  OP_SINGLE_STROKEINDICATOR        9       /* control type: LED, callback function: (none) */
#define  OP_SINGLE_REPORT                 10      /* control type: LED, callback function: (none) */
#define  OP_SINGLE_TICKET                 11      /* control type: LED, callback function: (none) */
#define  OP_SINGLE_YIELD                  12      /* control type: LED, callback function: (none) */
#define  OP_SINGLE_PRODUCTNAME            13      /* control type: textMsg, callback function: (none) */
#define  OP_SINGLE_TEXTMSG_2              14      /* control type: textMsg, callback function: (none) */
#define  OP_SINGLE_TIMERSTROKEINDICATOR   15      /* control type: timer, callback function: TimerForStrokeIndicator */
#define  OP_SINGLE_TIMERFORVARIANT        16      /* control type: timer, callback function: TimerForVariant */
#define  OP_SINGLE_COL_RUNNING            17      /* control type: numeric, callback function: (none) */
#define  OP_SINGLE_COL_FAIL               18      /* control type: numeric, callback function: (none) */
#define  OP_SINGLE_COL_PASS               19      /* control type: numeric, callback function: (none) */
#define  OP_SINGLE_STROKE_RED             20      /* control type: numeric, callback function: (none) */
#define  OP_SINGLE_STROKE_YELLOW          21      /* control type: numeric, callback function: (none) */
#define  OP_SINGLE_STROKE_ACT             22      /* control type: numeric, callback function: (none) */
#define  OP_SINGLE_VARIANT_RING           23      /* control type: ring, callback function: ChangeVariantRing */
#define  OP_SINGLE_TAB                    24      /* control type: tab, callback function: TabCallBack */
#define  OP_SINGLE_DECORATION_2           25      /* control type: deco, callback function: (none) */
#define  OP_SINGLE_TITEL2                 26      /* control type: textMsg, callback function: (none) */
#define  OP_SINGLE_USECOLOREDBG           27      /* control type: radioButton, callback function: (none) */
#define  OP_SINGLE_OPERATOR               28      /* control type: textMsg, callback function: (none) */
#define  OP_SINGLE_TIMER_MSA_RUNBUTTON    29      /* control type: timer, callback function: TimerForRunButtonIfMSAEnabled */
#define  OP_SINGLE_SEQFILEPATH            30      /* control type: textMsg, callback function: (none) */
#define  OP_SINGLE_GENERALTIMER           31      /* control type: timer, callback function: GeneralTimerCallback */
#define  OP_SINGLE_ORDERNUMBER            32      /* control type: textMsg, callback function: OrderNumberCallback */
#define  OP_SINGLE_TIMER_RUN_BUTTON       33      /* control type: timer, callback function: TimerForRunButton */
#define  OP_SINGLE_TIMERFORORDERNUM       34      /* control type: timer, callback function: TimerForOrderNumber */

#define  SAVING_POP                       4
#define  SAVING_POP_MSG                   2       /* control type: textMsg, callback function: (none) */
#define  SAVING_POP_SAVING                3       /* control type: scale, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_SPLITTER                2       /* control type: splitter, callback function: (none) */
#define  TABPANEL_TESTCASEPATHWAY         3       /* control type: listBox, callback function: (none) */
#define  TABPANEL_TESTERRORLOG            4       /* control type: listBox, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_2_TABLE                 2       /* control type: table, callback function: ValueTableCallBack */
#define  TABPANEL_2_BUFFERCPK             3       /* control type: numeric, callback function: (none) */
#define  TABPANEL_2_TEXTMSG               4       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_PROGRAM                  2
#define  MENUBAR_PROGRAM_MODE             3
#define  MENUBAR_PROGRAM_MODE_SUBMENU     4
#define  MENUBAR_PROGRAM_MODE_NORMAL      5       /* callback function: RunModeNormal */
#define  MENUBAR_PROGRAM_MODE_SINGLESTEP  6       /* callback function: RunModeSingleStep */
#define  MENUBAR_PROGRAM_MODE_STOPONFAIL  7       /* callback function: RunModeStopFail */
#define  MENUBAR_PROGRAM_MODE_SEPARATOR_4 8
#define  MENUBAR_PROGRAM_MODE_MSATYPE1    9       /* callback function: RunModeMSAType1 */
#define  MENUBAR_PROGRAM_ORDERNUMBER      10      /* callback function: ChangeOrderNumberMenu */
#define  MENUBAR_PROGRAM_SEPARATOR_2      11
#define  MENUBAR_PROGRAM_ABOUT            12      /* callback function: About */
#define  MENUBAR_PROGRAM_SEPARATOR_3      13
#define  MENUBAR_PROGRAM_QUIT             14      /* callback function: Quit */
#define  MENUBAR_SET                      15
#define  MENUBAR_SET_TICKET               16
#define  MENUBAR_SET_TICKET_SUBMENU       17
#define  MENUBAR_SET_TICKET_ENABLE        18      /* callback function: EnableTicketPrinter */
#define  MENUBAR_SET_REPORT               19
#define  MENUBAR_SET_REPORT_SUBMENU       20
#define  MENUBAR_SET_REPORT_ENABLE        21      /* callback function: EnableReport */
#define  MENUBAR_SET_REPORT_APPEND        22      /* callback function: AppendReport */
#define  MENUBAR_SET_REPORT_ONLYFAIL      23      /* callback function: OnlyFailedTests */
#define  MENUBAR_SET_UI                   24
#define  MENUBAR_SET_UI_SUBMENU           25
#define  MENUBAR_SET_UI_ACTUAL            26      /* callback function: UIActualTestStep */
#define  MENUBAR_SET_UI_REPORT            27      /* callback function: UIOnlyFailedTests */
#define  MENUBAR_SET_SEPARATOR            28
#define  MENUBAR_SET_RESETSTAT            29      /* callback function: ResetStatistics */


     /* Callback Prototypes: */

void CVICALLBACK About(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK AppendReport(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK ChangeHistogramBars(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK ChangeOrderNumberMenu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK ChangeVariantRing(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CloseGraphWindow(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK EnableReport(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK EnableTicketPrinter(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK GeneralTimerCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MeasCurvePanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MSAIndicatorCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK OnlyFailedTests(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK OpSingleCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OrderNumberCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PrintGraphWindow(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Quit(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ResetStatistics(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK RunModeMSAType1(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK RunModeNormal(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK RunModeSingleStep(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK RunModeStopFail(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK RunTest(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TabCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerForOrderNumber(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerForRunButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerForRunButtonIfMSAEnabled(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerForStrokeIndicator(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerForVariant(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK UIActualTestStep(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK UIOnlyFailedTests(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK ValueTableCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
