/**
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 * Objetivo :
 *  - Introduzir ASF e HAL
 *  - Configuracao de clock
 *  - Configuracao pino In/Out
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

/*  Default pin configuration (no attribute). */
#define _PIO_DEFAULT             (0u << 0)
/*  The internal pin pull-up is active. */
#define _PIO_PULLUP              (1u << 0)
/*  The internal glitch filter is active. */
#define _PIO_DEGLITCH            (1u << 1)
/*  The internal debouncing filter is active. */
#define _PIO_DEBOUNCE            (1u << 3)

//Declarar o LED_1
#define LED_PIO_1    PIOA       
#define LED_PIO_ID_1        ID_PIOA
#define LED_PIO_IDX_1       0
#define LED_PIO_IDX_MASK_1  (1<< LED_PIO_IDX_1)

#define LED_PIO_2    PIOC       
#define LED_PIO_ID_2        ID_PIOC
#define LED_PIO_IDX_2       30
#define LED_PIO_IDX_MASK_2  (1<< LED_PIO_IDX_2)

#define LED_PIO_3    PIOB       
#define LED_PIO_ID_3        ID_PIOB
#define LED_PIO_IDX_3       2
#define LED_PIO_IDX_MASK_3  (1<< LED_PIO_IDX_3)

// Declarando o Botão

#define BUT_PIO_1		PIOD
#define BUT_PIO_ID_1		ID_PIOD
#define BUT_PIO_IDX_1		28
#define BUT_PIO_IDX_MASK_1	(1<< BUT_PIO_IDX_1)

#define BUT_PIO_2		PIOC
#define BUT_PIO_ID_2		ID_PIOC
#define BUT_PIO_IDX_2		31
#define BUT_PIO_IDX_MASK_2	(1<< BUT_PIO_IDX_2)

#define BUT_PIO_3		PIOA
#define BUT_PIO_ID_3		ID_PIOA
#define BUT_PIO_IDX_3		19
#define BUT_PIO_IDX_MASK_3	(1<< BUT_PIO_IDX_3)

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/
void _pio_set(Pio *p_pio, const uint32_t ul_mask){
	p_pio-> PIO_SODR = ul_mask;
}

void _pio_clear(Pio *p_pio, const uint32_t ul_mask){
	p_pio-> PIO_CODR = ul_mask;
}

void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable){
	if (ul_pull_up_enable){
		p_pio->PIO_PUER = ul_mask;
	}else {
		p_pio->PIO_PUDR = ul_mask;
	}
}

void _pio_set_input(Pio *p_pio, const uint32_t ul_mask,const uint32_t ul_attribute){
	_pio_pull_up(p_pio,ul_mask,ul_attribute & _PIO_PULLUP);
	if(ul_attribute & _PIO_DEGLITCH){
		p_pio-> PIO_SCDR = ul_mask;
	}
	if(ul_attribute & _PIO_DEBOUNCE){
		p_pio-> PIO_SCDR = ul_mask;
	}
	if(ul_attribute & _PIO_PULLUP){
		p_pio-> PIO_SCDR = ul_mask;
	}
	
	
}

void _pio_set_output(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_default_level,
const uint32_t ul_multidrive_enable,
const uint32_t ul_pull_up_enable)
{
	p_pio->PIO_OER = ul_mask;
	p_pio->PIO_PER = ul_mask;
	_pio_pull_up(p_pio,ul_mask,ul_default_level & ul_multidrive_enable & ul_pull_up_enable);
	if (ul_multidrive_enable) {
		p_pio->PIO_PER = ul_mask;
		} else {
		p_pio->PIO_PDR = ul_mask;
		
	}
	
	if (ul_default_level) {
		p_pio->PIO_SODR = ul_mask;
		} else {
		p_pio->PIO_CODR = ul_mask;
	}
	

}

uint32_t _pio_get(Pio *p_pio, const pio_type_t ul_type,
        const uint32_t ul_mask)
{
	uint32_t ul_register;

	if ((ul_type == PIO_OUTPUT_0)) {
		ul_register = p_pio->PIO_ODSR;
	} else {
		ul_register = p_pio->PIO_PDSR;
	}

	if ((ul_register & ul_mask) == 0) {
		return 0;
	} else {
		return 1;
	}
}

void _delay_ms(int ms){
	int tempo = ms*200000;
	for(int i =0; i< tempo ; i++){
		asm("nop");
	}
}

// Função de inicialização do uC
void init(void)
{
	// Inicializa o clock
	sysclk_init();
	
	//Desativa o WatchDog Timer
	WDT ->WDT_MR = WDT_MR_WDDIS;
	
	// Ativa o PIO na qual o led foi conectado, para controlar o LED
	pmc_enable_periph_clk(LED_PIO_ID_1);
	pmc_enable_periph_clk(LED_PIO_ID_2);
	pmc_enable_periph_clk(LED_PIO_ID_3);
	
	
	// Ativa o botão
	pmc_enable_periph_clk(BUT_PIO_ID_1);
	pmc_enable_periph_clk(BUT_PIO_ID_2);
	pmc_enable_periph_clk(BUT_PIO_ID_3);
	
	//Inicializa PC8 como saída
	_pio_set_output(LED_PIO_1,LED_PIO_IDX_MASK_1, 0, 0, 0);
	_pio_set_output(LED_PIO_2,LED_PIO_IDX_MASK_2, 0, 0, 0);
	_pio_set_output(LED_PIO_3,LED_PIO_IDX_MASK_3, 0, 0, 0);
	
	// inicializa o botão
	pmc_enable_periph_clk(BUT_PIO_ID_1);
	pmc_enable_periph_clk(BUT_PIO_ID_2);
	pmc_enable_periph_clk(BUT_PIO_ID_3);
	

	_pio_set_input(BUT_PIO_1, BUT_PIO_IDX_MASK_1, _PIO_PULLUP | _PIO_DEBOUNCE);
	_pio_set_input(BUT_PIO_2, BUT_PIO_IDX_MASK_2, _PIO_PULLUP | _PIO_DEBOUNCE);
	_pio_set_input(BUT_PIO_3, BUT_PIO_IDX_MASK_3, _PIO_PULLUP | _PIO_DEBOUNCE);



	
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void)
{


  init();

  // super loop
  // aplicacoes embarcadas não devem sair do while(1).
  while (1)
  {
	if (_pio_get(BUT_PIO_1,PIO_INPUT, BUT_PIO_IDX_MASK_1)==0)
	{
		_pio_set(LED_PIO_1, LED_PIO_IDX_MASK_1);
		_delay_ms(500);
		_pio_clear(LED_PIO_1, LED_PIO_IDX_MASK_1);
		_delay_ms(1000);
	}
	if (_pio_get(BUT_PIO_2,PIO_INPUT, BUT_PIO_IDX_MASK_2)==0)
	{
		_pio_set(LED_PIO_2, LED_PIO_IDX_MASK_2);
		_delay_ms(500);
		_pio_clear(LED_PIO_2, LED_PIO_IDX_MASK_2);
		_delay_ms(100);
	}
	
	if (_pio_get(BUT_PIO_3,PIO_INPUT, BUT_PIO_IDX_MASK_3)==0)
	{
		_pio_set(LED_PIO_3, LED_PIO_IDX_MASK_3);
		_delay_ms(500);
		_pio_clear(LED_PIO_3, LED_PIO_IDX_MASK_3);
		_delay_ms(10000);
	}
	
	}
  return 0;
}
