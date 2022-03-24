import cv2

rawFileName = "video.mp4"
outputFileName = "example1.mp4"

videoCapture = cv2.VideoCapture(rawFileName)

fps = 30  # 保存视频的帧率
size = (90, 60)  # 保存视频的大小

videoWriter = cv2.VideoWriter(
    outputFileName, cv2.VideoWriter_fourcc(*"DIVX"), fps, size, False
)

i = 0

while True:
    success, frame = videoCapture.read()
    if success:
        i += 1
        if i >= 1:
            frame = cv2.resize(frame, size)
            frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            videoWriter.write(frame_gray)
    else:
        print("end")
        break

# fps = 30  # 保存视频的帧率
# size = (45, 30)  # 保存视频的大小

# videoWriter = cv2.VideoWriter(
#     outputFileName, cv2.VideoWriter_fourcc(*"DIVX"), fps, size, True
# )

# i = 0

# while True:
#     success, frame = videoCapture.read()
#     if success:
#         i += 1
#         if i >= 1:
#             frame = cv2.resize(frame, size)
#             videoWriter.write(frame)
#     else:
#         print("end")
#         break
