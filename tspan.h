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

@file tspan.h
 *
@brief Declaration of all exported functions
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

#ifndef TSPAN_H
#define TSPAN_H

/* INCLUDE FILES ***************************************************************/

#include <cvidef.h>
#include <cvirte.h>
#include <tsapicvi.h>
#include "gtslerr.h"   /* GTSL error handling */



/* GLOBAL TYPE DECLARATIONS ***************************************************/

/* GLOBAL CONSTANT DECLARATIONS ***********************************************/

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/

/* GLOBAL MACRO DEFINITIONS ***************************************************/

/* EXPORTED FUNCTIONS *********************************************************/
void __stdcall tspan_Setup(CAObjHandle sequenceContext, char* benchName, long* resourceID, short* errorOccurred, 
	long* errorCode, char errorMessage[]);
void __stdcall tspan_DisplayDebugPanel(CAObjHandle sequenceContext, long resourceID, short* errorOccurred, 
	long* errorCode, char errorMessage[]);
void __stdcall tspan_DisplayOperatorPanel(CAObjHandle sequenceContext, long resourceID, short* errorOccurred, 
	long* errorCode, char errorMessage[]);
void __stdcall tspan_DisplayResult(CAObjHandle sequenceContext, long resourceID, int result, short* errorOccurred, 
	long* errorCode, char errorMessage[]);
void __stdcall tspan_SetActualStepName(CAObjHandle sequenceContext, long resourceID, char actualStepName[], 
	short* errorOccurred, long* errorCode, char errorMessage[]);
void __stdcall tspan_InsertTextIntoTestReport(CAObjHandle sequenceContext, long resourceID, char text[], 
	short* errorOccurred, long* errorCode, char errorMessage[]);
void __stdcall tspan_HideActualPanel(CAObjHandle sequenceContext, long resourceID, short* errorOccurred, 
	long* errorCode, char errorMessage[]);
void __stdcall tspan_DisplayNumericValue(CAObjHandle sequenceContext, long resourceID, char nameOfStep[], 
	char commentOfStep[], char unit[], char cCompType[], char format[], double value, double lowerLimit, 
	double upperLimit, short* errorOccurred, long* errorCode, char errorMessage[]);
void __stdcall tspan_DisplayPassFail(CAObjHandle sequenceContext, long resourceID, char nameOfStep[], 
	char commentOfStep[], int result, short* errorOccurred, long* errorCode, char errorMessage[]);
void __stdcall tspan_DisplayString(CAObjHandle sequenceContext, long resourceID, char nameOfStep[], 
	char commentOfStep[], char compType[], char stringToCompare[], char expectedString[], short* errorOccurred, 
	long* errorCode, char errorMessage[]);
void __stdcall tspan_DisplayInCircuitTest(CAObjHandle sequenceContext, long resourceID, char nameOfStep[], 
	char commentOfStep[], int ict_ID, int ictProgram_ID, char cFormat[], int iOnlyFailedTests, short* errorOccurred, 
	long* errorCode, char errorMessage[]);
void __stdcall tspan_DisplayEazyScanTest(CAObjHandle sequenceContext, long resourceID, char nameOfStep[], 
	char commentOfStep[], int eazyscan_ID, int ESNProgram_ID, char cFormat[], int iOnlyFailedTests, short* errorOccurred, 
	long* errorCode, char errorMessage[]);
void __stdcall tspan_SingleStepNumericValue(CAObjHandle sequenceContext, long resourceID, char nameOfStep[], 
	char commentOfStep[], char unit[], char compType[], char format[], double value, double lowerLimit, 
	double upperLimit, short* errorOccurred, long* errorCode, char errorMessage[]);
void __stdcall tspan_SingleStepPassFail(CAObjHandle sequenceContext, long resourceID, char nameOfStep[], 
	char commentOfStep[], int passFail, short* errorOccurred, long* errorCode, char errorMessage[]);
void __stdcall tspan_SingleStepString(CAObjHandle sequenceContext, long resourceID, char nameOfStep[], 
	char commentOfStep[], char compType[], char stringToCompare[], char expectedString[], short* errorOccurred, 
	long* errorCode, char errorMessage[]);
void __stdcall tspan_WriteReport(CAObjHandle sequenceContext, long resourceID, char cPathName[], char cFileName[], 
	short* errorOccurred, long* errorCode, char errorMessage[]);
void __stdcall tspan_RunTest(CAObjHandle sequenceContext, long resourceID, long* iSerialIsCorrect, 
	long* iVariantlIsCorrect, short* errorOccurred, long* errorCode, char errorMessage[]);
void __stdcall tspan_SetSerialNumber(CAObjHandle sequenceContext, long resourceID, char cSerialNumber[], 
	short* errorOccurred, long* errorCode, char errorMessage[]);
void __stdcall tspan_GetSerialNumber(CAObjHandle sequenceContext, long resourceID, char cSerialNumber[], 
	short* errorOccurred, long* errorCode, char errorMessage[]);
void __stdcall tspan_GetAddionalSerialNumber (CAObjHandle sequenceContext, long resourceID, char cMessageText[],
    char cSerialNumber[], short *errorOccurred, long *errorCode, char errorMessage[]);
void __stdcall tspan_Cleanup(CAObjHandle sequenceContext, long resourceID, short* errorOccurred, 
	long* errorCode, char errorMessage[]);

#endif /* do not add code after this line */
/******************************************************************************/
/* END OF FILE                                                                */
 /******************************************************************************/
