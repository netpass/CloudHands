/*
 *
 *      Filename: ch_dns_rdata_opt.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_OPT_H
#define CH_DNS_RDATA_OPT_H

typedef struct ch_dns_rdata_opt_t ch_dns_rdata_opt_t;
typedef struct ch_dns_rdata_opt_entry_t ch_dns_rdata_opt_entry_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_list.h"

struct ch_dns_rdata_opt_t {

	ch_dns_rdata_t rdata;

	struct list_head entries;

	uint16_t entries_n;

};

struct ch_dns_rdata_opt_entry_t {

	struct list_head node;

	uint16_t code;
	uint16_t  dlen;
	unsigned char *data;

};

extern void ch_dns_rdata_opt_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_OPT_H*/
