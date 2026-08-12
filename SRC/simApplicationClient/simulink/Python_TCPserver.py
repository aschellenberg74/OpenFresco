import socket
import struct

# STREAM for TCP/IP
portNum = 7200
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serversocket.bind(('localhost',portNum))
serversocket.listen(5)

print('Server start on Port '+str(portNum))
while True:
    (connect, address) = serversocket.accept()
    print("Connected to client IP: {}".format(address))

    try:
        while True:
            msg_recv = connect.recv(24)
            if not len(msg_recv):
                break
            else:
                time = struct.unpack('<d', bytearray(msg_recv[ 0: 8]))[0]
                data = struct.unpack('<d', bytearray(msg_recv[ 8:16]))[0]
                flag = struct.unpack('<d', bytearray(msg_recv[16:24]))[0]
                print('Client sent a data %.4f, %8.4f, %.4f' % (time, data, flag) + ', then do some calculations and return it.')
                msg_send = struct.pack('<d', data*2+flag)
                connect.send(msg_send+msg_send+msg_send+msg_send)
    finally:
        connect.close()
        print('No data. Connection closed.')
