/* Includes ------------------------------------------------------------------*/
#include "hw_i2c.h"

/* interrupt vars */
static volatile uint8_t idx;
static volatile I2C_EVT last_event;
static volatile I2C_ERR last_error;

/* I2C Base Address */
static I2C_TypeDef *i2cx;

/* interrupt callbacks */
static void (*i2c_evt_callback)(void);
static void (*i2c_err_callback)(void);

/* */
I2C_EVT EVT_LIST[] = {
    I2C_EVT_SB,
    I2C_EVT_ADDR,
    I2C_EVT_ADD10,
    I2C_EVT_BTF,
    I2C_EVT_TxE,
    I2C_EVT_RxNE};

/* */
I2C_ERR ERR_LIST[] = {
    I2C_ERR_BERR,
    I2C_ERR_ARLO,
    I2C_ERR_AF,
    I2C_ERR_OVR,
    I2C_ERR_PECERR,
    I2C_ERR_TIMEOUT,
    I2C_ERR_SMBALERT};

int i2c_init(I2C_PERIPH periph, I2C_SPD spd, bool remap)
{
  /* GPIO handlers */
  stmgpio_t pin_i2c_scl, pin_i2c_sda;
  uint32_t temp;

  /* Apply periph dependent settings */
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    i2cx = I2C1;
    /* register interrupt handlers */
    NVIC_SetPriority(I2C1_EV_IRQn, I2C_EV_IRQ_PRIORITY);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_SetPriority(I2C1_ER_IRQn, I2C_ER_IRQ_PRIORITY);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // Enable the I2C clock
    if (remap)
    { // Set the pin remap for i2c1 if requested
      AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
      /* Setup the GPIO pins */
      pin_i2c_scl.port = I2C1_SCL_PORT_REMAP;
      pin_i2c_scl.pin = I2C1_SCL_PIN_REMAP;
      pin_i2c_sda.port = I2C1_SDA_PORT_REMAP;
      pin_i2c_sda.pin = I2C1_SDA_PIN_REMAP;
    }
    else
    {
      AFIO->MAPR &= ~AFIO_MAPR_I2C1_REMAP;
      /* Setup the GPIO pins */
      pin_i2c_scl.port = I2C1_SCL_PORT_DEFAULT;
      pin_i2c_scl.pin = I2C1_SCL_PIN_DEFAULT;
      pin_i2c_sda.port = I2C1_SDA_PORT_DEFAULT;
      pin_i2c_sda.pin = I2C1_SDA_PIN_DEFAULT;
    }
    break;
  case I2C_PERIPH_I2C2:
    i2cx = I2C2;
    /* register interrupt handlers */
    NVIC_SetPriority(I2C2_EV_IRQn, I2C_EV_IRQ_PRIORITY);
    NVIC_EnableIRQ(I2C2_EV_IRQn);
    NVIC_SetPriority(I2C2_ER_IRQn, I2C_ER_IRQ_PRIORITY);
    NVIC_EnableIRQ(I2C2_ER_IRQn);
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN; // Enable the I2C clock
    /* Setup the GPIO pins */
    pin_i2c_scl.port = I2C2_SCL_PORT_DEFAULT;
    pin_i2c_scl.pin = I2C2_SCL_PIN_DEFAULT;
    pin_i2c_sda.port = I2C2_SDA_PORT_DEFAULT;
    pin_i2c_sda.pin = I2C2_SDA_PIN_DEFAULT;
    break;
  default:
    return EXIT_FAILURE;
    break;
  }

  /* Disable i2c peripheral to apply settings */
  i2cx->CR1 &= ~I2C_CR1_PE;

  /* RM0008 Rev21 p.758 Step 1/5: Prog input periph clock I2Cx_CR2 */
  i2cx->CR2 &= ~I2C_CR2_DMAEN;   // Disable DMA
  i2cx->CR2 &= ~I2C_CR2_ITBUFEN; // Disable TX buffer interrupt
  i2cx->CR2 |= I2C_CR2_ITEVTEN;  // Enable event interrupt
  i2cx->CR2 |= I2C_CR2_ITERREN;  // Enable error interrupt
  i2cx->CR2 &= ~I2C_CR2_FREQ;    // Clear then Set APB bus frequency
  switch (RCC->CFGR & RCC_CFGR_PPRE2)
  {
  case RCC_CFGR_PPRE2_DIV16:
    i2cx->CR2 |= (uint32_t)(SystemCoreClock / 1000000) >> 4;
    break;
  case RCC_CFGR_PPRE2_DIV8:
    i2cx->CR2 |= (uint32_t)(SystemCoreClock / 1000000) >> 3;
    break;
  case RCC_CFGR_PPRE2_DIV4:
    i2cx->CR2 |= (uint32_t)(SystemCoreClock / 1000000) >> 2;
    break;
  case RCC_CFGR_PPRE2_DIV2:
    i2cx->CR2 |= (uint32_t)(SystemCoreClock / 1000000) >> 1;
    break;
  default:
    i2cx->CR2 |= (uint32_t)(SystemCoreClock / 1000000);
    break;
  }

  /* RM0008 Rev21 p.758 Step 2/5: Configure clock control regs RCC / CCR */
  /* RM0008 Rev21 p.758 Step 3/5: Configure rise time reg TRISE */
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; // Enable port B clock and alt func clock
  i2cx->TRISE &= ~I2C_TRISE_TRISE;
  switch (spd)
  {
  case I2C_SPD_STD:
    i2cx->CCR &= ~I2C_CCR_FS;  // Set to standard mode
    i2cx->CCR = (uint32_t)(5.0 * ((float)(i2cx->CR2 | I2C_CR2_FREQ))); // Set clock for 100kHz
    i2cx->TRISE |= 0x25;
    //i2cx->TRISE |= ((uint32_t)(i2cx->CR2 | I2C_CR2_FREQ)) + 1; // Set rise time
    i2cx->CCR = i2cx->CCR;
    break;
  case I2C_SPD_FAST:
    i2cx->CCR |= I2C_CCR_FS;  // Set to fast mode
    i2cx->CCR &= ~I2C_CCR_DUTY;  // Set low period double that of high
    if (i2cx->CCR | I2C_CCR_DUTY) {
      i2cx->CCR = (uint32_t)(5.0 / 6.0 * ((float)(i2cx->CR2 | I2C_CR2_FREQ))); // Set clock for 100kHz
    } else {
      i2cx->CCR = (uint32_t)(1.0 / 10.0 * ((float)(i2cx->CR2 | I2C_CR2_FREQ))); // Set clock for 100kHz
    }
    i2cx->TRISE = ((uint32_t)(0.3*((float)(i2cx->CR2 | I2C_CR2_FREQ)))) + 1; // Set rise time
    break;
  default:
    return EXIT_FAILURE;
    break;
  }

  /* RM0008 Rev21 p.758 Step 4/5: prog I2Cx_CR1 */
  while ((i2cx->CR1 & I2C_CR1_START) || ((i2cx->CR1 & I2C_CR1_STOP)))
    ;                              // blocking wait for start/stop before writing CR1
  i2cx->CR1 &= ~I2C_CR1_NOSTRETCH; // Enable clock stretching
  i2cx->CR1 &= ~I2C_CR1_ENGC;      // Disable general call
  i2cx->CR1 &= ~I2C_CR1_SMBUS;     // Disable SMBUS. Using I2C

  /* finish GPIO setup */
  pin_i2c_scl.cfg = OUT_ALT_OPENDRAIN;
  pin_i2c_scl.dir = OUTPUT_50MHz;
  pin_i2c_scl.pull = PULLUP;
  ASSERT_INT(stmgpio_setup(&pin_i2c_scl));
  pin_i2c_sda.cfg = OUT_ALT_OPENDRAIN;
  pin_i2c_sda.dir = OUTPUT_50MHz;
  pin_i2c_sda.pull = PULLUP;
  ASSERT_INT(stmgpio_setup(&pin_i2c_sda));

  /* Set slave address (to humour the silicon) */
  i2cx->OAR1 = 0x32;
  i2cx->OAR2 = 0x32;

  /* enable i2c peripheral to apply settings */
  i2cx->CR1 |= I2C_CR1_PE;

  /* Trigger reset before first usage */
  // i2cx_reset_periph();

  /* hooray */
  // dbg_log(DBG_TYPE_SUCCESS, DBG_CODE_INIT, DBG_LIB_NAME, sizeof(DBG_LIB_NAME));
  return EXIT_SUCCESS;
}

void i2c_reset_periph(void)
{
  i2cx->CR1 |= I2C_CR1_SWRST;
  // make sure the I2C lines are released and the bus is free
  i2cx->CR1 &= ~I2C_CR1_SWRST;
}

void i2c_enable_periph(void)
{
  /* enable peripheral */
  i2cx->CR1 |= I2C_CR1_PE;
  /* enable ack on byte rcv */
  i2c_ack_bit();
}

void i2c_ack_bit(void)
{
  /* enable ack on byte rcv */
  i2cx->CR1 |= I2C_CR1_ACK;
}

void i2c_nack_bit(void)
{
  /* enable ack on byte rcv */
  i2cx->CR1 &= ~I2C_CR1_ACK;
}

void i2c_disable_periph(void)
{
  /* disable peripheral */
  i2cx->CR1 &= ~I2C_CR1_PE;
}

void i2c_start(void)
{
  /* trigger start bit to be sent */
  i2cx->CR1 |= I2C_CR1_START;

  /* Workaround: Errata 2.8.7 can't start due to analogue filters
  if (i2cx->SR2 & I2C_SR2_BUSY)
  {
    // Step 1
    i2c_disable_periph();
    // Step 2
    // i2cx_scl.cfg = OUT_OPENDRAIN;
    // i2cx_sda.cfg = OUT_OPENDRAIN;
    // ASSERT_INT(stmgpio_setup(&i2cx_scl));
    // ASSERT_INT(stmgpio_setup(&i2cx_sda));
    // stmgpio_write(&i2cx_scl, PIN_HIGH);
    // stmgpio_write(&i2cx_sda, PIN_HIGH);
    GPIOB->CRH &= 0xFFF0;
    GPIOB->CRH |= 0x0005;
    GPIOB->CRH |= (0x0005 << 4);
    //GPIOB->ODR |= I2C_SCL_PIN;
    //GPIOB->ODR |= I2C_SDA_PIN;
    // Step 3
    // while(PIN_LOW == stmgpio_read(&i2cx_scl));
    // while(PIN_LOW == stmgpio_read(&i2cx_sda));
    while ((GPIOB->IDR & I2C_SCL_PIN))
      ;
    while ((GPIOB->IDR & I2C_SDA_PIN))
      ;
    // Step 4
    // stmgpio_write(&i2cx_sda, PIN_LOW);
    GPIOB->ODR &= ~I2C_SDA_PIN;
    // Step 5
    // while(PIN_HIGH == stmgpio_read(&i2cx_sda));
    while (!(GPIOB->IDR & I2C_SDA_PIN))
      ;
    // Step 6
    // stmgpio_write(&i2cx_scl, PIN_LOW);
    GPIOB->ODR &= ~I2C_SCL_PIN;
    // Step 7
    // while(PIN_HIGH == stmgpio_read(&i2cx_scl));
    while (!(GPIOB->IDR & I2C_SCL_PIN))
      ;
    // Step 8
    // stmgpio_write(&i2cx_scl, PIN_HIGH);
    GPIOB->ODR |= I2C_SCL_PIN;
    // Step 9
    // while(PIN_LOW == stmgpio_read(&i2cx_scl));
    while ((GPIOB->IDR & I2C_SCL_PIN))
      ;
    // Step 10
    // stmgpio_write(&i2cx_sda, PIN_HIGH);
    GPIOB->ODR |= I2C_SDA_PIN;
    // Step 11
    // while(PIN_LOW == stmgpio_read(&i2cx_sda));
    while (!(GPIOB->IDR & I2C_SDA_PIN))
      ;
    // Step 12
    // i2cx_scl.cfg = OUT_ALT_OPENDRAIN;
    // i2cx_sda.cfg = OUT_ALT_OPENDRAIN;
    // ASSERT_INT(stmgpio_setup(&i2cx_scl));
    // ASSERT_INT(stmgpio_setup(&i2cx_sda));
    GPIOB->CRH &= 0xFFF0;
    GPIOB->CRH |= 0x000D;
    GPIOB->CRH |= (0x000D << 4);
    // Step 13
    i2cx->CR1 |= I2C_CR1_SWRST;
    // Step 14
    i2cx->CR1 &= ~I2C_CR1_SWRST;
    // Step 15
    i2c_enable_periph();

  }*/
}

void i2c_stop(void)
{
  /* trigger start bit to be sent */
  i2cx->CR1 |= I2C_CR1_STOP;
}

int i2c_recv(uint8_t *data)
{
  /* If data reg not empty */
  ASSERT_BOOL(!(i2cx->SR1 & I2C_SR1_RXNE));
  *data = i2cx->DR;
  return EXIT_SUCCESS;
}

int i2c_send(uint8_t *data)
{
  /* send if data reg empty */
  ASSERT_BOOL(!(i2cx->SR1 & I2C_SR1_TXE));
  i2cx->DR = *data;
  return EXIT_SUCCESS;
}

int i2c_set_evt_callback(void (*func_ptr)(void))
{
  ASSERT_BOOL(NULL != func_ptr);
  i2c_evt_callback = func_ptr;
  return EXIT_SUCCESS;
}

int i2c_set_err_callback(void (*func_ptr)(void))
{
  ASSERT_BOOL(NULL != func_ptr);
  i2c_err_callback = func_ptr;
  return EXIT_SUCCESS;
}

uint16_t i2c_SR1_dummy_read()
{
  return i2cx->SR1;
}

uint16_t i2c_SR2_dummy_read()
{
  return i2cx->SR2;
}

I2C_EVT i2c_get_last_event(void)
{
  return last_event;
}

I2C_ERR i2c_get_last_error(void)
{
  return last_error;
}

/* i2cx event handler */
void i2cx_EV_IRQHandler(void)
{
  /* Check flags, log event */
  for (idx = 0; idx < sizeof(EVT_LIST); idx++)
  {
    if (i2cx->SR1 & ((uint32_t)(EVT_LIST[idx])))
    {
      last_event = EVT_LIST[idx];
    }
  }

  /* call a registered callback */
  if (NULL != i2c_evt_callback)
  {
    i2c_evt_callback();
  }

  /* clear flag, interrupt over */
  // NVIC_ClearPendingIRQ(i2cx_EV_IRQn);
}

/* i2cx error handler */
void i2cx_ER_IRQHandler(void)
{
  /* Check flags, log error */
  for (idx = 0; idx < sizeof(ERR_LIST); idx++)
  {
    if (i2cx->SR1 & ((uint32_t)(ERR_LIST[idx])))
    {
      last_error = ERR_LIST[idx];
    }
  }

  /* call a registered callback */
  if (NULL != i2c_err_callback)
  {
    i2c_err_callback();
  }

  /* clear flag, interrupt over */
  // NVIC_ClearPendingIRQ(i2cx_ER_IRQn);
}