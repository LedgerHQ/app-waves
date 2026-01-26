#pragma once

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool

#include "buffer.h"
#include "cx.h"

cx_err_t make_allowed_sign_steps(buffer_t *cdata);

// Read 20 bytes from buffer to path
void read_path_from_bytes(unsigned char *buffer, uint32_t *path);


cx_err_t make_sign_step(uint8_t chunk_data_size, buffer_t *cdata);
cx_err_t hash_stream_data(uint8_t chunk_data_size, buffer_t *cdata); 
int getMessageType(void);
void read_path_from_bytes(unsigned char *buffer, uint32_t *path);
uint32_t deserialize_uint32_t(unsigned char *buffer);
cx_err_t build_other_data_ui(); 
cx_err_t make_allowed_ui_steps(buffer_t *cdata, bool is_last);