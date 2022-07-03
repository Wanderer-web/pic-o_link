import socket
import cv2
import numpy as np
import time

HOST = "192.168.137.1"  # IP地址
PORT = 2333  # 端口
BUFSIZ = 30000  # 接收缓冲区大小
IMAGE_W = 80  # 图像宽度
IMAGE_H = 60  # 图像高度
FRAME_HEADER = b"CSU"  # 帧头
FRAME_END = b"USC"  # 帧尾
ADDR = (HOST, PORT)
udpSerSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udpSerSock.bind(ADDR)
udpRecvData = b""
recvCount = 1
errCount = 0
startTime = 0
totalTime = 0


while True:
    try:
        data, addr = udpSerSock.recvfrom(BUFSIZ)
        intervalTime = 1000 * (time.perf_counter() - startTime)
        startTime = time.perf_counter()
        totalTime += intervalTime
        udpRecvData += data
        headerIdx = udpRecvData.find(FRAME_HEADER)
        endIdx = udpRecvData.find(FRAME_END)
        if headerIdx != -1 and endIdx != -1:
            if (endIdx - headerIdx - len(FRAME_HEADER)) == IMAGE_H * IMAGE_W:  # 校验
                tmpData = udpRecvData[headerIdx + len(FRAME_HEADER) : endIdx]
                decimg = np.frombuffer(tmpData, np.uint8).reshape(
                    (IMAGE_H, IMAGE_W)
                )  # 将获取到的字符流数据转换成1维数组
                decimg = cv2.resize(decimg, (270, 180))
                recvCount += 1
                cv2.imshow("SERVER", decimg)  # 显示图像
                cv2.waitKey(2)
            else:
                print("lose one frame")
                errCount += 1
            udpRecvData = udpRecvData[endIdx + len(FRAME_END) : -1]
        print(
            "({}:{})[{}Bytes][{:.2f}ms][{:.1f}fps]Received from {}".format(
                recvCount,
                errCount,
                len(data),
                intervalTime,
                1000 / (totalTime / recvCount),
                addr,
            )
        )
    except Exception as e:
        errCount += 1
        print(e)
