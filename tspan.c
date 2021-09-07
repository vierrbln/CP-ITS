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

@file tspan.c
 *
@brief All exported functions to control teststand control panel
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

/* INCLUDE FILES **************************************************************/

#include "definitions.h"
#include "tspan.h"

/* GLOBAL CONSTANT DEFINITIONS ************************************************/

#ifndef COINIT_MULTITHREADED
#define COINIT_MULTITHREADED  0x0      // OLE calls objects on any thread.
#endif

/* GLOBAL VARIABLES DEFINITION ************************************************/

/* LOCAL DEFINES *************************************************/

/* LOCAL FUNCTION DEFINITIONS *************************************************/

/* LOCAL CONSTANT DEFINITIONS *************************************************/

/* LOCAL TYPE DEFINITIONS *****************************************************/

/* LOCAL MACRO DEFINITIONS ****************************************************/

/* LOCAL FUNCTION DECLARATIONS ************************************************/

/* EXPORTED FUNCTION DEFINITIONS **********************************************/

/* FUNCTION *****************************************************************/
/**
DllMain
*
@brief DLL Main function
*
@param hinstDLL: standard
@param fdwReason: standard
@param lpvReserved: standard
*
@return Exit error codes
*****************************************************************************/

int __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	int idx;

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:

			CA_InitActiveXThreadStyleForCurrentThread (0, COINIT_APARTMENTTHREADED);
			InitializeCriticalSection(&CriticalSection);
			hThreadReady = CreateEvent(NULL, TRUE, FALSE, NULL);
			hinstDLLOwn = hinstDLL;

			//Initalisierung
			for (giActualTestResult = 0; giActualTestResult < 50; giActualTestResult++)
			{
				giAllTestResults[giActualTestResult] = malloc (sizeof(TestResult));
				giAllTestResults[giActualTestResult]->cTestResult = -1;
				sprintf (giAllTestResults[giActualTestResult]->cSerial, "");
				sprintf (giAllTestResults[giActualTestResult]->cStartTime, "");
				sprintf (giAllTestResults[giActualTestResult]->cStopTime, "");
				sprintf (giAllTestResults[giActualTestResult]->cStartDate, "");
				sprintf (giAllTestResults[giActualTestResult]->cStopDate, "");
				giAllTestResults[giActualTestResult]->dExecutionTime = 0.0;
				giAllTestResults[giActualTestResult]->pFirst = NULL;
			}
			giActualTestResult = 0;

			//init all test results
			//GetModuleDir(__CVIUserHInst, cDLLPATH);
			if (hThreadReady == NULL)
			{
				return FALSE;
			}
			break;

		case DLL_PROCESS_DETACH:
			
			break;
	}

	return 1;
}

/* FUNCTION *****************************************************************/
/**
DllEntryPoint
*
@brief DLL entry point function
*
@param hinstDLL: standard
@param fdwReason: standard
@param lpvReserved: standard
*
@return Exit error codes
*****************************************************************************/

int __stdcall DllEntryPoint(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID
							lpvReserved)
{
	/* Included for compatibility with Borland    */
	CA_InitActiveXThreadStyleForCurrentThread (0, COINIT_APARTMENTTHREADED);  
	return DllMain(hinstDLL, fdwReason, lpvReserved);
}

/* FUNCTION *****************************************************************/
/**
tspan_Setup
*
@brief Setup function of control pannel. Init all resources
*
@param sequenceContext: Teststand sequence context (Input)
@param pBenchName: Benchname specified in application.ini (See. GTSL, R&S ResMgr) (Input)
@param pResourceId: if op successful, resource ID can be used to use all other functions (Output)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_Setup(CAObjHandle sequenceContext, char *pBenchName, long
						   *pResourceId, short *pErrorOccurred, long *pErrorCode, char errorMessage[])
{
	char cSectionName[1024];
	char cTraceBuffer[1024];
	char *cFixtureCode;
	char cTempBuffer[1024];
	char cTempBuffer2[1024];
	char cSystemID[1024];

	char cProjectDirectory[1024];
	char cDriveName[1024];
	char cDirectory[1024];
	char cFileName[1024];
	LPTSTR lpVolumeNameBuffer; // volume name buffer
	DWORD nVolumeNameSize; // length of name buffer
	DWORD lpVolumeSerialNumber; // volume serial number
	DWORD lpMaximumComponentLength; // maximum file name length
	DWORD lpFileSystemFlags; // file system options
	LPTSTR lpFileSystemNameBuffer; // file system name buffer
	DWORD nFileSystemNameSize;
	BOOL error;
	DWORD errorCode;

	short sLicenseOK;

	int idx;
	int iConfirmation;

	int iFileExists;
	int iFileSize;
	int iStatus;

	long lBytesWritten;
	long lMatched;
	long lResourceType;
	long lTrace;

	IniText IniFileHandle;

	BENCH_STRUCT *pBench = NULL;

	ERRORINFO pTSErrorInfo;

	HRESULT hResult;

	*pResourceId = 0;

	/*---------------------------------------------------------------------/
	/   Allocate the resource:
	/     Check whether "pBenchName" can be found in the INI files and
	/     return a resource ID for the bench. This resource ID is the
	/     "ticket" for any subsequent action dealing with this bench.
	/---------------------------------------------------------------------*/
	RESMGR_Alloc_Resource(sequenceContext, pBenchName, pResourceId,
						  pErrorOccurred, pErrorCode, errorMessage);

	/*---------------------------------------------------------------------/
	/  Check for trace flag:
	/    The "Trace" key in the bench section is searched and its
	/    value is checked. The result is recorded in the static
	/    variable trace
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		RESMGR_Compare_Value(sequenceContext,  *pResourceId, "", RESMGR_KEY_TRACE,
							 "1", &lTrace, pErrorOccurred, pErrorCode, errorMessage);
	}
	if (!*pErrorOccurred)
	{
		RESMGR_Set_Trace_Flag(*pResourceId, lTrace);

		if (lTrace)
		{
			RESMGR_Trace(">>TSPAN_Setup begin");
			RESMGR_Trace("Tracing for tspan.dll enabled");
			sprintf(cTraceBuffer, "Bench name %s -> Resource ID %ld", pBenchName,
					*pResourceId);
			RESMGR_Trace(cTraceBuffer);
		}
	}

	/*---------------------------------------------------------------------/
	/   Check the resource type:
	/     The tspan library requires that pBenchName refers to a bench,
	/     not to a single device. It is always recommended to use a bench
	/     instead of a device, because a bench makes it easy to add
	/     a device in future and to work with alternative devices
	/     like a ....
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		RESMGR_Get_Resource_Type(sequenceContext,  *pResourceId, &lResourceType,
								 pErrorOccurred, pErrorCode, errorMessage);
		if (!*pErrorOccurred)
		{
			if (lResourceType != RESMGR_TYPE_BENCH)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_NOT_A_BENCH;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
		}
	}

	/*---------------------------------------------------------------------/
	/   Check the license:
	/      If correct license file not found, the tspan library runs in
	/      demo mode
	/---------------------------------------------------------------------*/
	/*if (!*pErrorOccurred)
	{
		//Get TSVP system ID
	   RESMGR_Read_ROM (sequenceContext, cSystemID, pErrorOccurred, pErrorCode, errorMessage);
	   //If an error occurred -> System is not available
	   //Try to get hard disc ID
	   if (*pErrorOccurred)
	   {
		   *pErrorOccurred = FALSE;
		   GetProjectDir( cProjectDirectory );
		   SplitPath( cProjectDirectory, cDriveName, cDirectory, cFileName );
		   strcat( cDriveName, "\\" );
		   //Allocate the necessary memory
		   lpVolumeNameBuffer = malloc( 256 );
		   lpFileSystemNameBuffer = malloc( 256 );
		   error = GetVolumeInformation( cDriveName,  // root directory
		   lpVolumeNameBuffer,  // volume name buffer
		   256,  // length of name buffer
			&lpVolumeSerialNumber,  // volume serial number
			&lpMaximumComponentLength,  // maximum file name length
			&lpFileSystemFlags,  // file system options
		   lpFileSystemNameBuffer,  // file system name buffer
		   256 );
		   if ( lpVolumeSerialNumber != 0 )
		   {
			  sprintf( cSystemID, "0x%X", lpVolumeSerialNumber );
		   }
		   else
		   {
			  if ( !DEBUG )
			  {
				 MessagePopup( "CP-ITS ControlPanel - Error", "The installation of AE-ITS ApplicationEditor is wrong. You cannot work on Flash Drives and so on." );
				 *pErrorOccurred = TRUE;
			  }
		   }

	   }
	   //Check for valid lic files
	   if (!*pErrorOccurred)
	   {
		   sLicenseOK = lc_CheckLicense ("CP-ITS", cSystemID);
		   if (!sLicenseOK)
		   {
			   sLicenseOK = lc_CheckLicense ("SW-ITS", cSystemID);
		   }

	   }
	}*/

	sLicenseOK = TRUE;

	/*---------------------------------------------------------------------/
	/   Allocate a memory block:
	/     The tspan library uses a structure to store some private
	/     information along with the resource ID. This information can
	/     be retrieved in subsequent calls to the measurement functions.
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		RESMGR_Alloc_Memory(sequenceContext,  *pResourceId, sizeof(BENCH_STRUCT),
							(void **)(&pBench), pErrorOccurred, pErrorCode, errorMessage);
		if (! *pErrorOccurred)
		{
			/* set the memory block owner field to a unique value
			which identifies the ENOCEAN library as the owner of the memory.
			 */
			pBench->iOwner = TSPAN_ERR_BASE;

			/* set default values */
			if (sLicenseOK)
			{
				sprintf(cTraceBuffer, "Valid license found! (System ID: %s)", cSystemID);
				pBench->iDemoMode = FALSE;
			}
			if (!sLicenseOK)
			{
				sprintf(cTraceBuffer, "Demo mode is enabled, not valid license found! (System ID: %s)", cSystemID);
				pBench->iDemoMode = FALSE;
			}

			if (lTrace) RESMGR_Trace(cTraceBuffer);
			pBench->iSimulation = FALSE;
			pBench->iActualPanelHandle = 0;
		}
	}

	/*---------------------------------------------------------------------/
	/   Check for simulation flag:
	/     The "Simulation" key in the bench section is searched and its
	/     value is checked. The result is recorded in the memory block.
	/     Any other library-specific information like calibration,
	/     path for calibration files etc. may be handled the same way
	/     (not shown in the example).
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		RESMGR_Compare_Value(sequenceContext,  *pResourceId, "",
							 RESMGR_KEY_SIMULATION, "1", &lMatched, pErrorOccurred, pErrorCode,
							 errorMessage);
		if (! *pErrorOccurred)
		{
			if (lMatched)
			{
				pBench->iSimulation = TRUE;
				if (lTrace)
				{
					RESMGR_Trace("Simulation is enabled!");
				}
			}
		}
	}

	/*---------------------------------------------------------------------/
	/   Check for serial is mandatory flag
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for serial is mandatory flag");
		}
		sprintf(cSectionName, "bench->%s", pBenchName);
		RESMGR_Get_Key_Value(0, 1, cSectionName, "SerialIsMandatory", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = TSPAN_ERR_SERIALSETTINGNOTFOUND;
			formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
		}
		else
		{
			lBytesWritten = sscanf (cTempBuffer, "%i", &pBench->SerialIsMandatory);
			if (lBytesWritten != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_WRONGSERIALSETTING;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if ((pBench->SerialIsMandatory<0) || (pBench->SerialIsMandatory>2))
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_WRONGSERIALSETTING;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "SerialIsMandatory = %i", pBench->SerialIsMandatory);
			RESMGR_Trace(cTraceBuffer);
		}
	}

	/*---------------------------------------------------------------------/
	/   Check for serial length flag
	/      If serial is mandatory == 2 serial length must be checked
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred && (pBench->SerialIsMandatory == 2))
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for serial length flag");
		}
		sprintf(cSectionName, "bench->%s", pBenchName);
		RESMGR_Get_Key_Value(0, 1, cSectionName, "SerialLength", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = TSPAN_ERR_SERIALLENGTHENTRIENOTFOUND;
			formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
		}
		else
		{
			lBytesWritten = sscanf (cTempBuffer, "%i", &pBench->SerialLength);
			if (lBytesWritten <= 0)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_SERIALLENGTHENTRIENOTFOUND;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			else
			{
				pBench->SerialIsMandatory = abs(pBench->SerialLength);
			}
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "SerialLength = %i", pBench->SerialLength);
			RESMGR_Trace(cTraceBuffer);
		}
	}

	
	/*---------------------------------------------------------------------/
	/   Check for run button flag
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for run button flag");
		}
		sprintf(cSectionName, "bench->%s", pBenchName);
		RESMGR_Get_Key_Value(0, 1, cSectionName, "RunButtonIsDisabled", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (lBytesWritten)
		{
			lBytesWritten = sscanf (cTempBuffer, "%i", &pBench->RunButtonDisabled);
			if (lBytesWritten != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_WRONGRUNBUTTONSETTING;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "RunButtonIsDisabled = %i", pBench->RunButtonDisabled);
			RESMGR_Trace(cTraceBuffer);
		}
	}

	/*---------------------------------------------------------------------/
	/   Check for disable execution time flag  (Optional)
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for disable execution time flag");
		}
		sprintf(cSectionName, "bench->%s", pBenchName);
		RESMGR_Get_Key_Value(0, 1, cSectionName, "DisableExecutionTime", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (lBytesWritten)
		{
			lBytesWritten = sscanf (cTempBuffer, "%i", &pBench->DisableExecutionTime);
			if (lBytesWritten != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_WRONGRUNBUTTONSETTING;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
		} else {
			pBench->DisableExecutionTime = 0;
			*pErrorOccurred = FALSE;
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "DisableExecutionTime = %i", pBench->DisableExecutionTime);
			RESMGR_Trace(cTraceBuffer);
		}
	}


	/*---------------------------------------------------------------------/
	/   Check for title bar text
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for titlebar text");
		}
		sprintf(cSectionName, "bench->%s", pBenchName);
		RESMGR_Get_Key_Value(0, 1, cSectionName, "TitlebarText", pBench
							 ->cTitlebarText, 1024, &lBytesWritten, pErrorOccurred, pErrorCode,
							 errorMessage);
		if (!lBytesWritten)
		{
			*pErrorCode = TSPAN_ERR_TITLEBARTEXT_NOT_FOUND;
			formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
		}
		if(pBench->iDemoMode) sprintf(pBench->cTitlebarText, "hatec Prüftechnik OHG");
		if (lTrace)
		{
			sprintf(cTraceBuffer, "Title bar text: %s", pBench->cTitlebarText);
			RESMGR_Trace(cTraceBuffer);
		}
	}

	/*---------------------------------------------------------------------/
	/   Check for product name
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for product name");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "ProductName", pBench
							 ->cProductName, 1024, &lBytesWritten, pErrorOccurred, pErrorCode,
							 errorMessage);
		if (!lBytesWritten)
		{
			*pErrorCode = TSPAN_ERR_PRODUCTNAMETEXT_NOT_FOUND;
			formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
		}
		if(pBench->iDemoMode) sprintf(pBench->cProductName, "SW-ITS is running in demo mode");
		if (lTrace)
		{
			sprintf(cTraceBuffer, "Product name text: %s", pBench->cProductName);
			RESMGR_Trace(cTraceBuffer);
		}
	}

	/*---------------------------------------------------------------------/
	/   Check for panel type
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for panel type");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "PanelType", pBench->cPanelType,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			*pErrorCode = TSPAN_ERR_PANELTYPE_NOT_FOUND;
			formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
		} else {
			if(!strcmp(pBench->cPanelType, "SINGLE_EXT"))
			{	
				giErrorLogWindowEnabled = TRUE;
			} else {
				giErrorLogWindowEnabled = FALSE;
			}
		}
	}

	/*---------------------------------------------------------------------/
	/   Check panel type entry
	/---------------------------------------------------------------------*/

	if (!*pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check panel type entry");
		}
		if (!stricmp(pBench->cPanelType, "SINGLE"))
		{
			//do nothing
		} else  if (!stricmp(pBench->cPanelType, "SINGLE_EXT"))
		{
			//do nothing
		} else {
			*pErrorOccurred = TRUE;
			*pErrorCode = TSPAN_ERR_PANELTYPEENTRYNOTVALID;
			formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "Panel type: %s", pBench->cPanelType);
			RESMGR_Trace(cTraceBuffer);
		}

	}


	/*---------------------------------------------------------------------/
	/   Check for debug panel position
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for debug panel position");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "DebugPanelPosition", pBench
							 ->cDebugPosition, 1024, &lBytesWritten, pErrorOccurred, pErrorCode,
							 errorMessage);
		if (!lBytesWritten)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = TSPAN_ERR_DEBUGPANELPOSITION_NOT_FOUND;
			formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
		}
	}

	/*---------------------------------------------------------------------/
	/   Check debug panel position entry
	/---------------------------------------------------------------------*/

	if (!*pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check debug panel position entry");
		}
		if ((stricmp(pBench->cDebugPosition, "TOPLEFT")) && (stricmp(pBench
				->cDebugPosition, "TOPRIGHT")) && (stricmp(pBench->cDebugPosition,
						"BOTTOMLEFT")) && (stricmp(pBench->cDebugPosition, "BOTTOMRIGHT")))
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = TSPAN_ERR_PANELPOSITIONENTRYNOTVALID;
			formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "Debug panel position: %s", pBench->cDebugPosition)
			;
			RESMGR_Trace(cTraceBuffer);
		}
	}


	/*---------------------------------------------------------------------/
	/   Check for yield settings MinPassRate entry
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		pBench->YieldCheckEnabled = TRUE;
		if (lTrace)
		{
			RESMGR_Trace("Check for yield settings MinPassRate entry");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "MinPassRate", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->YieldCheckEnabled = FALSE;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "Min pass rate not specified, yield check disabled");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			lBytesWritten = sscanf (cTempBuffer, "%i", &pBench->MinPassRate);
			if (lBytesWritten != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_WRONGYIELDSETTING;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			else
			{
				if (lTrace)
				{
					sprintf(cTraceBuffer, "Yield settings MinPassRate: %i", pBench->MinPassRate);
					RESMGR_Trace(cTraceBuffer);
				}
			}
		}
	}

	/*---------------------------------------------------------------------/
	/   Check for yield settings AfterTestBoards entry
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for yield settings AfterTestedBoards entry");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "AfterTestedBoards", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->YieldCheckEnabled = FALSE;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "After tested boards not specified, yield check disabled");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			lBytesWritten = sscanf (cTempBuffer, "%i", &pBench->AfterTestedBoards);
			if (lBytesWritten != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_WRONGYIELDSETTING;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			else
			{
				if (lTrace)
				{
					sprintf(cTraceBuffer, "Yield settings AfterTestedBoards: %i", pBench->AfterTestedBoards);
					RESMGR_Trace(cTraceBuffer);
				}
			}
		}
	}

	/*---------------------------------------------------------------------/
	/   Check for yield settings ConsecutiveTests entry
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for yield settings ConsecutiveTests entry");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "ConsecutiveTests", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->YieldCheckEnabled = FALSE;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "Consecutive tests not specified, yield check disabled");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			lBytesWritten = sscanf (cTempBuffer, "%i", &pBench->ConsecutiveTests);
			if (lBytesWritten != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_WRONGYIELDSETTING;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			else
			{
				if (lTrace)
				{
					sprintf(cTraceBuffer, "Yield settings ConsecutiveTests: %i", pBench->ConsecutiveTests);
					RESMGR_Trace(cTraceBuffer);
				}
			}
		}
	}
	/*---------------------------------------------------------------------/
	/   Check for stroke counter enabled
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for stroke counter enabled entry");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "StrokeCounterEnable", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->StrokeCounterEnabled = FALSE;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "StrokeCounterEnable entry not found");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			lBytesWritten = sscanf (cTempBuffer, "%i", &pBench->StrokeCounterEnabled);

		}

		if (lTrace)
		{
			RESMGR_Trace("Check for fixture code entry from station globals");
		}
		hResult = TS_PropertyGetValString (sequenceContext, &pTSErrorInfo, "StationGlobals.TSVP.ActualFixtureCode", TS_PropOption_InsertIfMissing, &cFixtureCode);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		}
	}
	/*---------------------------------------------------------------------/
	/   Check for fixture code entry
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred && pBench->StrokeCounterEnabled)
	{


		if (lTrace)
		{
			RESMGR_Trace("Check for fixture code entry application ini");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "FixtureCode", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->StrokeCounterEnabled = FALSE;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "FixtureCode entry not found");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			lBytesWritten = sscanf (cTempBuffer, "%i", &pBench->FixtureCode);
			if (pBench->FixtureCode == -1)
			{

				if (lTrace)
				{
					RESMGR_Trace("Check for fixture code entry from station globals");
				}
				hResult = TS_PropertyGetValString (sequenceContext, &pTSErrorInfo, "StationGlobals.TSVP.ActualFixtureCode", 0, &cFixtureCode);
				if (hResult < 0)
				{
					*pErrorOccurred = TRUE;
					*pErrorCode = hResult;
					CA_GetAutomationErrorString(hResult, errorMessage, 1024);
				}
				lBytesWritten = sscanf (cFixtureCode, "%i", &pBench->FixtureCode);
				if (lBytesWritten <= 0)
				{
					pBench->StrokeCounterEnabled = FALSE;
					if (lTrace)
					{
						sprintf(cTraceBuffer, "FixtureCode entry not found");
						RESMGR_Trace(cTraceBuffer);
					}
					*pErrorOccurred = TRUE;
					*pErrorCode = TSPAN_ERR_FIXTURECODENOTSPEC;
					formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
				}
			}
		}
	}
	/*---------------------------------------------------------------------/
	/   Check for stroke counter file name
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred && pBench->StrokeCounterEnabled)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for stroke counter file name");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "StrokeCounterFile", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->StrokeCounterEnabled = FALSE;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "StrokeCounterEnable entry not found");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			iFileExists = FileExists (cTempBuffer, &iFileSize);
			if (iFileExists)
			{
				sprintf(pBench->StrokeCounterFile, cTempBuffer);
				*pErrorCode = iReadFixtureStrokeFile (pBench->StrokeCounterFile, pBench->FixtureCode, &pBench->StrokeActualCounts,
													  &pBench->StrokeCounterYellowLimit, &pBench->StrokeCounterRedLimit);
				if (*pErrorCode)
				{
					*pErrorOccurred = TRUE;
					formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
				}
			}
			else
			{
				iConfirmation = ConfirmPopup("Error CP-ITS", "Fixture stroke counter is enabled, but cannot find and \nopen fixture stroke counter file. Click \"Yes\" to continue anyway \nand disable the function temporarly");
				if (iConfirmation)
				{
					sprintf(pBench->StrokeCounterFile, "");
					pBench->StrokeCounterEnabled = 0;
				}
				else
				{

					*pErrorOccurred = TRUE;
					*pErrorCode = TSPAN_ERR_STROKECOUNTERFILENOTEXISTS;
					formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
				}
			}
		}
	}
	/*---------------------------------------------------------------------/
	/   Check for variants
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for variants");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "Variants", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->iVariantsEnabled = FALSE;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "Variants not specified, variant handling is disabled.");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			errno = 0;
			lBytesWritten = sscanf (cTempBuffer, "%i", &pBench->iVariantsEnabled);
			if (errno || (pBench->iVariantsEnabled <= 0) || (pBench->iVariantsEnabled > 100))
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_WRONGVARIANTCOUNT;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			else
			{
				if (lTrace)
				{
					sprintf(cTraceBuffer, "Count of variants: %i", pBench->iVariantsEnabled);
					RESMGR_Trace(cTraceBuffer);
				}
			}
		}
	}

	/*---------------------------------------------------------------------/
	/   Read variants
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred && (pBench->iVariantsEnabled > 0))
	{
		for (idx = 1; idx <= pBench->iVariantsEnabled; idx++)
		{
			sprintf(cTempBuffer2, "Variant%i", idx);

			if (lTrace)
			{
				sprintf(cTraceBuffer, "Read %s", cTempBuffer2);
				RESMGR_Trace(cTraceBuffer);
			}
			RESMGR_Get_Key_Value(0, 1, cSectionName, cTempBuffer2, cTempBuffer,
								 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
			if (!lBytesWritten)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_VARIANTNOTSPECIFIED;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			else
			{
				sprintf(pBench->cVariants[idx-1], cTempBuffer);
				if (lTrace)
				{
					sprintf(cTraceBuffer, "Variants %i: %s", idx, cTempBuffer);
					RESMGR_Trace(cTraceBuffer);
				}
			}
		}
	}

	/*---------------------------------------------------------------------/
	/   Check for briefing option
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for briefing option");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "Briefing", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten || !strcmp(cTempBuffer, "0"))
		{
			pBench->Briefing = FALSE;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "Briefing option not specified, Briefing is disabled.");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			pBench->Briefing = TRUE;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "Briefing option specified, Briefing is enabled.");
				RESMGR_Trace(cTraceBuffer);
			}

		}
	}


	/*---------------------------------------------------------------------/
	/   Check for TicketPrinterEnabled
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for TicketPrinterEnabled");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "TicketPrinterEnabled", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->iTicketPrinterEnabled = 4;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "TicketPrinterEnabled option not specified, ticket printer option can changed by any user.");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			pBench->iTicketPrinterEnabled = atoi (cTempBuffer);
			if (errno)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_TICKETPRINTERENABLEDNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if ((pBench->iTicketPrinterEnabled < 0) || (pBench->iTicketPrinterEnabled > 4))
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_TICKETPRINTERENABLEDNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if (lTrace)
			{
				sprintf(cTraceBuffer, "ticket printer option specified (Value: %i)", pBench->iTicketPrinterEnabled);
				RESMGR_Trace(cTraceBuffer);
			}

		}
	}

	/*---------------------------------------------------------------------/
	/   Check for ReportEnabled
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for ReportEnabled");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "ReportEnabled", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->iReportEnabled = 4;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "ReportEnabled option not specified, report enabled option can changed by any user.");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			pBench->iReportEnabled = atoi (cTempBuffer);
			if (errno)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_REPORTENABLEDNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if ((pBench->iReportEnabled < 0) || (pBench->iReportEnabled > 4))
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_REPORTENABLEDNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if (lTrace)
			{
				sprintf(cTraceBuffer, "report enabled option specified (Value: %i)", pBench->iReportEnabled);
				RESMGR_Trace(cTraceBuffer);
			}

		}
	}

	/*---------------------------------------------------------------------/
	/   Check for AppendReportToExisting
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for AppendReportToExisting");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "AppendReportToExisting", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->iAppendReportToExisting = 4;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "AppendReportToExisting option not specified, append to report option can changed by any user.");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			pBench->iAppendReportToExisting = atoi (cTempBuffer);
			if (errno)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_APPREPORTTOEXISTINGNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if ((pBench->iAppendReportToExisting < 0) || (pBench->iAppendReportToExisting > 4))
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_APPREPORTTOEXISTINGNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if (lTrace)
			{
				sprintf(cTraceBuffer, "Append to report option specified (Value: %i)", pBench->iAppendReportToExisting);
				RESMGR_Trace(cTraceBuffer);
			}

		}
	}

	/*---------------------------------------------------------------------/
	/   Check for ReportOnlyFailedTests
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for ReportOnlyFailedTests");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "ReportOnlyFailedTests", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->iReportOnlyFailedTests = 4;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "ReportOnlyFailedTests option not specified, report only failed option can changed by any user.");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			pBench->iReportOnlyFailedTests = atoi (cTempBuffer);
			if (errno)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_REPORTONLYFAILEDNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if ((pBench->iReportOnlyFailedTests < 0) || (pBench->iReportOnlyFailedTests > 4))
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_REPORTONLYFAILEDNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if (lTrace)
			{
				sprintf(cTraceBuffer, "Report only failed option specified (Value: %i)", pBench->iReportOnlyFailedTests);
				RESMGR_Trace(cTraceBuffer);
			}

		}
	}

	/*---------------------------------------------------------------------/
	/   Check for ResetStatisticsEnabled
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for ResetStatisticsEnabled");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "ResetStatisticsEnabled", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->iResetStatisticsEnabled = 1;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "ResetStatisticsEnabled option not specified, statistic can reset by any user.");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			pBench->iResetStatisticsEnabled = atoi (cTempBuffer);
			if (errno)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_RESETSTATISTICSNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if ((pBench->iResetStatisticsEnabled < 0) || (pBench->iResetStatisticsEnabled > 2))
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_RESETSTATISTICSNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if (lTrace)
			{
				sprintf(cTraceBuffer, "ResetStatisticsEnabled option specified (Value: %i)", pBench->iResetStatisticsEnabled);
				RESMGR_Trace(cTraceBuffer);
			}

		}
	}
	
	/*---------------------------------------------------------------------/
	/   Check for MSAOptionsEnabled
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for MSAOptionsEnabled");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "MSAOptionsEnabled", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			pBench->iMSAOptionsEnabled = 0;
			if (lTrace)
			{
				sprintf(cTraceBuffer, "MSAOptionsEnabled option not specified, msa option can changed by any user.");
				RESMGR_Trace(cTraceBuffer);
			}
		}
		else
		{
			pBench->iMSAOptionsEnabled = atoi (cTempBuffer);
			if (errno)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_MSAOPTIONENABLEDNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if ((pBench->iMSAOptionsEnabled < 0) || (pBench->iMSAOptionsEnabled > 1))
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_MSAOPTIONENABLEDNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if (lTrace)
			{
				sprintf(cTraceBuffer, "msa option specified (Value: %i)", pBench->iMSAOptionsEnabled);
				RESMGR_Trace(cTraceBuffer);
			}
			TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo,
		         "StationGlobals.TSVP.MSAType1.Enabled", TS_PropOption_InsertIfMissing, VFALSE);
			TS_PropertySetValNumber (sequenceContext, &pTSErrorInfo, 
				 "StationGlobals.TSVP.MSAType1.Reiterations", TS_PropOption_InsertIfMissing, (double) 0);
			TS_PropertySetValNumber (sequenceContext, &pTSErrorInfo, 
				 "StationGlobals.TSVP.MSAType1.ActualIndex", TS_PropOption_InsertIfMissing, (double) 0);

		}
	}


	/*---------------------------------------------------------------------/
	/   Check for TimingMSAWithRunButton
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for TimingMSAWithRunButton");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "TimingMSAWithRunButton", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			RESMGR_Trace("TimingMSAWithRunButton not found in app ini. Set value to standard value 5.0");
			pBench->TimingMSAWithRunButton = 5.0;
		}
		else
		{
			pBench->TimingMSAWithRunButton = atof (cTempBuffer);
			if (errno)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_RESETSTATISTICSNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if (lTrace)
			{
				sprintf(cTraceBuffer, "TimingMSAWithRunButton option specified (Value: %f)", pBench->TimingMSAWithRunButton);
				RESMGR_Trace(cTraceBuffer);
			}

		}
	}

	/*---------------------------------------------------------------------/
	/   Check for cpkLimit
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Check for cpkLimit");
		}
		RESMGR_Get_Key_Value(0, 1, cSectionName, "cpkLimit", cTempBuffer,
							 1024, &lBytesWritten, pErrorOccurred, pErrorCode, errorMessage);
		if (!lBytesWritten)
		{
			RESMGR_Trace("cpkLimit not found in app ini. Set value to standard value 1.33");
			pBench->cpkLimit = 1.33;
		}
		else
		{
			pBench->cpkLimit = atof (cTempBuffer);
			if (errno)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_RESETSTATISTICSNOTVALID;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			if (lTrace)
			{
				sprintf(cTraceBuffer, "cpkLimit option specified (Value: %f)", pBench->cpkLimit);
				RESMGR_Trace(cTraceBuffer);
			}

		}
	}

	/*---------------------------------------------------------------------/
	/   Load ControlPanelIni File and all included setting
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("Open ControlPanel.ini");
		}
		sprintf(cTempBuffer, "C:\\Users\\Public\\Documents\\SW-ITS\\Configuration\\ControlPanel.ini");
		iFileExists = 0;
		iFileExists = FileExists (cTempBuffer, &iFileSize);
		if (!iFileExists)
		{
			GetModuleDir (hinstDLLOwn, cProjectDirectory);

			sprintf(cTempBuffer, "%s\\ControlPanel.ini", cProjectDirectory);
			iFileExists = FileExists (cTempBuffer, &iFileSize);
			if (!iFileExists)
			{
				sprintf(pBench->cTestResultFontType, "Tahoma");
				pBench->iTestResultFontSize = 12;
				pBench->iTestResultFontUseBold = 0;
				pBench->iUseColoredBackGround = 1;
				pBench->iBGColorPassRed = 171;
				pBench->iBGColorPassGreen = 255;
				pBench->iBGColorPassBlue = 176;
				pBench->iBGColorFailRed = 255;
				pBench->iBGColorFailGreen = 171;
				pBench->iBGColorFailBlue = 171;
				pBench->iBGColorRunningRed = 255;
				pBench->iBGColorRunningGreen = 255;
				pBench->iBGColorRunningBlue = 191;
				if (lTrace)
				{
					RESMGR_Trace("ControlPanel.ini not found -> setting standard values");
				}
			}
		}
		if (iFileExists)
		{

			IniFileHandle = Ini_New (1);
			iStatus = Ini_ReadFromFile (IniFileHandle, cTempBuffer);
			if (iStatus)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetStringIntoBuffer (IniFileHandle, "TestResults", "Font", pBench->cTestResultFontType, 1024);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "Size", &pBench->iTestResultFontSize);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "UseBold", &pBench->iTestResultFontUseBold);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "UseColoredBackGround", &pBench->iUseColoredBackGround);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "BGColorPassRed", &pBench->iBGColorPassRed);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "BGColorPassGreen", &pBench->iBGColorPassGreen);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "BGColorPassBlue", &pBench->iBGColorPassBlue);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "BGColorFailRed", &pBench->iBGColorFailRed);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "BGColorFailGreen", &pBench->iBGColorFailGreen);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "BGColorFailBlue", &pBench->iBGColorFailBlue);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "BGColorRunningRed", &pBench->iBGColorRunningRed);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "BGColorRunningGreen", &pBench->iBGColorRunningGreen);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}
			iStatus = Ini_GetInt (IniFileHandle, "TestResults", "BGColorRunningBlue", &pBench->iBGColorRunningBlue);
			if (iStatus != 1)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = TSPAN_ERR_INIFILEERROR;
				formatError(errorMessage,  *pErrorCode,  *pResourceId, NULL);
			}

			Ini_Dispose (IniFileHandle);

		}

	}

	if (lTrace)
	{
		RESMGR_Trace("Set Teststand variables and internal variables");
	}

	if (!*pErrorOccurred)
	{
		TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo, "StationGlobals.TSVP.RunTest", TS_PropOption_InsertIfMissing, VFALSE);
		TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo, "StationGlobals.TSVP.QuitProgram", TS_PropOption_InsertIfMissing, VFALSE);
		TS_PropertySetValString(sequenceContext, &pTSErrorInfo, "StationGlobals.TSVP.SerialNumber", TS_PropOption_InsertIfMissing, "");
	}

	giActualIndex = 0;
	gdLowerLimit = 0.0;
	gdUpperLimit = 0.0;
	giHistogramInterval = 30;
	giYieldCheckEnabledFile = pBench->YieldCheckEnabled;
	sprintf(gcSelectedVariant,"");

	/*---------------------------------------------------------------------/
	/   Cleanup and error handling
	/---------------------------------------------------------------------*/
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_Setup end");
	}

}

/* FUNCTION *****************************************************************/
/**
tspan_DisplayDebugPanel
*
@brief Display the debug panel of CP-ITS
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_DisplayDebugPanel(CAObjHandle sequenceContext, long
									   pResourceId, short *pErrorOccurred, long *pErrorCode, char errorMessage[])
{
	char cTraceBuffer[1024];

	int iTabHandle;

	long lTrace;

	BENCH_STRUCT *pBench = NULL;

	CAObjHandle execution;
	ERRORINFO errorInfo;
	HRESULT hResult;
	static ThreadData threadDataItem =
	{
		0
	};
	CAObjHandle tmpExecutionObjHandle = 0;
	LPDISPATCH tmpExecutionDispPtr = NULL;
	CAObjHandle tmpStationGlobalsObjHandle = 0;
	LPDISPATCH tmpStationGlobalsDispPtr = NULL;


	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_DisplayDebugPanel begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}

	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("InterlockedIncrement");
		}

		// Ensure only 1 thread updates thread_count at a time
		InterlockedIncrement(&iThreadCount);

		if (lTrace)
		{
			sprintf(cTraceBuffer, "ThreadCount: %d", iThreadCount);
			RESMGR_Trace(cTraceBuffer);
			RESMGR_Trace("EnterCriticalSection");
		}

		//InitializeCriticalSection(&CriticalSection);

		EnterCriticalSection(&CriticalSection);

		if (iThreadCount > 1)
		{
			LeaveCriticalSection(&CriticalSection);
		}
		else
		{

			if (lTrace)
			{
				RESMGR_Trace("TS_SeqContextGetProperty");
			}

			//Get teststand executon reference
			hResult = TS_SeqContextGetProperty(sequenceContext, &errorInfo,
											   TS_SeqContextExecution, CAVT_OBJHANDLE, &execution);
			if (hResult < 0)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = hResult;
				CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			}

		}

	}

	if (lTrace)
	{
		RESMGR_Trace("Add a reference to the execution activeX automation object")
		;
	}
	// add a reference to the execution activeX automation object
	if (! *pErrorOccurred)
	{
		hResult = CA_GetDispatchFromObjHandle(execution, &tmpExecutionDispPtr);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		}
	}

	if (! *pErrorOccurred)
	{
		hResult = CA_CreateObjHandleFromIDispatch(tmpExecutionDispPtr, 1,
				  &tmpExecutionObjHandle);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		}
	}

	threadDataItem.execution = tmpExecutionObjHandle;
	if (lTrace)
	{
		RESMGR_Trace("Add a reference to the station globals activeX automation object");
	}
	// add a reference to the ThisContext activeX automation object
	if (! *pErrorOccurred)
	{
		hResult = CA_GetDispatchFromObjHandle(sequenceContext,
											  &tmpStationGlobalsDispPtr);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		}
	}
	if (! *pErrorOccurred)
	{
		hResult = CA_CreateObjHandleFromIDispatch(tmpStationGlobalsDispPtr, 1,
				  &tmpStationGlobalsObjHandle);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		}
	}
	if (! *pErrorOccurred)
	{
		threadDataItem.ThisContext = tmpStationGlobalsObjHandle;

		if (lTrace)
		{
			RESMGR_Trace("Create new thread for UIR control");
		}
		hThreadReady = 0;
		//Create new thread for UIR control
		hThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)
									 StartThreadForDebugPanel, (void*) &threadDataItem, 0, &pBench
									 ->gThreadID);

		// Wait until thread starts
		if (WaitForSingleObject(hThreadReady, WAITTMO) == WAIT_TIMEOUT)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = TSPAN_ERR_THREADWASNOTSTARTET;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}

	}
	if (lTrace)
	{
		RESMGR_Trace("LeaveCriticalSection");
	}
	LeaveCriticalSection(&CriticalSection);

	if (execution)
	{
		CA_DiscardObjHandle(execution);
		execution = 0;
	}

	Delay(0.3);

	if (lTrace) RESMGR_Trace("Set panel mode DEBUG_MODE");

	pBench->iMode = DEBUG_MODE;
	pBench->iActualPanelHandle = giActualPanelHandle;

	if (lTrace) RESMGR_Trace("Reset Measurement variables.");
	ResetMeasurements();
	if (pBench->iDemoMode)
	{
		//Settings for analog
		GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 0,
								  &iTabHandle);
		SetCtrlVal (iTabHandle, ANALOG_RESULT, "DEMO");
	}


	if(pBench->iVariantsEnabled)
	{
		InsertVariantsDebug (pBench, giActualPanelHandle, (void*) &threadDataItem);
		//Get current user
		hResult = TS_PropertySetValString (sequenceContext, &errorInfo, "StationGlobals.TSVP.SelectedVariant", 0, pBench->cSelectedVariant);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}


	}

	//ProcessDrawEvents();
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_DisplayDebugPanel end");
	}

}

/* FUNCTION *****************************************************************/
/**
tspan_DisplayOperatorPanel
*
@brief Display the single operator panel of CP-ITS
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_DisplayOperatorPanel(CAObjHandle sequenceContext, long
		pResourceId, short *pErrorOccurred, long *pErrorCode, char errorMessage[])
{
	char cTraceBuffer[1024];
	char cTextBuffer[1024];
	char *cUser;
	char *cStationID;
	long lTrace;

	int menuBar_ID;
	int SubMenu_ID;
	int Item_ID;

	int iCountOfVariants;

	int iBriefingPanel;
	char *cBriefingText;

	int idx, panel, ctrl;
	char *cVariantName;
	char *cVariantContent;
	char *cSeqPathName;
	char cCompleteVariantName[1024];

	BENCH_STRUCT *pBench = NULL;

	VBOOL bValue;
	VBOOL bCanChangeSettings;

	CAObjHandle execution;
	CAObjHandle pEngine;
	ERRORINFO errorInfo;
	HRESULT hResult;
	static ThreadData threadDataItem = {0};
	CAObjHandle tmpExecutionObjHandle = 0;
	LPDISPATCH tmpExecutionDispPtr = NULL;
	CAObjHandle tmpStationGlobalsObjHandle = 0;
	LPDISPATCH tmpStationGlobalsDispPtr = NULL;


	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_DisplayOperatorPanel begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}

	sprintf(cLastNameOfStep, pBench->cTitlebarText);

	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("InterlockedIncrement");
		}

		// Ensure only 1 thread updates thread_count at a time
		InterlockedIncrement(&iThreadCount);
		//InitializeCriticalSection(&CriticalSection);
		EnterCriticalSection(&CriticalSection);

		if (iThreadCount > 1)
		{
			LeaveCriticalSection(&CriticalSection);
		}
		else
		{

			if (lTrace)
			{
				RESMGR_Trace("TS_SeqContextGetProperty");
			}

			//Get teststand executon reference
			hResult = TS_SeqContextGetProperty(sequenceContext, &errorInfo,
											   TS_SeqContextExecution, CAVT_OBJHANDLE, &execution);
			if (hResult < 0)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = hResult;
				CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			}

		}

	}
	
	if (lTrace)
	{
		RESMGR_Trace(
			"Add a reference to the execution activeX automation object");
	}

	// add a reference to the execution activeX automation object
	if (! *pErrorOccurred)
	{
		hResult = CA_GetDispatchFromObjHandle(execution, &tmpExecutionDispPtr);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		}
	}

	if (! *pErrorOccurred)
	{
		hResult = CA_CreateObjHandleFromIDispatch(tmpExecutionDispPtr, 1,
				  &tmpExecutionObjHandle);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		}
	}

	threadDataItem.execution = tmpExecutionObjHandle;
	if (lTrace)
	{
		RESMGR_Trace(
			"Add a reference to the station globals activeX automation object");
	}
	// add a reference to the ThisContext activeX automation object
	if (! *pErrorOccurred)
	{
		hResult = CA_GetDispatchFromObjHandle(sequenceContext,
											  &tmpStationGlobalsDispPtr);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		}
	}
	if (! *pErrorOccurred)
	{
		hResult = CA_CreateObjHandleFromIDispatch(tmpStationGlobalsDispPtr, 1,
				  &tmpStationGlobalsObjHandle);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		}
	}
	if (! *pErrorOccurred)
	{
		threadDataItem.ThisContext = tmpStationGlobalsObjHandle;

		if (lTrace)
		{
			RESMGR_Trace("Create new thread for UIR control");
		}
		//Create new thread for UIR control
		hThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)
									 StartThreadForOperatorPanel, (void*) &threadDataItem, 0, &pBench
									 ->gThreadID);

		// Wait until thread starts
		if (WaitForSingleObject(hThreadReady, WAITTMO) == WAIT_TIMEOUT)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = TSPAN_ERR_THREADWASNOTSTARTET;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}
	if (lTrace)
	{
		RESMGR_Trace("LeaveCriticalSection");
	}
	
	LeaveCriticalSection(&CriticalSection);

	if (execution)
	{
		CA_DiscardObjHandle(execution);
		execution = 0;
	}

	Delay(0.3);

	if (lTrace)
	{
		RESMGR_Trace("Set panel mode OPERATOR_MODE");
	}


	pBench->iMode = OPERATOR_MODE;

	pBench->iActualPanelHandle = giActualPanelHandle;
	
	menuBar_ID = GetPanelMenuBar (giActualPanelHandle);

	SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TIMER_MSA_RUNBUTTON, ATTR_INTERVAL, pBench->TimingMSAWithRunButton);
	GetPanelHandleFromTabPage (giActualPanelHandle, OP_SINGLE_TAB, 1, &giActualTabHandle);
	SetCtrlVal (giActualTabHandle, TABPANEL_2_BUFFERCPK, pBench->cpkLimit);

	GetPanelHandleFromTabPage (giActualPanelHandle, OP_SINGLE_TAB, 0, &giActualTabHandle);
	pBench->iActualTabHandle = giActualTabHandle;

	SetActiveTabPage (giActualPanelHandle, OP_SINGLE_TAB, 0);

	////////////////////////////////////////////////////////////////////////////
	// Set report text attribs
	////////////////////////////////////////////////////////////////////////////
	SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_FONT, pBench->cTestResultFontType);
	SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTERRORLOG, ATTR_TEXT_FONT, pBench->cTestResultFontType);
	SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_POINT_SIZE, pBench->iTestResultFontSize);
	SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTERRORLOG, ATTR_TEXT_POINT_SIZE, pBench->iTestResultFontSize);
	if(pBench->iTestResultFontUseBold)
	{
		SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_BOLD, 1);
		SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTERRORLOG, ATTR_TEXT_BOLD, 1);     
	}
	else
	{
		SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_BOLD, 0);
		SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTERRORLOG, ATTR_TEXT_BOLD, 0); 
	}
	SetCtrlVal (giActualPanelHandle, OP_SINGLE_COL_RUNNING, MakeColor (pBench->iBGColorRunningRed, pBench->iBGColorRunningGreen, pBench->iBGColorRunningBlue));
	SetCtrlVal (giActualPanelHandle, OP_SINGLE_COL_PASS, MakeColor (pBench->iBGColorPassRed, pBench->iBGColorPassGreen, pBench->iBGColorPassBlue));
	SetCtrlVal (giActualPanelHandle, OP_SINGLE_COL_FAIL, MakeColor (pBench->iBGColorFailRed, pBench->iBGColorFailGreen, pBench->iBGColorFailBlue));
	SetCtrlVal (giActualPanelHandle, OP_SINGLE_USECOLOREDBG, pBench->iUseColoredBackGround);

	////////////////////////////////////////////////////////////////////////////
	// Get user privilige Debug.EditStationGlobals
	////////////////////////////////////////////////////////////////////////////

	hResult = TS_SeqContextGetEngine (sequenceContext, &errorInfo, &pEngine);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		return;
	}

	hResult = TS_EngineCurrentUserHasPrivilege (pEngine, &errorInfo, "Debug.EditStationGlobals", &bCanChangeSettings);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		return;
	}


	if (lTrace)
	{
		RESMGR_Trace("Set operator panel menu attributes");
	}

	////////////////////////////////////////////////////////////////////////////
	// Set menu attributes for report enabled option
	////////////////////////////////////////////////////////////////////////////

	if (pBench->iReportEnabled == 0)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportResults", TS_PropOption_InsertIfMissing, FALSE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_CHECKED, FALSE);
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_DIMMED, TRUE);
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_REPORT, FALSE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.ReportResults -> False");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iReportEnabled == 1)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportResults", TS_PropOption_InsertIfMissing, TRUE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_CHECKED, TRUE);
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_DIMMED, TRUE);
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_REPORT, TRUE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.ReportResults -> True");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iReportEnabled == 2)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportResults", TS_PropOption_InsertIfMissing, FALSE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_CHECKED, FALSE);
		if (!bCanChangeSettings)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_DIMMED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_DIMMED, FALSE);
		}
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_REPORT, FALSE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.ReportResults -> False");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iReportEnabled == 3)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportResults", TS_PropOption_InsertIfMissing, TRUE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_CHECKED, TRUE);
		if (!bCanChangeSettings)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_DIMMED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_DIMMED, FALSE);
		}
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_REPORT, TRUE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.ReportResults -> True");
			RESMGR_Trace(cTraceBuffer);
		}
	}


	if (pBench->iReportEnabled == 4)
	{
		//Set menu attribute
		hResult = TS_PropertyGetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportResults", TS_PropOption_InsertIfMissing, &bValue);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		if (bValue)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_CHECKED, TRUE);
			SetCtrlVal (giActualPanelHandle, OP_SINGLE_REPORT, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ENABLE, ATTR_CHECKED, FALSE);
			SetCtrlVal (giActualPanelHandle, OP_SINGLE_REPORT, FALSE);
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.ReportResults -> %i", bValue);
			RESMGR_Trace(cTraceBuffer);
		}
	}

	////////////////////////////////////////////////////////////////////////////
	// Set menu attributes for report only failed option
	////////////////////////////////////////////////////////////////////////////
	if (pBench->iReportOnlyFailedTests == 0)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportOnlyFail", TS_PropOption_InsertIfMissing, FALSE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_CHECKED, FALSE);
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_DIMMED, TRUE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.ReportOnlyFail -> False");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iReportOnlyFailedTests == 1)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportOnlyFail", TS_PropOption_InsertIfMissing, TRUE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_CHECKED, TRUE);
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_DIMMED, TRUE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.ReportOnlyFail -> True");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iReportOnlyFailedTests == 2)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportOnlyFail", TS_PropOption_InsertIfMissing, FALSE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_CHECKED, FALSE);
		if (!bCanChangeSettings)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_DIMMED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_DIMMED, FALSE);
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.ReportOnlyFail -> False");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iReportOnlyFailedTests == 3)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportOnlyFail", TS_PropOption_InsertIfMissing, TRUE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_CHECKED, TRUE);
		if (!bCanChangeSettings)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_DIMMED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_DIMMED, FALSE);
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.ReportOnlyFail -> False");
			RESMGR_Trace(cTraceBuffer);
		}
	}


	if (pBench->iReportOnlyFailedTests == 4)
	{
		//Set menu attribute
		hResult = TS_PropertyGetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportOnlyFail", TS_PropOption_InsertIfMissing, &bValue);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		if (bValue)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_CHECKED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_ONLYFAIL, ATTR_CHECKED, FALSE);
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.ReportOnlyFail -> %i", abs(bValue));
			RESMGR_Trace(cTraceBuffer);
		}
	}

	////////////////////////////////////////////////////////////////////////////
	// Set menu attributes for append report to existing option
	////////////////////////////////////////////////////////////////////////////
	if (pBench->iAppendReportToExisting == 0)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.AppendReport", TS_PropOption_InsertIfMissing, FALSE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_CHECKED, FALSE);
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_DIMMED, TRUE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.AppendReport -> False");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iAppendReportToExisting == 1)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.AppendReport", TS_PropOption_InsertIfMissing, TRUE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_CHECKED, TRUE);
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_DIMMED, TRUE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.AppendReport -> True");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iAppendReportToExisting == 2)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.AppendReport", TS_PropOption_InsertIfMissing, FALSE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_CHECKED, FALSE);
		if (!bCanChangeSettings)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_DIMMED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_DIMMED, FALSE);
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.AppendReport -> False");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iAppendReportToExisting == 3)
	{
		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.AppendReport", TS_PropOption_InsertIfMissing, TRUE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_CHECKED, TRUE);
		if (!bCanChangeSettings)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_DIMMED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_DIMMED, FALSE);
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.AppendReport -> True");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iAppendReportToExisting == 4)
	{
		//Set menu attribute
		hResult = TS_PropertyGetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.AppendReport", TS_PropOption_InsertIfMissing, &bValue);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		if (bValue)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_CHECKED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_REPORT_APPEND, ATTR_CHECKED, FALSE);
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.AppendReport -> %i", abs(bValue));
			RESMGR_Trace(cTraceBuffer);
		}
	}

	////////////////////////////////////////////////////////////////////////////
	// Set menu attributes for ticket printer enabled option
	////////////////////////////////////////////////////////////////////////////
	if (pBench->iTicketPrinterEnabled == 0)
	{
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.EnableTicketPrinter", TS_PropOption_InsertIfMissing, FALSE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_CHECKED, FALSE);
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_DIMMED, TRUE);
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_TICKET, FALSE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.EnableTicketPrinter -> False");
			RESMGR_Trace(cTraceBuffer);
		}
	}


	if (pBench->iTicketPrinterEnabled == 1)
	{

		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.EnableTicketPrinter", TS_PropOption_InsertIfMissing, TRUE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_CHECKED, TRUE);
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_DIMMED, TRUE);
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_TICKET, TRUE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.EnableTicketPrinter -> True");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iTicketPrinterEnabled == 2)
	{

		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.EnableTicketPrinter", TS_PropOption_InsertIfMissing, FALSE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_CHECKED, FALSE);
		if (!bCanChangeSettings)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_DIMMED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_DIMMED, FALSE);
		}
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_TICKET, FALSE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.EnableTicketPrinter -> False");
			RESMGR_Trace(cTraceBuffer);
		}
	}


	if (pBench->iTicketPrinterEnabled == 3)
	{

		//Set menu attribute
		hResult = TS_PropertySetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.EnableTicketPrinter", TS_PropOption_InsertIfMissing, TRUE);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_CHECKED, TRUE);
		if (!bCanChangeSettings)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_DIMMED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_DIMMED, FALSE);
		}
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_TICKET, TRUE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.EnableTicketPrinter -> True");
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (pBench->iTicketPrinterEnabled == 4)
	{
		//Set menu attribute
		hResult = TS_PropertyGetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.EnableTicketPrinter", TS_PropOption_InsertIfMissing, &bValue);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		if (bValue)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_CHECKED, TRUE);
			SetCtrlVal (giActualPanelHandle, OP_SINGLE_TICKET, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_TICKET_ENABLE, ATTR_CHECKED, FALSE);
			SetCtrlVal (giActualPanelHandle, OP_SINGLE_TICKET, FALSE);
		}
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.EnableTicketPrinter -> %i", abs(bValue));
			RESMGR_Trace(cTraceBuffer);
		}
	}


	////////////////////////////////////////////////////////////////////////////
	// Set menu attributes for msa option
	////////////////////////////////////////////////////////////////////////////
	if (pBench->iMSAOptionsEnabled == 0)
	{
		SetMenuBarAttribute (menuBar_ID, MENUBAR_PROGRAM_MODE_MSATYPE1, ATTR_DIMMED, TRUE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.iMSAOptionsEnabled -> False");
			RESMGR_Trace(cTraceBuffer);
		}
	}


	if (pBench->iMSAOptionsEnabled == 1)
	{

		SetMenuBarAttribute (menuBar_ID, MENUBAR_PROGRAM_MODE_MSATYPE1, ATTR_DIMMED, FALSE);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "StationGlobals.TSVP.iMSAOptionsEnabled -> True");
			RESMGR_Trace(cTraceBuffer);
		}
	}



	////////////////////////////////////////////////////////////////////////////
	// Set menu attributes for reset statistics
	////////////////////////////////////////////////////////////////////////////
	if (pBench->iResetStatisticsEnabled == 0)
	{

		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_RESETSTAT, ATTR_DIMMED, TRUE);

	}

	if (pBench->iResetStatisticsEnabled == 1)
	{

		SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_RESETSTAT, ATTR_DIMMED, FALSE);

	}

	if (pBench->iResetStatisticsEnabled == 2)
	{

		if (!bCanChangeSettings)
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_RESETSTAT, ATTR_DIMMED, TRUE);
		}
		else
		{
			SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_RESETSTAT, ATTR_DIMMED, FALSE);
		}

	}

		//Set menu attribute
	hResult = TS_PropertyGetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.UI_ActualTestStep", TS_PropOption_InsertIfMissing, &bValue);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		return;
	}
	SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_UI_ACTUAL, ATTR_CHECKED, bValue);
	if (lTrace)
	{
		sprintf(cTraceBuffer, "StationGlobals.TSVP.UI_ActualTestStep -> %i", abs(bValue));
		RESMGR_Trace(cTraceBuffer);
	}

	//Set menu attribute
	hResult = TS_PropertyGetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.UI_ReportOnlyFail", TS_PropOption_InsertIfMissing, &bValue);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		return;
	}
	SetMenuBarAttribute (menuBar_ID, MENUBAR_SET_UI_REPORT, ATTR_CHECKED, bValue);
	if (lTrace)
	{
		sprintf(cTraceBuffer, "StationGlobals.TSVP.UI_ReportOnlyFail -> %i", abs(bValue));
		RESMGR_Trace(cTraceBuffer);
	}

	giSerialIsMandatory = pBench->SerialIsMandatory;
	giSerialLength = pBench->SerialLength;
	if (giSerialIsMandatory) SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_SERIAL, ATTR_DIMMED, FALSE);
	if (lTrace)
	{
		sprintf(cTraceBuffer, "UIR Serial number settings -> %i", giSerialIsMandatory);
		RESMGR_Trace(cTraceBuffer);
	}

	giRunButtonDisabled = pBench->RunButtonDisabled;
	if (lTrace)
	{
		sprintf(cTraceBuffer, "Run button disabled settings -> %i", giRunButtonDisabled);
		RESMGR_Trace(cTraceBuffer);
	}


	//Get current user
	hResult = TS_PropertyGetValString (sequenceContext, &errorInfo, "StationGlobals.TS.CurrentUser.LoginName", 0, &cUser);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		return;
	}
	
	sprintf(cTraceBuffer, "Current User: %s", cUser);
	SetCtrlVal(giActualPanelHandle, OP_SINGLE_OPERATOR, cTraceBuffer);
	if (lTrace)
	{
		RESMGR_Trace(cTraceBuffer);
	}

	CA_FreeMemory (cUser);

	//Get Station ID
	hResult = TS_PropertyGetValString (sequenceContext, &errorInfo, "StationGlobals.Station_ID", 0, &cStationID);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		return;
	}

	if (lTrace)
	{
		RESMGR_Trace("Set panel attributes");
	}
	
	//Set panel attributes
	sprintf(cTextBuffer, "Product: %s @ Station ID: %s", pBench->cProductName, cStationID);
	SetCtrlVal (giActualPanelHandle, OP_SINGLE_PRODUCTNAME, cTextBuffer);
	SetCtrlVal (giActualPanelHandle, OP_SINGLE_TESTSTATUS_MSG, "Waiting...");

	CA_FreeMemory (cStationID);


	if (lTrace)
	{
		RESMGR_Trace("Begin display briefing");
	}

	if(pBench->Briefing)
	{
		iBriefingPanel = LoadPanelEx (0, "testcontrolpanel_single.uir", BRIEFING, __CVIUserHInst);
		//Get current user
		hResult = TS_PropertyGetValString (sequenceContext, &errorInfo, "FileGlobals.BriefingText", 0, &cBriefingText);
		if (hResult < 0)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = hResult;
			CA_GetAutomationErrorString(hResult, errorMessage, 1024);
			return;
		}
		ResetTextBox(iBriefingPanel, BRIEFING_TEXTBOX, "");
		SetCtrlVal (iBriefingPanel, BRIEFING_TEXTBOX, cBriefingText);
		DisplayPanel(iBriefingPanel);
		GetUserEvent (1, &panel, &ctrl);
		while ((panel != iBriefingPanel) && (ctrl != BRIEFING_CONTINUE)) GetUserEvent (1, &panel, &ctrl);
		DiscardPanel(iBriefingPanel);
		CA_FreeMemory (cBriefingText);

	}

	if (lTrace)
	{
		RESMGR_Trace("Set variants");
	}

	if(pBench->iVariantsEnabled)
	{
		sprintf(pBench->cSelectedVariant, "");
		InsertVariants (pBench, giActualPanelHandle, (void*) &threadDataItem);
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_DIMMED, TRUE);
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_VARIANT_RING, ATTR_VISIBLE, TRUE);
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TIMERFORVARIANT, ATTR_ENABLED, TRUE);
		//SetCtrlVal (giActualPanelHandle, OP_SINGLE_VARIANT_RING, "Please select a variant!");
	}
	else
	{
		sprintf(pBench->cSelectedVariant, "");
		if (!giRunButtonDisabled) SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_DIMMED, FALSE);
	}


	if (lTrace)
	{
		RESMGR_Trace("Set yield options");
	}


	SetCtrlVal (giActualPanelHandle, OP_SINGLE_YIELD, pBench->YieldCheckEnabled);
	giYieldCheckEnabledMenu = pBench->YieldCheckEnabled;

	/*************************************************************************/

	//  Stroke Counter Settings

	/*************************************************************************/
	if (pBench->StrokeCounterEnabled && (pBench->StrokeActualCounts >= pBench->StrokeCounterYellowLimit))
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, ATTR_ON_COLOR, VAL_YELLOW);
	if (pBench->StrokeCounterEnabled && (pBench->StrokeActualCounts >= pBench->StrokeCounterRedLimit))
	{
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TIMERSTROKEINDICATOR, ATTR_ENABLED, TRUE);
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, ATTR_ON_COLOR, VAL_RED);
	}
	if (pBench->StrokeCounterEnabled)
	{
		sprintf (cTextBuffer, "Stroke Indicator (%i)", pBench->StrokeActualCounts);
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, ATTR_LABEL_TEXT, cTextBuffer);
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_STROKE_ACT, pBench->StrokeActualCounts);
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_STROKE_YELLOW, pBench->StrokeCounterYellowLimit);
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_STROKE_RED, pBench->StrokeCounterRedLimit);
	}

	SetCtrlVal (giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, pBench->StrokeCounterEnabled);
	SetActiveCtrl(giActualPanelHandle, OP_SINGLE_SERIAL); 
	giActualItemIndex = 0;
    giActualErrorLogItemIndex = 0;
	
	SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_VISIBLE, 1);
	
	if (giErrorLogWindowEnabled) 
	{	
		SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTERRORLOG, ATTR_VISIBLE, 1);
		SetCtrlAttribute (giActualTabHandle, TABPANEL_SPLITTER, ATTR_VISIBLE, 1);
		SetTabPageAttribute (giActualPanelHandle, OP_SINGLE_TAB, 0, ATTR_LABEL_TEXT, "__Actual Test Results & Failed Tests Log");
	}
	SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_BGCOLOR, 0xFAFAFA);
	SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTERRORLOG, ATTR_TEXT_BGCOLOR, 0xFAFAFA);
	vOrderGUIElementsAfterResize(giActualPanelHandle);
	
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_DisplayOperatorPanel end");
	}

}

/* FUNCTION *****************************************************************/
/**
tspan_DisplayResult
*
@brief Set the result in operator panel of CP-ITS - use in callback PostUUT
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param iResult: set the reault of a test run, message colors etc (Input) 0 -> PASS, other -> FAIL
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_DisplayResult (CAObjHandle sequenceContext, long pResourceId,
									int iResult, short *pErrorOccurred,
									long *pErrorCode, char errorMessage[])
{
	char cTraceBuffer[1024];
	int menubarhandle;
	int iMenuID;
	long lTrace;
	int lCountOfLines, iLeft;
	double dExecTime;

	int iColorValue;
	int iEnableColor;

	char *cSerial;
	char cTextBuffer[1024];

	ERRORINFO pTSErrorInfo;
	HRESULT hResult;


	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_DisplayResult begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}

	if (! *pErrorOccurred)
	{
		if (pBench->SerialIsMandatory >= 1)
		{
			hResult = TS_PropertyGetValString (sequenceContext, &pTSErrorInfo, "StationGlobals.TSVP.SerialNumber", 0, &cSerial);
			if (hResult < 0)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = hResult;
				CA_GetAutomationErrorString(hResult, errorMessage, 1024);
				return;
			}
		}

	}



	if (! *pErrorOccurred)
	{
		gdStopTimeStamp = HRESTIM_getTimeStamp();
		menubarhandle = GetPanelMenuBar (giActualPanelHandle);
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_ACTUALTESTSTEP, ATTR_LABEL_TEXT, "Overall Status");
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_VARIANT_RING, ATTR_DIMMED, FALSE);

		sprintf(giAllTestResults[giActualTestResult]->cStopTime, TimeStr());
		sprintf(giAllTestResults[giActualTestResult]->cStopDate, cOwnDateStr());
		giAllTestResults[giActualTestResult]->cTestResult = (char) iResult;
		giAllTestResults[giActualTestResult]->dExecutionTime = gdStopTimeStamp - gdStartTimeStamp;
		giTestIsRunning = FALSE;

		if (giMSAType1IsEnabled)
		{
			giMSAType1ActualIndex--;
			if (giMSAType1ActualIndex == 0)
			{
				giMSAType1IsEnabled = FALSE;
				giMSAType1Reiterations = 0;
				SetMenuBarAttribute(menubarhandle, MENUBAR_PROGRAM_MODE_MSATYPE1, ATTR_CHECKED, VFALSE);
				SetMenuBarAttribute (menubarhandle, MENUBAR_PROGRAM_MODE_MSATYPE1, ATTR_ITEM_NAME, "MSA Type __1");
				SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_MSA_INDICATOR, ATTR_OFF_COLOR, VAL_GRAY);
				SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_MSA_INDICATOR, ATTR_LABEL_TEXT, "MSA Option");
				SetCtrlVal (giActualPanelHandle, OP_SINGLE_MSA_INDICATOR, 0);

				TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo,
										 "StationGlobals.TSVP.MSAType1.Enabled", TS_PropOption_InsertIfMissing, VFALSE);
				TS_PropertySetValNumber (sequenceContext, &pTSErrorInfo,
										 "StationGlobals.TSVP.MSAType1.Reiterations", TS_PropOption_InsertIfMissing, (double) giMSAType1Reiterations);
				TS_PropertySetValNumber (sequenceContext, &pTSErrorInfo,
										 "StationGlobals.TSVP.MSAType1.ActualIndex", TS_PropOption_InsertIfMissing, (double) giMSAType1ActualIndex);

				GetCtrlAttribute (giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, ATTR_LEFT, &iLeft);
				SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, ATTR_LEFT, iLeft - 25);

			}
			else
			{

				TS_PropertySetValNumber (sequenceContext, &pTSErrorInfo,
										 "StationGlobals.TSVP.MSAType1.ActualIndex", TS_PropOption_InsertIfMissing, (double) giMSAType1ActualIndex);
				sprintf(cTextBuffer, "MSA Option (%d)", giMSAType1ActualIndex);
				SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_MSA_INDICATOR, ATTR_LABEL_TEXT, cTextBuffer);
			}
		}


		if(!iResult)
		{
			giPassUUTs++;
			giCountOfUUTs++;
			giConsecutiveFailed = 0;
			if (pBench->SerialIsMandatory > 0)
			{
				sprintf(cTextBuffer, "TEST PASSED");
				CA_FreeMemory (cSerial);
			}
			else
			{
				sprintf(cTextBuffer, "TEST PASSED");
			}
			SetCtrlVal (giActualPanelHandle, OP_SINGLE_TESTSTATUS_MSG, cTextBuffer);
			if (lTrace) RESMGR_Trace("UUT passed");

			//Set BG Color to specified value
			GetCtrlVal (giActualPanelHandle, OP_SINGLE_COL_PASS, &iColorValue);
			GetCtrlVal (giActualPanelHandle, OP_SINGLE_USECOLOREDBG, &iEnableColor);
			if (iEnableColor)
			{
				SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_BGCOLOR, iColorValue);
			}
			SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TESTSTATUS_MSG, ATTR_TEXT_BGCOLOR, iColorValue);

		}
		else
		{
			giFailUUTs++;
			giCountOfUUTs++;
			giConsecutiveFailed++;
			if (pBench->SerialIsMandatory == 1)
			{
				sprintf(cTextBuffer, "TEST FAILED");
				CA_FreeMemory (cSerial);
			}
			else
			{
				sprintf(cTextBuffer, "TEST FAILED");
			}
			SetCtrlVal (giActualPanelHandle, OP_SINGLE_TESTSTATUS_MSG, cTextBuffer);
			if (lTrace) RESMGR_Trace("UUT failed");

			//Set BG Color to specified value
			GetCtrlVal (giActualPanelHandle, OP_SINGLE_COL_FAIL, &iColorValue);
			GetCtrlVal (giActualPanelHandle, OP_SINGLE_USECOLOREDBG, &iEnableColor);
			if (iEnableColor)
			{
				SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_BGCOLOR, iColorValue);
			}
			SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TESTSTATUS_MSG, ATTR_TEXT_BGCOLOR, iColorValue);
		}
		if (lTrace) RESMGR_Trace("Solve Q-Rate");
		//Solve quality rate
		gdQRate = (giPassUUTs * 100.0) / giCountOfUUTs,
		sprintf(cTraceBuffer, "Count: %i, Pass: %i, Fail: %i, Q: %.2f %%",  giCountOfUUTs, giPassUUTs, giFailUUTs, gdQRate);
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_ACTUALTESTSTEP, cTraceBuffer);
		if (lTrace) RESMGR_Trace(cTraceBuffer);
		//Check yield settings
		if (giYieldCheckEnabledMenu && giYieldCheckEnabledFile)
		{
			//If Q-Rate lower than allowed
			if ((gdQRate < (double) pBench->MinPassRate) && (pBench->AfterTestedBoards < giCountOfUUTs))
			{
				sprintf (cTraceBuffer, "Q-Rate is lower than allowed. Please inform your superior. Test programm is stopped.\n\n\
Actual yield settings:\nMinPassRate: %i %%\nAfterTestedBoards: %i\nConsecutiveTests: %i\n\n\
Actual yield:\nQ-Rate: %.2f %%\nTested Boards: %i\nPassed: %i\nFailed: %i\nConsecutive Failed: %i",
						 pBench->MinPassRate, pBench->AfterTestedBoards, pBench->ConsecutiveTests,
						 gdQRate, giCountOfUUTs, giPassUUTs, giFailUUTs, giConsecutiveFailed);
				MessagePopup ("CP-ITS ControlPanel - Yield control", cTraceBuffer);
				if (lTrace) RESMGR_Trace(cTraceBuffer);
				TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo,
										 "StationGlobals.TSVP.QuitProgram", TS_PropOption_InsertIfMissing, TRUE);
			}
			//If consecutive failed test greater than allowed
			else if (giConsecutiveFailed >= pBench->ConsecutiveTests)
			{
				sprintf (cTraceBuffer, "Consective failed tests exceeds selected maximum. Please inform your superior. Test programm is stopped.\n\n\
Actual yield settings:\nMinPassRate: %i %%\nAfterTestedBoards: %i\nConsecutiveTests: %i\n\n\
Actual yield:\nQ-Rate: %.2f %%\nTested Boards: %i\nPassed: %i\nFailed: %i\nConsecutive Failed: %i",
						 pBench->MinPassRate, pBench->AfterTestedBoards, pBench->ConsecutiveTests,
						 gdQRate, giCountOfUUTs, giPassUUTs, giFailUUTs, giConsecutiveFailed);
				MessagePopup ("CP-ITS ControlPanel - Yield control", cTraceBuffer);
				if (lTrace) RESMGR_Trace(cTraceBuffer);
				TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo,
										 "StationGlobals.TSVP.QuitProgram", TS_PropOption_InsertIfMissing, TRUE);
			}
			else
			{
				//Every thing is o.k.
				if (lTrace) RESMGR_Trace("Every thing is o.k.");
				if (giRunButtonDisabled) SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_DIMMED, TRUE);
				SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_LABEL_TEXT, "RUN TEST [ENTER]");
				SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_SHORTCUT_KEY, VAL_ENTER_VKEY);
	 			if (giMSAType1IsEnabled && !giRunButtonDisabled)
				{
					ResetTimer (giActualPanelHandle, OP_SINGLE_TIMER_MSA_RUNBUTTON);
					SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TIMER_MSA_RUNBUTTON, ATTR_ENABLED, TRUE);
				}
			}
		}
		else
		{
			//Yield checking not enabled
			if (lTrace) RESMGR_Trace("Yield checking not enabled");
			if (giRunButtonDisabled) SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_DIMMED, TRUE);
			SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_LABEL_TEXT, "RUN TEST [ENTER]");
			SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_SHORTCUT_KEY, VAL_ENTER_VKEY);
	 			
	 
			if (giMSAType1IsEnabled && !giRunButtonDisabled)
			{
				ResetTimer (giActualPanelHandle, OP_SINGLE_TIMER_MSA_RUNBUTTON);
				SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TIMER_MSA_RUNBUTTON, ATTR_ENABLED, TRUE);
			}
		}
		//Some panel settings
		if (lTrace) RESMGR_Trace("Reset UIR");
		if(giSerialIsMandatory) SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_SERIAL, ATTR_DIMMED, FALSE);
		if(giSerialIsMandatory && !giMSAType1IsEnabled) SetCtrlVal (giActualPanelHandle, OP_SINGLE_SERIAL, "");
		//Insert execution time in operator user interface
		dExecTime = gdStopTimeStamp - gdStartTimeStamp;
		if (pBench->DisableExecutionTime == 0) 
		{
			InsertListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, -1, "", 1);
			giActualItemIndex++;
			
			sprintf(cTraceBuffer, "\033fg%06XTotal execution time: %.3f seconds", VAL_BLACK, dExecTime); 
			InsertListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, -1, cTraceBuffer, 1);
			giActualItemIndex++;
			InsertListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, -1, "", 1);
			giActualItemIndex++;
			
			if (SETINDEX) SetCtrlIndex(giActualTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
		
		}
		
		iMenuID = -1;
		while (iMenuID != 0)
		{
			if(iMenuID == -1)
			{
				GetMenuBarAttribute (menubarhandle, 0, ATTR_FIRST_MENU_ID, &iMenuID);
			}
			else
			{
				GetMenuBarAttribute (menubarhandle, iMenuID, ATTR_NEXT_MENU_ID, &iMenuID);
			}
			if(iMenuID) SetMenuBarAttribute(menubarhandle, iMenuID, ATTR_DIMMED, FALSE);
		}

		SetActiveCtrl(giActualPanelHandle, OP_SINGLE_SERIAL);

		//Check for demo mode
		if ((pBench->iDemoMode) && (giCountOfUUTs > 3))
			SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_DIMMED, TRUE);
		if (pBench->iDemoMode)
		{
			SetCtrlVal (giActualPanelHandle, OP_SINGLE_TESTSTATUS_MSG, "DEMOMODE");
		}
	}

	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_DisplayResult end");
	}

}

/* FUNCTION *****************************************************************/
/**
tspan_SetActualStepName
*
@brief Set a text in the actual test step resource
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param actualStepName: Test message (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_SetActualStepName (CAObjHandle sequenceContext,
										long pResourceId,
										char actualStepName[],
										short *pErrorOccurred,
										long *pErrorCode,
										char errorMessage[])
{
	char cTraceBuffer[1024];
	char cStepNameWithOutTabs[1024];
	long lTrace;



	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_SetActualStepName begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred) //
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}

	if (!*pErrorOccurred && (pBench->iMode == OPERATOR_MODE))
	{
		iKillTabInStepName (actualStepName, cStepNameWithOutTabs, strlen(actualStepName));
		SetCtrlVal (giActualPanelHandle, OP_SINGLE_ACTUALTESTSTEP, cStepNameWithOutTabs);
		if (lTrace)
		{
			sprintf(cTraceBuffer, "Actual step name: %s",  cStepNameWithOutTabs);
			RESMGR_Trace(cTraceBuffer);
		}
	}

	

	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_SetActualStepName end");
	}
}

/* FUNCTION *****************************************************************/
/**
tspan_InsertTextIntoTestReport
*
@brief Insert a text message into test report resource
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cText: text message, use "\n" for a new line (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_InsertTextIntoTestReport (CAObjHandle sequenceContext,
		long pResourceId, char cText[],
		short *pErrorOccurred,
		long *pErrorCode,
		char errorMessage[])
{
	char cTraceBuffer[1024];
	char cTempBuffer[1024];
	long lCountOfLines;
	long lTrace;

	int idx, idy, idz;
	int iCountOfLF;
	int iStringLength;
	
	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_InsertTextIntoTestReport begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}

	if (!*pErrorOccurred && (pBench->iMode == OPERATOR_MODE))
	{
		
		iStringLength = strlen(cText);
		iCountOfLF = 0;
		for (idx = 0; idx < iStringLength; idx++)
		{
			if (cText[idx] == '\n') iCountOfLF++;
		}
		if (iCountOfLF)
		{ 
			idx = 0;
			idz = 0;
			while (cText[idx] != '\0')
			{
				if(cText[idx] == '\n')
				{
					if (idz > 0)
					{
						cTempBuffer[idz] = '\0';
						sprintf(cTraceBuffer, "\033fg%06X%s", VAL_BLACK, cTempBuffer); 
						InsertListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, -1, cTraceBuffer, 1);
						giActualItemIndex++;
						idz = 0;
					}
					InsertListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, -1, "", 1);
					giActualItemIndex++;
					idx++; 
				} else {
					cTempBuffer[idz] = cText[idx];
					idz++;
					idx++;
				}	
			}
		} else {
			sprintf(cTraceBuffer, "\033fg%06X%s", VAL_BLACK, cText); 
			InsertListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, -1, cTraceBuffer, 1);
			giActualItemIndex++;
		
		}
		
		if (SETINDEX) SetCtrlIndex(giActualTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
		
		if (lTrace)
		{
			sprintf(cTraceBuffer, "Text: %s",  cText);
			RESMGR_Trace(cTraceBuffer);
		}
	}

	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_InsertTextIntoTestReport end");
	}
}

/* FUNCTION *****************************************************************/
/**
tspan_HideActualPanel
*
@brief Hide the actual displayed gui
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_HideActualPanel(CAObjHandle sequenceContext, long
									 pResourceId, short *pErrorOccurred, long *pErrorCode, char errorMessage[])
{

	char cTraceBuffer[1024];

	long lTrace;


	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);


	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_HideActualPanel begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}

	if (pBench->StrokeCounterEnabled && (pBench->iMode == OPERATOR_MODE))
		GetCtrlVal (giActualPanelHandle, OP_SINGLE_STROKE_ACT, &pBench->StrokeActualCounts);

	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace("InterlockedDecrement");
		}
		// Ensure only 1 thread updates thread_count at a time
		InterlockedDecrement(&iThreadCount);

		if (lTrace)
		{
			RESMGR_Trace("EnterCriticalSection");
		}

		EnterCriticalSection(&CriticalSection);
		if (iThreadCount > 0)
		{
			LeaveCriticalSection(&CriticalSection);
			return ;
		}

		if (lTrace)
		{
			RESMGR_Trace("LeaveCriticalSection");
		}

		LeaveCriticalSection(&CriticalSection);

		if (lTrace)
		{
			RESMGR_Trace("PostDeferredCallToThread");
		}

		// Call function in new thread to quit new thread
		PostDeferredCallToThread(QuitThread, 0, pBench->gThreadID);

		if (hThreadHandle != NULL)
		{
			// wait for the thread to complete.
			WaitForSingleObject(hThreadHandle, INFINITE);
			CloseHandle(hThreadHandle);
		}

		DiscardPanel(pBench->iActualPanelHandle);
		sprintf(gcSelectedVariant, "");


	}

	if (lTrace) RESMGR_Trace("Set panel mode OFF");
	pBench->iMode = OFF_MODE;

	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_HideActualPanel end");
	}

}

/* FUNCTION *****************************************************************/
/**
tspan_DisplayNumericValue
*
@brief Display a numeric limit test in the actual displayed GUI
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cNameOfStep: Name of test step (Input)
@param cCommentOfStep: Comment of test step (Input)
@param cUnit: Unit of test step (Input)
@param cCompType: Comaprision type of test step, eg "GELE" (Input)
@param cFormat: Format string of given double values (Input)
@param dValue: Measurement value (Input)
@param dLowerLimit: Lower limit (Input)
@param dUpperLimit: Upper limit (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_DisplayNumericValue(CAObjHandle sequenceContext, long pResourceId,
		char cNameOfStep[], char cCommentOfStep[], char cUnit[], char cCompType[], char cFormat[], double dValue,
		double dLowerLimit, double dUpperLimit, short *pErrorOccurred, long
		*pErrorCode, char errorMessage[])
{

	int iTabHandle;
	char cTraceBuffer[1024];
	char cStepNameWithOutTabs[1024];
	long lTrace;
	long lCompType;
	long lItemsFormatted;

	double dMean;
	double dDev;
	double dCP, dCPKUL, dCPKLL;
	long lCountOfLines;

	int iComparisionResult;

	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_DisplayNumericValue begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}
	
	//Change all tabs in step name to whitespaces
	if (! *pErrorOccurred)
	{
		iKillTabInStepName (cNameOfStep, cStepNameWithOutTabs, strlen(cNameOfStep));
	}

	//Solve Comp-Type
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace(">>Parameters begin");
			sprintf(cTraceBuffer, "cNameOfStep: %s", cNameOfStep);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cCommentOfStep: %s", cCommentOfStep);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cUnit: %s", cUnit);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cCompType: %s", cCompType);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cFormat: %s", cFormat);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "dValue: %f", dValue);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "dLowerLimit: %f", dLowerLimit);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "dUpperLimit: %f", dUpperLimit);
			RESMGR_Trace(cTraceBuffer);
			RESMGR_Trace("<<Parameters end");
		}


		if (!strcmp(cCompType, "EQ")) lCompType = COMP_EQ;
		else if (!strcmp(cCompType, "NE")) lCompType = COMP_NEQ;
		else if (!strcmp(cCompType, "GT")) lCompType = COMP_GT;
		else if (!strcmp(cCompType, "LT")) lCompType = COMP_LT;
		else if (!strcmp(cCompType, "GE")) lCompType = COMP_GE;
		else if (!strcmp(cCompType, "LE")) lCompType = COMP_LE;
		else if (!strcmp(cCompType, "GTLT")) lCompType = COMP_GTLT;
		else if (!strcmp(cCompType, "GELE")) lCompType = COMP_GELE;
		else if (!strcmp(cCompType, "GELT")) lCompType = COMP_GELT;
		else if (!strcmp(cCompType, "GTLE")) lCompType = COMP_GTLE;
		else if (!strcmp(cCompType, "LTGT")) lCompType = COMP_LTGT;
		else if (!strcmp(cCompType, "LEGE")) lCompType = COMP_LEGE;
		else if (!strcmp(cCompType, "LEGT")) lCompType = COMP_LEGT;
		else if (!strcmp(cCompType, "LTGE")) lCompType = COMP_LTGE;
		else if (!strcmp(cCompType, "LOG")) lCompType = COMP_NOCOMP;
		else
		{
			if (pBench->iOwner != TSPAN_ERR_BASE)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
				formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
			}
		}
		if ((lCompType != lLastCompType) && (pBench->iMode == DEBUG_MODE))
		{
			lLastCompType = lCompType;
			SetDebugUirNum (giActualPanelHandle);
		}
	}


	if (! *pErrorOccurred)
	{
		if (pBench->iMode == DEBUG_MODE)
		{
			if (strcmp(cLastNameOfStep, cStepNameWithOutTabs))
			{
				if (lTrace) RESMGR_Trace("Reset uir, because of different step name");
				sprintf(cLastNameOfStep, cStepNameWithOutTabs);
				//Analog
				GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 0, &iTabHandle);
				DeleteGraphPlot (iTabHandle, ANALOG_ANALOG, -1, VAL_IMMEDIATE_DRAW);
				//Histogramm
				GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 1, &iTabHandle);
				DeleteGraphPlot (iTabHandle, HISTOGRAM_HISTOGRAMM, -1, VAL_IMMEDIATE_DRAW);
				//Graph
				GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 2, &iTabHandle);
				DeleteGraphPlot (iTabHandle, GRAPH_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				giMinValue = 0;
				giMaxValue = 0;
				giLowerLimit = 0;
				giUpperLimit = 0;
				ResetMeasurements();
				if(!ShowCPKValues_Enabled)
				{
					SetCtrlVal (giActualPanelHandle, DEBUG_CPK, cStepNameWithOutTabs);
				}

			}
			//Settings for analog
			GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 0,
									  &iTabHandle);

			/*SetCtrlAttribute(iTabHandle, ANALOG_MEASUREMENT, ATTR_MIN_VALUE,
			   dLowerLimit);
			SetCtrlAttribute(iTabHandle, ANALOG_MEASUREMENT, ATTR_MAX_VALUE,
				dUpperLimit);*/
			//SetCtrlVal(iTabHandle, ANALOG_MEASUREMENT, dValue);
			SetCtrlVal(iTabHandle, ANALOG_UNIT, cUnit);
			if (lTrace) RESMGR_Trace("Format data");
			if (strlen(cFormat))
			{
				lItemsFormatted = sprintf (cTraceBuffer, cFormat, dValue);
				if (lItemsFormatted <= 0)
				{
					*pErrorOccurred = TRUE;
					*pErrorCode = TSPAN_ERR_WRONGFORMATYTE;
					formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
					goto ErrorTag;
				}
				FormatValues (cTraceBuffer, cFormat, dValue);
			}
			else
			{
				sprintf(cTraceBuffer, "%.9f", dValue);
			}
			if (lTrace) RESMGR_Trace("Set analog value");
			SetCtrlVal(iTabHandle, ANALOG_VALUE, cTraceBuffer);
			if (lTrace) RESMGR_Trace(cTraceBuffer);
			//Plot lower limit
			if ((gdLowerLimit != dLowerLimit) || !giActualIndex)
			{
				DeleteGraphPlot (iTabHandle, ANALOG_ANALOG, giLowerLimit, VAL_IMMEDIATE_DRAW);
				giLowerLimit = iPlotBarForAnalog (iTabHandle, dLowerLimit, VAL_RED);
				gdLowerLimit = dLowerLimit;
				if (lTrace) RESMGR_Trace("Plot lower limit into graph");
			}
			//Plot upper limit
			if (lCompType > 6)
			{
				if ((gdUpperLimit != dUpperLimit) || !giActualIndex)
				{
					DeleteGraphPlot (iTabHandle, ANALOG_ANALOG, giUpperLimit, VAL_IMMEDIATE_DRAW);
					giUpperLimit = iPlotBarForAnalog (iTabHandle, dUpperLimit, VAL_RED);
					gdUpperLimit = dUpperLimit;
					if (lTrace) RESMGR_Trace("Plot upper limit into graph");

				}
			}
			else
			{
				giUpperLimit = 0;
			}
			//Plot max value
			if ((gdMaxValue < dValue) || !giActualIndex)
			{
				DeleteGraphPlot (iTabHandle, ANALOG_ANALOG, giMaxValue, VAL_IMMEDIATE_DRAW);
				giMaxValue = iPlotBarForAnalog (iTabHandle, dValue, VAL_YELLOW);
				gdMaxValue = dValue;
				if (lTrace) RESMGR_Trace("Plot max value into graph");

			}

			//Plot min value
			if ((gdMinValue > dValue) || !giActualIndex)
			{
				DeleteGraphPlot (iTabHandle, ANALOG_ANALOG, giMinValue, VAL_IMMEDIATE_DRAW);
				giMinValue = iPlotBarForAnalog (iTabHandle, dValue, VAL_YELLOW);
				gdMinValue = dValue;
				if (lTrace) RESMGR_Trace("Plot min into graph");

			}
			DeleteGraphPlot (iTabHandle, ANALOG_ANALOG, giActualValue, VAL_IMMEDIATE_DRAW);
			giActualValue = iPlotBarForAnalog (iTabHandle, dValue, VAL_WHITE);
			if (lTrace) RESMGR_Trace("Compare data -> debug mode");
			switch (lCompType)
			{
				case COMP_EQ:
					iComparisionResult = iCompareEQ(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_NEQ:
					iComparisionResult = iCompareNEQ(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GT:
					iComparisionResult = iCompareGT(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LT:
					iComparisionResult = iCompareLT(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GE:
					iComparisionResult = iCompareGE(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LE:
					iComparisionResult = iCompareLE(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GTLT:
					iComparisionResult = iCompareGTLT(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GELE:
					iComparisionResult = iCompareGELE(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GELT:
					iComparisionResult = iCompareGELT(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GTLE:
					iComparisionResult = iCompareGTLE(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LTGT:
					iComparisionResult = iCompareLTGT(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LEGE:
					iComparisionResult = iCompareLEGE(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LEGT:
					iComparisionResult = iCompareLEGT(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LTGE:
					iComparisionResult = iCompareLTGE(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_NOCOMP:
					iComparisionResult = iCompareNOCOMP(iTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				default:
					iComparisionResult = iCompareGELE(iTabHandle, dValue, pBench->iMode, pBench->SerialIsMandatory, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
			}

			//Array
			gdMeasurements[giActualIndex] = dValue;
			giActualIndex++;
			if (giActualIndex > MEAS_ARRAY_SIZE - 1)  giActualIndex = 1;
			SetActiveTabPage (giActualPanelHandle, DEBUG_TAB, 0);
			if (ShowCPKValues_Enabled && (giActualIndex > 5))
			{
				if (lTrace) RESMGR_Trace("Solve cpk values");
				StdDev (gdMeasurements, giActualIndex - 1, &dMean, &dDev);
				if (lCompType > 6) dCP = (dUpperLimit - dLowerLimit) / (6 * dDev);
				dCPKLL = (dMean - dLowerLimit) / (3 * dDev);
				if (lCompType > 6) dCPKUL = (dUpperLimit - dMean) / (3 * dDev);
				if (lCompType > 6)
				{
					if (dCP > 1000)
					{
						sprintf(cTraceBuffer, "cp: %.2E - cpk LL: %.2E - cpk UL: %.2E", dCP , dCPKLL, dCPKUL);
					}
					else
					{
						sprintf(cTraceBuffer, "cp: %.2f - cpk LL: %.2f - cpk UL: %.2f", dCP , dCPKLL, dCPKUL);
					}

				}
				else
				{
					if (dCP > 1000)
					{
						sprintf(cTraceBuffer, "cp: %.2E - cpk LL: %.2E", dCP , dCPKLL);
					}
					else
					{
						sprintf(cTraceBuffer, "cp: %.2f - cpk LL: %.2f", dCP , dCPKLL);
					}


				}
				SetCtrlVal (giActualPanelHandle, DEBUG_CPK, cTraceBuffer);
				if (lTrace) RESMGR_Trace(cTraceBuffer);

			}
		}
		else if (pBench->iMode == OPERATOR_MODE)
		{
			if (lTrace) RESMGR_Trace("Compare data -> operator mode");
			switch (lCompType)
			{
				case COMP_EQ:
					iComparisionResult = iCompareEQ(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_NEQ:
					iComparisionResult = iCompareNEQ(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GT:
					iComparisionResult = iCompareGT(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LT:
					iComparisionResult = iCompareLT(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GE:
					iComparisionResult = iCompareGE(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LE:
					iComparisionResult = iCompareLE(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GTLT:
					iComparisionResult = iCompareGTLT(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GELE:
					iComparisionResult = iCompareGELE(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GELT:
					iComparisionResult = iCompareGELT(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_GTLE:
					iComparisionResult = iCompareGTLE(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LTGT:
					iComparisionResult = iCompareLTGT(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LEGE:
					iComparisionResult = iCompareLEGE(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LEGT:
					iComparisionResult = iCompareLEGT(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_LTGE:
					iComparisionResult = iCompareLTGE(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				case COMP_NOCOMP:
					iComparisionResult = iCompareNOCOMP(giActualTabHandle, pBench->iMode, pBench->SerialIsMandatory, dValue, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;
				default:
					iComparisionResult = iCompareGELE(giActualTabHandle, dValue, pBench->iMode, pBench->SerialIsMandatory, dLowerLimit, dUpperLimit, cStepNameWithOutTabs, cUnit, cFormat);
					break;

			}
			
			giAllTestResults[giActualTestResult]->pFirst = pAppendTestStep (giAllTestResults[giActualTestResult]->pFirst,
					0, cStepNameWithOutTabs, cCommentOfStep, NULL, dValue, dLowerLimit, dUpperLimit, cUnit, cCompType, cFormat, iComparisionResult, NULL, NULL);
			
		}
		else
		{
			return ;
		}

	}

ErrorTag:

	//ProcessDrawEvents();

	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_DisplayNumericValue end");
	}

}

/* FUNCTION *****************************************************************/
/**
tspan_DisplayPassFail
*
@brief Display a PASS/FAIL test in the actual displayed GUI
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cNameOfStep: Name of test step (Input)
@param cCommentOfStep: Comment of test step (Input)
@param lResult: Test result of the test step 0 -> FAIL, 1 -> PASS (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_DisplayPassFail (CAObjHandle sequenceContext,
									  long pResourceId, char cNameOfStep[],
									  char cCommentOfStep[], int lResult,
									  short *pErrorOccurred, long *pErrorCode,
									  char errorMessage[])
{

	char cTraceBuffer[1024];
	char cStepNameWithOutTabs[1024];
	char cErrorLogTraceBuffer[1024]; 
	
	int iTabHandle;
	long lTrace;
	long lCountOfLines;

	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_DisplayPassFail begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}
	
	//Change all tabs in step name to whitespaces
	if (! *pErrorOccurred)
	{
		iKillTabInStepName (cNameOfStep, cStepNameWithOutTabs, strlen(cNameOfStep));
	}

	if (!*pErrorOccurred)
	{
		//Tracing parameters
		if (lTrace)
		{
			RESMGR_Trace(">>Parameters begin");
			sprintf(cTraceBuffer, "cNameOfStep: %s", cNameOfStep);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cCommentOfStep: %s", cCommentOfStep);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "lResult: %i", lResult);
			RESMGR_Trace(cTraceBuffer);
			RESMGR_Trace("<<Parameters end");
		}
		//Check last comp type
		if ((lLastCompType != COMP_PAFA) && (pBench->iMode == DEBUG_MODE))
		{
			SetDebugUirPafa (giActualPanelHandle);
			lLastCompType = COMP_PAFA;
		}
		if(pBench->iMode == DEBUG_MODE)
		{
			if (lTrace) RESMGR_Trace("Compare data -> debug mode");
			if (strcmp(cLastNameOfStep, cStepNameWithOutTabs))
			{
				if (lTrace) RESMGR_Trace("Reset uir, because of different step name");
				sprintf(cLastNameOfStep, cStepNameWithOutTabs);
				//Analog
				GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 0, &iTabHandle);
				DeleteGraphPlot (iTabHandle, ANALOG_ANALOG, -1, VAL_IMMEDIATE_DRAW);
				//Histogramm
				GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 1, &iTabHandle);
				DeleteGraphPlot (iTabHandle, HISTOGRAM_HISTOGRAMM, -1, VAL_IMMEDIATE_DRAW);
				//Graph
				GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 2, &iTabHandle);
				DeleteGraphPlot (iTabHandle, GRAPH_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				giMinValue = 0;
				giMaxValue = 0;
				giLowerLimit = 0;
				giUpperLimit = 0;
				ResetMeasurements();
				SetCtrlVal (giActualPanelHandle, DEBUG_CPK, cStepNameWithOutTabs);
			}
			if (lTrace) RESMGR_Trace("Display result in uir");
			GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 0, &iTabHandle);
			if (lResult)
			{
				SetCtrlVal(iTabHandle, ANALOG_RED, 0);
				SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
				SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
			}
			else
			{
				SetCtrlVal(iTabHandle, ANALOG_RED, 1);
				SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
				SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
			}
		}
		if(pBench->iMode == OPERATOR_MODE)
		{
			if (lTrace) RESMGR_Trace("Compare data -> operator mode");
			
			if (lResult) sprintf(cTraceBuffer, "\033fg%06X%s: Pass/Fail -> PASS", VAL_BLACK, cStepNameWithOutTabs);
			if (!lResult) sprintf(cTraceBuffer, "\033fg%06X%s: Pass/Fail -> FAIL", 0xDD0000, cStepNameWithOutTabs);
			InsertListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, -1, cTraceBuffer, 1);
			giActualItemIndex++;
			if (SETINDEX) SetCtrlIndex(giActualTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);

			if (giErrorLogWindowEnabled && !lResult)
			{
				if(pBench->SerialIsMandatory)
				{
					sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
				} else {
					sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
				}
				InsertListItem (giActualTabHandle, TABPANEL_TESTERRORLOG, -1, cErrorLogTraceBuffer, 1);
				giActualErrorLogItemIndex++;
				if (SETINDEX) SetCtrlIndex(giActualTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
			}
			
			
			giAllTestResults[giActualTestResult]->pFirst = pAppendTestStep (giAllTestResults[giActualTestResult]->pFirst,
					1, cStepNameWithOutTabs, cCommentOfStep, NULL, 0.0, 0.0, 0.0, NULL, NULL, NULL, lResult, NULL, NULL);

		}
	}

	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_DisplayPassFail end");
	}
}

/* FUNCTION *****************************************************************/
/**
tspan_DisplayString
*
@brief Display a string value test in the actual displayed GUI
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cNameOfStep: Name of test step (Input)
@param cCommentOfStep: Comment of test step (Input)
@param cCompType: NI Teststand comparision type, eg "CaseSensitive" (Input)
@param cStringToCompare: String value to compare (Input)
@param cExpectedString: Expected string value (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_DisplayString (CAObjHandle sequenceContext, long pResourceId,
									char cNameOfStep[], char cCommentOfStep[],
									char cCompType[], char cStringToCompare[],
									char cExpectedString[], short *pErrorOccurred,
									long *pErrorCode, char errorMessage[])
{
	char cTraceBuffer[1024];
	char cStepNameWithOutTabs[1024];
	char cErrorLogTraceBuffer[1024];
	
	int iTabHandle;
	long lTrace;
	long lCountOfLines;

	long lResult = 0;

	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_DisplayString begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (!*pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}
	
	//Change all tabs in step name to whitespaces
	if (! *pErrorOccurred)
	{
		iKillTabInStepName (cNameOfStep, cStepNameWithOutTabs, strlen(cNameOfStep));
	}

	if (!*pErrorOccurred)
	{
		//Tracing parameters
		if (lTrace)
		{
			RESMGR_Trace(">>Parameters begin");
			sprintf(cTraceBuffer, "cNameOfStep: %s", cNameOfStep);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cCommentOfStep: %s", cCommentOfStep);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cCompType: %s", cCompType);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cStringToCompare: %s", cStringToCompare);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cExpectedString: %s", cExpectedString);
			RESMGR_Trace(cTraceBuffer);
			RESMGR_Trace("<<Parameters end");
		}
		//Check last comp type
		if ((lLastCompType > -2) && (pBench->iMode == DEBUG_MODE))
		{
			SetDebugUirString (giActualPanelHandle);
		}
		if (!strcmp("CaseSensitive", cCompType))
		{
			lLastCompType = COMP_STRN;
		}
		else
		{
			lLastCompType = COMP_STRI;
		}
		if(pBench->iMode == DEBUG_MODE)
		{
			if (lTrace) RESMGR_Trace("Compare data -> debug mode");
			if (strcmp(cLastNameOfStep, cStepNameWithOutTabs))
			{
				if (lTrace) RESMGR_Trace("Reset uir, because of different step name");
				sprintf(cLastNameOfStep, cStepNameWithOutTabs);
				//Analog
				GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 0, &iTabHandle);
				DeleteGraphPlot (iTabHandle, ANALOG_ANALOG, -1, VAL_IMMEDIATE_DRAW);
				//Histogramm
				GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 1, &iTabHandle);
				DeleteGraphPlot (iTabHandle, HISTOGRAM_HISTOGRAMM, -1, VAL_IMMEDIATE_DRAW);
				//Graph
				GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 2, &iTabHandle);
				DeleteGraphPlot (iTabHandle, GRAPH_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				giMinValue = 0;
				giMaxValue = 0;
				giLowerLimit = 0;
				giUpperLimit = 0;
				ResetMeasurements();
				SetCtrlVal (giActualPanelHandle, DEBUG_CPK, cStepNameWithOutTabs);
			}
			if (lTrace) RESMGR_Trace("Display result in uir");
			GetPanelHandleFromTabPage(giActualPanelHandle, DEBUG_TAB, 0, &iTabHandle);
			ResetTextBox (iTabHandle, ANALOG_STRING, "");
			if (lLastCompType == COMP_STRN)
			{
				if (strcmp(cStringToCompare, cExpectedString))
				{
					SetCtrlVal(iTabHandle, ANALOG_RED, 1);
					SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
					SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
				}
				else
				{
					SetCtrlVal(iTabHandle, ANALOG_RED, 0);
					SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
					SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
				}
				sprintf(cTraceBuffer, "String: \"%s\"", cStringToCompare);
				InsertTextBoxLine (iTabHandle, ANALOG_STRING, -1, cTraceBuffer);
				sprintf(cTraceBuffer, "Limit : \"%s\"", cExpectedString);
				InsertTextBoxLine (iTabHandle, ANALOG_STRING, -1, cTraceBuffer);
				InsertTextBoxLine (iTabHandle, ANALOG_STRING, -1, "Type: Case Sensitive");
			}
			if (lLastCompType == COMP_STRI)
			{
				if (stricmp(cStringToCompare, cExpectedString))
				{
					SetCtrlVal(iTabHandle, ANALOG_RED, 1);
					SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
					SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
				}
				else
				{
					SetCtrlVal(iTabHandle, ANALOG_RED, 0);
					SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
					SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
				}
				sprintf(cTraceBuffer, "String: \"%s\"", cStringToCompare);
				InsertTextBoxLine (iTabHandle, ANALOG_STRING, -1, cTraceBuffer);
				sprintf(cTraceBuffer, "Limit : \"%s\"", cExpectedString);
				InsertTextBoxLine (iTabHandle, ANALOG_STRING, -1, cTraceBuffer);
				InsertTextBoxLine (iTabHandle, ANALOG_STRING, -1, "Type: Ignore Case");
			}
		}
		if(pBench->iMode == OPERATOR_MODE)
		{
			if (lTrace) RESMGR_Trace("Compare data -> operator mode");
			if (lLastCompType == COMP_STRN)
			{
				if (strcmp(cStringToCompare, cExpectedString))
				{
					sprintf(cTraceBuffer, "\033fg%06X%s: \"%s\" (Limit: \"%s\", Case Sensitive) -> FAIL", 0xDD0000, cStepNameWithOutTabs, cStringToCompare, cExpectedString);
				}
				else
				{
					sprintf(cTraceBuffer, "\033fg%06X%s: \"%s\" (Limit: \"%s\", Case Sensitive) -> PASS", VAL_BLACK, cStepNameWithOutTabs, cStringToCompare, cExpectedString);
					lResult++;
				}
			}
			if (lLastCompType == COMP_STRI)
			{
				if (stricmp(cStringToCompare, cExpectedString))
				{
					sprintf(cTraceBuffer, "\033fg%06X%s: \"%s\" (Limit: \"%s\", Ignore Case) -> FAIL", 0xDD0000, cStepNameWithOutTabs, cStringToCompare, cExpectedString);
				}
				else
				{
					sprintf(cTraceBuffer, "\033fg%06X%s: \"%s\" (Limit: \"%s\", Ignore Case) -> PASS", VAL_BLACK, cStepNameWithOutTabs, cStringToCompare, cExpectedString);
					lResult++;
				}
			}

			InsertListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, -1, cTraceBuffer, 1);
			giActualItemIndex++;
			if (SETINDEX) SetCtrlIndex(giActualTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
			
			if (giErrorLogWindowEnabled && !lResult)
			{
				if(pBench->SerialIsMandatory)
				{
					sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
				} else {
					sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
				}
				InsertListItem (giActualTabHandle, TABPANEL_TESTERRORLOG, -1, cErrorLogTraceBuffer, 1);
				giActualErrorLogItemIndex++;
				if (SETINDEX) SetCtrlIndex(giActualTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
			}
			
			giAllTestResults[giActualTestResult]->pFirst = pAppendTestStep (giAllTestResults[giActualTestResult]->pFirst,
					2, cStepNameWithOutTabs, cCommentOfStep, NULL, 0.0, 0.0, 0.0, NULL, cCompType, NULL, lResult, cStringToCompare, cExpectedString);
		}
	}

	//ProcessDrawEvents();

	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_DisplayString end");
	}
}

/* FUNCTION *****************************************************************/
/**
tspan_DisplayInCircuitTest
*
@brief Display the results of last executed in-circuit-test of R&S CompactTSVP in  displayed GUI
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cNameOfStep: Name of test step (Input)
@param cCommentOfStep: Comment of test step (Input)
@param ict_ID: ICT resource ID that you obtained vom ict.dll
@param ictProgram_ID: ICT program ID that you obtained vom ict.dll
@param cFormat: format string of all double values (Input)
@param iOnlyFailedTests: 1-> only failed test are reported, 0-> all test are reported (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_DisplayInCircuitTest (CAObjHandle sequenceContext,
                                          long pResourceId, char cNameOfStep[],
                                          char cCommentOfStep[], int ict_ID, int ictProgram_ID, char cFormat[], int iOnlyFailedTests,
                                          short *pErrorOccurred,
											long *pErrorCode, char errorMessage[])
{

	int iTabHandle;
	char cTraceBuffer[1024];
	char cStepNameWithOutTabs[1024];
	
	long lTrace;
	long lCompType;
	long lItemsFormatted;

	double dMean;
	double dDev;
	double dCP, dCPKUL, dCPKLL;
	long lCountOfLines;
	
	long NumberOfEntries;
	int iLoopIndex;
	
	char cNameOfStepExt[1024];
	char cTestName[1024];
	char cTestSatus[1024];
	int iTestID;
	char cPartID[1024];
	char cTimeStamp[1024];
	double dValue;
	double dLowerLimit;
	double dUpperLimit;
	char cUnit[1024];
	char cReportText[1024];
	double dDuration;
	
	char cPrintBuffer[1024];
    char cErrorLogTraceBuffer[1024];
    char cLowerLimit[1024];
    char cUpperLimit[1024];
    char cValue[1024];
   
	char cCompleteReportText[300000];
	
	int iComparisionResult;

	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>tspan_DisplayInCircuitTest begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}

	//Change all tabs in step name to whitespaces
	if (! *pErrorOccurred)
	{
		iKillTabInStepName (cNameOfStep, cStepNameWithOutTabs, strlen(cNameOfStep));
	}
	
	//Solve Comp-Type
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace(">>Parameters begin");
			sprintf(cTraceBuffer, "cNameOfStep: %s", cNameOfStep);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cCommentOfStep: %s", cCommentOfStep);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "ict_ID: %d", ict_ID);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "ictProgram_ID: %d", ictProgram_ID);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cFormat: %s", cFormat);
			RESMGR_Trace(cTraceBuffer);
			RESMGR_Trace("<<Parameters end");
		}

	}


	if (!*pErrorOccurred)
	{
		if (pBench->iMode == DEBUG_MODE)
		{
			//do nothing ... ict cannot be executed in debug mode...
		}
		else if (pBench->iMode == OPERATOR_MODE)
		{
			if (lTrace) RESMGR_Trace("Compare data -> operator mode");
			
			if (iOnlyFailedTests)
			{	
				ICT_Load_Detailed_Report (sequenceContext, ict_ID, ictProgram_ID, 1, &NumberOfEntries, &pErrorOccurred, &pErrorCode, errorMessage);
			} else {
				ICT_Load_Detailed_Report (sequenceContext, ict_ID, ictProgram_ID, 0, &NumberOfEntries, &pErrorOccurred, &pErrorCode, errorMessage);
			}
			if (NumberOfEntries <= 0) goto ErrorTag;
			
			cCompleteReportText[0] = '\0';
			
			for (iLoopIndex = 0; iLoopIndex < NumberOfEntries; iLoopIndex++)
			{
			
				ICT_Get_Detailed_Report_Entry (sequenceContext, ict_ID, ictProgram_ID, iLoopIndex, cTestName, cTestSatus, &iTestID, cPartID, cTimeStamp, &dValue, &dLowerLimit, &dUpperLimit, cUnit, cReportText, &dDuration, &pErrorOccurred, &pErrorCode, errorMessage);
				
				if (strlen(cFormat))
				{
				 FormatValues (cValue, cFormat, dValue);
				 FormatValues (cLowerLimit, cFormat, dLowerLimit);
				 FormatValues (cUpperLimit, cFormat, dUpperLimit);
				}
				else
				{
				 sprintf(cValue, "%.9f", dValue);
				 sprintf(cLowerLimit, "%.9f", dLowerLimit);
				 sprintf(cUpperLimit, "%.9f", dUpperLimit);
				}
				
				iComparisionResult = 0;
				sprintf (cNameOfStepExt, "%s - %s", cStepNameWithOutTabs, cTestName);
				if ((dValue >= dLowerLimit) && (dValue <= dUpperLimit))
				{
				 sprintf(cPrintBuffer, "\033fg%06X%s: %s %s (%s <= x <= %s) -> PASS\n", VAL_BLACK, cNameOfStepExt,
				    cValue, cUnit, cLowerLimit, cUpperLimit);
				 iComparisionResult++;
				}
				else
				{
				 sprintf(cPrintBuffer, "\033fg%06X%s: %s %s (%s <= x <= %s) -> FAIL\n", 0xDD0000, cNameOfStepExt,
				    cValue, cUnit, cLowerLimit, cUpperLimit);
				 if(pBench->SerialIsMandatory)
				 {
					sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cPrintBuffer);
				 } else {
					sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cPrintBuffer);
				 }
				 if (strlen(cReportText)) 
				 {
					 strcat(cPrintBuffer, "Report-Text: ");
					 strcat(cPrintBuffer, cReportText);
					 strcat(cPrintBuffer, "\n");
				 }
				 if (strlen(cPartID)) 
				 {
					 strcat(cPrintBuffer, "Part-ID: ");
					 strcat(cPrintBuffer, cPartID);
					 strcat(cPrintBuffer, "\n");
				 }
				}
				
				InsertListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, -1, cPrintBuffer, 1);
				giActualItemIndex++;
				
				if (giErrorLogWindowEnabled && !iComparisionResult)
				{
					
					InsertListItem (giActualTabHandle, TABPANEL_TESTERRORLOG, -1, cErrorLogTraceBuffer, 1);
					giActualErrorLogItemIndex++;
				}
				
				giAllTestResults[giActualTestResult]->pFirst = pAppendTestStep (giAllTestResults[giActualTestResult]->pFirst,
						0, cNameOfStepExt, cCommentOfStep, NULL, dValue, dLowerLimit, dUpperLimit, cUnit, "GELE", cFormat, iComparisionResult, NULL, NULL);
			
			}

			if (SETINDEX) SetCtrlIndex(giActualTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
			if (SETINDEX) SetCtrlIndex(giActualTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);;
			
		}
	} 
	

ErrorTag:

	if (lTrace)
	{
		//Skipped, function ICT_Load_Detailed_Report reset the pointer to  pErrorCode
		/*if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage); 
			RESMGR_Trace(cTraceBuffer);
		}  */
		RESMGR_Trace("<<tspan_DisplayInCircuitTest end");
	}

}

/* FUNCTION *****************************************************************/
/**
tspan_DisplayEazyScanTest
*
@brief Display the results of last executed E-Scan-Test of R&S CompactTSVP in  displayed GUI
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cNameOfStep: Name of test step (Input)
@param cCommentOfStep: Comment of test step (Input)
@param eazyscan_ID: eazyscan resource ID that you obtained vom eazyscan.dll
@param esnProgram_ID: ESN program ID that you obtained vom eazyscan.dll
@param cFormat: format string of all double values (Input)
@param iOnlyFailedTests: 1-> only failed test are reported, 0-> all test are reported (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_DisplayEazyScanTest(CAObjHandle sequenceContext, long pResourceId, char cNameOfStep[], 
	char cCommentOfStep[], int eazyscan_ID, int esnProgram_ID, char cFormat[], int iOnlyFailedTests, short* pErrorOccurred, 
	long* pErrorCode, char errorMessage[])

{

	int iTabHandle;
	char cTraceBuffer[1024];
	char cStepNameWithOutTabs[1024];
	
	long lTrace;
	long lCompType;
	long lItemsFormatted;

	double dMean;
	double dDev;
	double dCP, dCPKUL, dCPKLL;
	long lCountOfLines;
	
	long NumberOfEntries;
	int iLoopIndex;
	
	char cNameOfStepExt[1024];
	char cTestName[1024];
	long lTestStatus = 0;
	int iTestID;
	char cPartID[1024];
	char cTimeStamp[1024];
	double dValue;
	double dLowerLimit;
	double dUpperLimit;
	char cUnit[1024];
	char cReportText[1024];
	double dDuration;
	
	char cPrintBuffer[1024];
    char cErrorLogTraceBuffer[1024];
    char cLowerLimit[1024];
    char cUpperLimit[1024];
    char cValue[1024];
   
	char cCompleteReportText[300000];
	
	int iComparisionResult;

	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>tspan_DisplayEazyScanTest begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}
	
	//Change all tabs in step name to whitespaces
	if (! *pErrorOccurred)
	{
		iKillTabInStepName (cNameOfStep, cStepNameWithOutTabs, strlen(cNameOfStep));
	}

	//Solve Comp-Type
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			RESMGR_Trace(">>Parameters begin");
			sprintf(cTraceBuffer, "cNameOfStep: %s", cNameOfStep);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cCommentOfStep: %s", cCommentOfStep);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "easyscan_ID: %d", eazyscan_ID);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "esnProgram_ID: %d", esnProgram_ID);
			RESMGR_Trace(cTraceBuffer);
			sprintf(cTraceBuffer, "cFormat: %s", cFormat);
			RESMGR_Trace(cTraceBuffer);
			RESMGR_Trace("<<Parameters end");
		}

	}


	if (!*pErrorOccurred)
	{
		if (pBench->iMode == DEBUG_MODE)
		{
			//do nothing ... ict cannot be executed in debug mode...
		}
		else if (pBench->iMode == OPERATOR_MODE)
		{
			if (lTrace) RESMGR_Trace("Compare data -> operator mode");
			
			if (iOnlyFailedTests)
			{	
				escan_Load_Detailed_Report (sequenceContext, eazyscan_ID, esnProgram_ID, 1, &NumberOfEntries, &pErrorOccurred, &pErrorCode, errorMessage);
			} else {
				escan_Load_Detailed_Report (sequenceContext, eazyscan_ID, esnProgram_ID, 0, &NumberOfEntries, &pErrorOccurred, &pErrorCode, errorMessage);
			}
			if (NumberOfEntries <= 0) goto ErrorTag;
			
			cCompleteReportText[0] = '\0';
			
			for (iLoopIndex = 0; iLoopIndex < NumberOfEntries; iLoopIndex++)
			{
			
				escan_Get_Detailed_Report_Entry (sequenceContext, eazyscan_ID, esnProgram_ID, iLoopIndex, cTestName, &lTestStatus, cTimeStamp, &dValue, &dLowerLimit, &dUpperLimit, cUnit, &dDuration, &pErrorOccurred, &pErrorCode, errorMessage);
				
				if (strlen(cFormat))
				{
				 FormatValues (cValue, cFormat, dValue);
				 FormatValues (cLowerLimit, cFormat, dLowerLimit);
				 FormatValues (cUpperLimit, cFormat, dUpperLimit);
				}
				else
				{
				 sprintf(cValue, "%.9f", dValue);
				 sprintf(cLowerLimit, "%.9f", dLowerLimit);
				 sprintf(cUpperLimit, "%.9f", dUpperLimit);
				}
				
				iComparisionResult = 0;
				sprintf (cNameOfStepExt, "%s - %s", cStepNameWithOutTabs, cTestName);
				if ((dValue >= dLowerLimit) && (dValue <= dUpperLimit))
				{
				 sprintf(cPrintBuffer, "\033fg%06X%s: %s %s (%s <= x <= %s) -> PASS\n", VAL_BLACK, cNameOfStepExt,
				    cValue, cUnit, cLowerLimit, cUpperLimit);
				 iComparisionResult++;
				}
				else
				{
				 sprintf(cPrintBuffer, "\033fg%06X%s: %s %s (%s <= x <= %s) -> FAIL\n", 0xDD0000, cNameOfStepExt,
				    cValue, cUnit, cLowerLimit, cUpperLimit);
				 if(pBench->SerialIsMandatory)
				 {
					sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cPrintBuffer);
				 } else {
					sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cPrintBuffer);
				 }
				}
				
				InsertListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, -1, cPrintBuffer, 1);
				giActualItemIndex++;
				
				if (giErrorLogWindowEnabled && !iComparisionResult)
				{
					
					InsertListItem (giActualTabHandle, TABPANEL_TESTERRORLOG, -1, cErrorLogTraceBuffer, 1);
					giActualErrorLogItemIndex++;
				}
				
				giAllTestResults[giActualTestResult]->pFirst = pAppendTestStep (giAllTestResults[giActualTestResult]->pFirst,
						0, cNameOfStepExt, cCommentOfStep, NULL, dValue, dLowerLimit, dUpperLimit, cUnit, "GELE", cFormat, iComparisionResult, NULL, NULL);
			
			}

			if (SETINDEX) SetCtrlIndex(giActualTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
			if (SETINDEX) SetCtrlIndex(giActualTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);;
			
		}
	} 
	

ErrorTag:

	if (lTrace)
	{
		//Skipped, function ICT_Load_Detailed_Report reset the pointer to  pErrorCode
		/*if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage); 
			RESMGR_Trace(cTraceBuffer);
		}  */
		RESMGR_Trace("<<tspan_DisplayEazyScanTest end");
	}

}


/* FUNCTION *****************************************************************/
/**
tspan_SingleStepNumericValue
*
@brief Display the results of a numreic limit test in a message box for single step mode in Teststand
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cNameOfStep: Name of test step (Input)
@param cCommentOfStep: Comment of test step (Input)
@param cUnit: Unit of test step (Input)
@param cCompType: Comaprision type of test step, eg "GELE" (Input)
@param cFormat: Format string of given double values (Input)
@param dValue: Measurement value (Input)
@param dLowerLimit: Lower limit (Input)
@param dUpperLimit: Upper limit (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_SingleStepNumericValue (CAObjHandle sequenceContext,
		long pResourceId, char cNameOfStep[],
		char cCommentOfStep[], char cUnit[],
		char cCompType[], char cFormat[],
		double dValue, double dLowerLimit,
		double dUpperLimit,
		short *pErrorOccurred,
		long *pErrorCode, char errorMessage[])
{

	char cBuffer[4096];
	char cStepNameWithOutTabs[4096];
	char cValue[20];
	int Panel;
	int iPanel;
	int iCtrl;
	int iResult;

	ERRORINFO pTSErrorInfo;

	char cTraceBuffer[1024];

	long lTrace;

	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_SingleStepNumericValue begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}
	
	//Change all tabs in step name to whitespaces
	if (! *pErrorOccurred)
	{
		iKillTabInStepName (cNameOfStep, cStepNameWithOutTabs, strlen(cNameOfStep));
	}

	if (! *pErrorOccurred)
	{
		if (lTrace) RESMGR_Trace("Load panel");
		Panel = LoadPanelEx (0, "testdebugpanel.uir", SINGLESTEP, __CVIUserHInst);
		iResult = FALSE;
		if (lTrace) RESMGR_Trace("Copy data");
		sprintf(cBuffer, "Test case: %s\n", cStepNameWithOutTabs);
		strcat(cBuffer, "Comment: ");
		strcat(cBuffer, cCommentOfStep);
		strcat(cBuffer, "\n\n");
		strcat(cBuffer, "Measurement: ");
		FormatValues (cValue, cFormat, dValue);
		strcat(cBuffer, cValue);
		strcat(cBuffer, " ");
		strcat(cBuffer, cUnit);
		strcat(cBuffer, "\n");
		if (strcmp(cCompType, "NOCOMP"))
		{
			if (((!strcmp(cCompType, "GTLT")) || (!strcmp(cCompType, "GELE")) ||
					(!strcmp(cCompType, "GELT")) || (!strcmp(cCompType, "GTLE")) ||
					(!strcmp(cCompType, "LTGT")) || (!strcmp(cCompType, "LEGE")) ||
					(!strcmp(cCompType, "LEGT")) || (!strcmp(cCompType, "LTGE"))))
			{
				strcat(cBuffer, "Lower Limit: ");
				FormatValues(cValue, cFormat, dLowerLimit);
				strcat(cBuffer, cValue);
				strcat(cBuffer, " ");
				strcat(cBuffer, cUnit);
				strcat(cBuffer, "\n");
			}
			else
			{
				strcat(cBuffer, "Limit: ");
				FormatValues(cValue, cFormat, dLowerLimit);
				strcat(cBuffer, cValue);
				strcat(cBuffer, " ");
				strcat(cBuffer, cUnit);
				strcat(cBuffer, "\n");
			}
		}
		if ((!strcmp(cCompType, "GTLT")) || (!strcmp(cCompType, "GELE")) ||
				(!strcmp(cCompType, "GELT")) || (!strcmp(cCompType, "GTLE")) ||
				(!strcmp(cCompType, "LTGT")) || (!strcmp(cCompType, "LEGE")) ||
				(!strcmp(cCompType, "LEGT")) || (!strcmp(cCompType, "LTGE")))
		{
			strcat(cBuffer, "Upper Limit: ");
			FormatValues(cValue, cFormat, dUpperLimit);
			strcat(cBuffer, cValue);
			strcat(cBuffer, " ");
			strcat(cBuffer, cUnit);
			strcat(cBuffer, "\n");
		}
		strcat(cBuffer, "\n");
		strcat(cBuffer, "Comparision: ");
		strcat(cBuffer, cCompType);
		strcat(cBuffer, "\n\n");
		strcat(cBuffer, "Test Result: ");

		if (!strcmp(cCompType, "EQ"))
		{
			if (dValue == dLowerLimit)
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "NEQ"))
		{
			if (dValue != dLowerLimit)
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "GT"))
		{
			if (dValue > dLowerLimit)
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "LT"))
		{
			if (dValue < dLowerLimit)
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "GE"))
		{
			if (dValue >= dLowerLimit)
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "LE"))
		{
			if (dValue <= dLowerLimit)
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "GTLT"))
		{
			if ((dValue > dLowerLimit) && (dValue < dUpperLimit))
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "GELE"))
		{
			if ((dValue >= dLowerLimit) && (dValue <= dUpperLimit))
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "GELT"))
		{
			if ((dValue > dLowerLimit) && (dValue <= dUpperLimit))
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "GTLE"))
		{
			if ((dValue >= dLowerLimit) && (dValue < dUpperLimit))
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "LTGT"))
		{
			if ((dValue < dLowerLimit) || (dValue > dUpperLimit))
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "LEGE"))
		{
			if ((dValue <= dLowerLimit) || (dValue >= dUpperLimit))
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "LEGT"))
		{
			if ((dValue <= dLowerLimit) || (dValue > dUpperLimit))
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "LTGE"))
		{
			if ((dValue < dLowerLimit) || (dValue >= dUpperLimit))
			{
				strcat(cBuffer, "Passed");
				iResult = TRUE;
			}
			else
			{
				strcat(cBuffer, "Failed");
			}
		}
		else if (!strcmp(cCompType, "NOCOMP"))
		{
			strcat(cBuffer, "No Comparison");
		}


		SetCtrlVal(Panel, SINGLESTEP_TEXTMSG, cBuffer);
		if(!iResult) SetPanelAttribute (Panel, ATTR_BACKCOLOR, VAL_RED);
		if(iResult) SetPanelAttribute (Panel, ATTR_BACKCOLOR, VAL_GREEN);
		DefaultCtrl (Panel, SINGLESTEP_CONTINUE);
		SetActiveCtrl (Panel, SINGLESTEP_CONTINUE);
		InstallPopup(Panel);
		if (lTrace) RESMGR_Trace("Install popup and wait for user event");
		GetUserEvent (1, &iPanel, &iCtrl);

		if (iCtrl == SINGLESTEP_REPEAT)
		{
			TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo,
									 "Parameters.Repeat", 0, TRUE);
			if (lTrace) RESMGR_Trace("User input repeat");

		}
		if (iCtrl == SINGLESTEP_TERMINATE)
		{
			TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo,
									 "Locals.Terminate", TS_PropOption_InsertIfMissing, TRUE);
			if (lTrace) RESMGR_Trace("User input terminate");
		}

		RemovePopup(Panel);

		DiscardPanel(Panel);
	}
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_SingleStepNumericValue end");
	}
}

/* FUNCTION *****************************************************************/
/**
tspan_SingleStepPassFail
*
@brief Display the results of a PASS/FAIL test in a message box for single step mode in Teststand
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cNameOfStep: Name of test step (Input)
@param cCommentOfStep: Comment of test step (Input)
@param lResult: Test result of the test step 0 -> FAIL, 1 -> PASS (Input) 
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_SingleStepPassFail (CAObjHandle sequenceContext,
		long pResourceId, char cNameOfStep[],
		char cCommentOfStep[], int lResult,
		short *pErrorOccurred, long *pErrorCode,
		char errorMessage[])
{
	char cBuffer[4096];
	char cStepNameWithOutTabs[4096];
	int Panel;
	int iPanel;
	int iCtrl;
	ERRORINFO pTSErrorInfo;

	char cTraceBuffer[1024];

	long lTrace;

	BENCH_STRUCT *pBench = NULL;



	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_SingleStepPassFail begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}
	
	//Change all tabs in step name to whitespaces
	if (! *pErrorOccurred)
	{
		iKillTabInStepName (cNameOfStep, cStepNameWithOutTabs, strlen(cNameOfStep));
	}

	if (! *pErrorOccurred)
	{
		if (lTrace) RESMGR_Trace("Load panel");
		Panel = LoadPanelEx (0, "testdebugpanel.uir", SINGLESTEP, __CVIUserHInst);
		if (lTrace) RESMGR_Trace("Copy data");
		sprintf(cBuffer, "Test case: %s\n", cStepNameWithOutTabs);
		strcat(cBuffer, "Comment: ");
		strcat(cBuffer, cCommentOfStep);
		strcat(cBuffer, "\n\n");
		strcat(cBuffer, "Test Result: ");
		if (lResult) strcat(cBuffer, "Passed\n\n");
		if (!lResult) strcat(cBuffer, "Failed\n\n");
		strcat(cBuffer, "Comparision: Pass/Fail");

		SetCtrlVal(Panel, SINGLESTEP_TEXTMSG, cBuffer);
		if(!lResult) SetPanelAttribute (Panel, ATTR_BACKCOLOR, VAL_RED);
		if(lResult) SetPanelAttribute (Panel, ATTR_BACKCOLOR, VAL_GREEN);
		DefaultCtrl (Panel, SINGLESTEP_CONTINUE);
		SetActiveCtrl (Panel, SINGLESTEP_CONTINUE);
		InstallPopup(Panel);
		if (lTrace) RESMGR_Trace("Install popup and wait for user event");
		GetUserEvent (1, &iPanel, &iCtrl);

		if (iCtrl == SINGLESTEP_REPEAT)
		{
			TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo,
									 "Parameters.Repeat", 0, TRUE);
			if (lTrace) RESMGR_Trace("User input repeat");
		}
		if (iCtrl == SINGLESTEP_TERMINATE)
		{
			TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo,
									 "Locals.Terminate", TS_PropOption_InsertIfMissing, TRUE);
			if (lTrace) RESMGR_Trace("User input terminate");
		}

		RemovePopup(Panel);

		DiscardPanel(Panel);
	}
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_SingleStepPassFail end");
	}
}

/* FUNCTION *****************************************************************/
/**
tspan_SingleStepString
*
@brief Display the results of a string value test in a message box for single step mode in Teststand
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cNameOfStep: Name of test step (Input)
@param cCommentOfStep: Comment of test step (Input)
@param cCompType: NI Teststand comparision type, eg "CaseSensitive" (Input)
@param cStringToCompare: String value to compare (Input)
@param cExpectedString: Expected string value (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_SingleStepString (CAObjHandle sequenceContext,
									   long pResourceId, char cNameOfStep[],
									   char cCommentOfStep[], char cCompType[],
									   char cStringToCompare[],
									   char cExpectedString[],
									   short *pErrorOccurred, long *pErrorCode,
									   char errorMessage[])
{
	char cBuffer[4096];
	char cStepNameWithOutTabs[4096];
	int Panel;
	int iResult;
	int iPanel;
	int iCtrl;
	ERRORINFO pTSErrorInfo;

	char cTraceBuffer[1024];

	long lTrace;

	BENCH_STRUCT *pBench = NULL;



	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_SingleStepString begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}
	
	//Change all tabs in step name to whitespaces
	if (! *pErrorOccurred)
	{
		iKillTabInStepName (cNameOfStep, cStepNameWithOutTabs, strlen(cNameOfStep));
	}

	if (!*pErrorOccurred)
	{
		if (lTrace) RESMGR_Trace("Load panel");
		Panel = LoadPanelEx (0, "testdebugpanel.uir", SINGLESTEP, __CVIUserHInst);
		if (lTrace) RESMGR_Trace("Copy data");
		sprintf(cBuffer, "Test case: %s\n", cStepNameWithOutTabs);
		strcat(cBuffer, "Comment: ");
		strcat(cBuffer, cCommentOfStep);
		strcat(cBuffer, "\n\n");
		strcat(cBuffer, "String: ");
		strcat(cBuffer, cStringToCompare);
		strcat(cBuffer, "\n");
		strcat(cBuffer, "Limit: ");
		strcat(cBuffer, cExpectedString);
		strcat(cBuffer, "\n\n");
		strcat(cBuffer, "Comparision: ");
		strcat(cBuffer, cCompType);
		strcat(cBuffer, "\n\n");
		strcat(cBuffer, "Test Result: ");
		if (!strcmp("CaseSensitive", cCompType))
		{
			if (strcmp(cStringToCompare, cExpectedString))
			{
				strcat(cBuffer, "Failed\n");
				iResult = FALSE;
			}
			else
			{
				strcat(cBuffer, "Passed\n");
				iResult = TRUE;
			}
		}
		else
		{
			if (stricmp(cStringToCompare, cExpectedString))
			{
				strcat(cBuffer, "Failed\n");
				iResult = FALSE;
			}
			else
			{
				strcat(cBuffer, "Passed\n");
				iResult = TRUE;
			}
		}
		SetCtrlVal(Panel, SINGLESTEP_TEXTMSG, cBuffer);
		if(!iResult) SetPanelAttribute (Panel, ATTR_BACKCOLOR, VAL_RED);
		if(iResult) SetPanelAttribute (Panel, ATTR_BACKCOLOR, VAL_GREEN);
		DefaultCtrl (Panel, SINGLESTEP_CONTINUE);
		SetActiveCtrl (Panel, SINGLESTEP_CONTINUE);
		InstallPopup(Panel);
		if (lTrace) RESMGR_Trace("Install popup and wait for user event");

		GetUserEvent (1, &iPanel, &iCtrl);

		if (iCtrl == SINGLESTEP_REPEAT)
		{
			TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo,
									 "Parameters.Repeat", 0, TRUE);
			if (lTrace) RESMGR_Trace("User input repeat");
		}
		if (iCtrl == SINGLESTEP_TERMINATE)
		{
			TS_PropertySetValBoolean(sequenceContext, &pTSErrorInfo,
									 "Locals.Terminate", TS_PropOption_InsertIfMissing, TRUE);
			if (lTrace) RESMGR_Trace("User input terminate");
		}

		RemovePopup(Panel);

		DiscardPanel(Panel);
	}
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_SingleStepString end");
	}
}


/* FUNCTION *****************************************************************/
/**
tspan_WriteReport
*
@brief Write a report file onto disc
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cPathNameOrg: path to report file (Input)
@param cFileName: file name of report file (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_WriteReport (CAObjHandle sequenceContext,  long pResourceId,
								  char cPathNameOrg[], char cFileName[], short *pErrorOccurred, long *pErrorCode,
								  char errorMessage[])
{
	char cTraceBuffer[1024];

	long lTrace;

	BENCH_STRUCT *pBench = NULL;
	int idx, idy;
	int iConfirm;
	int iStatus;
	int iSize;
	int iCount;
	int iConverted;
	int iCharsWritten;
	unsigned int iLength;
	char cFileAndPathName[1024];
	char cCountFileName[1024];
	char TextMessage[1024];
	char cPathName[1024];
	char *cSerialNumber = NULL;
	char *cReportText = NULL;
	char *cConvertedReportText = NULL;
	char *cBenchName = NULL;
	char *cTestStation_ID = NULL;
	char *cDate = NULL;
	char *cTime = NULL;

	VBOOL bAppend;
	ERRORINFO errorInfo;
	HRESULT hResult;

	FILE *pFile = NULL;


	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_WriteReport begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);

	if (*pErrorOccurred) goto ERRORTAG;
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
			goto ERRORTAG;
		}
	}

	//Get mode
	hResult = TS_PropertyGetValBoolean (sequenceContext, &errorInfo, "StationGlobals.TSVP.AppendReport", TS_PropOption_InsertIfMissing, &bAppend);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		goto ERRORTAG;
	}
	//Get serial
	hResult = TS_PropertyGetValString (sequenceContext, &errorInfo, "StationGlobals.TSVP.SerialNumber", 0, &cSerialNumber);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		goto ERRORTAG;
	}
	//Get report text
	hResult = TS_PropertyGetValString (sequenceContext, &errorInfo, "StationGlobals.TSVP.ReportText", 0, &cReportText);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		goto ERRORTAG;
	}
	//Get bench name
	hResult = TS_PropertyGetValString (sequenceContext, &errorInfo, "StationGlobals.TSVP.BenchName1", 0, &cBenchName);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		goto ERRORTAG;
	}
	//Get teststation id
	hResult = TS_PropertyGetValString (sequenceContext, &errorInfo, "StationGlobals.Station_ID", 0, &cTestStation_ID);
	if (hResult < 0)
	{
		*pErrorOccurred = TRUE;
		*pErrorCode = hResult;
		CA_GetAutomationErrorString(hResult, errorMessage, 1024);
		goto ERRORTAG;
	}

	//Check report text for special characters
	iLength = strlen (cReportText);
	iCount = 0;
	for (idx = 0; idx < iLength; idx++)
	{
		if (cReportText[idx] == '%') iCount++;
		//if (cReportText[idx] == '\\') iCount++;
	}

	iConverted = 0;
	if (iCount > 0)
	{
		iConverted = 1;
		cConvertedReportText = NULL;
		cConvertedReportText = malloc (sizeof(char) * (iLength + iCount + 1));
		if (!cConvertedReportText)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = -100001;
			sprintf (errorMessage, "Out of memory!");
			goto ERRORTAG;
		}
		idy = 0;
		for (idx = 0; idx < iLength; idx++)
		{
			if (cReportText[idx] == '%')
			{
				cConvertedReportText[idy] = cReportText[idx];
				idy++;
				cConvertedReportText[idy] = '%';
				idy++;
			} /*else if (cReportText[idx] == '\\')
		   {
				cConvertedReportText[idy] = cReportText[idx];
				idy++;
				cConvertedReportText[idy] = '\\';
				idy++;
		   }*/ else
			{
				cConvertedReportText[idy] = cReportText[idx];
				idy++;

			}
		}

		//End tag for string
		cConvertedReportText[idy] = '\0';
	}


	//if path name with last back slash -> delete this fucking back slash

	sprintf(cPathName, "%s", cPathNameOrg);
	iLength = strlen(cPathName);
	if (cPathName[iLength-1] == '\\') cPathName[iLength-1] = '\0';

	//Check the path name
	iStatus = SetDir (cPathName);
	if (iStatus)
	{
		iConfirm = ConfirmPopup ("CP-ITS ControlPanel","The specified path does not exist. Click \"Yes\" to create the path."
								 " Click \"No\" to return with an error.");
		if (iConfirm)
		{
			iStatus = MakeDir (cPathName);
			if (iStatus)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = iStatus;
				sprintf(errorMessage, "Cannot create path for report file!");
				goto ERRORTAG;
			}
		}
		else
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = iStatus;
			sprintf(errorMessage, "Report path does not exits!");
			goto ERRORTAG;
		}
	}
	if (strlen(cFileName))
	{
		sprintf(cFileAndPathName, "%s\\%s", cPathName, cFileName);
		if (!bAppend)
		{
			iCount = 0;
			iStatus = FileExists (cFileAndPathName, &iSize);
			while (iStatus == 1)
			{
				iCount++;
				sprintf (cCountFileName, "%s\\%s_%d.txt", cPathName, cFileName, iCount);
				iStatus = FileExists (cCountFileName, &iSize);
			}
			if (!iCount)
			{
				pFile = fopen (cFileAndPathName, "w");
			}
			else
			{
				pFile = fopen (cCountFileName, "w");
			}
			if (!pFile)
			{
				sprintf(TextMessage, "Cannot create report file! (%s)", cFileAndPathName);
				MessagePopup("CP-ITS ControlPanel - Error (1)", TextMessage);
				goto ERRORTAG;
			}

		}
		else
		{
			pFile = fopen (cFileAndPathName, "a");
			if (!pFile)
			{
				sprintf(TextMessage, "Cannot create report file! (%s)", cFileAndPathName);
				MessagePopup("CP-ITS ControlPanel - Error (2)", TextMessage);
				goto ERRORTAG;
			}
		}
	}
	else
	{
		if (strlen(cSerialNumber))
		{
			if (!bAppend)
			{
				cDate = cOwnDateStr();
				cDate[2] = '_';
				cDate[5] = '_';
				cTime = TimeStr();
				cTime[2] = '_';
				cTime[5] = '_';
				sprintf(cFileAndPathName, "%s\\%s_%s_%s_%s.txt", cPathName, cBenchName, cSerialNumber, cDate, cTime);
				pFile = fopen (cFileAndPathName, "w");
				if (!pFile)
				{
					sprintf(TextMessage, "Cannot create report file! (%s)", cFileAndPathName);
					MessagePopup("CP-ITS ControlPanel - Error (3)", TextMessage);
					goto ERRORTAG;
				}
			}
			else
			{
				sprintf(cFileAndPathName, "%s\\%s_%s.txt", cPathName, cBenchName, cSerialNumber);
				pFile = fopen (cFileAndPathName, "a");
				if (!pFile)
				{
					sprintf(TextMessage, "Cannot create report file! (%s)", cFileAndPathName);
					MessagePopup("CP-ITS ControlPanel - Error (4)", TextMessage);
					goto ERRORTAG;
				}
			}
		}
		else
		{
			if (!bAppend)
			{
				cDate = cOwnDateStr();
				cDate[2] = '_';
				cDate[5] = '_';
				cTime = TimeStr();
				cTime[2] = '_';
				cTime[5] = '_';
				sprintf(cFileAndPathName, "%s\\%s_%s_%s_%s.txt", cPathName, cBenchName, cDate, cTime, cTestStation_ID);
				pFile = fopen (cFileAndPathName, "w");
				if (!pFile)
				{
					sprintf(TextMessage, "Cannot create report file! (%s)", cFileAndPathName);
					MessagePopup("CP-ITS ControlPanel - Error (5)", TextMessage);
					goto ERRORTAG;
				}

			}
			else
			{
				sprintf(cFileAndPathName, "%s\\%s.txt", cPathName, cBenchName);
				pFile = fopen (cFileAndPathName, "a");
				if (!pFile)
				{
					sprintf(TextMessage, "Cannot create report file! (%s)", cFileAndPathName);
					MessagePopup("CP-ITS ControlPanel - Error (6)", TextMessage);
					goto ERRORTAG;
				}
			}
		}
	}

	if (iConverted) iCharsWritten = fprintf(pFile, cConvertedReportText);
	if (!iConverted) iCharsWritten = fprintf(pFile, cReportText);

	if (!iCharsWritten)
	{
		sprintf(TextMessage, "Cannot create report file! (%s)", cFileAndPathName);
		MessagePopup("CP-ITS ControlPanel - Error (7)", TextMessage);
		goto ERRORTAG;
	}


ERRORTAG:
	if (pFile) fclose(pFile);
	if (cConvertedReportText) free (cConvertedReportText);

	CA_FreeMemory (cSerialNumber);
	CA_FreeMemory (cReportText);
	CA_FreeMemory (cBenchName);
	CA_FreeMemory (cTestStation_ID);
	//CA_FreeMemory (cDate);
	//CA_FreeMemory (cTime);

	/*---------------------------------------------------------------------/
	/   Cleanup and error handling
	/---------------------------------------------------------------------*/
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_WriteReport end");
	}

}

/* FUNCTION *****************************************************************/
/**
tspan_RunTest
*
@brief Helper function to control teststand without run button of control panel
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param iSerialIsCorrect: 0 -> serial number in gui is not correct, otherwise -> 1  (Output)
@param iVariantIsCorrect: 0 -> selected variant is not correct, otherwise -> 1 (Output)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_RunTest (CAObjHandle sequenceContext, long pResourceId, long *iSerialIsCorrect,  long *iVariantIsCorrect,
							  short *pErrorOccurred, long *pErrorCode, char errorMessage[])
{
	char cTraceBuffer[1024];
	char cTextBuffer[1024];
	char cSerial[1024];
	long lTrace;

	int menubarhandle;
	int iNumberOfMenus;
	int idx;
	int iVisibleTextBoxLines;
	int iMenuID;
	int iStrokeActualCounts;
	int iYellowLimit;
	int iRedLimit;

	int iColorValue;
	int iEnableColor;

	double dDateTime;

	BENCH_STRUCT *pBench = NULL;

	ERRORINFO errorInfo;
	HRESULT hResult;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_RunTest begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}


	if (! *pErrorOccurred)
	{
		sprintf(cSerial, "");
		*iSerialIsCorrect = 1;
		*iVariantIsCorrect = 1;
		if ((pBench->iVariantsEnabled > 0) && (!strlen(gcSelectedVariant)))
		{
			*iVariantIsCorrect = 0;
			goto ERRORTAG;
		}
		if (giSerialIsMandatory)
		{
			GetCtrlVal(giActualPanelHandle, OP_SINGLE_SERIAL, cSerial);
			if (!strlen(cSerial) && (giSerialIsMandatory == 1))
			{
				*iSerialIsCorrect = 0;
				goto ERRORTAG;
			}
			if ((strlen(cSerial) != giSerialLength) && (giSerialIsMandatory > 1))
			{
				*iSerialIsCorrect = 0;
				goto ERRORTAG;
			}
			hResult = TS_PropertySetValString(sequenceContext, &errorInfo, "StationGlobals.TSVP.SerialNumber",
											  TS_PropOption_InsertIfMissing, cSerial);
			if (hResult < 0)
			{
				*pErrorOccurred = TRUE;
				*pErrorCode = hResult;
				CA_GetAutomationErrorString(hResult, errorMessage, 1024);
				goto ERRORTAG;
			}
			sprintf(gcSerialNumber, cSerial);
			*iSerialIsCorrect = 1;  
		}
		SetActiveTabPage (giActualPanelHandle, OP_SINGLE_TAB, 0);
		SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_DIMMED, FALSE);
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_LABEL_TEXT, "TERMITATE TEST [CTRL+T]");
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_RUN_TEST, ATTR_SHORTCUT_KEY, VAL_MENUKEY_MODIFIER|'T');

		SetCtrlAttribute(giActualPanelHandle, OP_SINGLE_SERIAL, ATTR_DIMMED, TRUE);
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_VARIANT_RING, ATTR_DIMMED, TRUE);
		menubarhandle = GetPanelMenuBar(giActualPanelHandle);
		iMenuID = -1;
		while (iMenuID != 0)
		{
			if(iMenuID == -1)
			{
				GetMenuBarAttribute (menubarhandle, 0, ATTR_FIRST_MENU_ID, &iMenuID);
			}
			else
			{
				GetMenuBarAttribute (menubarhandle, iMenuID, ATTR_NEXT_MENU_ID, &iMenuID);
			}
			if(iMenuID) SetMenuBarAttribute(menubarhandle, iMenuID, ATTR_DIMMED, TRUE);
		}
		
		GetPanelHandleFromTabPage (giActualPanelHandle, OP_SINGLE_TAB, 0, &giActualTabHandle);
		GetCtrlAttribute (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_VISIBLE_LINES, &iVisibleTextBoxLines);
		if (iVisibleTextBoxLines) DeleteListItem (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, 0, -1);
		giActualItemIndex = 0;
		
		//Set BG Color to specified value
		GetCtrlVal (giActualPanelHandle, OP_SINGLE_COL_RUNNING, &iColorValue);
		GetCtrlVal (giActualPanelHandle, OP_SINGLE_USECOLOREDBG, &iEnableColor);
		if (iEnableColor)
		{
			SetCtrlAttribute (giActualTabHandle, TABPANEL_TESTCASEPATHWAY, ATTR_TEXT_BGCOLOR, iColorValue);
		}
		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TESTSTATUS_MSG, ATTR_TEXT_BGCOLOR, iColorValue);
		SetCtrlVal(giActualPanelHandle, OP_SINGLE_TESTSTATUS_MSG, "TEST RUNNING");
		
		

		SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_ACTUALTESTSTEP, ATTR_LABEL_TEXT, "Current Test Step");
		TS_PropertySetValBoolean(sequenceContext, &errorInfo, "StationGlobals.TSVP.RunTest", TS_PropOption_InsertIfMissing, VTRUE);
		gdStartTimeStamp = HRESTIM_getTimeStamp();
		GetCtrlVal (giActualPanelHandle, OP_SINGLE_STROKE_ACT, &iStrokeActualCounts);
		GetCtrlVal (giActualPanelHandle, OP_SINGLE_STROKE_YELLOW, &iYellowLimit);
		GetCtrlVal (giActualPanelHandle, OP_SINGLE_STROKE_RED, &iRedLimit);
		if ((pBench->StrokeCounterEnabled == TRUE) && (iStrokeActualCounts >= 0))
		{
			GetCtrlVal (giActualPanelHandle, OP_SINGLE_STROKE_ACT, &iStrokeActualCounts);
			iStrokeActualCounts++;
			SetCtrlVal (giActualPanelHandle, OP_SINGLE_STROKE_ACT, iStrokeActualCounts);
			sprintf (cTextBuffer, "Stroke Indicator (%i)", iStrokeActualCounts);
			SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, ATTR_LABEL_TEXT, cTextBuffer);
			if (iStrokeActualCounts >= iYellowLimit)
				SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, ATTR_ON_COLOR, VAL_YELLOW);
			if (iStrokeActualCounts >= iRedLimit)
			{
				SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_TIMERSTROKEINDICATOR, ATTR_ENABLED, TRUE);
				SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_STROKEINDICATOR, ATTR_ON_COLOR, VAL_RED);
			}

		}


		//List of Measuremts
		giActualTestResult++;
		if (giActualTestResult > 49) giActualTestResult = 0;
		giTestIsRunning = TRUE;

		if (giMSAType1IsEnabled) SetCtrlAttribute (giActualPanelHandle, OP_SINGLE_MSA_INDICATOR, ATTR_ON_COLOR, VAL_YELLOW);

		GetCurrentDateTime (&dDateTime);
		FormatDateTimeString (dDateTime, "%d.%m.%Y - %H:%M:%S", cTraceBuffer, 1024);
		sprintf(giAllTestResults[giActualTestResult]->cStartDate, cTraceBuffer);
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

		if (giAllTestResults[giActualTestResult]->pFirst) vFreeAllTestResults (giAllTestResults[giActualTestResult]->pFirst);
		giAllTestResults[giActualTestResult]->pFirst = NULL;

	}

ERRORTAG:

	/*---------------------------------------------------------------------/
	/   Cleanup and error handling
	/---------------------------------------------------------------------*/
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_RunTest end");
	}

}

/* FUNCTION *****************************************************************/
/**
tspan_SetSerialNumber
*
@brief Helper function to set the serial number of control panel
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cSerialNumber: Given string is set in serial number resource in gui  (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param cErrorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_SetSerialNumber (CAObjHandle sequenceContext,
									  long pResourceId, char cSerialNumber[],
									  short *pErrorOccurred, long *pErrorCode, char cErrorMessage[])
{
	BENCH_STRUCT *pBench = NULL;
	char cTraceBuffer[1024];
	long lTrace;



	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>tspan_SetSerialNumber begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, cErrorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(cErrorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}


	if (! *pErrorOccurred)
	{
		SetCtrlVal(giActualPanelHandle, OP_SINGLE_SERIAL, cSerialNumber);
		sprintf(gcSerialNumber, cSerialNumber); 
	}


ERRORTAG:

	/*---------------------------------------------------------------------/
	/   Cleanup and error handling
	/---------------------------------------------------------------------*/
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, cErrorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<tspan_SetSerialNumber end");
	}




}

/* FUNCTION *****************************************************************/
/**
tspan_GetSerialNumber
*
@brief Helper function to get the serial number of control panel
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cSerialNumber: After execution this var contain the content of serial number resource in gui (Output)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param cErrorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_GetSerialNumber (CAObjHandle sequenceContext,
                                     long pResourceId, char cSerialNumber[],
                                     short *pErrorOccurred, long *pErrorCode,
                                     char cErrorMessage[])
{
	BENCH_STRUCT *pBench = NULL;
	char cTraceBuffer[1024];
	long lTrace;



	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>tspan_GetSerialNumber begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, cErrorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(cErrorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}


	if (! *pErrorOccurred)
	{
		GetCtrlVal(giActualPanelHandle, OP_SINGLE_SERIAL, cSerialNumber);
		sprintf(gcSerialNumber, cSerialNumber);
	}


ERRORTAG:

	/*---------------------------------------------------------------------/
	/   Cleanup and error handling
	/---------------------------------------------------------------------*/
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, cErrorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<tspan_GetSerialNumber end");
	}




}

/* FUNCTION *****************************************************************/
/**
tspan_GetAddionalSerialNumber
*
@brief Helper function to retrieve a addtional serial number from user
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param cMessageText: Message for user (Input)
@param cSerialNumber: After execution this var contain the content of serial number resource in gui (Output)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param cErrorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_GetAddionalSerialNumber (CAObjHandle sequenceContext,
                                     long pResourceId, char cMessageText[], char cSerialNumber[], 
                                     short *pErrorOccurred, long *pErrorCode,
                                     char cErrorMessage[])
{
	BENCH_STRUCT *pBench = NULL;
	char cTraceBuffer[1024];
	char cResponseBuffer[1024]; 
	long lTrace;



	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>tspan_GetAddionalSerialNumber begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, cErrorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(cErrorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}


	if (! *pErrorOccurred)
	{
		PromptPopup ("CP-ITS ControlPanel", cMessageText, cResponseBuffer, 100);
		sprintf(cSerialNumber, cResponseBuffer);
	}


ERRORTAG:

	/*---------------------------------------------------------------------/
	/   Cleanup and error handling
	/---------------------------------------------------------------------*/
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, cErrorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<tspan_GetAddionalSerialNumber end");
	}




    
}

/* FUNCTION *****************************************************************/
/**
tspan_Cleanup
*
@brief Free all resources in control panel
*
@param sequenceContext: Teststand sequence context (Input)
@param pResourceId: resource ID that be obtained from the setup function (Input)
@param pErrorOccurred: Teststand error flag (Output) 0 -> no error, 1 -> error occurred
@param pErrorCode: Teststand error code (Output)
@param errorMessage: Teststand error message (Output)
*
@return none
*****************************************************************************/

void __stdcall tspan_Cleanup(CAObjHandle sequenceContext, long pResourceId,
							 short *pErrorOccurred, long *pErrorCode, char errorMessage[])
{
	char cTraceBuffer[1024];
	char cTempBuffer[1024];
	int iError;
	long lTrace;

	BENCH_STRUCT *pBench = NULL;

	lTrace = RESMGR_Get_Trace_Flag(pResourceId);

	if (lTrace)
	{
		RESMGR_Trace(">>TSPAN_Cleanup begin");
	}
	/*---------------------------------------------------------------------/
	/   Retrieve the memory pointer:
	/     Get a pointer to the memory block to check the configuration
	/---------------------------------------------------------------------*/
	RESMGR_Get_Mem_Ptr(sequenceContext, pResourceId, (void **)(&pBench),
					   pErrorOccurred, pErrorCode, errorMessage);
	/*---------------------------------------------------------------------/
	/   Check for memory block owner:
	/     To be sure that the given resource ID belongs to the ENOCEAN
	/     library, we check the "owner" field of the memory block if it
	/     contains the "magic number" we have stored there in the
	/     ENOCEAN_Setup function
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		if (pBench->iOwner != TSPAN_ERR_BASE)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = GTSL_ERR_WRONG_RESOURCE_ID;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
		}
	}


	if (pBench->StrokeCounterEnabled)
	{
		iError = iWriteFixtureStrokeFile (pBench->StrokeCounterFile, pBench->FixtureCode, pBench->StrokeActualCounts);
		if (iError)
		{
			*pErrorOccurred = TRUE;
			*pErrorCode = TSPAN_ERR_WRITEFIXTURECODECFG;
			formatError(errorMessage,  *pErrorCode, pResourceId, NULL);
			sprintf (cTempBuffer, "\nCurrent fixture strokes: %i", pBench->StrokeActualCounts);
			strcat (errorMessage, cTempBuffer);
		}
	}

	/*---------------------------------------------------------------------/
	/   Dispose memory:
	/     Free the memory block associated with the resource ID.
	/     Note that pBench is no longer valid now because it points
	/     to dynamic memory that has been released!
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		RESMGR_Free_Memory(sequenceContext, pResourceId, pErrorOccurred,
						   pErrorCode, errorMessage);
	}
	pBench = NULL;

	/*---------------------------------------------------------------------/
	/   Free resource:
	/     The resource ID (our "ticket") is given back to the resource
	/     manager and may be reused in a subsequent RESMGR_Alloc_Resource
	/     call.
	/---------------------------------------------------------------------*/
	if (! *pErrorOccurred)
	{
		RESMGR_Free_Resource(sequenceContext, pResourceId, pErrorOccurred,
							 pErrorCode, errorMessage);
	}
	if (! *pErrorOccurred)
	{
		if (lTrace)
		{
			sprintf(cTraceBuffer, "Free Resource ID %ld", pResourceId);
			RESMGR_Trace(cTraceBuffer);
		}
	}

	/*---------------------------------------------------------------------/
	/   Cleanup and error handling
	/---------------------------------------------------------------------*/
	if (lTrace)
	{
		if (*pErrorOccurred)
		{
			sprintf(cTraceBuffer, "Error %ld : %s",  *pErrorCode, errorMessage);
			RESMGR_Trace(cTraceBuffer);
		}
		RESMGR_Trace("<<TSPAN_Cleanup end");
	}

}


/* END OF FILE *****************************************************************/
