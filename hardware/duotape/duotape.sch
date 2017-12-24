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
LIBS:duotape-cache
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
Text Notes 3300 3200 0    60   ~ 12
C64
Text Notes 7900 2250 0    60   ~ 12
Datasette A
$Comp
L 4053 U1
U 1 1 5A0B2088
P 5400 4100
F 0 "U1" H 5500 4100 50  0000 C CNN
F 1 "4053" H 5500 3900 50  0000 C CNN
F 2 "Housings_DIP:DIP-16_W7.62mm" H 5400 4100 60  0001 C CNN
F 3 "" H 5400 4100 60  0001 C CNN
	1    5400 4100
	-1   0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 5A0B20F7
P 3300 5300
F 0 "C1" H 3325 5400 50  0000 L CNN
F 1 "100n" H 3325 5200 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Rect_L7_W2_P5" H 3338 5150 50  0001 C CNN
F 3 "" H 3300 5300 50  0000 C CNN
	1    3300 5300
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 5A0B217A
P 7400 2550
F 0 "R2" V 7480 2550 50  0000 C CNN
F 1 "1k" V 7400 2550 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 7330 2550 50  0001 C CNN
F 3 "" H 7400 2550 50  0000 C CNN
	1    7400 2550
	0    1    -1   0   
$EndComp
$Comp
L R R3
U 1 1 5A0B2241
P 7400 4000
F 0 "R3" V 7480 4000 50  0000 C CNN
F 1 "1k" V 7400 4000 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 7330 4000 50  0001 C CNN
F 3 "" H 7400 4000 50  0000 C CNN
	1    7400 4000
	0    1    -1   0   
$EndComp
$Comp
L R R1
U 1 1 5A0B22DF
P 6450 5000
F 0 "R1" V 6530 5000 50  0000 C CNN
F 1 "1k" V 6450 5000 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 6380 5000 50  0001 C CNN
F 3 "" H 6450 5000 50  0000 C CNN
	1    6450 5000
	0    1    1    0   
$EndComp
$Comp
L GND #PWR01
U 1 1 5A0B2396
P 5400 5600
F 0 "#PWR01" H 5400 5350 50  0001 C CNN
F 1 "GND" H 5400 5450 50  0000 C CNN
F 2 "" H 5400 5600 50  0000 C CNN
F 3 "" H 5400 5600 50  0000 C CNN
	1    5400 5600
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR02
U 1 1 5A0B23BE
P 5400 3250
F 0 "#PWR02" H 5400 3100 50  0001 C CNN
F 1 "+5V" H 5400 3390 50  0000 C CNN
F 2 "" H 5400 3250 50  0000 C CNN
F 3 "" H 5400 3250 50  0000 C CNN
	1    5400 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 3250 5400 3450
$Comp
L +5V #PWR03
U 1 1 5A0B316E
P 6700 4900
F 0 "#PWR03" H 6700 4750 50  0001 C CNN
F 1 "+5V" H 6700 5040 50  0000 C CNN
F 2 "" H 6700 4900 50  0000 C CNN
F 3 "" H 6700 4900 50  0000 C CNN
	1    6700 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 4600 4600 5100
Wire Wire Line
	4600 4600 4700 4600
Wire Wire Line
	6100 4500 6200 4500
Wire Wire Line
	6200 4500 6200 5000
Wire Wire Line
	6200 4600 6100 4600
Wire Wire Line
	6200 4700 6100 4700
Connection ~ 6200 4600
Wire Wire Line
	5400 4750 5400 5600
Connection ~ 6200 4700
Wire Wire Line
	6700 4900 6700 5000
Wire Wire Line
	6700 5000 6600 5000
Wire Wire Line
	5400 4850 6350 4850
Wire Wire Line
	6350 4850 6350 4400
Wire Wire Line
	6350 4400 6100 4400
Connection ~ 5400 4850
$Comp
L GND #PWR04
U 1 1 5A0B3E24
P 3050 4200
F 0 "#PWR04" H 3050 3950 50  0001 C CNN
F 1 "GND" H 3050 4050 50  0000 C CNN
F 2 "" H 3050 4200 50  0000 C CNN
F 3 "" H 3050 4200 50  0000 C CNN
	1    3050 4200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 5A0B3E4C
P 3750 4200
F 0 "#PWR05" H 3750 3950 50  0001 C CNN
F 1 "GND" H 3750 4050 50  0000 C CNN
F 2 "" H 3750 4200 50  0000 C CNN
F 3 "" H 3750 4200 50  0000 C CNN
	1    3750 4200
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR06
U 1 1 5A0B3E74
P 2950 3100
F 0 "#PWR06" H 2950 2950 50  0001 C CNN
F 1 "+5V" H 2950 3240 50  0000 C CNN
F 2 "" H 2950 3100 50  0000 C CNN
F 3 "" H 2950 3100 50  0000 C CNN
	1    2950 3100
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR07
U 1 1 5A0B3E9C
P 3850 3100
F 0 "#PWR07" H 3850 2950 50  0001 C CNN
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
Motor_C64
Text Label 2950 3700 2    60   ~ 0
Read_C64
Text Label 2950 3800 2    60   ~ 0
Write_C64
Text Label 2950 3900 2    60   ~ 0
Sense_C64
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
	3650 3400 3750 3400
Wire Wire Line
	3750 3400 3750 4200
Wire Wire Line
	3850 3100 3850 3500
Wire Wire Line
	3850 3500 3650 3500
Wire Wire Line
	3150 3500 2950 3500
Wire Wire Line
	2950 3500 2950 3100
Text Label 3850 3600 0    60   ~ 0
Motor_C64
Text Label 3850 3700 0    60   ~ 0
Read_C64
Text Label 3850 3800 0    60   ~ 0
Write_C64
Text Label 3850 3900 0    60   ~ 0
Sense_C64
Wire Wire Line
	3650 3900 4600 3900
Wire Wire Line
	3650 3800 4500 3800
Wire Wire Line
	3650 3700 4400 3700
Wire Wire Line
	3650 3600 4400 3600
$Comp
L EDGECONN_6X2 CN2
U 1 1 5A0B4998
P 8150 2700
F 0 "CN2" H 8150 2350 60  0000 C CNN
F 1 "EDGECONN_6X2" H 8150 3050 60  0000 C CNN
F 2 "edgeconn_6x2:EDGECONN_6x2_IMP" H 11500 1250 60  0001 C CNN
F 3 "" H 11500 1250 60  0000 C CNN
	1    8150 2700
	1    0    0    -1  
$EndComp
Text Notes 7900 3650 0    60   ~ 12
Datasette B
Wire Wire Line
	4400 3600 4400 2650
Wire Wire Line
	4400 2650 7900 2650
Text Label 8600 2650 0    60   ~ 0
Motor_C64
Wire Wire Line
	8400 2650 8600 2650
Text Label 8600 2750 0    60   ~ 0
Read_A
Text Label 8600 2850 0    60   ~ 0
Write_A
Text Label 8600 2950 0    60   ~ 0
Sense_A
Wire Wire Line
	8400 2950 8600 2950
Wire Wire Line
	8400 2850 8600 2850
Wire Wire Line
	8400 2750 8600 2750
Text Label 7700 2750 2    60   ~ 0
Read_A
Text Label 7700 2850 2    60   ~ 0
Write_A
Text Label 7700 2950 2    60   ~ 0
Sense_A
$Comp
L GND #PWR08
U 1 1 5A0B5750
P 8500 3150
F 0 "#PWR08" H 8500 2900 50  0001 C CNN
F 1 "GND" H 8500 3000 50  0000 C CNN
F 2 "" H 8500 3150 50  0000 C CNN
F 3 "" H 8500 3150 50  0000 C CNN
	1    8500 3150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 5A0B5778
P 7800 3150
F 0 "#PWR09" H 7800 2900 50  0001 C CNN
F 1 "GND" H 7800 3000 50  0000 C CNN
F 2 "" H 7800 3150 50  0000 C CNN
F 3 "" H 7800 3150 50  0000 C CNN
	1    7800 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 3150 7800 2450
Wire Wire Line
	7800 2450 7900 2450
Wire Wire Line
	8400 2450 8500 2450
Wire Wire Line
	8500 2450 8500 3150
$Comp
L +5V #PWR010
U 1 1 5A0B5829
P 8600 2350
F 0 "#PWR010" H 8600 2200 50  0001 C CNN
F 1 "+5V" H 8600 2490 50  0000 C CNN
F 2 "" H 8600 2350 50  0000 C CNN
F 3 "" H 8600 2350 50  0000 C CNN
	1    8600 2350
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR011
U 1 1 5A0B5851
P 7700 2350
F 0 "#PWR011" H 7700 2200 50  0001 C CNN
F 1 "+5V" H 7700 2490 50  0000 C CNN
F 2 "" H 7700 2350 50  0000 C CNN
F 3 "" H 7700 2350 50  0000 C CNN
	1    7700 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 2350 7700 2550
Wire Wire Line
	7550 2550 7900 2550
Wire Wire Line
	8400 2550 8600 2550
Wire Wire Line
	8600 2550 8600 2350
Connection ~ 7700 2550
Wire Wire Line
	7250 2550 7150 2550
Wire Wire Line
	7150 2550 7150 2850
Connection ~ 7150 2850
Text Label 8750 4100 0    60   ~ 0
Motor_C64
Text Label 8750 4200 0    60   ~ 0
Read_B
Text Label 8750 4300 0    60   ~ 0
Write_B
Text Label 8750 4400 0    60   ~ 0
Sense_B
Text Label 7650 4200 2    60   ~ 0
Read_B
Text Label 7650 4300 2    60   ~ 0
Write_B
Text Label 7650 4400 2    60   ~ 0
Sense_B
Wire Wire Line
	6100 4200 7950 4200
Wire Wire Line
	6600 4300 7950 4300
Wire Wire Line
	6500 4400 7950 4400
Wire Wire Line
	8450 4100 8750 4100
Wire Wire Line
	8450 4200 8750 4200
Wire Wire Line
	8450 4300 8750 4300
Wire Wire Line
	8450 4400 8750 4400
$Comp
L GND #PWR012
U 1 1 5A0B61F5
P 7750 4600
F 0 "#PWR012" H 7750 4350 50  0001 C CNN
F 1 "GND" H 7750 4450 50  0000 C CNN
F 2 "" H 7750 4600 50  0000 C CNN
F 3 "" H 7750 4600 50  0000 C CNN
	1    7750 4600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 5A0B6295
P 8650 4600
F 0 "#PWR013" H 8650 4350 50  0001 C CNN
F 1 "GND" H 8650 4450 50  0000 C CNN
F 2 "" H 8650 4600 50  0000 C CNN
F 3 "" H 8650 4600 50  0000 C CNN
	1    8650 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 3900 8650 4600
Wire Wire Line
	8450 3900 8650 3900
Wire Wire Line
	7750 3900 7950 3900
Wire Wire Line
	7750 3900 7750 4600
Wire Wire Line
	6800 4100 7950 4100
$Comp
L +5V #PWR014
U 1 1 5A0B643E
P 7650 3800
F 0 "#PWR014" H 7650 3650 50  0001 C CNN
F 1 "+5V" H 7650 3940 50  0000 C CNN
F 2 "" H 7650 3800 50  0000 C CNN
F 3 "" H 7650 3800 50  0000 C CNN
	1    7650 3800
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR015
U 1 1 5A0B6466
P 8750 3800
F 0 "#PWR015" H 8750 3650 50  0001 C CNN
F 1 "+5V" H 8750 3940 50  0000 C CNN
F 2 "" H 8750 3800 50  0000 C CNN
F 3 "" H 8750 3800 50  0000 C CNN
	1    8750 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8750 4000 8750 3800
Wire Wire Line
	8450 4000 8750 4000
Wire Wire Line
	7550 4000 7950 4000
Wire Wire Line
	7650 4000 7650 3800
Connection ~ 6800 2650
Connection ~ 7650 4000
Wire Wire Line
	7250 4000 7150 4000
Wire Wire Line
	7150 4000 7150 4300
Connection ~ 7150 4300
$Comp
L GND #PWR016
U 1 1 5A0B6BA5
P 3300 5650
F 0 "#PWR016" H 3300 5400 50  0001 C CNN
F 1 "GND" H 3300 5500 50  0000 C CNN
F 2 "" H 3300 5650 50  0000 C CNN
F 3 "" H 3300 5650 50  0000 C CNN
	1    3300 5650
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR017
U 1 1 5A0B6BCD
P 3300 4950
F 0 "#PWR017" H 3300 4800 50  0001 C CNN
F 1 "+5V" H 3300 5090 50  0000 C CNN
F 2 "" H 3300 4950 50  0000 C CNN
F 3 "" H 3300 4950 50  0000 C CNN
	1    3300 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 5450 3300 5650
Wire Wire Line
	3300 4950 3300 5150
$Comp
L PWR_FLAG #FLG018
U 1 1 5A0B6CF4
P 2950 5000
F 0 "#FLG018" H 2950 5095 50  0001 C CNN
F 1 "PWR_FLAG" H 2950 5180 50  0000 C CNN
F 2 "" H 2950 5000 50  0000 C CNN
F 3 "" H 2950 5000 50  0000 C CNN
	1    2950 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 5000 2950 5050
Wire Wire Line
	2950 5050 3300 5050
Connection ~ 3300 5050
$Comp
L PWR_FLAG #FLG019
U 1 1 5A0B6D95
P 2950 5450
F 0 "#FLG019" H 2950 5545 50  0001 C CNN
F 1 "PWR_FLAG" H 2950 5630 50  0000 C CNN
F 2 "" H 2950 5450 50  0000 C CNN
F 3 "" H 2950 5450 50  0000 C CNN
	1    2950 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 5450 2950 5550
Wire Wire Line
	2950 5550 3300 5550
Connection ~ 3300 5550
$Comp
L EDGECONN_6X2 CN3
U 1 1 5A0B774A
P 8200 4150
F 0 "CN3" H 8200 3800 60  0000 C CNN
F 1 "EDGECONN_6X2" H 8200 4500 60  0000 C CNN
F 2 "VERTCONN_6X2:VERTCONN_6X2" H 11550 2700 60  0001 C CNN
F 3 "" H 11550 2700 60  0000 C CNN
	1    8200 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 3700 4400 4100
Wire Wire Line
	4400 4100 4700 4100
Wire Wire Line
	4700 3500 4500 3500
Wire Wire Line
	4500 3500 4500 3800
Wire Wire Line
	4600 3900 4600 3800
Wire Wire Line
	4600 3800 4700 3800
Wire Wire Line
	6300 2750 7900 2750
Wire Wire Line
	6300 2750 6300 4100
Wire Wire Line
	6300 4100 6100 4100
Wire Wire Line
	6400 2950 7900 2950
Wire Wire Line
	6400 2950 6400 3800
Wire Wire Line
	6400 3800 6100 3800
Wire Wire Line
	6100 3900 6500 3900
Wire Wire Line
	6500 2850 7900 2850
Wire Wire Line
	6500 2850 6500 3500
Wire Wire Line
	6500 3500 6100 3500
Wire Wire Line
	6600 3600 6100 3600
Wire Wire Line
	6500 3900 6500 4400
Wire Wire Line
	6600 4300 6600 3600
Wire Wire Line
	6800 4100 6800 2650
$Comp
L SW_SPDT SW1
U 1 1 5A0B8CB0
P 5800 5100
F 0 "SW1" H 5800 5270 50  0000 C CNN
F 1 "SW_SPDT" H 5800 4900 50  0000 C CNN
F 2 "Switch_TL36P0:Switch_TL36P0" H 5800 5100 50  0001 C CNN
F 3 "" H 5800 5100 50  0000 C CNN
	1    5800 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 5100 5600 5100
Connection ~ 5400 5100
Wire Wire Line
	6000 5000 6300 5000
Connection ~ 6200 5000
Wire Wire Line
	6000 5200 6100 5200
Wire Wire Line
	6100 5200 6100 5450
Wire Wire Line
	6100 5450 5400 5450
Connection ~ 5400 5450
$EndSCHEMATC
