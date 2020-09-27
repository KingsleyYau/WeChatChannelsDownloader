Catalog
=================

   * [WeChatChannel下载器](#WeChatChannel下载器)
      
      
# WeChatChannel下载器
下载微信视频号里面的指定视频

## 原理
直接下载的MP4文件会将前面20000h字节和固定的key做异或</br>
如何获得视频URL就靠抓包了，需要屏蔽掉QUIC协议（UDP/443），然后抓80端口</br>
URL例子
```
http://wxapp.tc.qq.com/251/20302/stodownload?encfilekey=G83YYE2iciaib491UK8yGibLXDKSgvVgk8CgnTwJLwVPt0Micv8OPgfPIic08nGRQA9npw9ZFgL8mCHWehQ8CZRW3Out2PA93mmz5ZMMX3zk7xAbBnH3iaDdPRWQ0HpEnOJxibHabxmpzwa6VGzHDEDQ5Hjacaw4S0c3icPae&bizid=1023&dotrans=934&hy=SZ&idx=1&m=219abab90465b969dbf235ffb5df6a41&token=AxricY7RBHdV3hUtSjd0bFD5NUmoTX72oZne2AHC9eltDhxEOKv4w7HqiceITynbJtLB2OrXjtpV0
```
<img height="240" src="https://github.com/KingsleyYau/WeChatChannelsDownloader/blob/master/res.png?raw=true"/>

## Compile and Run
```bash
make && ./main

```