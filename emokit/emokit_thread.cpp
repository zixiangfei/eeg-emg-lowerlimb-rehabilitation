#include "emokit_thread.h"
#include <msgbox.h>
using namespace std;

#define RAW_FRAME_QUEUE_SIZE  1

//queue <emokit_raw_frame_data> raw_frame_queue;
pthread_mutex_t emokit_mutex;

struct emokit_device*        eomkit_dev;
struct emokit_raw_frame_data emokit_raw_frame_date;

int emokit_int_res = ERROR;
int thread_start_or_stop = ERROR;


/**
 * @Function  emokit_thread_init
 * @param 	  null
 * @return	  null
 * @exception 初始化emokit驱动，及线程
 */
void emokit_thread_init()
{
    int thread_int_res = ERROR;
    int messagebox_res = ERROR;
 //   QMessageBox msgBox;
    /*emokit驱动初始化*/
    emokit_int_res = emokit_init();
    while(emokit_int_res > 0)
    {


        MsgBox msgBox;
        msgBox.Waring("Emotiv Init Failed!\nEmotiv Restart Init!");
        if(msgBox.exec())
        {
            emokit_int_res = emokit_init();
        }
        else
        {
            emokit_int_res = CANNEL;
        }
        qDebug("thread_start_or_stop:%d",thread_start_or_stop);
    }
    /*emokit初始化成功，开始线程初始化*/
    if(emokit_int_res == OK )
    {
	/*初始化成功后将start——stop变量赋值为OK*/
        thread_start_or_stop = OK;
        /*互斥信号量初始化*/
        thread_int_res = pthread_mutex_init(&emokit_mutex,NULL);
        if(thread_int_res != OK)
        {
            qFatal("emokit_mutex initialization failed!");
            perror("emokit_mutex initialization failed!");
            exit(EXIT_FAILURE);
        }
        /*线程初始化*/
        thread_int_res = pthread_create(&emokit_thread,NULL,emokit_thread_function,NULL);
        if(thread_int_res != OK)
        {
            qFatal("emokit_thread creation failed!");
            perror("emokit_thread creation failed!");
            exit(EXIT_FAILURE);
        }

/*        thread_int_res = pthread_create(&ack_thread,NULL,ack_thread_function,NULL);
        if(thread_int_res != OK)
        {
            qFatal("ack_thread creation failed!");
            perror("ack_thread creation failed!");
            exit(EXIT_FAILURE);
        }

        thread_int_res = pthread_create(&serial_port_thread,NULL,serial_port_thread_function,NULL);
        if(thread_int_res != OK)
        {
            qFatal("ack_thread creation failed!");
            perror("ack_thread creation failed!");
            exit(EXIT_FAILURE);
        }*/
    }
}


















/*
 * @Function  push_raw_data
 * @param 	  emokit_raw_frame_data
 * @return	  null
 * @exception 将数据添加到队列，队列长度为5，当长度为5，
 * 如果再添加进来，则队头数据将被舍弃，将新数据添加到队尾。
 */
//void push_raw_data(struct emokit_raw_frame_data emokit_raw_frame_dat)
//{
//    if(raw_frame_queue.size() < RAW_FRAME_QUEUE_SIZE)
//    {
//        raw_frame_queue.push(emokit_raw_frame_dat);
//    }
//    else
//    {
//        /*舍弃队头元素*/
//        raw_frame_queue.pop();
//        /*将新元素添加到队尾*/
//        raw_frame_queue.push(emokit_raw_frame_dat);
//    }
//}

/*
 * @Function  pop_raw_data
 * @param 	  null
 * @return	  null
 * @exception 返回队头元素,当队列为空时返回数据都为0。
 */
//bool pop_raw_data()
//{
//    struct emokit_raw_frame_data data;
//    if(!raw_frame_queue.empty())
//    {
//        data = raw_frame_queue.front();
//        printf("pop data!\n");
//        memcpy(eomkit_dev->raw_frame,data.raw_frame_data,32);
//        raw_frame_queue.pop();
//        return true;

//    }
//    return false;
//}

/*
 * @Function  emokit_read_raw_frame_thread
 * @param 	  arg
 * @return	  void
 * @exception 读取数据线程。
 */
//void *emokit_read_raw_frame_thread(void *arg)
//{
//    int res = ERROR;
//    res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
//    if(res == OK)
//    {
//        printf("PTHREAD_CANCEL_ENABLE ok!\n");
//        res = ERROR;
//    }
//    res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
//    if(res == OK)
//    {
//        printf("PTHREAD_CANCEL_ASYNCHRONOUS ok!\n");
//        res = ERROR;
//    }
//    while(1)
//    {
//        if(thread_start_or_stop == OK)
//        {
//            /*获取互斥变量*/
//            pthread_mutex_lock(&emokit_mutex);
//            /*读取数据*/
//            if(emokit_read_data(eomkit_dev)>0)
//            {
//                printf("read data from emokit ok!\n");

//                /*将读取的数据入队*/
//                push_raw_data(emokit_raw_frame_date);
//                printf("push_raw_data ok! \n");
//            }
//            /*释放互斥变量*/
//            pthread_mutex_unlock(&emokit_mutex);
//            usleep(7000);
//        }
//        else
//        {
//            if(thread_start_or_stop == STOP)
//            {
//                usleep(8000);
//            }
//        }
//    }
//    pthread_exit(0);
//    return NULL;
//}
