#!/bin/bash

echo $# > /proc/sys/fs/mqueue/msg_max

program='./Application/applicationProcess.out '$*

eval $program

