/* Includes ------------------------------------------------------------------*/
#include "hw_i2c1.h"

// https://controllerstech.com/stm32-i2c-configuration-using-registers/

/* GPIO handlers */
static stmgpio_t *i2c1_scl;
static stmgpio_t *i2c1_sda;

int i2c1_init(void)
{
  /* We only need these during init */
  stmgpio_setup_t i2c1_scl_setup;
  stmgpio_setup_t i2c1_sda_setup;

  /* register interrupt handlers */
  RET_ON_FAIL(IRQ_SetHandler(I2C1_EV_IRQn, i2c1_ev_irq_handler));
  RET_ON_FAIL(IRQ_SetPriority(I2C1_EV_IRQn, I2C1_EV_IRQ_PRIORITY));
  RET_ON_FAIL(IRQ_Enable(I2C1_EV_IRQn));
  RET_ON_FAIL(IRQ_SetHandler(I2C1_ER_IRQn, i2c1_er_irq_handler));
  RET_ON_FAIL(IRQ_SetPriority(I2C1_ER_IRQn, I2C1_ER_IRQ_PRIORITY));
  RET_ON_FAIL(IRQ_Enable(I2C1_ER_IRQn));

  /* Enable the I2C1 clock */
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  /* USING PB8 and PB9, so enable port B clock if not already */
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

  /* Setup the GPIO pins */
  i2c1_scl_setup.port = I2C_SCL_PORT;
  i2c1_scl_setup.pin = I2C_SCL_PIN;
  i2c1_scl_setup.cfg = OUT_ALT_OPENDRAIN;
  i2c1_scl_setup.dir = OUTPUT_10MHZ;
  i2c1_scl_setup.pull = PULLUP;
  i2c1_scl = stmgpio_setup_gpio(&i2c1_scl_setup);

  i2c1_sda_setup.port = I2C_SDA_PORT;
  i2c1_sda_setup.pin = I2C_SDA_PIN;
  i2c1_sda_setup.cfg = OUT_ALT_OPENDRAIN;
  i2c1_sda_setup.dir = OUTPUT_10MHZ;
  i2c1_sda_setup.pull = PULLUP;
  i2c1_sda = stmgpio_setup_gpio(&i2c1_sda_setup);

  /* Set the pin remap for i2c1 */
  AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;

  /* Disable i2c1 peripheral to apply settings */
  I2C1->CR1 &= ~I2C_CR1_PE;

  /* Disable clock stretching */
  I2C1->CR1 |= I2C_CR1_NOSTRETCH;

  /* Disable general call */
  I2C1->CR1 &= ~I2C_CR1_ENGC;

  /* Disable SMBUS. Using I2C */
  I2C1->CR1 &= ~I2C_CR1_SMBUS;

  /* Disable DMA */
  I2C1->CR2 &= ~I2C_CR2_DMAEN;

  /* Enable DMA TX buffer interrupt */
  I2C1->CR2 |= I2C_CR2_ITBUFEN;

  /* Enable event interrupt */
  I2C1->CR2 |= I2C_CR2_ITEVTEN;

  /* Enable error interrupt */
  I2C1->CR2 |= I2C_CR2_ITERREN;

  /* Inform of APB bus frequency */
  I2C1->CR2 &= ~I2C_CR2_FREQ;
  I2C1->CR2 |= APB_BUS_FREQ_MHZ;

  /* Set to standard mode */
  I2C1->CCR &= ~I2C_CCR_FS;

  /* Set clock for 100kHz */
  I2C1->CCR &= ~I2C_CCR_CCR;
  I2C1->CCR |= 250;

  /* Set rise time for 1000ns */
  I2C1->TRISE &= ~I2C_TRISE_TRISE;
  I2C1->TRISE |= 50;

  /* Enable i2c1 peripheral */
  I2C1->CR1 |= I2C_CR1_PE;

  /* Trigger reset before first usage */
  i2c1_reset_periph();

  return EXIT_SUCCESS;
}

void i2c1_reset_periph(void)
{
  I2C1->CR1 |= I2C_CR1_SWRST;
  // make sure the I2C lines are released and the bus is free
  I2C1->CR1 &= ~I2C_CR1_SWRST;
}

void i2c1_enable_periph(void)
{
  I2C1->CR1 |= I2C_CR1_PE;
}

void i2c1_disable_periph(void)
{
  I2C1->CR1 &= ~I2C_CR1_PE;
}

void i2c1_start(void) {
  I2C1->CR1 |= I2C_CR1_ACK;
  I2C1->CR1 |= I2C_CR1_START;
}

uint8_t i2c1_read(void)
{

}

void i2c1_write(uint8_t data)
{
  while (!(I2C1->SR1 & (1 << 7))); // wait for TXE bit to set

  /* Set register bit */
  I2C1->DR = data;
  while (!(I2C1->SR1 & (1 << 2)))
    ; // wait for BTF bit to set
}

/* I2C1 event handler */
void i2c1_ev_irq_handler(void){

  IRQ_ClearPending(I2C1_EV_IRQn);
  IRQ_EndOfInterrupt (I2C1_EV_IRQn);
}

/* I2C1 error handler */
void i2c1_er_irq_handler(void){

  IRQ_ClearPending(I2C1_ER_IRQn);
  IRQ_EndOfInterrupt (I2C1_ER_IRQn);
}