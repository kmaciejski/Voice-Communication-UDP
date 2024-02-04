/**
 * \file
 *
 * \brief ADC related functionality implementation.
 *
 (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/**
 * \defgroup doc_driver_adc_init ADC Init Driver
 * \ingroup doc_driver_adc
 *
 * \section doc_driver_adc_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

#include <adc.h>

/**
 * \brief Initialize adc interface
 * \return Initialization status.
 */
int8_t ADC_0_init()
{

	// ADC0.CALIB = ADC_DUTYCYC_DUTY50_gc; /* 50% Duty cycle */

	ADC0.CTRLB = ADC_SAMPNUM_ACC8_gc; /* Accumulate 8 samples */

	ADC0.CTRLC = ADC_PRESC_DIV16_gc     /* CLK_PER divided by 16 */
	             | ADC_REFSEL_INTREF_gc /* Internal reference */
	             | 1 << ADC_SAMPCAP_bp; /* Sample Capacitance Selection: enabled */

	ADC0.CTRLD = 0 << ADC_ASDV_bp        /* Automatic Sampling Delay Variation: disabled */
	             | 0x1 << ADC_SAMPDLY_gp /* Sampling Delay Selection: 0x1 */
	             | ADC_INITDLY_DLY0_gc;  /* Delay 0 CLK_ADC cycles */

	// ADC0.CTRLE = ADC_WINCM_NONE_gc; /* No Window Comparison */

	// ADC0.DBGCTRL = 0 << ADC_DBGRUN_bp; /* Debug run: disabled */

	ADC0.EVCTRL = 1 << ADC_STARTEI_bp; /* Start Event Input Enable: enabled */

	ADC0.INTCTRL = 1 << ADC_RESRDY_bp  /* Result Ready Interrupt Enable: enabled */
	               | 0 << ADC_WCMP_bp; /* Window Comparator Interrupt Enable: disabled */

	ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc; /* ADC input pin 5 */

	// ADC0.SAMPCTRL = 0x0 << ADC_SAMPLEN_gp; /* Sample length: 0x0 */

	// ADC0.WINHT = 0x0; /* Window Comparator High Threshold: 0x0 */

	// ADC0.WINLT = 0x0; /* Window Comparator Low Threshold: 0x0 */

	ADC0.CTRLA = 1 << ADC_ENABLE_bp     /* ADC Enable: enabled */
	             | 0 << ADC_FREERUN_bp  /* ADC Freerun mode: disabled */
	             | ADC_RESSEL_10BIT_gc  /* 10-bit mode */
	             | 1 << ADC_RUNSTBY_bp; /* Run standby mode: enabled */

	return 0;
}
