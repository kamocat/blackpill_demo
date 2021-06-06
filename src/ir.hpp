#include <cstdint>

class IR{
    public:
        IR(uint8_t pin, bool invert);
        IR(void);
        void tx(void);
        void tx(uint16_t msg);
     private:
        uint8_t head, a, b;
        uint8_t pin;
        uint8_t lo, hi;
        uint32_t pulse_hi = 700;
        uint32_t pulse0 = 390;
        uint32_t pulse1 = 1570;
        uint32_t blank = 4547;
        void send_byte(uint8_t b);
        void blanking_pulse(void);
        void send_once(void);
};