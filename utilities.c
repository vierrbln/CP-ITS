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

@file utilities.c
 *
@brief All helper functions of control panel
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

 *****************************************************************************
 *****************************************************************************/

/* INCLUDE FILES ************************************************************/

#include "definitions.h"
#include "tspan.h"

/* GLOBAL VARIABLES DEFINITION **********************************************/

/* LOCAL DEFINES *************************************************/

/* LOCAL FUNCTION DEFINITIONS ***********************************************/

/* LOCAL CONSTANT DEFINITIONS ***********************************************/

/* LOCAL TYPE DEFINITIONS ***************************************************/

/* LOCAL MACRO DEFINITIONS **************************************************/

/* LOCAL FUNCTION DECLARATIONS **********************************************/

/* GLOBAL FUNCTION DEFINITIONS **********************************************/

/* FUNCTION *****************************************************************/
/**
StartThreadForDebugPanel
*
@brief Debugpanel || Starts the new thread for GUI
*
@param data: Thread data (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int StartThreadForDebugPanel(void *data)
{
	int iTabHandle;


	ThreadData *threadData = (ThreadData*)data;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	//  CoInitializeEx() is not declared in objbase.h which ships with CVI 5.0.
	//  You will need to declare this function yourself using the following declaration:
	//  WINOLEAPI  CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);

	//  You must also use objbase.lib which ships with VC++ 5.0.


	if ((giActualPanelHandle = LoadPanelEx(0, "testdebugpanel.uir", DEBUG,
										   __CVIUserHInst)) < 0)
	{
		return  -1;
	}

	if ((giMenuBarHandle = LoadMenuBarEx(giActualPanelHandle,
										 "testdebugpanel.uir", MENUBAR, __CVIUserHInst)) < 0)
	{
		return  -1;
	}

	DisplayPanel(giActualPanelHandle);

	GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 0, &iTabHandle);
	SetCtrlAttribute(iTabHandle, ANALOG_RED, ATTR_ON_COLOR, VAL_RED);
	SetCtrlAttribute(iTabHandle, ANALOG_GREEN, ATTR_ON_COLOR, VAL_GREEN);

	sprintf(cLastNameOfStep, "Powered by softwerk4R");

	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SETTINGS_ALWAYSONTOP, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SETTINGS_SHOWCPK, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SETTINGS_HISTO_I10, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SETTINGS_HISTO_I20, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SETTINGS_HISTO_I30, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SETTINGS_HISTO_I40, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SETTINGS_HISTO_I50, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_UTILITIES_CLEARDATA, ATTR_CALLBACK_DATA, data);

	// Signal main thread that the new thread started
	SetEvent(hThreadReady);

	RunUserInterface();

	DiscardPanel(giActualPanelHandle);
	if (threadData->execution)
	{
		CA_DiscardObjHandle(threadData->execution);
		threadData->execution = 0;
	}
	if (threadData->ThisContext)
	{
		CA_DiscardObjHandle(threadData->ThisContext);
		threadData->ThisContext = 0;
	}
Error:
	return 0;
}

/* FUNCTION *****************************************************************/
/**
StartThreadForOperatorPanel
*
@brief Operatorpanel || Starts the new thread for GUI
*
@param data: Thread data (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int StartThreadForOperatorPanel(void *data)
{
	int iTabHandle;
	ThreadData *threadData = (ThreadData*) data;

	//CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CA_InitActiveXThreadStyleForCurrentThread (0, COINIT_APARTMENTTHREADED);  
	//  CoInitializeEx() is not declared in objbase.h which ships with CVI 5.0.
	//  You will need to declare this function yourself using the following declaration:
	//  WINOLEAPI  CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);

	//  You must also use objbase.lib which ships with VC++ 5.0.


	if ((giActualPanelHandle = LoadPanelEx(0, "testcontrolpanel_single.uir",
										   OP_SINGLE, __CVIUserHInst)) < 0)
	{
		return  - 1;
	}

	if ((gMeasCurve = LoadPanelEx(0, "testcontrolpanel_single.uir",
								  MEASCURVE, __CVIUserHInst)) < 0)
	{
		return  - 1;
	}

	if ((gSplash = LoadPanelEx(0, "Splash.uir",
							   SPLASH, __CVIUserHInst)) < 0)
	{
		return  - 1;
	}
	if ((giMenuBarHandle = LoadMenuBarEx(giActualPanelHandle,
										 "testcontrolpanel_single.uir", MENUBAR, __CVIUserHInst)) < 0)
	{
		return  - 1;
	}

	SetPanelAttribute(giActualPanelHandle, ATTR_MIN_HEIGHT_FOR_SCALING, 641);
	SetPanelAttribute(giActualPanelHandle, ATTR_MIN_WIDTH_FOR_SCALING, 859);
	SetPanelAttribute(giActualPanelHandle, ATTR_WINDOW_ZOOM, VAL_MAXIMIZE);
	SetPanelAttribute(giActualPanelHandle, ATTR_TITLE, cLastNameOfStep);
	SetPanelAttribute(giActualPanelHandle, ATTR_CALLBACK_DATA, data);

	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_REPORT, ATTR_OFF_COLOR, VAL_PANEL_GRAY);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_TICKET, ATTR_OFF_COLOR, VAL_PANEL_GRAY);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_YIELD, ATTR_OFF_COLOR, VAL_PANEL_GRAY);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, ATTR_OFF_COLOR, VAL_PANEL_GRAY);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_MSA_INDICATOR, ATTR_OFF_COLOR, VAL_PANEL_GRAY);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_REPORT, ATTR_ON_COLOR, VAL_DK_GREEN);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_TICKET, ATTR_ON_COLOR, VAL_DK_GREEN);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_YIELD, ATTR_ON_COLOR, VAL_DK_GREEN);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, ATTR_ON_COLOR, VAL_DK_GREEN);

	// Use thread data in callbacks
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_CALLBACK_DATA, data);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_TIMER_MSA_RUNBUTTON, ATTR_CALLBACK_DATA, data);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_TIMERFORVARIANT, ATTR_CALLBACK_DATA, data);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_VARIANT_RING, ATTR_CALLBACK_DATA, data);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_MSA_INDICATOR, ATTR_CALLBACK_DATA, data);
	SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_GENERALTIMER, ATTR_CALLBACK_DATA, data);
	
	GetPanelHandleFromTabPage(giActualPanelHandle, OP_SINGLE_TAB, 1, &iTabHandle);
	SetCtrlAttribute(iTabHandle, TABPANEL_2_TABLE, ATTR_CALLBACK_DATA, data);
	
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_PROGRAM_QUIT, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_PROGRAM_ABOUT, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_PROGRAM_MODE_NORMAL, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_PROGRAM_MODE_SINGLESTEP, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_PROGRAM_MODE_STOPONFAIL, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SET_RESETSTAT, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SET_REPORT_ENABLE, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SET_REPORT_APPEND, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SET_TICKET_ENABLE, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SET_UI_ACTUAL, ATTR_CALLBACK_DATA, data);
	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_SET_UI_REPORT, ATTR_CALLBACK_DATA, data);

	SetMenuBarAttribute(giMenuBarHandle, MENUBAR_PROGRAM_MODE_MSATYPE1, ATTR_CALLBACK_DATA, data);

	// Signal main thread that the new thread started
	SetEvent(hThreadReady);
	
	DisplayPanel(giActualPanelHandle);

	RunUserInterface();

	DiscardPanel(giActualPanelHandle);
	if (threadData->execution)
	{
		CA_DiscardObjHandle(threadData->execution);
		threadData->execution = 0;
	}
	if (threadData->ThisContext)
	{
		CA_DiscardObjHandle(threadData->ThisContext);
		threadData->ThisContext = 0;
	}
Error:
	return 0;
}



/* FUNCTION *****************************************************************/
/**
formatError
*
@brief formats a error message
*
@param buffer: Contains the error message after correct funtiocn (Output)
@param code: Error code (Input)
@param resId: Resource id (Input)
@param benchDevice: Bench device name (Input)
*
@return none
*****************************************************************************/

void formatError(char buffer[], int code, long resId, char *benchDevice)
{
	char *pMsg = NULL;
	char resourceName[RESMGR_MAX_NAME_LENGTH + 1] = "";
	char tempMsg[GTSL_ERROR_BUFFER_SIZE] = "";
	short tempOcc = FALSE;
	long tempCode = 0;
	int written = 0;
	GTSL_ERROR_ENTRY *pErr = errorTable; /* pointer into error entry table */

	/* find the error message for a given error code */
	while (pErr->string != NULL)
	{
		if (pErr->value == code)
		{
			pMsg = pErr->string;
			break;
		}
		pErr++;
	}
	if (pMsg == NULL)
	{
		/* should never happen */
		pMsg = "(no message available for this code)";
	}

	/* setup the error message */

	/* 1) Library name */
	strcpy(buffer, GTSL_ERRMSG_PREFIX_LIBRARY);
	strcat(buffer, LIBRARY_NAME);
	strcat(buffer, "\n");

	/* 2) Bench name, only if a valid ID is given */
	if (resId != RESMGR_INVALID_ID)
	{
		/* read the resource name into a local buffer */
		RESMGR_Get_Resource_Name(0, resId, resourceName, sizeof(resourceName),
								 &written, &tempOcc, &tempCode, tempMsg);
		if ((!tempOcc) && (written > 0))
		{
			/* append the name */
			strcat(buffer, GTSL_ERRMSG_PREFIX_BENCH);
			strcat(buffer, resourceName);
			strcat(buffer, "\n");
		}
	}

	/* 3) Bench device, if given */
	if (benchDevice != NULL)
	{
		strcat(buffer, GTSL_ERRMSG_PREFIX_BENCH_DEVICE);
		strcat(buffer, benchDevice);
		strcat(buffer, "\n");
	}

	/* 4) Error message */
	strcat(buffer, GTSL_ERRMSG_PREFIX_ERRMSG);
	strcat(buffer, pMsg);
}

/* FUNCTION *****************************************************************/
/**
ResetMeasurements
*
@brief Operatorpanel || Starts the new thread for GUI
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

void ResetMeasurements (void)
{
	int idx;
	for (idx = 0; idx < MEAS_ARRAY_SIZE; idx++)
		gdMeasurements[idx] = 0.0;
	giActualIndex = 0;
}

/* FUNCTION *****************************************************************/
/**
iPlotBarForAnalog
*
@brief Plot a bar into debug panel
*
@param iTabHandle: GUI handle (input)
@param dValue: Measurement value to plot (Input)
@param iColor: Color for plot (Input)
*
@return none
*****************************************************************************/

int iPlotBarForAnalog (int iTabHandle, double dValue, int iColor)
{
	double dXArray[2];
	double dYArray[2];

	dXArray[0] = dValue;
	dXArray[1] = dValue;
	dYArray[0] = 0.0;
	dYArray[1] = 1.0;
	return PlotXY (iTabHandle, ANALOG_ANALOG, dXArray, dYArray, 2, VAL_DOUBLE,
				   VAL_DOUBLE, VAL_FAT_LINE, VAL_NO_POINT, VAL_SOLID, 1, iColor);
}

/* FUNCTION *****************************************************************/
/**
SetDebugUirPafa
*
@brief Sets the debug panel in PASS/FAIL mode
*
@param iPanelHandle: Panel Handle (Input)
*
@return none
*****************************************************************************/

void SetDebugUirPafa (int iPanelHandle)
{
	int iTabHandle;

	GetPanelHandleFromTabPage(iPanelHandle, DEBUG_TAB, 0, &iTabHandle);
	//Analog
	SetCtrlAttribute (iTabHandle, ANALOG_VALUE, ATTR_VISIBLE, FALSE);
	SetCtrlAttribute (iTabHandle, ANALOG_UNIT, ATTR_VISIBLE, FALSE);
	SetCtrlAttribute (iTabHandle, ANALOG_ANALOG, ATTR_VISIBLE, FALSE);
	SetCtrlAttribute (iTabHandle, ANALOG_STRING, ATTR_VISIBLE, FALSE);
	//Histogramm
	GetPanelHandleFromTabPage(iPanelHandle, DEBUG_TAB, 1, &iTabHandle);
	SetCtrlAttribute (iTabHandle, GRAPH_GRAPH, ATTR_DIMMED, TRUE);
	//Graph
	GetPanelHandleFromTabPage(iPanelHandle, DEBUG_TAB, 2, &iTabHandle);
	SetCtrlAttribute (iTabHandle, HISTOGRAM_HISTOGRAMM, ATTR_DIMMED, TRUE);

}

/* FUNCTION *****************************************************************/
/**
SetDebugUirNum
*
@brief Sets the debug panel in numeric limit test mode
*
@param iPanelHandle: Panel Handle (Input)
*
@return none
*****************************************************************************/

void SetDebugUirNum (int iPanelHandle)
{
	int iTabHandle;

	GetPanelHandleFromTabPage(iPanelHandle, DEBUG_TAB, 0, &iTabHandle);
	//Analog
	SetCtrlAttribute (iTabHandle, ANALOG_VALUE, ATTR_VISIBLE, TRUE);
	SetCtrlAttribute (iTabHandle, ANALOG_UNIT, ATTR_VISIBLE, TRUE);
	SetCtrlAttribute (iTabHandle, ANALOG_ANALOG, ATTR_VISIBLE, TRUE);
	SetCtrlAttribute (iTabHandle, ANALOG_STRING, ATTR_VISIBLE, FALSE);
	//Histogramm
	GetPanelHandleFromTabPage(iPanelHandle, DEBUG_TAB, 1, &iTabHandle);
	SetCtrlAttribute (iTabHandle, GRAPH_GRAPH, ATTR_DIMMED, FALSE);
	//Graph
	GetPanelHandleFromTabPage(iPanelHandle, DEBUG_TAB, 2, &iTabHandle);
	SetCtrlAttribute (iTabHandle, HISTOGRAM_HISTOGRAMM, ATTR_DIMMED, FALSE);

}

/* FUNCTION *****************************************************************/
/**
SetDebugUirString
*
@brief Sets the debug panel in string value test mode
*
@param iPanelHandle: Panel Handle (Input)
*
@return none
*****************************************************************************/

void SetDebugUirString (int iPanelHandle)
{
	int iTabHandle;

	GetPanelHandleFromTabPage(iPanelHandle, DEBUG_TAB, 0, &iTabHandle);
	//Analog
	SetCtrlAttribute (iTabHandle, ANALOG_VALUE, ATTR_VISIBLE, FALSE);
	SetCtrlAttribute (iTabHandle, ANALOG_UNIT, ATTR_VISIBLE, FALSE);
	SetCtrlAttribute (iTabHandle, ANALOG_ANALOG, ATTR_VISIBLE, FALSE);
	SetCtrlAttribute (iTabHandle, ANALOG_STRING, ATTR_VISIBLE, TRUE);
	//Histogramm
	GetPanelHandleFromTabPage(iPanelHandle, DEBUG_TAB, 1, &iTabHandle);
	SetCtrlAttribute (iTabHandle, GRAPH_GRAPH, ATTR_DIMMED, FALSE);
	//Graph
	GetPanelHandleFromTabPage(iPanelHandle, DEBUG_TAB, 2, &iTabHandle);
	SetCtrlAttribute (iTabHandle, HISTOGRAM_HISTOGRAMM, ATTR_DIMMED, FALSE);

}

/* FUNCTION *****************************************************************/
/**
InsertVariants
*
@brief Insert the variants in the opertor gui
*
@param pPointer: bench pointer that contains the variants (Input)
@param PanelHandle: Panel Handle (Input)
@param data: Thread date for gui (Input)
*
@return none
*****************************************************************************/

void InsertVariants (BENCH_STRUCT *pPointer, int PanelHandle, void *data)
{
	//VBOOL bValue;
	int iCountOfVariants;
	int menuBar_ID;
	int SubMenu_ID;
	int Item_ID;
	int idx;

	char cCompleteVariantName[1024];
	char cBuffer[1024];

	char *cVariantName;
	char *cVariantContent;

	menuBar_ID = GetPanelMenuBar (PanelHandle);
	//Check variants
	if (pPointer->iVariantsEnabled)
	{

		SubMenu_ID = NewMenu (menuBar_ID, "Variants", -1);
		for (idx = 0; idx < pPointer->iVariantsEnabled; idx++)
		{
			Item_ID = NewMenuItem (menuBar_ID, SubMenu_ID, pPointer->cVariants[idx], -1, 0, ChangeVariant, data);
			InsertListItem (PanelHandle, OP_SINGLE_VARIANT_RING, -1, pPointer->cVariants[idx], pPointer->cVariants[idx]);
		}

	}
}

/* FUNCTION *****************************************************************/
/**
InsertVariantsDebug
*
@brief Insert the variants in the debug gui
*
@param pPointer: bench pointer that contains the variants (Input)
@param PanelHandle: Panel Handle (Input)
@param data: Thread date for gui (Input)
*
@return none
*****************************************************************************/

void InsertVariantsDebug (BENCH_STRUCT *pPointer, int PanelHandle, void *data)
{
	int iCountOfVariants;
	int menuBar_ID;
	int SubMenu_ID;
	int Item_ID;
	int idx;

	char cCompleteVariantName[1024];
	char cBuffer[1024];

	char *cVariantName;
	char *cVariantContent;

	menuBar_ID = GetPanelMenuBar (PanelHandle);
	//Check variants
	if (pPointer->iVariantsEnabled)
	{

		SubMenu_ID = NewMenu (menuBar_ID, "Variants", -1);
		for (idx = 0; idx < pPointer->iVariantsEnabled; idx++)
		{
			Item_ID = NewMenuItem (menuBar_ID, SubMenu_ID, pPointer->cVariants[idx], -1, 0, ChangeVariantDebug, data);
			if (idx == 0)
			{
				SetMenuBarAttribute (menuBar_ID, Item_ID, ATTR_CHECKED, TRUE);
				sprintf(pPointer->cSelectedVariant, pPointer->cVariants[idx]);
			}
		}

	}
}

/* FUNCTION *****************************************************************/
/**
QuitThread
*
@brief Debug adn Operator panel - Quit gui
*
@param callbackData: Callback data, means thread data (Input)
*
@return none
*****************************************************************************/

void CVICALLBACK QuitThread(void *callbackData)
{
	QuitUserInterface(0);
}

/* FUNCTION *****************************************************************/
/**
iReadFixtureStrokeFile
*
@brief Read fixture stroke counts from file
*
@param cFileName: Filepath and name to cfg file (Input)
@param iFixtureCode: Fixture code to read values (Input)
@param iActualCounts: Actual stroke counts (Output)
@param iYellowLimit: Limit for yellow led in gui (Output)
@param iRedLimit: Limit for red led in gui (Output)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int iReadFixtureStrokeFile (char *cFileName, int iFixtureCode, int *iActualCounts, int *iYellowLimit, int *iRedLimit)
{
	int iError, iSectionExists;
	char cSection[1024];
	IniText pIniFileHandle;

	pIniFileHandle = Ini_New (0);
	if (!pIniFileHandle) return -1;

	iError = Ini_ReadFromFile (pIniFileHandle, cFileName);
	if (iError)
	{
		Ini_Dispose (pIniFileHandle);
		return -1;
	}

	sprintf (cSection, "FixtureCode%i", iFixtureCode);
	iSectionExists = Ini_SectionExists (pIniFileHandle, cSection);
	if (!iSectionExists)
	{
		Ini_Dispose (pIniFileHandle);
		return TSPAN_ERR_FIXTURECODENOTFOUND;
	}

	iError = Ini_GetInt (pIniFileHandle, cSection, "ActualStrokeCounts", iActualCounts);
	if (iError <= 0)
	{
		Ini_Dispose (pIniFileHandle);
		return TSPAN_ERR_ACTUALSTROKECOUNTSNOTFOUND;
	}

	iError = Ini_GetInt (pIniFileHandle, cSection, "YellowLimit", iYellowLimit);
	if (iError <= 0)
	{
		Ini_Dispose (pIniFileHandle);
		return TSPAN_ERR_YELLOWLIMITNOTFOUND;
	}

	iError = Ini_GetInt (pIniFileHandle, cSection, "RedLimit", iRedLimit);
	if (iError <= 0)
	{
		Ini_Dispose (pIniFileHandle);
		return TSPAN_ERR_REDLIMITNOTFOUND;
	}

	Ini_Dispose (pIniFileHandle);


	return 0;
}

/* FUNCTION *****************************************************************/
/**
iWriteFixtureStrokeFile
*
@brief Write fixture stroke counts to file
*
@param cFileName: Filepath and name to cfg file (Input)
@param iFixtureCode: Fixture code to read values (Input)
@param iActualCounts: Actual stroke counts (Input)
*
@return Errorcode 0 -> no error, > 0 warnings, < 0 errors
*****************************************************************************/

int iWriteFixtureStrokeFile (char *cFileName, int iFixtureCode, int iActualCounts)
{
	int iError, iSectionExists, iReadOnly, iSystem, iHidden, iArchive, iResult;
	char cSection[1024];
	IniText pIniFileHandle;
	double dTimeOut;
	double dActualTime;
	
	
	iResult = GetFileAttrs (cFileName, &iReadOnly, &iSystem, &iHidden, &iArchive);
	if (iResult) return -1;
	
	//Maximum waiting time is 10 seconds
	dTimeOut = 10.0;
	dActualTime = 0.0;
	while ((iReadOnly == 0) && (dActualTime <= dTimeOut))
	{
		Delay(0.5);
		dActualTime = dActualTime + 0.5;
		iResult = GetFileAttrs (cFileName, &iReadOnly, &iSystem, &iHidden, &iArchive);  
		if (iResult) return -1;
	}
	
	if (iReadOnly == 0)
	{
		return -1;		
	}
	
	iResult = SetFileAttrs (cFileName, 0, 0, 0, 0);    
	if (iResult) return -1;
		
	pIniFileHandle = Ini_New (0);
	if (!pIniFileHandle) return -1;

	iError = Ini_ReadFromFile (pIniFileHandle, cFileName);
	if (iError)
	{
		Ini_Dispose (pIniFileHandle);
		return -1;
	}

	sprintf (cSection, "FixtureCode%i", iFixtureCode);
	iSectionExists = Ini_SectionExists (pIniFileHandle, cSection);
	if (!iSectionExists)
	{
		Ini_Dispose (pIniFileHandle);
		return -1;
	}

	iError = Ini_PutInt (pIniFileHandle, cSection, "ActualStrokeCounts", iActualCounts);
	if (iError < 0)
	{
		Ini_Dispose (pIniFileHandle);
		return -1;
	}

	iError = Ini_WriteToFile (pIniFileHandle, cFileName);
	if (iError)
	{
		Ini_Dispose (pIniFileHandle);
		return -1;
	}


	Ini_Dispose (pIniFileHandle);

	iResult = SetFileAttrs (cFileName, 1, 0, 0, 0);
	if (iResult) return -1;
	
	return 0;
}

/* FUNCTION *****************************************************************/
/**
vOrderGUIElementsAfterResize
*
@brief Resize all Gui elements if user sizing the panel
*
@param panel: Panel handle, normaly operator panel handle (Input)
*
@return nome
*****************************************************************************/

void vOrderGUIElementsAfterResize (int panel)
{
	int iPanelHeight;
	int iPanelWidth;
	int iTextPointSize;
	int iTabPanelHandle;
	int idx, iTop;

	char cTextBuffer[1024];

	GetPanelAttribute (panel, ATTR_HEIGHT, &iPanelHeight);
	GetPanelAttribute (panel, ATTR_WIDTH, &iPanelWidth);
	SetCtrlAttribute (panel, OP_SINGLE_TITEL2, ATTR_LEFT, iPanelWidth - 79);
	SetCtrlAttribute (panel, OP_SINGLE_TEXTMSG_2, ATTR_LEFT, iPanelWidth - 75);
	SetCtrlAttribute (panel, OP_SINGLE_DECORATION, ATTR_TOP, iPanelHeight - 27);
	SetCtrlAttribute (panel, OP_SINGLE_DECORATION, ATTR_WIDTH, iPanelWidth - 6);
	SetCtrlAttribute (panel, OP_SINGLE_DECORATION_2, ATTR_WIDTH, iPanelWidth - 6);
	SetCtrlAttribute (panel, OP_SINGLE_PRODUCTNAME, ATTR_TOP, 25);
	SetCtrlAttribute (panel, OP_SINGLE_PRODUCTNAME, ATTR_WIDTH, iPanelWidth - 100);
	SetCtrlAttribute (panel, OP_SINGLE_SEQFILEPATH, ATTR_WIDTH, iPanelWidth - 100);
	SetCtrlAttribute (panel, OP_SINGLE_YIELD, ATTR_TOP, iPanelHeight - 22);
	SetCtrlAttribute (panel, OP_SINGLE_TICKET, ATTR_TOP, iPanelHeight - 22);
	SetCtrlAttribute (panel, OP_SINGLE_REPORT, ATTR_TOP, iPanelHeight - 22);
	SetCtrlAttribute (panel, OP_SINGLE_STROKEINDICATOR, ATTR_TOP, iPanelHeight - 22);
	SetCtrlAttribute (panel, OP_SINGLE_MSA_INDICATOR, ATTR_TOP, iPanelHeight - 22);
	SetCtrlAttribute (panel, OP_SINGLE_OPERATOR, ATTR_TOP, iPanelHeight - 22);
	SetCtrlAttribute (panel, OP_SINGLE_OPERATOR, ATTR_LEFT, iPanelWidth - 217);
	SetCtrlAttribute (panel, OP_SINGLE_ACTUALTESTSTEP, ATTR_TOP, iPanelHeight - 65);
	SetCtrlAttribute (panel, OP_SINGLE_ACTUALTESTSTEP, ATTR_WIDTH, iPanelWidth - 30);
	SetCtrlAttribute (panel, OP_SINGLE_ACTUALTESTSTEP_DESC, ATTR_TOP, iPanelHeight - 80);

	SetCtrlAttribute (panel, OP_SINGLE_PRODUCTNAME, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
	SetCtrlAttribute (panel, OP_SINGLE_SEQFILEPATH, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
	SetCtrlAttribute (panel, OP_SINGLE_TITEL2, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
	SetCtrlAttribute (panel, OP_SINGLE_TEXTMSG_2, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
	
	SetCtrlAttribute (panel, OP_SINGLE_SEQFILEPATH, ATTR_TOP, 58);
	SetCtrlAttribute (panel, OP_SINGLE_SEQFILEPATH, ATTR_LEFT, 15);
	
	//Running / Wait / PASS / FAIL
	SetCtrlAttribute (panel, OP_SINGLE_TESTSTATUS_MSG, ATTR_TOP, iPanelHeight - 123);
	SetCtrlAttribute (panel, OP_SINGLE_TESTSTATUS_MSG, ATTR_LEFT, 5);
	SetCtrlAttribute (panel, OP_SINGLE_TESTSTATUS_MSG, ATTR_WIDTH, (iPanelWidth - 50) / 3);
	SetCtrlAttribute (panel, OP_SINGLE_TESTSTATUS_MSG, ATTR_HEIGHT, 40);
	//SetCtrlAttribute (panel, OP_SINGLE_TESTSTATUS_MSG, ATTR_TEXT_BGCOLOR, VAL_RED);

	//Variante
	SetCtrlAttribute (panel, OP_SINGLE_VARIANT_RING, ATTR_TOP, iPanelHeight - 160);
	SetCtrlAttribute (panel, OP_SINGLE_VARIANT_RING, ATTR_LEFT, 5);
	SetCtrlAttribute (panel, OP_SINGLE_VARIANT_RING, ATTR_WIDTH, (iPanelWidth - 50) / 3);

	//Seriennummer
	SetCtrlAttribute (panel, OP_SINGLE_SERIAL, ATTR_TOP, iPanelHeight - 124);
	SetCtrlAttribute (panel, OP_SINGLE_SERIAL, ATTR_LEFT, iPanelWidth - (2*(iPanelWidth / 3)) + 8);
	SetCtrlAttribute (panel, OP_SINGLE_SERIAL, ATTR_WIDTH, (iPanelWidth - 50) / 3);

	//Run button
	SetCtrlAttribute (panel, OP_SINGLE_RUN_TEST, ATTR_TOP, iPanelHeight - 125);
	SetCtrlAttribute (panel, OP_SINGLE_RUN_TEST, ATTR_LEFT, iPanelWidth - (iPanelWidth / 3) + 8);
	SetCtrlAttribute (panel, OP_SINGLE_RUN_TEST, ATTR_WIDTH, (iPanelWidth - 50) / 3);
	SetCtrlAttribute (panel, OP_SINGLE_RUN_TEST, ATTR_HEIGHT, 42);

	//Tab
	SetCtrlAttribute (panel, OP_SINGLE_TAB, ATTR_HEIGHT, iPanelHeight - 285 + 19);
	SetCtrlAttribute (panel, OP_SINGLE_TAB, ATTR_WIDTH, iPanelWidth - 6);


	GetPanelHandleFromTabPage (panel, OP_SINGLE_TAB, 1, &iTabPanelHandle);
	//table
	SetCtrlAttribute (iTabPanelHandle, TABPANEL_2_TABLE, ATTR_HEIGHT, iPanelHeight - 285);
	SetCtrlAttribute (iTabPanelHandle, TABPANEL_2_TABLE, ATTR_WIDTH, iPanelWidth - 10);
	GetCtrlAttribute (iTabPanelHandle, TABPANEL_2_TABLE, ATTR_TEXT_POINT_SIZE, &iTextPointSize);

	GetPanelHandleFromTabPage (panel, OP_SINGLE_TAB, 0, &iTabPanelHandle);
	
	//Report all test steps
	SetCtrlAttribute (iTabPanelHandle, TABPANEL_TESTCASEPATHWAY, ATTR_HEIGHT, iPanelHeight - 285);
	if (giErrorLogWindowEnabled) 
	{
		SetCtrlAttribute (iTabPanelHandle, TABPANEL_TESTCASEPATHWAY, ATTR_WIDTH, (iPanelWidth / 2));
	} else {
		SetCtrlAttribute (iTabPanelHandle, TABPANEL_TESTCASEPATHWAY, ATTR_WIDTH, iPanelWidth - 10);
	}
	GetCtrlAttribute (iTabPanelHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_POINT_SIZE, &iTextPointSize);
	
	if (giErrorLogWindowEnabled) 
	{
		GetCtrlAttribute (iTabPanelHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TOP, &iTop);   
		SetCtrlAttribute (iTabPanelHandle, TABPANEL_TESTERRORLOG, ATTR_TOP, iTop);   
		SetCtrlAttribute (iTabPanelHandle, TABPANEL_TESTERRORLOG, ATTR_HEIGHT, iPanelHeight - 285);
		SetCtrlAttribute (iTabPanelHandle, TABPANEL_TESTERRORLOG, ATTR_WIDTH, (iPanelWidth / 2) - 14);
		SetCtrlAttribute (iTabPanelHandle, TABPANEL_TESTERRORLOG, ATTR_LEFT, (iPanelWidth / 2) + 4);
		SetCtrlAttribute (iTabPanelHandle, TABPANEL_SPLITTER, ATTR_LEFT, (iPanelWidth / 2));      
		SetCtrlAttribute (iTabPanelHandle, TABPANEL_SPLITTER, ATTR_HEIGHT, iPanelHeight - 295);  
	}

	GetCtrlAttribute (iTabPanelHandle, TABPANEL_TESTCASEPATHWAY, ATTR_VISIBLE_LINES, &giVisibleLines);
	if (iTextPointSize < 13)
	{
		SetCtrlAttribute (panel, OP_SINGLE_TAB, ATTR_HEIGHT, (giVisibleLines * (iTextPointSize + 2)) + 30);
	}
	else
	{
		SetCtrlAttribute (panel, OP_SINGLE_TAB, ATTR_HEIGHT, (giVisibleLines * (iTextPointSize + 3)) + 30);
	}

	return;
}

/* FUNCTION *****************************************************************/
/**
pAppendTestStep
*
@brief Append a test step to test step list for showing last 50 test runs
*
@param pFirstTestStep: Pointer to the first element of the list (Input)
@param cTestCaseType: Test case type, 0 -> Numeric, 1 -> PassFail, 2 -> String (Input)
@param cStepName: Name of step (Input)
@param cComment: Comment of step (Input)
@param cLogMessage: Log message of step (Input)
@param dMeasurement: Measurement value (if numeric) (Input)
@param dLowerLimit: Lower limit (if numeric) (Input)
@param dUpperLimit: Upper limit (if numeric) (Input)
@param cUnit: String unit (if numeric) (Input)
@param cCompType: Comparision tyoe (if numeric or string) (Input)
@param cFormat: Format string (if numeric) (Input)
@param iResult: Test result 0 -> FAIL, 1 -> PASS (Input)
@param cStringToCompare: String to compare (if string test) (Input)
@param cExpectedString: Expected string (if string test) (Input)
*
@return Pointer to test step list, NULL if an error occurred
*****************************************************************************/

TestStep *pAppendTestStep (TestStep *pFirstTestStep, char cTestCaseType, char *cStepName, char *cComment, char *cLogMessage,
						   double dMeasurement, double dLowerLimit, double dUpperLimit, char *cUnit, char *cCompType, char *cFormat,
						   int iResult, char *cStringToCompare, char *cExpectedString)
{
	TestStep *pNextPointer;

	int idx, idy;

	// 0 -> Numeric, 1 -> PassFail, 2 -> String
	if (!pFirstTestStep)
	{
		pFirstTestStep = malloc (sizeof(TestStep));
		if (!pFirstTestStep)
		{
			//Hier noch Fehlerauswertung einbauen....
			return NULL;
		}
		pFirstTestStep->cTestCaseType = cTestCaseType;
		if (cStepName) sprintf(pFirstTestStep->cStepName, cStepName);
		if (cComment) sprintf(pFirstTestStep->cComment, cComment);
		if (cLogMessage) sprintf(pFirstTestStep->cLogMessage, cLogMessage);
		switch (cTestCaseType)
		{
			case 0:
				pFirstTestStep->dMeasurement = dMeasurement;
				pFirstTestStep->dLowerLimit = dLowerLimit;
				pFirstTestStep->dUpperLimit = dUpperLimit;
				if (cUnit) sprintf(pFirstTestStep->cUnit, cUnit);
				if (cCompType) sprintf(pFirstTestStep->cCompType, cCompType);
				if (cFormat)
				{
					idy = 0;
					for (idx = 0; idx < strlen(cFormat); idx++)
					{
						pFirstTestStep->cFormat[idy] = cFormat[idx];
						/*if (pFirstTestStep->cFormat[idy] == '%')
						{
							idy++;
							pFirstTestStep->cFormat[idy] = '%';
						}*/
						idy++;
					}
					pFirstTestStep->cFormat[idy] = '\0';
				}

				pFirstTestStep->iResult = iResult;
				sprintf(pFirstTestStep->cStringToCompare, "");
				sprintf(pFirstTestStep->cExpectedString, "");
				break;
			case 1:
				pFirstTestStep->dMeasurement = 0.0;
				pFirstTestStep->dLowerLimit = 0.0;
				pFirstTestStep->dUpperLimit = 0.0;
				sprintf(pFirstTestStep->cUnit, "");
				sprintf(pFirstTestStep->cCompType, "");
				sprintf(pFirstTestStep->cFormat, "");
				pFirstTestStep->iResult = iResult;
				sprintf(pFirstTestStep->cStringToCompare, "");
				sprintf(pFirstTestStep->cStringToCompare, "");
				break;
			case 2:
				pFirstTestStep->dMeasurement = 0.0;
				pFirstTestStep->dLowerLimit = 0.0;
				pFirstTestStep->dUpperLimit = 0.0;
				sprintf(pFirstTestStep->cUnit, "");
				if (cCompType) sprintf(pFirstTestStep->cCompType, cCompType);
				sprintf(pFirstTestStep->cFormat, "");
				pFirstTestStep->iResult = iResult;
				if (cStringToCompare) sprintf(pFirstTestStep->cStringToCompare, cStringToCompare);
				if (cExpectedString) sprintf(pFirstTestStep->cExpectedString, cExpectedString);
				break;

		}
		pFirstTestStep->pNext = NULL;
	}
	else
	{
		pNextPointer = pFirstTestStep;
		while (pNextPointer->pNext)
		{
			pNextPointer = pNextPointer->pNext;
		}
		pNextPointer->pNext = malloc (sizeof(TestStep));
		if (!(pNextPointer->pNext))
		{
			//Hier noch Fehlerauswertung einbauen....
			return NULL;
		}
		pNextPointer = pNextPointer->pNext;
		pNextPointer->cTestCaseType = cTestCaseType;
		if (cStepName) sprintf(pNextPointer->cStepName, cStepName);
		if (cComment) sprintf(pNextPointer->cComment, cComment);
		if (cLogMessage) sprintf(pNextPointer->cLogMessage, cLogMessage);
		switch (cTestCaseType)
		{
			case 0:
				pNextPointer->dMeasurement = dMeasurement;
				pNextPointer->dLowerLimit = dLowerLimit;
				pNextPointer->dUpperLimit = dUpperLimit;
				if (cUnit) sprintf(pNextPointer->cUnit, cUnit);
				if (cCompType) sprintf(pNextPointer->cCompType, cCompType);
				if (cFormat)
				{
					idy = 0;
					for (idx = 0; idx < strlen(cFormat); idx++)
					{
						pNextPointer->cFormat[idy] = cFormat[idx];
						/*if (pNextPointer->cFormat[idy] == '%')
						{
							idy++;
							pNextPointer->cFormat[idy] = '%';
						}*/
						idy++;
					}
					pNextPointer->cFormat[idy] = '\0';
				}
				pNextPointer->iResult = iResult;
				sprintf(pNextPointer->cStringToCompare, "");
				sprintf(pNextPointer->cExpectedString, "");
				break;
			case 1:
				pNextPointer->dMeasurement = 0.0;
				pNextPointer->dLowerLimit = 0.0;
				pNextPointer->dUpperLimit = 0.0;
				sprintf(pNextPointer->cUnit, "");
				sprintf(pNextPointer->cCompType, "");
				sprintf(pNextPointer->cFormat, "");
				pNextPointer->iResult = iResult;
				sprintf(pNextPointer->cStringToCompare, "");
				sprintf(pNextPointer->cExpectedString, "");
				break;
			case 2:
				pNextPointer->dMeasurement = 0.0;
				pNextPointer->dLowerLimit = 0.0;
				pNextPointer->dUpperLimit = 0.0;
				sprintf(pNextPointer->cUnit, "");
				if (cCompType) sprintf(pNextPointer->cCompType, cCompType);
				sprintf(pNextPointer->cFormat, "");
				pNextPointer->iResult = iResult;
				if (cStringToCompare) sprintf(pNextPointer->cStringToCompare, cStringToCompare);
				if (cExpectedString) sprintf(pNextPointer->cExpectedString, cExpectedString);
				break;

		}
		pNextPointer->pNext = NULL;

	}
	return pFirstTestStep;
}

/* FUNCTION *****************************************************************/
/**
vFreeAllTestResults
*
@brief Free all alocated memory for last 50 test runs / results
*
@param pFirst: Pointer to the first element of the list (Input)
*
@return None
*****************************************************************************/

void vFreeAllTestResults (TestStep *pFirst)
{
	TestStep *pNext;
	TestStep *pToFree;

	pNext = pFirst;
	while (pNext)
	{
		pToFree = pNext;
		pNext = pNext->pNext;
		free(pToFree);
	}
}


/* FUNCTION *****************************************************************/
/**
vChangePointToComma
*
@brief Change all points in given string into commas
*
@param cValue: String with poinst, normally numeric values (Input)
@param iLength: Length of goven string (Input)
*
@return None
*****************************************************************************/

void vChangePointToComma(char* cValue, int iLength)
{
  int idx;
  for (idx = 0; idx < iLength; idx++)
  {
    if (cValue[idx] == '.')
    {
      cValue[idx] = ',';
    }
  }
}

/* FUNCTION *****************************************************************/
/**
cOwnDateStr
*
@brief Generates a date string
*
@return Date string in format 23.10.2010
*****************************************************************************/

char *cOwnDateStr (void)
{
	static char timeStr[20];
	
	time_t curTime = time(0);
	strftime(timeStr, sizeof(timeStr), "%d.%m.%Y", localtime(&curTime));
	
	return timeStr;
}

/* FUNCTION *****************************************************************/
/**
KillTabInStepName
*
@brief Find tabs in given string and replace it with space char. Special function
to format the name of steps. Was ness tabs was used in name of steps in teststand to
format special step names.
*
@param cInputString: String (NameOfStep) that can contains tabs (Input)
@param cOutputString: After correct execution that string is equal to input without tabs
@param iLength: Length of given string (Input)
*
@return Found tabs
*****************************************************************************/
int iKillTabInStepName (char *cInputString, char *cOutputString, int iLength)
{
	int idx, idy;
	int iFoundTabs;
	
	iFoundTabs = 0;
	idy = 0;
	for (idx = 0; idx < iLength; idx++)
	{
		if ((cInputString[idx] == '\\') && (cInputString[idx+1] == 't'))
		{
			cOutputString[idy] = ' ';
			iFoundTabs++;
			idx++;
			idy++;
		} else {
			cOutputString[idy] = cInputString[idx];
			idy++;
		}
	}
	cOutputString[idy] = '\0';
	return iFoundTabs;
}
