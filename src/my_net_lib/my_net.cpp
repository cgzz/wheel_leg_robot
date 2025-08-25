#include "my_net_config.h"

#include "my_motion.h"

void my_net_init()
{
    my_wifi_init();     // 初始化网络
    my_web_asyn_init(); // 初始化网页
}

void send_data_update()
{
}
void rece_data_update()
{
}
uint32_t my_net_update()
{
    send_data_update();
    uint32_t dt_ms = my_web_data_update();
    rece_data_update();
    return dt_ms;
}