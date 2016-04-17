/*
 * @file       rm_tx.h
 * @brief      Definitions used by rsync transmitter (A).
 * @author     Piotr Gregor <piotrek.gregor at gmail.com>
 * @version    0.1.2
 * @date       2 Jan 2016 11:18 AM
 * @copyright  LGPLv2.1
 */


#ifndef RSYNCME_TX_H
#define RSYNCME_TX_H


#include "rm.h"
#include "rm_rx.h"

/*
 * @brief   Locally sync files @x and @y such that
 *          @y becomes same as @x.
 * @param   flags: bits
 *          0: if set, create file @y if it doesn't exist
 *          1:
 *          2:
 *          3:
 *          4:
 *          5:
 *          6:
 *          7:
 * @return  0 on success, negative value otherwise:
 *          -1 couldn't open @x
 *          -2 @y doesn't exist and --force set but couldn't create @y
 *          -3 couldn't open @y and --force not set, what should I use?
 *          -4 couldn't stat @x
 *          -5 couldn't stat @y
 *          -6 internal error: in rm_rx_insert_nonoverlapping_ch_ch_local
 */
int
rm_tx_local_push(const char *x, const char *y,
			uint32_t L, rm_push_flags flags);

int
rm_tx_remote_push(const char *x, const char *y,
		struct sockaddr_in *remote_addr,
		uint32_t L);

#endif	// RSYNCME_TX_H
