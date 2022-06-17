/* Includes ------------------------------------------------------------------*/
#include "hw_i2c1.h"

/* interrupt vars */
static volatile uint8_t idx;
static volatile I2C1_EVT last_event;
static volatile I2C1_ERR last_error;
static volatile I2C1_STATE curr_state;

/* GPIO handlers */
static stmgpio_t i2c1_scl;
static stmgpio_t i2c1_sda;

/* interrupt callbacks */
static void (*i2c1_evt_callback)(void);
static void (*i2c1_err_callback)(void);

/* */
const I2C1_EVT EVT_LIST[] = {
    I2C1_EVT.I2C1_EVT_SB,
    I2C1_EVT.I2C1_EVT_ADDR,
    I2C1_EVT.I2C1_EVT_ADD10,
    I2C1_EVT.I2C1_EVT_BTF,
    I2C1_EVT.I2C1_EVT_TxE,
    I2C1_EVT.I2C1_EVT_RxNE};

/* */
const I2C1_ERR ERR_LIST[] = {
    I2C1_ERR.I2C1_ERR_BERR,
    I2C1_ERR.I2C1_ERR_ARLO,
    I2C1_ERR.I2C1_ERR_AF,
    I2C1_ERR.I2C1_ERR_OVR,
    I2C1_ERR.I2C1_ERR_PECERR,
    I2C1_ERR.I2C1_ERR_TIMEOUT,
    I2C1_ERR.I2C1_ERR_SMBALERT};

int i2c1_init(void)
{
  /* register interrupt handlers */
  RET_FAIL(IRQ_SetHandler(I2C1_EV_IRQn, i2c1_ev_irq_handler));
  RET_FAIL(IRQ_SetPriority(I2C1_EV_IRQn, I2C1_EV_IRQ_PRIORITY));
  RET_FAIL(IRQ_Enable(I2C1_EV_IRQn));
  RET_FAIL(IRQ_SetHandler(I2C1_ER_IRQn, i2c1_er_irq_handler));
  RET_FAIL(IRQ_SetPriority(I2C1_ER_IRQn, I2C1_ER_IRQ_PRIORITY));
  RET_FAIL(IRQ_Enable(I2C1_ER_IRQn));

  /* Enable the I2C1 clock */
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  /* USING PB8 and PB9, so enable port B clock if not already */
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

  /* Setup the GPIO pins */
  i2c1_scl.port = I2C_SCL_PORT;
  i2c1_scl.pin = I2C_SCL_PIN;
  i2c1_scl.cfg = OUT_ALT_OPENDRAIN;
  i2c1_scl.dir = OUTPUT_10MHZ;
  i2c1_scl.pull = PULLUP;
  RET_FAIL(stmgpio_setup_gpio(&i2c1_scl));

  i2c1_sda.port = I2C_SDA_PORT;
  i2c1_sda.pin = I2C_SDA_PIN;
  i2c1_sda.cfg = OUT_ALT_OPENDRAIN;
  i2c1_sda.dir = OUTPUT_10MHZ;
  i2c1_sda.pull = PULLUP;
  RET_FAIL(stmgpio_setup_gpio(&i2c1_sda));

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

  /* hooray */
  dbg_log(DBG_TYPE_SUCCESS, DBG_CODE_INIT, DBG_LIB_NAME, sizeof(DBG_LIB_NAME));
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
  /* enable peripheral */
  I2C1->CR1 |= I2C_CR1_PE;
  /* enable ack on byte rcv */
  I2C1->CR1 |= I2C_CR1_ACK;
}

void i2c1_disable_periph(void)
{
  /* disable peripheral */
  I2C1->CR1 &= ~I2C_CR1_PE;
}

void i2c1_start(void)
{
  /* trigger start bit to be sent */
  I2C1->CR1 |= I2C_CR1_START;
}

int i2c1_recv(uint8_t *data)
{
  /* If data reg not empty */
  if (!(I2C1->SR1 & I2C_SR1_RXNE))
  {
    *data = I2C1->DR;
    return EXIT_SUCCESS;
  }
  else
  {
    return EXIT_FAILURE;
  }
}

int i2c1_send(uint8_t *data)
{
  /* send if data reg empty */
  if (!(I2C1->SR1 & I2C_SR1_TXE))
  {
    I2C1->DR = *data;
    return EXIT_SUCCESS;
  }
  else
  {
    return EXIT_FAILURE;
  }
}

int i2c1_set_evt_callback(void (*func_ptr)(void))
{
  if (NULL != func_ptr)
  {
    i2c1_evt_callback = func_ptr;
    return EXIT_SUCCESS;
  }
  else
  {
    return EXIT_FAILURE;
  }
}

int i2c1_set_err_callback(void (*func_ptr)(void))
{
  if (NULL != func_ptr)
  {
    i2c1_err_callback = func_ptr;
    return EXIT_SUCCESS;
  }
  else
  {
    return EXIT_FAILURE;
  }
}

I2C1_EVT i2c1_get_last_event(void)
{
  return last_event;
}

I2C1_ERR i2c1_get_last_error(void)
{
  return last_error;
}

/* I2C1 event handler */
void i2c1_ev_irq_handler(void)
{
  /* Check flags, log event */
  for (idx = 0; idx < sizeof(EVT_LIST); idx++)
  {
    if (I2C1->SR1 & ((uint32_t)(EVT_LIST[idx])))
    {
      last_event = EVT_LIST[idx];
    }
  }

  /* call a registered callback */
  if (NULL != i2c1_evt_callback)
  {
    i2c1_evt_callback();
  }

  /* clear flag, interrupt over */
  IRQ_ClearPending(I2C1_EV_IRQn);
  IRQ_EndOfInterrupt(I2C1_EV_IRQn);
}

/* I2C1 error handler */
void i2c1_er_irq_handler(void)
{
  /* Check flags, log error */
  for (idx = 0; idx < sizeof(ERR_LIST); idx++)
  {
    if (I2C1->SR1 & ((uint32_t)(ERR_LIST[idx])))
    {
      last_error = ERR_LIST[idx];
    }
  }

  /* call a registered callback */
  if (NULL != i2c1_err_callback)
  {
    i2c1_err_callback();
  }

  /* clear flag, interrupt over */
  IRQ_ClearPending(I2C1_ER_IRQn);
  IRQ_EndOfInterrupt(I2C1_ER_IRQn);
}