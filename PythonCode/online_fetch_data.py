
# !/usr/bin/env python3
# -*- coding:utf-8 -*-

# Author: FANG Junying, fangjunying@neuracle.cn

# Versions:
# 	v0.1: 2018-08-14, orignal
# 	v1.0: 2020-06-04，update demo
# Copyright (c) 2016 Neuracle, Inc. All Rights Reserved. http://neuracle.cn/

from neuracle_lib.dataServer import DataServerThread
import time
import numpy as np
import matplotlib.pyplot as plt
import scipy.io as scio
import joblib
from extract_feature import *

def analysisData():
    '''
    在线数据获取演示：每隔一秒钟，获取数据（数据长度 = time_buffer)
    '''
    N, flagstop = 0, False
    try:
        while not flagstop:  # get data in one second step
            nUpdate = thread_data_server.GetDataLenCount()
            if nUpdate > (1 * target_device['srate'] - 1):
                N += 1
                raw_data = thread_data_server.GetBufferData()
                #print(data)
                thread_data_server.ResetDataLenCount()
                # time.sleep(1)
            if N > 2:
                flagstop = True
    except:
        pass
    ## 结束线程
    #thread_data_server.stop()

    #print(data.shape)
    #tt = np.array(range(data.shape[1])) / target_device['srate']
    #plt.figure()
    #plt.plot(tt, data[1,:] )
    #plt.plot(tt, data[-1, :])
    #plt.show()
    raw_data = raw_data['data1'][:, :-100]
    for channel_data in raw_data:
    # 对每个通道的数据进行特征提取和处理
        output = emg_processing_1(channel_data)
        devlop_data = devlop_precess(output, 1000)
        m_MV_TA = np.max(devlop_data)
        devlop_data = devlop_data / m_MV_TA
        MAV = extract_MAV(devlop_data)
        X_test.append(MAV[0])

        ZC = extract_ZC(output)
        # ZC_max = np.max(ZC)
        # ZC = ZC / ZC_max
        X_test.append(ZC[0])
        WL = extract_WL(devlop_data)

        X_test.append(WL[0])
        RMS = extract_RMS(devlop_data)
        X_test.append(RMS[0])
        ACT = calculate_ACT(devlop_data)
        X_test.append(ACT[0])

    X_test = np.array(X_test)

    # 从文件加载模型
    clf = joblib.load('/home/pi/Desktop/RecoveredEquipment/PythonCode/svm_model.pkl')

    # 使用模型进行预测
    y_pred = clf.predict([X_test])
    #print(y_pred)
    return y_pred[0]



