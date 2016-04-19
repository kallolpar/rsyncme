/*
 * @file        rm_tcp.c
 * @brief       Send data over TCP.
 * @author      Piotr Gregor <piotrek.gregor at gmail.com>
 * @version     0.1.2
 * @date        18 Apr 2016 00:45 AM
 * @copyright   LGPLv2.1
 */


#include "rm_tcp.h"
#include "rm.h"


ssize_t
rm_tcp_rx(int fd, void *buf, size_t bytes_n)
{
    size_t      bytes_read_total = 0;
    ssize_t     bytes_read;

    if (bytes_n == 0) return 0;

    while (bytes_read_total != bytes_n)
    {
        do
        {
            bytes_read = read(fd, buf + bytes_read_total,
                                (bytes_n - bytes_read_total));
        } while ((bytes_read == -1) && (errno == EINTR));

        if (bytes_read == -1)
        {
            /* Bad. */
            return -1;
        }
        bytes_read_total += bytes_read;
    }
    return bytes_read_total;
}

ssize_t
rm_tcp_tx(int fd, void *buf, size_t bytes_n)
{
    size_t      bytes_written_total = 0;
    ssize_t     bytes_written;

    while (bytes_written_total != bytes_n)
    {
        do
        {
            bytes_written = write(fd, buf + bytes_written_total,
                            (bytes_n - bytes_written_total));
        } while ((bytes_written == -1) && (errno == EINTR));

        if (bytes_written == -1)
        {
            /* Bad. */
            return -1;
        }
        bytes_written_total += bytes_written;
    }
    return bytes_written_total;
}

int
rm_tcp_tx_ch_ch_ref(int fd, const struct rm_ch_ch_ref *e)
{
    ssize_t bytes_written;
    unsigned char buf[RM_CH_CH_REF_SIZE], *pbuf;

    /* serialize data */
    pbuf = rm_serialize_u32(buf, e->ch_ch.f_ch);
    memcpy(pbuf, &e->ch_ch.s_ch, RM_STRONG_CHECK_BYTES);
    pbuf += RM_STRONG_CHECK_BYTES;
    pbuf = rm_serialize_size_t(pbuf, e->ref);

    /* tx over TCP connection */
    bytes_written = rm_tcp_tx(fd, buf, RM_CH_CH_REF_SIZE);
    if (bytes_written == -1 || (size_t)bytes_written != RM_CH_CH_REF_SIZE)
        return -1;
    return 0;
}
