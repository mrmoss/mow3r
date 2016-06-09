#ifndef MOTOR_HPP
#define MOTOR_HPP

class motor_t
{
    public:
        motor_t(const uint8_t dir_pin,const uint8_t pwm_pin):
            dir_pin_m(dir_pin),pwm_pin_m(pwm_pin),forward_m(HIGH)
        {
            stop();
        }

        void reverse()
        {
            forward_m=!forward_m;
        }

        void drive(const int16_t speed)
        {
            drive(abs(speed),speed>=0);
        }

        void drive(const uint8_t speed,const bool dir)
        {
            digitalWrite(dir_pin_m,((forward_m&&speed>=0)||(!forward_m&&!speed>=0)));
            analogWrite(pwm_pin_m,speed);
        }

        void stop()
        {
            digitalWrite(dir_pin_m,forward_m);
            analogWrite(pwm_pin_m,0);
        }

    private:
        uint8_t dir_pin_m;
        uint8_t pwm_pin_m;
        bool forward_m;
};

#endif
