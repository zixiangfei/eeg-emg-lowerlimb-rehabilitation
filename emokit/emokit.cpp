/* Copyright (c) 2010, Daeken and Skadge
 * Copyright (c) 2011-2012, OpenYou Organization (http://openyou.org)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
//============================================================================
// Name        : emokit.cpp
// Author      : zlj
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : emotiv的驱动程序
//============================================================================


#include "emokit.h"
#include <QDebug>


/**
 * @Function  emokit_create
 * @param     null
 * @return 	  emokit_device
 * @exception emokit驱动创建
 */
struct emokit_device* emokit_create()
{
    struct emokit_device* s = (struct emokit_device*)malloc(sizeof(struct emokit_device));
    memset(s,0,sizeof(struct emokit_device));
    s->_is_open = 0;
    s->_is_inited = 0;
    hid_init();
    s->_is_inited = 1;
    return s;
}

/**
 * @Function  emokit_get_count
 * @param     emokit_device，device_vid，device_pid
 * @return 	  int
 * @exception emokit驱动计数
 */
int emokit_get_count(struct emokit_device* s, int device_vid, int device_pid)
{
    int count = 0;
    struct hid_device_info* devices;
    struct hid_device_info* device_cur;
    if (!s->_is_inited)
    {
        return E_EMOKIT_NOT_INITED;
    }
    devices = hid_enumerate(device_vid, device_pid); //枚举设备 一般使用（0,0） 枚举所有设备

    device_cur = devices;
    while(device_cur) {
        ++count;
        device_cur = device_cur->next;
    }

    hid_free_enumeration(devices); //释放所用到的链表
    return count;
}

/**
 * @Function  emokit_identify_device
 * @param     hid_device
 * @return 	  int
 * @exception emokit驱动认证
 */
int emokit_identify_device(hid_device *dev) {
    /* currently we check to see if the feature report matches the consumer
       model and if not we assume it's a research model.*/
    int nbytes, i, dev_type = EMOKIT_CONSUMER;
    unsigned char buf[EMOKIT_REPORT_SIZE];
    unsigned char report_consumer[] = {0x00, 0xa0, 0xff, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00};
    buf[0] = EMOKIT_REPORT_ID;
    nbytes = hid_get_feature_report(dev, buf, sizeof(buf));
    if (nbytes != EMOKIT_REPORT_SIZE)
    {
        return -1;
    }
    for (i=0; i < nbytes; ++i) {
        if (buf[i] != report_consumer[i]) {
            dev_type = EMOKIT_RESEARCH;
            break;
        }
    }
    return dev_type;
}

int emokit_init_crypto(struct emokit_device* s, int dev_type) {
    emokit_get_crypto_key(s, dev_type);

    //libmcrypt initialization
    s->td = mcrypt_module_open(MCRYPT_RIJNDAEL_128, NULL, MCRYPT_ECB, NULL);
    s->blocksize = mcrypt_enc_get_block_size(s->td); //should return a 16bits blocksize

    s->block_buffer = (unsigned char *)malloc(s->blocksize);

    mcrypt_generic_init(s->td, s->key, EMOKIT_KEYSIZE, NULL);
    return 0;
}

/**
 * @Function  emokit_get_count
 * @param     emokit_device，device_vid，device_pid
 * @return 	  int
 * @exception emokit驱动打开
 */
int emokit_open(struct emokit_device* s, int device_vid, int device_pid, unsigned int device_index)
{
    int dev_type;
    unsigned int count = 0;
    struct hid_device_info* devices;
    struct hid_device_info* device_cur;
    if (!s->_is_inited)
    {
        return E_EMOKIT_NOT_INITED;
    }
    devices = hid_enumerate(device_vid, device_pid);

    device_cur = devices;
    while(device_cur) {
        if(count == device_index) {
            s->_dev = hid_open_path(device_cur->path);
            break;
        }
        ++count;
        device_cur = device_cur->next;
    }

    hid_free_enumeration(devices);
    if(!s->_dev) {
        return E_EMOKIT_NOT_OPENED;
    }
    s->_is_open = 1;
    //dev_type = emokit_identify_device(s->_dev);
    dev_type = EMOKIT_RESEARCH;
    hid_get_serial_number_string(s->_dev, s->serial, MAX_STR);
    emokit_init_crypto(s, dev_type);

    return 0;
}


/**
 * @Function  emokit_close
 * @param     emokit_device
 * @return 	  int
 * @exception emokit驱动关闭
 */
int emokit_close(struct emokit_device* s)
{
    if(!s->_is_open)
    {
        return E_EMOKIT_NOT_OPENED;
    }
    hid_close(s->_dev);
    s->_is_open = 0;
    return 0;
}
/**
 * @Function  emokit_read_data
 * @param     emokit_device，emokit_raw_frame_data
 * @return 	  int
 * @exception emokit数据读取
 */
int emokit_read_data(struct emokit_device* s)
{
    return hid_read(s->_dev,s->raw_frame, 32);
}


/**
 * @brief emokit_get_crypto_key
 * @param s
 * @param dev_type
 */
void emokit_get_crypto_key(struct emokit_device* s, int dev_type) {
    unsigned char type = (unsigned char) dev_type;
    unsigned int l = 16;
    type &= 0xF;
    type = (type == 0);

    s->key[0] = (uint8_t)s->serial[l-1];
    s->key[1] = '\0';
    s->key[2] = (uint8_t)s->serial[l-2];
    if(type) {
        s->key[3] = 'H';
        s->key[4] = (uint8_t)s->serial[l-1];
        s->key[5] = '\0';
        s->key[6] = (uint8_t)s->serial[l-2];
        s->key[7] = 'T';
        s->key[8] = (uint8_t)s->serial[l-3];
        s->key[9] = '\x10';
        s->key[10] = (uint8_t)s->serial[l-4];
        s->key[11] = 'B';
    }
    else {
        s->key[3] = 'T';
        s->key[4] = (uint8_t)s->serial[l-3];
        s->key[5] = '\x10';
        s->key[6] = (uint8_t)s->serial[l-4];
        s->key[7] = 'B';
        s->key[8] = (uint8_t)s->serial[l-1];
        s->key[9] = '\0';
        s->key[10] = (uint8_t)s->serial[l-2];
        s->key[11] = 'H';
    }
    s->key[12] = (uint8_t)s->serial[l-3];
    s->key[13] = '\0';
    s->key[14] = (uint8_t)s->serial[l-4];
    s->key[15] = 'P';
}

/**
 * @brief emokit_deinit
 * @param s
 */
void emokit_deinit(struct emokit_device* s) {
    mcrypt_generic_deinit (s->td);
    mcrypt_module_close(s->td);
}


/**
 * @brief PrintHex
 * @param data
 * @param len
 */
void PrintHex(unsigned char* data, int len)
{
    int i;

    for(i=0; i<len; i++)
    {
        printf("%02x",data[i]);
    }
    printf("\n");
}

/**
 * @brief emokit_delete
 * @param dev
 */
void emokit_delete(struct emokit_device* dev)
{
    emokit_deinit(dev);
    free(dev);
}

/**
 * @brief emokit_get_raw_frame
 * @param dev
 * @param buf
 */
void emokit_get_raw_frame(struct emokit_device* dev, unsigned char buf[32]) {
    memcpy(buf, dev->raw_unenc_frame, 32);
}

/**
 * @Function  emokit_init
 * @param     emokit_device
 * @return 	  int
 * @exception emokit初始化
 */
//int emokit_init()
//{
//    int emokit_count;
//    /*添加文件权限*/
// //   system("su -c 'chmod 777 /dev/hidraw*'");

//    eomkit_dev = emokit_create();
//    emokit_count = emokit_get_count(eomkit_dev, EMOKIT_VID, EMOKIT_PID);
//    qDebug("emokit_count: %d", emokit_count);

//    int r = emokit_open(eomkit_dev, EMOKIT_VID, EMOKIT_PID, 1);
//    if(r != 0)
//    {

//        emokit_close(eomkit_dev);
//        emokit_delete(eomkit_dev);
//        eomkit_dev = emokit_create();
//        r = emokit_open(eomkit_dev, EMOKIT_VID, EMOKIT_PID, 0);
//        if (r!=0) {
//            qDebug("CANNOT CONNECT: %d", r);
//            return E_EMOKIT_INT_ERROR;
//        }
//    }
//    return OK;
//}





