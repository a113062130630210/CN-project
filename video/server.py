import socket, cv2, pickle, imutils

# Socket Create
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
host_ip = "127.0.0.1"
port = 7777
socket_address = (host_ip, port)
server_socket.bind(socket_address)
print("Listening at:", socket_address)

while True:
    data, addr = server_socket.recvfrom(1024)
    print("Connected to:", addr)
    if data:
        vid = cv2.VideoCapture(0)

        while vid.isOpened():
            ret, image = vid.read()
            if not ret:
                break
            image = imutils.resize(image, width=640)
            encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 80]
            result, encimage = cv2.imencode('.jpg', image, encode_param)
            
            img_serialize = pickle.dumps(encimage)
            
            print("Send", len(img_serialize))
            server_socket.sendto(img_serialize, addr)
                
            cv2.imshow("Video from Server", image)
            key = cv2.waitKey(10)
            if key == 13:
                break
