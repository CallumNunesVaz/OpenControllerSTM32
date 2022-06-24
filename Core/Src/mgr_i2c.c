/* Includes ------------------------------------------------------------------*/
#include "mgr_i2c.h"

/* buffer data storage and buffer handlers */
static buffer_t msg_buf;
static i2c_msg_t msg_buf_data[I2C_MSG_BUF_LEN];

/* storage for working message */
static i2c_msg_t msg_cur;
static uint8_t bytes_to_send;
static uint8_t bytes_to_recv;

/* state transitions */
static I2C_EVENT last_evt;
bool state_executed;
bool last_evt_processed;
static void (*cur_state)(void);
static trans_t trans_tab[] = {
    {i2c_state_idle, I2C_NEW_DATA, i2c_state_start_bit},
    {i2c_state_start_bit, I2C_SB_SENT, i2c_state_tx_data},
    {i2c_state_tx_data, I2C_BYTE_SENT, i2c_state_tx_data},
    {i2c_state_tx_data, I2C_LAST_BYTE_SENT_R, i2c_state_start_bit},
    {i2c_state_start_bit, I2C_RPT_SB_SENT, i2c_state_rx_data},
    {i2c_state_tx_data, I2C_LAST_BYTE_SENT_W, i2c_state_stop_bit},
    {i2c_state_rx_data, I2C_BYTE_RECV, i2c_state_rx_data},
    {i2c_state_rx_data, I2C_LAST_BYTE_RECV, i2c_state_stop_bit},
    {i2c_state_stop_bit, I2C_PB_SENT, i2c_state_idle}};

/* /*/
I2C_EVENT i2c_decode_i2c1_event(I2C1_EVT i2c1_evt)
{
  I2C_EVENT evt = I2C_ERROR;

  switch (i2c1_evt)
  {
  case I2C1_EVT_SB:
    if (bytes_to_send > 0)
    {
      evt = I2C_SB_SENT;
    }
    else
    {
      evt = I2C_RPT_SB_SENT;
    }
    break;
  case I2C1_EVT_ADDR:
    evt = I2C_BYTE_SENT;
    break;
  case I2C1_EVT_ADD10:
    evt = I2C_BYTE_SENT;
    break;
  /* ignore BTF. No worth in this machine */
  case I2C1_EVT_BTF:
    break;
  case I2C1_EVT_TxE:
    /* if we've sent all the bytes */
    if (bytes_to_send)
    {
      /* if its a write operation */
      if (bytes_to_recv)
      {
        evt = I2C_LAST_BYTE_SENT_W;
      }
      /* else a read operation */
      else
      {
        evt = I2C_LAST_BYTE_SENT_R;
      }
    }
    /* else we need to send more bytes */
    else
    {
      evt = I2C_BYTE_SENT;
    }
    break;
  case I2C1_EVT_RxNE:
    /* if we've recv'd all the bytes */
    if (bytes_to_recv)
    {
      evt = I2C_LAST_BYTE_RECV;
    }
    /* else we need to send more bytes */
    else
    {
      evt = I2C_BYTE_RECV;
    }
    break;
  default:
    evt = I2C_ERROR;
    break;
  }

  return evt;
}

/* A device driver is queuing a message */
int i2c_queue_msg(i2c_msg_t *msg)
{
  /* checks */
  ASSERT((NULL != msg->buf_send_ptr) && (NULL != msg->buf_recv_ptr) && (MSG_SEND_MIN <= msg->n_send) && (MSG_RECV_MIN <= msg->n_recv));

  /* write message to the buffer */
  ASSERT(buf_write(&msg_buf, msg, sizeof(i2c_msg_t)));

  /* poll in case we can start this instantly */
  i2c_poll_fsm();

  return EXIT_SUCCESS;
}

int i2c_init(void)
{
  // init_trans(&t1, (int)I2C_NEW_DATA, i2c_disable, i2c_disable);
  cur_state = i2c_state_idle;

  /* Initialise i2c driver */
  ASSERT(i2c1_init());

  /* Register our callbacks with the driver */
  i2c1_set_evt_callback(i2c_event_callback);
  i2c1_set_err_callback(i2c_error_callback);

  /* Initialise buffer */
  ASSERT(buf_init(&msg_buf, msg_buf_data, sizeof(msg_buf_data)));

  /* Trigger reset before first usage */
  i2c_reset();
  i2c_enable();

  return EXIT_SUCCESS;
}

void i2c_reset(void)
{
  /* reset hardware perihperal */
  i2c1_reset_periph();

  /* reset buffers */
  buf_reset(&msg_buf);

  /*reset flags*/
  state_executed = false;
}

void i2c_enable(void)
{
  /* Enable hardware perihperal */
  i2c1_enable_periph();
}

void i2c_disable(void)
{
  /* Disable hardware perihperal */
  i2c1_disable_periph();
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
    /* init for new data */
    buf_read(&msg_buf, &msg_cur, sizeof(i2c_msg_t));
    bytes_to_send = msg_cur.n_send;
    bytes_to_recv = msg_cur.n_recv;

    /* Configure state machine */
    last_evt = I2C_NEW_DATA;
    i2c_trig_state_trans();
  }
}

void i2c_state_start_bit(void)
{
  if (!state_executed)
  {
    /* trigger start bit sending */
    i2c1_start();
    state_executed = true;
  }
}

void i2c_state_tx_data(void)
{
  if (!state_executed)
  {
    /* Write new data to send */
    if (EXIT_SUCCESS == i2c1_send(&(msg_cur.buf_send_ptr[bytes_to_send])))
    {
      bytes_to_send--;
    }
    else
    {
      last_evt = I2C_ERROR;
      i2c_trig_state_trans();
    }
    state_executed = true;
  }
}

void i2c_state_rx_data(void)
{
  if (!state_executed)
  {
    if (EXIT_SUCCESS == i2c1_recv(&(msg_cur.buf_recv_ptr)[msg_cur.n_recv - bytes_to_recv]))
    {
      bytes_to_recv--;
      if (bytes_to_recv)
      {
        i2c1_nack_bit();
      }
      else
      {
        i2c1_ack_bit();
      }
    }
    else
    {
      last_evt = I2C_ERROR;
      i2c_trig_state_trans();
    }
    state_executed = true;
  }
}

void i2c_state_stop_bit(void)
{
  i2c1_stop();
}

void i2c_state_error(void)
{
  while (1)
    ;
}

void i2c_trig_state_trans(void)
{
  static uint8_t idx;

  /* reset state entry condition */
  state_executed = false;

  /* Find state transition action */
  for (idx = 0; idx < sizeof(trans_tab); idx++)
  {
    if ((cur_state == trans_tab[idx].func_ptr_cur) && (trans_tab[idx].evt == last_evt))
    {
      cur_state = trans_tab[idx].func_ptr_new;
    }
  }

  /* If not found, must be an error */
  if ((sizeof(trans_tab) <= idx) || (I2C_ERROR == last_evt))
  {
    cur_state = i2c_state_error;
  }

  /* Call new state function */
  // cur_state();
}

void i2c_event_callback()
{
  /* Figure out what happened */
  last_evt = i2c_decode_i2c1_event(i2c1_get_last_event());

  /* Transition to new state based on event */
  i2c_trig_state_trans();

  /* Run state actions */
  cur_state();

  /* humour i2c peripheral */
  // i2c1_SR1_dummy_read();
  // i2c1_SR2_dummy_read();
}

void i2c_error_callback()
{
  i2c_state_error();
}