/* Includes ------------------------------------------------------------------*/
#include "mgr_i2c.h"

/* buffer data storage and buffer handlers */
static buffer_t msg_buf;
static i2c_msg_t i2c_msg_buf_data[I2C_MSG_BUF_LEN];

/* storage for working message */
static i2c_msg_t msg_cur;
static bytes_sent;
static bytes_recv;

/* state transitions */
static I2C1_EVT last_evt;
bool state_init;
static void (*cur_state)(void);
static trans_t trans_tab[] = {
    {i2c_state_idle, I2C_NEW_DATA, i2c_state_start_bit},
    {i2c_state_start_bit, I2C1_EVT_SB, i2c_state_tx_addr},
    {i2c_state_tx_addr, I2C1_EVT_ADDR, i2c_state_tx_data},
    {i2c_state_tx_data, I2C1_EVT_BTF, i2c_state_rx_data},
    {i2c_state_rx_data, I2C1_EVT_RxNE, i2c_state_stop_bit},
    {i2c_state_stop_bit, I2C1_EVT_, i2c_state_idle}};

/* A device driver is queuing a message */
int i2c_queue_msg(i2c_msg_t *msg)
{
  /* checks */
  if ((NULL == msg->buf_send) || (NULL == msg->buf_recv) || (MSG_SEND_MIN > msg->n_send) || (MSG_RECV_MIN > msg->n_recv))
  {
    return EXIT_FAILURE;
  }

  /* write message to the buffer */
  if (EXIT_FAILURE == buf_write(&msg_buf, msg, sizeof(i2c_msg_t)))
  {
    return EXIT_FAILURE;
  }

  /* poll in case we can start this instantly */
  i2c_poll_fsm();

  return EXIT_SUCCESS;
}

int i2c_init(void)
{
  // init_trans(&t1, (int)I2C_NEW_DATA, i2c_disable, i2c_disable);
  cur_state = i2c_state_idle;
  state_init = true;

  /* Initialise i2c driver */
  RET_FAIL(i2c1_init())

  /* Register our callbacks with the driver */
  i2c1_set_evt_callback(i2c_event_callback);
  i2c1_set_err_callback(i2c_error_callback);

  /* Initialise buffer */
  RET_FAIL(buf_init(&msg_buf, i2c_msg_buf_data, sizeof(i2c_msg_buf_data)))

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
    bytes_sent = 0;
    bytes_recv = 0;
    buf_read(&msg_buf, &msg_cur, sizeof(i2c_msg_t));
    i2c_state_transition();
  }  
}

void i2c_state_start_bit(void)
{
  if (state_init) {
    i2c1_start();
    state_init = false;
  } else {
    if (I2C1_EVT_SB == i2c1_get_last_event()) {
      i2c_state_transition();
    }
  }
}

void i2c_state_tx_data(void)
{
  if (state_init) {
    i2c1_send(&(msg_cur.buf_send[bytes_sent]));
    state_init = false;
  } else {
    if (I2C1_EVT_ADDR == last_evt) {
        bytes_sent++;
        i2c_state_transition();
    }
    if (I2C1_EVT_BTF == last_evt) {
      bytes_sent++; 
      if (bytes_sent < msg_cur.n_send) {
        state_init = false;
      } else {
        i2c_state_transition();
      }
    }
  }
}

void i2c_state_rx_data(void)
{
}

void i2c_state_stop_bit(void)
{
}

void i2c_state_error(void)
{
  
}

void i2c_state_transition(void)
{
  static uint8_t idx;

  /* reset state entry condition */
  state_init = true;

  /* Find state transition action */
  for (idx = 0; idx < sizeof(trans_tab); idx++)
  {
    if ((cur_state == trans_tab[idx].func_ptr_cur) && (trans_tab[idx].evt == last_evt))
    {
      cur_state = trans_tab[idx].func_ptr_new;
    }
  }

  /* If not found, must be an error */
  if (sizeof(trans_tab) <= idx)
  {
    cur_state = i2c_state_error;
  }

  /* Call new state function */
  cur_state();
}

void i2c_event_callback()
{
  last_evt = i2c1_get_last_event();
  cur_state();
}

void i2c_error_callback()
{
  i2c_state_error();
}