/****************************************************************************/
/**

Copyright 2007-2021 Robert Renner

This file is part of SW-ITS.

SW-ITS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SW-ITS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SW-ITS.  If not, see <http://www.gnu.org/licenses/>.

Diese Datei ist Teil von SW-ITS.

SW-ITS ist Freie Software: Sie können es unter den Bedingungen
der GNU General Public License, wie von der Free Software Foundation,
Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
veröffentlichten Version, weiter verteilen und/oder modifizieren.

SW-ITS wird in der Hoffnung, dass es nützlich sein wird, aber
OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
Siehe die GNU General Public License für weitere Details.

Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
Programm erhalten haben. Wenn nicht, siehe <https://www.gnu.org/licenses/>.

 *****************************************************************************
 *****************************************************************************/

/* INCLUDE FILES **************************************************************/

#include "definitions.h"
#include "tspan.h"

/* GLOBAL VARIABLES DEFINITION ************************************************/

/* LOCAL DEFINES *************************************************/

/* LOCAL FUNCTION DEFINITIONS *************************************************/

/* LOCAL CONSTANT DEFINITIONS *************************************************/

/* LOCAL TYPE DEFINITIONS *****************************************************/

/* LOCAL MACRO DEFINITIONS ****************************************************/

/* LOCAL FUNCTION DECLARATIONS ************************************************/

/* GLOBAL FUNCTION DEFINITIONS **********************************************/

/* FUNCTION *****************************************************************/
/**
RunButtonHit
*
@brief OperatorPanel || If run button is enabled in application.ini, this button starts a test run in teststand
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int RunButtonHit (int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int menubarhandle;
  int iNumberOfMenus;
  int idx;
  int iMenuID;
  int iVisibleTextBoxLines;
  char cSerial[1024];
  char cTextBuffer[1024];
  int iStrokeActualCounts;
  int iStrokeCounterEnabled;
  int iYellowLimit;
  int iRedLimit;
  int iColorValue;
  int iEnableColor;
  int iActualTabHandle;
  double dDateTime;
  //Disable Timer for automatic run button hit if msa is enabled  
  if (giMSAType1IsEnabled)
  {
    SetCtrlAttribute(panel, OP_SINGLE_TIMER_MSA_RUNBUTTON, ATTR_ENABLED, FALSE);
  }
  sprintf(cSerial, "");
  if (giSerialIsMandatory)
  {
    GetCtrlVal(panel, OP_SINGLE_SERIAL, cSerial);
    if (!strlen(cSerial) && (giSerialIsMandatory == 1))
    {
      MessagePopup("CP-ITS ControlPanel - Error", "Please enter serial number! Test cannot be executed.");
      return 0;
    }
    if ((strlen(cSerial) != giSerialLength) && (giSerialIsMandatory > 1))
    {
      MessagePopup("CP-ITS ControlPanel - Error", "The length of serial number is not valid. Please enter valid number! Test cannot be executed.");
      return 0;
    }
    sprintf(gcSerialNumber, cSerial);
  }
  threadData = (ThreadData*)callbackData;
  SetActiveTabPage(panel, OP_SINGLE_TAB, 0);
  SetCtrlAttribute(panel, OP_SINGLE_RUN_TEST, ATTR_DIMMED, FALSE);
  SetCtrlAttribute(panel, OP_SINGLE_RUN_TEST, ATTR_LABEL_TEXT, "TERMINATE TEST [CTRL+T]");
  SetCtrlAttribute(panel, OP_SINGLE_RUN_TEST, ATTR_SHORTCUT_KEY, VAL_MENUKEY_MODIFIER | 'T');
  SetCtrlAttribute(panel, OP_SINGLE_SERIAL, ATTR_DIMMED, TRUE);
  SetCtrlAttribute(panel, OP_SINGLE_VARIANT_RING, ATTR_DIMMED, TRUE);
  menubarhandle = GetPanelMenuBar(panel);
  iMenuID =  - 1;
  while (iMenuID != 0)
  {
    if (iMenuID ==  - 1)
    {
      GetMenuBarAttribute(menubarhandle, 0, ATTR_FIRST_MENU_ID, &iMenuID);
    }
    else
    {
      GetMenuBarAttribute(menubarhandle, iMenuID, ATTR_NEXT_MENU_ID, &iMenuID);
    }
    if (iMenuID)
    {
      SetMenuBarAttribute(menubarhandle, iMenuID, ATTR_DIMMED, TRUE);
    }
  }
  GetPanelHandleFromTabPage(panel, OP_SINGLE_TAB, 0, &iActualTabHandle);
  GetCtrlAttribute(iActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_VISIBLE_LINES, &iVisibleTextBoxLines);
  if (iVisibleTextBoxLines)
  {
    DeleteListItem(iActualTabHandle, TABPANEL_TESTCASEPATHWAY, 0,  - 1);
  }
  giActualItemIndex = 0;
  //Set BG Color to specified value
  GetCtrlVal(panel, OP_SINGLE_COL_RUNNING, &iColorValue);
  GetCtrlVal(panel, OP_SINGLE_USECOLOREDBG, &iEnableColor);
  if (iEnableColor)
  {
    SetCtrlAttribute(iActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_BGCOLOR, iColorValue);
  }
  SetCtrlAttribute(panel, OP_SINGLE_TESTSTATUS_MSG, ATTR_TEXT_BGCOLOR, iColorValue);
  SetCtrlVal(panel, OP_SINGLE_TESTSTATUS_MSG, "TEST RUNNING");
  SetCtrlAttribute(panel, OP_SINGLE_ACTUALTESTSTEP, ATTR_LABEL_TEXT, "Current Test Step");
  gdStartTimeStamp = HRESTIM_getTimeStamp();
  if (giSerialIsMandatory)
  {
    TS_PropertySetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.SerialNumber", TS_PropOption_InsertIfMissing, cSerial);
  }
  TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.RunTest", TS_PropOption_InsertIfMissing, VTRUE);
  GetCtrlVal(panel, OP_SINGLE_STROKE_ACT, &iStrokeActualCounts);
  GetCtrlVal(panel, OP_SINGLE_STROKE_YELLOW, &iYellowLimit);
  GetCtrlVal(panel, OP_SINGLE_STROKE_RED, &iRedLimit);
  GetCtrlVal(panel, OP_SINGLE_STROKEINDICATOR, &iStrokeCounterEnabled);
  if ((iStrokeCounterEnabled == TRUE) && (iStrokeActualCounts >= 0))
  {
    iStrokeActualCounts++;
    SetCtrlVal(panel, OP_SINGLE_STROKE_ACT, iStrokeActualCounts);
    sprintf(cTextBuffer, "Stroke Indicator (%i)", iStrokeActualCounts);
    SetCtrlAttribute(panel, OP_SINGLE_STROKEINDICATOR, ATTR_LABEL_TEXT, cTextBuffer);
    if (iStrokeActualCounts >= iYellowLimit)
    {
      SetCtrlAttribute(panel, OP_SINGLE_STROKEINDICATOR, ATTR_ON_COLOR, VAL_YELLOW);
    }
    if (iStrokeActualCounts >= iRedLimit)
    {
      SetCtrlAttribute(panel, OP_SINGLE_TIMERSTROKEINDICATOR, ATTR_ENABLED, TRUE);
      SetCtrlAttribute(panel, OP_SINGLE_STROKEINDICATOR, ATTR_ON_COLOR, VAL_RED);
    }
  }
  //List of Measuremts
  giActualTestResult++;
  if (giActualTestResult > 49)
  {
    giActualTestResult = 0;
  }
  giTestIsRunning = TRUE;
  GetCurrentDateTime(&dDateTime);
  FormatDateTimeString(dDateTime, "%d.%m.%Y - %H:%M:%S", cTextBuffer, 1024);
  sprintf(giAllTestResults[giActualTestResult]->cStartDate, cTextBuffer);
  if (giSerialIsMandatory)
  {
    sprintf(giAllTestResults[giActualTestResult]->cSerial, cSerial);
  }
  else
  {
    sprintf(giAllTestResults[giActualTestResult]->cSerial, "Index %i", giCountOfUUTs + 1);
  }
  if (strlen(gcSelectedVariant))
  {
    sprintf(giAllTestResults[giActualTestResult]->cVariant, gcSelectedVariant);
  }
  else
  {
    sprintf(giAllTestResults[giActualTestResult]->cVariant, "");
  }
  if (giAllTestResults[giActualTestResult]->pFirst)
  {
    vFreeAllTestResults(giAllTestResults[giActualTestResult]->pFirst);
  }
  giAllTestResults[giActualTestResult]->pFirst = NULL;
  return 0;
}

/* FUNCTION *****************************************************************/
/**
ClearData
*
@brief Debugpanel || Clear all stored measurement eg in histogram
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK ClearData(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  int iTabHandle;
  switch (event)
  {
    case EVENT_COMMIT:
      //Analog
      GetPanelHandleFromTabPage(panel, DEBUG_TAB, 0, &iTabHandle);
      DeleteGraphPlot(iTabHandle, ANALOG_ANALOG,  - 1, VAL_IMMEDIATE_DRAW);
      //Histogramm
      GetPanelHandleFromTabPage(panel, DEBUG_TAB, 1, &iTabHandle);
      DeleteGraphPlot(iTabHandle, HISTOGRAM_HISTOGRAMM,  - 1, VAL_IMMEDIATE_DRAW);
      //Graph
      GetPanelHandleFromTabPage(panel, DEBUG_TAB, 2, &iTabHandle);
      DeleteGraphPlot(iTabHandle, GRAPH_GRAPH,  - 1, VAL_IMMEDIATE_DRAW);
      giMinValue = 0;
      giMaxValue = 0;
      giLowerLimit = 0;
      giUpperLimit = 0;
      ResetMeasurements();
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
TimerForVariant
*
@brief Operatorpanel || Variant is blinking if no variant is selected
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK TimerForVariant(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  int iColor;
  switch (event)
  {
    case EVENT_TIMER_TICK:
      GetCtrlAttribute(panel, OP_SINGLE_VARIANT_RING, ATTR_LABEL_BGCOLOR, &iColor);
      if (iColor != VAL_RED)
      {
        SetCtrlAttribute(panel, OP_SINGLE_VARIANT_RING, ATTR_LABEL_BGCOLOR, VAL_RED);
      }
      if (iColor == VAL_RED)
      {
        SetCtrlAttribute(panel, OP_SINGLE_VARIANT_RING, ATTR_LABEL_BGCOLOR, VAL_TRANSPARENT);
      }
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
TimerForStrokeIndicator
*
@brief Operatorpanel || Stroke indicator is blinking if maximum contacts are reached
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK TimerForStrokeIndicator(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  int iOn;
  switch (event)
  {
    case EVENT_TIMER_TICK:
      GetCtrlVal(panel, OP_SINGLE_STROKEINDICATOR, &iOn);
      SetCtrlVal(panel, OP_SINGLE_STROKEINDICATOR, !iOn);
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
RunTest
*
@brief Operatorpanel || Starts a test run or terminates a test run
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK RunTest(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  char cButtonLabel[1024];
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  HRESULT hResult;
  threadData = (ThreadData*)callbackData;
  switch (event)
  {
    case EVENT_COMMIT:
      GetCtrlAttribute(panel, control, ATTR_LABEL_TEXT, cButtonLabel);
      if (cButtonLabel[0] == 'R')
      {
        RunButtonHit(panel, control, event, callbackData, eventData1, eventData2);
      }
      else
      {
        hResult = TS_ExecutionTerminate(threadData->execution, &pTSErrorInfo);
      }
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
Quit
*
@brief Operatorpanel || Quit a teststand sequence
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK Quit(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  threadData = (ThreadData*)callbackData;
  TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.QuitProgram", TS_PropOption_InsertIfMissing, VTRUE);
} 

/* FUNCTION *****************************************************************/
/**
RunModeNormal
*
@brief Operatorpanel || Set the run mode of teststand sequence to normal mode
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK RunModeNormal(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int iChecked;
  char cTraceBuffer[1024];
  threadData = (ThreadData*)callbackData;
  GetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_NORMAL, ATTR_CHECKED, & iChecked);
  if (!iChecked)
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.StopOnFail", TS_PropOption_InsertIfMissing, VFALSE);
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.SingleStep", TS_PropOption_InsertIfMissing, VFALSE);
    giYieldCheckEnabledMenu = VTRUE;
    if (giYieldCheckEnabledFile)
    {
      SetCtrlVal(panel, OP_SINGLE_YIELD, VTRUE);
    }
    SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_NORMAL, ATTR_CHECKED, VTRUE);
    SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_SINGLESTEP, ATTR_CHECKED, VFALSE);
    SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_STOPONFAIL, ATTR_CHECKED, VFALSE);
    giPassUUTs = 0;
    giFailUUTs = 0;
    giConsecutiveFailed = 0;
    giCountOfUUTs = 0;
    gdQRate = 0;
    sprintf(cTraceBuffer, "Count: %i, Pass: %i, Fail: %i, Q: %.2f %%", giCountOfUUTs, giPassUUTs, giFailUUTs, gdQRate);
    SetCtrlVal(panel, OP_SINGLE_ACTUALTESTSTEP, cTraceBuffer);
  }
}

/* FUNCTION *****************************************************************/
/**
RunModeSingleStep
*
@brief Operatorpanel || Set the run mode of teststand sequence to single step mode
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK RunModeSingleStep(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  threadData = (ThreadData*)callbackData;
  TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.StopOnFail", TS_PropOption_InsertIfMissing, VFALSE);
  TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.SingleStep", TS_PropOption_InsertIfMissing, VTRUE);
  giYieldCheckEnabledMenu = VFALSE;
  SetCtrlVal(panel, OP_SINGLE_YIELD, VFALSE);
  SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_NORMAL, ATTR_CHECKED, VFALSE);
  SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_SINGLESTEP, ATTR_CHECKED, VTRUE);
  SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_STOPONFAIL, ATTR_CHECKED, VFALSE);
}

/* FUNCTION *****************************************************************/
/**
RunModeStopFail
*
@brief Operatorpanel || Set the run mode of teststand sequence to stop on fail mode
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK RunModeStopFail(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  threadData = (ThreadData*)callbackData;
  TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.StopOnFail", TS_PropOption_InsertIfMissing, VTRUE);
  TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.SingleStep", TS_PropOption_InsertIfMissing, VFALSE);
  giYieldCheckEnabledMenu = VFALSE;
  SetCtrlVal(panel, OP_SINGLE_YIELD, VFALSE);
  SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_NORMAL, ATTR_CHECKED, VFALSE);
  SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_SINGLESTEP, ATTR_CHECKED, VFALSE);
  SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_STOPONFAIL, ATTR_CHECKED, VTRUE);
}

/* FUNCTION *****************************************************************/
/**
EnableReport
*
@brief Operatorpanel || Enables the report file writing 
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK EnableReport(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int iChecked;
  threadData = (ThreadData*)callbackData;
  GetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, &iChecked);
  if (iChecked)
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.ReportResults", TS_PropOption_InsertIfMissing, VFALSE);
  }
  else
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.ReportResults", TS_PropOption_InsertIfMissing, VTRUE);
  }
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, !iChecked);
  SetCtrlVal(panel, OP_SINGLE_REPORT, !iChecked);
}

/* FUNCTION *****************************************************************/
/**
AppendReport
*
@brief Operatorpanel || Enables the append mode for report file writing 
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK AppendReport(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int iChecked;
  threadData = (ThreadData*)callbackData;
  GetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, &iChecked);
  if (iChecked)
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.AppendReport", TS_PropOption_InsertIfMissing, VFALSE);
  }
  else
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.AppendReport", TS_PropOption_InsertIfMissing, VTRUE);
  }
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, !iChecked);
}

/* FUNCTION *****************************************************************/
/**
OnlyFailedTests
*
@brief Operatorpanel || Enables the only failed mode for report file writing 
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK OnlyFailedTests(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int iChecked;
  threadData = (ThreadData*)callbackData;
  GetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, &iChecked);
  if (iChecked)
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.ReportOnlyFail", TS_PropOption_InsertIfMissing, VFALSE);
  }
  else
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.ReportOnlyFail", TS_PropOption_InsertIfMissing, VTRUE);
  }
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, !iChecked);
}

/* FUNCTION *****************************************************************/
/**
UIActualTestStep
*
@brief Operatorpanel || Enables the setting of actual test step while running teststeps 
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK UIActualTestStep(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int iChecked;
  threadData = (ThreadData*)callbackData;
  GetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, &iChecked);
  if (iChecked)
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.UI_ActualTestStep", TS_PropOption_InsertIfMissing, VFALSE);
  }
  else
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.UI_ActualTestStep", TS_PropOption_InsertIfMissing, VTRUE);
  }
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, !iChecked);
}

/* FUNCTION *****************************************************************/
/**
UIOnlyFailedTests
*
@brief Operatorpanel || Enables the reporting of teststeps in gui with option only failed tests 
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK UIOnlyFailedTests(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int iChecked;
  threadData = (ThreadData*)callbackData;
  GetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, &iChecked);
  if (iChecked)
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.UI_ReportOnlyFail", TS_PropOption_InsertIfMissing, VFALSE);
  }
  else
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.UI_ReportOnlyFail", TS_PropOption_InsertIfMissing, VTRUE);
  }
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, !iChecked);
}

/* FUNCTION *****************************************************************/
/**
EnableTicketPrinter
*
@brief Operatorpanel || Enables the ticket printer reporting option 
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK EnableTicketPrinter(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int iChecked;
  threadData = (ThreadData*)callbackData;
  GetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, &iChecked);
  if (iChecked)
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.EnableTicketPrinter", TS_PropOption_InsertIfMissing, VFALSE);
  }
  else
  {
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.EnableTicketPrinter", TS_PropOption_InsertIfMissing, VTRUE);
  }
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, !iChecked);
  SetCtrlVal(panel, OP_SINGLE_TICKET, !iChecked);
}

/* FUNCTION *****************************************************************/
/**
ChangeVariant
*
@brief Operatorpanel || Set the correspondig teststand var if the operator changes the variant
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK ChangeVariant(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  HRESULT hResult;
  char cName[1024];
  char cBuffer[1024];
  char cFirstListValue[1024];
  int iFirstItem, iNextItem, VariantsMenu_ID;
  threadData = (ThreadData*)callbackData;
  GetMenuBarAttribute(menuBar, menuItem, ATTR_ITEM_NAME, cName);
  sprintf(gcSelectedVariant, cName);
  hResult = TS_PropertySetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.SelectedVariant", TS_PropOption_InsertIfMissing, cName);
  sprintf(cBuffer, "%s", cName);
  SetCtrlVal(panel, OP_SINGLE_VARIANT_RING, cBuffer);
  //Delete "please select a variant"
  GetValueFromIndex(panel, OP_SINGLE_VARIANT_RING, 0, cFirstListValue);
  if (!strcmp(cFirstListValue, "No variant selected"))
  {
    DeleteListItem(panel, OP_SINGLE_VARIANT_RING, 0, 1);
  }
  if (!giRunButtonDisabled)
  {
    iVariantIsSet = 1;
  }
  GetMenuBarAttribute(menuBar, 0, ATTR_FIRST_MENU_ID, &iFirstItem);
  GetMenuBarAttribute(menuBar, iFirstItem, ATTR_MENU_NAME, cName);
  iNextItem = 1;
  VariantsMenu_ID = 1;
  while (iNextItem)
  {
    GetMenuBarAttribute(menuBar, iFirstItem, ATTR_NEXT_MENU_ID, &iNextItem);
    if (iNextItem)
    {
      GetMenuBarAttribute(menuBar, iNextItem, ATTR_MENU_NAME, cName);
    }
    if (!strcmp(cName, "Variants"))
    {
      VariantsMenu_ID = iNextItem;
      iNextItem = 0;
    }
    iFirstItem = iNextItem;
  }
  GetMenuBarAttribute(menuBar, VariantsMenu_ID, ATTR_FIRST_ITEM_ID, & iFirstItem);
  SetMenuBarAttribute(menuBar, iFirstItem, ATTR_CHECKED, 0);
  iNextItem = 1;
  while (iNextItem)
  {
    GetMenuBarAttribute(menuBar, iFirstItem, ATTR_NEXT_ITEM_ID, &iNextItem);
    if (iNextItem)
    {
      SetMenuBarAttribute(menuBar, iNextItem, ATTR_CHECKED, 0);
    }
    iFirstItem = iNextItem;
  }
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, 1);
  SetCtrlAttribute(panel, OP_SINGLE_TIMERFORVARIANT, ATTR_ENABLED, FALSE);
  SetCtrlAttribute(panel, OP_SINGLE_VARIANT_RING, ATTR_LABEL_BGCOLOR, VAL_TRANSPARENT);
}

/* FUNCTION *****************************************************************/
/**
ChangeVariantDebug
*
@brief Debugpanel || Set the correspondig teststand var if the operator changes the variant
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK ChangeVariantDebug(int menuBar, int menuItem, void* callbackData, int panel)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  HRESULT hResult;
  char cName[1024];
  char cBuffer[1024];
  int iFirstItem, iNextItem, VariantsMenu_ID;
  threadData = (ThreadData*)callbackData;
  GetMenuBarAttribute(menuBar, menuItem, ATTR_ITEM_NAME, cName);
  hResult = TS_PropertySetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.SelectedVariant", TS_PropOption_InsertIfMissing, cName);
  GetMenuBarAttribute(menuBar, 0, ATTR_FIRST_MENU_ID, &iFirstItem);
  GetMenuBarAttribute(menuBar, iFirstItem, ATTR_MENU_NAME, cName);
  iNextItem = 1;
  VariantsMenu_ID = 1;
  while (iNextItem)
  {
    GetMenuBarAttribute(menuBar, iFirstItem, ATTR_NEXT_MENU_ID, &iNextItem);
    if (iNextItem)
    {
      GetMenuBarAttribute(menuBar, iNextItem, ATTR_MENU_NAME, cName);
    }
    if (!strcmp(cName, "Variants"))
    {
      VariantsMenu_ID = iNextItem;
      iNextItem = 0;
    }
    iFirstItem = iNextItem;
  }
  GetMenuBarAttribute(menuBar, VariantsMenu_ID, ATTR_FIRST_ITEM_ID, & iFirstItem);
  SetMenuBarAttribute(menuBar, iFirstItem, ATTR_CHECKED, 0);
  iNextItem = 1;
  while (iNextItem)
  {
    GetMenuBarAttribute(menuBar, iFirstItem, ATTR_NEXT_ITEM_ID, &iNextItem);
    if (iNextItem)
    {
      SetMenuBarAttribute(menuBar, iNextItem, ATTR_CHECKED, 0);
    }
    iFirstItem = iNextItem;
  }
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, 1);
}

/* FUNCTION *****************************************************************/
/**
Tab
*
@brief Debugpanel || If tab changed, make some solves (time, histo etc)
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK Tab(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  int iTabIndex;
  int iTabHandle;
  int iMinIndex, iMaxIndex;
  double dMax, dMin;
  int iNumberOfModes;
  double dModes[2048];
  int iHistogramArray[1024];
  double dAxisArray[1024];
  int iScaleMode;
  switch (event)
  {
    case EVENT_ACTIVE_TAB_CHANGE:
      GetActiveTabPage(panel, control, &iTabIndex);
      GetPanelHandleFromTabPage(panel, DEBUG_TAB, iTabIndex, &iTabHandle);
      switch (iTabIndex)
      {
      case 0:
        break;
      case 1:
        //Histogramm
        DeleteGraphPlot(iTabHandle, HISTOGRAM_HISTOGRAMM,  - 1, VAL_IMMEDIATE_DRAW);
        if (giActualIndex > 5)
        {
          //giHistrogramInterval
          MaxMin1D(gdMeasurements, giActualIndex, &dMax, &iMaxIndex, &dMin, & iMinIndex);
          //ModeEx (gdMeasurements, giActualIndex, giHistogramInterval, dModes, &iNumberOfModes);
          Histogram(gdMeasurements, giActualIndex, dMin, dMax, iHistogramArray, dAxisArray, giHistogramInterval);
          PlotXY(iTabHandle, HISTOGRAM_HISTOGRAMM, dAxisArray, iHistogramArray, giHistogramInterval, VAL_DOUBLE, VAL_INTEGER, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
          GetAxisScalingMode(iTabHandle, HISTOGRAM_HISTOGRAMM, VAL_LEFT_YAXIS, &iScaleMode, &dMin, &dMax);
          PlotLine(iTabHandle, HISTOGRAM_HISTOGRAMM, gdLowerLimit, dMin, gdLowerLimit, dMax, VAL_GREEN);
          if (giUpperLimit)
          {
            PlotLine(iTabHandle, HISTOGRAM_HISTOGRAMM, gdUpperLimit, dMin, gdUpperLimit, dMax, VAL_GREEN);
          }
        }
        break;
      case 2:
        //Graph
        DeleteGraphPlot(iTabHandle, GRAPH_GRAPH,  - 1, VAL_IMMEDIATE_DRAW);
        if (giActualIndex > 5)
        {
          PlotWaveform(iTabHandle, GRAPH_GRAPH, gdMeasurements, giActualIndex, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_SIMPLE_DOT, VAL_SOLID, 1, VAL_RED);
          GetAxisScalingMode(iTabHandle, HISTOGRAM_HISTOGRAMM, VAL_BOTTOM_XAXIS, &iScaleMode, &dMin, &dMax);
          PlotLine(iTabHandle, HISTOGRAM_HISTOGRAMM, dMin, gdLowerLimit, dMax, gdLowerLimit, VAL_GREEN);
          if (giUpperLimit)
          {
            PlotLine(iTabHandle, HISTOGRAM_HISTOGRAMM, dMin, gdUpperLimit, dMax, gdUpperLimit, VAL_GREEN);
          }
        }
        break;
      }
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
AlwaysOnTop
*
@brief Debugpanel || Enables/Disables the always on top mode of debug panel
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK AlwaysOnTop(int menuBar, int menuItem, void* callbackData, int panel)
{
  int iChecked;
  GetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, &iChecked);
  if (!iChecked)
  {
    SetPanelAttribute(panel, ATTR_FLOATING, VAL_FLOAT_ALWAYS);
  }
  else
  {
    SetPanelAttribute(panel, ATTR_FLOATING, VAL_FLOAT_NEVER);
  }
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, !iChecked);
}

/* FUNCTION *****************************************************************/
/**
ShowCpkValues
*
@brief Debugpanel || Enables/Disables the cpk values output in dbeug panel (run loop on sele....)
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK ShowCpkValues(int menuBar, int menuItem, void* callbackData, int panel)
{
  double dMean = 0.0;
  double dDev = 0.0;
  double dCPKLL = 0.0;
  double dCPKUL = 0.0;
  double dCP = 0.0;
  char cTraceBuffer[1024];
  GetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, &ShowCPKValues_Enabled);
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, !ShowCPKValues_Enabled);
  ShowCPKValues_Enabled = !ShowCPKValues_Enabled;
  if (ShowCPKValues_Enabled && (giActualIndex > 5))
  {
    StdDev(gdMeasurements, giActualIndex - 1, &dMean, &dDev);
    if (lLastCompType > 6)
    {
      dCP = (gdUpperLimit - gdLowerLimit) / (6* dDev);
    }
    dCPKLL = (dMean - gdLowerLimit) / (3* dDev);
    if (lLastCompType > 6)
    {
      dCPKUL = (gdUpperLimit - dMean) / (3* dDev);
    }
    if (lLastCompType > 6)
    {
      sprintf(cTraceBuffer, "cp: %.2f - cpk LL: %.2f - cpk UL: %.2f", dCP, dCPKLL, dCPKUL);
    }
    else
    {
      sprintf(cTraceBuffer, "cp: %.2f - cpk LL: %.2f", dCP, dCPKLL);
    }
    SetCtrlVal(panel, DEBUG_CPK, cTraceBuffer);
  }
  if (!ShowCPKValues_Enabled)
  {
    SetCtrlVal(panel, DEBUG_CPK, cLastNameOfStep);
  }
}

/* FUNCTION *****************************************************************/
/**
ChangeHistogramInterval
*
@brief Debugpanel || Sets the histogram interval in histo view
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK ChangeHistogramInterval(int menuBar, int menuItem, void* callbackData, int panel)
{
  SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_HISTO_I10, ATTR_CHECKED, FALSE);
  SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_HISTO_I20, ATTR_CHECKED, FALSE);
  SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_HISTO_I30, ATTR_CHECKED, FALSE);
  SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_HISTO_I40, ATTR_CHECKED, FALSE);
  SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_HISTO_I50, ATTR_CHECKED, FALSE);
  SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, TRUE);
  switch (menuItem)
  {
    case MENUBAR_SETTINGS_HISTO_I10:
      giHistogramInterval = 10;
      break;
    case MENUBAR_SETTINGS_HISTO_I20:
      giHistogramInterval = 20;
      break;
    case MENUBAR_SETTINGS_HISTO_I30:
      giHistogramInterval = 30;
      break;
    case MENUBAR_SETTINGS_HISTO_I40:
      giHistogramInterval = 40;
      break;
    case MENUBAR_SETTINGS_HISTO_I50:
      giHistogramInterval = 50;
      break;
    default:
      giHistogramInterval = 30;
      break;
  }
}

/* FUNCTION *****************************************************************/
/**
ClearDataMenu
*
@brief Debugpanel || Delete all measurements in debug panel
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK ClearDataMenu(int menuBar, int menuItem, void* callbackData, int panel)
{
  int iTabHandle;
  //Analog
  GetPanelHandleFromTabPage(panel, DEBUG_TAB, 0, &iTabHandle);
  DeleteGraphPlot(iTabHandle, ANALOG_ANALOG,  - 1, VAL_IMMEDIATE_DRAW);
  //Histogramm
  GetPanelHandleFromTabPage(panel, DEBUG_TAB, 1, &iTabHandle);
  DeleteGraphPlot(iTabHandle, HISTOGRAM_HISTOGRAMM,  - 1, VAL_IMMEDIATE_DRAW);
  //Graph
  GetPanelHandleFromTabPage(panel, DEBUG_TAB, 2, &iTabHandle);
  DeleteGraphPlot(iTabHandle, GRAPH_GRAPH,  - 1, VAL_IMMEDIATE_DRAW);
  ResetMeasurements();
}

/* FUNCTION *****************************************************************/
/**
Measurement
*
@brief TBD
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK Measurement(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  switch (event)
  {
    case EVENT_VAL_CHANGED:
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
ResetStatistics
*
@brief Operatorpanel || Delete all measurements in operaoter panel  // resets the gui
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK ResetStatistics(int menuBar, int menuItem, void* callbackData, int panel)
{
  char cTraceBuffer[1024];
  int iConfirm, idx;
  int iTabHandle;
  iConfirm = ConfirmPopup("CP-ITS", "Are you sure you want to delete all the statistic data?");
  if (iConfirm)
  {
    giPassUUTs = 0;
    giFailUUTs = 0;
    giConsecutiveFailed = 0;
    giCountOfUUTs = 0;
    gdQRate = 0;
    sprintf(cTraceBuffer, "Count: %i, Pass: %i, Fail: %i, Q: %.2f %%", giCountOfUUTs, giPassUUTs, giFailUUTs, gdQRate);
    SetCtrlVal(panel, OP_SINGLE_ACTUALTESTSTEP, cTraceBuffer);
    SetActiveTabPage(panel, OP_SINGLE_TAB, 0);
    GetPanelHandleFromTabPage(panel, OP_SINGLE_TAB, 0, &iTabHandle);
    DeleteListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY, 0,  - 1);
    DeleteListItem(iTabHandle, TABPANEL_TESTERRORLOG, 0,  - 1);
    giActualItemIndex = 0;
    SetCtrlAttribute(iTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_BGCOLOR, 0xFAFAFA);
    GetPanelHandleFromTabPage(panel, OP_SINGLE_TAB, 1, &iTabHandle);
    DeleteTableRows(iTabHandle, TABPANEL_2_TABLE, 1,  - 1);
    DeleteTableColumns(iTabHandle, TABPANEL_2_TABLE, 1,  - 1);
    for (idx = 0; idx < 50; idx++)
    {
      giAllTestResults[idx]->cTestResult =  - 1;
      sprintf(giAllTestResults[idx]->cSerial, "");
      sprintf(giAllTestResults[idx]->cStartTime, "");
      sprintf(giAllTestResults[idx]->cStartDate, "");
      sprintf(giAllTestResults[idx]->cStopTime, "");
      sprintf(giAllTestResults[idx]->cStopDate, "");
      giAllTestResults[idx]->dExecutionTime = 0.0;
      sprintf(giAllTestResults[idx]->cVariant, "");
      if (giAllTestResults[idx]->pFirst)
      {
        vFreeAllTestResults(giAllTestResults[idx]->pFirst);
      }
      giAllTestResults[idx]->pFirst = NULL;
    }
    giActualTestResult = 0;
    SetCtrlVal(panel, OP_SINGLE_TESTSTATUS_MSG, "Waiting...");
    SetCtrlAttribute(panel, OP_SINGLE_TESTSTATUS_MSG, ATTR_TEXT_BGCOLOR, VAL_LT_GRAY);
  }
}

/* FUNCTION *****************************************************************/
/**
OpSingleCallback
*
@brief Operatorpanel || Operatorpanel callback, quit programm, resize event
*
@param panel: Panel id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK OpSingleCallback(int panel, int event, void* callbackData, int eventData1, int eventData2)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  threadData = (ThreadData*)callbackData;
  switch (event)
  {
    case EVENT_GOT_FOCUS:
      break;
    case EVENT_PANEL_SIZE:
      vOrderGUIElementsAfterResize(panel);
      break;
    case EVENT_CLOSE:
      TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.QuitProgram", TS_PropOption_InsertIfMissing, VTRUE);
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
About
*
@brief Operatorpanel || Shows som einfos about the control panel
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK About(int menuBar, int menuItem, void* callbackData, int panel)
{
  char cTextMessage[1024];
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  threadData = (ThreadData*)callbackData;
  sprintf(cTextMessage, "CP-ITS ControlPanel - Version %s ", LIB_VERSION);
  SetCtrlVal(gSplash, SPLASH_SELECTEDSEQ, cTextMessage);
  SetCtrlAttribute(gSplash, SPLASH_SELECTEDSEQ, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
  SetCtrlAttribute(gSplash, SPLASH_LABEL, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
  SetCtrlAttribute(gSplash, SPLASH_PRODUCT, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
  DisplayPanel(gSplash);
}

/* FUNCTION *****************************************************************/
/**
ChangeVariantRing
*
@brief Operatorpanel || Changes the variant if the operator make a new selection of ring comp
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK ChangeVariantRing(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  HRESULT hResult;
  char cName[1024];
  char cBuffer[1024];
  char cFirstListValue[1024];
  char cMenuItemName[1024];
  int iFirstItem, iNextItem, VariantsMenu_ID;
  int menuBar;
  threadData = (ThreadData*)callbackData;
  switch (event)
  {
    case EVENT_VAL_CHANGED:
      //Get Ctrl val
      GetCtrlVal(panel, OP_SINGLE_VARIANT_RING, cName);
      //Set global var
      sprintf(gcSelectedVariant, cName);
      //Set teststand variable
      hResult = TS_PropertySetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.SelectedVariant", TS_PropOption_InsertIfMissing, cName);
      //Delete "please select a variant"
      GetValueFromIndex(panel, OP_SINGLE_VARIANT_RING, 0, cFirstListValue);
      if (!strcmp(cFirstListValue, "No variant selected"))
      {
        DeleteListItem(panel, OP_SINGLE_VARIANT_RING, 0, 1);
      }
      if (!giRunButtonDisabled)
      {
        iVariantIsSet = 1;
      }
      menuBar = GetPanelMenuBar(panel);
      GetMenuBarAttribute(menuBar, 0, ATTR_FIRST_MENU_ID, &iFirstItem);
      GetMenuBarAttribute(menuBar, iFirstItem, ATTR_MENU_NAME, cName);
      iNextItem = 1;
      VariantsMenu_ID = 1;
      while (iNextItem)
      {
        GetMenuBarAttribute(menuBar, iFirstItem, ATTR_NEXT_MENU_ID, &iNextItem);
        if (iNextItem)
        {
          GetMenuBarAttribute(menuBar, iNextItem, ATTR_MENU_NAME, cName);
        }
        if (!strcmp(cName, "Variants"))
        {
          VariantsMenu_ID = iNextItem;
          iNextItem = 0;
        }
        iFirstItem = iNextItem;
      }
      GetMenuBarAttribute(menuBar, VariantsMenu_ID, ATTR_FIRST_ITEM_ID, & iFirstItem);
      GetMenuBarAttribute(menuBar, iFirstItem, ATTR_ITEM_NAME, cMenuItemName);
      if (strcmp(gcSelectedVariant, cMenuItemName))
      {
        SetMenuBarAttribute(menuBar, iFirstItem, ATTR_CHECKED, 0);
      }
      else
      {
        SetMenuBarAttribute(menuBar, iFirstItem, ATTR_CHECKED, 1);
      }
      iNextItem = 1;
      while (iNextItem)
      {
        GetMenuBarAttribute(menuBar, iFirstItem, ATTR_NEXT_ITEM_ID, &iNextItem);
        if (iNextItem)
        {
          GetMenuBarAttribute(menuBar, iNextItem, ATTR_ITEM_NAME, cMenuItemName);
          if (strcmp(gcSelectedVariant, cMenuItemName))
          {
            SetMenuBarAttribute(menuBar, iNextItem, ATTR_CHECKED, 0);
          }
          else
          {
            SetMenuBarAttribute(menuBar, iNextItem, ATTR_CHECKED, 1);
          }
        }
        iFirstItem = iNextItem;
      }
      SetCtrlAttribute(panel, OP_SINGLE_TIMERFORVARIANT, ATTR_ENABLED, FALSE);
      SetCtrlAttribute(panel, OP_SINGLE_VARIANT_RING, ATTR_LABEL_BGCOLOR, VAL_TRANSPARENT);
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
TabCallBack
*
@brief Operatorpanel || If tab changed the last 50 test are printed in gui
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK TabCallBack(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  int iActiveTab;
  int iActualTabHandle;
  int idx, idy;
  int iTestStepIndex;
  int iTestStepCount, iMaxTestStepCount;
  int iTempTestStepIndex;
  int iActualInsertionIndex;
  int iTempInsertionIndex;
  int iRowInsertionIndex;
  int iNumberOfColumns;
  int iNumberOfRows;
  int iTextLength;
  int iMaxTextLength;
  int iPassColorValue, iFailColorValue;
  int iInsertRowsComplete = 0;
  char cTextBuffer[1024];
  char cRowLabelText[1024];
  TestStep* pNextPointer;
  TestStep* pNextSearchPointer;
  int iTestStepFound;
  //For cpk solving ...
  double dMeasurementsArray[50];
  double dMeasurementValue;
  int iMeasValueCount;
  double dMean = 0.0;
  double dDev = 0.0;
  double dCPKLL = 0.0;
  double dCPKUL = 0.0;
  double dCP = 0.0;
  double dLowerLimit = 0.0;
  double dUpperLimit = 0.0;
  float fCpkLimit;
  FILE* pFile;
  int iDebugMode = 0;
  switch (event)
  {
    case EVENT_ACTIVE_TAB_CHANGE:
      GetActiveTabPage(panel, control, &iActiveTab);
      if (iActiveTab == 0){
      }
      else if (iActiveTab == 1)
      {
        GetPanelHandleFromTabPage(panel, OP_SINGLE_TAB, 1, &iActualTabHandle);
        GetCtrlVal(panel, OP_SINGLE_COL_PASS, &iPassColorValue);
        GetCtrlVal(panel, OP_SINGLE_COL_FAIL, &iFailColorValue);
        //No values ....
        if (!giCountOfUUTs)
        {
          //Delete old table
          DeleteTableRows(iActualTabHandle, TABPANEL_2_TABLE, 1,  - 1);
          DeleteTableColumns(iActualTabHandle, TABPANEL_2_TABLE, 1,  - 1);
          InsertTableColumns(iActualTabHandle, TABPANEL_2_TABLE,  - 1, 1, VAL_USE_MASTER_CELL_TYPE);
          SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 1, ATTR_COLUMN_WIDTH, 69* 6);
          InsertTableRows(iActualTabHandle, TABPANEL_2_TABLE,  - 1, 1, VAL_USE_MASTER_CELL_TYPE);
          SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(1, 1), "You have to run the test at least on time. No values are available...");
          return 0;
        }
        if (iDebugMode)
        {
          pFile = fopen("C:\\Temp\\CP_ITS_Data.txt", "w");
          if (pFile)
          {
            iActualInsertionIndex = giActualTestResult;
            for (idx = 0; idx < 50; idx++)
            {
              if (iActualInsertionIndex < 0)
              {
                iActualInsertionIndex = 49;
              }
              if (giAllTestResults[iActualInsertionIndex]->cTestResult >= 0)
              {
                fprintf(pFile, "idx: %d\n", idx);
                fprintf(pFile, "iActualInsertionIndex: %d\n\n", iActualInsertionIndex);
                fprintf(pFile, "cTestResult: %d\n", giAllTestResults[iActualInsertionIndex]->cTestResult);
                fprintf(pFile, "cSerial: %s\n", giAllTestResults[iActualInsertionIndex]->cSerial);
                fprintf(pFile, "cStartTime: %s\n", giAllTestResults[iActualInsertionIndex]->cStartTime);
                fprintf(pFile, "cStartDate: %s\n", giAllTestResults[iActualInsertionIndex]->cStartDate);
                fprintf(pFile, "cStopTime: %s\n", giAllTestResults[iActualInsertionIndex]->cStopTime);
                fprintf(pFile, "cStopDate: %s\n", giAllTestResults[iActualInsertionIndex]->cStopDate);
                fprintf(pFile, "dExecutionTime: %.2f\n", giAllTestResults[iActualInsertionIndex]->dExecutionTime);
                fprintf(pFile, "cVariant: %s\n\n", giAllTestResults[iActualInsertionIndex]->cVariant);
                idy = 1;
                pNextPointer = giAllTestResults[iActualInsertionIndex]->pFirst;
                while (pNextPointer)
                {
                  fprintf(pFile, "##################################### Test Step %d #############################################\n", idy);
                  fprintf(pFile, "cTestCaseType: %d\n", pNextPointer->cTestCaseType);
                  fprintf(pFile, "cStepName: %s\n", pNextPointer->cStepName);
                  fprintf(pFile, "cComment: %s\n", pNextPointer->cComment);
                  fprintf(pFile, "cLogMessage: %s\n", pNextPointer->cLogMessage);
                  fprintf(pFile, "dMeasurement: %f\n", pNextPointer->dMeasurement);
                  fprintf(pFile, "dLowerLimit: %f\n", pNextPointer->dLowerLimit);
                  fprintf(pFile, "dUpperLimit: %f\n", pNextPointer->dUpperLimit);
                  fprintf(pFile, "cUnit: %s\n", pNextPointer->cUnit);
                  fprintf(pFile, "cCompType: %s\n", pNextPointer->cCompType);
                  fprintf(pFile, "cFormat: \n");
                  fprintf(pFile, "iResult: %d\n", pNextPointer->iResult);
                  fprintf(pFile, "cStringToCompare: %s\n", pNextPointer->cStringToCompare);
                  fprintf(pFile, "cExpectedString: %s\n", pNextPointer->cExpectedString);
                  idy++;
                  pNextPointer = pNextPointer->pNext;
                }
                fprintf(pFile, "\n\n");
                fprintf(pFile, "##################################################################################################");
                fprintf(pFile, "\n\n");
              }
              iActualInsertionIndex--;
            }
            fclose(pFile);
          }
        }
        SetCtrlAttribute(iActualTabHandle, TABPANEL_2_TABLE, ATTR_VISIBLE, 0);
        //ProcessDrawEvents();
        //Delete old table
        DeleteTableRows(iActualTabHandle, TABPANEL_2_TABLE, 1,  - 1);
        DeleteTableColumns(iActualTabHandle, TABPANEL_2_TABLE, 1,  - 1);
        //LowerLimit / ExpectedString
        InsertTableColumns(iActualTabHandle, TABPANEL_2_TABLE,  - 1, 1, VAL_USE_MASTER_CELL_TYPE);
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 1, ATTR_LABEL_TEXT, "LowerLimit");
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 1, ATTR_USE_LABEL_TEXT, 1);
        //UpperLimit
        InsertTableColumns(iActualTabHandle, TABPANEL_2_TABLE,  - 1, 1, VAL_USE_MASTER_CELL_TYPE);
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 2, ATTR_LABEL_TEXT, "UpperLimit");
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 2, ATTR_USE_LABEL_TEXT, 1);
        //Unit
        InsertTableColumns(iActualTabHandle, TABPANEL_2_TABLE,  - 1, 1, VAL_USE_MASTER_CELL_TYPE);
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 3, ATTR_LABEL_TEXT, "Unit");
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 3, ATTR_USE_LABEL_TEXT, 1);
        //CompType
        InsertTableColumns(iActualTabHandle, TABPANEL_2_TABLE,  - 1, 1, VAL_USE_MASTER_CELL_TYPE);
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 4, ATTR_LABEL_TEXT, "ComparisionType");
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 4, ATTR_USE_LABEL_TEXT, 1);
        //Comment
        InsertTableColumns(iActualTabHandle, TABPANEL_2_TABLE,  - 1, 1, VAL_USE_MASTER_CELL_TYPE);
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 5, ATTR_LABEL_TEXT, "Comment");
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, 5, ATTR_USE_LABEL_TEXT, 1);
        //Insert Columns
        if (giTestIsRunning)
        {
          iActualInsertionIndex = giActualTestResult - 1;
        }
        else
        {
          iActualInsertionIndex = giActualTestResult;
        }
        for (idx = 0; idx < 50; idx++)
        {
          if (iActualInsertionIndex < 0)
          {
            iActualInsertionIndex = 49;
          }
          if (giAllTestResults[iActualInsertionIndex]->cTestResult >= 0)
          {
            InsertTableColumns(iActualTabHandle, TABPANEL_2_TABLE, idx + 1, 1, VAL_USE_MASTER_CELL_TYPE);
            SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, idx + 1, ATTR_USE_LABEL_TEXT, 1);
            if (giAllTestResults[iActualInsertionIndex]->cSerial)
            {
              if (strlen(giAllTestResults[iActualInsertionIndex]->cSerial))
              {
                SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, idx + 1, ATTR_LABEL_TEXT, giAllTestResults[iActualInsertionIndex]->cSerial);
              }
              else
              {
                sprintf(cTextBuffer, "%d", iActualInsertionIndex);
                SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, idx + 1, ATTR_LABEL_TEXT, giAllTestResults[iActualInsertionIndex]->cSerial);
              }
            }
            else
            {
              sprintf(cTextBuffer, "%d", iActualInsertionIndex);
              SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, idx + 1, ATTR_LABEL_TEXT, giAllTestResults[iActualInsertionIndex]->cSerial);
            }
          }
          iActualInsertionIndex--;
        }
        //Insert Rows
        iTestStepIndex = 1;
        GetNumTableColumns(iActualTabHandle, TABPANEL_2_TABLE, &iNumberOfColumns);
        //Result Row
        InsertTableRows(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, 1, VAL_USE_MASTER_CELL_TYPE);
        SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_USE_LABEL_TEXT, 1);
        SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_LABEL_TEXT, "Result: ");
        iTestStepIndex++;
        //Variant Row
        if (strlen(gcSelectedVariant))
        {
          InsertTableRows(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, 1, VAL_USE_MASTER_CELL_TYPE);
          SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_USE_LABEL_TEXT, 1);
          SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_LABEL_TEXT, "Variant: ");
          iTestStepIndex++;
        }
        //Start time stamp row
        InsertTableRows(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, 1, VAL_USE_MASTER_CELL_TYPE);
        SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_USE_LABEL_TEXT, 1);
        SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_LABEL_TEXT, "Start time stamp: ");
        iTestStepIndex++;
        //Execution time row
        InsertTableRows(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, 1, VAL_USE_MASTER_CELL_TYPE);
        SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_USE_LABEL_TEXT, 1);
        SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_LABEL_TEXT, "Execution time: ");
        iTestStepIndex++;
        //Search for max teststep entries
        if (giTestIsRunning)
        {
          iTempInsertionIndex = giActualTestResult - 1;
          iRowInsertionIndex = giActualTestResult - 1;
        }
        else
        {
          iTempInsertionIndex = giActualTestResult;
          iRowInsertionIndex = giActualTestResult;
        }
        iMaxTestStepCount = 0;
        for (idx = 0; idx < 50; idx++)
        {
          if (iTempInsertionIndex < 0)
          {
            iTempInsertionIndex = 49;
          }
          if (giAllTestResults[iTempInsertionIndex]->cTestResult >= 0)
          {
            iTestStepCount = 0;
            pNextPointer = giAllTestResults[iTempInsertionIndex]->pFirst;
            //Count list length
            while (pNextPointer)
            {
              pNextPointer = pNextPointer->pNext;
              iTestStepCount++;
            }
            if (iTestStepCount > iMaxTestStepCount)
            {
              iMaxTestStepCount = iTestStepCount;
              iRowInsertionIndex = iTempInsertionIndex;
            }
          }
          //Next Test Run
          iTempInsertionIndex--;
        }
        //Insert Unit of exec time
        SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 2, 4), "sec");
        SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 2, 4), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
        //Insert row with max test step indices
        pNextPointer = giAllTestResults[iRowInsertionIndex]->pFirst;
        while (pNextPointer)
        {
          InsertTableRows(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, 1, VAL_USE_MASTER_CELL_TYPE);
          SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_USE_LABEL_TEXT, 1);
          sprintf(cTextBuffer, "%s: ", pNextPointer->cStepName);
          SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_LABEL_TEXT, cTextBuffer);
          //If numeric
          if (pNextPointer->cTestCaseType == 0)
          {
            if (!strcmp(pNextPointer->cCompType, "EQ") || !strcmp(pNextPointer->cCompType, "NE") || !strcmp(pNextPointer->cCompType, "GT") || !strcmp(pNextPointer->cCompType, "GE") || !strcmp(pNextPointer->cCompType, "GTLT") || !strcmp(pNextPointer->cCompType, "GELE") || !strcmp(pNextPointer->cCompType, "GELT") || !strcmp(pNextPointer->cCompType, "GTLE") || !strcmp(pNextPointer->cCompType, "LTGT") || !strcmp(pNextPointer->cCompType, "LEGE") || !strcmp(pNextPointer->cCompType, "LEGT") || !strcmp(pNextPointer->cCompType, "LTGE"))
            {
              FormatValues(cTextBuffer, pNextPointer->cFormat, pNextPointer->dLowerLimit);
              SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 4, iTestStepIndex), cTextBuffer);
            }
            if (!strcmp(pNextPointer->cCompType, "LT") || !strcmp(pNextPointer->cCompType, "LE") || !strcmp(pNextPointer->cCompType, "GTLT") || !strcmp(pNextPointer->cCompType, "GELE") || !strcmp(pNextPointer->cCompType, "GELT") || !strcmp(pNextPointer->cCompType, "GTLE") || !strcmp(pNextPointer->cCompType, "LTGT") || !strcmp(pNextPointer->cCompType, "LEGE") || !strcmp(pNextPointer->cCompType, "LEGT") || !strcmp(pNextPointer->cCompType, "LTGE"))
            {
              FormatValues(cTextBuffer, pNextPointer->cFormat, pNextPointer->dUpperLimit);
              SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 3, iTestStepIndex), cTextBuffer);
            }
            SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 2, iTestStepIndex), pNextPointer->cUnit);
            sprintf(cTextBuffer, "Numeric [%s]", pNextPointer->cCompType);
            SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 1, iTestStepIndex), cTextBuffer);
            SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, iTestStepIndex), pNextPointer->cComment);
          }
          //If Pass/Fail
          if (pNextPointer->cTestCaseType == 1)
          {
            sprintf(cTextBuffer, "Pass/Fail");
            SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 1, iTestStepIndex), cTextBuffer);
          }
          //If String
          if (pNextPointer->cTestCaseType == 2)
          {
            SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 4, iTestStepIndex), pNextPointer->cExpectedString);
            sprintf(cTextBuffer, "String [%s]", pNextPointer->cCompType);
            SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 1, iTestStepIndex), cTextBuffer);
            SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, iTestStepIndex), pNextPointer->cComment);
          }
          SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 4, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
          SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 3, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
          SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 2, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
          SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 1, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
          SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
          iTestStepIndex++;
          pNextPointer = pNextPointer->pNext;
        }
        //Insert Values
        if (giTestIsRunning)
        {
          iActualInsertionIndex = giActualTestResult - 1;
        }
        else
        {
          iActualInsertionIndex = giActualTestResult;
        }
        for (idx = 0; idx < 50; idx++)
        {
          if (iActualInsertionIndex < 0)
          {
            iActualInsertionIndex = 49;
          }
          if (giAllTestResults[iActualInsertionIndex]->cTestResult >= 0)
          {
            iTestStepIndex = 1;
            SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
            //First line Test Result 
            if (giAllTestResults[iActualInsertionIndex]->cTestResult == 0)
            {
              SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), "PASS");
              SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), ATTR_TEXT_BGCOLOR, iPassColorValue);
              iTestStepIndex++;
            }
            else
            {
              SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), "FAIL");
              SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), ATTR_TEXT_BGCOLOR, iFailColorValue);
              iTestStepIndex++;
            }
            //If used -> Variant
            if (strlen(giAllTestResults[iActualInsertionIndex]->cVariant))
            {
              SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), giAllTestResults[iActualInsertionIndex]->cVariant);
              SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
              iTestStepIndex++;
            }
            //Date Time Stamp
            sprintf(cTextBuffer, "%s", giAllTestResults[iActualInsertionIndex]->cStartDate);
            SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), cTextBuffer);
            SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
            iTestStepIndex++;
            //Execution time
            sprintf(cTextBuffer, "%.2f", giAllTestResults[iActualInsertionIndex]->dExecutionTime);
            SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), cTextBuffer);
            SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
            iTestStepIndex++;
            //Result of each Step
            pNextPointer = giAllTestResults[iActualInsertionIndex]->pFirst;
            while (pNextPointer)
            {
              //Compare Test Step Name
              GetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_LABEL_TEXT, cRowLabelText);
              if (strlen(cRowLabelText))
              {
                //Cut ": " from label text
                cRowLabelText[strlen(cRowLabelText) - 2] = '\0';
              }
              if (strcmp(pNextPointer->cStepName, cRowLabelText))
              {
                iTestStepFound = 0;
                iTempTestStepIndex = iTestStepIndex + 1;
                GetNumTableRows(iActualTabHandle, TABPANEL_2_TABLE, &iNumberOfRows);
                while ((iTempTestStepIndex <= iNumberOfRows) && !iTestStepFound)
                {
                  //next row
                  GetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTempTestStepIndex, ATTR_LABEL_TEXT, cRowLabelText);
                  if (strlen(cRowLabelText))
                  {
                    //Cut ": " from label text
                    cRowLabelText[strlen(cRowLabelText) - 2] = '\0';
                  }
                  if (!strcmp(pNextPointer->cStepName, cRowLabelText))
                  {
                    //Test step found
                    iTestStepFound++;
                  }
                  else
                  {
                    iTempTestStepIndex++;
                  }
                }
                //If found set the new index
                if (iTestStepFound)
                {
                  iTestStepIndex = iTempTestStepIndex;
                  //If not found insert new row
                }
                else
                {
                  InsertTableRows(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, 1, VAL_USE_MASTER_CELL_TYPE);
                  SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_USE_LABEL_TEXT, 1);
                  sprintf(cTextBuffer, "%s: ", pNextPointer->cStepName);
                  SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, iTestStepIndex, ATTR_LABEL_TEXT, cTextBuffer);
                  //If numeric
                  if (pNextPointer->cTestCaseType == 0)
                  {
                    FormatValues(cTextBuffer, pNextPointer->cFormat, pNextPointer->dLowerLimit);
                    SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 4, iTestStepIndex), cTextBuffer);
                    FormatValues(cTextBuffer, pNextPointer->cFormat, pNextPointer->dUpperLimit);
                    SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 3, iTestStepIndex), cTextBuffer);
                    SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 2, iTestStepIndex), pNextPointer->cUnit);
                    sprintf(cTextBuffer, "Numeric [%s]", pNextPointer->cCompType);
                    SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 1, iTestStepIndex), cTextBuffer);
                    SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, iTestStepIndex), pNextPointer->cComment);
                  }
                  //If Pass/Fail
                  if (pNextPointer->cTestCaseType == 1)
                  {
                    sprintf(cTextBuffer, "Pass/Fail");
                    SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 1, iTestStepIndex), cTextBuffer);
                  }
                  //If String
                  if (pNextPointer->cTestCaseType == 2)
                  {
                    SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 4, iTestStepIndex), pNextPointer->cExpectedString);
                    sprintf(cTextBuffer, "String [%s]", pNextPointer->cCompType);
                    SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 1, iTestStepIndex), cTextBuffer);
                    SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, iTestStepIndex), pNextPointer->cComment);
                  }
                  SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 4, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
                  SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 3, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
                  SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 2, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
                  SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 1, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
                  SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
                }
              }
              //If numeric
              if (pNextPointer->cTestCaseType == 0)
              {
                FormatValues(cTextBuffer, pNextPointer->cFormat, pNextPointer->dMeasurement);
                SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), cTextBuffer);
              }
              //If Pass/Fail
              if (pNextPointer->cTestCaseType == 1)
              {
                if (pNextPointer->iResult == 1)
                {
                  SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), "PASS");
                }
                else
                {
                  SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), "FAIL");
                }
              }
              //If string
              if (pNextPointer->cTestCaseType == 2)
              {
                SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), pNextPointer->cStringToCompare);
              }
              if (pNextPointer->iResult == 1)
              {
                SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), ATTR_TEXT_BGCOLOR, iPassColorValue);
              }
              else
              {
                SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), ATTR_TEXT_BGCOLOR, iFailColorValue);
              }
              SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx + 1, iTestStepIndex), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
              iTestStepIndex++;
              pNextPointer = pNextPointer->pNext;
            }
          }
          //Next Test Run
          iActualInsertionIndex--;
        }
        GetCtrlVal(iActualTabHandle, TABPANEL_2_BUFFERCPK, &fCpkLimit);
        //Insert cpk value column
        InsertTableColumns(iActualTabHandle, TABPANEL_2_TABLE,  - 1, 1, VAL_USE_MASTER_CELL_TYPE);
        GetNumTableColumns(iActualTabHandle, TABPANEL_2_TABLE, &iNumberOfColumns);
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, iNumberOfColumns, ATTR_LABEL_TEXT, "Cpk-Value");
        SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, iNumberOfColumns, ATTR_USE_LABEL_TEXT, 1);
        //Solve cpk value
        GetNumTableRows(iActualTabHandle, TABPANEL_2_TABLE, &iNumberOfRows);
        GetNumTableColumns(iActualTabHandle, TABPANEL_2_TABLE, &iNumberOfColumns);
        for (idx = 1; idx <= iNumberOfRows; idx++)
        {
          GetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 2, idx), cTextBuffer);
          if (!strcmp(cTextBuffer, "Numeric [GELE]"))
          {
            iMeasValueCount = 0;
            for (idy = 1; idy < (iNumberOfColumns - 5); idy++)
            {
              //Get measurement value from table
              GetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idy, idx), cTextBuffer);
              if (strlen(cTextBuffer))
              {
                dMeasurementValue = atof(cTextBuffer);
                if (!errno)
                {
                  dMeasurementsArray[iMeasValueCount] = dMeasurementValue;
                  iMeasValueCount++;
                }
              }
            }
            //Get lower limit from table
            GetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 5, idx), cTextBuffer);
            if (strlen(cTextBuffer))
            {
              dLowerLimit = atof(cTextBuffer);
              if (errno)
              {
                dLowerLimit = 1000000.0;
              }
            }
            //Get upper limit from table
            GetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns - 4, idx), cTextBuffer);
            if (strlen(cTextBuffer))
            {
              dUpperLimit = atof(cTextBuffer);
              if (errno)
              {
                dUpperLimit = 1000000.0;
              }
            }
            if (dUpperLimit > dLowerLimit)
            {
              StdDev(dMeasurementsArray, iMeasValueCount, &dMean, &dDev);
              dCPKLL = (dMean - dLowerLimit) / (3* dDev);
              dCPKUL = (dUpperLimit - dMean) / (3* dDev);
              if (dCPKLL > dCPKUL)
              {
                sprintf(cTextBuffer, "%.2f", dCPKLL);
                if (dCPKLL > fCpkLimit)
                {
                  SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, idx), ATTR_TEXT_BGCOLOR, iPassColorValue);
                }
                else
                {
                  SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, idx), ATTR_TEXT_BGCOLOR, iFailColorValue);
                }
              }
              else
              {
                sprintf(cTextBuffer, "%.2f", dCPKUL);
                if (dCPKUL > fCpkLimit)
                {
                  SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, idx), ATTR_TEXT_BGCOLOR, iPassColorValue);
                }
                else
                {
                  SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, idx), ATTR_TEXT_BGCOLOR, iFailColorValue);
                }
              }
              SetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, idx), cTextBuffer);
              SetTableCellAttribute(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(iNumberOfColumns, idx), ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED);
            }
          }
        }
        //Format row label width
        GetNumTableRows(iActualTabHandle, TABPANEL_2_TABLE, &iNumberOfRows);
        iMaxTextLength = 0;
        for (idy = 1; idy <= iNumberOfRows; idy++)
        {
          SetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, idy, ATTR_LABEL_JUSTIFY, VAL_CENTER_RIGHT_JUSTIFIED);
          GetTableRowAttribute(iActualTabHandle, TABPANEL_2_TABLE, idy, ATTR_LABEL_TEXT, cTextBuffer);
          iTextLength = strlen(cTextBuffer);
          if (iTextLength > iMaxTextLength)
          {
            iMaxTextLength = iTextLength;
          }
        }
        SetCtrlAttribute(iActualTabHandle, TABPANEL_2_TABLE, ATTR_ROW_LABELS_WIDTH, iMaxTextLength* 8);
        //Format column width
        GetNumTableColumns(iActualTabHandle, TABPANEL_2_TABLE, &iNumberOfColumns);
        for (idx = 1; idx <= iNumberOfColumns; idx++)
        {
          GetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, idx, ATTR_LABEL_TEXT, cTextBuffer);
          iMaxTextLength = strlen(cTextBuffer);
          //Get all row values	
          for (idy = 1; idy <= iNumberOfRows; idy++)
          {
            GetTableCellVal(iActualTabHandle, TABPANEL_2_TABLE, MakePoint(idx, idy), cTextBuffer);
            iTextLength = strlen(cTextBuffer);
            if (iTextLength > iMaxTextLength)
            {
              iMaxTextLength = iTextLength;
            }
          }
          //Make columns width not wider than 700
          if (iMaxTextLength > 100)
          {
            iMaxTextLength = 100;
          }
          SetTableColumnAttribute(iActualTabHandle, TABPANEL_2_TABLE, idx, ATTR_COLUMN_WIDTH, iMaxTextLength* 7);
        }
        //Make it visible
        SetCtrlAttribute(iActualTabHandle, TABPANEL_2_TABLE, ATTR_VISIBLE, 1);
      }
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
ValueTableCallBack
*
@brief Operatorpanel || Table of last 50 test results
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK ValueTableCallBack(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  Rect pSelection;
  int iCountOfRows;
  int iCountOfColumns;
  int idx, idy;
  int iCountOfMeasurements;
  int iResultColorValue;
  char cResult;
  int iActualColorValue;
  int iSelectionStatus;
  int iStatus;
  char cCompType[50];
  char cTempBuffer[1024];
  double dLowerLimits[50];
  double dUpperLimits[50];
  //double dMeasurements[50];
  //double dLowerLimit;  
  //double dUpperLimit;  
  char cUnit[100];
  char cComment[1024];
  char cTestStepName[1024];
  char cCpk[50];
  char cPathName[1024];
  char cSerial[1024];
  FILE* pFile;
  char cValue[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int iHistogramArray[1024];
  int iHistoResolution;
  double dAxisArray[1024];
  int iScaleMode;
  double dMin, dMax;
  int iMaxIndex, iMinIndex;
  MsaHeader MsaPrintHeader;
  CellValue CellPrintValue;
  int iBackGroundColor;
  threadData = (ThreadData*)callbackData;
  char* cTempBuffer2;
  int iSavePopupPanel;
  switch (event)
  {
    case EVENT_COMMIT:
      break;
    case EVENT_TABLE_ROW_COL_LABEL_CLICK:
      //Spalte pSelection.width == 1
      //Zeile pSelection.height == 1   
      GetTableSelection(panel, TABPANEL_2_TABLE, &pSelection);
      GetNumTableRows(panel, TABPANEL_2_TABLE, &iCountOfRows);
      GetNumTableColumns(panel, TABPANEL_2_TABLE, &iCountOfColumns);
      //If the user selected a row show measurement panel
      if ((pSelection.height == 1) && (pSelection.top > 4) && (iCountOfColumns > 16))
      {
        GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 2, pSelection.top), cCompType);
        //If selected row a numeric limit test continue
        if (cCompType[0] == 'N')
        {
          GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 5, pSelection.top), cLowerLimit);
          gdLowerLimit = atof(cLowerLimit);
          GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 4, pSelection.top), cUpperLimit);
          gdUpperLimit = atof(cUpperLimit);
          for (idx = 1; idx <= (iCountOfColumns - 6); idx++)
          {
            GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(idx, pSelection.top), cTempBuffer);
            gdMeasurements[idx - 1] = atof(cTempBuffer);
            dLowerLimits[idx - 1] = gdLowerLimit;
            dUpperLimits[idx - 1] = gdUpperLimit;
          }
          GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 3, pSelection.top), cUnit);
          GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 1, pSelection.top), cComment);
          GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns, pSelection.top), cCpk);
          GetTableRowAttribute(panel, TABPANEL_2_TABLE, pSelection.top, ATTR_LABEL_TEXT, cTestStepName);
          cTestStepName[strlen(cTestStepName) - 2] = '\0';
          DeleteGraphPlot(gMeasCurve, MEASCURVE_GRAPH,  - 1, VAL_IMMEDIATE_DRAW);
          PlotWaveform(gMeasCurve, MEASCURVE_GRAPH, gdMeasurements, idx - 1, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
          PlotWaveform(gMeasCurve, MEASCURVE_GRAPH, dLowerLimits, idx - 1, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_GREEN);
          PlotWaveform(gMeasCurve, MEASCURVE_GRAPH, dUpperLimits, idx - 1, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_GREEN);
          sprintf(cTempBuffer, "%s - CompType: %s, Lower Limit: %s, Upper Limit: %s, Unit: %s", cTestStepName, cCompType, cLowerLimit, cUpperLimit, cUnit);
          SetCtrlAttribute(gMeasCurve, MEASCURVE_GRAPH, ATTR_LABEL_TEXT, cTempBuffer);
          SetCtrlAttribute(gMeasCurve, MEASCURVE_GRAPH, ATTR_PLOT_BGCOLOR, VAL_BLACK);
          SetCtrlAttribute(gMeasCurve, MEASCURVE_GRAPH, ATTR_XPRECISION, 0);
          SetCtrlAttribute(gMeasCurve, MEASCURVE_GRAPH, ATTR_PLOT_BGCOLOR, VAL_WHITE);
          //Histogramm
          DeleteGraphPlot(gMeasCurve, MEASCURVE_HISTOGRAM,  - 1, VAL_IMMEDIATE_DRAW);
          giCountOfHistoVals = iCountOfColumns - 6;
          MaxMin1D(gdMeasurements, giCountOfHistoVals, &dMax, &iMaxIndex, &dMin, &iMinIndex);
          GetCtrlVal(gMeasCurve, MEASCURVE_HISTOSLIDER, &iHistoResolution);
          Histogram(gdMeasurements, giCountOfHistoVals, dMin, dMax, iHistogramArray, dAxisArray, iHistoResolution);
          PlotXY(gMeasCurve, MEASCURVE_HISTOGRAM, dAxisArray, iHistogramArray, iHistoResolution, VAL_DOUBLE, VAL_INTEGER, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
          GetAxisScalingMode(gMeasCurve, MEASCURVE_HISTOGRAM, VAL_LEFT_YAXIS, &iScaleMode, &dMin, &dMax);
          PlotLine(gMeasCurve, MEASCURVE_HISTOGRAM, gdLowerLimit, dMin, gdLowerLimit, dMax, VAL_GREEN);
          PlotLine(gMeasCurve, MEASCURVE_HISTOGRAM, gdUpperLimit, dMin, gdUpperLimit, dMax, VAL_GREEN);
          SetCtrlAttribute(gMeasCurve, MEASCURVE_HISTOGRAM, ATTR_YPRECISION, 0);
          SetCtrlAttribute(gMeasCurve, MEASCURVE_HISTOGRAM, ATTR_PLOT_BGCOLOR, VAL_WHITE);
          InstallPopup(gMeasCurve);
        }
      }
      else if ((pSelection.height == 1) && (pSelection.top > 4) && (iCountOfColumns <= 16))
      {
        MessagePopup("CP-ITS", "Not enough test runs!");
      }
      else if ((pSelection.width == 1) && (pSelection.left < (iCountOfColumns - 5)))
      {
        GetTableColumnAttribute(panel, TABPANEL_2_TABLE, pSelection.left, ATTR_LABEL_TEXT, cSerial);
        sprintf(cTempBuffer, "%s_Measurement_Data.csv", cSerial);
        iSelectionStatus = FileSelectPopup("C:\\", cTempBuffer, "*.csv; *.txt", "CP-ITS Save Measurements", VAL_SAVE_BUTTON, 0, 0, 1, 1, cPathName);
        if (!iSelectionStatus)
        {
          return 0;
        }
        pFile = fopen(cPathName, "w");
        if (pFile)
        {
          fprintf(pFile, "This file was written by CP-ITS Version %s - Date: %s - Time: %s\n\n", LIB_VERSION, cOwnDateStr(), TimeStr());
          TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "RunState.SequenceFile.Path", TS_PropOption_InsertIfMissing, &cTempBuffer2);
          fprintf(pFile, "Sequence File Name: %s\n", cTempBuffer2);
          CA_FreeMemory(cTempBuffer2);
          TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "ThisContext.StationGlobals.TS.CurrentUser.LoginName", TS_PropOption_InsertIfMissing, &cTempBuffer2);
          fprintf(pFile, "User Login Name: %s\n", cTempBuffer2);
          CA_FreeMemory(cTempBuffer2);
          TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.PhysicalIniFile", TS_PropOption_InsertIfMissing, &cTempBuffer2);
          fprintf(pFile, "Physical Ini File: %s\n", cTempBuffer2);
          CA_FreeMemory(cTempBuffer2);
          TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.ApplicationIniFile", TS_PropOption_InsertIfMissing, &cTempBuffer2);
          fprintf(pFile, "Application Ini File: %s\n", cTempBuffer2);
          CA_FreeMemory(cTempBuffer2);
          TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.BenchName1", TS_PropOption_InsertIfMissing, &cTempBuffer2);
          fprintf(pFile, "Bench Name: %s\n", cTempBuffer2);
          CA_FreeMemory(cTempBuffer2);
          TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.Station_ID", TS_PropOption_InsertIfMissing, &cTempBuffer2);
          fprintf(pFile, "Station Name: %s\n\n", cTempBuffer2);
          CA_FreeMemory(cTempBuffer2);
          fprintf(pFile, "Serial: %s\n", cSerial);
          GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(pSelection.left, 1), cTempBuffer);
          GetTableCellAttribute(panel, TABPANEL_2_TABLE, MakePoint(pSelection.left, 1), ATTR_TEXT_BGCOLOR, &iResultColorValue);
          cResult = cTempBuffer[0];
          fprintf(pFile, "Result: %s\n", cTempBuffer);
          GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(pSelection.left, 2), cTempBuffer);
          fprintf(pFile, "Variant: %s\n", cTempBuffer);
          GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(pSelection.left, 3), cTempBuffer);
          fprintf(pFile, "Start time stamp: %s\n", cTempBuffer);
          GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(pSelection.left, 4), cTempBuffer);
          fprintf(pFile, "Execution time: %s\n\n", cTempBuffer);
          fprintf(pFile, "StepName; Value; LowerLimit; UpperLimit; Unit; CompType; Comment; Result\n");
          for (idx = 5; idx <= iCountOfRows; idx++)
          {
            GetTableRowAttribute(panel, TABPANEL_2_TABLE, idx, ATTR_LABEL_TEXT, cTestStepName);
            cTestStepName[strlen(cTestStepName) - 2] = '\0';
            GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(pSelection.left, idx), cValue);
            GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 5, idx), cLowerLimit);
            GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 4, idx), cUpperLimit);
            GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 3, idx), cUnit);
            GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 2, idx), cCompType);
            GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 1, idx), cComment);
            GetTableCellAttribute(panel, TABPANEL_2_TABLE, MakePoint(pSelection.left, idx), ATTR_TEXT_BGCOLOR, &iActualColorValue);
            if (cCompType[0] == 'N')
            {
              vChangePointToComma(cValue, strlen(cValue));
              vChangePointToComma(cLowerLimit, strlen(cLowerLimit));
              vChangePointToComma(cUpperLimit, strlen(cUpperLimit));
            }
            if (cResult == 'P')
            {
              if (iActualColorValue == iResultColorValue)
              {
                fprintf(pFile, "%s; %s; %s; %s; %s; %s; %s; PASS\n", cTestStepName, cValue, cLowerLimit, cUpperLimit, cUnit, cCompType, cComment);
              }
              else
              {
                fprintf(pFile, "%s; %s; %s; %s; %s; %s; %s; FAIL\n", cTestStepName, cValue, cLowerLimit, cUpperLimit, cUnit, cCompType, cComment);
              }
            }
            else
            {
              if (iActualColorValue != iResultColorValue)
              {
                fprintf(pFile, "%s; %s; %s; %s; %s; %s; %s; PASS\n", cTestStepName, cValue, cLowerLimit, cUpperLimit, cUnit, cCompType, cComment);
              }
              else
              {
                fprintf(pFile, "%s; %s; %s; %s; %s; %s; %s; FAIL\n", cTestStepName, cValue, cLowerLimit, cUpperLimit, cUnit, cCompType, cComment);
              }
            }
          }
          fprintf(pFile, "\n");
          fclose(pFile);
        }
        else
        {
          MessagePopup("CP-ITS", "Could not write file!");
          return 0;
        }
      }
      else if ((pSelection.width == iCountOfColumns) && (pSelection.height == iCountOfRows))
      {
        //Select File name
        sprintf(cTempBuffer, "CP_ITS_Measurement_Data.msa");
        iSelectionStatus = FileSelectPopup("C:\\", cTempBuffer, "*.msa; *.csv; *.txt", "CP-ITS Save Measurements", VAL_SAVE_BUTTON, 0, 0, 1, 1, cPathName);
        if (!iSelectionStatus)
        {
          return 0;
        }
        //User selected Text or CSV File
        if (cPathName[strlen(cPathName) - 1] != 'a')
        {
          pFile = fopen(cPathName, "w");
          if (pFile)
          {
            fprintf(pFile, "This file was written by CP-ITS Version %s - Date: %s - Time: %s\n\n", LIB_VERSION, cOwnDateStr(), TimeStr());
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "RunState.SequenceFile.Path", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            fprintf(pFile, "Sequence File Name: %s\n", cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "ThisContext.StationGlobals.TS.CurrentUser.LoginName", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            fprintf(pFile, "User Login Name: %s\n", cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.PhysicalIniFile", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            fprintf(pFile, "Physical Ini File: %s\n", cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.ApplicationIniFile", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            fprintf(pFile, "Application Ini File: %s\n", cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.BenchName1", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            fprintf(pFile, "Bench Name: %s\n", cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.Station_ID", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            fprintf(pFile, "Station Name: %s\n\n", cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            //Headlines
            fprintf(pFile, "Step Name; ");
            for (idx = 1; idx <= iCountOfColumns; idx++)
            {
              GetTableColumnAttribute(panel, TABPANEL_2_TABLE, idx, ATTR_LABEL_TEXT, cSerial);
              fprintf(pFile, "%s; ", cSerial);
            }
            fprintf(pFile, "\n");
            fprintf(pFile, "Result; ");
            //Result line
            for (idx = 1; idx <= iCountOfColumns; idx++)
            {
              GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(idx, 1), cValue);
              fprintf(pFile, "%s; ", cValue);
            }
            fprintf(pFile, "\n");
            fprintf(pFile, "Variant; ");
            //Result line
            for (idx = 1; idx <= iCountOfColumns; idx++)
            {
              GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(idx, 2), cValue);
              fprintf(pFile, "%s; ", cValue);
            }
            fprintf(pFile, "\n");
            fprintf(pFile, "Start time stamp; ");
            //Result line
            for (idx = 1; idx <= iCountOfColumns; idx++)
            {
              GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(idx, 3), cValue);
              fprintf(pFile, "%s; ", cValue);
            }
            fprintf(pFile, "\n");
            fprintf(pFile, "Execution time; ");
            //Result line
            for (idx = 1; idx <= iCountOfColumns; idx++)
            {
              GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(idx, 4), cValue);
              vChangePointToComma(cValue, strlen(cValue));
              fprintf(pFile, "%s; ", cValue);
            }
            fprintf(pFile, "\n");
            for (idx = 5; idx <= iCountOfRows; idx++)
            {
              GetTableRowAttribute(panel, TABPANEL_2_TABLE, idx, ATTR_LABEL_TEXT, cTestStepName);
              cTestStepName[strlen(cTestStepName) - 2] = '\0';
              fprintf(pFile, "%s; ", cTestStepName);
              GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 5, idx), cLowerLimit);
              GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 4, idx), cUpperLimit);
              GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 3, idx), cUnit);
              GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 2, idx), cCompType);
              GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns - 1, idx), cComment);
              GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(iCountOfColumns, idx), cCpk);
              if (cCompType[0] == 'N')
              {
                vChangePointToComma(cLowerLimit, strlen(cLowerLimit));
                vChangePointToComma(cUpperLimit, strlen(cUpperLimit));
                vChangePointToComma(cCpk, strlen(cCpk));
              }
              for (idy = 1; idy <= (iCountOfColumns - 6); idy++)
              {
                GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(idy, idx), cValue);
                if (cCompType[0] == 'N')
                {
                  vChangePointToComma(cValue, strlen(cValue));
                }
                fprintf(pFile, "%s; ", cValue);
              }
              fprintf(pFile, "%s; ", cLowerLimit);
              fprintf(pFile, "%s; ", cUpperLimit);
              fprintf(pFile, "%s; ", cUnit);
              fprintf(pFile, "%s; ", cCompType);
              fprintf(pFile, "%s; ", cComment);
              fprintf(pFile, "%s; ", cCpk);
              fprintf(pFile, "\n");
            }
            fprintf(pFile, "\n");
            fclose(pFile);
          }
          else
          {
            MessagePopup("CP-ITS", "Could not write file!");
            return 0;
          }
        }
        else
        {
          iSavePopupPanel = LoadPanelEx(0, "testcontrolpanel_single.uir", SAVING_POP, __CVIUserHInst);
          InstallPopup(iSavePopupPanel);
          pFile = fopen(cPathName, "wb");
          if (pFile)
          {
            sprintf(MsaPrintHeader.cSaver, "CP-ITS");
            sprintf(MsaPrintHeader.cSaverVersion, LIB_VERSION);
            sprintf(MsaPrintHeader.cFileDate, cOwnDateStr());
            sprintf(MsaPrintHeader.cFileTime, TimeStr());
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "RunState.SequenceFile.Path", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            sprintf(MsaPrintHeader.cSequenceFileName, cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "ThisContext.StationGlobals.TS.CurrentUser.LoginName", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            sprintf(MsaPrintHeader.cUserLoginName, cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.PhysicalIniFile", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            sprintf(MsaPrintHeader.cPhysicalIniName, cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.ApplicationIniFile", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            sprintf(MsaPrintHeader.cApplicationIniName, cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.BenchName1", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            sprintf(MsaPrintHeader.cBenchName, cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.Station_ID", TS_PropOption_InsertIfMissing, &cTempBuffer2);
            sprintf(MsaPrintHeader.cStationName, cTempBuffer2);
            CA_FreeMemory(cTempBuffer2);
            MsaPrintHeader.iCountOfColumns = iCountOfColumns;
            MsaPrintHeader.iCountOfRows = iCountOfRows;
            iStatus = fwrite(&MsaPrintHeader, sizeof(struct msa_header), 1, pFile);
            if ((iStatus != 1) || errno)
            {
              MessagePopup("CP-ITS", "An error occurred while writing this file!");
              fclose(pFile);
              RemovePopup(iSavePopupPanel);
              DiscardPanel(iSavePopupPanel);
              return 0;
            }
            SetCtrlAttribute(iSavePopupPanel, SAVING_POP_SAVING, ATTR_MAX_VALUE, iCountOfColumns);
            for (idx = 0; idx <= iCountOfColumns; idx++)
            {
              SetCtrlVal(iSavePopupPanel, SAVING_POP_SAVING, idx);
              Delay(0.02);
              ProcessDrawEvents();
              if (idx > 0)
              {
                GetTableColumnAttribute(panel, TABPANEL_2_TABLE, idx, ATTR_LABEL_TEXT, cTempBuffer);
                sprintf(CellPrintValue.cContent, cTempBuffer);
                CellPrintValue.iBackGroundColor = 0;
                CellPrintValue.iColumn = idx;
                CellPrintValue.iRow = 0;
                iStatus = fwrite(&CellPrintValue, sizeof(struct cell_value), 1, pFile);
                if ((iStatus != 1) || errno)
                {
                  MessagePopup("CP-ITS", "An error occurred while writing this file!");
                  fclose(pFile);
                  RemovePopup(iSavePopupPanel);
                  DiscardPanel(iSavePopupPanel);
                  return 0;
                }
                for (idy = 1; idy <= iCountOfRows; idy++)
                {
                  GetTableCellVal(panel, TABPANEL_2_TABLE, MakePoint(idx, idy), cTempBuffer);
                  GetTableCellAttribute(panel, TABPANEL_2_TABLE, MakePoint(idx, idy), ATTR_TEXT_BGCOLOR, &iBackGroundColor);
                  sprintf(CellPrintValue.cContent, cTempBuffer);
                  CellPrintValue.iBackGroundColor = iBackGroundColor;
                  CellPrintValue.iColumn = idx;
                  CellPrintValue.iRow = idy;
                  iStatus = fwrite(&CellPrintValue, sizeof(struct cell_value), 1, pFile);
                  if ((iStatus != 1) || errno)
                  {
                    MessagePopup("CP-ITS", "An error occurred while writing this file!");
                    fclose(pFile);
                    RemovePopup(iSavePopupPanel);
                    DiscardPanel(iSavePopupPanel);
                    return 0;
                  }
                }
              }
              else
              {
                sprintf(CellPrintValue.cContent, "Begin");
                CellPrintValue.iBackGroundColor = 0;
                CellPrintValue.iColumn = 0;
                CellPrintValue.iRow = 0;
                iStatus = fwrite(&CellPrintValue, sizeof(struct cell_value), 1, pFile);
                if ((iStatus != 1) || errno)
                {
                  MessagePopup("CP-ITS", "An error occurred while writing this file!");
                  fclose(pFile);
                  RemovePopup(iSavePopupPanel);
                  DiscardPanel(iSavePopupPanel);
                  return 0;
                }
                for (idy = 1; idy <= iCountOfRows; idy++)
                {
                  GetTableRowAttribute(panel, TABPANEL_2_TABLE, idy, ATTR_LABEL_TEXT, cTempBuffer);
                  sprintf(CellPrintValue.cContent, cTempBuffer);
                  CellPrintValue.iBackGroundColor = 0;
                  CellPrintValue.iColumn = idx;
                  CellPrintValue.iRow = idy;
                  iStatus = fwrite(&CellPrintValue, sizeof(struct cell_value), 1, pFile);
                  if ((iStatus != 1) || errno)
                  {
                    MessagePopup("CP-ITS", "An error occurred while writing this file!");
                    fclose(pFile);
                    RemovePopup(iSavePopupPanel);
                    DiscardPanel(iSavePopupPanel);
                    return 0;
                  }
                }
              }
            }
            fclose(pFile);
            RemovePopup(iSavePopupPanel);
            DiscardPanel(iSavePopupPanel);
          }
          else
          {
            MessagePopup("CP-ITS", "Could not write file!");
            return 0;
          }
        }
      }
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
CloseGraphWindow
*
@brief Operatorpanel || Hit close button of graph window
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK CloseGraphWindow(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  switch (event)
  {
    case EVENT_COMMIT:
      RemovePopup(gMeasCurve);
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
MeasCurvePanelCallback
*
@brief Operatorpanel ||  Hit windows close button of graph window 
*
@param panel: Panel id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK MeasCurvePanelCallback(int panel, int event, void* callbackData, int eventData1, int eventData2)
{
  switch (event)
  {
    case EVENT_GOT_FOCUS:
      break;
    case EVENT_LOST_FOCUS:
      break;
    case EVENT_CLOSE:
      RemovePopup(gMeasCurve);
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
RunModeMSAType1
*
@brief Operatorpanel ||  Menu enables the msa measurement
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK RunModeMSAType1(int menuBar, int menuItem, void* callbackData, int panel)
{
  int iStatus, iLeft;
  char cResponeBuffer[101];
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int iChecked;
  char cTraceBuffer[1024];
  threadData = (ThreadData*)callbackData;
  GetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, &iChecked);
  if (!iChecked)
  {
    giMSAType1Reiterations = 0;
    while ((giMSAType1Reiterations < 25) || (giMSAType1Reiterations > 50))
    {
      iStatus = PromptPopup("CP-ITS", "You have  enabled the MSA-Analysis Type 1. Please type the number reiterations and hit OK (25...50)", cResponeBuffer, 100);
      if (iStatus)
      {
        return 0;
      }
      giMSAType1Reiterations = atoi(cResponeBuffer);
    }
    giMSAType1ActualIndex = giMSAType1Reiterations;
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.MSAType1.Enabled", TS_PropOption_InsertIfMissing, VTRUE);
    TS_PropertySetValNumber(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.MSAType1.Reiterations", TS_PropOption_InsertIfMissing, (double)giMSAType1Reiterations);
    TS_PropertySetValNumber(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.MSAType1.ActualIndex", TS_PropOption_InsertIfMissing, (double)giMSAType1ActualIndex);
    SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, VTRUE);
    sprintf(cTraceBuffer, "MSA Type __1 (%d Reiterations)", giMSAType1Reiterations);
    SetMenuBarAttribute(menuBar, menuItem, ATTR_ITEM_NAME, cTraceBuffer);
    SetCtrlAttribute(panel, OP_SINGLE_MSA_INDICATOR, ATTR_ON_COLOR, VAL_RED);
    SetCtrlAttribute(panel, OP_SINGLE_MSA_INDICATOR, ATTR_OFF_COLOR, VAL_BLACK);
    sprintf(cTraceBuffer, "MSA Option (%d)", giMSAType1ActualIndex);
    SetCtrlAttribute(panel, OP_SINGLE_MSA_INDICATOR, ATTR_LABEL_TEXT, cTraceBuffer);
    SetCtrlVal(panel, OP_SINGLE_MSA_INDICATOR, 1);
    GetCtrlAttribute(panel, OP_SINGLE_STROKEINDICATOR, ATTR_LEFT, &iLeft);
    SetCtrlAttribute(panel, OP_SINGLE_STROKEINDICATOR, ATTR_LEFT, iLeft + 25);
    giMSAType1IsEnabled = TRUE;
  }
  else
  {
    if (giMSAType1ActualIndex)
    {
      iStatus = ConfirmPopup("CP-ITS", "MSA-Analysis Type 1 is enabled. Are you sure to quit?");
      if (!iStatus)
      {
        return 0;
      }
    }
    giMSAType1ActualIndex = 0;
    giMSAType1IsEnabled = FALSE;
    SetMenuBarAttribute(menuBar, menuItem, ATTR_CHECKED, VFALSE);
    sprintf(cTraceBuffer, "MSA Type __1");
    SetMenuBarAttribute(menuBar, menuItem, ATTR_ITEM_NAME, cTraceBuffer);
    SetCtrlAttribute(panel, OP_SINGLE_MSA_INDICATOR, ATTR_OFF_COLOR, VAL_GRAY);
    SetCtrlAttribute(panel, OP_SINGLE_MSA_INDICATOR, ATTR_LABEL_TEXT, "MSA Option");
    SetCtrlVal(panel, OP_SINGLE_MSA_INDICATOR, 0);
    TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.MSAType1.Enabled", TS_PropOption_InsertIfMissing, VFALSE);
    TS_PropertySetValNumber(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.MSAType1.Reiterations", TS_PropOption_InsertIfMissing, (double)giMSAType1Reiterations);
    TS_PropertySetValNumber(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.MSAType1.ActualIndex", TS_PropOption_InsertIfMissing, (double)giMSAType1ActualIndex);
    GetCtrlAttribute(panel, OP_SINGLE_STROKEINDICATOR, ATTR_LEFT, &iLeft);
    SetCtrlAttribute(panel, OP_SINGLE_STROKEINDICATOR, ATTR_LEFT, iLeft - 25);
  }
}

/* FUNCTION *****************************************************************/
/**
MSAIndicatorCallBack
*
@brief Operatorpanel ||  Disabled the acutal msa test run - double click on indoecator 
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK MSAIndicatorCallBack(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  int iStatus, iLeft;
  char cTraceBuffer[1024];
  int menuBar;
  threadData = (ThreadData*)callbackData;
  switch (event)
  {
    case EVENT_LEFT_DOUBLE_CLICK:
      if (giMSAType1ActualIndex)
      {
        iStatus = ConfirmPopup("CP-ITS", "MSA-Analysis is enabled. Are you sure to quit?");
        if (!iStatus)
        {
          return 0;
        }
      }
      giMSAType1ActualIndex = 0;
      giMSAType1IsEnabled = FALSE;
      menuBar = GetPanelMenuBar(panel);
      SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_MSATYPE1, ATTR_CHECKED, VFALSE);
      sprintf(cTraceBuffer, "MSA Type __1");
      SetMenuBarAttribute(menuBar, MENUBAR_PROGRAM_MODE_MSATYPE1, ATTR_ITEM_NAME, cTraceBuffer);
      SetCtrlAttribute(panel, OP_SINGLE_MSA_INDICATOR, ATTR_OFF_COLOR, VAL_GRAY);
      SetCtrlAttribute(panel, OP_SINGLE_MSA_INDICATOR, ATTR_LABEL_TEXT, "MSA Option");
      SetCtrlVal(panel, OP_SINGLE_MSA_INDICATOR, 0);
      TS_PropertySetValBoolean(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.MSAType1.Enabled", TS_PropOption_InsertIfMissing, VFALSE);
      TS_PropertySetValNumber(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.MSAType1.Reiterations", TS_PropOption_InsertIfMissing, (double)giMSAType1Reiterations);
      TS_PropertySetValNumber(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.MSAType1.ActualIndex", TS_PropOption_InsertIfMissing, (double)giMSAType1ActualIndex);
      GetCtrlAttribute(panel, OP_SINGLE_STROKEINDICATOR, ATTR_LEFT, &iLeft);
      SetCtrlAttribute(panel, OP_SINGLE_STROKEINDICATOR, ATTR_LEFT, iLeft - 25);
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
ChangeHistogramBars
*
@brief Debug panel ||  Change histogram solution
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK ChangeHistogramBars(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  int iHistogramArray[1024];
  double dAxisArray[1024];
  int iScaleMode;
  double dMin, dMax;
  int iMaxIndex, iMinIndex;
  int iValue;
  switch (event)
  {
    case EVENT_VAL_CHANGED:
      //Histogramm
      GetCtrlVal(panel, control, &iValue);
      DeleteGraphPlot(panel, MEASCURVE_HISTOGRAM,  - 1, VAL_IMMEDIATE_DRAW);
      MaxMin1D(gdMeasurements, giCountOfHistoVals, &dMax, &iMaxIndex, &dMin, &iMinIndex);
      Histogram(gdMeasurements, giCountOfHistoVals, dMin, dMax, iHistogramArray, dAxisArray, iValue);
      PlotXY(panel, MEASCURVE_HISTOGRAM, dAxisArray, iHistogramArray, iValue, VAL_DOUBLE, VAL_INTEGER, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
      GetAxisScalingMode(panel, MEASCURVE_HISTOGRAM, VAL_LEFT_YAXIS, &iScaleMode, &dMin, &dMax);
      PlotLine(panel, MEASCURVE_HISTOGRAM, gdLowerLimit, dMin, gdLowerLimit, dMax, VAL_GREEN);
      PlotLine(panel, MEASCURVE_HISTOGRAM, gdUpperLimit, dMin, gdUpperLimit, dMax, VAL_GREEN);
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
PrintGraphWindow
*
@brief Operator panel || Print button
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK PrintGraphWindow(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  switch (event)
  {
    case EVENT_COMMIT:
      PrintPanel(panel, "", 1, VAL_FULL_PANEL, 1);
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
TimerForRunButtonIfMSAEnabled
*
@brief Operator panel || timer for automatic restart of test run
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK TimerForRunButtonIfMSAEnabled(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  switch (event)
  {
    case EVENT_TIMER_TICK:
      RunButtonHit(panel, control, event, callbackData, eventData1, eventData2);
      break;
  }
  return 0;
}

/* FUNCTION *****************************************************************/
/**
GeneralTimerCallback
*
@brief Operator panel || General timer, sets the sequence file and path name in gui
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK GeneralTimerCallback(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
  ThreadData* threadData;
  ERRORINFO pTSErrorInfo;
  char cTempBuffer[1024];
  char* cTempBuffer2;
  threadData = (ThreadData*)callbackData;
  switch (event)
  {
    case EVENT_TIMER_TICK:
      TS_PropertyGetValString(threadData->ThisContext, &pTSErrorInfo, "RunState.SequenceFile.Path", TS_PropOption_InsertIfMissing, &cTempBuffer2);
      sprintf(cTempBuffer, "Sequence-File: %s", cTempBuffer2);
      SetCtrlVal(panel, OP_SINGLE_SEQFILEPATH, cTempBuffer);
      CA_FreeMemory(cTempBuffer2);
      SetCtrlArrayAttribute(control, ATTR_ENABLED, 0);
      break;
  }
  return 0;
}


/* FUNCTION *****************************************************************/
/**
OrderNumberCallback
*
@brief Operator panel || Double click start process to change order number
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK OrderNumberCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int iSelect;
	ThreadData* threadData;
	ERRORINFO pTSErrorInfo;
	char cTempBuffer[1024];
	threadData = (ThreadData*)callbackData;
	
	switch (event)
	{
		case EVENT_LEFT_DOUBLE_CLICK:
			if ((giOrderNumberIsMandatory == 1) || (giOrderNumberIsMandatory == 3)) 
			{
				sprintf(cTempBuffer, "");
				do 
				{
					iSelect = GenericMessagePopup ("CP-ITS ControlPanel", "Please enter the order number and click <Confirm>.", "Confirm", "Cancel", "", cTempBuffer, 
						50, 0, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN2);
					if (iSelect == 2) return 0;
				} while (strlen(cTempBuffer) == 0);
				sprintf(gcOrderNumber, cTempBuffer);
			} else if ((giOrderNumberIsMandatory == 2) || (giOrderNumberIsMandatory == 4)) 
			{
				sprintf(cTempBuffer, "");
				do 
				{
					iSelect = GenericMessagePopup ("CP-ITS ControlPanel", "Please enter the order number and click <Confirm>.", "Confirm", "Cancel", "", cTempBuffer, 
						giOrderNumberLength, 0, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN2);
					if (iSelect == 2) return 0;
				} while (strlen(cTempBuffer) != giOrderNumberLength);
				sprintf(gcOrderNumber, cTempBuffer);
			}
			sprintf(cTempBuffer, "Order Number: %s", gcOrderNumber);
			SetCtrlVal(panel, OP_SINGLE_ORDERNUMBER, cTempBuffer);
			TS_PropertySetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.OrderNumber", TS_PropOption_InsertIfMissing, gcOrderNumber);  
			SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TIMERFORORDERNUM, ATTR_ENABLED, FALSE);
			SetCtrlAttribute(panel, OP_SINGLE_ORDERNUMBER, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
			iOrderNumberIsSet = 1;
			break;
	}
	return 0;
}

/* FUNCTION *****************************************************************/
/**
ChangeOrderNumberMenu
*
@brief Operator panel || Menu item starts process to change the order number
*
@param menuBar: Menu bar id of resource (Input)
@param menuItem: Menu item id of resource (Input)
@param callbackData: Given callback data (Input)
@param panel: Panel id of resource (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

void CVICALLBACK ChangeOrderNumberMenu (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	int iSelect;
	ThreadData* threadData;
	ERRORINFO pTSErrorInfo;
	char cTempBuffer[1024];
	threadData = (ThreadData*)callbackData;
	
	if ((giOrderNumberIsMandatory == 1) || (giOrderNumberIsMandatory == 3)) 
	{
		sprintf(cTempBuffer, "");
		do 
		{
			iSelect = GenericMessagePopup ("CP-ITS ControlPanel", "Please enter the order number and click <Confirm>.", "Confirm", "Cancel", "", cTempBuffer, 
				50, 0, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN2);
			if (iSelect == 2) return;
		} while (strlen(cTempBuffer) > 0);
		sprintf(gcOrderNumber, cTempBuffer);
	} else if ((giOrderNumberIsMandatory == 2) || (giOrderNumberIsMandatory == 4)) 
	{
		sprintf(cTempBuffer, "");
		do 
		{
			iSelect = GenericMessagePopup ("CP-ITS ControlPanel", "Please enter the order number and click <Confirm>.", "Confirm", "Cancel", "", cTempBuffer, 
				giOrderNumberLength, 0, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN2);
			if (iSelect == 2) return;
		} while (strlen(cTempBuffer) != giOrderNumberLength);
		sprintf(gcOrderNumber, cTempBuffer);
	}
	sprintf(cTempBuffer, "Order Number: %s", gcOrderNumber);
	SetCtrlVal(panel, OP_SINGLE_ORDERNUMBER, cTempBuffer);
	TS_PropertySetValString(threadData->ThisContext, &pTSErrorInfo, "StationGlobals.TSVP.OrderNumber", TS_PropOption_InsertIfMissing, gcOrderNumber); 
	SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TIMERFORORDERNUM, ATTR_ENABLED, FALSE);
	SetCtrlAttribute(panel, OP_SINGLE_ORDERNUMBER, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
	iOrderNumberIsSet = 1;
}

/* FUNCTION *****************************************************************/
/**
TimerForOrderNumber
*
@brief Operator panel || If order number enabled and not set thru Operator or test sequence. Resource is blinking.
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK TimerForOrderNumber (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int iColor;
	switch (event)
	{
		case EVENT_TIMER_TICK:
			GetCtrlAttribute(panel, OP_SINGLE_ORDERNUMBER, ATTR_TEXT_BGCOLOR, &iColor);
		      if (iColor != VAL_RED)
		      {
		        SetCtrlAttribute(panel, OP_SINGLE_ORDERNUMBER, ATTR_TEXT_BGCOLOR, VAL_RED);
		      }
		      if (iColor == VAL_RED)
		      {
		        SetCtrlAttribute(panel, OP_SINGLE_ORDERNUMBER, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
		      }
			break;
	}
	return 0;
}

/* FUNCTION *****************************************************************/
/**
TimerForRunButton
*
@brief Operator panel || Check variant settings and order number settings. If all set, run button is enabled
*
@param panel: Panel id of resource (Input)
@param control: Control id of resource (Input)
@param event: event type id of callback (Input)
@param callbackData: Given callback data (Input)
@param eventData1: Given event data 1 (Input)
@param eventData2: Given event data 2 (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int CVICALLBACK TimerForRunButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			if (iVariantIsSet && iOrderNumberIsSet)
			{
				SetCtrlAttribute(panel, OP_SINGLE_RUN_TEST, ATTR_DIMMED, FALSE);
				SetCtrlAttribute(panel, control, ATTR_ENABLED, FALSE);
			}
			break;
			
			}
	return 0;
}
