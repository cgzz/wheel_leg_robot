extern bool key1;
extern float send1;
extern float send2;
extern float send3;
// extern float joyX;
// extern float joyY;

struct Telemetry
{
    float yaw, pitch, roll;
    float err_pos, err_spd, err_ang;
    float wel_spd1, wel_spd2;
    float tor_base, tor_yaw, tor_vel;
};

void my_udp_init();
void my_udp_send();

void my_udp_receive();
