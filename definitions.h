/****************************************************************************/
/**

Copyright 2007-2020 Robert Renner

This file is part of SW-ITS for Rohde &  Schwarz CompactTSVP.

SW-ITS for Rohde &  Schwarz CompactTSVP is free software: you can
redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option)
any later version. SW-ITS for Rohde &  Schwarz CompactTSVP is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details. You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

@file definitions.h
 *
@brief All other declarations
 *
@version 2.4.0.0
 *
@author Robert Renner <A HREF="mailto:trelliscoded@hotmail.com">trelliscoded@hotmail.com</A>\n
 *
language: ANSI-C ISO/IEC9899:1990
 *
<b>History:</b>
- <b>23.11.2007 R. Renner</b>
- Initial revision

 ******************************************************************************
 *****************************************************************************/

#ifndef TSPAN_DEF_H
#define TSPAN_DEF_H

/* INCLUDE FILES *************************************************************/

#include <cvirte.h>
#include <objbase.h>
#include <utility.h>
#include <userint.h>
#include <ansi_c.h>
#include <tsapicvi.h>
#include <formatio.h>
#include <analysis.h>
#include <resmgr.h>
#include "inifile.h"
#include "hrestim.h"
#include "ict.h"
#include "eazyscan.h"
#include "splash.h"
#include "toolbox.h"
#include "testdebugpanel.h"
#include "testcontrolpanel_single.h"
#include "gtslerr.h"   /* GTSL error handling */


/* GLOBAL DEFINES ************************************************************/

#define SETINDEX 1

#define MEAS_ARRAY_SIZE 1024

#define WAITTMO 3000
#define OFF_MODE 0
#define DEBUG_MODE 1
#define OPERATOR_MODE 2

#define COMP_EQ     1      //"=="
#define COMP_NEQ    2      //"!="
#define COMP_GT     3      //">"
#define COMP_LT     4      //"<"
#define COMP_GE     5      //">="
#define COMP_LE     6      //"<="
#define COMP_GTLT   7      //"><"
#define COMP_GELE   8      //">=<="
#define COMP_GELT   9      //">=<"
#define COMP_GTLE   10     //"><="
#define COMP_LTGT   11     //"<>"
#define COMP_LEGE   12     //"<=>="
#define COMP_LEGT   13     //"<=>"
#define COMP_LTGE   14     //"<>="
#define COMP_NOCOMP 0      //"LOG"
#define COMP_PAFA   -1     //PASSFAIL
#define COMP_STRN   -2     //String value (normal)
#define COMP_STRI   -3     //String value (case sensitive)

#define TSPAN_ERR_BASE                          -1003000
#define TSPAN_ERR_NOT_A_BENCH                   (TSPAN_ERR_BASE - 1)    /* -1003001 */
#define TSPAN_ERR_TITLEBARTEXT_NOT_FOUND        (TSPAN_ERR_BASE - 2)    /* -1003002 */
#define TSPAN_ERR_PRODUCTNAMETEXT_NOT_FOUND     (TSPAN_ERR_BASE - 3)    /* -1003003 */
#define TSPAN_ERR_PANELTYPE_NOT_FOUND           (TSPAN_ERR_BASE - 4)    /* -1003004 */
#define TSPAN_ERR_DEBUGPANELPOSITION_NOT_FOUND  (TSPAN_ERR_BASE - 5)    /* -1003005 */
#define TSPAN_ERR_PANELTYPEENTRYNOTVALID        (TSPAN_ERR_BASE - 6)    /* -1003006 */
#define TSPAN_ERR_PANELPOSITIONENTRYNOTVALID    (TSPAN_ERR_BASE - 7)    /* -1003007 */
#define TSPAN_ERR_THREADWASNOTSTARTET           (TSPAN_ERR_BASE - 8)    /* -1003008 */
#define TSPAN_ERR_WRONGCOMPYTE                  (TSPAN_ERR_BASE - 9)    /* -1003009 */
#define TSPAN_ERR_WRONGFORMATYTE                (TSPAN_ERR_BASE - 10)   /* -1003010 */
#define TSPAN_ERR_WRONGYIELDSETTING             (TSPAN_ERR_BASE - 11)   /* -1003011 */
#define TSPAN_ERR_SERIALSETTINGNOTFOUND         (TSPAN_ERR_BASE - 12)   /* -1003012 */
#define TSPAN_ERR_WRONGSERIALSETTING            (TSPAN_ERR_BASE - 13)   /* -1003013 */
#define TSPAN_ERR_SERIALLENGTHENTRIENOTFOUND    (TSPAN_ERR_BASE - 14)   /* -1003014 */
#define TSPAN_ERR_WRONGVARIANTCOUNT             (TSPAN_ERR_BASE - 15)   /* -1003015 */  
#define TSPAN_ERR_VARIANTNOTSPECIFIED           (TSPAN_ERR_BASE - 16)   /* -1003016 */ 
#define TSPAN_ERR_WRONGRUNBUTTONSETTING         (TSPAN_ERR_BASE - 17)   /* -1003017 */ 
#define TSPAN_ERR_STROKECOUNTERFILENOTEXISTS    (TSPAN_ERR_BASE - 18)   /* -1003018 */ 
#define TSPAN_ERR_TICKETPRINTERENABLEDNOTVALID  (TSPAN_ERR_BASE - 19)   /* -1003019 */ 
#define TSPAN_ERR_REPORTENABLEDNOTVALID         (TSPAN_ERR_BASE - 20)   /* -1003020 */ 
#define TSPAN_ERR_APPREPORTTOEXISTINGNOTVALID   (TSPAN_ERR_BASE - 21)   /* -1003021 */ 
#define TSPAN_ERR_REPORTONLYFAILEDNOTVALID      (TSPAN_ERR_BASE - 22)   /* -1003022 */ 
#define TSPAN_ERR_RESETSTATISTICSNOTVALID       (TSPAN_ERR_BASE - 23)   /* -1003023 */ 
#define TSPAN_ERR_FIXTURECODENOTSPEC            (TSPAN_ERR_BASE - 24)   /* -1003024 */ 
#define TSPAN_ERR_FIXTURECODENOTFOUND           (TSPAN_ERR_BASE - 25)   /* -1003025 */ 
#define TSPAN_ERR_ACTUALSTROKECOUNTSNOTFOUND    (TSPAN_ERR_BASE - 26)   /* -1003026 */ 
#define TSPAN_ERR_YELLOWLIMITNOTFOUND           (TSPAN_ERR_BASE - 27)   /* -1003027 */ 
#define TSPAN_ERR_REDLIMITNOTFOUND              (TSPAN_ERR_BASE - 28)   /* -1003028 */ 
#define TSPAN_ERR_WRITEFIXTURECODECFG           (TSPAN_ERR_BASE - 29)   /* -1003029 */ 
#define TSPAN_ERR_INIFILEERROR                  (TSPAN_ERR_BASE - 30)   /* -1003030 */ 
#define TSPAN_ERR_MSAOPTIONENABLEDNOTVALID      (TSPAN_ERR_BASE - 31)   /* -1003031 */ 
#define TSPAN_ERR_WRONGORDERNUMSETTING          (TSPAN_ERR_BASE - 32)   /* -1003032 */
#define TSPAN_ERR_ORDERNUMLENGTHENTRIENOTFOUND  (TSPAN_ERR_BASE - 33)   /* -1003033 */  

/* Error code to message reference table */
static GTSL_ERROR_TABLE errorTable =
{
	/* library specific error codes and messages */
	{
		TSPAN_ERR_NOT_A_BENCH, "The given resource is not a bench"
	}
	,
	{
		TSPAN_ERR_TITLEBARTEXT_NOT_FOUND,
		"The titlebar text was not specified in the application.ini"
	}
	,
	{
		TSPAN_ERR_PRODUCTNAMETEXT_NOT_FOUND,
		"The product name text was not specified in the application.ini"
	}
	,
	{
		TSPAN_ERR_PANELTYPE_NOT_FOUND,
		"The panel type was not specified in the application.ini"
	}
	,
	{
		TSPAN_ERR_DEBUGPANELPOSITION_NOT_FOUND,
		"The debug panel position was not specified in the application.ini"
	}
	,
	{
		TSPAN_ERR_PANELTYPEENTRYNOTVALID,
		"The panel type entry specified in the application.ini is not valid"
	}
	,
	{
		TSPAN_ERR_PANELPOSITIONENTRYNOTVALID,
		"The panel position entry specified in the application.ini is not valid"
	}
	,
	{
		TSPAN_ERR_THREADWASNOTSTARTET,
		"Thread was not started. Possible cause: UIR file was not found."
	}
	,
	{
		TSPAN_ERR_WRONGCOMPYTE,
		"Comparison type is not supported."
	}
	,
	{
		TSPAN_ERR_WRONGFORMATYTE,
		"Format type is not supported."
	}
	,
	{
		TSPAN_ERR_WRONGYIELDSETTING,
		"Wrong yield setting parameter. Please check application.ini"
	}
	,
	{
		TSPAN_ERR_SERIALSETTINGNOTFOUND,
		"SerialIsMandatory entry not found. Please check application.ini"
	}
	,
	{
		TSPAN_ERR_WRONGSERIALSETTING,
		"Wrong SerialIsMandatory setting parameter. Please check application.ini"
	}
	,
	{
		TSPAN_ERR_SERIALLENGTHENTRIENOTFOUND,
		"SerialLength entry not found. Please check application.ini"
	}
	,
	{
		TSPAN_ERR_WRONGVARIANTCOUNT,
		"Wrong Variant setting parameter. Please check application.ini"
	}
	,
	{
		TSPAN_ERR_VARIANTNOTSPECIFIED,
		"Variants not specified. Please check application.ini"
	}
	,
	{
		TSPAN_ERR_WRONGRUNBUTTONSETTING,
		"Wrong run button setting parameter. Please check application.ini"
	}
	,
	{
		TSPAN_ERR_STROKECOUNTERFILENOTEXISTS,
		"Stroke counting is enabled, but the stroke counter file does not exists. Please check application.ini"
	}
	,
	{
		TSPAN_ERR_TICKETPRINTERENABLEDNOTVALID,
		"The entry TicketPrinterEnabled is wrong. Please check format and value in application.ini"
	}
	,
	{
		TSPAN_ERR_REPORTENABLEDNOTVALID,
		"The entry ReportEnabled is wrong. Please check format and value in application.ini"
	}
	,
	{
		TSPAN_ERR_APPREPORTTOEXISTINGNOTVALID,
		"The entry AppendReportToExisting is wrong. Please check format and value in application.ini"
	}
	,
	{
		TSPAN_ERR_REPORTONLYFAILEDNOTVALID,
		"The entry ReportOnlyFailedTests is wrong. Please check format and value in application.ini"
	}
	,
	{
		TSPAN_ERR_RESETSTATISTICSNOTVALID,
		"The entry ResetStatisticsEnabled is wrong. Please check format and value in application.ini"
	}
	,
	{
		TSPAN_ERR_FIXTURECODENOTSPEC,
		"Fixture code not specified in StationGlobals.TSVP.ActualFixtureCode or application.ini"
	}
	,
	{
		TSPAN_ERR_FIXTURECODENOTFOUND,
		"Fixture code specified in StationGlobals.TSVP.ActualFixtureCode or application.ini could not be found in FixtureStrokeCounters.cfg"
	}
	,
	{
		TSPAN_ERR_ACTUALSTROKECOUNTSNOTFOUND,
		"Actual stroke counts entry for the specified fixture code could not be found in file FixtureStrokeCounters.cfg"
	}
	,
	{
		TSPAN_ERR_YELLOWLIMITNOTFOUND,
		"Yellow limit entry for the specified fixture code could not be found in file FixtureStrokeCounters.cfg"
	}
	,
	{
		TSPAN_ERR_REDLIMITNOTFOUND,
		"Red limit entry for the specified fixture code could not be found in file FixtureStrokeCounters.cfg"
	}
	,
	{
		TSPAN_ERR_WRITEFIXTURECODECFG,
		"An error occurred while writing the fixture stroke counter file. Note the current fixture strokes!"
	}
	,
	{
		TSPAN_ERR_INIFILEERROR,
		"An error occurred while reading from ControlPanel.ini. Please check the content and syntax in this file!"
	}
	,
	{
		TSPAN_ERR_MSAOPTIONENABLEDNOTVALID,
		"The entry MSAOptionsEnabled is wrong. Please check format and value in application.ini"
	}
	,
		{
		TSPAN_ERR_WRONGORDERNUMSETTING,
		"Wrong order number setting parameter. Please check application.ini"
	}
	,
	{
		TSPAN_ERR_ORDERNUMLENGTHENTRIENOTFOUND,
		"order number length entry not found. Please check application.ini"
	}
	,
	 
	/* include common GTSL error codes and messages */
	GTSL_ERROR_CODES_AND_MESSAGES,

	/* this must be the last entry ! */
	{
		0, NULL
	}
};

typedef struct
{
   int iOwner; /* memory block owner                          */
   int iSimulation; /* driver simulation                           */
   int iDemoMode; /* driver simulation                           */
   int iMode;
   char cTitlebarText[1024]; /* Text shown in title bar                     */
   char cProductName[1024]; /* Test shown as product name                  */
   char cSequenceFilePath[1024]; /* Test shown as product name                  */
   char cPanelType[20]; /* panel type is specified in application.ini  */
   // Valid values SINGLE
   char cDebugPosition[20]; /* debug panel position on screen              */
   // Valid values TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMDOWN
   int iActualPanelHandle;
   int iActualTabHandle;
   DWORD gThreadID;
   
   //Yield settings
   int YieldCheckEnabled;
   int MinPassRate;
   int AfterTestedBoards;
   int ConsecutiveTests;
   
   //Variants
   int iVariantsEnabled;
   char cSelectedVariant[1024];
   char cVariants[100][1024];
   
   //Serial number settings
   int SerialIsMandatory;
   int SerialLength; 
   
   //Odern number settings
   int OrderNumberIsMandatory;
   int OrderNumberLength; 
   
   int RunButtonDisabled;
   int DisableExecutionTime;
   int Briefing;
   
   float TimingMSAWithRunButton;
   float cpkLimit;
   
   //Fixture stroke counter
   int StrokeCounterEnabled;
   int FixtureCode;
   char StrokeCounterFile[1024];
   int StrokeCounterYellowLimit;
   int StrokeCounterRedLimit;
   int StrokeActualCounts;
   
   //Menu settings
   int iTicketPrinterEnabled;
   int iReportEnabled;
   int iAppendReportToExisting;
   int iReportOnlyFailedTests;
   int iResetStatisticsEnabled;
   int iMSAOptionsEnabled;
   
   //Test result window
   char cTestResultFontType[1024];
   int iTestResultFontSize;
   int iTestResultFontUseBold;
   int iUseColoredBackGround;
   int iBGColorPassRed;
   int iBGColorPassGreen;
   int iBGColorPassBlue;
   int iBGColorFailRed;
   int iBGColorFailGreen;
   int iBGColorFailBlue;
   int iBGColorRunningRed;
   int iBGColorRunningGreen;
   int iBGColorRunningBlue;

} BENCH_STRUCT;

typedef struct threadDataRec
{
   CAObjHandle execution;
   CAObjHandle ThisContext;
} ThreadData;


typedef struct teststep
{
	// 0 -> Numeric, 1 -> PassFail, 2 -> String
	char cTestCaseType;
	
	char cStepName[1024];
	char cComment[1024];
	char cLogMessage[1024];
	
	double dMeasurement;
	double dLowerLimit;
	double dUpperLimit;
	
	char cUnit[50];
	char cCompType[50];
	char cFormat[10];
	
	//Pass/Fail
	int iResult;
	
	//String
	char cStringToCompare[1024];
	char cExpectedString[1024];
	
	struct teststep *pNext;
	
} TestStep;

typedef struct testresult
{
	char cTestResult;
	char cSerial[1024];
	char cStartTime[30];
	char cStartDate[30];
	char cStopTime[30];
	char cStopDate[30];
	double dExecutionTime;
	char cVariant[1024];
	struct teststep *pFirst;
} TestResult;

typedef struct msa_header
{
	char cSaver[10];
	char cSaverVersion[20];      
	char cFileDate[20];
	char cFileTime[20];
	char cSequenceFileName[1024];
	char cUserLoginName[1024];
	char cPhysicalIniName[1024];
	char cApplicationIniName[1024];      
	char cBenchName[1024];  
	char cStationName[1024];    
	int iCountOfColumns;
	int iCountOfRows;
} MsaHeader;

typedef struct cell_value
{
	int iColumn;
	int iRow;	
	int iBackGroundColor;
	char cContent[1024];
} CellValue;

/* GLOBAL VARIABLES DEFINITION ***********************************************/

static const char LIBRARY_NAME[] = "TSPAN"; /* Library Version String */
static const char LIB_VERSION[] = "TSPAN 2.4.0.5"; /* Library Version String */
static char *cDLLPATH;

TestResult *giAllTestResults[50];
int giActualTestResult;
int giTestIsRunning;

int giMSAType1IsEnabled;
int giMSAType1Reiterations;
int giMSAType1ActualIndex;

CRITICAL_SECTION CriticalSection;
int giActualPanelHandle;
int giActualTabHandle;
int giMenuBarHandle;
int giActualValue;
char gcSelectedVariant[1024];
int giVariantsEnabled;
int iThreadCount;
HANDLE hThreadReady;
HANDLE hThreadHandle;

HINSTANCE hinstDLLOwn;

double gdMeasurements[1024];
double gdLowerLimit;
double gdUpperLimit;
int giActualIndex;
int giHistogramInterval;
char cLastNameOfStep[1024];
char gcSerialNumber[1024];
char gcOrderNumber[1024];
char gcSelectedVariant[1024];
int lLastCompType;

int giCountOfHistoVals;

int gSplash;
int gMeasCurve;

int giSerialIsMandatory;
int giSerialLength;
int giOrderNumberIsMandatory;
int giOrderNumberLength;
int giRunButtonDisabled;
int iVariantIsSet;
int iOrderNumberIsSet;
int giErrorLogWindowEnabled;

int giLowerLimit;
int giUpperLimit;
int giMaxValue;
int giMinValue;
double gdMaxValue;
double gdMinValue;
int ShowCPKValues_Enabled;

int giVisibleLines;
int giActualItemIndex;
int giActualErrorLogItemIndex;

int giYieldCheckEnabledMenu; 
int giYieldCheckEnabledFile;
int giPassUUTs;
int giFailUUTs;
int giCountOfUUTs;
int giConsecutiveFailed;
double gdQRate;  

double gdStartTimeStamp;
double gdStopTimeStamp;

/* GLOABL FUNCTION DECLARATIONS ***********************************************/

void ResetMeasurements (void);

void CVICALLBACK ChangeVariant (int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ChangeVariantDebug (int menubar, int menuItem, void *callbackData, int panel);

int iCompareEQ(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareNEQ(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareGT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareLT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareGE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareLE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareGTLT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareGELE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareGELT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareGTLE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareLTGT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareLEGE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareLEGT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareLTGE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
int iCompareNOCOMP(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024]);
void FormatValues (char cValue[1024], char *cFormat, double dValue);

int iReadFixtureStrokeFile (char *cFileName, int iFixtureCode, int *iActualCounts, int *iYellowLimit, int *iRedLimit);
int iWriteFixtureStrokeFile (char *cFileName, int iFixtureCode, int iActualCounts);
void vOrderGUIElementsAfterResize(int panel);

WINOLEAPI CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);
int StartThreadForDebugPanel(void *data);
int StartThreadForOperatorPanel(void *data);
int StartThreadForAdjustmentPanel(void *data); 
void CVICALLBACK QuitThread(void *callbackData);
int iPlotBarForAnalog (int iTabHandle, double dValue, int iColor);
void SetDebugUirPafa (int iPanelHandle);
void SetDebugUirNum (int iPanelHandle);
void SetDebugUirString (int iPanelHandle);
void InsertVariants (BENCH_STRUCT *pPointer, int PanelHandle, void *data);
void InsertVariantsDebug (BENCH_STRUCT *pPointer, int PanelHandle, void *data);
TestStep *pAppendTestStep (TestStep *pFirstTestStep, char cTestCaseType, char *cStepName, char *cComment, char *cLogMessage,
	double Measurement, double LowerLimit, double UpperLimit, char *cUnit, char *cCompType, char *cFormat, 
	int iResult, char *cStringToCompare, char *cExpectedString);
void vFreeAllTestResults (TestStep *pFirst);
void formatError (char buffer[], int code, long resId, char *benchDevice);
void vChangePointToComma(char* cValue, int iLength);
char *cOwnDateStr (void);    
int iKillTabInStepName (char *cInputString, char *OutputString, int iLength);


#endif 

