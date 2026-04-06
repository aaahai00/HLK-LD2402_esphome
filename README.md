# esphome-HLK-LD2402
基础代码及实现来自[大佬分享](https://bbs.hassbian.com/thread-29879-1-1.html)  
本人仅进行少量修改以适配最新esphome

### 大概改动
~~AI写的，有bug我不管（doge)~~  
工程模式更新频率外显(现在可以直接在yaml编辑)  
距离传感器添加了一个抖动值，小于指定值则不进行上报  
考虑支持更复杂一点的功能（请别抱希望，新建文件夹）

### 配置文件相关
本项目有提供两个配置文件，建议使用带有en的文件进行使用，因为cn的传感器名称塞了一大堆零宽空格（为了防止转码后名称重复）  
如果有中文需求建议使用en文件交给AI，改成使用mqtt协议的就可以避免名称冲突了，如果不介意也可以使用，没什么致命错误（大概？）  
如果喜欢的话建议去[原帖](https://bbs.hassbian.com/thread-29879-1-1.htm)给大佬点个收藏点个赞  





## 鸣谢
### [大佬分享](https://bbs.hassbian.com/thread-29879-1-1.html)   [源头项目](https://github.com/Mc-Joung/hlk_ld2402_esphome/) ~~AI~~
