/*****************************************************************************************************************************
This project was supported by the National Basic Research 973 Program of China under Grant No.2011CB302301
Huazhong University of Science and Technology (HUST)   Wuhan National Laboratory for Optoelectronics

FileName： ssd.h
Author: Hu Yang		Version: 2.1	Date:2011/12/02
Description: 

History:
<contributor>     <time>        <version>       <desc>                   <e-mail>
Yang Hu	        2009/09/25	      1.0		    Creat SSDsim       yanghu@foxmail.com
                2010/05/01        2.x           Change 
Zhiming Zhu     2011/07/01        2.0           Change               812839842@qq.com
Shuangwu Zhang  2011/11/01        2.1           Change               820876427@qq.com
Chao Ren        2011/07/01        2.0           Change               529517386@qq.com
Hao Luo         2011/01/01        2.0           Change               luohao135680@gmail.com
*****************************************************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include "initialize.h"
#include "flash.h"
#include "pagemap.h"

//lxc for compression
#define MAPPINGTABLE_READ 2
#define MAPPINGTABLE_WRITE 3

// lxc for compression
#define MAPPING_ENTRY_SIZE 8 //2*4 bytes
#define NUM_ENTRIES_PER_DIR 512
#define MAX_CACHED_DIR_SLOTS_SIZE  40000  //1048576 //1MB= 1048576    here is 10MB, 20MB is 2097152
#define MAX_CACHED_ENTRIES_SIZE 40000  //1048576 //1MB= 1048576=131072 numbers of entries    here is 10MB, 20MB is 2097152



//lxc for compression
#include "debug.h"
#include"bi_list.h"
#include"dictionary.h"
#include"diffXmatch.h"
#include"js_list.h"
#include"xmatchPro.h"
#define COMPRESSION_MAPPING // compression macro definition
#define COMPRESSION_COST_TIME_ONE_PAGE 100 // 100ns
#define DFTL // demand-based FTL
//#define PFTL // normal page FTL
//#define ENTRY_UNIT // the granualarity of entry in DFTL_buffer
#define TRANSLATION_PAGE_UNIT //the granularity of translation page in DFTL_buffer

#define COMP_VS_UNCOMP_IN_DFTL_BUFFER 0.1 //this is the propotion of uncompression in 
#define COMP_MAX_CACHED_DIR_SLOTS_SIZEIN_BYTE 40000 //As the compression, so there is no any limitation for the size.
#define UNCOMP_MAX_CACHED_ENTRY_SIZEIN_BYTE 2048 //should be MAPPING_ENTRY_SIZE's integer multiple.



#define COMP_PAGE_NUMBER_ONCE  4
#define SET_INTO_LIMITTED_BUFFER // this is to set the normal pagemaping into DFTL mapping.
#define MAX_INT64  0x7fffffffffffffffll

//lxcv1
//#define DEBUGONLYGC //open for debug
//#define DEBUG //open for debug
//#define DEBUGQUEUE //open for monitering queue length and head state.
//#define DEBUGSUSPEND //open for suspend debug
//#define DEBUGTRACE_OUT //open for trace_out function debug
//#define DEBUG_CURR_TIME //open for debug for ssd_current_time
void printfsub_request_status(struct ssd_info *ssd);
void printf_ssd_request_queue(struct ssd_info *ssd);
//lxcprogram
void printf_ssd_gc_node(struct ssd_info *ssd);
//lxcprogram_gc
int judging_read_in_gc_chip(struct ssd_info *ssd, int channel);
int printf_every_chip_read_subrequest(struct ssd_info *ssd);
int printf_every_chip_static_subrequest(struct ssd_info *ssd);
//lxcprogram_de-prioritize
int calc_req_slack_time(struct request * req);
//lxcprogram_GC
int printf_gc_node_information(struct gc_operation* gc_node);
    


//lxcprogram_for_adjust_write_suspension_number
int statistic_write_request_number(struct ssd_info *ssd);
int no_greedy_RF_suspension(struct ssd_info *ssd);

//lxc for compression 
int flush_mappingtable_into_files(struct ssd_info *ssd, char* filename);
struct ssd_info *simulate(struct ssd_info *);

int get_requests(struct ssd_info *);

struct ssd_info *buffer_management(struct ssd_info *);

//lxc for compression

struct ssd_info *DFTL_buffer_management(struct ssd_info * ssd);
//struct ssd_info *COMPRESSION_DFTL_buffer_management(struct ssd_info * ssd);
//only get read and write requests's need_distr_flag, not do any creat_sub_requests here !!!


unsigned int lpn2ppn(struct ssd_info *, unsigned int lsn);

struct ssd_info *distribute(struct ssd_info *);
//lxc for compression
int delete_evict_subrequest_from_this_req(struct ssd_info* ssd);
int polling_whether_finish_evict_subrequests(struct ssd_info* ssd);
struct ssd_info* create_evict_mappingtable_subrequests(struct ssd_info* ssd);
int delete_subs_in_this_req(struct ssd_info* ssd, struct request* req);
int delete_subs_bilist_in_this_req(struct request* req, struct _bi_list* should_evict_lpn_data);
int Pop_DFTL_buffer(struct ssd_info* ssd, struct request* req);
struct ssd_info* create_fetch_mappingtable_subrequests(struct ssd_info* ssd);
int polling_whether_finish_fetch_subrequests(struct ssd_info* ssd);
int delete_fetch_subrequest_from_this_req(struct ssd_info* ssd);
int insert2DFTL_buffer(struct ssd_info* ssd, struct request* req);  
int get_realsize_inbyte_byposition_in_mt(struct ssd_info* ssd, int lpn);// return real size
void delete_request_without_subs(struct request* req);

void If_hitInBUffer_move_theNode_ahead_in_readrequest(struct ssd_info* ssd, struct buffer_info* buffer, struct buffer_group* buffer_node);
void If_hitInBUffer_move_theNode_ahead_in_writerequest(struct ssd_info* ssd, struct buffer_info* buffer, struct buffer_group* buffer_node);

struct ssd_info* COMPRESSION_DFTL_deal_in_distributing_subrequests(struct ssd_info* ssd);
struct ssd_info* DFTL_deal_in_distributing_subrequests(struct ssd_info* ssd);
struct ssd_info *DFTL_distribute(struct ssd_info *);

int append_on_request_compression_to_uncompression_lpn_data(struct request* req, int lpn, int state);
int append_on_request_missing_in_data_buffer_lpn_data(struct request* req, int lpn, int state);
int append_on_request_missing_in_DFTL_buffer_lpn_data(struct request* req, int lpn, int state);
struct ssd_info* write_request_get_missing_in_databuffer(struct ssd_info* ssd, unsigned int lpn, int state, struct request* req);

struct ssd_info* value_request_DFTL_parameters_for_distribute_subrequests(struct ssd_info* ssd);
void read_value_request_DFTL_parameters(struct ssd_info* ssd, struct request* req);
void write_value_request_DFTL_parameters(struct ssd_info* ssd, struct request* req);
int get_translation_number_by_data_lpn(int data_lpn);
struct buffer_group* node_isnot_in_DFTL_buffer(struct ssd_info* ssd, int lpn);

int append_on_request_evict_in_DFTL_buffer_lpn_data(struct request* req, int lpn, int state);
//int get_translation_page_number(int* translation_page_temp, int i, int temp_node_lpn);

int compare_DFTL_buffer_remainder_space_with_missing_in_DFTL_buffer(struct ssd_info* ssd,  struct request* request);
// lxc for compression for COMPRESSION two buffers

struct buffer_group* node_isnot_in_two_DIFF_DFTL_buffer(struct ssd_info* ssd, int lpn, int* sign);
void COMPRESSION_write_value_request_DFTL_parameters(struct ssd_info* ssd, struct request* req);
void COMPRESSION_read_value_request_DFTL_parameters(struct ssd_info* ssd, struct request* req);
int compression_compare_DFTL_buffer_remainder_space_with_compression_area_coming(struct ssd_info* ssd,  struct request* request);
void get_compression_translation_page_size(struct ssd_info* ssd);
int get_size_from_compression_to_uncompression(struct ssd_info* ssd, struct request* request);
void make_real_evict_pages_for_compression_part(struct ssd_info* ssd, struct request* request, int should_evict_size);
int get_uncompression_tail_page_compressed_size(struct ssd_info* ssd, int off_set);
int move_pages_from_uncompression_to_compression(struct ssd_info* ssd, struct request* request);
int insert2DFTL_UNCOM_buffer(struct ssd_info* ssd, struct request* req);
void printf_read_request_need_distr_flag_value_and_illustion(struct ssd_info* ssd, struct request* req);
void printf_write_request_need_distr_flag_value_and_illustion(struct ssd_info* ssd, struct request* req);
void printf_requests_basic_paramters_value (struct ssd_info* ssd,struct request* new_request);
void printf_mappingtable_relevant_subrequest_in_different_process(struct ssd_info* ssd, struct request* req);
void printf_one_request_subrequests(struct ssd_info* ssd, struct request* request);
void test_finish_deletion(struct ssd_info* ssd);
void get_no_repeat_pages_number_from_lpns(struct ssd_info* ssd, struct request* req,struct _bi_list*  temp_bilist, int* input);





void trace_output(struct ssd_info *);

void statistic_output(struct ssd_info *);

unsigned int size(unsigned int);

unsigned int transfer_size(struct ssd_info *, int, unsigned int, struct request *);

int64_t find_nearest_event(struct ssd_info *);

void free_all_node(struct ssd_info *);

struct ssd_info *make_aged(struct ssd_info *);

struct ssd_info *no_buffer_distribute(struct ssd_info *);
