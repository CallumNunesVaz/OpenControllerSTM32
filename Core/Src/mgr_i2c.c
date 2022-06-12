/* Includes ------------------------------------------------------------------*/
#include "mgr_i2c.h"

/* buffer data storage and buffer handlers */
static buffer_t i2c_cmd_buf;
static i2c_msg_t i2c_cmd_buf_data[I2C_MSG_BUF_LEN];

static i2c_msg_t msg_cur;

static bool send_immediatly;

/* state transitions */
static void (*current_state)(void);
static state_exec_class_t state_exec;
static trans_t i2c_transitions[] = {
  {i2c_state_idle,  I2C_NEW_DATA,   i2c_state_start},
  {i2c_state_start, I2C_START_FIN,  i2c_state_tx},
  {i2c_state_tx,    I2C_TX_FIN,     i2c_state_tx},
  {i2c_state_start, I2C_RX_FIN,     i2c_state_tx},
  {i2c_state_start, I2C_STOP_FIN,   i2c_state_tx}
};


void i2c_queue_msg(i2c_msg_t *msg){
  /* write message to the buffer */
  buf_write(&i2c_cmd_buf, msg, sizeof(i2c_msg_t));
  /* poll in case we can start this instantly */
  i2c_poll();
}

void i2c_state_transition(void){
  /* perform clean up */
  state_exec = STATE_EXEC_EXIT;
}

int i2c_init(void)
{
  i2c_set_send_immediatly(true);

  //init_trans(&t1, (int)I2C_NEW_DATA, i2c_disable, i2c_disable);
  current_state = i2c_state_idle;
  state_exec = STATE_EXEC_ENTRY;

  /* Initialise i2c driver */
  if (EXIT_FAILURE == i2c1_init()) {
    return EXIT_FAILURE;
  }
  
  /* Initialise buffers */
  if (EXIT_FAILURE == buf_init(&buf_tx, buf_tx_data, sizeof(buf_tx_data))) {
    return EXIT_FAILURE;
  }
  if (EXIT_FAILURE == buf_init(&buf_rx, buf_rx_data, sizeof(buf_rx_data))) {
    return EXIT_FAILURE;
  }

  /* Trigger reset before first usage */
  i2c_reset();

  return EXIT_SUCCESS;
}

void i2c_reset(void)
{
  /* reset hardware perihperal */
  i2c1_reset();

  /* reset buffers */
  buf_reset(&buf_tx);
  buf_reset(&buf_rx);
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
void i2c_poll(void)
{
  /* Do something if buffer not empty or already doing something */
  if ((!buf_is_empty(&i2c_cmd_buf)) || (i2c_state_idle != current_state)){

  }
}

void i2c_state_idle(void){
  switch (class)
  {
  case STATE_EXEC_ENTRY:
    /* code */
    break;
  case STATE_EXEC_RECUR:
    /* code */
    break;
  case STATE_EXEC_EXIT:
    /* code */
    break;
  default:
    break;
  }
}

void i2c_state_start(void){
}

void i2c_state_tx(void){
}

void i2c_state_rx(void){
}

void i2c_state_stop(void){
}

void i2c_state_error(void){
}