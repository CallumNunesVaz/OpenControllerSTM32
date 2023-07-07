#include "drv_gpioexp.h"

static uint8_t test_msg[] = {PCF8574_ADDR | I2C_WRITE_BIT, 0xFF};

static uint8_t buf_tx_data[GPIOEXP_I2C_TX_BUF_LEN];
static uint8_t buf_rx_data[GPIOEXP_I2C_RX_BUF_LEN];

void gpioexp_init(void){
    /* initialise buffers */
    gpioexp_set_pin(4);
}

void gpioexp_set_pin(uint8_t pin) { /*
    i2c_msg_t msg;

    /* 
    if (0x07 < pin) {
        return;
    }

    /* construct message 
    msg.n_send = 2;
    msg.n_recv = 0;
    msg.buf_send_ptr = buf_tx_data;
    msg.buf_send_ptr = buf_rx_data;

    /* write tx buffer 
    buf_tx_data[0] = PCF8574_ADDR | I2C_WRITE_BIT;
    buf_tx_data[1] = 0x01 << pin;

    i2c_queue_msg(&msg); */
}

void gpioexp_get_pin(uint8_t pin) {

}

