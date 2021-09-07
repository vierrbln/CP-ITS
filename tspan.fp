s��         ~�     y'  �   �   ����                               tspan       CP-ITS ControlPanel                           �  � ��CAObjHandle  �    The component CP-ITS control panel provides functions to display measurement values for test engineers or operators. The tspan.dll controls the settings of CP-ITS ControlPanel and changes variables in National Instruments Teststand to control the sequence in NI Teststand. 

Refer the manual of SW-ITS to get some informations about the tags in the application.ini that controls CP-ITS and the variables that controls CP-ITS ControlPanel in NI Teststand. 

Normally you don't need this help. All needed functions are used in the sequence templates for NI Teststand. Only if you want program your own sequences, you should read in this help file.     �    The function tspan_Setup loads all information from the application.ini into the memory and returns a handle to control the component CP-ITS ControlPanel.     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     p    Pass the pointer to a string containing the logical name or a bench name from the application layer .ini file.     '    Returns the Resource ID of the bench.    � 3 �  �  �    Sequence Context                  I � Q    `    ErrorOccurred                     � � �    `    ErrorCode                         � �_    `    ErrorMessage                      � � �    `    Bench Name                        4 �    `    ResourceID                         0    	            	            	                	            n    This function displays the small debug panel. This panel is used for debug some sequences with NI Teststand.     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.    P 7 �  �  �    Sequence Context                  �	 P    `    ErrorOccurred                     	 	 �    `    ErrorCode                         	�	^    `    ErrorMessage                      
B � �    `    ResourceID                         0    	            	            	                �    This function displays the operator panel. This panel is used to show all results and measurements and gives the possibility to changes some settings of report and ticket printer and so on.     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.    z 7 �  �  �    Sequence Context                  �	 P    `    ErrorOccurred                     J	 �    `    ErrorCode                         �	^    `    ErrorMessage                      l � �    `    ResourceID                         0    	            	            	                �    This function sets the test status from yellow status to green or red status (pass or fail) and enables all resources of the gui. Its normally used the sequence call PostUUT in NI Teststand.     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     J    Pass the result of your test procedure.

0 - Test failed
1 - Test passed    � 6 W  �  �    Sequence Context                  $ � _    `    ErrorOccurred                     u � �    `    ErrorCode                         � �m    `    ErrorMessage                      � 7O    `    ResourceID                        � � �     �    Result                             0    	            	            	                0    �    This functions sets the resource "Current step name" in the operator interface of CP-ITS ControlPanel to the given value (text).     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     [    The given value would be appear in the resource "Actual Step Name" of the operator panel.     - �  �  �    Sequence Context                  � � 7    `    ErrorOccurred                     � � �    `    ErrorCode                         P �E    `    ErrorMessage                       � i    `    ResourceID                        K �    �    ActualStepName                     0    	            	            	                    f    This function puts a new line in the resource "Test Report" and display the given text in this line.     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     _    Pass here the text that should be appear in the resource "Test Result" of the operator panel.    � - �  �  �    Sequence Context                  
 � 7    `    ErrorOccurred                     [ � �    `    ErrorCode                         � �E    `    ErrorMessage                      } � i    `    ResourceID                        � �    �    Text                               0    	            	            	                    �    This function hides the actual displayed panel of CP-ITS ControlPanel. You can use this function to hide the operator panel and the debug panel.     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.    ( 7 �  �  �    Sequence Context                  �	 P    `    ErrorOccurred                     �	 �    `    ErrorCode                         Z	^    `    ErrorMessage                        �    `    ResourceID                         0    	            	            	                |    This function displays the result of a numeric limit test in the operator panel or the debug panel of CP-ITS ControlPanel.     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     ?    Pass here the name of the test step that should be displayed.     B    Pass here the comment of the test step that should be displayed.     C    Pass here your measured value that should compared and displayed.         Pass here the lower limit.         Pass here the upper limit.     K    Pass here the unit (free text) of the test step that should be displayed.    �    Pass here the comparision type of the test step that should be displayed.

Possible values are:

Only lower limit is used
"EQ" -> Equal: ==
"NE" -> Not equal: !=
"GT" -> Greater than: >
"LT" -> Lower than: <
"GE" -> Greater equal than: >=
"LE" -> Lower equal than: <=

Lower and upper limit is used
"GTLT" -> ><
"GELE" -> >= <=
"GELT" -> >= <
"GTLE" -> >  <=
"LTGT" -> <  >
"LEGE" -> <= >=
"LEGT" -> <= >
"LTGE" -> <  >=

Lower and upper limits would be ignored
"LOG" -> no comparision, only log     A    Pass here a valid formatting string in C style like "%.2f" e.g.    " 2 z  �  �    Sequence Context                  "� O 
   `    ErrorOccurred                     "� �    `    ErrorCode                         #Ab    `    ErrorMessage                      $ 2j    `    ResourceID                        $< i d    �    NameOfStep                        $� i1    �    CommentOfStep                     $� �     �    Value                             % � �    �    LowerLimit                        %< �� 	   �    UpperLimit                        %` �     �    Unit                              %� � �    �    CompType                          '� ��    �    Format                             0    	            	            	                                                w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     ?    Pass here the name of the test step that should be displayed.     B    Pass here the comment of the test step that should be displayed.     R    Pass here the result of your test that should be displayed.

0 -> Fail
1 -> Pass    *� ) d  �  �    Sequence Context                  +r O    `    ErrorOccurred                     +� �    `    ErrorCode                         ,%b    `    ErrorMessage                      ,� (1    `    ResourceID                        -  d _    �    NameOfStep                        -g d'    �    CommentOfStep                     -� � �     �    Result                             0    	            	            	                            w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     ?    Pass here the name of the test step that should be displayed.     B    Pass here the comment of the test step that should be displayed.     �    Pass the one the following strings:

"CaseSensitive" -> Case of the characters would not be ignored
"IgnoreCase" -> Case of the characters would be ignored     )    Pass here your "measured" string value.     .    Pass here the string that your are expected.    /� ) d  �  �    Sequence Context                  0k O    `    ErrorOccurred                     0� �    `    ErrorCode                         1b 	   `    ErrorMessage                      1� )8    `    ResourceID                        2 d _    �    NameOfStep                        2` d'    �    CommentOfStep                     2� �     �    CompType                          3O � �    �    StringToCompare                   3� �^    �    ExpectedString                     0    	            	            	                                    |    This function displays the result of a numeric limit test in the operator panel or the debug panel of CP-ITS ControlPanel.     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     ?    Pass here the name of the test step that should be displayed.     B    Pass here the comment of the test step that should be displayed.     A    Pass here a valid formatting string in C style like "%.2f" e.g.    6� 2 z  �  �    Sequence Context                  7 O    `    ErrorOccurred                     7] � 	   `    ErrorCode                         7�b 
   `    ErrorMessage                      8 2j    `    ResourceID                        8� i d    �    NameOfStep                        9 i1    �    CommentOfStep                   ���� � W     �    Ict_ID                          ���� �2     �    IctProgram_ID                     9K � W    �    Format                          ���� �     �    OnlyFailedTests                    0    	            	            	                        0    0        0    |    This function displays the result of a numeric limit test in the operator panel or the debug panel of CP-ITS ControlPanel.     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     ?    Pass here the name of the test step that should be displayed.     B    Pass here the comment of the test step that should be displayed.     A    Pass here a valid formatting string in C style like "%.2f" e.g.    <� 2 z  �  �    Sequence Context                  =& O    `    ErrorOccurred                     =w � 	   `    ErrorCode                         =�b 
   `    ErrorMessage                      >� 2j    `    ResourceID                        >� i d    �    NameOfStep                        ? i1    �    CommentOfStep                   ���� � W     �    eazyscan_ID                     ���� �.     �    ESNProgram_ID                     ?e � W    �    Format                          ���� �     �    OnlyFailedTests                    0    	            	            	                        0    0        0    w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     ?    Pass here the name of the test step that should be displayed.     B    Pass here the comment of the test step that should be displayed.     C    Pass here your measured value that should compared and displayed.         Pass here the lower limit.         Pass here the upper limit.     K    Pass here the unit (free text) of the test step that should be displayed.    �    Pass here the comparision type of the test step that should be displayed.

Possible values are:

Only lower limit is used
"EQ" -> Equal: ==
"NE" -> Not equal: !=
"GT" -> Greater than: >
"LT" -> Lower than: <
"GE" -> Greater equal than: >=
"LE" -> Lower equal than: <=

Lower and upper limit is used
"GTLT" -> ><
"GELE" -> >= <=
"GELT" -> >= <
"GTLE" -> >  <=
"LTGT" -> <  >
"LEGE" -> <= >=
"LEGT" -> <= >
"LTGE" -> <  >=

Lower and upper limits would be ignored
"LOG" -> no comparision, only log     A    Pass here a valid formatting string in C style like "%.2f" e.g.    B=  U  �  �    Sequence Context                  B� O 
   `    ErrorOccurred                     C �    `    ErrorCode                         Cob    `    ErrorMessage                      D/ E    `    ResourceID                        Dj i Q    �    NameOfStep                        D� i1    �    CommentOfStep                     D� �     �    Value                             EF � �    �    LowerLimit                        Ej �� 	   �    UpperLimit                        E� �     �    Unit                              E� � �    �    CompType                          G� ��    �    Format                             0    	            	            	                                                R    Pass here the result of your test that should be displayed.

0 -> Fail
1 -> Pass     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     ?    Pass here the name of the test step that should be displayed.     B    Pass here the comment of the test step that should be displayed.    K{  U  �  �    Sequence Context                  K� O    `    ErrorOccurred                     LK �    `    ErrorCode                         L�b    `    ErrorMessage                      Mm E    `    ResourceID                        M� i d    �    NameOfStep                        M� i1    �    CommentOfStep                   ���� � �     �    Result                             0    	            	            	                            w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     3    The Resource ID returned from the Setup function.     ?    Pass here the name of the test step that should be displayed.     B    Pass here the comment of the test step that should be displayed.     �    Pass the one the following strings:

"CaseSensitive" -> Case of the characters would not be ignored
"IgnoreCase" -> Case of the characters would be ignored     )    Pass here your "measured" string value.     .    Pass here the string that your are expected.    P  U  �  �    Sequence Context                  P� O    `    ErrorOccurred                     P� �    `    ErrorCode                         QLb 	   `    ErrorMessage                      R E    `    ResourceID                        RG i d    �    NameOfStep                        R� i1    �    CommentOfStep                     R� � -    �    CompType                          S} � �    �    StringToCompare                   S� �x    �    ExpectedString                     0    	            	            	                0    0               �    This functions create a report file and uses some NI Teststand variables as rules for the creation process.

You can control the report file by setting the following Teststand variables:

Used for creating the file name:
StationGlobals.TSVP.AppendReport
StationGlobals.TSVP.SerialNumber
StationGlobals.TSVP.BenchName1
StationGlobals.Station_ID

Content of the file:
StationGlobals.TSVP.ReportText

Refer to SW-ITS manual for the path and file build options...

     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     5    The Resource ID returned from the Setup function.

     F    Pass here a valid directory in which the report file will be stored.    /    Pass a file name without extenstion. You pass NULL for creating the file name by some rules and the content of the described variables of Teststand.

For using this function outside of NI Teststand you must pass a valid file name without extenstion.

The extension of the report files are always .txt!    X # =  �  �    Sequence Context                  X�	 K    `    ErrorOccurred                     X�	 �    `    ErrorCode                         YB	Y    `    ErrorMessage                      Z _ _    `    Resource ID                       Z? � Z    �    cPathName                         Z� �#    �    cFileName                          0    	            	            	                       �    CP-ITS ControlPanel controls some variables in NI Teststand to start the test process after hit the button Run Test [ENTER]. If you work without this button (e.g. you check a closed box sensor and start the test afterwards) you will need this function to set the appropriate variables in NI Teststand.

The function returns the flags that the typed serial number is ok and a variant is selected if specified.     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     4    The Resource ID returned from the Setup function.
     k    Returns the status of the serial number:

0 -> serial number is not correct
1 -> serial number is correct     �    Returns the selection status of the variants there are specified in the application.ini.

0 -> no variant is selected
1 -> a variant is selected by operator    _ %   �  �    Sequence Context                  _� 6    `    ErrorOccurred                     _� �    `    ErrorCode                         `@I    `    ErrorMessage                      a  p O    `    Resource ID                       a< u     �    SerialIsCorrect                   a� �     �    VariantIsCorrect                   0    	            	            	                	            	           �    This functions create a report file and uses some NI Teststand variables as rules for the creation process.

You can control the report file by setting the following Teststand variables:

Used for creating the file name:
StationGlobals.TSVP.AppendReport
StationGlobals.TSVP.SerialNumber
StationGlobals.TSVP.BenchName1
StationGlobals.Station_ID

Content of the file:
StationGlobals.TSVP.ReportText

Refer to SW-ITS manual for the path and file build options...

     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     5    The Resource ID returned from the Setup function.

     F    Pass here a valid directory in which the report file will be stored.    e� # =  �  �    Sequence Context                  fc	 K    `    ErrorOccurred                     f�	 �    `    ErrorCode                         g	Y    `    ErrorMessage                      g� � c    `    Resource ID                       h �    �    cSerialNumber                      0    	            	            	                   �    This functions create a report file and uses some NI Teststand variables as rules for the creation process.

You can control the report file by setting the following Teststand variables:

Used for creating the file name:
StationGlobals.TSVP.AppendReport
StationGlobals.TSVP.SerialNumber
StationGlobals.TSVP.BenchName1
StationGlobals.Station_ID

Content of the file:
StationGlobals.TSVP.ReportText

Refer to SW-ITS manual for the path and file build options...

     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     5    The Resource ID returned from the Setup function.

     F    Pass here a valid directory in which the report file will be stored.    k� # =  �  �    Sequence Context                  l&	 K    `    ErrorOccurred                     lw	 �    `    ErrorCode                         l�	Y    `    ErrorMessage                      m� � c    `    Resource ID                       m� �    �    cSerialNumber                      0    	            	            	                   �    This functions create a report file and uses some NI Teststand variables as rules for the creation process.

You can control the report file by setting the following Teststand variables:

Used for creating the file name:
StationGlobals.TSVP.AppendReport
StationGlobals.TSVP.SerialNumber
StationGlobals.TSVP.BenchName1
StationGlobals.Station_ID

Content of the file:
StationGlobals.TSVP.ReportText

Refer to SW-ITS manual for the path and file build options...

     w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     5    The Resource ID returned from the Setup function.

     G    After correct execution this var contains the content of reponse box.     +    Pass here a c string as message for user.    qj # =  �  �    Sequence Context                  q�	 K    `    ErrorOccurred                     r:	 �    `    ErrorCode                         r�	Y    `    ErrorMessage                      s\ � c    `    Resource ID                       s� � �    �    cSerialNumber                     s� �    �    cMessageText                       0    	            	            	                        w    For use with TestStand: The TestStand sequence context ("ThisContext")

For use in other environments: Must be zero.
     I    Error Flag.
          
Return :
   0 -> No Error
   1 -> Error Occured
     Z    Error code. Negative values indicate an error.

Return:

   0 -> No Error
 < 0 -> Error
     �    Error message buffer (1024 bytes)

In case of an error, the error message is copied into the error message buffer. The buffer remains unchanged if the function returns successfully.
     4    The Resource ID returned from the Setup function.
    u� 7 �  �  �    Sequence Context                  vB	 G    `    ErrorOccurred                     v�	 �    `    ErrorCode                         v�	U    `    ErrorMessage                      w� � �    `    Resource ID                        0    	            	            	             ����         &  c     K.    Setup                           ����         �  
}     K.    DisplayDebugPanel               ����         �  �     K.    DisplayOperatorPanel            ����         �  $     K.    DisplayResult                   ����         �  �     K.    SetActualStepName               ����                K.    InsertTextIntoTestReport        ����         �   U     K.    HideActualPanel                 ����         !�  '�     K.    DisplayNumericValue             ����       ����  .     K.    DisplayPassFail                 ����       ����  3� 
    K.    DisplayString                   ����         6	  9�     K.    DisplayInCircuitTest            ����         <#  ?�     K.    DisplayEazyScanTest             ����       ����  H#     K.    SingleStepNumericValue          ����         K!  N9     K.    SingleStepPassFail              ����       ����  S� 
    K.    SingleStepString                ����         V9  [�     K.    WriteReport                     ����         ]l  bU     K.    RunTest                         ����         d  ha     K.    SetSerialNumber                 ����         i�  n$     K.    GetSerialNumber                 ����         o�  t     K.    GetAddionalSerialNumber         ����       ����  w�     K.    Cleanup                                �                                     DSetup                             ����General Functions                    DDisplay Debug Panel                  DDisplay Operator Panel               DDisplay Result                       DSet Actual Step Name                 DInsert Text Into Test Report         DHide Actual Panel                 ����Tests                                DDisplay Numeric Limit Test           DDisplay Pass/Fail Test               DDisplay String Value Test            DDisplay In-Circuit-Test Test         DDisplay E-Scan-Test               ����SingleStep                           DSingle Step Numeric                  DSingle Step Pass/Fail                DSingle Step String Value          ����Utilitiy functions                   DWrite Report File                    DRun Test From Teststand              DSet serial number                    DGet serial number                    DGet additional serial number         DCleanup                         