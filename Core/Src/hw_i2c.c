/* Includes ------------------------------------------------------------------*/
#include "hw_i2c.h"

/* interrupt vars */
static volatile uint8_t idx;
static volatile I2C_EVT last_event;
static volatile I2C_ERR last_error;

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

/*
void HAL_I2C_MspInit(I2C_HandleTypeDef *i2cHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if (i2cHandle->Instance == I2C1)
  {
    GPIO_InitStruct.Pin = I2CINT_CLK_Pin | I2CINT_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    __HAL_AFIO_REMAP_I2C1_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
  }
}
*/

int i2c_init(I2C_PERIPH periph, I2C_SPD spd, bool remap)
{
  /* function used multiple times for clock calcs */
  float core_freq_mhz;
  /* GPIO handlers */
  stmgpio_t pin_i2c_scl, pin_i2c_sda;
  /* I2C base address */
  I2C_TypeDef *i2cx;

  /* set peripheral pointer for later use */
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    i2cx = I2C1;
    break;
  case I2C_PERIPH_I2C2:
    i2cx = I2C2;
    break;
  default:
    return EXIT_FAILURE;
    break;
  }

  /* Disable i2c peripheral to apply settings */
  i2cx->CR1 &= ~I2C_CR1_PE;

  /* GPIO setup */
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    if (remap)
    {
      pin_i2c_scl.port = I2C1_SCL_PORT_REMAP;
      pin_i2c_scl.pin = I2C1_SCL_PIN_REMAP;
      pin_i2c_sda.port = I2C1_SDA_PORT_REMAP;
      pin_i2c_sda.pin = I2C1_SDA_PIN_REMAP;
    }
    else
    {
      pin_i2c_scl.port = I2C1_SCL_PORT_DEFAULT;
      pin_i2c_scl.pin = I2C1_SCL_PIN_DEFAULT;
      pin_i2c_sda.port = I2C1_SDA_PORT_DEFAULT;
      pin_i2c_sda.pin = I2C1_SDA_PIN_DEFAULT;
    }
    break;
  case I2C_PERIPH_I2C2:
    pin_i2c_scl.port = I2C2_SCL_PORT_DEFAULT;
    pin_i2c_scl.pin = I2C2_SCL_PIN_DEFAULT;
    pin_i2c_sda.port = I2C2_SDA_PORT_DEFAULT;
    pin_i2c_sda.pin = I2C2_SDA_PIN_DEFAULT;
    break;
  default:
    return EXIT_FAILURE;
    break;
  }
  pin_i2c_scl.cfg = OUT_ALT_OPENDRAIN;
  pin_i2c_scl.dir = OUTPUT_50MHZ;
  pin_i2c_scl.pull = PULLUP;
  ASSERT_INT(stmgpio_setup(&pin_i2c_scl));
  pin_i2c_sda.cfg = OUT_ALT_OPENDRAIN;
  pin_i2c_sda.dir = OUTPUT_50MHZ;
  pin_i2c_sda.pull = PULLUP;
  ASSERT_INT(stmgpio_setup(&pin_i2c_sda));

  /* Remap AFTER GPIO init (buggy silicon) */
  if (I2C_PERIPH_I2C1 == periph)
  {
    if (remap)
    {
      AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
    }
    else
    {
      AFIO->MAPR &= ~AFIO_MAPR_I2C1_REMAP;
    }
  }

  /* Apply periph clocks and interrupts */
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    /* register interrupt handlers */
    NVIC_SetPriority(I2C1_EV_IRQn, I2C_EV_IRQ_PRIORITY);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_SetPriority(I2C1_ER_IRQn, I2C_ER_IRQ_PRIORITY);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;     // Enable the I2C1 clock
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST; // Make sure peripheral not in reset
    break;
  case I2C_PERIPH_I2C2:
    /* register interrupt handlers */
    NVIC_SetPriority(I2C2_EV_IRQn, I2C_EV_IRQ_PRIORITY);
    NVIC_EnableIRQ(I2C2_EV_IRQn);
    NVIC_SetPriority(I2C2_ER_IRQn, I2C_ER_IRQ_PRIORITY);
    NVIC_EnableIRQ(I2C2_ER_IRQn);
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;     // Enable the I2C2 clock
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C2RST; // Make sure peripheral not in reset
    break;
  default:
    return EXIT_FAILURE;
    break;
  }

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
  core_freq_mhz = (float)(i2cx->CR2 | I2C_CR2_FREQ);

  /* RM0008 Rev21 p.758 Step 2/5: Configure clock control regs RCC / CCR */
  /* RM0008 Rev21 p.758 Step 3/5: Configure rise time reg TRISE */
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; // Enable port B clock and alt func clock
  i2cx->TRISE &= ~I2C_TRISE_TRISE;
  switch (spd)
  {
  case I2C_SPD_STD:
    i2cx->CCR &= ~I2C_CCR_FS;                    // Set to standard mode
    i2cx->CCR = (uint32_t)(5.0 * core_freq_mhz); // Set clock for 100kHz
    i2cx->TRISE |= 0x25;
    // i2cx->TRISE |= ((uint32_t)(i2cx->CR2 | I2C_CR2_FREQ)) + 1; // Set rise time
    i2cx->CCR = i2cx->CCR;
    break;
  case I2C_SPD_FAST:
    i2cx->CCR |= I2C_CCR_FS;    // Set to fast mode
    i2cx->CCR &= ~I2C_CCR_DUTY; // Set low period double that of high
    if (i2cx->CCR | I2C_CCR_DUTY)
    {
      i2cx->CCR = (uint32_t)(5.0 / 6.0 * core_freq_mhz); // Set clock for 100kHz
    }
    else
    {
      i2cx->CCR = (uint32_t)(0.1 * core_freq_mhz); // Set clock for 100kHz
    }
    i2cx->TRISE = ((uint32_t)(0.3 * core_freq_mhz)) + 1; // Set rise time
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

  /* Set slave address (to humour the silicon) */
  i2cx->OAR1 = 0x32;
  i2cx->OAR2 = 0x32;

  /* enable */
  i2c_enable_periph(periph);

  /* Trigger reset before first usage (REQUIRED! Silicon <= adequate) */
  //i2c_reset_periph(periph);

  /* hooray */
  // dbg_log(DBG_TYPE_SUCCESS, DBG_CODE_INIT, DBG_LIB_NAME, sizeof(DBG_LIB_NAME));
  return EXIT_SUCCESS;
}

void i2c_reset_periph(I2C_PERIPH periph)
{
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    while (I2C1->SR2 & I2C_SR2_BUSY)
      ;
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    break;
  case I2C_PERIPH_I2C2:
    while (I2C2->SR2 & I2C_SR2_BUSY)
      ;
    I2C2->CR1 |= I2C_CR1_SWRST;
    I2C2->CR1 &= ~I2C_CR1_SWRST;
    break;
  default:
    break;
  }
}

void i2c_enable_periph(I2C_PERIPH periph)
{
  /* enable peripheral */
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    I2C1->CR1 |= I2C_CR1_PE;
    break;
  case I2C_PERIPH_I2C2:
    I2C2->CR1 |= I2C_CR1_PE;
    break;
  default:
    break;
  }
  /* enable ack on byte rcv */
  i2c_ack_bit(periph);
}

void i2c_ack_bit(I2C_PERIPH periph)
{
  /* enable ack on byte rcv */
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    I2C1->CR1 |= I2C_CR1_ACK;
    break;
  case I2C_PERIPH_I2C2:
    I2C2->CR1 |= I2C_CR1_ACK;
    break;
  default:
    break;
  }
}

void i2c_nack_bit(I2C_PERIPH periph)
{
  /* enable ack on byte rcv */
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    I2C1->CR1 &= ~I2C_CR1_ACK;
    break;
  case I2C_PERIPH_I2C2:
    I2C2->CR1 &= ~I2C_CR1_ACK;
    break;
  default:
    break;
  }
}

void i2c_disable_periph(I2C_PERIPH periph)
{
  /* disable peripheral */
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    I2C1->CR1 &= ~I2C_CR1_PE;
    break;
  case I2C_PERIPH_I2C2:
    I2C2->CR1 &= ~I2C_CR1_PE;
    break;
  default:
    break;
  }
}

void i2c_start(I2C_PERIPH periph)
{
  /* I2C base address */
  I2C_TypeDef *i2cx;
  /* GPIO handlers */
  stmgpio_t pin_i2c_scl, pin_i2c_sda;

  /* set peripheral pointer for later use */
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    i2cx = I2C1;
    break;
  case I2C_PERIPH_I2C2:
    i2cx = I2C2;
    break;
  default:
    return EXIT_FAILURE;
    break;
  }

  /* trigger start bit to be sent */
  I2C1->CR1 |= I2C_CR1_START;

  /* Workaround: Errata 2.8.7 can't start due to analogue filters */
  if (!(I2C1->SR2 & I2C_SR2_BUSY))
  {
    // Step 1
    I2C1->CR1 &= ~I2C_CR1_PE;
    // Step 2
    GPIOB->CRH &= 0xFF00;
    GPIOB->CRH |= 0x0077; // General Purpose Output Open-Drain
    GPIOB->ODR |= 0x0003 << 8; // Write high
    // Step 3
    while (!(GPIOB->IDR &= 0x0003 << 8)); // check in IDR
    // Step 4
    GPIOB->CRH |= 0x0070; // General Purpose Output Open-Drain
    GPIOB->ODR &= ~(0x0002 << 8); // Write low
    // Step 5
    while (GPIOB->IDR & (0x0002 << 8));
    // Step 6
    GPIOB->CRH |= 0x0007; // General Purpose Output Open-Drain
    GPIOB->ODR &= ~(0x0001 << 8); // Write low
    // Step 7
    while (GPIOB->IDR & (0x0001 << 8));
    // Step 8
    GPIOB->CRH |= 0x0007; // General Purpose Output Open-Drain
    GPIOB->ODR |= 0x0001 << 8; // Write high
    // Step 9
    while (!(GPIOB->IDR & (0x0001 << 8)));
    // Step 10
    GPIOB->CRH |= 0x0070; // General Purpose Output Open-Drain
    GPIOB->ODR |= 0x0002 << 8; // Write high
    // Step 11
    while (!(GPIOB->IDR & (0x0002 << 8)));
    // Step 12
    GPIOB->CRH |= 0x00FF; // Alt Func Open-Drain
    // Step 13
    I2C1->CR1 |= I2C_CR1_SWRST;
    // Step 14
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    // Step 15
    I2C1->CR1 |= I2C_CR1_PE;
  }
}

void i2c_stop(I2C_PERIPH periph)
{
  /* trigger start bit to be sent */
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    I2C1->CR1 |= I2C_CR1_STOP;
    break;
  case I2C_PERIPH_I2C2:
    I2C2->CR1 |= I2C_CR1_STOP;
    break;
  default:
    break;
  }
}

int i2c_recv(I2C_PERIPH periph, uint8_t *data)
{
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    ASSERT_BOOL(!(I2C1->SR1 & I2C_SR1_RXNE));
    *data = I2C1->DR;
    break;
  case I2C_PERIPH_I2C2:
    ASSERT_BOOL(!(I2C2->SR1 & I2C_SR1_RXNE));
    *data = I2C2->DR;
    break;
  default:
    return EXIT_FAILURE;
    break;
  }
  return EXIT_SUCCESS;
}

int i2c_send(I2C_PERIPH periph, uint8_t *data)
{
  i2c_SR1_dummy_read(periph);
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    ASSERT_BOOL(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = *data;
    break;
  case I2C_PERIPH_I2C2:
    ASSERT_BOOL(!(I2C2->SR1 & I2C_SR1_TXE));
    I2C2->DR = *data;
    break;
  default:
    return EXIT_FAILURE;
    break;
  }
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

uint16_t i2c_SR1_dummy_read(I2C_PERIPH periph)
{
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    return I2C1->SR1;
    break;
  case I2C_PERIPH_I2C2:
    return I2C2->SR1;
    break;
  default:
    return EXIT_FAILURE;
    break;
  }
}

uint16_t i2c_SR2_dummy_read(I2C_PERIPH periph)
{
  switch (periph)
  {
  case I2C_PERIPH_I2C1:
    return I2C1->SR2;
    break;
  case I2C_PERIPH_I2C2:
    return I2C2->SR2;
    break;
  default:
    return EXIT_FAILURE;
    break;
  }
}

I2C_EVT i2c_get_last_event(void)
{
  return last_event;
}

I2C_ERR i2c_get_last_error(void)
{
  return last_error;
}

/* i2cx event handler 
void I2C1_EV_IRQHandler(void)
{
  /* Check flags, log event 
  for (idx = 0; idx < sizeof(EVT_LIST); idx++)
  {
    if (i2cx->SR1 & ((uint32_t)(EVT_LIST[idx])))
    {
      last_event = EVT_LIST[idx];
    }
  }

  /* call a registered callback 
  if (NULL != i2c_evt_callback)
  {
    i2c_evt_callback();
  }

  /* clear flag, interrupt over 
  // NVIC_ClearPendingIRQ(i2cx_EV_IRQn);
}

/* i2cx error handler 
void I2C1_ER_IRQHandler(void)
{
  /* Check flags, log error 
  for (idx = 0; idx < sizeof(ERR_LIST); idx++)
  {
    if (i2cx->SR1 & ((uint32_t)(ERR_LIST[idx])))
    {
      last_error = ERR_LIST[idx];
    }
  }

  /* call a registered callback 
  if (NULL != i2c_err_callback)
  {
    i2c_err_callback();
  }

  /* clear flag, interrupt over 
  // NVIC_ClearPendingIRQ(i2cx_ER_IRQn);
}
*/