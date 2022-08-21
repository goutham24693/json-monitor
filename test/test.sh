#!/bin/bash
#set -x


json_file="test.json"
json_file_tmp="test.json_tmp"
program_path="./src/"
exe="run.exe"
exepath=$program_path$exe
logfile="indigo.log"

create_def_json_file() 
{
	# remove any existing file 
	rm -f $json_file
	echo "{" >> $json_file
	echo "    \"event\": \"Ready To Wait\"," >> $json_file
	echo "    \"value\": \"READY_VALUE\"" >> $json_file
	echo "}" >> $json_file
}


start_program()
{
	killall $exe 2>/dev/null
	$exepath $json_file	> $logfile &
}

send_ready_event()
{
	cp $json_file $json_file_tmp
	sed -i 's/.*event.*/    "event": "Ready To Wait",/' $json_file_tmp
	sed -i 's/.*value.*/    "value": "READY_VALUE"/' $json_file_tmp 
	mv $json_file_tmp $json_file
	#cat $json_file
}

send_starting_event()
{
	cp $json_file $json_file_tmp
	sed -i 's/.*event.*/    "event": "Starting",/' $json_file_tmp
	sed -i 's/.*value.*/    "value": "STARTING_VALUE"/' $json_file_tmp 
	mv $json_file_tmp $json_file
	#cat $json_file
}

send_complete_event()
{
	cp $json_file $json_file_tmp
	sed -i 's/.*event.*/    "event": "Complete",/' $json_file_tmp
	sed -i 's/.*value.*/    "value": "COMPLETE_VALUE"/' $json_file_tmp 
	mv $json_file_tmp $json_file
	#cat $json_file
}

send_invalid_event()
{
	cp $json_file $json_file_tmp
	sed -i 's/.*event.*/    "event": "Readyyyy To Wait",/' $json_file_tmp
	sed -i 's/.*value.*/    "value": "COMPLETE_VALUE"/' $json_file_tmp 
	mv $json_file_tmp $json_file
	#cat $json_file
}

#testcase 1
# move to different states to test expected behavior
# order of events - "ready to wait" -> "starting" -> "complete"
test_case1()
{
	send_ready_event
	send_starting_event
	send_complete_event

	sleep 1  # adding sleep to let the process exit and log file contents getting flushed

	#cat $logfile
	cat $logfile | grep "STARTING_VALUE" > /dev/null
	if [ $? != 0 ]; then
		echo "state not ok"
		rm $logfile
		touch $logfile
		return 1
	fi
	#cat $logfile
	cat $logfile | grep "COMPLETE_VALUE" > /dev/null
	if [ $? != 0 ]; then
		echo "state not ok"
		rm $logfile
		touch $logfile
		return 1
	fi

	rm $logfile
	touch $logfile
	return 0
}

#testcase 2
# give invalid state as initial state and check if program accepts it
test_case2()
{
	send_invalid_event
	send_starting_event
	send_complete_event

	sleep 1  # adding sleep to let the process exit and log file contents getting flushed

	#cat $logfile
	cat $logfile | grep "STARTING_VALUE" > /dev/null
	if [ $? != 0 ]; then
		echo "state not ok"
		rm $logfile
		touch $logfile
		return 1
	fi
	#cat $logfile
	cat $logfile | grep "COMPLETE_VALUE" > /dev/null
	if [ $? != 0 ]; then
		echo "state not ok"
		rm $logfile
		touch $logfile
		return 1
	fi

	rm $logfile
	touch $logfile
	return 0
}

# main
create_def_json_file

start_program

test_case1
if [ $? == 0 ]; then
echo "testcase 1 OK"
else
echo "testcase 1 NOK"
fi

test_case2
if [ $? != 0 ]; then
echo "testcase 3 OK"
else
echo "testcase 3 NOK"
fi
