import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import butter, filtfilt,lfilter
from scipy import signal
import scipy.io as scio

'''
函数的输入参数包括：
signal_data：原始的sEMG信号数据。
fs：采样频率，默认为1000 Hz。
freq、Q：陷波滤波器的参数，分别表示带阻频率和质量因数，默认为50 Hz和30。
cutoff：高通滤波器的截止频率，默认为30 Hz。
函数的处理流程如下：
使用 signal.iirnotch() 函数设计一个陷波滤波器，并将该滤波器应用于原始信号 signal_data 上，得到 notch_filtered_signal。
使用 signal.butter() 函数设计一个二阶的零相移高通滤波器，将截止频率 cutoff 转换为归一化频率，并将滤波器应用于 notch_filtered_signal 上，得到经过高通滤波的 filtered_signal。
返回预处理后的信号 filtered_signal。
该函数的作用是先使用陷波滤波器去除50 Hz的工频干扰，然后使用零相移高通滤波器去除运动伪迹，以提取出较干净的sEMG信号。最后返回预处理后的信号供后续分析和处理使用。  
'''
def emg_processing_1(signal_data, fs=1000, freq=50, Q=30, cutoff=30):
    b, a = signal.iirnotch(freq, Q, fs)
    notch_filtered_signal  = signal.lfilter(b, a, signal_data)
    nyquist_freq = 0.5 * fs
    normalized_cutoff = cutoff / nyquist_freq
    b, a = signal.butter(2, normalized_cutoff, btype='high', analog=False)
    filtered_signal = signal.filtfilt(b, a, notch_filtered_signal )
    return filtered_signal

def devlop_precess(output,fs,LOWPASSRATE = 4, NUMPASSES = 2):
    EMGFWR = np.abs(output)
    # 线性包络
    Wn = LOWPASSRATE/(fs/2)
    b, a = butter(NUMPASSES, Wn, 'low')
    EMGLE = filtfilt(b, a, EMGFWR)
    return EMGLE


def extract_MAV(signal):
    """
    将信号按照时间窗100个点一组进行分组，并计算每组的滑动窗口下绝对值平均值（MAV）
    :param signal: 连续的肌电信号，类型为numpy数组
    :return: MAV特征值列表，类型为numpy数组
    """
    # 将信号按照100个点一组进行分组
    #  # 设定处理窗口的宽度

    signal_groups = signal.reshape(-1, window_width)
    mean_values = np.mean(np.abs(signal_groups), axis=1)
    return mean_values

def extract_ZC(signal, threshold=0):
    """
    将信号按照时间窗100个点一组进行分组，并计算每组的滑动窗口下零穿越数（ZC）
    :param signal: 连续的肌电信号，类型为numpy数组
    :param threshold: 幅值阈值，默认为0
    :return: ZC特征值列表，类型为numpy数组
    """
    # 将信号按照100个点一组进行分组


    windows = signal.reshape(-1, window_width)

    # 计算每个时间窗口下的零穿越数，即ZC
    zc = np.sum(np.abs(np.diff(np.signbit(windows-threshold).astype(np.int_))), axis=1)/2

    return zc

def extract_WL(signal):
    """
    将信号按照时间窗100个点一组进行分组，并计算每组的滑动窗口下波形长（WL）
    :param signal: 连续的肌电信号，类型为numpy数组
    :return: WL特征值列表，类型为numpy数组
    """
    # 将信号按照100个点一组进行分组

    windows = signal.reshape(-1, window_width)

    # 计算每个时间窗口下的波形长，即WL
    wl = np.sum(np.abs(np.diff(windows)), axis=1)

    return wl

def extract_RMS(signal):
    """
    将信号按照时间窗100个点一组进行分组，并计算每组信号的均方根（RMS）
    :param signal: 连续的肌电信号，类型为numpy数组
    :return: RMS特征值列表，类型为numpy数组
    """
    # 将信号按照100个点一组进行分组

    windows = signal.reshape(-1, window_width)

    # 计算每个时间窗口下的均方根，即RMS
    rms = np.sqrt(np.mean(np.square(windows), axis=1))

    return rms

def calculate_ACT(signal):
    """
    根据预处理后的sEMG信号计算肌肉激活度（ACT）
    :param signal: 连续的sEMG信号，类型为numpy数组
    :return: ACT特征值，类型为float
    """
    #  # 设定处理窗口的宽度
    lamda1 = 0.5
    lamda2 = 0.5
    bata1 = lamda1 + lamda2
    bata2 = lamda1 * lamda2
    arfa = 1 + bata1 + bata2
    d = 10 

    u = np.zeros(len(signal) + d)

    e = signal  # 示例输入信号序列

    for ii in range(len(e)):
        u[ii] = (arfa*e[ii] - bata1*u[ii+d-1] - bata2*u[ii+d-2])
    A = -1.5
    a = (np.exp(A*np.array(u)) - 1) / (np.exp(A) - 1)
    a = a[10:]
    signal_groups = a.reshape(-1, window_width)
    mean_values = np.mean(signal_groups, axis=1)
    return mean_values

#window_width = 100

#pathlist = ['静息', '静息1', '静息2', '屈1', '屈2', '屈3', '伸', '伸1', '伸2', '伸3']
#for i in range(len(pathlist)):
#        
#    dataPath = '../' + pathlist[i] + '/matlab.mat'
#
#    raw_data = scio.loadmat(dataPath)
#    raw_data = raw_data['data1'][:,3000:-3000]
#    # print(raw_data.shape)
#    features = np.zeros((8,5,len(raw_data[0]) // 100))
#    channel_index = 0
#    for channel_data in raw_data:
#        # 对每个通道的数据进行特征提取和处理
#        output = emg_processing_1(channel_data)
#        devlop_data = devlop_precess(output, 1000)
#        m_MV_TA = np.max(devlop_data)
#        devlop_data = devlop_data / m_MV_TA
#        MAV = extract_MAV(devlop_data)
#        features[channel_index, 0, :] = MAV
#        ZC = extract_ZC(output)
#        # ZC_max = np.max(ZC)
#        # ZC = ZC / ZC_max
#        features[channel_index, 1, :] = ZC
#        WL = extract_WL(devlop_data)
#        features[channel_index, 2, :] = WL
#        RMS = extract_RMS(devlop_data)
#        features[channel_index, 3, :] = RMS
#        ACT = calculate_ACT(devlop_data)
#        features[channel_index, 4, :] = ACT
#        channel_index += 1

    features_new = np.transpose(features, (2, 0, 1))
    # features_new = features_new.reshape(len(raw_data[0] // 100),40)
    features_new = features_new.reshape(features_new.shape[0],features_new.shape[1] * features_new.shape[2])
    if '静息' in pathlist[i]:
        move = np.zeros(features_new.shape[0])
    elif '屈' in pathlist[i]:
        move = np.ones(features_new.shape[0])
    else:
        move = np.ones(features_new.shape[0]) + 1
    # print(features_new)
    outputPath = '../' + pathlist[i] + '/feature.mat'
    scio.savemat(outputPath, {'feature': features_new, 'move':move})


