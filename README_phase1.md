# 計網 project phase1

## 1. socket 傳文字

我的設計是讓 client 跟 server 交互傳文字，並且由 client 先傳

### 執行方式

執行 makefile 即可，它會把 server.cpp 跟 client.cpp 編譯並分別命名執行檔為 server 跟 client

```makefile
make
```

開啟 server

```bash
./server
```

開啟 client (要先開啟 server 才能開啟 client，否則會 Connection refused)

```bash
./client
```

開啟 client 後，client 的 terminal 會出現

```bash
Please say something to server:     
```

這時候輸入的文字 client 就可以傳給 server，並且出現在 server 的 terminal 上，之後 server 的 terminal 會出現 

```bash
Please say something to client: 
```

這時候 server 可以傳文字給 client，並且出現在 client 的 terminal 上，並且 client 的 terminal 又會出現

```bash
 Please say something to server:     
```

以此類推



最後可以 make clean 刪除 server 跟 client 兩個執行檔

```makefile
make clean
```



## 2. personal profile webpage

http://oasis1.csie.ntu.edu.tw:9413/

示意圖:

![image-20221027150707946](/home/kevin/.config/Typora/typora-user-images/image-20221027150707946.png)

我把 server 放在工作站的 oasis1 上，並且在 tmux 執行 server，這樣即使沒連到工作站 server 也在執行 (影片中我是在背景執行 server，而不是在 tmux，但我後來發現登出工作站後 server 不會繼續執行，因此後來改用 tmux 執行)

