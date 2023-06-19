/* Copyright 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* I2C cross-platform code for Chrome EC */
#if 0
    #include "NuMicro.h"

    #include "common.h"
    #include "ec_i2c.h"
#else
    #include "utcpdlib.h"
#endif

const struct i2c_port_t *get_i2c_port(const int port)
{
    int i;
#ifdef SW

    /*
     * If the EC's I2C driver implementation is task event based and the
     * I2C is accessed before the task is initialized, it causes the system
     * panic hence these I2C will fall back to bitbang mode if enabled at
     * board level and will again switch back to event based I2C upon task
     * initialization.
     */
    if (task_start_called())
    {
        /* Find the matching port in i2c_ports[] table. */
        for (i = 0; i < i2c_ports_used; i++)
        {
            if (i2c_ports[i].port == port)
                return &i2c_ports[i];
        }
    }

    if (IS_ENABLED(CONFIG_I2C_BITBANG))
    {
        /* Find the matching port in i2c_bitbang_ports[] table. */
        for (i = 0; i < i2c_bitbang_ports_used; i++)
        {
            if (i2c_bitbang_ports[i].port == port)
                return &i2c_bitbang_ports[i];
        }
    }

#endif
    return NULL;
}


int i2c_xfer(const int port,
             const uint16_t addr_flags,
             const uint8_t *out, int out_size,
             uint8_t *in, int in_size)
{

    int rv;
#ifdef SW
    i2c_lock(port, 1);
    rv = i2c_xfer_unlocked(port, addr_flags,
                           out, out_size, in, in_size,
                           I2C_XFER_SINGLE);
    i2c_lock(port, 0);
#endif
    return rv;
}

void i2c_lock(int port, int lock)
{

}



/* sw */
int i2c_read32(const int port,
               const uint16_t addr_flags,
               int offset, uint32_t *data)
{
    uint32_t i;
    uint32_t BaseAddr;

    if (port == 0)
        BaseAddr = UTCPD0_BASE;
    else
        BaseAddr = UTCPD1_BASE;

    *data = inp32(BaseAddr + offset);
    return EC_SUCCESS;
}
#ifdef SW
/* Not use in UTCPD */
int i2c_write32(const int port,
                const uint16_t addr_flags,
                int offset, int data)
{
    uint8_t buf[1 + sizeof(uint32_t)];

    buf[0] = offset & 0xff;

    if (I2C_IS_BIG_ENDIAN(addr_flags))
    {
        buf[1] = (data >> 24) & 0xff;
        buf[2] = (data >> 16) & 0xff;
        buf[3] = (data >> 8) & 0xff;
        buf[4] = data & 0xff;
    }
    else
    {
        buf[1] = data & 0xff;
        buf[2] = (data >> 8) & 0xff;
        buf[3] = (data >> 16) & 0xff;
        buf[4] = (data >> 24) & 0xff;
    }

    return platform_ec_i2c_write(port, addr_flags, buf,
                                 sizeof(uint32_t) + 1);
}
#endif

int i2c_read16(const int port,
               const uint16_t addr_flags,
               int offset, int *data)
{
#ifdef SW
    int rv;
    uint8_t reg, buf[sizeof(uint16_t)];

    reg = offset & 0xff;
    /* I2C read 16-bit word: transmit 8-bit offset, and read 16bits */
    rv = platform_ec_i2c_read(port, addr_flags, reg, buf,
                              sizeof(uint16_t));

    if (rv)
        return rv;

    if (I2C_IS_BIG_ENDIAN(addr_flags))
        *data = ((int)buf[0] << 8) | buf[1];
    else
        *data = ((int)buf[1] << 8) | buf[0];

    return EC_SUCCESS;
#else
    uint32_t i;
    uint32_t BaseAddr;

    if (port == 0)
        BaseAddr = UTCPD0_BASE;
    else
        BaseAddr = UTCPD1_BASE;

    *data = inp16((BaseAddr + offset));
    return EC_SUCCESS;
#endif
}

int i2c_write16(const int port,
                const uint16_t addr_flags,
                int offset, int data)
{
    uint8_t buf[1 + sizeof(uint16_t)];
#ifdef SW
    buf[0] = offset & 0xff;

    if (I2C_IS_BIG_ENDIAN(addr_flags))
    {
        buf[1] = (data >> 8) & 0xff;
        buf[2] = data & 0xff;
    }
    else
    {
        buf[1] = data & 0xff;
        buf[2] = (data >> 8) & 0xff;
    }

    return platform_ec_i2c_write(port, addr_flags, buf,
                                 1 + sizeof(uint16_t));
#else
    uint32_t i;
    uint32_t BaseAddr;

    if (port == 0)
        BaseAddr = UTCPD0_BASE;
    else
        BaseAddr = UTCPD1_BASE;

    outp16(BaseAddr + offset, data & 0xFFFF);

    return EC_SUCCESS;
#endif
}

int i2c_read8(const int port,
              const uint16_t addr_flags,
              int offset, int *data)
{
    int rv;
    uint8_t reg = offset;
    uint8_t buf;
#ifdef SW
    reg = offset;

    rv = platform_ec_i2c_read(port, addr_flags, reg, &buf,
                              sizeof(uint8_t));

    if (!rv)
        *data = buf;

    return rv;
#else
    uint32_t i;
    uint32_t BaseAddr;

    if (port == 0)
        BaseAddr = UTCPD0_BASE;
    else
        BaseAddr = UTCPD1_BASE;

    *data = inp8(BaseAddr + offset);
    return EC_SUCCESS;
#endif
}

int i2c_write8(const int port,
               const uint16_t addr_flags,
               int offset, int data)
{
#ifdef SW
    uint8_t buf[2];
    buf[0] = offset;
    buf[1] = data;

    return platform_ec_i2c_write(port, addr_flags, buf, sizeof(buf));
#else
    uint32_t i;
    uint32_t BaseAddr;

    if (port == 0)
        BaseAddr = UTCPD0_BASE;
    else
        BaseAddr = UTCPD1_BASE;

    if (offset == 0xE0)
    {
        printf("data = 0x%x\n", data);
    }

    outp8((BaseAddr + offset), data & 0xFF);
    return EC_SUCCESS;
#endif
}
/*
port:        port_num
addr_flags:
offset:      register offset
mask:        write value
action:      action
*/

int i2c_update8(const int port,
                const uint16_t addr_flags,
                const int offset,
                const uint8_t mask,
                const enum mask_update_action action)
{
    int rv;
    int read_val;
    int write_val;
    //#ifdef SW
    rv = i2c_read8(port, addr_flags, offset, &read_val);

    if (rv)
        return rv;

    write_val = (action == MASK_SET) ? (read_val | mask)
                : (read_val & ~mask);

    //  if (IS_ENABLED(CONFIG_I2C_UPDATE_IF_CHANGED) && write_val == read_val)
    //      return EC_SUCCESS;

    return i2c_write8(port, addr_flags, offset, write_val);
    //#else
    //       return EC_SUCCESS;
    //#endif
}

int i2c_update16(const int port,
                 const uint16_t addr_flags,
                 const int offset,
                 const uint16_t mask,
                 const enum mask_update_action action)
{
    int rv;
    int read_val;
    int write_val;

    rv = i2c_read16(port, addr_flags, offset, &read_val);

    if (rv)
        return rv;

    write_val = (action == MASK_SET) ? (read_val | mask)
                : (read_val & ~mask);

    //  if (IS_ENABLED(CONFIG_I2C_UPDATE_IF_CHANGED) && write_val == read_val)
    //      return EC_SUCCESS;

    return i2c_write16(port, addr_flags, offset, write_val);

}
#ifdef SW
int i2c_field_update8(const int port,
                      const uint16_t addr_flags,
                      const int offset,
                      const uint8_t field_mask,
                      const uint8_t set_value)
{
    int rv;
    int read_val;
    int write_val;

    rv = i2c_read8(port, addr_flags, offset, &read_val);

    if (rv)
        return rv;

    write_val = (read_val & (~field_mask)) | set_value;

    //  if (IS_ENABLED(CONFIG_I2C_UPDATE_IF_CHANGED) && write_val == read_val)
    //      return EC_SUCCESS;

    return i2c_write8(port, addr_flags, offset, write_val);
}

int i2c_field_update16(const int port,
                       const uint16_t addr_flags,
                       const int offset,
                       const uint16_t field_mask,
                       const uint16_t set_value)
{
    int rv;
    int read_val;
    int write_val;

    rv = i2c_read16(port, addr_flags, offset, &read_val);

    if (rv)
        return rv;

    write_val = (read_val & (~field_mask)) | set_value;

    //  if (IS_ENABLED(CONFIG_I2C_UPDATE_IF_CHANGED) && write_val == read_val)
    //      return EC_SUCCESS;

    return i2c_write16(port, addr_flags, offset, write_val);

}


int i2c_read_offset16(const int port,
                      const uint16_t addr_flags,
                      uint16_t offset, int *data, int len)
{
    int rv;
    uint8_t buf[sizeof(uint16_t)], addr[sizeof(uint16_t)];

    if (len < 0 || len > 2)
        return EC_ERROR_INVAL;

    addr[0] = (offset >> 8) & 0xff;
    addr[1] = offset & 0xff;

    /* I2C read 16-bit word: transmit 16-bit offset, and read buffer */
    rv = i2c_xfer(port, addr_flags, addr, 2, buf, len);

    if (rv)
        return rv;

    if (len == 1)
    {
        *data = buf[0];
    }
    else
    {
        if (I2C_IS_BIG_ENDIAN(addr_flags))
            *data = ((int)buf[0] << 8) | buf[1];
        else
            *data = ((int)buf[1] << 8) | buf[0];
    }

    return EC_SUCCESS;

}

int i2c_write_offset16(const int port,
                       const uint16_t addr_flags,
                       uint16_t offset, int data, int len)
{
    uint8_t buf[2 + sizeof(uint16_t)];

    if (len < 0 || len > 2)
        return EC_ERROR_INVAL;

    buf[0] = (offset >> 8) & 0xff;
    buf[1] = offset & 0xff;

    if (len == 1)
    {
        buf[2] = data & 0xff;
    }
    else
    {
        if (I2C_IS_BIG_ENDIAN(addr_flags))
        {
            buf[2] = (data >> 8) & 0xff;
            buf[3] = data & 0xff;
        }
        else
        {
            buf[2] = data & 0xff;
            buf[3] = (data >> 8) & 0xff;
        }
    }

    return i2c_xfer(port, addr_flags, buf, 2 + len, NULL, 0);
}

int i2c_read_offset16_block(const int port,
                            const uint16_t addr_flags,
                            uint16_t offset, uint8_t *data, int len)
{
    uint8_t addr[sizeof(uint16_t)];

    addr[0] = (offset >> 8) & 0xff;
    addr[1] = offset & 0xff;

    return i2c_xfer(port, addr_flags, addr, 2, data, len);
}

int i2c_write_offset16_block(const int port,
                             const uint16_t addr_flags,
                             uint16_t offset, const uint8_t *data, int len)
{
    int rv;
    uint8_t addr[sizeof(uint16_t)];

    addr[0] = (offset >> 8) & 0xff;
    addr[1] = offset & 0xff;

    /*
     * Split into two transactions to avoid the stack space consumption of
     * appending the destination address with the data array.
     */
    i2c_lock(port, 1);
    rv = i2c_xfer_unlocked(port, addr_flags, addr, 2, NULL, 0,
                           I2C_XFER_START);

    if (!rv)
        rv = i2c_xfer_unlocked(port, addr_flags,
                               data, len, NULL, 0, I2C_XFER_STOP);

    i2c_lock(port, 0);

    return rv;
}


int i2c_read_string(const int port,
                    const uint16_t addr_flags,
                    int offset, uint8_t *data, int len)
{
    int i, rv;
    uint8_t reg, block_length;

    if (!IS_ENABLED(CONFIG_SMBUS_PEC) && I2C_USE_PEC(addr_flags))
        return EC_ERROR_UNIMPLEMENTED;

    reg = offset;
    i2c_lock(port, 1);

    for (i = 0; i <= CONFIG_I2C_NACK_RETRY_COUNT; i++)
    {
        int data_length;

        /*
         * Send device reg space offset, and read back block length.
         * Keep this session open without a stop.
         */
        rv = i2c_xfer_unlocked(port, addr_flags,
                               &reg, 1, &block_length, 1,
                               I2C_XFER_START);

        if (rv)
            continue;

        if (len && block_length > (len - 1))
            data_length = len - 1;
        else
            data_length = block_length;

        if (IS_ENABLED(CONFIG_SMBUS_PEC) &&
                I2C_USE_PEC(addr_flags))
        {
            uint8_t addr_8bit =
                I2C_STRIP_FLAGS(addr_flags) << 1;
            uint8_t out[3] = {addr_8bit, reg, addr_8bit | 1};
            uint8_t pec, pec_remote;

            rv = i2c_xfer_unlocked(port, addr_flags,
                                   0, 0, data, data_length, 0);
            data[data_length] = 0;

            if (rv)
                continue;

            pec = cros_crc8(out, sizeof(out));
            pec = cros_crc8_arg(&block_length, 1, pec);
            pec = cros_crc8_arg(data, data_length, pec);

            /* read all remaining bytes */
            block_length -= data_length;

            while (block_length)
            {
                uint8_t byte;

                rv = i2c_xfer_unlocked(port, addr_flags,
                                       NULL, 0, &byte, 1, 0);

                if (rv)
                    break;

                pec = cros_crc8_arg(&byte, 1, pec);
                --block_length;
            }

            if (rv)
                continue;

            rv = i2c_xfer_unlocked(port, addr_flags, NULL, 0,
                                   &pec_remote, 1, I2C_XFER_STOP);

            if (rv)
                continue;

            if (pec != pec_remote)
                rv = EC_ERROR_CRC;
        }
        else
        {
            rv = i2c_xfer_unlocked(port, addr_flags,
                                   0, 0, data, data_length,
                                   I2C_XFER_STOP);
            data[data_length] = 0;

            if (rv)
                continue;
        }

        /* execution reaches here implies rv=0, so we can exit now */
        break;
    }

    i2c_lock(port, 0);
    return rv;
}
#endif

int i2c_read_block(const int port, const uint16_t addr_flags, int offset,
                   uint8_t *data, int len)
{

#ifdef SW
    int rv;
    uint8_t reg_address = offset;
    rv = i2c_xfer(port, addr_flags, &reg_address, 1, data, len);
    return rv;
#else
    uint32_t i;
    uint32_t BaseAddr;

    if (port == 0)
        BaseAddr = UTCPD0_BASE;
    else
        BaseAddr = UTCPD1_BASE;

    for (i = 0; i < len; i = i + 1)
        * (data + i) = inp8((BaseAddr + offset) + i);

    return EC_SUCCESS;
#endif
}

int i2c_write_block(const int port,
                    const uint16_t addr_flags,
                    int offset, const uint8_t *data, int len)
{
#ifdef SW
    int i, rv;
    uint8_t reg_address = offset, pec = 0;

    if (!IS_ENABLED(CONFIG_SMBUS_PEC) && I2C_USE_PEC(addr_flags))
        return EC_ERROR_UNIMPLEMENTED;

    if (IS_ENABLED(CONFIG_SMBUS_PEC) && I2C_USE_PEC(addr_flags))
    {
        uint8_t addr_8bit = I2C_STRIP_FLAGS(addr_flags) << 1;

        pec = cros_crc8(&addr_8bit, sizeof(uint8_t));
        pec = cros_crc8_arg(data, len, pec);
    }

    /*
     * Split into two transactions to avoid the stack space consumption of
     * appending the destination address with the data array.
     */
    i2c_lock(port, 1);

    for (i = 0; i <= CONFIG_I2C_NACK_RETRY_COUNT; i++)
    {
        rv = i2c_xfer_unlocked(port, addr_flags,
                               &reg_address, 1, NULL, 0,
                               I2C_XFER_START);

        if (rv)
            continue;

        if (I2C_USE_PEC(addr_flags))
        {
            rv = i2c_xfer_unlocked(port, addr_flags,
                                   data, len, NULL, 0, 0);

            if (rv)
                continue;

            rv = i2c_xfer_unlocked(port, addr_flags,
                                   &pec, sizeof(uint8_t), NULL, 0,
                                   I2C_XFER_STOP);

            if (rv)
                continue;
        }
        else
        {
            rv = i2c_xfer_unlocked(port, addr_flags,
                                   data, len, NULL, 0,
                                   I2C_XFER_STOP);

            if (rv)
                continue;
        }

        /* execution reaches here implies rv=0, so we can exit now */
        break;
    }

    i2c_lock(port, 0);

    return rv;
#else
#if 1 //new
    uint32_t i;
    uint32_t BaseAddr;
    uint32_t u32data;

    if (port == 0)
        BaseAddr = UTCPD0_BASE;
    else
        BaseAddr = UTCPD1_BASE;

    for (i = 0; i < len; i = i + 4)
    {
#if 0    /* M23 APB didn't support byte access */
        outp8((BaseAddr + offset) + i, *(data + i));
#else
        u32data = *data | (*(data + 1) << 8) | (*(data + 2) << 16) | (*(data + 3) << 24);
        outp32((BaseAddr + offset) + i, u32data);
        data = data + 4;
#endif
    }

#else //old
    uint32_t i;
    uint32_t BaseAddr;
    uint32_t u32data;

    if (port == 0)
        BaseAddr = UTCPD0_BASE;
    else
        BaseAddr = UTCPD1_BASE;

    for (i = 0; i < len; i = i + 4)
    {
        u32data = *data | (*(data + 1) << 8) | (*(data + 2) << 16) | (*(data + 3) << 24);
        outp32((BaseAddr + offset) + i, u32data);
        data = data + 4;
    }

#endif
    return EC_SUCCESS;
#endif
}

