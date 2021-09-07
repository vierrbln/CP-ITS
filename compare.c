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

@file compare.c
 *
@brief Numeric comparision functions of control panel
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
iCompareEQ
*
@brief Compares a numeric limit test in op and debug mode -> Type EQ
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareEQ(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if (dValue == dLowerLimit)
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
    }
    if (dValue == dLowerLimit)
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x == %s) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x == %s) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareNEQ
*
@brief Compares a numeric limit test in op and debug mode -> Type NEQ
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareNEQ(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if (dValue != dLowerLimit)
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
    }
    if (dValue != dLowerLimit)
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x != %s) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x != %s) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareGT
*
@brief Compares a numeric limit test in op and debug mode -> Type GT
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareGT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if (dValue > dLowerLimit)
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
    }
    if (dValue > dLowerLimit)
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x > %s) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x > %s) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareLT
*
@brief Compares a numeric limit test in op and debug mode -> Type LT
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareLT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if (dValue < dLowerLimit)
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
    }
    if (dValue < dLowerLimit)
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x < %s) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x < %s) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareGE
*
@brief Compares a numeric limit test in op and debug mode -> Type GE
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareGE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if (dValue >= dLowerLimit)
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
    }
    if (dValue >= dLowerLimit)
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x >= %s) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x >= %s) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareLE
*
@brief Compares a numeric limit test in op and debug mode -> Type LE
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareLE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if (dValue <= dLowerLimit)
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
    }
    if (dValue <= dLowerLimit)
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x <= %s) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x <= %s) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareGTLT
*
@brief Compares a numeric limit test in op and debug mode -> Type GTLT
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareGTLT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if ((dValue > dLowerLimit) && (dValue < dUpperLimit))
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
      FormatValues(cUpperLimit, cFormat, dUpperLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
      sprintf(cUpperLimit, "%.9f", dUpperLimit);
    }
    if ((dValue > dLowerLimit) && (dValue < dUpperLimit))
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (%s < x < %s) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (%s < x < %s) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareGELE
*
@brief Compares a numeric limit test in op and debug mode -> Type GELE
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareGELE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if ((dValue >= dLowerLimit) && (dValue <= dUpperLimit))
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
      FormatValues(cUpperLimit, cFormat, dUpperLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
      sprintf(cUpperLimit, "%.9f", dUpperLimit);
    }
    if ((dValue >= dLowerLimit) && (dValue <= dUpperLimit))
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (%s <= x <= %s) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (%s <= x <= %s) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareGELT
*
@brief Compares a numeric limit test in op and debug mode -> Type GELT
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareGELT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if ((dValue >= dLowerLimit) && (dValue < dUpperLimit))
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
      FormatValues(cUpperLimit, cFormat, dUpperLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
      sprintf(cUpperLimit, "%.9f", dUpperLimit);
    }
    if ((dValue > dLowerLimit) && (dValue <= dUpperLimit))
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (%s <= x < %s) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (%s <= x < %s) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareGTLE
*
@brief Compares a numeric limit test in op and debug mode -> Type GTLE
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareGTLE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if ((dValue > dLowerLimit) && (dValue <= dUpperLimit))
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
      FormatValues(cUpperLimit, cFormat, dUpperLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
      sprintf(cUpperLimit, "%.9f", dUpperLimit);
    }
    if ((dValue > dLowerLimit) && (dValue <= dUpperLimit))
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (%s < x <= %s) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (%s < x <= %s) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareLTGT
*
@brief Compares a numeric limit test in op and debug mode -> Type LTGT
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareLTGT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if ((dValue > dUpperLimit) && (dValue < dLowerLimit))
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
      FormatValues(cUpperLimit, cFormat, dUpperLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
      sprintf(cUpperLimit, "%.9f", dUpperLimit);
    }
    if ((dValue > dUpperLimit) || (dValue < dLowerLimit))
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x < %s OR %s < x) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x < %s OR %s < x) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareLEGE
*
@brief Compares a numeric limit test in op and debug mode -> Type LEGE
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareLEGE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if ((dValue >= dUpperLimit) && (dValue <= dLowerLimit))
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
      FormatValues(cUpperLimit, cFormat, dUpperLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
      sprintf(cUpperLimit, "%.9f", dUpperLimit);
    }
    if ((dValue >= dUpperLimit) || (dValue <= dLowerLimit))
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x <= %s OR %s <= x) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x <= %s OR %s <= x) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareLEGT
*
@brief Compares a numeric limit test in op and debug mode -> Type LEGT
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareLEGT(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if ((dValue > dUpperLimit) && (dValue <= dLowerLimit))
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
      FormatValues(cUpperLimit, cFormat, dUpperLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
      sprintf(cUpperLimit, "%.9f", dUpperLimit);
    }
    if ((dValue > dUpperLimit) || (dValue <= dLowerLimit))
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x <= %s OR %s < x) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x <= %s OR %s < x) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  } //
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareLTGE
*
@brief Compares a numeric limit test in op and debug mode -> Type LTGE
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, 0 -> FAIL. 1 -> PASS
*****************************************************************************/

int iCompareLTGE(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 0;
  if (iMode == DEBUG_MODE)
  {
    if ((dValue >= dUpperLimit) && (dValue < dLowerLimit))
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 0);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 1);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "PASS");
      iComparisionResult++;
    }
    else
    {
      SetCtrlVal(iTabHandle, ANALOG_RED, 1);
      SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
      SetCtrlVal(iTabHandle, ANALOG_RESULT, "FAIL");
    }
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
      FormatValues(cLowerLimit, cFormat, dLowerLimit);
      FormatValues(cUpperLimit, cFormat, dUpperLimit);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
      sprintf(cLowerLimit, "%.9f", dLowerLimit);
      sprintf(cUpperLimit, "%.9f", dUpperLimit);
    }
    if ((dValue >= dUpperLimit) || (dValue < dLowerLimit))
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x < %s OR %s <= x) -> PASS", VAL_BLACK, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
      iComparisionResult++;
    }
    else
    {
      sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (x < %s OR %s <= x) -> FAIL", 0xDD0000, cNameOfStep, cValue, cUnit, cLowerLimit, cUpperLimit);
    }
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}

/* FUNCTION *****************************************************************/
/**
iCompareNOCOMP
*
@brief Compares a numeric limit test in op and debug mode -> Type nom comparision
*
@param iTabHandle: Panel id of resource (Input)
@param iMode: Mode selector (Operator, Debug) (Input)
@param iSerialIsMandatory: Only operator mode, serial is printed in FAIL LOG (Input)
@param dValue: Measurement (Input)
@param dLowerLimit: Lower limit of comparision (Input)
@param dUpperLimit: Upper limit of comparision (Input)
@param cNameOfStep: Name of test step (Input)
@param cUnit: Unit of measurement (Input)
@param cFormat: Format string of measurement (Input)
*
@return Comparision result, allways PASS
*****************************************************************************/

int iCompareNOCOMP(int iTabHandle, int iMode, int iSerialIsMandatory, double dValue, double dLowerLimit, double dUpperLimit, char cNameOfStep[1024], char cUnit[1024], char cFormat[1024])
{
  char cTraceBuffer[1024];
  char cErrorLogTraceBuffer[1024];
  char cLowerLimit[1024];
  char cUpperLimit[1024];
  char cValue[1024];
  char cText[100000];
  int iComparisionResult = 1;
  if (iMode == DEBUG_MODE)
  {
    SetCtrlVal(iTabHandle, ANALOG_RED, 0);
    SetCtrlVal(iTabHandle, ANALOG_GREEN, 0);
    SetCtrlVal(iTabHandle, ANALOG_RESULT, "");
  }
  if (iMode == OPERATOR_MODE)
  {
    if (strlen(cFormat))
    {
      FormatValues(cValue, cFormat, dValue);
    }
    else
    {
      sprintf(cValue, "%.9f", dValue);
    }
    sprintf(cTraceBuffer, "\033fg%06X%s: %s %s (No comparison)", VAL_BLACK, cNameOfStep, cValue, cUnit);
    InsertListItem(iTabHandle, TABPANEL_TESTCASEPATHWAY,  - 1, cTraceBuffer, 1);
    giActualItemIndex++;
    if (SETINDEX)
    {
      SetCtrlIndex(iTabHandle, TABPANEL_TESTCASEPATHWAY, giActualItemIndex - 1);
    }
    if (giErrorLogWindowEnabled && !iComparisionResult)
    {
      if (iSerialIsMandatory)
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - SNR: %s - Step: %s", cOwnDateStr(), TimeStr(), gcSerialNumber, cTraceBuffer);
      }
      else
      {
        sprintf(cErrorLogTraceBuffer, "Timestamp: %s %s - Step: %s", cOwnDateStr(), TimeStr(), cTraceBuffer);
      }
      InsertListItem(iTabHandle, TABPANEL_TESTERRORLOG,  - 1, cErrorLogTraceBuffer, 1);
      giActualErrorLogItemIndex++;
      if (SETINDEX)
      {
        SetCtrlIndex(iTabHandle, TABPANEL_TESTERRORLOG, giActualErrorLogItemIndex - 1);
      }
    }
  }
  return iComparisionResult;
}


/* FUNCTION *****************************************************************/
/**
FormatValues
*
@brief Formats a value output
*
@param cValue: After correct execution the value contains the formated double value (Output)
@param cFormat: Format string of measurement (Input)
@param dValue: Value that be printed into cValue (Input)
*
@return none
*****************************************************************************/

void FormatValues(char cValue[1024], char* cFormat, double dValue)
{
  if (!strcmp(cFormat, "%i"))
  {
    //MessagePopup("bla", "Integer");
    sprintf(cValue, cFormat, (int)dValue);
    return ;
  }
  else if (!strcmp(cFormat, "%u"))
  {
    //MessagePopup("bla", "unsigend Integer");
    sprintf(cValue, cFormat, (unsigned int)dValue);
    return ;
  }
  else if (!strcmp(cFormat, "%#x"))
  {
    //MessagePopup("bla", "klein hex"); 
    sprintf(cValue, cFormat, (int)dValue);
    return ;
  }
  else if (!strcmp(cFormat, "%#X"))
  {
    //MessagePopup("bla", "groﬂ hex"); 
    sprintf(cValue, cFormat, (int)dValue);
    return ;
  }
  else if (!strcmp(cFormat, "%#o"))
  {
    //MessagePopup("bla", "octal"); 
    sprintf(cValue, cFormat, (int)dValue);
    return ;
  }
  else
  {
    sprintf(cValue, cFormat, dValue);
    return ;
  }
}
