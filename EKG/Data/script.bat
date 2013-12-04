REM written by mickl, AGH UST 
CLS
@ECHO OFF
ECHO Hello,
ECHO PhysioNet available indexes: 100-124, 200-234
SET /p index=Please insert record index 
IF index == "" GOTO err
ECHO Downloading samples, record #%index%
rdsamp -r mitdb/%index% > %index%a.txt
ECHO Succesfully saved %index%a.txt
ECHO Downloading physical samples, record #%index%
rdsamp -r mitdb/%index% -p > %index%b.txt
ECHO Succesfully saved %index%b.txt
ECHO Downloading annotations, record #%index%
rdann -r mitdb/%index% -a atr > %index%c.txt
ECHO Succesfully saved %index%c.txt
ECHO Downloading notes, record #%index%
wfdbdesc mitdb/%index% >%index%d.txt
ECHO Succesfully saved %index%d.txt