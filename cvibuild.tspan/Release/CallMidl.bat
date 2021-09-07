set PATH=c:\program files (x86)\national instruments\cvi2017\bin\clang\3.3;%PATH%
@echo "C:\Program Files (x86)\National Instruments\Shared\MSDTRedistributables\BuildTools\8.1\midl.exe" %1 /win32 /mktyplib203 /cpp_cmd "clang.exe" /cpp_opt "-E -x odl -fms-compatibility" /tlb %2 /I"C:\Program Files (x86)\National Instruments\Shared\MSDTRedistributables\SDKHeaderFiles\8.1"
@"C:\Program Files (x86)\National Instruments\Shared\MSDTRedistributables\BuildTools\8.1\midl.exe" %1 /win32 /mktyplib203 /cpp_cmd "clang.exe" /cpp_opt "-E -x odl -fms-compatibility" /tlb %2 /I"C:\Program Files (x86)\National Instruments\Shared\MSDTRedistributables\SDKHeaderFiles\8.1" > midl.out 2>&1
@echo MIDL complete
@if errorlevel 1 goto err
@exit 0
:err
@echo MIDL failed
@exit 1
