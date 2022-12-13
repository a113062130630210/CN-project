import socket, cv2, pickle

client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
host_ip = "127.0.0.1"
port = 7777
print("Connecting to:", host_ip, port)
# client_socket.connect((host_ip, port))
client_socket.sendto("Hello".encode(), (host_ip, port))
data = b""

while True:
    data = client_socket.recv(65535)
    # print("Received", len(data))
    
    frame = pickle.loads(data)
    frame = cv2.imdecode(frame, 1)
    cv2.imshow("Receiving Video ", frame)   
    key = cv2.waitKey(10)
    if key == 13:
        break   
    
client_socket.close()
