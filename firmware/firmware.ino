#include "motor.hpp"
#include "packet.hpp"
#include "relay.hpp"

motor_t rf(8,7);
motor_t rb(10,9);
motor_t lf(6,5);
motor_t lb(4,3);
relay_t blade_enable(11);
relay_t blade_spin(12);
packet_parser_t packet_parser;
uint32_t deadman_timeout=500;
uint32_t deadman_timer=millis()+deadman_timeout;

void handle_cmd(const packet_cmd_t& cmd)
{
    rf.drive(cmd.R);
    rb.drive(cmd.R);
    lf.drive(cmd.L);
    lb.drive(cmd.L);
    blade_enable.set(cmd.flags&FLAG_BLADE_ENABLE);
    blade_spin.set(cmd.flags&FLAG_BLADE_SPIN);
    deadman_timer=millis()+deadman_timeout;
}

void setup()
{
    rf.reverse();
    rb.reverse();
    handle_cmd({0,0,0});
    Serial.begin(115200);
}

void loop()
{
    if(millis()>deadman_timer)
    {
        handle_cmd({0,0,0});
        deadman_timer=millis()+deadman_timeout;
    }
    uint8_t b;
    while(Serial.available()>0&&Serial.readBytes((char*)&b,1)==1)
    {
        if(packet_parser.parse(b))
        {
            packet_cmd_t cmd;
            if(packet_parser.recv_cmd(cmd))
                handle_cmd(cmd);
        }
    }

}
