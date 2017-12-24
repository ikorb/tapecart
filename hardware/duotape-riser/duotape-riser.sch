EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:switches
LIBS:edgeconn_2x6
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L GND #PWR01
U 1 1 5A0B3E24
P 3050 4200
F 0 "#PWR01" H 3050 3950 50  0001 C CNN
F 1 "GND" H 3050 4050 50  0000 C CNN
F 2 "" H 3050 4200 50  0000 C CNN
F 3 "" H 3050 4200 50  0000 C CNN
	1    3050 4200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 5A0B3E4C
P 3750 4200
F 0 "#PWR02" H 3750 3950 50  0001 C CNN
F 1 "GND" H 3750 4050 50  0000 C CNN
F 2 "" H 3750 4200 50  0000 C CNN
F 3 "" H 3750 4200 50  0000 C CNN
	1    3750 4200
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR03
U 1 1 5A0B3E74
P 2950 3100
F 0 "#PWR03" H 2950 2950 50  0001 C CNN
F 1 "+5V" H 2950 3240 50  0000 C CNN
F 2 "" H 2950 3100 50  0000 C CNN
F 3 "" H 2950 3100 50  0000 C CNN
	1    2950 3100
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR04
U 1 1 5A0B3E9C
P 3850 3100
F 0 "#PWR04" H 3850 2950 50  0001 C CNN
F 1 "+5V" H 3850 3240 50  0000 C CNN
F 2 "" H 3850 3100 50  0000 C CNN
F 3 "" H 3850 3100 50  0000 C CNN
	1    3850 3100
	1    0    0    -1  
$EndComp
$Comp
L EDGECONN_6X2 CN1
U 1 1 5A0B4018
P 3400 3650
F 0 "CN1" H 3400 3300 60  0000 C CNN
F 1 "EDGECONN_6X2" H 3400 4000 60  0000 C CNN
F 2 "edgeconn_6x2:EDGECONN_6x2_IMP" H 6750 2200 60  0001 C CNN
F 3 "" H 6750 2200 60  0000 C CNN
	1    3400 3650
	1    0    0    -1  
$EndComp
Text Label 2950 3600 2    60   ~ 0
Motor
Text Label 2950 3700 2    60   ~ 0
Read
Text Label 2950 3800 2    60   ~ 0
Write
Text Label 2950 3900 2    60   ~ 0
Sense
Wire Wire Line
	2950 3600 3150 3600
Wire Wire Line
	3150 3700 2950 3700
Wire Wire Line
	3150 3800 2950 3800
Wire Wire Line
	2950 3900 3150 3900
Wire Wire Line
	3150 3400 3050 3400
Wire Wire Line
	3050 3400 3050 4200
Wire Wire Line
	3650 3400 4250 3400
Wire Wire Line
	3750 3400 3750 4200
Wire Wire Line
	3850 3100 3850 3500
Wire Wire Line
	3650 3500 4250 3500
Wire Wire Line
	3150 3500 2950 3500
Wire Wire Line
	2950 3500 2950 3100
Text Label 3850 3600 0    60   ~ 0
Motor
Text Label 3850 3700 0    60   ~ 0
Read
Text Label 3850 3800 0    60   ~ 0
Write
Text Label 3850 3900 0    60   ~ 0
Sense
$Comp
L EDGECONN_6X2 CN2
U 1 1 5A0B4998
P 4500 3650
F 0 "CN2" H 4500 3300 60  0000 C CNN
F 1 "EDGECONN_6X2" H 4500 4000 60  0000 C CNN
F 2 "edgeconn_6x2:EDGECONN_6x2_IMP" H 7850 2200 60  0001 C CNN
F 3 "" H 7850 2200 60  0000 C CNN
	1    4500 3650
	1    0    0    -1  
$EndComp
Text Label 4950 3600 0    60   ~ 0
Motor
Wire Wire Line
	4750 3600 4950 3600
Text Label 4950 3700 0    60   ~ 0
Read
Text Label 4950 3800 0    60   ~ 0
Write
Text Label 4950 3900 0    60   ~ 0
Sense
Wire Wire Line
	4750 3900 4950 3900
Wire Wire Line
	4750 3800 4950 3800
Wire Wire Line
	4750 3700 4950 3700
$Comp
L GND #PWR05
U 1 1 5A0B5750
P 4850 4100
F 0 "#PWR05" H 4850 3850 50  0001 C CNN
F 1 "GND" H 4850 3950 50  0000 C CNN
F 2 "" H 4850 4100 50  0000 C CNN
F 3 "" H 4850 4100 50  0000 C CNN
	1    4850 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 3400 4850 3400
Wire Wire Line
	4850 3400 4850 4100
$Comp
L +5V #PWR06
U 1 1 5A0B5829
P 4950 3300
F 0 "#PWR06" H 4950 3150 50  0001 C CNN
F 1 "+5V" H 4950 3440 50  0000 C CNN
F 2 "" H 4950 3300 50  0000 C CNN
F 3 "" H 4950 3300 50  0000 C CNN
	1    4950 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 3500 4950 3500
Wire Wire Line
	4950 3500 4950 3300
Wire Wire Line
	3650 3600 4250 3600
Connection ~ 3850 3500
Connection ~ 3750 3400
Wire Wire Line
	3650 3700 4250 3700
Wire Wire Line
	3650 3800 4250 3800
Wire Wire Line
	4250 3900 3650 3900
$Comp
L PWR_FLAG #FLG07
U 1 1 5A3FB463
P 2750 3150
F 0 "#FLG07" H 2750 3245 50  0001 C CNN
F 1 "PWR_FLAG" H 2750 3330 50  0000 C CNN
F 2 "" H 2750 3150 50  0000 C CNN
F 3 "" H 2750 3150 50  0000 C CNN
	1    2750 3150
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG08
U 1 1 5A3FB47B
P 2850 4150
F 0 "#FLG08" H 2850 4245 50  0001 C CNN
F 1 "PWR_FLAG" H 2850 4330 50  0000 C CNN
F 2 "" H 2850 4150 50  0000 C CNN
F 3 "" H 2850 4150 50  0000 C CNN
	1    2850 4150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2850 4150 3050 4150
Connection ~ 3050 4150
Wire Wire Line
	2950 3200 2750 3200
Wire Wire Line
	2750 3200 2750 3150
Connection ~ 2950 3200
$EndSCHEMATC
