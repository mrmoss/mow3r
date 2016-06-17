#ifndef RELAY_HPP
#define RELAY_HPP

class relay_t
{
    public:
        relay_t(const uint8_t pin):pin_m(pin)
        {
            pinMode(pin_m,OUTPUT);
            digitalWrite(pin_m,HIGH);
        }

        void set(const bool active)
        {
            digitalWrite(pin_m,!active);
        }

    private:
        uint8_t pin_m;
};

#endif
