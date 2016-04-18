/*
 * @file        rm_session.h
 * @brief       Rsync session.
 * @author      Piotr Gregor <piotrek.gregor at gmail.com>
 * @version     0.1.2
 * @date        02 Nov 2016 04:08 PM
 * @copyright   LGPLv2.1
 */


#ifndef RSYNCME_SESSION_H
#define RSYNCME_SESSION_H

#include "rm_defs.h"
#include "twlist.h"


struct rm_session
{
	struct twhlist_node     hlink;  /* hashtable handle */
	struct twlist_head      link;   /* list handle */

	uint32_t                session_id;
	pthread_mutex_t         session_mutex;
	pthread_t               ch_ch_tid;
	pthread_t               delta_tid;

    /* receiver */
    struct twlist_head      rx_ch_ch_list;
    pthread_mutex_t         rx_ch_ch_list_mutex;

	struct rsyncme          *rm;    /* pointer to global
                                       rsyncme object */

    enum rm_session_type    type;
    void                    *prvt;
};

/* receiver of file (delta vector) */
struct rm_session_push_rx
{
    int     fd; /* socket handle */
};

/* receiver of file (delta vector) */
struct rm_session_pull_rx
{
    int     fd; /* socket handle */
};

/* @brief   Creates new session. */
struct rm_session *
rm_session_create(struct rsyncme *engine, enum rm_session_type t);

void
rm_session_free(struct rm_session *s);

/* @brief   Rx checksums calculated by receiver (B) on nonoverlapping
 * blocks. */
void *
rm_session_ch_ch_rx_f(void *arg);

/* @brief Tx delta reconstruction data. */
void *
rm_session_delta_tx_f(void *arg);

/* @brief  Tx checksums calculated by receiver (B) on nonoverlapping
 * blocks to sender (A). */
void *
rm_session_ch_ch_tx_f(void *arg);

/* @brief  Rx delta reconstruction data and do reconstruction. */
void *
rm_session_delta_rx_f(void *arg);


#endif  /* RSYNCME_SESSION_H */

