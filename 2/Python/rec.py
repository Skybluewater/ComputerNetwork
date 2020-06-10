import serial
import serial.tools.list_ports


class Communication():

    # 初始化
    def __init__(self, com, bps, bytesize, parity, stopbits,timeout,):
        self.port = com
        self.bps = bps
        self.timeout = timeout
        self.bytesize = bytesize
        self.parity = parity
        self.stopbits = stopbits
        global Ret
        try:
            # 打开串口，并得到串口对象
            self.main_engine = serial.Serial(self.port,
                                             self.bps,
                                             bytesize=self.bytesize,
                                             parity=self.parity,
                                             stopbits=self.stopbits,
                                             timeout=self.timeout)
            # 判断是否打开成功
            if (self.main_engine.is_open):
                Ret = True
        except Exception as e:
            print("---异常---：", e)

    # 打印设备基本信息
    def Print_Name(self):
        print("name: ", self.main_engine.name)  # 设备名字

    def Print_Attri(self):
        print("port: " , self.main_engine.port)  # 读或者写端口
        print("baudrate: " , self.main_engine.baudrate)  # 波特率
        print("bytesize: " , self.main_engine.bytesize)  # 字节大小
        print("parity: " , self.main_engine.parity)  # 校验位
        print("stopbits: " , self.main_engine.stopbits)  # 停止位
        print("timeout: " , self.main_engine.timeout)  # 读超时设置
        print("writeTimeout: " , self.main_engine.writeTimeout)  # 写超时

    # 发数据
    def Send_data(self, data):
        self.main_engine.write(data.encode("utf-8"))


    def Recive_data(self):
        # 循环接收数据，此为死循环，可用线程实现
        print("开始接收数据：")
        while True:
            try:
                if self.main_engine.in_waiting:
                    # 整体接收
                    data = self.main_engine.read(self.main_engine.in_waiting).decode("utf-8")#方式一
                    if (data == "exit"):  # 退出标志
                        break
                    else:
                        print("接收数据：", data)
            except Exception as e:
                print("异常报错：", e)


Ret = False  # 是否创建成功标志

Engine1 = Communication("com2", 9600,serial.EIGHTBITS,serial.PARITY_ODD,serial.STOPBITS_ONE, None)
if (Ret):
    print("设备名称： ")
    Engine1.Print_Name()
    print("设备属性： ")
    Engine1.Print_Attri()
    Engine1.Recive_data()