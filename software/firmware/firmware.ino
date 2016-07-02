#include "motor.hpp"
#include "packet.hpp"
#include "relay.hpp"

motor_t lf(12,11);
motor_t lb(10,9);
motor_t rf(6,5);
motor_t rb(8,7);
relay_t blade_enable(A0);
relay_t blade_spin(A1);
packet_parser_t packet_parser;
uint32_t heartbeat_timeout=100;
uint32_t heartbeat_timer=millis()+heartbeat_timeout;
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
    if(millis()>heartbeat_timer)
    {
        send_heartbeat();
        heartbeat_timer=millis()+heartbeat_timeout;
    }
    if(millis()>deadman_timer)
    {
        handle_cmd({0,0,0});
        deadman_timer=millis()+deadman_timeout;
    }
    uint8_t temp;
    while(Serial.available()>0&&Serial.readBytes((char*)&temp,1)==1)
    {
        if(packet_parser.parse(temp))
        {
            packet_cmd_t cmd;
            if(packet_parser.recv_cmd(cmd))
                handle_cmd(cmd);
        }
    }

}
