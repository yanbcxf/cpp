========================================================================
    动态链接库：FfplaySdl2Pure 项目概述
========================================================================

FFMPEG中结构体很多。最关键的结构体可以分成以下几类： 
a) 解协议（http,rtsp,rtmp,mms） 
AVIOContext，URLProtocol，URLContext主要存储视音频使用的协议的类型以及状态。URLProtocol存储输入视音频使用的封装格式。每种协议都对应一个URLProtocol结构。（注意：FFMPEG中文件也被当做一种协议“file”） 

b) 解封装（flv,avi,rmvb,mp4） 
AVFormatContext主要存储视音频封装格式中包含的信息；AVInputFormat存储输入视音频使用的封装格式。每种视音频封装格式都对应一个AVInputFormat 结构。 

c) 解码（h264,mpeg2,aac,mp3） 
每个AVStream存储一个视频/音频流的相关数据；每个AVStream对应一个AVCodecContext，存储该视频/音频流使用解码方式的相关数据；每个AVCodecContext中对应一个AVCodec，包含该视频/音频对应的解码器。每种解码器都对应一个AVCodec结构。 

d) 存数据 
视频的话，每个结构一般是存一帧；音频可能有好几帧 解码前数据：AVPacket 解码后数据：AVFrame


========================================================================

我们发现在AVFrame中有一个比较重要的字段叫做nb_samples，这个字段名为采样数，此字段可以结合音频数据格式计算这个frame->data有多大，其实这个字段联合采样率还可以计算音频一秒有多少个packet。 
计算公式如下： num_pkt = 采样率 / nb_samples; 这样我们就知道了音频每秒的包数目（可以见到理解为帧），
有了此数据计算pts就和视频一模一样了， 
计算公式如下： 第n个包的pts = n * (（1 / timbase）/ num_pkt); 
很多音频时间基和采样率成倒数，那么根据公式我们的音频pts就可以很简单的以nb_samples递增了，如下： 
第一个包：pts = 0 * nb_samples; 
第二个包：pts = 1 * nb_samples; 
第三个包：pts = 2 * nb_samples;