/*
 *
 *      Filename: ch_http_parser.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 17:06:30
 * Last Modified: 2018-05-18 18:11:51
 */

#include "ch_http_parser.h"
#include "ch_log.h"
#include "ch_shm_format.h"
#include "ch_packet_record.h"
#include "ch_http_session_entry.h"
#include "ch_http_session.h"
#include "ch_session_store.h"

#include "do_http_parse.c"
#include "do_http_format.c"
#include "do_http_create.c"

static ch_pp_parser_t http_pp_parser = {

	.pp_pool = NULL,
	.parse_context = NULL,
	.proto_id = PROTOCOL_HTTP,
	.pkt_rcd_type = PKT_RECORD_TYPE_TCP_HTTP,
	.session_entry_create = do_http_session_entry_create,
	.request_parse = do_http_request_parse,
	.response_parse = do_http_response_parse,
	.session_format = do_http_session_format,
	.session_entry_clean = do_http_session_entry_clean
};

int ch_http_parser_init(ch_pp_pool_t *pp_pool,const char *cfname){

	ch_http_parse_context_t *parse_context = (ch_http_parse_context_t*)ch_palloc(pp_pool->mp,sizeof(*parse_context));

	if(ch_buffer_init(&parse_context->g_buffer)){
	
		ch_log(CH_LOG_ERR,"Cannot create buffer for http parser!");
		return -1;
	}
	
	parse_context->hcontext = ch_http_context_create(pp_pool->pwork->pcontext,cfname);
	if(parse_context->hcontext == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot load http config file:%s",cfname);
		return -1;
	}

	parse_context->req_fpath = ch_fpath_create(pp_pool->mp,parse_context->hcontext->req_body_dir,
		parse_context->hcontext->create_body_dir_type,0,0);
	
	parse_context->res_fpath = ch_fpath_create(pp_pool->mp,parse_context->hcontext->res_body_dir,
		parse_context->hcontext->create_body_dir_type,0,0);

	if(parse_context->req_fpath == NULL || parse_context->res_fpath == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create file path for store http body!");
		return -1;
	}

	http_pp_parser.parse_context = parse_context;

	ch_pp_parser_register(pp_pool,&http_pp_parser);

	return 0;
}


