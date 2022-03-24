import serial
import time
import cv2
import numpy as np

useCamera = False
videoPath = ".\\example1.mp4"

try:
    ser = serial.Serial(
        port="com14",
        baudrate=3000000,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        rtscts=False,
        xonxoff=False,
    )
    ser.set_buffer_size(20000,20000)
    ser.open()  # 打开端口
except Exception as e:
    print(e)

videoCapture = (
    cv2.VideoCapture(videoPath) if useCamera is False else cv2.VideoCapture(0)
)
i = 0

while True:
    startTime = time.perf_counter()
    success, frame = videoCapture.read()
    if success:
        if useCamera is True:
            frame = cv2.resize(frame, (90, 60))
        frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        buf = bytearray(np.array(frame_gray).astype(np.uint8))
        ser.write(buf)
        print(
            "{}bytes:{:.2f}ms".format(
                len(buf), (time.perf_counter() - startTime) * 1000
            )
        )
        time.sleep(0.025)
    else:
        print("end")
        videoCapture.release()
        break
