文件名				功能				debug 大小	release大小

ap_common.c		:	资源操作,其他函数		
ap_message.c		:	消息处理,热键处理		
ui_delete.c		:	删除文件控件			
ui_directory.c		:	目录浏览控间			
ui_main.c		:	主界面显示函数			
ui_menu.c		:	功能菜单控件			
ui_progress.c		:	进度条控件			
ui_slider.c		:	滑动杆控件			


//========= debug 模式
建议段大小: 4K
-b(CODE)COM_MAIN_D,COM_MAIN=605000,1000,00000
-b(CODE)COM_MSG,COM_CTRL,COM_RES=615000,1000,10000


//========= release 模式
在iar的c编译选项里#define NAPDEBUG
优化: -s9

建议段大小: 1.5 KBytes
-b(CODE)COM_MAIN_D,COM_MAIN=601400,0600,00000
-b(CODE)COM_MSG,COM_CTRL,COM_RES=611400,0600,10000


