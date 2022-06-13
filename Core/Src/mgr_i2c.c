/* Includes ------------------------------------------------------------------*/
#include "mgr_i2c.h"

/* buffer data storage and buffer handlers */
static buffer_t msg_buf;
static i2c_msg_t i2c_msg_buf_data[I2C_MSG_BUF_LEN];

/* storage for working message */
static i2c_msg_t msg_cur;
static i2c_event_t last_evt;

/* state transitions */
static void (*cur_state)(void);
static trans_t trans_tab[] = {
    {i2c_state_idle, I2C_NEW_DATA, i2c_state_start_bit},
    {i2c_state_start_bit, I2C_START_FIN, i2c_state_tx_byte},
    {i2c_state_tx_byte, I2C_TX_FIN, i2c_state_tx_byte},
    {i2c_state_start_bit, I2C_RX_FIN, i2c_state_tx_byte},
    {i2c_state_start_bit, I2C_STOP_FIN, i2c_state_tx_byte}};

/* message queue */
void i2c_queue_msg(i2c_msg_t *msg)
{
  /* write message to the buffer */
  buf_write(&msg_buf, msg, sizeof(i2c_msg_t));
  /* poll in case we can start this instantly */
  i2c_poll_fsm();
}

void i2c_state_transition(void)
{
  static uint8_t idx;

  /* Find state transition action */
  for (idx = 0; idx < sizeof(trans_tab); idx++)
  {
    if ((cur_state == trans_tab[idx].func_ptr_cur) && (trans_tab[idx].evt == last_evt))
    {
      cur_state = trans_tab[idx].func_ptr_new;
    }
  }
  /* If no find, then oh no! */
  if (sizeof(trans_tab) <= idx)
  {
    cur_state = i2c_state_error;
  }
}

int i2c_init(void)
{
  // init_trans(&t1, (int)I2C_NEW_DATA, i2c_disable, i2c_disable);
  cur_state = i2c_state_idle;

  /* Initialise i2c driver */
  RET_ON_FAIL(i2c1_init())

  /* Initialise buffer */
  RET_ON_FAIL(buf_init(&msg_buf, i2c_msg_buf_data, sizeof(i2c_msg_buf_data)))

  /* Trigger reset before first usage */
  i2c_reset();

  return EXIT_SUCCESS;
}

void i2c_reset(void)
{
  /* reset hardware perihperal */
  i2c1_reset();

  /* reset buffers */
  buf_reset(&msg_buf);
}

void i2c_enable(void)
{
  /* Enable hardware perihperal */
  i2c1_enable();
}

void i2c_disable(void)
{
  /* Disable hardware perihperal */
  i2c1_disable();
}

uint8_t i2c_read(void)
{
}

void i2c_write(uint8_t data)
{
}

/* Come back around to execute pending tasks */
void i2c_poll_fsm(void)
{
  cur_state();
}

void i2c_state_idle(void)
{
  /* Do something if buffer not empty */
  if (!buf_is_empty(&msg_buf))
  {
    last_evt = I2C_NEW_DATA;
    buf_read(&msg_buf, &msg_cur, sizeof(i2c_msg_t));
  }

  /* Transition to next state */
  i2c_state_transition();
}

void i2c_state_start_bit(void)
{
  /* trigger start bit */
  i2c1_start();

  /* if start bit has finished */
  if (false) {
    last_evt = I2C_START_FIN;
    /* Transition to next state */
    i2c_state_transition();
  }
}

void i2c_state_tx_byte(void)
{
  while (0 < msg_cur.n_send) {
    msg_cur.n_send--;
  }
}

void i2c_state_rx_byte(void)
{
}

void i2c_state_stop_bit(void)
{
}

void i2c_state_error(void)
{
  while(1);
}