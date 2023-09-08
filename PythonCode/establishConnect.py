# This Python file uses the following encoding: utf-8

# if __name__ == "__main__":
#     pass

def connectEmgDevice():
    ## 配置设备
    #192.168.31.95
    neuracle = dict(device_name='Neuracle', hostname='192.168.31.95', port=8712,
                    # srate=1000, chanlocs=['Pz', 'POz', 'PO3', 'PO4', 'PO5', 'PO6', 'Oz', 'O1', 'O2', 'TRG'], n_chan=10)
                    # srate=1000, chanlocs=['S01', 'S02', 'S03', 'S04', 'TRG'], n_chan=5
                    srate = 1000, chanlocs=['P1', 'P2', 'P3', 'P4', 'P5', 'P6', 'P7', 'P8', 'TRG'], n_chan=9)

    heeg = dict(device_name='HEEG', hostname='127.0.0.1', port=8172,
                srate=4000, chanlocs=['ch'] * 512, n_chan=513)

    dsi = dict(device_name='DSI-24', hostname='127.0.0.1', port=8844,
               srate=300,
               chanlocs=['P3', 'C3', 'F3', 'Fz', 'F4', 'C4', 'P4', 'Cz', 'CM', 'A1', 'Fp1', 'Fp2', 'T3', 'T5', 'O1',
                         'O2', 'X3', 'X2', 'F7', 'F8', 'X1', 'A2', 'T6', 'T4', 'TRG'], n_chan=25)
    neuroscan = dict(device_name='Neuroscan', hostname='127.0.0.1', port=4000,
                     srate=1000,
                     chanlocs=['Pz', 'POz', 'PO3', 'PO4', 'PO5', 'PO6', 'Oz', 'O1', 'O2', 'TRG'] + ['TRG'], n_chan=65)
    device = [neuracle,heeg, dsi, neuroscan]
    ### 选着设备型号,默认Neuracle
    target_device = device[0]
    ## 初始化 DataServerThread 线程
    time_buffer = 1  # second
    thread_data_server = DataServerThread(device=target_device['device_name'], n_chan=target_device['n_chan'],
                                          srate=target_device['srate'], t_buffer=time_buffer)
    ### 建立TCP/IP连接
    notconnect = thread_data_server.connect(hostname=target_device['hostname'], port=target_device['port'])
    if notconnect:
        return -1
        #raise TypeError("Can't connect recorder, Please open the hostport ")
    else:
        # 启动线程
        thread_data_server.Daemon = True
        thread_data_server.start()
        print('Data server connected')
        return 0
