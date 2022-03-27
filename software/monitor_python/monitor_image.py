import socket
import cv2
import numpy as np
import time

HOST = "192.168.137.1"
PORT = 2333
BUFSIZ = 30000
recvCount = 0
errCount = 0
startTime = 0
totalTime = 0
ADDR = (HOST, PORT)

udpSerSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udpSerSock.bind(ADDR)

while True:
    data, addr = udpSerSock.recvfrom(BUFSIZ)
    recvCount += 1
    intervalTime = 1000 * (time.perf_counter() - startTime)
    startTime = time.perf_counter()
    totalTime += intervalTime
    print(
        "({}:{})[{}Bytes][{:.2f}ms][{:.2f}ms]Received from {}".format(
            recvCount,
            errCount,
            len(data),
            intervalTime,
            totalTime / recvCount,
            addr,
        )
    )
    try:
        data = np.frombuffer(data, np.uint8)  # 将获取到的字符流数据转换成1维数组
        decimg = data.reshape((60, 90))
        decimg = cv2.resize(decimg, (270, 180))
        cv2.imshow("SERVER", decimg)  # 显示图像
        cv2.waitKey(5)
    except Exception as e:
        errCount += 1
        print(e)
