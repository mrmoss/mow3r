#include "motor.hpp"
#include "packet.hpp"
#include "relay.hpp"

motor_t rf(8,7);
motor_t rb(10,9);
motor_t lf(6,5);
motor_t lb(4,3);
relay_t blade_enable(11);
relay_t blade_spin(12);
packet_parser_t parser;

void setup()
{
    rf.reverse();
    rb.reverse();
    //rf.drive(60);
    //rb.drive(60);
    //blade_enable.set(true);
    //delay(150);
    //blade_spin.set(true);
    Serial.begin(115200);
}

void loop()
{
    uint8_t b;
    while(Serial.available()>0&&Serial.readBytes((char*)&b,1)==1)
    {
        if(parser.parse(b))
        {
            cmd_t cmd;
            if(parser.recv_cmd(cmd))
            {
                Serial.print(cmd.L);
                Serial.print(",");
                Serial.println(cmd.R);
            }
        }
    }
            
}
