//
// Created by Tom on 25.06.2024.
//

#ifndef QMC5883L_H
#define QMC5883L_H

#include <cstdint>

#define QMC5883L_I2C_ADDR           0x0D

#define QMC5883L_X_LSB              0x00
#define QMC5883L_X_MSB              0x01
#define QMC5883L_Y_LSB              0x02
#define QMC5883L_Y_MSB              0x03
#define QMC5883L_Z_LSB              0x04
#define QMC5883L_Z_MSB              0x05
#define QMC5883L_STATUS             0x06
#define QMC5883L_TEMP_LSB           0x07
#define QMC5883L_TEMP_MSB           0x08
#define QMC5883L_CONTROL_1          0x09
#define QMC5883L_CONTROL_2          0x0A
#define QMC5883L_SET_RESET_PERIOD   0x0B
#define QMC5883L_CHIP_ID            0x0D

enum class OSR : uint8_t {
    OSR_512 = 0,
    OSR_256 = 1,
    OSR_128 = 2,
    OSR_64 = 3
};

enum class RNG : uint8_t {
    RNG_2G = 0,
    RNG_8G = 1
};

enum class ODR : uint8_t {
    ODR_10HZ = 0,
    ODR_50HZ = 1,
    ODR_100HZ = 2,
    ODR_200HZ = 3
};

enum class MODE : uint8_t {
    STANDBY = 0,
    CONTINUOUS = 1
};

struct qmc5883l_status{
    bool dor; // Data Skipped
    bool ovl; // Data Overflow
    bool drdy; // Data Ready
};
struct qmc5883l_config {
    explicit qmc5883l_config(OSR osr = OSR::OSR_512, RNG rng = RNG::RNG_2G, ODR odr = ODR::ODR_10HZ, MODE mode = MODE::STANDBY, bool rol_pnt = false, bool int_enb = false, uint8_t s_r_period = 0x01)
            : osr(osr), rng(rng), odr(odr), mode(mode), rol_pnt(rol_pnt), int_enb(int_enb), s_r_period(s_r_period) {}

    OSR osr; // Over sampling rate
    RNG rng; // Magnetic field measurement range or sensitivity of the sensors
    ODR odr; // Output data update rate
    MODE mode; // Operational mode
    bool rol_pnt; // Pointer roll-over enable
    bool int_enb; // Interrupt pin enable
    uint8_t s_r_period; // SET/RESET Period (to periodically re-calibrate the sensor)
};

class QMC5883L {
public:
    int init(uint8_t i2c_address = QMC5883L_I2C_ADDR);

    int read_chip_id(uint8_t *destination);

    int read_x(float *destination);
    int read_y(float *destination);
    int read_z(float *destination);
    int read_temp(float *destination);
    int read_status(qmc5883l_status *destination);
    int read_config(qmc5883l_config *destination);

    [[nodiscard]] int soft_reset() const;

    int set_config(qmc5883l_config *source) const;
private:
    uint8_t i2c_address;

    [[nodiscard]] int read_byte(uint8_t register_address, uint8_t *destination);
};

#endif // QMC5883L_H
