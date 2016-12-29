/* startup code for ATSAMD09
 *
 * Written 2016 by Ingo Korb, no copyright claimed
 */
        .syntax unified

	.section .vectors

	.macro	except label
	.weak	\label
	.set	\label, __unhandled_exception
	.word	\label
	.endm

	/* Cortex M0-DS exception vectors */
	.word	__stack
	.word	_start                 //  1 - Reset
	except	NMI_Handler            //  2 - NMI
	except	HardFault_Handler      //  3 - HardFault
	.word	__unhandled_exception  //  4 - reserved
	.word	__unhandled_exception  //  5 - reserved
	.word	__unhandled_exception  //  6 - reserved
	.word	__unhandled_exception  //  7 - reserved
	.word	__unhandled_exception  //  8 - reserved
	.word	__unhandled_exception  //  9 - reserved
	.word	__unhandled_exception  // 10 - reserved

	except	SVC_Handler            // 11 - SVCall

	.word	__unhandled_exception  // 12 - reserved
	.word	__unhandled_exception  // 13 - reserved

	except	PendSV_Handler         // 14 - PendSV
	except	SysTick_Handler        // 15 - SysTick

	except  PM_Handler             // 16 - Power Manager
	except  SYSCTRL_Handler        // 17 - System Control
	except  WDR_Handler            // 18 - Watchdog Timer
	except  RTC_Handler            // 19 - Real-Time Counter

	except  EIC_Handler            // 20 - External Interrupt Controller
	except  NVMCTRL_Handler        // 21 - Non-Volatile Memory Controller
	except  DMAC_Handler           // 22 - Direct Memory Access Controller
	except  IRQ7_Handler           // 23 - Interrupt 7 (Reserved)
	
	except  EVSYS_Handler          // 24 - Event System Interface
	except  SERCOM0_Handler        // 25 - Serial Communication Interface 0
	except  SERCOM1_Handler        // 26 - Serial Communication Interface 1
	except  IRQ11_Handler          // 27 - Interrupt 11 (Reserved)
	
	except  IRQ12_Handler          // 28 - Interrupt 12 (Reserved)
	except  TC1_Handler            // 29 - Basic Timer Counter 1
	except  TC2_Handler            // 30 - Basic Timer Counter 2
	except  ADC_Handler            // 31 - Analog Digital Converter

        except  IRQ16_Handler          // 32 - Interrupt 16 (Reserved)
        except  IRQ17_Handler          // 33 - Interrupt 17 (Reserved)
        except  IRQ18_Handler          // 34 - Interrupt 18 (Reserved)
        except  IRQ19_Handler          // 35 - Interrupt 19 (Reserved)
	
        except  IRQ20_Handler          // 36 - Interrupt 20 (Reserved)
        except  IRQ21_Handler          // 37 - Interrupt 21 (Reserved)
        except  IRQ22_Handler          // 38 - Interrupt 22 (Reserved)
        except  IRQ23_Handler          // 39 - Interrupt 23 (Reserved)
	
        except  IRQ24_Handler          // 40 - Interrupt 24 (Reserved)
        except  IRQ25_Handler          // 41 - Interrupt 25 (Reserved)
        except  IRQ26_Handler          // 42 - Interrupt 26 (Reserved)
        except  IRQ27_Handler          // 43 - Interrupt 27 (Reserved)
	
        except  IRQ28_Handler          // 44 - Interrupt 28 (Reserved)
        except  IRQ29_Handler          // 45 - Interrupt 29 (Reserved)
        except  IRQ30_Handler          // 46 - Interrupt 30 (Reserved)
        except  IRQ31_Handler          // 47 - Interrupt 31 (Reserved)
	
 
	.section .text

	/* handler for otherwise-unused exceptions */
	.weak __unhandled_exception
	.thumb_func
__unhandled_exception:
        b       __unhandled_exception
	
	
	.global _start
	.thumb_func
_start:
        /* copy data section to RAM */
        ldr     r0, =__data_load_start
        ldr     r1, =__data_load_end
        ldr     r2, =__data_start

dataloop:
        cmp     r0, r1
        bge     dataexit
        ldr     r3, [r0]
        str     r3, [r2]
        adds    r0, r0, #4
        adds    r2, r2, #4
        b       dataloop
dataexit:

	/* clear BSS */
        ldr     r0, =0
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__
bssloop:
        cmp     r1, r2
        bge     bssexit
	str	r0, [r1]
	adds	r1, r1, #4
        b       bssloop
bssexit:

        // FIXME: Set up process stack if used (msp is set by hardware)
        
	/* start main() */
	bl	main

mainexited:
        b       mainexited
