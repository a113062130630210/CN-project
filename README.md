# 計網 project phase2

B09901133 林天行
B09901171 黃柏睿

## 留言板功能

## 註冊登入登出功能

## 聲音串流

```bash
cd voice
pip install -r requirements.txt

# MacOS may need to increase UDP package size limit
sudo sysctl -w net.inet.udp.maxdgram=65535

# Run Server
python3 server.py <host IP> <port> <file>
python3 server.py 127.0.0.1 7777 song.wav

# Run Client
python3 client.py <server IP> <post>
python3 client.py 127.0.0.1 7777
```

我用pyaudio這個套件讀取聲音檔，然後server用UDP一個一個frame傳，client使用兩個thread，一個負責把資料接收到queue裡，另一個負責按照frame rate播放聲音，達到同步聲音串流

## 視訊串流

```bash
cd voice
pip install -r requirements.txt

# MacOS may need to increase UDP package size limit
sudo sysctl -w net.inet.udp.maxdgram=65535

# Run Server
python3 server.py <host IP> <port>
python3 server.py 127.0.0.1 7777

# Run Client
python3 client.py <server IP> <post>
python3 client.py 127.0.0.1 7777
```

我用opencv讀取webcam的影像，經過裁剪跟jpg壓縮後用UDP傳出來，client接收後解壓縮再顯示在另一個視窗中

## 額外功能

### HTTPS

用nginx過cloudflare的憑證，達到最高安全性

### Multithread

用兩個thread讓聲音串流可以同時接收資料跟播放聲音
