#include "qmc5883l.h"
#include <hardware/i2c.h>

int QMC5883L::read_byte(uint8_t register_address, uint8_t *destination) {
    if (i2c_write_blocking(i2c_default, i2c_address, &register_address, 1, true) <= 0) {
        return PICO_ERROR_GENERIC;
    }
    if (i2c_read_blocking(i2c_default, i2c_address, destination, 1, false) <= 0) {
        return PICO_ERROR_GENERIC;
    }
    return PICO_OK;
}

int QMC5883L::init(uint8_t i2c_addr) {
    i2c_address = i2c_addr;

    qmc5883l_config config_values(
            OSR::OSR_512,   // Over sampling rate
            RNG::RNG_2G,    // Magnetic field measurement range
            ODR::ODR_200HZ, // Output data update rate
            MODE::CONTINUOUS, // Operational mode
            false, // Pointer roll-over disable
            false, // Interrupt pin disable
            0x00   // Example SET/RESET period
    );

    if (set_config(&config_values) != PICO_OK) {
        return PICO_ERROR_GENERIC;
    }

    sleep_ms(100);  // Delay to allow sensor to stabilize
    return PICO_OK;
}

int QMC5883L::read_chip_id(uint8_t *destination) {
    return read_byte(QMC5883L_CHIP_ID, destination);
}

int QMC5883L::read_x(float *destination) {
    uint8_t msb, lsb = 0;

    if (read_byte(QMC5883L_X_MSB, &msb) != PICO_OK) {
        return PICO_ERROR_GENERIC;
    }
    if (read_byte(QMC5883L_X_LSB, &lsb) != PICO_OK) {
        return PICO_ERROR_GENERIC;
    }

    auto x = static_cast<int16_t>((msb << 8) | lsb);
    *destination = static_cast<float>(x);

    return PICO_OK;
}

int QMC5883L::read_y(float *destination) {
    uint8_t msb, lsb = 0;

    if (read_byte(QMC5883L_Y_MSB, &msb) != PICO_OK) {
        return PICO_ERROR_GENERIC;
    }
    if (read_byte(QMC5883L_Y_LSB, &lsb) != PICO_OK) {
        return PICO_ERROR_GENERIC;
    }

    auto y = static_cast<int16_t>((msb << 8) | lsb);
    *destination = static_cast<float>(y);

    return PICO_OK;
}

int QMC5883L::read_z(float *destination) {
    uint8_t msb, lsb = 0;

    if (read_byte(QMC5883L_Z_MSB, &msb) != PICO_OK) {
        return PICO_ERROR_GENERIC;
    }
    if (read_byte(QMC5883L_Z_LSB, &lsb) != PICO_OK) {
        return PICO_ERROR_GENERIC;
    }

    auto z = static_cast<int16_t>((msb << 8) | lsb);
    *destination = static_cast<float>(z);

    return PICO_OK;
}

int QMC5883L::read_temp(float *destination) {
    uint8_t msb, lsb = 0;

    if (read_byte(QMC5883L_TEMP_MSB, &msb) != PICO_OK) {
        return PICO_ERROR_GENERIC;
    }
    if (read_byte(QMC5883L_TEMP_LSB, &lsb) != PICO_OK) {
        return PICO_ERROR_GENERIC;
    }

    auto temp = static_cast<int16_t>((msb << 8) | lsb);
    *destination = static_cast<float>(temp) / 100;

    return PICO_OK;
}

int QMC5883L::soft_reset() const {
    uint8_t source[2] = {
            QMC5883L_CONTROL_1,
            0x80
    };

    if (i2c_write_blocking(i2c_default, i2c_address, source, 2, false) <= 0) {
        return PICO_ERROR_GENERIC;
    }
    sleep_ms(100);  // Allow time for reset
    return PICO_OK;
}

int QMC5883L::set_config(qmc5883l_config *source) const {
    uint8_t data[2] = {
            QMC5883L_CONTROL_1,
            (static_cast<uint8_t>(source->osr) << 6) |
            (static_cast<uint8_t>(source->rng) << 4) |
            (static_cast<uint8_t>(source->odr) << 2) |
            static_cast<uint8_t>(source->mode)
    };

    if (i2c_write_blocking(i2c_default, i2c_address, data, 2, false) <= 0) {
        return PICO_ERROR_GENERIC;
    }

    data[0] = QMC5883L_CONTROL_2;
    data[1] = (source->rol_pnt ? 0x40 : 0x00) | (source->int_enb ? 0x01 : 0x00);

    if (i2c_write_blocking(i2c_default, i2c_address, data, 2, false) <= 0) {
        return PICO_ERROR_GENERIC;
    }

    data[0] = QMC5883L_SET_RESET_PERIOD;
    data[1] = source->s_r_period;

    if (i2c_write_blocking(i2c_default, i2c_address, data, 2, false) <= 0) {
        return PICO_ERROR_GENERIC;
    }

    return PICO_OK;
}

int QMC5883L::read_config(qmc5883l_config *destination){
    uint8_t control_1 = 0;
    if(read_byte(QMC5883L_CONTROL_1, &control_1) != PICO_OK){
        return PICO_ERROR_GENERIC;
    }
    uint8_t control_2 = 0;
    if(read_byte(QMC5883L_CONTROL_2, &control_2) != PICO_OK){
        return PICO_ERROR_GENERIC;
    }
    uint8_t srp = 0;
    if(read_byte(QMC5883L_SET_RESET_PERIOD, &srp) != PICO_OK){
        return PICO_ERROR_GENERIC;
    }

    destination->mode = static_cast<MODE>(control_1 & 0x3);
    destination->odr = static_cast<ODR>((control_1 >> 2) & 0x3);
    destination->rng = static_cast<RNG>((control_1 >> 4) & 0x3);
    destination->osr = static_cast<OSR>((control_1 >> 6) & 0x3);

    destination->int_enb = control_2 & 0x1;
    destination->rol_pnt = control_2 & 0x40;
    destination->s_r_period = srp;
    return PICO_OK;
}

int QMC5883L::read_status(qmc5883l_status *destination) {
    uint8_t status = 0;
    if(read_byte(QMC5883L_STATUS, &status) != PICO_OK){
        return PICO_ERROR_GENERIC;
    }

    destination->dor = status & 0x4;
    destination->ovl = status & 0x2;
    destination->drdy = status & 0x1;

    return PICO_OK;
}
