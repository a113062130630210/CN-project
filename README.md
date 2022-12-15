# 計網 project phase2

- B09901133 林天行
- B09901171 黃柏睿

Page link: [https://cn-project.ntuee.org](https://cn-project.ntuee.org)

## 留言板功能

```bash
make
./server
```

連上 https://cn-project.ntuee.org 後，先在註冊頁面註冊完並登入後，可以看到留言板內容，並且會利用 cookie 的 id 欄位去看現在登入的帳號是誰，並在留言前標記是哪個帳號留言的
(盡量不要發有特殊字元的留言，有可能該字元會變成空白)

## 註冊登入登出功能
```bash
make
./server
```

連上 https://cn-project.ntuee.org 後，可以註冊並登入進入留言板，在留言板內可以登出，有使用 cookie 的 id 欄位記錄現在使用者是誰，登出後會將 cookie 的 id 設成 None (原本是使用 expires 並將日期設成一個之前的日期，但並沒有成功)，server 會去偵測如果 cookie id 是 None 並且要 access 留言板(代表已經登出)，會重新導回登入頁面

## 聲音串流

```bash
cd voice
# May need to install portaudio first
# Ubuntu
sudo apt install portaudio19-dev python3-pyaudio
# MacOS
brew install portaudio

pip install -r requirements.txt

# MacOS may need to increase UDP package size limit
sudo sysctl -w net.inet.udp.maxdgram=65535

# Run Server
python3 server.py <host IP> <port> <file>
python3 server.py 127.0.0.1 7777 song.wav

# Server can use ifconfig to check its own IP and tell the client

# Run Client
python3 client.py <server IP> <port>
python3 client.py 127.0.0.1 7777
```

我用 pyaudio 這個套件讀取聲音檔，然後 server 用 UDP 一個一個 frame 傳，client 使用兩個 thread，一個負責把資料接收到 queue 裡，另一個負責按照 frame rate 播放聲音，達到同步聲音串流

## 視訊串流

```bash
cd video
pip install -r requirements.txt

# MacOS may need to increase UDP package size limit
sudo sysctl -w net.inet.udp.maxdgram=65535

# Run Server
python3 server.py <host IP> <port>
python3 server.py 127.0.0.1 7777

# Server can use ifconfig to check its own IP and tell the client

# Run Client
python3 client.py <server IP> <port>
python3 client.py 127.0.0.1 7777
```

我用 opencv 讀取 webcam 的影像，經過裁剪跟 jpg 壓縮後用 UDP 傳出來，client 接收後解壓縮再顯示在另一個視窗中

## 額外功能

### HTTPS

用 nginx 過 cloudflare 的憑證，達到最高安全性

### Multithread

用兩個 thread 讓聲音串流可以同時接收資料跟播放聲音
