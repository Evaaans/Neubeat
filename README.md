Neubeat
=======

The Jubeatist Project &lt;Neubeat> by Evaaans

百度Jubeat吧吧友Evaaans创建的跨平台练习器计划  
（For more details -> http://tieba.baidu.com/p/2908318494）  

由于UP主本人工作繁忙一直无暇维护，故于2014年12月开放源代码（MIT License）分享给有兴♂趣的朋友！  
游戏核心编写于2014年3月，使用引擎为Cocos2dx 3.0 rc0  
p.s. 由于引擎本身体积庞大，故本Repo中暂不含引擎请您自行到Cocos2dx官网下载  

注意事项
--------
由于引擎（3.0 rc0）自带的音频模块CocosDenshion较弱，不足以完成音乐游戏的开发需求  
UP主当时自行扩充了几个必要接口（SimpleAudioEngine.h）如下：  
1> 	virtual void setBackgroundMusicPosition(int position); // 即seek to  
2>	virtual int getBackgroundMusicPosition(); // 获取BGM当前播放位置  
3>  virtual int getBackgroundMusicLength(); // 获取BGM总长度  

以上几个接口在不同平台对应不同的底层实现，还需您下载Cocos2dx后自行完善，并不太复杂 ：）  
推荐使用跨平台兼容性较好的Fmod音频引擎  

免责声明
--------
本项目仅供爱好者学习交流使用，采用MIT协议即授予您最大的开发自由度  
但若投入盈利或其他原因引起的纠纷本人概不负责，请支持正版Jubeat  
