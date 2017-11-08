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
LIBS:w25q16dv
LIBS:lp2950-3.3
LIBS:edgeconn_2x6
LIBS:tapecart-diy-cache
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
L ATTINY44-P U2
U 1 1 5872C338
P 2150 4750
F 0 "U2" H 1300 5500 50  0000 C CNN
F 1 "ATTINY44-P" H 2850 4000 50  0000 C CNN
F 2 "Housings_DIP:DIP-14_W7.62mm_LongPads" H 2150 4550 50  0001 C CIN
F 3 "" H 2150 4750 50  0000 C CNN
	1    2150 4750
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X03 P2
U 1 1 59386D77
P 10000 3600
F 0 "P2" H 10000 3800 50  0000 C CNN
F 1 "CONN_02X03" H 10000 3400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03" H 10000 2400 50  0001 C CNN
F 3 "" H 10000 2400 50  0000 C CNN
	1    10000 3600
	1    0    0    -1  
$EndComp
$Comp
L W25Q16DV U3
U 1 1 593870AF
P 7300 4800
F 0 "U3" H 7550 5100 60  0000 C CNN
F 1 "W25Q16DV" H 7300 4500 60  0000 C CNN
F 2 "Housings_DIP:DIP-8_W7.62mm_LongPads" H 7300 5200 60  0000 C CNN
F 3 "" H 7450 5200 60  0000 C CNN
	1    7300 4800
	1    0    0    -1  
$EndComp
$Comp
L LP2950-3.3 U1
U 1 1 59387569
P 9550 1250
F 0 "U1" H 9700 1054 50  0000 C CNN
F 1 "LP2950-3.3" H 9550 1450 50  0000 C CNN
F 2 "TO_SOT_Packages_THT:TO-92_Inline_Wide" H 9550 1250 50  0001 C CNN
F 3 "" H 9550 1250 50  0000 C CNN
	1    9550 1250
	1    0    0    -1  
$EndComp
$Comp
L CP C2
U 1 1 593875B8
P 10100 1500
F 0 "C2" H 10125 1600 50  0000 L CNN
F 1 "4.7uF" H 10125 1400 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D5_L11_P2" H 10138 1350 50  0001 C CNN
F 3 "" H 10100 1500 50  0000 C CNN
	1    10100 1500
	1    0    0    -1  
$EndComp
$Comp
L CP C1
U 1 1 593875F5
P 9000 1500
F 0 "C1" H 9025 1600 50  0000 L CNN
F 1 "4.7uF" H 9025 1400 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D5_L11_P2" H 9038 1350 50  0001 C CNN
F 3 "" H 9000 1500 50  0000 C CNN
	1    9000 1500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 593877EC
P 9550 1850
F 0 "#PWR01" H 9550 1600 50  0001 C CNN
F 1 "GND" H 9550 1700 50  0000 C CNN
F 2 "" H 9550 1850 50  0000 C CNN
F 3 "" H 9550 1850 50  0000 C CNN
	1    9550 1850
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR02
U 1 1 5938789C
P 10500 1100
F 0 "#PWR02" H 10500 950 50  0001 C CNN
F 1 "+3.3V" H 10500 1240 50  0000 C CNN
F 2 "" H 10500 1100 50  0000 C CNN
F 3 "" H 10500 1100 50  0000 C CNN
	1    10500 1100
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR03
U 1 1 593878BE
P 8600 1100
F 0 "#PWR03" H 8600 950 50  0001 C CNN
F 1 "+5V" H 8600 1240 50  0000 C CNN
F 2 "" H 8600 1100 50  0000 C CNN
F 3 "" H 8600 1100 50  0000 C CNN
	1    8600 1100
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG04
U 1 1 593878E7
P 9000 1100
F 0 "#FLG04" H 9000 1195 50  0001 C CNN
F 1 "PWR_FLAG" H 9000 1280 50  0000 C CNN
F 2 "" H 9000 1100 50  0000 C CNN
F 3 "" H 9000 1100 50  0000 C CNN
	1    9000 1100
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG05
U 1 1 59387909
P 8600 1600
F 0 "#FLG05" H 8600 1695 50  0001 C CNN
F 1 "PWR_FLAG" H 8600 1780 50  0000 C CNN
F 2 "" H 8600 1600 50  0000 C CNN
F 3 "" H 8600 1600 50  0000 C CNN
	1    8600 1600
	1    0    0    -1  
$EndComp
$Comp
L Resonator Y1
U 1 1 59387B7A
P 3500 5850
F 0 "Y1" H 3500 6075 50  0000 C CNN
F 1 "8 MHz" H 3500 6000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 3475 5850 50  0001 C CNN
F 3 "" H 3475 5850 50  0000 C CNN
	1    3500 5850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 59387DF4
P 3500 6150
F 0 "#PWR06" H 3500 5900 50  0001 C CNN
F 1 "GND" H 3500 6000 50  0000 C CNN
F 2 "" H 3500 6150 50  0000 C CNN
F 3 "" H 3500 6150 50  0000 C CNN
	1    3500 6150
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 59387E38
P 950 4700
F 0 "C3" H 975 4800 50  0000 L CNN
F 1 "100nF" H 975 4600 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 988 4550 50  0001 C CNN
F 3 "" H 950 4700 50  0000 C CNN
	1    950  4700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR07
U 1 1 59387EB7
P 950 5500
F 0 "#PWR07" H 950 5250 50  0001 C CNN
F 1 "GND" H 950 5350 50  0000 C CNN
F 2 "" H 950 5500 50  0000 C CNN
F 3 "" H 950 5500 50  0000 C CNN
	1    950  5500
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR08
U 1 1 59387EDD
P 950 4000
F 0 "#PWR08" H 950 3850 50  0001 C CNN
F 1 "+3.3V" H 950 4140 50  0000 C CNN
F 2 "" H 950 4000 50  0000 C CNN
F 3 "" H 950 4000 50  0000 C CNN
	1    950  4000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 593881E8
P 8450 6050
F 0 "#PWR09" H 8450 5800 50  0001 C CNN
F 1 "GND" H 8450 5900 50  0000 C CNN
F 2 "" H 8450 6050 50  0000 C CNN
F 3 "" H 8450 6050 50  0000 C CNN
	1    8450 6050
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR010
U 1 1 5938820E
P 8450 4200
F 0 "#PWR010" H 8450 4050 50  0001 C CNN
F 1 "+3.3V" H 8450 4340 50  0000 C CNN
F 2 "" H 8450 4200 50  0000 C CNN
F 3 "" H 8450 4200 50  0000 C CNN
	1    8450 4200
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 59388234
P 8450 4800
F 0 "C4" H 8475 4900 50  0000 L CNN
F 1 "100nF" H 8475 4700 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 8488 4650 50  0001 C CNN
F 3 "" H 8450 4800 50  0000 C CNN
	1    8450 4800
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR011
U 1 1 593886F3
P 6650 4200
F 0 "#PWR011" H 6650 4050 50  0001 C CNN
F 1 "+3.3V" H 6650 4340 50  0000 C CNN
F 2 "" H 6650 4200 50  0000 C CNN
F 3 "" H 6650 4200 50  0000 C CNN
	1    6650 4200
	1    0    0    -1  
$EndComp
Text Notes 8350 1950 0    60   ~ 0
Input: 1uF or more\nAlu/Tantal/Ceramic ok
Text Notes 9700 2050 0    60   ~ 0
Output: 2.2uF or more\nAlu/Tantal ok, NO Ceramic!\nESR min 0.05ohm, max 5ohm
$Comp
L LED D1
U 1 1 59388FDE
P 4000 5250
F 0 "D1" H 4000 5350 50  0000 C CNN
F 1 "LED" H 4000 5150 50  0000 C CNN
F 2 "LEDs:LED-3MM" H 4000 5250 50  0001 C CNN
F 3 "" H 4000 5250 50  0000 C CNN
	1    4000 5250
	1    0    0    -1  
$EndComp
$Comp
L R R8
U 1 1 5938900F
P 4500 5250
F 0 "R8" V 4580 5250 50  0000 C CNN
F 1 "330" V 4500 5250 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 4430 5250 50  0001 C CNN
F 3 "" H 4500 5250 50  0000 C CNN
	1    4500 5250
	0    1    1    0   
$EndComp
$Comp
L BS170 Q1
U 1 1 59389058
P 4200 2350
F 0 "Q1" H 4400 2425 50  0000 L CNN
F 1 "BS170" H 4400 2350 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Inline_Wide" H 4400 2275 50  0001 L CIN
F 3 "" H 4200 2350 50  0000 L CNN
	1    4200 2350
	1    0    0    -1  
$EndComp
$Comp
L BS170 Q2
U 1 1 593890BF
P 5300 2350
F 0 "Q2" H 5500 2425 50  0000 L CNN
F 1 "BS170" H 5500 2350 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Inline_Wide" H 5500 2275 50  0001 L CIN
F 3 "" H 5300 2350 50  0000 L CNN
	1    5300 2350
	-1   0    0    -1  
$EndComp
$Comp
L BS170 Q3
U 1 1 593890FC
P 6100 2350
F 0 "Q3" H 6300 2425 50  0000 L CNN
F 1 "BS170" H 6300 2350 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Inline_Wide" H 6300 2275 50  0001 L CIN
F 3 "" H 6100 2350 50  0000 L CNN
	1    6100 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR012
U 1 1 5939C508
P 10350 4350
F 0 "#PWR012" H 10350 4100 50  0001 C CNN
F 1 "GND" H 10350 4200 50  0000 C CNN
F 2 "" H 10350 4350 50  0000 C CNN
F 3 "" H 10350 4350 50  0000 C CNN
	1    10350 4350
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR013
U 1 1 5939C53A
P 10350 3400
F 0 "#PWR013" H 10350 3250 50  0001 C CNN
F 1 "+3.3V" H 10350 3540 50  0000 C CNN
F 2 "" H 10350 3400 50  0000 C CNN
F 3 "" H 10350 3400 50  0000 C CNN
	1    10350 3400
	1    0    0    -1  
$EndComp
Text Label 10350 3600 0    60   ~ 0
MOSI
Text Label 9600 3500 2    60   ~ 0
MISO
Text Label 9600 3600 2    60   ~ 0
SCK
Text Label 9600 3700 2    60   ~ 0
Reset
Text Label 3450 4050 1    60   ~ 0
Motor3
Text Label 3650 4050 1    60   ~ 0
Read3
Text Label 3450 4350 0    60   ~ 0
SSEL
Text Label 3450 4450 0    60   ~ 0
Write3
Text Label 3450 4550 0    60   ~ 0
SCK
Text Label 3450 4650 0    60   ~ 0
MISO
Text Label 3450 4750 0    60   ~ 0
MOSI
Text Label 3450 4850 0    60   ~ 0
Sense3
Text Label 3400 5350 0    60   ~ 0
Reset
$Comp
L EDGECONN_6X2 CN1
U 1 1 5939EA47
P 1800 1450
F 0 "CN1" H 1800 1100 60  0000 C CNN
F 1 "EDGECONN_6X2" H 1800 1800 60  0000 C CNN
F 2 "edgeconn_6x2:EDGECONN_6x2_IMP_5mm" H 5150 0   60  0001 C CNN
F 3 "" H 5150 0   60  0000 C CNN
	1    1800 1450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 5939EC6C
P 2150 1800
F 0 "#PWR014" H 2150 1550 50  0001 C CNN
F 1 "GND" H 2150 1650 50  0000 C CNN
F 2 "" H 2150 1800 50  0000 C CNN
F 3 "" H 2150 1800 50  0000 C CNN
	1    2150 1800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 5939EC9E
P 1450 1800
F 0 "#PWR015" H 1450 1550 50  0001 C CNN
F 1 "GND" H 1450 1650 50  0000 C CNN
F 2 "" H 1450 1800 50  0000 C CNN
F 3 "" H 1450 1800 50  0000 C CNN
	1    1450 1800
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR016
U 1 1 5939EF01
P 2250 1100
F 0 "#PWR016" H 2250 950 50  0001 C CNN
F 1 "+5V" H 2250 1240 50  0000 C CNN
F 2 "" H 2250 1100 50  0000 C CNN
F 3 "" H 2250 1100 50  0000 C CNN
	1    2250 1100
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR017
U 1 1 5939EF33
P 1350 1100
F 0 "#PWR017" H 1350 950 50  0001 C CNN
F 1 "+5V" H 1350 1240 50  0000 C CNN
F 2 "" H 1350 1100 50  0000 C CNN
F 3 "" H 1350 1100 50  0000 C CNN
	1    1350 1100
	1    0    0    -1  
$EndComp
Text Label 2250 1400 0    60   ~ 0
Motor5
Text Label 2250 1500 0    60   ~ 0
Read5
Text Label 2250 1600 0    60   ~ 0
Write5
Text Label 2250 1700 0    60   ~ 0
Sense5
Text Label 1350 1400 2    60   ~ 0
Motor5
Text Label 1350 1500 2    60   ~ 0
Read5
Text Label 1350 1600 2    60   ~ 0
Write5
Text Label 1350 1700 2    60   ~ 0
Sense5
$Comp
L R R1
U 1 1 5939FD6C
P 3150 2100
F 0 "R1" V 3230 2100 50  0000 C CNN
F 1 "680" V 3150 2100 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 3080 2100 50  0001 C CNN
F 3 "" H 3150 2100 50  0000 C CNN
	1    3150 2100
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 5939FDA3
P 3150 2700
F 0 "R2" V 3230 2700 50  0000 C CNN
F 1 "560" V 3150 2700 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 3080 2700 50  0001 C CNN
F 3 "" H 3150 2700 50  0000 C CNN
	1    3150 2700
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 5939FDDC
P 3850 2700
F 0 "R3" V 3930 2700 50  0000 C CNN
F 1 "3.3k" V 3850 2700 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 3780 2700 50  0001 C CNN
F 3 "" H 3850 2700 50  0000 C CNN
	1    3850 2700
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 593A013F
P 5200 1250
F 0 "R4" V 5280 1250 50  0000 C CNN
F 1 "3.3k" V 5200 1250 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 5130 1250 50  0001 C CNN
F 3 "" H 5200 1250 50  0000 C CNN
	1    5200 1250
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 593A017C
P 6200 1250
F 0 "R5" V 6280 1250 50  0000 C CNN
F 1 "3.3k" V 6200 1250 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 6130 1250 50  0001 C CNN
F 3 "" H 6200 1250 50  0000 C CNN
	1    6200 1250
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 593A01BB
P 5450 2700
F 0 "R6" V 5530 2700 50  0000 C CNN
F 1 "3.3k" V 5450 2700 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 5380 2700 50  0001 C CNN
F 3 "" H 5450 2700 50  0000 C CNN
	1    5450 2700
	0    1    1    0   
$EndComp
$Comp
L R R7
U 1 1 593A01FE
P 5950 2700
F 0 "R7" V 6030 2700 50  0000 C CNN
F 1 "3.3k" V 5950 2700 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 5880 2700 50  0001 C CNN
F 3 "" H 5950 2700 50  0000 C CNN
	1    5950 2700
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR018
U 1 1 593A0374
P 5200 900
F 0 "#PWR018" H 5200 750 50  0001 C CNN
F 1 "+5V" H 5200 1040 50  0000 C CNN
F 2 "" H 5200 900 50  0000 C CNN
F 3 "" H 5200 900 50  0000 C CNN
	1    5200 900 
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR019
U 1 1 593A03B6
P 6200 900
F 0 "#PWR019" H 6200 750 50  0001 C CNN
F 1 "+5V" H 6200 1040 50  0000 C CNN
F 2 "" H 6200 900 50  0000 C CNN
F 3 "" H 6200 900 50  0000 C CNN
	1    6200 900 
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR020
U 1 1 593A03F8
P 5700 2150
F 0 "#PWR020" H 5700 2000 50  0001 C CNN
F 1 "+3.3V" H 5700 2290 50  0000 C CNN
F 2 "" H 5700 2150 50  0000 C CNN
F 3 "" H 5700 2150 50  0000 C CNN
	1    5700 2150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR021
U 1 1 593A0595
P 3150 3200
F 0 "#PWR021" H 3150 2950 50  0001 C CNN
F 1 "GND" H 3150 3050 50  0000 C CNN
F 2 "" H 3150 3200 50  0000 C CNN
F 3 "" H 3150 3200 50  0000 C CNN
	1    3150 3200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR022
U 1 1 593A05D7
P 4300 3200
F 0 "#PWR022" H 4300 2950 50  0001 C CNN
F 1 "GND" H 4300 3050 50  0000 C CNN
F 2 "" H 4300 3200 50  0000 C CNN
F 3 "" H 4300 3200 50  0000 C CNN
	1    4300 3200
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 593B5D56
P 10000 4100
F 0 "C5" H 10025 4200 50  0000 L CNN
F 1 "100nF" H 10025 4000 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 10038 3950 50  0001 C CNN
F 3 "" H 10000 4100 50  0000 C CNN
	1    10000 4100
	0    1    1    0   
$EndComp
$Comp
L W25Q16DV U4
U 1 1 593BAE9C
P 7300 5650
F 0 "U4" H 7550 5950 60  0000 C CNN
F 1 "W25Q16DV" H 7300 5350 60  0000 C CNN
F 2 "Housings_SOIC:SOIJ-8_5.3x5.3mm_Pitch1.27mm" H 7300 5250 60  0000 C CNN
F 3 "" H 7450 6050 60  0000 C CNN
	1    7300 5650
	1    0    0    -1  
$EndComp
Text Notes 8300 6450 2    60   ~ 0
Note: Place either U3 or U4, not both\n(alternative footprints)
$Comp
L +3.3V #PWR023
U 1 1 593BB300
P 8250 5300
F 0 "#PWR023" H 8250 5150 50  0001 C CNN
F 1 "+3.3V" H 8250 5440 50  0000 C CNN
F 2 "" H 8250 5300 50  0000 C CNN
F 3 "" H 8250 5300 50  0000 C CNN
	1    8250 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9550 1500 9550 1850
Wire Wire Line
	10100 1750 10100 1650
Wire Wire Line
	8600 1750 10100 1750
Connection ~ 9550 1750
Wire Wire Line
	9000 1650 9000 1750
Wire Wire Line
	8600 1200 9150 1200
Wire Wire Line
	9000 1100 9000 1350
Wire Wire Line
	9950 1200 10500 1200
Wire Wire Line
	8600 1100 8600 1200
Connection ~ 9000 1200
Connection ~ 10100 1200
Wire Wire Line
	10500 1200 10500 1100
Wire Wire Line
	950  4000 950  4550
Wire Wire Line
	950  4150 1100 4150
Connection ~ 950  4150
Wire Wire Line
	950  4850 950  5500
Wire Wire Line
	950  5350 1100 5350
Connection ~ 950  5350
Wire Wire Line
	7850 4600 8450 4600
Wire Wire Line
	8450 4200 8450 4650
Connection ~ 8450 4600
Wire Wire Line
	8450 4950 8450 6050
Wire Wire Line
	8450 5000 7850 5000
Connection ~ 8450 5000
Wire Wire Line
	6650 4850 6750 4850
Wire Wire Line
	6650 4950 6750 4950
Connection ~ 6650 4850
Wire Wire Line
	3500 6150 3500 6050
Wire Wire Line
	3200 5050 3700 5050
Wire Wire Line
	3200 5150 3300 5150
Wire Wire Line
	3300 5150 3300 5850
Wire Wire Line
	3300 5850 3350 5850
Wire Wire Line
	3650 5850 3700 5850
Wire Wire Line
	3700 5850 3700 5050
Wire Wire Line
	3850 5250 3200 5250
Wire Wire Line
	10350 3400 10350 3500
Wire Wire Line
	10350 3500 10250 3500
Wire Wire Line
	10250 3700 10350 3700
Wire Wire Line
	10350 3700 10350 4350
Wire Wire Line
	10350 3600 10250 3600
Wire Wire Line
	9600 3500 9750 3500
Wire Wire Line
	9600 3600 9750 3600
Wire Wire Line
	9600 3700 9750 3700
Wire Wire Line
	3400 5350 3200 5350
Wire Wire Line
	3450 4150 3200 4150
Wire Wire Line
	3650 4250 3200 4250
Wire Wire Line
	3200 4350 8050 4350
Wire Wire Line
	6200 4450 3200 4450
Wire Wire Line
	3200 4550 6750 4550
Wire Wire Line
	3200 4650 6750 4650
Wire Wire Line
	3200 4750 6750 4750
Wire Wire Line
	5200 4850 3200 4850
Wire Wire Line
	8050 4350 8050 5600
Wire Wire Line
	8050 4750 7850 4750
Wire Wire Line
	6750 4550 6750 4450
Wire Wire Line
	6750 4450 7950 4450
Wire Wire Line
	7950 4450 7950 5700
Wire Wire Line
	7950 4850 7850 4850
Wire Wire Line
	1550 1200 1450 1200
Wire Wire Line
	1450 1200 1450 1800
Wire Wire Line
	2050 1200 2800 1200
Wire Wire Line
	2150 1200 2150 1800
Wire Wire Line
	2700 1300 2050 1300
Wire Wire Line
	2250 1300 2250 1100
Wire Wire Line
	1550 1300 1350 1300
Wire Wire Line
	1350 1300 1350 1100
Wire Wire Line
	2700 1000 2700 1300
Connection ~ 2250 1300
Wire Wire Line
	1350 1700 1550 1700
Wire Wire Line
	1550 1600 1350 1600
Wire Wire Line
	1350 1500 1550 1500
Wire Wire Line
	1550 1400 1350 1400
Wire Wire Line
	2050 1400 3150 1400
Wire Wire Line
	2050 1500 4300 1500
Wire Wire Line
	2050 1600 6200 1600
Wire Wire Line
	2050 1700 5200 1700
Wire Wire Line
	3150 2850 3150 3200
Wire Wire Line
	3150 2250 3150 2550
Wire Wire Line
	3450 4150 3450 2400
Wire Wire Line
	3450 2400 3150 2400
Connection ~ 3150 2400
Wire Wire Line
	3150 1400 3150 1950
Wire Wire Line
	3650 2400 3650 4250
Wire Wire Line
	3650 2400 4000 2400
Wire Wire Line
	3850 2400 3850 2550
Connection ~ 3850 2400
Wire Wire Line
	3850 2850 3850 3000
Wire Wire Line
	3850 3000 4300 3000
Wire Wire Line
	4300 2550 4300 3200
Connection ~ 4300 3000
Wire Wire Line
	4300 1500 4300 2150
Wire Wire Line
	5500 2400 5900 2400
Wire Wire Line
	5600 2700 5800 2700
Wire Wire Line
	5700 2150 5700 2700
Connection ~ 5700 2400
Connection ~ 5700 2700
Wire Wire Line
	5300 2700 5200 2700
Wire Wire Line
	5200 2550 5200 4850
Wire Wire Line
	6100 2700 6200 2700
Wire Wire Line
	6200 2550 6200 4450
Wire Wire Line
	6650 4200 6650 5800
Wire Wire Line
	5200 1400 5200 2150
Wire Wire Line
	6200 1400 6200 2150
Wire Wire Line
	6200 900  6200 1100
Wire Wire Line
	5200 1100 5200 900 
Connection ~ 5200 1700
Connection ~ 6200 1600
Connection ~ 5200 2700
Connection ~ 6200 2700
Wire Wire Line
	10100 1200 10100 1350
Wire Wire Line
	8600 1600 8600 1750
Connection ~ 9000 1750
Wire Wire Line
	10150 4100 10350 4100
Connection ~ 10350 4100
Wire Wire Line
	9850 4100 9700 4100
Wire Wire Line
	9700 4100 9700 3700
Connection ~ 9700 3700
Wire Wire Line
	7950 5700 7850 5700
Connection ~ 7950 4850
Wire Wire Line
	8050 5600 7850 5600
Connection ~ 8050 4750
Wire Wire Line
	7850 5850 8450 5850
Connection ~ 8450 5850
Wire Wire Line
	7850 5450 8250 5450
Wire Wire Line
	8250 5450 8250 5300
Wire Wire Line
	6650 5800 6750 5800
Connection ~ 6650 4950
Wire Wire Line
	6750 5700 6650 5700
Connection ~ 6650 5700
Wire Wire Line
	6750 5500 6550 5500
Wire Wire Line
	6550 5500 6550 4650
Connection ~ 6550 4650
Wire Wire Line
	6750 5600 6450 5600
Wire Wire Line
	6450 5600 6450 4750
Connection ~ 6450 4750
Wire Wire Line
	4350 5250 4150 5250
Wire Wire Line
	4750 5250 4650 5250
$Comp
L +3.3V #PWR024
U 1 1 59989E3C
P 4750 5100
F 0 "#PWR024" H 4750 4950 50  0001 C CNN
F 1 "+3.3V" H 4750 5240 50  0000 C CNN
F 2 "" H 4750 5100 50  0000 C CNN
F 3 "" H 4750 5100 50  0000 C CNN
	1    4750 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 5100 4750 5250
$Comp
L CONN_01X02 P1
U 1 1 5998A64A
P 2750 800
F 0 "P1" H 2750 950 50  0000 C CNN
F 1 "CONN_01X02" V 2850 800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 2750 800 50  0001 C CNN
F 3 "" H 2750 800 50  0000 C CNN
	1    2750 800 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2800 1200 2800 1000
Connection ~ 2150 1200
$EndSCHEMATC
