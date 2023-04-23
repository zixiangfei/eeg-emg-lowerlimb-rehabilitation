#ifndef EMOKIT_DECODE_H_
#define EMOKIT_DECODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emokit.h"


unsigned char battery_value(unsigned char in);

int get_level(unsigned char frame[32], const unsigned char bits[14]);

struct emokit_contact_quality handle_quality(struct emokit_device* s);

int emokit_get_next_raw(struct emokit_device* s);

struct emokit_frame emokit_get_next_frame(struct emokit_device* s);








#endif
