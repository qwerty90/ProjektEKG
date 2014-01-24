echo Downloading samples, record $1
LD_PRELOAD=libcurl.so rdsamp -r mitdb/$1 > $1a.txt
echo Succesfully saved $1a.txt
echo Downloading physical samples, record $1
LD_PRELOAD=libcurl.so rdsamp -r mitdb/$1 -p > $1b.txt
echo Succesfully saved $1b.txt
echo Downloading annotations, record $1
LD_PRELOAD=libcurl.so rdann -r mitdb/$1 -a atr > $1c.txt
echo Succesfully saved $1c.txt
echo Downloading notes, record $1
LD_PRELOAD=libcurl.so wfdbdesc mitdb/$1 >$1d.txt
echo Succesfully saved $1d.txt
