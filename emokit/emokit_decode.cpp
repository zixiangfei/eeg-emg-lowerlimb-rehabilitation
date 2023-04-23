//============================================================================
// Name        : emokit_decode.cpp
// Author      : zlj
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : emotiv的解密程序
//============================================================================

#include "emokit_decode.h"



const unsigned char F3_MASK[14]     = {10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7};
const unsigned char FC6_MASK[14]    = {214, 215, 200, 201, 202, 203, 204, 205, 206, 207, 192, 193, 194, 195};
const unsigned char P7_MASK[14]     = {84, 85, 86, 87, 72, 73, 74, 75, 76, 77, 78, 79, 64, 65};
const unsigned char T8_MASK[14]     = {160, 161, 162, 163, 164, 165, 166, 167, 152, 153, 154, 155, 156, 157};
const unsigned char F7_MASK[14]     = {48, 49, 50, 51, 52, 53, 54, 55, 40, 41, 42, 43, 44, 45};
const unsigned char F8_MASK[14]     = {178, 179, 180, 181, 182, 183, 168, 169, 170, 171, 172, 173, 174, 175};
const unsigned char T7_MASK[14]     = {66, 67, 68, 69, 70, 71, 56, 57, 58, 59, 60, 61, 62, 63};
const unsigned char P8_MASK[14]     = {158, 159, 144, 145, 146, 147, 148, 149, 150, 151, 136, 137, 138, 139};
const unsigned char AF4_MASK[14]    = {196, 197, 198, 199, 184, 185, 186, 187, 188, 189, 190, 191, 176, 177};
const unsigned char F4_MASK[14]     = {216, 217, 218, 219, 220, 221, 222, 223, 208, 209, 210, 211, 212, 213};
const unsigned char AF3_MASK[14]    = {46, 47, 32, 33, 34, 35, 36, 37, 38, 39, 24, 25, 26, 27};
const unsigned char O2_MASK[14]     = {140, 141, 142, 143, 128, 129, 130, 131, 132, 133, 134, 135, 120, 121};
const unsigned char O1_MASK[14]     = {102, 103, 88, 89, 90, 91, 92, 93, 94, 95, 80, 81, 82, 83};
const unsigned char FC5_MASK[14]    = {28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 8, 9};
const unsigned char QUALITY_MASK[14]= {99,100,101,102,103,104,105,106,107,108,109,110,111,112};

/**
 * @Function  battery_value
 * @param     in
 * @return
 * @exception the percentage battery value given the unencrypted report value
 */
unsigned char battery_value(unsigned char in) {

    if (in>=248) return 100;
    else {
        switch(in) {
            case 247:return 99; break;
            case 246:return 97; break;
            case 245:return 93; break;
            case 244:return 89; break;
            case 243:return 85; break;
            case 242:return 82; break;
            case 241:return 77; break;
            case 240:return 72; break;
            case 239:return 66; break;
            case 238:return 62; break;
            case 237:return 55; break;
            case 236:return 46; break;
            case 235:return 32; break;
            case 234:return 20; break;
            case 233:return 12; break;
            case 232:return 6; break;
            case 231:return 4 ; break;
            case 230:return 3; break;
            case 229:return 2; break;
            case 228:
            case 227:
            case 226:
                return 1;
                break;
            default:
                return 0;
        }
    }
}

/**
 * @brief get_level
 * @param frame
 * @param bits
 * @return
 */
int get_level(unsigned char frame[32], const unsigned char bits[14]) {
    signed char i;
    char b,o;
    int level = 0;

    for (i = 13; i >= 0; --i) {
        level <<= 1;
        b = bits[i]/8+1;
        o = bits[i] % 8 ;

        level |= (frame[b] >> o) & 1;
    }
    return level;
}

/**
 * @Function  handle_quality
 * @param     emokit_device
 * @return 	  last_quality
 * @exception decode and update the s->last_quality, return s->last_quality
 */
struct emokit_contact_quality handle_quality(struct emokit_device* s) {
    int current_contact_quality=get_level(s->raw_unenc_frame,QUALITY_MASK);
    switch(s->raw_unenc_frame[0]) {
            case 0:
                s->last_quality.F3=current_contact_quality;
                break;
            case 1:
                s->last_quality.FC5=current_contact_quality;
                break;
            case 2:
                s->last_quality.AF3=current_contact_quality;
                break;
            case 3:
                s->last_quality.F7=current_contact_quality;
                break;
            case 4:
                s->last_quality.T7=current_contact_quality;
                break;
            case 5:
                s->last_quality.P7=current_contact_quality;
                break;
            case 6:
                s->last_quality.O1=current_contact_quality;
                break;
            case 7:
                s->last_quality.O2=current_contact_quality;
                break;
            case 8:
                s->last_quality.P8=current_contact_quality;
                break;
            case 9:
                s->last_quality.T8=current_contact_quality;
                break;
            case 10:
                s->last_quality.F8=current_contact_quality;
                break;
            case 11:
                s->last_quality.AF4=current_contact_quality;
                break;
            case 12:
                s->last_quality.FC6=current_contact_quality;
                break;
            case 13:
                s->last_quality.F4=current_contact_quality;
                break;
            case 14:
                s->last_quality.F8=current_contact_quality;
                break;
            case 15:
                s->last_quality.AF4=current_contact_quality;
                break;
            case 64:
                s->last_quality.F3=current_contact_quality;
                break;
            case 65:
                s->last_quality.FC5=current_contact_quality;
                break;
            case 66:
                s->last_quality.AF3=current_contact_quality;
                break;
            case 67:
                s->last_quality.F7=current_contact_quality;
                break;
            case 68:
                s->last_quality.T7=current_contact_quality;
                break;
            case 69:
                s->last_quality.P7=current_contact_quality;
                break;
            case 70:
                s->last_quality.O1=current_contact_quality;
                break;
            case 71:
                s->last_quality.O2=current_contact_quality;
                break;
            case 72:
                s->last_quality.P8=current_contact_quality;
                break;
            case 73:
                s->last_quality.T8=current_contact_quality;
                break;
            case 74:
                s->last_quality.F8=current_contact_quality;
                break;
            case 75:
                s->last_quality.AF4=current_contact_quality;
                break;
            case 76:
                s->last_quality.FC6=current_contact_quality;
                break;
            case 77:
                s->last_quality.F4=current_contact_quality;
                break;
            case 78:
                s->last_quality.F8=current_contact_quality;
                break;
            case 79:
                s->last_quality.AF4=current_contact_quality;
                break;
            case 80:
                s->last_quality.FC6=current_contact_quality;
                break;
            default:
                break;
    }
    return (s->last_quality);
}



/**
 * @Function  emokit_get_next_raw
 * @param
 * @return
 * @exception
 */
int emokit_get_next_raw(struct emokit_device* s) {
    //Two blocks of 16 bytes must be read.

    if (memcpy (s->block_buffer, s->raw_frame, s->blocksize)) {
        mdecrypt_generic (s->td, s->block_buffer, s->blocksize);
        memcpy(s->raw_unenc_frame, s->block_buffer, s->blocksize);
    }
    else {
        return -1;
    }

    if (memcpy(s->block_buffer, s->raw_frame + s->blocksize, s->blocksize)) {
        mdecrypt_generic (s->td, s->block_buffer, s->blocksize);
        memcpy(s->raw_unenc_frame + s->blocksize, s->block_buffer, s->blocksize);
    }
    else {
        return -1;
    }

    return 0;
}

/**
 * @Function  emokit_get_next_frame
 * @param     emokit_device
 * @return 	  emokit_frame
 * @exception
 */
struct emokit_frame emokit_get_next_frame(struct emokit_device* s) {
    struct emokit_frame k;
    memset(s->raw_unenc_frame, 0, 32);

    if (emokit_get_next_raw(s)<0) {
        k.counter=0;
        return k;
    }

    memset(&k.cq,0,sizeof(struct emokit_contact_quality));
    if (s->raw_unenc_frame[0] & 128) {
        k.counter = 128;
        k.battery = battery_value( s->raw_unenc_frame[0] );
        s->last_battery=k.battery;
    } else {
        k.counter = s->raw_unenc_frame[0];
        k.battery = s->last_battery;
    }
    k.F3 = get_level(s->raw_unenc_frame, F3_MASK);
    k.FC6 = get_level(s->raw_unenc_frame, FC6_MASK);
    k.P7 = get_level(s->raw_unenc_frame, P7_MASK);
    k.T8 = get_level(s->raw_unenc_frame, T8_MASK);
    k.F7 = get_level(s->raw_unenc_frame, F7_MASK);
    k.F8 = get_level(s->raw_unenc_frame, F8_MASK);
    k.T7 = get_level(s->raw_unenc_frame, T7_MASK);
    k.P8 = get_level(s->raw_unenc_frame, P8_MASK);
    k.AF4 = get_level(s->raw_unenc_frame, AF4_MASK);
    k.F4 = get_level(s->raw_unenc_frame, F4_MASK);
    k.AF3 = get_level(s->raw_unenc_frame, AF3_MASK);
    k.O2 = get_level(s->raw_unenc_frame, O2_MASK);
    k.O1 = get_level(s->raw_unenc_frame, O1_MASK);
    k.FC5 = get_level(s->raw_unenc_frame, FC5_MASK);

    k.gyroX = s->raw_unenc_frame[29] - 102;
    k.gyroY = s->raw_unenc_frame[30] - 104;

    k.cq=handle_quality(s);

    return k;
}
