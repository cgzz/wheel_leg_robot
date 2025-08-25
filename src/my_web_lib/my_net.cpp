#include "my_net.h"

void my_net_init()
{
    my_wifi_init();     // 初始化网络
    my_web_asyn_init(); // 初始化网页
}

void my_net_update()
{
}