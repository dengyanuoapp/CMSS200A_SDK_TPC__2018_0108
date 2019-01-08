1、welcome代码的地址和大小
   起始地址必须是0xff9800，大小不得超过2KB，即地址范围是0xff9800~0xff9fff
   
2、图片资源
   为符合接口读取，打包时会对图片资源进行按512byte补齐。即1023byte打进固件会变成1024byte。
   
   
3、接口
   提供两个接口，一为获取显示buffer的起始地址和长度，另一个为按扇区读取logo数据到此buffer
   
   sGetLogoData?_
   参数：WR6 -- res资源扇区偏移， 0起始； R11 -- 读取的扇区个数
   返回： R11 -- 0 正确    


   sGetBuffInfo?_:
   参数：无
   返回： WR4 -- buffer地址； WR6 -- buffer长度