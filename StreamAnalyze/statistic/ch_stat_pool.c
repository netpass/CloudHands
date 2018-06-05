/*
 *
 *      Filename: ch_stat_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-05 15:06:13
 * Last Modified: 2018-06-05 19:35:57
 */

#include "ch_stat_pool.h"
#include "ch_log.h"

#define STAT_ENTRY_COUNT(time_up,time_tv) ((time_up)/(time_tv))

static size_t _stat_msize_cal(uint64_t stat_time_up,uint64_t stat_time_tv){

	size_t msize = 0;
	size_t c = STAT_ENTRY_COUNT(stat_time_up,stat_time_tv);

	msize += sizeof(ch_stat_pool_hdr_t);
	msize += (sizeof(ch_stat_obj_hdr_t)*STAT_NUM);
	msize += (sizeof(ch_stat_entry_t)*c*STAT_NUM);

	msize += 64;

	return msize;
}

static void * _stat_obj_addr(ch_stat_pool_t *st_pool){

	void *addr;
	size_t esize = 0;

	ch_stat_mpool_t *st_mpool = &st_pool->st_mpool;
	ch_stat_pool_hdr_t *p_hdr = st_pool->p_hdr;


	esize = STAT_ENTRY_COUNT(p_hdr->stat_time_up,p_hdr->stat_time_tv)*sizeof(ch_stat_entry_t)+sizeof(ch_stat_obj_hdr_t);

	if(ch_stat_mpool_full(st_mpool,esize))
		return NULL;

	addr = ch_stat_mpool_pos(st_mpool);

	ch_stat_mpool_update(st_mpool,esize);

	return addr;
}

ch_stat_pool_t * ch_stat_pool_create(ch_pool_t *mp,const char *mmap_fname,
	uint64_t stat_time_up,uint64_t stat_time_tv){

	void *addr;
	int i;

	ch_stat_pool_t *st_pool = NULL;
	ch_stat_mpool_t *st_mpool;
	ch_stat_obj_t *stat_obj;

	st_pool = (ch_stat_pool_t *)ch_pcalloc(mp,sizeof(*st_pool));

	st_pool->mp = mp;

	st_mpool = &st_pool->st_mpool;

	/*load mmap memory pool*/
	if(ch_stat_mpool_init(st_mpool,mmap_fname,_stat_msize_cal(stat_time_up,stat_time_tv))){
		ch_log(CH_LOG_ERR,"Cannot load mmap memory pool!");
		return NULL;
	}

	st_pool->p_hdr = (ch_stat_pool_hdr_t*)ch_stat_mpool_alloc(st_mpool,sizeof(ch_stat_pool_hdr_t));

	if(st_mpool->is_new_created){
	
		st_pool->p_hdr->base_time = ch_get_current_timems()/1000;
		st_pool->p_hdr->stat_time_up = stat_time_up;
		st_pool->p_hdr->stat_time_tv = stat_time_tv;
	}

	for(i = 0;i<STAT_NUM;i++){
	
		stat_obj = &st_pool->stat_objs[i];

		addr = _stat_obj_addr(st_pool);
		if(addr == NULL){
		
			ch_log(CH_LOG_ERR,"No enough memory used to alloc stat object!");
			return NULL;
		}

		if(st_mpool->is_new_created)
			ch_stat_obj_init(stat_obj,addr,i,STAT_ENTRY_COUNT(stat_time_up,stat_time_tv));
		else
			ch_stat_obj_load(stat_obj,addr);
	}

	return st_pool;
}

void ch_stat_pool_handle(ch_stat_pool_t *st_pool,uint64_t time,uint64_t pkt_size,int pkt_type){


}


void ch_stat_pool_update(ch_stat_pool_t *st_pool){

	void *addr;
	void *npos;
	int i;
	ch_stat_obj_t *stat_obj;
	ch_stat_mpool_t *st_mpool = &st_pool->st_mpool;
	uint64_t time = ch_get_current_timems()/1000;
	ch_stat_pool_hdr_t *p_hdr = st_pool->p_hdr;

	if(time-p_hdr->base_time<p_hdr->stat_time_up){
	
		return;
	}

	/*reset*/
	p_hdr->base_time = time;
	npos = (void*)(st_pool->p_hdr+1);

	ch_stat_mpool_pos_set(st_mpool,npos);

	for(i = 0;i<STAT_NUM;i++){
	
		stat_obj = &st_pool->stat_objs[i];

		addr = _stat_obj_addr(st_pool);
		
		ch_stat_obj_reset(stat_obj,addr);
	}

}

