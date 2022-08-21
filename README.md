# Simple producer consumer program with two threads

## Introduction

This program can monitor changes in a JSON file (events.json) and updates the state inside the program. When there is a change in "event" object, the program internally updates its state and prints the value from "value" object in the JSON file.

The program accepts the JSON file path as the input argument and starts monitoring the file if it exists.

The program reads the JSON file at regular interval of 5000 microseconds, but if the JSON file is expected to change quicker, change READ_TIME_IN_MICROSEC MACRO in the src/global.h file

## Building on Linux 

Prerequisites:

 - `gcc`

 - make

 - support for POSIX library to compile pthread calls

```sh
$ git clone git@gitlab.com:indigo-interview/Goutham_Durairaj_Interview/Interview_Test.git -b indigo_19Jul
$ cd Interview_Test/
$ make
```

## Usage

After compiling, the executable (run.exe) can be started along with the input JSON file as shown below.

```sh
$ ./run.exe /Users/goutham/Desktop/LONELYMAN/c/bt/test/event.json
```

As the value of "event" object in the JSON file passed as input changes, we get output to something similar to,

```sh
State: Waiting Value:READY_VALUE
State: In Progress Value:STARTING VALUE
State: Done Value:COMPLETE_VALUE
```

## Source Code Info
1. main.c        - main thread, that handles the inputs from monitor thread   
2. monitor.c     - helper thread, that reads the JSON file and updates data structure
3. event_data.c  - common data structures, lock
4. utils.c       - utility functions

## Limitation
The program cannot handle steping down of states for eg: if current state is "Starting" it cannot go to "Ready To Wait", in this scenario the program stays in the highest state: "Starting" state.

The JSON file is read at an interval to save memory utilisation.
