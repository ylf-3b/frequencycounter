#!/bin/bash
# Frequency logging with arduino


stty -F /dev/ttyACM0 ispeed 9600 ospeed 9600 -ignpar cs8 -cstopb -echo -hupcl

echo "start logging"

while read line 
	do
		
	case "$line" in
		
		Log:*) timestamp=$(date)
		values=${line#"Log:"}
 		echo "$timestamp,$values" >> frequencylog.csv
		echo "$timestamp,$values"
		;;

		Debug:*) echo -e "\033[34m$line\033[0m"
		;;
		
		*) echo -e "\033[31mreceived and ignored unknown command: \033[1m»$line«\033[0m"
		;;
	esac
done < /dev/ttyACM0

# ... til the end
echo "end"


