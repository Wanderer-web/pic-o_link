import socket

HOST = "192.168.137.1"
PORT = 2333
BUFSIZ = 30000
recvCount = 0
ADDR = (HOST, PORT)

udpSerSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udpSerSock.bind(ADDR)

while True:
    data, addr = udpSerSock.recvfrom(BUFSIZ)
    recvCount += 1
    try:
        print(
            "({})[{}Bytes]Received from {}:\n{}\n".format(
                recvCount, len(data), addr, data.decode("utf-8")
            )
        )
    except Exception as e:
        print(e)
