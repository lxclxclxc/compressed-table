/********************************************************************************************************************************
This project was supported by the National Basic Research 973 Program of China under Grant No.2011CB302301
Huazhong University of Science and Technology (HUST)   Wuhan National Laboratory for Optoelectronics

FileName： ssd.c
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
#include "ssd.h"
#define WRITE_RATIO_IN_REQUEST_QUEUE  0.3

/*************************get mapping table********************************/
int flush_mappingtable_into_files(struct ssd_info *ssd, char* filename)
{
    long int page_numbers=0;
    struct dram_info *dram = ssd->dram;

    int temp_i = 0;



    FILE* fp = fopen(filename, "w");


    int page_num = ssd->parameter->page_block * ssd->parameter->block_plane * ssd->parameter->plane_die * ssd->parameter->die_chip * ssd->parameter->chip_num;

    printf("page_num is %d\n", page_num);

    struct entry *map_entry;            //该项是映射表结构体指针,each entry indicate a mapping information

    unsigned int pn;                //物理号，既可以表示物理页号，也可以表示物理子页号，也可以表示物理块号

    for (temp_i = 0; temp_i < page_num; temp_i ++){

        fprintf(fp, "%d ", dram->map->map_entry[temp_i].pn);  

        //if(dram->map->map_entry[temp_i].pn == 0){
        //printf("all 0");
        //}
        struct local* location = NULL;
        if(dram->map->map_entry[temp_i].pn != 0){
            printf("lpn %d is ppn %d\n ", temp_i, dram->map->map_entry[temp_i].pn);

          location = find_location(ssd, dram->map->map_entry[temp_i].pn);
          ssdsim_msg("channel %d, chip %d,  die %d, plane %d, block %d, page %d", location->channel, location->chip, location->die, location->plane, location->block, location->page);

        }

    }


    fclose(fp);

}//end of flush_mappingtable_into_files


















/*************************get mapping table********************************/



int printf_gc_node_information(struct gc_operation* gc_node)
{

//    printf(" befor service gc, gc_node information is gc_node, chip %d, die %d, plane %d,  gc_node->victim_block_number is %d, gc copy_page_number is %d, copy_valid_number is %d, pos in copy is %d,  read %d, write %d, erase %d, read_inner %d, write inner %d, read_write_end %d, last_page_in_victim %d, victim_block_freepage %d, chip_status %d, sign for_preemptive %d, chip_next_predict_time %lld\n", gc_node->chip, gc_node->die, gc_node->plane, gc_node->victim_block_number,gc_node->copy_page_number, gc_node->copy_valid_number, gc_node->pos_in_copy, gc_node->read, gc_node->write, gc_node->erase, gc_node->gc_read_inner, gc_node->gc_write_inner, gc_node->read_write_end, gc_node->last_page_in_victim_deal, gc_node->victim_block_freepage_statistic, gc_node->chip_status, gc_node->sign_for_preemptive, gc_node->chip_next_predict_time_diff);

}




int printf_every_chip_static_subrequest(struct ssd_info *ssd)
{
    unsigned int tmp_channel,tmp_chip, i, j;
    unsigned int temp_chip_read_queue_length[100][100];
    unsigned int temp_chip_write_queue_length[100][100];
    struct sub_request *sub_for_queue = NULL;
    struct sub_request *sub_for_read = NULL;
    struct sub_request *sub_for_write = NULL;
#ifdef DEBUG
    //    printf("this simulator has %d channel, and %d chips in each channel, total chips is %d.\n",ssd->parameter->channel_number, ssd->channel_head[i].chip, ssd->parameter->chip_num);
#endif

    for(i = 0; i < 100; i++ ){
        for(j = 0; j < 100; j++ ){
            temp_chip_read_queue_length[i][j] = 0;
            temp_chip_write_queue_length[i][j] = 0;
        }
    }
    //   printf("initial ok\n");
    for(i = 0; i < ssd->parameter->channel_number; i++ ){
        //for read request count here.
        sub_for_read = ssd->channel_head[i].subs_r_head;
        while(sub_for_read != NULL){
            tmp_channel = sub_for_read->location->channel;
            tmp_chip = sub_for_read->location->chip;
            temp_chip_read_queue_length[tmp_channel][tmp_chip] ++;


            sub_for_read = sub_for_read->next_node;

        }//end of read while.

        //for write request count here.
        sub_for_write = ssd->channel_head[i].subs_w_head;
        while(sub_for_write != NULL){
#ifdef DEBUG
            //printf("sub_write address %lld\n",sub_for_write);
#endif


            tmp_channel = sub_for_write->location->channel;
            tmp_chip = sub_for_write->location->chip;
            temp_chip_write_queue_length[tmp_channel][tmp_chip] ++;


            sub_for_write = sub_for_write->next_node;

        }//end of write while.

    }//end of channel loop
    tmp_channel = ssd->parameter->channel_number;
    tmp_chip = ssd->parameter->chip_num / ssd->parameter->channel_number;

    for(i = 0; i < tmp_channel; i++ ){
#ifdef DEBUG  
        printf("channel %d 's sub_r_tail address is %lld\n", i, ssd->channel_head[i].subs_r_tail);
        printf("channel %d 's sub_r_head address is %lld\n", i, ssd->channel_head[i].subs_r_head);
        printf("channel %d 's sub_w_head address is %lld\n", i, ssd->channel_head[i].subs_w_head);
        printf("channel %d 's sub_w_tail address is %lld\n", i, ssd->channel_head[i].subs_w_tail);
#endif
        for(j = 0; j < tmp_chip; j++ ){
        //    printf("collection1 channel %d, chip %d, has %d read sub-requests.\n",i,j, temp_chip_read_queue_length[i][j]);
        //    printf("collection1 channel %d, chip %d, has %d write sub-requests.\n",i,j, temp_chip_write_queue_length[i][j]);

            printf("0, %d, %d, %d\n", i, j, temp_chip_write_queue_length[i][j]);
            printf("1, %d, %d, %d\n", i, j, temp_chip_read_queue_length[i][j]);


#ifdef DEBUGQUEUE
//            printf("collection1 channel %d, chip %d, has %d read sub-requests.\n",i,j, temp_chip_read_queue_length[i][j]);
//            printf("collection1 channel %d, chip %d, has %d write sub-requests.\n",i,j, temp_chip_write_queue_length[i][j]);
//            printf("channel %d, chip %d, now,num_w_cycle %d \n",i,j, ssd->channel_head[i].chip_head[j].num_w_cycle);
//            printf("channel %d, chip %d, chip_read_length  %d \n",i,j, ssd->channel_head[i].chip_head[j].chip_read_queue_length);
//            printf("channel %d, chip %d, chip_write_length  %d \n",i,j, ssd->channel_head[i].chip_head[j].chip_write_queue_length);



            //*****************here only for head of queue's state.***********//
            sub_for_queue = ssd->channel_head[i].subs_r_head;

            while(sub_for_queue != NULL){
                if(sub_for_queue->location->chip == j){
                    break;
                }
                sub_for_queue = sub_for_queue->next_node;
            }
            if(sub_for_queue != NULL){
                printf("collection2 channel %d, chip %d, first head sub_read_request state %d\n", i, j, sub_for_queue->current_state);
            }else {
                printf("collection2 channel %d, chip %d, has no sub_read_request now\n", i, j);
            }


            sub_for_queue = ssd->channel_head[i].subs_w_head;

            while(sub_for_queue != NULL){
                if(sub_for_queue->location->chip == j){
                    break;
                }
                sub_for_queue = sub_for_queue->next_node;
            }
            if(sub_for_queue != NULL){
                printf("collection2 channel %d, chip %d, first head sub_write_request state %d\n", i, j, sub_for_queue->current_state);
            }else {
                printf("collection2 channel %d, chip %d, has no sub_write_request now\n", i, j);
            }

            //*****************here only for head of queue's state.***********//
#endif
            if(temp_chip_read_queue_length[i][j] != ssd->channel_head[i].chip_head[j].chip_read_queue_length) {
                printf("lxc,read length is wrong here\n");
            }
            if(temp_chip_write_queue_length[i][j] != ssd->channel_head[i].chip_head[j].chip_write_queue_length) {
                printf("lxc,write length is wrong here\n");
            }
        }
    }
}


int printf_every_chip_read_subrequest(struct ssd_info *ssd)
{
    unsigned int tmp_channel,tmp_chip, i, j;
    unsigned int temp_chip_read_queue_length[100][100];
    unsigned int temp_chip_write_queue_length[100][100];
    struct sub_request *sub_for_queue = NULL;
    struct sub_request *sub_for_read = NULL;
    struct sub_request *sub_for_write = NULL;
#ifdef DEBUG
    //    printf("this simulator has %d channel, and %d chips in each channel, total chips is %d.\n",ssd->parameter->channel_number, ssd->channel_head[i].chip, ssd->parameter->chip_num);
#endif

    for(i = 0; i < 100; i++ ){
        for(j = 0; j < 100; j++ ){
            temp_chip_read_queue_length[i][j] = 0;
            temp_chip_write_queue_length[i][j] = 0;
        }
    }
    //   printf("initial ok\n");
    for(i = 0; i < ssd->parameter->channel_number; i++ ){
        //for read request count here.
        sub_for_read = ssd->channel_head[i].subs_r_head;
        while(sub_for_read != NULL){
            tmp_channel = sub_for_read->location->channel;
            tmp_chip = sub_for_read->location->chip;
            temp_chip_read_queue_length[tmp_channel][tmp_chip] ++;


            sub_for_read = sub_for_read->next_node;

        }//end of read while.

        //for write request count here.
        sub_for_write = ssd->channel_head[i].subs_w_head;
        while(sub_for_write != NULL){

            //printf("sub_write address %lld\n",sub_for_write);



            tmp_channel = sub_for_write->location->channel;
            tmp_chip = sub_for_write->location->chip;
            temp_chip_write_queue_length[tmp_channel][tmp_chip] ++;


            sub_for_write = sub_for_write->next_node;

        }//end of write while.

    }//end of channel loop
    tmp_channel = ssd->parameter->channel_number;
    tmp_chip = ssd->parameter->chip_num / ssd->parameter->channel_number;

    for(i = 0; i < tmp_channel; i++ ){
#ifdef DEBUG  
        printf("channel %d 's sub_r_tail address is %lld\n", i, ssd->channel_head[i].subs_r_tail);
        printf("channel %d 's sub_r_head address is %lld\n", i, ssd->channel_head[i].subs_r_head);
        printf("channel %d 's sub_w_head address is %lld\n", i, ssd->channel_head[i].subs_w_head);
        printf("channel %d 's sub_w_tail address is %lld\n", i, ssd->channel_head[i].subs_w_tail);
#endif
        for(j = 0; j < tmp_chip; j++ ){
//#ifdef DEBUGQUEUE
            if(temp_chip_read_queue_length[i][j] > 0){
                //printf("channel %d, chip %d, has %d read sub-requests.\n",i,j, temp_chip_read_queue_length[i][j]);
              //  printf("channel %d, chip %d, has %d write sub-requests.\n",i,j, temp_chip_write_queue_length[i][j]);
            //    printf("channel %d, chip %d, now,num_w_cycle %d \n",i,j, ssd->channel_head[i].chip_head[j].num_w_cycle);
               // printf("channel %d, chip %d, chip_read_length  %d \n",i,j, ssd->channel_head[i].chip_head[j].chip_read_queue_length);
            }
//            //*****************here only for head of queue's state.***********//
//            sub_for_queue = ssd->channel_head[i].subs_r_head;
//
//            while(sub_for_queue != NULL){
//                if(sub_for_queue->location->chip == j){
//                    break;
//                }
//                sub_for_queue = sub_for_queue->next_node;
//            }
//            if(sub_for_queue != NULL){
//                printf("channel %d, chip %d, first head sub_read_request state %d\n", i, j, sub_for_queue->current_state);
//            }else {
//                printf("channel %d, chip %d, has no sub_read_request now\n", i, j);
//            }
//
//
//            sub_for_queue = ssd->channel_head[i].subs_w_head;
//
//            while(sub_for_queue != NULL){
//                if(sub_for_queue->location->chip == j){
//                    break;
//                }
//                sub_for_queue = sub_for_queue->next_node;
//            }
//            if(sub_for_queue != NULL){
//                printf("channel %d, chip %d, first head sub_write_request state %d\n", i, j, sub_for_queue->current_state);
//            }else {
//                printf("channel %d, chip %d, has no sub_write_request now\n", i, j);
//            }
//
            //*****************here only for head of queue's state.***********//
//#endif
          //  printf("channel %d, chip %d, chip_write_length  %d \n",i,j, ssd->channel_head[i].chip_head[j].chip_write_queue_length);
            if(temp_chip_read_queue_length[i][j] != ssd->channel_head[i].chip_head[j].chip_read_queue_length) {
                printf("lxc,read length is wrong here, new temp length is %d. write in old chip is %d \n",temp_chip_read_queue_length[i][j], ssd->channel_head[i].chip_head[j].chip_read_queue_length);
            }
            if(temp_chip_write_queue_length[i][j] != ssd->channel_head[i].chip_head[j].chip_write_queue_length) {
    //            printf("lxc,write length is wrong here\n");

                printf("lxc,write length is wrong here, new temp length is %d. write in old chip is %d \n",temp_chip_write_queue_length[i][j], ssd->channel_head[i].chip_head[j].chip_write_queue_length);
            }
        }
    }
}




int judging_read_in_gc_chip(struct ssd_info *ssd, int channel)
{   unsigned int tmp_chip;
    struct gc_operation *gc_node;
    struct sub_request *sub_for_read = NULL;
    if(ssd->channel_head[channel].gc_command != NULL){
//        printf("now judging gc, channel is %d\n", channel);
        tmp_chip = ssd->channel_head[channel].gc_command->chip;// because now channel GC only has one GC chip once time.
        sub_for_read = ssd->channel_head[channel].subs_r_head;

        while(sub_for_read != NULL){
            if (sub_for_read->location->chip == tmp_chip){

  //      printf("now has read, so begin interrupt,!!!!!\n");
                return 0;

            } 
            sub_for_read = sub_for_read->next_node;

        }
        if(sub_for_read == NULL){

//        printf("now no read, jumpout from judging\n");
            return 1;
        }
    }
}



void printf_ssd_gc_node(struct ssd_info *ssd)
{
//    struct gc_operation *gc_node = NULL;
//    int i =0;
//    for(i = 0; i < ssd->parameter->channel_number; i++ ){
//    gc_node = ssd->channel_head[i].gc_command;
//#ifdef DEBUG
//printf("now is in gc_node information\n");
//#endif
//if(gc_node == NULL ){
//    continue;
//
//}
//    while(gc_node != NULL){
//        printf("gc_request %d,gc_node chip %d ,die %d, plane %d, block %d, page %d, state %d, priority %d, block and page only for interruptible gc record\n ", ssd->gc_request, gc_node->chip, gc_node->die, gc_node->plane, gc_node->block, gc_node->page, gc_node->state, gc_node->priority);
//
//        printf("the sign of the read %d, write %d , erase %d , victim_block_num %d, read_inner %d, write_inner %d,read_write_end %d, freepage_statistic %d\n ",  gc_node->read, gc_node->write, gc_node->erase, gc_node->victim_block_number, gc_node->gc_read_inner, gc_node->gc_write_inner, gc_node->read_write_end, gc_node->victim_block_freepage_statistic);
//        gc_node = gc_node->next_node;
//    }
//    }
}




void printf_ssd_request_queue(struct ssd_info *ssd)
{
    struct request* temp_request;
    unsigned int temp_num_request = ssd->request_queue_length;
    if(temp_num_request == 0){
        printf("ssd request_queue has none\n");
    }else{
        temp_request = ssd->request_queue;
        printf("ssd request_queue status: request_length is %d,\n", ssd->request_queue_length);
        while(temp_request != NULL){
           printf("ssd request_queue , lsn : %d, size : %d, operation :%d   ***1read,0write,complete_lsn_count :%d, distri_flag :%d, begin_time :%llu\n",temp_request->lsn,temp_request->size,temp_request->operation,temp_request->complete_lsn_count,temp_request->distri_flag,temp_request->begin_time);
            if(temp_request == ssd->request_tail){
                break;
            }else{
                temp_request = temp_request->next_node;
            }
        }

    }
}

//lxc for compression
void printf_one_request_subrequests(struct ssd_info* ssd, struct request* request)
{
    struct request* temp_request = request;
    struct sub_request* temp_sub = request->subs;
    if(temp_request == NULL){
        printf("wrong in printf_one_request_subrequests\n");
        while(1){

        }
    }else{
        if(temp_sub == NULL){
            printf("wrong in printf_one_request_subrequests\n");
            while(1){

            }
        }else{

            while(temp_sub != NULL){
                printf("sub_request address %lld\n", temp_sub);
                temp_sub = temp_sub->next_subs;
            }

        }
    }
}//end of printf_one_request_subrequests



//lxc for compression
void printf_mappingtable_relevant_subrequest_in_different_process(struct ssd_info* ssd, struct request* req)
{
    struct request* temp_request;


    ///*************step1, before this founction, request has been valued in 4 parameters, total_wait_evict, total_wait_fetch_mappingtable, create_or_notyet_evict_subrequests, create_or_notyet_fetch_subrequests. so this step is for creat_evict_subrequests or creat_fetch_subrequests and then waiting for getting missing mappingtable*****************************///



    if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 1))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 0) ){
        printf("%d request address %lld, should evict subrequests are :", req->operation, req);
        printf_one_request_subrequests(ssd,req);
    }//end of step1

    ///****************step2, polling and checking whether evict mapping table finished under this if condition.*****************************///

    if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 1))&&((req->create_or_notyet_evict_subrequests == 1))&&(req->create_or_notyet_fetch_subrequests == 0) ){

        printf("%d request address %lld, after evicting :", req->operation, req);
        if(req->subs == NULL){
            printf("OK, now finished and deleted request's evicting mapping table subrequests\n");
        }else{
            printf("wrong in polling and checking evict finishing\n");
                while(1){

                }
        }
    }//end of step2

    ///***************step3, after evict,or not any evict actions at all, then create fetching mapping table subrequests***************************/// 

    if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 0) ){
        printf("%d request address %lld, fetching mappingtable relevant subrequests are :", req->operation, req);
        printf_one_request_subrequests(ssd,req);


    }//end of step3


    ///****************step4, polling and checking whether fetching mapping table finished under this if condition.*****************************///

    if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 1) ){




        printf("%d request address %lld, after finishing fetching :", req->operation, req);
        if(req->subs == NULL){
            printf("OK, now finished and deleted request's evicting mapping table subrequests\n");
        }else{
            printf("wrong in polling and checking fetch finishing\n");
                while(1){

                }
        }

    }//end of step4


    ///****************step5, not any missing mappingtable or already got the mapping table, then do normal creat_sub_request.*****************************///

    if( (req->total_wait_fetch_mappingtable == 0)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 0)&& req->create_or_notyet_normal_subrequests == 0  ){
        printf("%d request address %lld,after mappingtable dealing, then normal subrequests: \n", req->operation, req);
        printf_one_request_subrequests(ssd,req);

    }//end of step5


}//end of printf_mappingtable_relevant_subrequest_in_different_process


//注意，sub_request是在channel结构体中的，而并不是按照chip分的。。////
/////////////add for status  debug lxc ////////////
void printfsub_request_status(struct ssd_info *ssd)
{
//    unsigned int i,j,k;
//    struct sub_request *sub = NULL , *p =NULL;
//
//    for(i=0; i < ssd->parameter->channel_number; i++){
//
//        printf("channel: %d,////channel status:: current_state: %d, current_time: %llu, next_state: %d, next_state_predict_time: %llu \n",i,ssd->channel_head[i].current_state, ssd->channel_head[i].current_time, ssd->channel_head[i].next_state, ssd->channel_head[i].next_state_predict_time);
//
//        printf("chip 0:////chip status:: current_state:%d, current_time: %llu, next_state: %d, next_state_predict_time: %llu \n",ssd->channel_head[i].chip_head[0].current_state,  ssd->channel_head[i].chip_head[0].current_time, ssd->channel_head[i].chip_head[0].next_state, ssd->channel_head[i].chip_head[0].next_state_predict_time);
//
//        printf("chip 1:////chip status:: current_state:%d, current_time: %llu, next_state: %d, next_state_predict_time: %llu \n",ssd->channel_head[i].chip_head[1].current_state,  ssd->channel_head[i].chip_head[1].current_time, ssd->channel_head[i].chip_head[1].next_state, ssd->channel_head[i].chip_head[1].next_state_predict_time);
//        //printf("sub is 0x%x, ssd->channel_head[%d].subs_r_tail is 0x%x , ssd->channel_head[i].subs_r_head is 0x%x\n",sub, i,( ssd->channel_head[i]).subs_r_tail,  ( ssd->channel_head[i]).subs_r_head);
//        if((ssd->channel_head[i]).subs_r_head != NULL){
//            sub = (ssd->channel_head[i]).subs_r_head;
//         //   printf("i is %d, == channel is %d, NULL is %d\n ",i, sub->location->channel,NULL);
//            //printf("11 if\n");
//        }else{
//            sub = NULL;
//            //printf("11 else\n");
//        }
//
//        while(sub != NULL){
//            printf("read operation. chip: %d, lpn: %d, ppn: %d  operation: %d,size: %d,   ////sub_request status:: current_state: %d, current_time: %llu, next_state: %d, next_state_predict_time: %llu, state(only for write): %d\n", sub->location->chip, sub->lpn, sub->ppn,sub->operation, sub->size, sub->current_state, sub->current_time,sub->next_state, sub->next_state_predict_time, sub->state) ;
//        //    printf("sub is 0x%x, ssd->channel_head[%d].subs_r_tail is 0x%x , ssd->channel_head[i].subs_r_head is 0x%x\n",sub,i ,( ssd->channel_head[i]).subs_r_tail,  ( ssd->channel_head[i]).subs_r_head);
//            p = sub;
//            if(sub == (ssd->channel_head[i]).subs_r_tail){
//                //         printf("break\n");
//                break;
//            }else{
//                //       printf("else\n");
//                sub = sub->next_node;
//            }
//        }
//
//
//
//        //print write queue
//        if((ssd->channel_head[i]).subs_w_head != NULL){
//            sub = (ssd->channel_head[i]).subs_w_head;
//            //printf("i is %d, == channel is %d, NULL is %d\n ",i, sub->location->channel,NULL);
//            //printf("11 if\n");
//        }else{
//            sub = NULL;
//            //printf("11 else\n");
//        }
//
//
//        while(sub != NULL){
//            printf("write operation.  chip: %d, lpn: %llu, ppn: %llu  operation: %d,size: %d,   ////sub_request status:: current_state: %d, current_time: %llu, next_state: %d, next_state_predict_time: %llu, state(only for write): %d\n", sub->location->chip, sub->lpn, sub->ppn,sub->operation, sub->size, sub->current_state, sub->current_time,sub->next_state, sub->next_state_predict_time, sub->state) ;
//            p = sub;
//            if(sub == ssd->channel_head[i].subs_w_tail){
//                break;
//            }else{
//                sub = sub->next_node;
//            }
//        }
//
//
//
//    }
}




//
////lxc for compression
////this functon is for only calculating the missing lpn number. Using the return value to make a decision of whether needing fetching. the value above 0 means missing happens.
//int get_missing_number(int* missing_lpn_data)
//{
//
//}
//
//
////lxc for compression
//// To find the lpn in DFTL buffer. 1 means existing in buffer_DFTL, 0 means no.
//int find_in_buffer_DFTL(struct ssd_info* ssd, int lpn)
//{
//
//}
//
//
////lxc for compression
////this is function to analyse lpn_data which has been dealed by databuffer.
//int* get_missing_lpn_from_one_request(struct ssd_info* ssd, int *lpn_data, int sub_request_number)
//{
//int temp = 0;
//int * missing_lpn_data = (int *)malloc(sizeof(int)*(sub_request_number));
//        memset(lpn_data, -1, sizeof(int)*(last_lpn - first_lpn));
//
//
//for (temp = 0; temp < sub_request_number; temp ++){
//    if(lpn_data[temp] != -1){
//    if(find_in_buffer_DFTL(ssd, lpn_data[temp]) == 1){
//     missing_lpn_data[temp] = lpn_data[temp];   
//    }
//
//}
//}
////lxc for compression
////this founction is only for read request.
//int*  read_request_before_distribute_and_get_the_missing_lpn(struct ssd_info* ssd, struct request* coming_read_request)
//{
//    unsigned int lsn, lpn,first_lpn, last_lpn, i;
//
//    unsigned int *need_distr_flag; 
//    unsigned int need_distb_flag;
//    struct request* new_request = coming_read_request;
//    unsigned int missing_real_number = 0;
//
//
//    lsn = new_request->lsn;
//    lpn = new_request->lsn / ssd->parameter->subpage_page;
//    last_lpn = (new_request->lsn + new_request->size - 1) / ssd->parameter->subpage_page;
//    first_lpn = new_request->lsn / ssd->parameter->subpage_page;
//
//    int *lpn_data = (int *)malloc(sizeof(int)*(last_lpn - first_lpn + 1));  
//    int * missing_lpn_data = (int *)malloc(sizeof(int)*(last_lpn - first_lpn +1));
//
//    memset(lpn_data, -1, sizeof(int)*(last_lpn - first_lpn + 1));
//    memset(missing_lpn_data, -1, sizeof(int)*(last_lpn - first_lpn + 1));
//    //deal with need_distr_flag[], if index's lpn has no 
//    while(lpn <= last_lpn){
//
//        unsigned int * temp;
//        index = (lpn - first_lpn) / (32 / ssd->parameter->subpage_page);
//
//        for(i = 0; i < 32/ssd->parameter->subpage_page; i++){
//
//            need_distb_flag = new_request->need_distr_flag[index] & ( (0x0000000f << (i * ssd->parameter->subpage_page) ));
//            if(need_distb_flag != 0){
//
//                *(lpn_data + i) = lpn;
//                if(lpn > (1<<32)){
//                    printf("lpn is too big than 4bytes length\n");
//                    while(1){
//                    }
//                }
//            }
//            lpn++;
//        }//end of for loop
//    }//end of while
//    for(i = 0; i < (last_lpn - first_lpn + 1); i++){
//        printf("the read request lpn_data is %d\n", lpn_data[i]);
//    }
//    for(i = 0; i < (last_lpn - first_lpn + 1) / (32 / ssd->parameter->subpage_page); i++){
//        printf("the need_distr_flag is 0x%x\n", new_request->need_distr_flag[index]);
//    }
//
//    missing_lpn_data = get_missing_lpn_from_one_request(ssd, lpn_data, last_lpn - first_lpn + 1);
//    missing_real_number = get_missing_number(missing_lpn_data); 
//    if(missing_real_number > 0){
//        coming_read_request->total_wait_fetch_mappingtable = 1;
//    }
//
//    return missing_lpn_data;
//}


//lxc for compression
int get_size_from_compression_to_uncompression(struct ssd_info* ssd, struct request* request)
{
    int i = 0, size = 0, temp_len, temp_lpn;
    struct _bi_node* temp_node ;
    temp_len = request->compression_to_uncompression->len;
    temp_node = request->compression_to_uncompression->node_head;
	if(temp_node == NULL){
            return 0;
	}
	
    for(i = 0; i < temp_len; i++){

        temp_lpn = temp_node->lpn_and_state[0];
        if(temp_node == NULL){
            ssdsim_msg("wrong in get_size_from_compression_to_uncompression");
            while(1){

            }
        }
        size += ssd->dram->mt->compression_size[temp_lpn]; // one_page_compression_size ;
        temp_node = temp_node->nextNode;
        //temp_lpn = temp_node->lpn_and_state[0];

    }
    return size;


}//end of get_size_from_compression_to_uncompression

//lxc for compression
//this function can produce the real evicting write page to NANDflash!!! can be append on the req->evict_bilist
//parameters are: ssd, request, should evict size.
void make_real_evict_pages_for_compression_part(struct ssd_info* ssd, struct request* request, int should_evict_size)
{
    int buffer_DFTL_existing_void_size;
    struct buffer_group* temp_tail;
    int temp_lpn, temp_state;
    temp_tail = ssd->dram->mt->buffer_DFTL->buffer_tail;

    int page_num = ssd->parameter->page_block * ssd->parameter->block_plane * ssd->parameter->plane_die * ssd->parameter->die_chip * ssd->parameter->chip_num;
    while(should_evict_size > 0){

        temp_lpn = temp_tail->group;//this part should be translation pages lpn, not data lpn.!!!!!
        //checking the lpn number
        if(temp_lpn > page_num){
            ssdsim_msg("wrong in new compresion compare, wrong lpn number");
            while(1){

            }
        }

        temp_state = temp_tail->stored;
        append_on_request_evict_in_DFTL_buffer_lpn_data(request, temp_lpn, temp_state);
        should_evict_size -= ssd->dram->mt->compression_size[temp_lpn];
        //checking the size.
        if(ssd->dram->mt->compression_size[temp_lpn] > ssd->parameter->page_capacity){
            ssdsim_msg("wrong in make_real_evict_pages, wrong size");
            while(1){

            }
        }

        temp_tail = temp_tail->LRU_link_pre;
    }


    //append_on_request_evict_in_DFTL_buffer_lpn_data(request, temp_lpn, temp_state);

}//end of make_real_evict_pages_for_compression_part

//lxc for compression
//off_set is offset from uncompression buffer's tail node
//here is also so important!!!!!!!!!since we update all the compression size of translation page after every write requests!!!!every write requests, since read requests will not affect mappingtable. 
int get_uncompression_tail_page_compressed_size(struct ssd_info* ssd, int off_set)
{
    int temp_lpn, temp_dirty, i, temp_size;
    struct buffer_group* temp_node;
    temp_node = ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail;
    //this step to get fixed offset treenode in uncompression buffer
    if(off_set == 0){
        temp_node = ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail;

    }else{

        temp_node = ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail;
        for(i = 0; i < off_set; i++){
            temp_node = temp_node->LRU_link_pre;
        }

    }
    temp_lpn = temp_node->group;
    temp_dirty = ssd->dram->mt->compression_dirty[temp_lpn];
    if(temp_dirty == 1) { // this part is to guess to predict the size in the future.

        temp_size = ssd->dram->mt->compression_size[temp_lpn];
//        if( (temp_size + 128) > ssd->parameter->page_capacity){
//            temp_size = ssd->parameter->page_capacity;
//
//        }

    }else{
        temp_size = ssd->dram->mt->compression_size[temp_lpn];
    }
    ssdsim_msg("get compressed pagenubmer is %d, compressed_size is %d", temp_lpn, temp_size );
    return temp_size;

}//end of get_uncompression_tail_page_compressed_size



//lxc for compression
//this founction, the evict page's deduplication has been done in "append_on_request_missing_in_DFTL_buffer_lpn_data", and then compare the space in two  mappingtable buffer...
//during the comparing, then get evict  bi_list
int new_compression_compare_DFTL_buffer_remainder_space_with_compression_area_coming(struct ssd_info* ssd, struct request* request)
{
    struct _bi_list* bilist =  request->missing_in_DFTL_buffer_lpn_data;
    struct _bi_node* binode = bilist->node_head;
    int i = 0, evict_number = 0, evict_size = 0;
//    struct buffer_group* temp_tail;
    int temp_lpn, temp_state;

    int page_num = ssd->parameter->page_block * ssd->parameter->block_plane * ssd->parameter->plane_die * ssd->parameter->die_chip * ssd->parameter->chip_num;
    // int compression_to_uncompression_size = 0, uncompression_existing_void_size = 0, uncompression_incoming_page_number = 0, prediction_page_size = 0, adding_size_in_compression = 0, compression_part_existing_void_size  = 0, evicting_pagenumber_from_uncompression_part = 0 , evicting_size_from_uncompression_part = 0 ;
    int incoming_space_size_for_compression_part = 0;

    int existing_space_inByte = (ssd->dram->mt->max_cached_dir_slots_sizein_byte) - (ssd->dram->mt->now_cached_dir_slots_sizein_byte);

    ssdsim_msg("this time, we get the new existing space size is positive or negative?? %d", existing_space_inByte);
    //step1 getting the existing_space_inByte

    //step2 incoming space size in the future.
    for(i = 0; i < bilist->len; i++){


        temp_lpn = binode->lpn_and_state[0];
        //checking the lpn.
        if(temp_lpn > page_num){
            ssdsim_msg("wrong in new compresion compare, wrong lpn number");
            while(1){

            }
        }
        incoming_space_size_for_compression_part += ssd->dram->mt->compression_size[temp_lpn]; // one_page_compression_size ;
        //checking the size.
        if(ssd->dram->mt->compression_size[temp_lpn] > ssd->parameter->page_capacity){
            ssdsim_msg("wrong in new compression compare, wrong size");
            while(1){

            }
        }
        //checking binode validation
        if(binode == NULL){
            ssdsim_msg("wrong in binode in new compression compare");
            while(1){

            }
        }
        binode = binode->nextNode;
    }
    ssdsim_msg("now the incoming_space_size_for_compression_part size is %d", incoming_space_size_for_compression_part);
    //step3, now we can get the evicting size
    evict_size = incoming_space_size_for_compression_part - existing_space_inByte; 
    if(evict_size < 0 ){
        return 1;
    }else{

        make_real_evict_pages_for_compression_part(ssd, request, evict_size);

        ssdsim_msg("now is for the request lsn %lld, size %d, begintime %lld to create evict pages now", request->lsn, request->size, request->begin_time);
        return 0;
    }

    //
    //    //step1, get the evict number
    //    //step1.1 how many bytes will enter compression area?? only uncompression --->compression,A.  and if hitting in compression, compression will go into uncompression, B. the size should be: A-B. B already get in request->compression_to_uncompression;
    //    //how to get A value. (incoming from nandflash+ pagesize* B number -existing space in uncompression part)
    //    //if(incoming from nandflash+ pagesize* B number) C >0, then C's tail compression size should be: compression size+ 128 comparing pagesize) ( if  C-existing space in uncompression part >0 then get the B then chose the size enough from compresion part's tail
    //    //B: size: get_size_of_bilist.
    //    //A: if(missing and )
    //    //step1 get the size from compression to uncompression
    //    compression_to_uncompression_size = get_size_from_compression_to_uncompression(ssd, request);
    //    ssdsim_msg("now get the compression_to_uncompression_size is %d bytes ", compression_to_uncompression_size);
    //    //step2, get the number of page from uncompression to compression
    //    uncompression_existing_void_size = ssd->dram->mt->uncompression_max_cached_dir_slots_sizein_byte - ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte; 
    //
    //    ssdsim_msg("now uncompression_existing_void_size is %d bytes ", uncompression_existing_void_size);
    //    compression_part_existing_void_size = ssd->dram->mt->max_cached_dir_slots_sizein_byte - ssd->dram->mt->now_cached_dir_slots_sizein_byte;
    //
    //    ssdsim_msg("now compression_existing_void_size is %d bytes ", compression_part_existing_void_size);
    //    uncompression_incoming_page_number = bilist->len + request->compression_to_uncompression->len;
    //    
    //    ssdsim_msg("now uncompression_incoming_page_number is %d pagenumbers ", uncompression_incoming_page_number);
    //    //if no page will enter uncompression part, then that means: the request already been hitted in uncompression buffered, even the compression buffer, should incuring page entering into uncompression part.
    //    if(uncompression_incoming_page_number  != 0){
    //        evicting_size_from_uncompression_part = uncompression_incoming_page_number * (ssd->parameter->page_capacity) - uncompression_existing_void_size;  
    //        if(evicting_size_from_uncompression_part > 0){
    //            if(evicting_size_from_uncompression_part % ssd->parameter->page_capacity != 0){
    //                ssdsim_msg("wrong in calculation evicting size from uncompression_part");
    //                while(1){
    //
    //                }
    //            }
    //        }
    //    ssdsim_msg("now evicting_size_from_uncompression_part is %d bytes", evicting_size_from_uncompression_part);
    //
    //        /*********here judging the pages of moving between compression and uncompression***********************/
    //        if((compression_to_uncompression_size > 0 )||(evicting_size_from_uncompression_part > 0)){
    //
    //            request->if_need_movement_between_com_and_uncom_flag_and_page_numbers = bilist->len + (evicting_size_from_uncompression_part / ssd->parameter->page_capacity);
    //
    //            request->the_number_of_evicting_from_uncom_to_com = (evicting_size_from_uncompression_part / ssd->parameter->page_capacity);
    //       
    //        }
    //        /*********here judging the pages of moving between compression and uncompression*****************/
    //        if(evicting_size_from_uncompression_part <= 0){
    //            return 1;
    //
    //        }else{
    //            //real evicting pages to flash should be got from compression part
    //            //actually, we want to get the uncompression's tail's page's compression size first, but it is difficult.
    //            evicting_pagenumber_from_uncompression_part = evicting_size_from_uncompression_part / ssd->parameter->page_capacity;
    //			if(evicting_size_from_uncompression_part != 0){
    //				if(evicting_size_from_uncompression_part % ssd->parameter->page_capacity != 0){
    //					ssdsim_msg("wrong in get evicting_size_from_uncompression_part");
    //					while(1){
    //
    //					}
    //				}
    //			}
    //            for(i = 0; i < evicting_pagenumber_from_uncompression_part ; i++){
    //
    //                prediction_page_size += get_uncompression_tail_page_compressed_size(ssd, i);//here is our prediction function, since if request is write reuqest, that means page has been modified!!
    //                ssdsim_msg("page number from uncompression to compression is %d, and after compression these pages total size is %d", evicting_pagenumber_from_uncompression_part, prediction_page_size );
    //                if(prediction_page_size > evicting_pagenumber_from_uncompression_part * ssd->parameter->page_capacity){
    //                    ssdsim_msg("compression itself wrong here!!!!");
    //                    while(1){
    //
    //                    }
    //                }
    //
    //            }
    //
    //            if(prediction_page_size  < compression_to_uncompression_size ){
    //                return 1; 
    //            }else{//judging for the compression part's existing space vs will be used space.
    //                if((prediction_page_size - compression_to_uncompression_size) > compression_part_existing_void_size ){
    //
    //                    make_real_evict_pages_for_compression_part(ssd, request, (prediction_page_size-compression_to_uncompression_size - compression_part_existing_void_size));
    //                    ssdsim_msg("now is for the request lsn %lld, size %d, begintime %lld to create evict pages now", request->lsn, request->size, request->begin_time);
    //                    return 0;
    //                }else{
    //                    return 1;
    //                }
    //
    //            }
    //        }
    //    }//only the uncompression incoming page number is not 0,then maybe there is evict page.otherwise,there is no evict page.
    //
}//end of new_compression_compare_DFTL_buffer_remainder_space_with_compression_area_coming











//lxc for compression
//this founction, the evict page's deduplication has been done in "append_on_request_missing_in_DFTL_buffer_lpn_data", and then compare the space in two  mappingtable buffer...
//during the comparing, then get evict  bi_list
int compression_compare_DFTL_buffer_remainder_space_with_compression_area_coming(struct ssd_info* ssd, struct request* request)
{
    struct _bi_list* bilist =  request->missing_in_DFTL_buffer_lpn_data;
    int i = 0, evict_number;
    struct buffer_group* temp_tail;
    int temp_lpn, temp_state;
    int existing_space_inByte = (ssd->dram->mt->max_cached_dir_slots_sizein_byte) - (ssd->dram->mt->now_cached_dir_slots_sizein_byte);
    int compression_to_uncompression_size = 0, uncompression_existing_void_size = 0, uncompression_incoming_page_number = 0, prediction_page_size = 0, adding_size_in_compression = 0, compression_part_existing_void_size  = 0, evicting_pagenumber_from_uncompression_part = 0 , evicting_size_from_uncompression_part = 0 ;
    //step1, get the evict number
    //step1.1 how many bytes will enter compression area?? only uncompression --->compression,A.  and if hitting in compression, compression will go into uncompression, B. the size should be: A-B. B already get in request->compression_to_uncompression;
    //how to get A value. (incoming from nandflash+ pagesize* B number -existing space in uncompression part)
    //if(incoming from nandflash+ pagesize* B number) C >0, then C's tail compression size should be: compression size+ 128 comparing pagesize) ( if  C-existing space in uncompression part >0 then get the B then chose the size enough from compresion part's tail
    //B: size: get_size_of_bilist.
    //A: if(missing and )
    //step1 get the size from compression to uncompression
    compression_to_uncompression_size = get_size_from_compression_to_uncompression(ssd, request);
    ssdsim_msg("now get the compression_to_uncompression_size is %d bytes ", compression_to_uncompression_size);
    //step2, get the number of page from uncompression to compression
    uncompression_existing_void_size = ssd->dram->mt->uncompression_max_cached_dir_slots_sizein_byte - ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte; 

    ssdsim_msg("now uncompression_existing_void_size is %d bytes ", uncompression_existing_void_size);
    compression_part_existing_void_size = ssd->dram->mt->max_cached_dir_slots_sizein_byte - ssd->dram->mt->now_cached_dir_slots_sizein_byte;

    ssdsim_msg("now compression_existing_void_size is %d bytes ", compression_part_existing_void_size);
    uncompression_incoming_page_number = bilist->len + request->compression_to_uncompression->len;
    
    ssdsim_msg("now uncompression_incoming_page_number is %d pagenumbers ", uncompression_incoming_page_number);
    //if no page will enter uncompression part, then that means: the request already been hitted in uncompression buffered, even the compression buffer, should incuring page entering into uncompression part.
    if(uncompression_incoming_page_number  != 0){
        evicting_size_from_uncompression_part = uncompression_incoming_page_number * (ssd->parameter->page_capacity) - uncompression_existing_void_size;  
        if(evicting_size_from_uncompression_part > 0){
            if(evicting_size_from_uncompression_part % ssd->parameter->page_capacity != 0){
                ssdsim_msg("wrong in calculation evicting size from uncompression_part");
                while(1){

                }
            }
        }
    ssdsim_msg("now evicting_size_from_uncompression_part is %d bytes", evicting_size_from_uncompression_part);

        /*********here judging the pages of moving between compression and uncompression***********************/
        if((compression_to_uncompression_size > 0 )||(evicting_size_from_uncompression_part > 0)){

            request->if_need_movement_between_com_and_uncom_flag_and_page_numbers = bilist->len + (evicting_size_from_uncompression_part / ssd->parameter->page_capacity);

            request->the_number_of_evicting_from_uncom_to_com = (evicting_size_from_uncompression_part / ssd->parameter->page_capacity);
       
        }
        /*********here judging the pages of moving between compression and uncompression*****************/
        if(evicting_size_from_uncompression_part <= 0){
            return 1;

        }else{
            //real evicting pages to flash should be got from compression part
            //actually, we want to get the uncompression's tail's page's compression size first, but it is difficult.
            evicting_pagenumber_from_uncompression_part = evicting_size_from_uncompression_part / ssd->parameter->page_capacity;
			if(evicting_size_from_uncompression_part != 0){
				if(evicting_size_from_uncompression_part % ssd->parameter->page_capacity != 0){
					ssdsim_msg("wrong in get evicting_size_from_uncompression_part");
					while(1){

					}
				}
			}
            for(i = 0; i < evicting_pagenumber_from_uncompression_part ; i++){

                prediction_page_size += get_uncompression_tail_page_compressed_size(ssd, i);//here is our prediction function, since if request is write reuqest, that means page has been modified!!
                ssdsim_msg("page number from uncompression to compression is %d, and after compression these pages total size is %d", evicting_pagenumber_from_uncompression_part, prediction_page_size );
                if(prediction_page_size > evicting_pagenumber_from_uncompression_part * ssd->parameter->page_capacity){
                    ssdsim_msg("compression itself wrong here!!!!");
                    while(1){

                    }
                }

            }

            if(prediction_page_size  < compression_to_uncompression_size ){
                return 1; 
            }else{//judging for the compression part's existing space vs will be used space.
                if((prediction_page_size - compression_to_uncompression_size) > compression_part_existing_void_size ){

                    make_real_evict_pages_for_compression_part(ssd, request, (prediction_page_size-compression_to_uncompression_size - compression_part_existing_void_size));
                    ssdsim_msg("now is for the request lsn %lld, size %d, begintime %lld to create evict pages now", request->lsn, request->size, request->begin_time);
                    return 0;
                }else{
                    return 1;
                }

            }
        }
    }//only the uncompression incoming page number is not 0,then maybe there is evict page.otherwise,there is no evict page.

}//end of compression_compare_DFTL_buffer_remainder_space_with_compression_area_coming







//lxc for compression
//this founction, the evict page's deduplication has been done in "append_on_request_missing_in_DFTL_buffer_lpn_data", and then compare the space in mappingtable buffer...
//during the comparing, then get evict  bi_list
int compare_DFTL_buffer_remainder_space_with_missing_in_DFTL_buffer(struct ssd_info* ssd, struct request* request)
{
    struct _bi_list* bilist =  request->missing_in_DFTL_buffer_lpn_data;
    int i = 0, evict_number;
    struct buffer_group* temp_tail;
    int temp_lpn, temp_state;



#ifdef TRANSLATION_PAGE_UNIT 
    int existing_space_inByte = (ssd->dram->mt->max_cached_dir_slots_sizein_byte) - (ssd->dram->mt->now_cached_dir_slots_sizein_byte);



    traceTranslationbuffer_msg("now, comparing DFTL_buffer size for evicting. now cached size is %d, existing_space size inByte is %d", (ssd->dram->mt->now_cached_dir_slots_sizein_byte),existing_space_inByte);



    //step1, get the evict number

     evict_number = (bilist->len) - (existing_space_inByte / ssd->parameter->page_capacity) ;
     if((existing_space_inByte % ssd->parameter->page_capacity) != 0){
         evict_number += 1;
     }
    //because here, there is no compression, so donnot have to do % operation, they are only integerate value.
    if(evict_number <= 0) {
        traceTranslationbuffer_msg("enough space, no evict");

        return 1;

    }else{

        traceTranslationbuffer_msg("need evict translation page number is:");
        temp_tail = ssd->dram->mt->buffer_DFTL->buffer_tail;
        //step2,produce the  evict pages. the evict number has been fixed.
        for(i = 0; i < evict_number; i++){
            temp_lpn = temp_tail->group;//this part should be translation pages lpn, not data lpn.!!!!!
            temp_state = temp_tail->stored;
            append_on_request_evict_in_DFTL_buffer_lpn_data(request, temp_lpn, temp_state);
            temp_tail = temp_tail->LRU_link_pre;

            traceTranslationbuffer_msg("%d", temp_lpn);
        }

        return 1;

    }
#endif //end of TRANSLATION_PAGE_UNIT 

    //for the entry granularity

#ifdef ENTRY_UNIT


    //step1, get the evict number
    evict_number = (bilist->len) - (((ssd->dram->mt->max_cached_entries_sizein_byte) - (ssd->dram->mt->now_cached_entries_sizein_byte)) / ssd->dram->mt->mapping_entry_size) ;



    traceEntrybuffer_msg("now, comparing DFTL_buffer size for evicting. now cached size is %d, existing_space size inByte is %d",  (ssd->dram->mt->now_cached_entries_sizein_byte), ((ssd->dram->mt->max_cached_entries_sizein_byte) - (ssd->dram->mt->now_cached_entries_sizein_byte)));

    if(evict_number <= 0) {
        traceEntrybuffer_msg("enough space, no evict");
        return 1;

    }else{


        traceEntrybuffer_msg("now evict, entries lpn are :");
        temp_tail = ssd->dram->mt->buffer_DFTL->buffer_tail;
        //step2,produce the  evict pages. the evict number has been fixed.
        for(i = 0; i < evict_number; i++){
            temp_lpn = temp_tail->group;
            temp_state = temp_tail->stored;
            append_on_request_evict_in_DFTL_buffer_lpn_data(request, temp_lpn, temp_state);
            temp_tail = temp_tail->LRU_link_pre;



            traceEntrybuffer_msg("%d", temp_lpn);
        }



        return 1;
    }
#endif

}//end of compare_DFTL_buffer_remainder_space_with_missing_in_DFTL_buffer

//lxc for compression

//int get_translation_page_number(int* translation_page_temp, int i, int temp_node_lpn)
//{
//
//
//
//}

//lxc for compression
int get_translation_number_by_data_lpn(int data_lpn)
{
    int temp_translation_page = data_lpn /  NUM_ENTRIES_PER_DIR;
    return temp_translation_page;

}//end of get_translation_number_by_data_lpn



//lxc for compression
//this is the new part for the entry size of uncompression part.  if hit it in compression area, then return sign is 1, if hit it in uncompression area, then return sign is 2, if buffer_node == NULL, sign will lost effect.
struct buffer_group* new_node_isnot_in_two_DIFF_DFTL_buffer(struct ssd_info* ssd, int lpn, int* sign)
{

    int temp_translation_page = -1;
    struct buffer_group* buffer_node,  key;
    temp_translation_page = get_translation_number_by_data_lpn(lpn);

    key.group = temp_translation_page;
    buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL,
            (TREE_NODE *) &key);
    if(buffer_node == NULL){

        //in if is finding in uncompression part.
        key.group = lpn;
        buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL,
                (TREE_NODE *) &key);

        buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL_UNCOM,
                (TREE_NODE *) &key);
        if(buffer_node == NULL){

			*sign = 0;
            return buffer_node;
        }else{
            *sign = 2;
            return buffer_node;
        }

    }else{
        *sign = 1; 
        return buffer_node;
    }
}//end of new node_isnot_in_two_DIFF_DFTL_buffer 








//lxc for compression
//if hit it in compression area, then return sign is 1, if hit it in uncompression area, then return sign is 2, if buffer_node == NULL, sign will lost effect.
struct buffer_group* node_isnot_in_two_DIFF_DFTL_buffer(struct ssd_info* ssd, int lpn, int* sign)
{

    int temp_translation_page = -1;
    struct buffer_group* buffer_node,  key;
    temp_translation_page = get_translation_number_by_data_lpn(lpn);

    key.group = temp_translation_page;
    buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL,
            (TREE_NODE *) &key);
    if(buffer_node == NULL){

        //in if is finding in uncompression part.
        buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL_UNCOM,
                (TREE_NODE *) &key);
        if(buffer_node == NULL){

			*sign = 0;
            return buffer_node;
        }else{
            *sign = 2;
            return buffer_node;
        }

    }else{
        *sign = 1; 
        return buffer_node;
    }
}//end of node_isnot_in_two_DIFF_DFTL_buffer 

//lxc for compression
//return the buffer_node struct,  NULL means:node is not in DFTL buffer.  no NULL reverse.
struct buffer_group* node_isnot_in_DFTL_buffer(struct ssd_info* ssd, int lpn) 
{


#ifdef TRANSLATION_PAGE_UNIT 
    int temp_translation_page = -1;
    struct buffer_group* buffer_node,  key;
    temp_translation_page = get_translation_number_by_data_lpn(lpn);

    key.group = temp_translation_page;
    buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL,
            (TREE_NODE *) &key);
    return buffer_node;

#endif

#ifdef ENTRY_UNIT
    struct buffer_group key, *buffer_node;

    key.group = lpn;
    buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL,
            (TREE_NODE *) &key);


    return buffer_node;


#endif

}//end of node_isnot_in_DFTL_buffer

//lxc for compression
//this is only for write requests
void If_hitInBUffer_move_theNode_ahead_in_writerequest(struct ssd_info* ssd,struct buffer_info* buffer, struct buffer_group* buffer_node)
{


struct buffer_group* temp_buffer_node,  key;

    key.group = buffer_node->group; 
    temp_buffer_node = (struct buffer_group *) avlTreeFind(buffer,
            (TREE_NODE *) &key);
    if(temp_buffer_node == NULL){

        ssdsim_msg("wrong in hit and move in read requst 2");
        while(1){

        }

    }



    if(buffer == ssd->dram->mt->buffer_DFTL){
        if (buffer->buffer_head != buffer_node) {
            if (buffer->buffer_tail == buffer_node) {
                buffer->buffer_tail = buffer_node->LRU_link_pre;
                buffer_node->LRU_link_pre->LRU_link_next = NULL;
            } else if (buffer_node != buffer->buffer_head) {
                buffer_node->LRU_link_pre->LRU_link_next = buffer_node->LRU_link_next;
                buffer_node->LRU_link_next->LRU_link_pre = buffer_node->LRU_link_pre;
            }
            buffer_node->LRU_link_next = buffer->buffer_head;
            buffer->buffer_head->LRU_link_pre = buffer_node;
            buffer_node->LRU_link_pre = NULL;
            buffer->buffer_head = buffer_node;
        }
    }
    //this is for uncompression part area.
    if(buffer == ssd->dram->mt->buffer_DFTL_UNCOM){
        if (buffer->buffer_head != buffer_node) {
            if (buffer->buffer_tail == buffer_node) {
                buffer->buffer_tail = buffer_node->LRU_link_pre;
                buffer_node->LRU_link_pre->LRU_link_next = NULL;
            } else if (buffer_node != buffer->buffer_head) {
                buffer_node->LRU_link_pre->LRU_link_next = buffer_node->LRU_link_next;
                buffer_node->LRU_link_next->LRU_link_pre = buffer_node->LRU_link_pre;
            }
            buffer_node->LRU_link_next = buffer->buffer_head;
            buffer->buffer_head->LRU_link_pre = buffer_node;
            buffer_node->LRU_link_pre = NULL;
            buffer->buffer_head = buffer_node;
        }

    }

    //here means hit once.
    buffer->write_hit++;

}//end of If_hitInBUffer_move_theNode_ahead_in_writerequest






//lxc for compression
//this is only for read requests
void If_hitInBUffer_move_theNode_ahead_in_readrequest(struct ssd_info* ssd, struct buffer_info* buffer, struct buffer_group* buffer_node)
{

    struct buffer_group* temp_buffer_node,  key;

    key.group = buffer_node->group; 
    temp_buffer_node = (struct buffer_group *) avlTreeFind(buffer,
            (TREE_NODE *) &key);
    if(temp_buffer_node == NULL){

        ssdsim_msg("wrong in hit and move in read requst 1");
        while(1){

        }

    }

    if(buffer == ssd->dram->mt->buffer_DFTL){
        if (buffer->buffer_head != buffer_node) {
            if (buffer->buffer_tail == buffer_node) {
                buffer->buffer_tail = buffer_node->LRU_link_pre;
                buffer_node->LRU_link_pre->LRU_link_next = NULL;
            } else if (buffer_node != buffer->buffer_head) {
                buffer_node->LRU_link_pre->LRU_link_next = buffer_node->LRU_link_next;
                buffer_node->LRU_link_next->LRU_link_pre = buffer_node->LRU_link_pre;
            }
            buffer_node->LRU_link_next = buffer->buffer_head;
            buffer->buffer_head->LRU_link_pre = buffer_node;
            buffer_node->LRU_link_pre = NULL;
            buffer->buffer_head = buffer_node;
        }
    }
    //this is for uncompression part area.
    if(buffer == ssd->dram->mt->buffer_DFTL_UNCOM){
        if (buffer->buffer_head != buffer_node) {
            if (buffer->buffer_tail == buffer_node) {
                buffer->buffer_tail = buffer_node->LRU_link_pre;
                buffer_node->LRU_link_pre->LRU_link_next = NULL;
            } else if (buffer_node != buffer->buffer_head) {
                buffer_node->LRU_link_pre->LRU_link_next = buffer_node->LRU_link_next;
                buffer_node->LRU_link_next->LRU_link_pre = buffer_node->LRU_link_pre;
            }
            buffer_node->LRU_link_next = buffer->buffer_head;
            buffer->buffer_head->LRU_link_pre = buffer_node;
            buffer_node->LRU_link_pre = NULL;
            buffer->buffer_head = buffer_node;
        }

    }

    //here means hit once.
    buffer->read_hit++;

}//end of If_hitInBUffer_move_theNode_ahead_in_readrequest



//lxc for compression
void COMPRESSION_read_value_request_DFTL_parameters(struct ssd_info* ssd, struct request* req)
{
	int i = 0, j = 0, k = 0;
	int sign = 0;
	struct _bi_node* temp_node = NULL;
	struct _bi_node* temp_node1 = NULL;
	struct _bi_list* temp_list = NULL;
	int temp_node_lpn = -1;
	int temp_node_state = -1;
	struct buffer_group* buffer_node = NULL; 

//repaired part 
    unsigned int need_distb_flag = 0, lsn, last_lpn, first_lpn, index;

    unsigned int start, end, first_lsn, last_lsn, lpn, flag = 0, flag_attached = 0, full_page;
    unsigned int  sub_size;
    struct sub_request *sub;
    int *complt;

    full_page = ~(0xffffffff << ssd->parameter->subpage_page);


    //lsn = req->lsn;
    //lpn = req->lsn / ssd->parameter->subpage_page;
    //last_lpn = (req->lsn + req->size - 1) / ssd->parameter->subpage_page;
    //first_lpn = req->lsn / ssd->parameter->subpage_page;

    if(req->operation != 1){
        printf("logical error\n");
        while(1){

        }
        return;
    }




//step1 to get missing in data buffer lpn.

    ssdsim_msg("1 in compression read value requeste parameters, req->lsn %d", req->lsn);
    if (req != NULL) {
        if (req->distri_flag == 0) {
            //如果还有一些读请求需要处理
            if (req->complete_lsn_count != ssd->request_tail->size) {

    ssdsim_msg("2 in read value requeste parameters, req->lsn %d", req->lsn);
                first_lsn = req->lsn;
                last_lsn = first_lsn + req->size;
                complt = req->need_distr_flag;
                start = first_lsn - first_lsn % ssd->parameter->subpage_page;
                end = (last_lsn / ssd->parameter->subpage_page + 1) * ssd->parameter->subpage_page;
                i = (end - start) / 32;



                while (i >= 0) {
                    /*************************************************************************************
                     *一个32位的整型数据的每一位代表一个子页，32/ssd->parameter->subpage_page就表示有多少页，
                     *这里的每一页的状态都存放在了 req->need_distr_flag中，也就是complt中，通过比较complt的
                     *每一项与full_page，就可以知道，这一页是否处理完成。如果没处理完成则通过creat_sub_request
                     函数创建子请求。
                     *************************************************************************************/
                    for (j = 0; j < 32 / ssd->parameter->subpage_page; j++) {
                        k = (complt[((end - start) / 32 - i)] >> (ssd->parameter->subpage_page * j)) & full_page;

                                ssdsim_msg("0 append on missing in data_buffer k is %d", k);
                        if (k != 0) {
                            lpn = start / ssd->parameter->subpage_page +
                                ((end - start) / 32 - i) * 32 / ssd->parameter->subpage_page + j;

                                ssdsim_msg("1  append on missing in data_buffer lpn is %d", lpn);
                            sub_size = transfer_size(ssd, k, lpn, req);

                                ssdsim_msg("sub_size is %d", sub_size);
                            if (sub_size == 0) {
                                continue;
                            } else {
                                //sub = creat_sub_request(ssd, lpn, sub_size, 0, req, req->operation);

                                append_on_request_missing_in_data_buffer_lpn_data( req, lpn, 15);
                                ssdsim_msg("2 append on missing in data_buffer lpn is %d", lpn);

                            }
                        }
                    }
                    i = i - 1;
                }//end of while

                //lxcprogram_de-prioritize.
                //1、 req's every sub_request's slack_time.
                //2、 insert the new sub_requests into channel_queue's respective chip
                //        calc_req_slack_time(req);
                //        reordering_req(req);

				//step1.1 here there is no missing in databuffer.
            } else {
                //req->begin_time = ssd->current_time;
				req->response_time = ssd->current_time + 1000;
                req->total_wait_evict = 0; // 1 means needing wait, 0 do not have to wait or finishing evicting.
                req->total_wait_fetch_mappingtable = 0; // 1 means needing fetching, 0 dont have to fetch or finish fetching
                req->create_or_notyet_evict_subrequests = 0;
                req->create_or_notyet_fetch_subrequests = 0;
                req->create_or_notyet_normal_subrequests = 1;
                ssdsim_msg("compression part, req %lld has been set the create_or_notyet_normal sign %d", req, req->create_or_notyet_normal_subrequests);
            //    return;
            }

        }
    }
//end step1






		//step2,according to the lpn in 'missing_in_data_buffer_lpn_data',check every lpn in DFTL_buffer.
		// check both  compression buffer and uncompression buffer.

		temp_list = req->missing_in_data_buffer_lpn_data;
		temp_node = temp_list->node_tail;//since the insert is head insert,  head:high,tail: low.  

		ssdsim_msg("read request address %lld, lsn %d, length in missing data buffer is %d,", req, req->lsn, temp_list->len);

		if(temp_list->len != 0){//the begin of step2
			//step2.1 check  compression buffer.
			for(i = 0; i < temp_list->len; i++){

				temp_node_lpn = temp_node->lpn_and_state[0];
				temp_node_state = temp_node->lpn_and_state[1];

	//			buffer_node = node_isnot_in_two_DIFF_DFTL_buffer(ssd, temp_node_lpn, &sign);
                //new compression
				buffer_node = new_node_isnot_in_two_DIFF_DFTL_buffer(ssd, temp_node_lpn, &sign);

				if(buffer_node == NULL){

#ifdef SET_INTO_LIMITTED_BUFFER
					append_on_request_missing_in_DFTL_buffer_lpn_data(req, temp_node_lpn, temp_node_state);//also there are 2 buffers, but we only append the lpn on this name
#endif
					//and increase the read_miss_hit number
					ssd->dram->mt->buffer_DFTL->read_miss_hit ++;

				}else{
					//only when buffer_node is not NULL, then sign can be used.
					if(sign == 1){//hit in compression area buffer.

#ifdef SET_INTO_LIMITTED_BUFFER
                        If_hitInBUffer_move_theNode_ahead_in_readrequest(ssd, ssd->dram->mt->buffer_DFTL, buffer_node);
//here, just check the moving result.
                    if(buffer_node->group != ssd->dram->mt->buffer_DFTL->buffer_head->group){

                        ssdsim_msg("write request moving the node's value %d to DFTL_buffer headr is wrong ", buffer_node->group);
                        while(1){

                        }
                    }
						//ont only move ahead, but also add to the request's compressionToUncompression_blistqueue.

                    traceTranslationbuffer_msg("compression , read request hitting,ssd just move it to the header,  value number is %d ", buffer_node->group);
						append_on_request_compression_to_uncompression_lpn_data( req, temp_node_lpn, temp_node_state);
#endif
					}else{//hit in uncompression area buffer.
						if(sign != 2){
							ssdsim_msg("wrong in value read request parameter");
							while(1){

							}
						}

#ifdef SET_INTO_LIMITTED_BUFFER
						If_hitInBUffer_move_theNode_ahead_in_readrequest(ssd, ssd->dram->mt->buffer_DFTL_UNCOM, buffer_node);//this function has finished the hit add in buffer_DFTL_UNCOM
						//here, just check the moving result.
						if(buffer_node->group != ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head->group){

                        ssdsim_msg("write request moving the node's value %d to DFTL_buffer headr is wrong ", buffer_node->group);
                        while(1){

                        }
                    }
						//uncompression part, just need to move ahead and then record the hit number
						req->hit_in_uncompression_buffer_number ++;//this is only for the evict page number.
                        //new compression
                        ssd->dram->mt->buffer_DFTL_UNCOM->read_hit ++;

#endif
					}



				}

				if(temp_node == NULL){
					printf("bi_list wrong in read_value_request_DFTL_parameters, missing_in_data_buffer_lpn read wrong, then stop\n");
					while(1){

					}
				}


				//temp_node = temp_node->nextNode;
				temp_node = temp_node->prevNode;

			}//end of for loop


		}//end of step2
//		else{//never go into here, just for logical checking. since at the begining, we has judged the 
//			ssdsim_msg("here will be never happen in CMPRESSION_read_value_request_DFTL_parameters");
//			while(1){
//
//			}
//
//		}

		//step3, after getting all the missing in the mappingtable buffer, then get the evict lpn 
		temp_list = req->missing_in_DFTL_buffer_lpn_data;
		temp_node = temp_list->node_head;//here, there is no affect in head or tail
		//since compression ftl contains not only missing_in_mappingtable buffer, but also related with uncompression, so we should do it at any time, not only depends on temp_list->len.


        //!!!here is very important!!   new!!! only current_time can be changed.
        //new compression  we should know that, when there existing fetching or evicting, then we can ignore the compression and decompression.
        if(temp_list->len == 0){
            if(req->compression_to_uncompression->len > 0){
                if(ssd->current_time == req->begin_time){//this current_time == begin_time is very important here.
                    ssd->current_time = ssd->current_time + (req->compression_to_uncompression->len + req->missing_in_DFTL_buffer_lpn_data->len) * COMPRESSION_COST_TIME_ONE_PAGE;

                }
            }
        }

        if(temp_list->len != 0){//the begin of step3
            //compression_compare_DFTL_buffer_remainder_space_with_compression_area_coming(ssd, req);
            //new compression
            new_compression_compare_DFTL_buffer_remainder_space_with_compression_area_coming(ssd, req);

        }
        //    if(temp_list->len != 0){//the begin of step3
		//            compare_DFTL_buffer_remainder_space_with_missing_in_DFTL_buffer(ssd, req);
		//    }//end of the step3



		//step4, according to the list values, to value other parameters.
		if(req->missing_in_DFTL_buffer_lpn_data->len != 0){

			req->total_wait_fetch_mappingtable = 1;
			ssd->missing_mapping_and_cannot_get_traceline = 1;

		}
		if(req->should_evict_lpn_data->len != 0){
			req->total_wait_evict = 1;
		}

		req->create_or_notyet_evict_subrequests = 0;
		req->create_or_notyet_fetch_subrequests = 0;

		//return ssd;




   //last step:checking the signs logically
        if(req->missing_in_data_buffer_lpn_data->len == 0){
            if( (req->total_wait_evict != 0) || ( req->total_wait_fetch_mappingtable != 0) || (ssd->missing_mapping_and_cannot_get_traceline != 0) ){
                ssdsim_msg("1 wrong in compression read  value request DFTL ");
                while(1){

                }
            }
        }


        if(req->missing_in_DFTL_buffer_lpn_data->len == 0){
            if( (req->total_wait_evict != 0) || ( req->total_wait_fetch_mappingtable != 0) || (ssd->missing_mapping_and_cannot_get_traceline != 0) ){
                ssdsim_msg("2 wrong in compression read value request DFTL ");
                while(1){

                }
            }
        }

        if(req->should_evict_lpn_data->len == 0){
            if(req->missing_in_DFTL_buffer_lpn_data->len != 0){
                if( (req->total_wait_evict != 0) || (ssd->missing_mapping_and_cannot_get_traceline == 0) ){
                    ssdsim_msg("3 wrong in compression read  value request DFTL ");
                    while(1){

                    }
                }

            }
        }
        ssdsim_msg("finish compression read value parameters check");

        ssdsim_msg("before outside compression read request address %lld, lsn %d, length in missing data buffer is %d,", req, req->lsn, req->missing_in_data_buffer_lpn_data->len);

	}//end of COMPRESSION_read_value_request_DFTL_parameters




//lxc for compression
//this function is for read request. from its need_distr_flag

void read_value_request_DFTL_parameters(struct ssd_info* ssd, struct request* req)
{
    int i = 0, j = 0, k = 0;

    struct _bi_node* temp_node = NULL;
    struct _bi_node* temp_node1 = NULL;
    struct _bi_list* temp_list = NULL;
    int temp_node_lpn = -1;
    int temp_node_state = -1;
    struct buffer_group* buffer_node = NULL; 



    unsigned int need_distb_flag = 0, lsn, last_lpn, first_lpn, index;

    unsigned int start, end, first_lsn, last_lsn, lpn, flag = 0, flag_attached = 0, full_page;
    unsigned int  sub_size;
    struct sub_request *sub;
    int *complt;

    full_page = ~(0xffffffff << ssd->parameter->subpage_page);


    //lsn = req->lsn;
    //lpn = req->lsn / ssd->parameter->subpage_page;
    //last_lpn = (req->lsn + req->size - 1) / ssd->parameter->subpage_page;
    //first_lpn = req->lsn / ssd->parameter->subpage_page;






    if(req->operation != 1){
        printf("logical error\n");
        while(1){

        }
        return;
    }




//
//    if(*(req->need_distr_flag) == 0){// every req has need_dist_flag, and 0 means dont have to disturb subrequests
//        req->total_wait_evict = 0; // 1 means needing wait, 0 do not have to wait or finishing evicting.
//        req->total_wait_fetch_mappingtable = 0; // 1 means needing fetching, 0 dont have to fetch or finish fetching
//        req->create_or_notyet_evict_subrequests = 0;
//        req->create_or_notyet_fetch_subrequests = 0;
//        req->create_or_notyet_fetch_subrequests = 0;
//        req->create_or_notyet_normal_subrequests = 1;
//        ssdsim_msg("req %lld has been set the create_or_notyet_normal sign %d", req->create_or_notyet_normal_subrequests);
//
//        return;
//    }else{//read missed in data buffer hitting.
//        //step1, get the missing lpn in databuffer and value the parameter.
//        while(lpn <= last_lpn){
//
//            unsigned int * temp;
//            index = (lpn - first_lpn) / (32 / ssd->parameter->subpage_page);
//
//
//            need_distb_flag = req->need_distr_flag[index] & ( (0x0000000f << ( ((lpn - first_lpn) %(32 /ssd->parameter->subpage_page)) * ssd->parameter->subpage_page) ));
//                if(need_distb_flag != 0){
//
//                    //                if(lpn > (1<<32)){
//                    //                    printf("lpn is too big than 4bytes length\n");
//                    //                    while(1){
//                    //                    }
//                    //                }
//
//                    append_on_request_missing_in_data_buffer_lpn_data( req, lpn, need_distb_flag);
//
//            }//end of if
//
//                lpn++;
//        }//end of step1
//





    ssdsim_msg("1 in read value requeste parameters, req->lsn %d", req->lsn);
    if (req != NULL) {
        if (req->distri_flag == 0) {
            //如果还有一些读请求需要处理
            if (req->complete_lsn_count != ssd->request_tail->size) {

    ssdsim_msg("2 in read value requeste parameters, req->lsn %d", req->lsn);
                first_lsn = req->lsn;
                last_lsn = first_lsn + req->size;
                complt = req->need_distr_flag;
                start = first_lsn - first_lsn % ssd->parameter->subpage_page;
                end = (last_lsn / ssd->parameter->subpage_page + 1) * ssd->parameter->subpage_page;
                i = (end - start) / 32;

                //lxc for compression

                //get_one_request_missing_part();

                while (i >= 0) {
                    /*************************************************************************************
                     *一个32位的整型数据的每一位代表一个子页，32/ssd->parameter->subpage_page就表示有多少页，
                     *这里的每一页的状态都存放在了 req->need_distr_flag中，也就是complt中，通过比较complt的
                     *每一项与full_page，就可以知道，这一页是否处理完成。如果没处理完成则通过creat_sub_request
                     函数创建子请求。
                     *************************************************************************************/
                    for (j = 0; j < 32 / ssd->parameter->subpage_page; j++) {
                        k = (complt[((end - start) / 32 - i)] >> (ssd->parameter->subpage_page * j)) & full_page;

                                ssdsim_msg("0 append on missing in data_buffer k is %d", k);
                        if (k != 0) {
                            lpn = start / ssd->parameter->subpage_page +
                                ((end - start) / 32 - i) * 32 / ssd->parameter->subpage_page + j;

                                ssdsim_msg("1  append on missing in data_buffer lpn is %d", lpn);
                            sub_size = transfer_size(ssd, k, lpn, req);

                                ssdsim_msg("sub_size is %d", sub_size);
                            if (sub_size == 0) {
                                continue;
                            } else {
                                //sub = creat_sub_request(ssd, lpn, sub_size, 0, req, req->operation);

                                append_on_request_missing_in_data_buffer_lpn_data( req, lpn, 15);
                                ssdsim_msg("2 append on missing in data_buffer lpn is %d", lpn);

                            }
                        }
                    }
                    i = i - 1;
                }//end of while

                //lxcprogram_de-prioritize.
                //1、 req's every sub_request's slack_time.
                //2、 insert the new sub_requests into channel_queue's respective chip
                //        calc_req_slack_time(req);
                //        reordering_req(req);


            } else {
                //req->begin_time = ssd->current_time;
                
				req->response_time = ssd->current_time + 1000;
                req->total_wait_evict = 0; // 1 means needing wait, 0 do not have to wait or finishing evicting.
                req->total_wait_fetch_mappingtable = 0; // 1 means needing fetching, 0 dont have to fetch or finish fetching
                req->create_or_notyet_evict_subrequests = 0;
                req->create_or_notyet_fetch_subrequests = 0;
                req->create_or_notyet_normal_subrequests = 1;
                ssdsim_msg("req %lld has been set the create_or_notyet_normal sign %d", req, req->create_or_notyet_normal_subrequests);
            //    return;
            }

        }
    }




        //step2,according to the lpn in 'missing_in_data_buffer_lpn_data',check every lpn in DFTL_buffer.

        temp_list = req->missing_in_data_buffer_lpn_data;
        temp_node = temp_list->node_tail;//since the insert is head insert,  head:high,tail: low.  
        ssdsim_msg("read request address %lld, lsn %d, length in missing data buffer is %d,", req, req->lsn, temp_list->len);

        if(temp_list->len != 0){//the begin of step2
            for(i = 0; i < temp_list->len; i++){

                temp_node_lpn = temp_node->lpn_and_state[0];
                temp_node_state = temp_node->lpn_and_state[1];

                buffer_node = node_isnot_in_DFTL_buffer(ssd, temp_node_lpn);

                if(buffer_node == NULL){
#ifdef SET_INTO_LIMITTED_BUFFER
                    append_on_request_missing_in_DFTL_buffer_lpn_data(req, temp_node_lpn, temp_node_state);
#endif
                    //and increase the read_miss_hit number
                    ssd->dram->mt->buffer_DFTL->read_miss_hit ++;

                }else{

#ifdef SET_INTO_LIMITTED_BUFFER
                    //here, this funtion issue the adding of the read_hit number.
                    If_hitInBUffer_move_theNode_ahead_in_readrequest(ssd, ssd->dram->mt->buffer_DFTL, buffer_node);

                    traceEntrybuffer_msg("read request hitting, ssd just move it to header, entry lpn is %d", buffer_node->group);
                    traceTranslationbuffer_msg("read request hitting,ssd just move it to the header,  value number is %d ", buffer_node->group);
                    //here, just check the moving result.
                    if(buffer_node != ssd->dram->mt->buffer_DFTL->buffer_head){

                        ssdsim_msg("write request moving the node's value %d to DFTL_buffer headr is wrong ", buffer_node->group);
                        while(1){

                        }
                    }
#endif
                }
                //just checking the bi_list's validation here.
                if(temp_node == NULL){
                    printf("read reqeust  bi_list wrong in read_value_request_DFTL_parameters, missing_in_data_buffer_lpn read wrong, then stop\n");
                    while(1){

                    }
                }


                //temp_node = temp_node->nextNode;
                temp_node = temp_node->prevNode;

            }//end of for loop

        }//end of step2
        else{//if there is no missing in data_buffer, then do nothing in step 2. 
            //since read cannot be judgement only in DFTL_buffer_management, so ,here can be ...!!!
//            ssdsim_msg("here will be never happen in read_value_request_DFTL_parameters");
//            while(1){
//
//            }

        }




        //step3, after getting all the missing in the mappingtable buffer, then get the evict lpn 
        temp_list = req->missing_in_DFTL_buffer_lpn_data;
        temp_node = temp_list->node_head;//here, there is no affect in head or tail

        if(temp_list->len != 0){//the begin of step3
            //            int translation_page_temp[temp_list->len];
            //
            //            for(i = 0; i < temp_list->len; i++ ){
            //                translation_page_temp[i] = -1;
            //            }
            //                for(i = 0; i < temp_list->len; i++ ){
            //                    temp_node1 = temp_node
            //                    temp_node_lpn = temp_node1->lpn_and_state[0];
            //                    temp_node_state = temp_node1->lpn_and_state[1];
            //
            //                    get_translation_page_number(translation_page_temp, i, temp_node_lpn);
            //
            //                    temp_node = temp_node->nextNode;
            //
            //                }
            //
            compare_DFTL_buffer_remainder_space_with_missing_in_DFTL_buffer(ssd, req);
        }//end of the step3



        //step4, according to the list values, to value other parameters.
        if(req->missing_in_DFTL_buffer_lpn_data->len != 0){

            req->total_wait_fetch_mappingtable = 1;
            ssd->missing_mapping_and_cannot_get_traceline = 1;

        }
        if(req->should_evict_lpn_data->len != 0){
            req->total_wait_evict = 1;
        }

        req->create_or_notyet_evict_subrequests = 0;
        req->create_or_notyet_fetch_subrequests = 0;

        //return ssd;


    //last step:checking the signs logically
        if(req->missing_in_data_buffer_lpn_data->len == 0){
            if( (req->total_wait_evict != 0) || ( req->total_wait_fetch_mappingtable != 0) || (ssd->missing_mapping_and_cannot_get_traceline != 0) ){
                ssdsim_msg("1 wrong in read  value request DFTL ");
                while(1){

                }
            }
        }


        if(req->missing_in_DFTL_buffer_lpn_data->len == 0){
            if( (req->total_wait_evict != 0) || ( req->total_wait_fetch_mappingtable != 0) || (ssd->missing_mapping_and_cannot_get_traceline != 0) ){
                ssdsim_msg("2 wrong in read  value request DFTL ");
                while(1){

                }
            }
        }

        if(req->should_evict_lpn_data->len == 0){
            if(req->missing_in_DFTL_buffer_lpn_data->len != 0){
                if( (req->total_wait_evict != 0) || (ssd->missing_mapping_and_cannot_get_traceline == 0) ){
                    ssdsim_msg("3 wrong in read  value request DFTL ");
                    while(1){

                    }
                }

            }
        }
        ssdsim_msg("finish read value parameters check");

        ssdsim_msg("before outside read request address %lld, lsn %d, length in missing data buffer is %d,", req, req->lsn, req->missing_in_data_buffer_lpn_data->len);
}//read_value_request_DFTL_parameters


//lxc for compression
void COMPRESSION_write_value_request_DFTL_parameters(struct ssd_info* ssd, struct request* req)

{
    int i = 0;
    int* sign = NULL;
    struct _bi_node* temp_node = NULL;
    struct _bi_node* temp_node1 = NULL;
    struct _bi_list* temp_list = NULL;
    int temp_node_lpn = -1;
    int temp_node_state = -1;
    struct buffer_group* buffer_node = NULL; 

    if(req->operation != 0){
        printf("logical error\n");
        while(1){

        }
    }


    temp_list = req->missing_in_data_buffer_lpn_data; 
    //temp_node = temp_list->node_head; 
    temp_node = temp_list->node_tail; 

    if(temp_list->len == 0){
        req->total_wait_evict = 0; // 1 means needing wait, 0 do not have to wait or finishing evicting.
        req->total_wait_fetch_mappingtable = 0; // 1 means needing fetching, 0 dont have to fetch or finish fetching
        req->create_or_notyet_evict_subrequests = 0;
        req->create_or_notyet_fetch_subrequests = 0;
        req->create_or_notyet_normal_subrequests = 1;
        return;
    }else{

                //step2,according to the lpn in 'missing_in_data_buffer_lpn_data',check every lpn in DFTL_buffer.
        // check both  compression buffer and uncompression buffer.


        if(temp_list->len != 0){//the begin of step2
            //step2.1 check  compression buffer.
            for(i = 0; i < temp_list->len; i++){

                temp_node_lpn = temp_node->lpn_and_state[0];
                temp_node_state = temp_node->lpn_and_state[1];

                //buffer_node = node_isnot_in_two_DIFF_DFTL_buffer(ssd, temp_node_lpn, &sign);
                //new compression
				buffer_node = new_node_isnot_in_two_DIFF_DFTL_buffer(ssd, temp_node_lpn, &sign);

                if(buffer_node == NULL){
                    append_on_request_missing_in_DFTL_buffer_lpn_data(req, temp_node_lpn, temp_node_state);//also there are 2 buffers, but we only append the lpn on this name

                    //and increase the read_miss_hit number
                    ssd->dram->mt->buffer_DFTL->write_miss_hit ++;

                }else{
                    //only when buffer_node is not NULL, then sign can be used.
                    if(sign == 1){//hit in compression area buffer.

                        If_hitInBUffer_move_theNode_ahead_in_writerequest(ssd, ssd->dram->mt->buffer_DFTL, buffer_node);
                        //ont only move ahead, but also add to the request's compressionToUncompression_blistqueue.

                    traceTranslationbuffer_msg("compression write request hitting,ssd just move it to the header,  value number is %d ", buffer_node->group);
                        append_on_request_compression_to_uncompression_lpn_data( req, temp_node_lpn, temp_node_state);

                    //here, just check the moving result.
                    if(buffer_node->group != ssd->dram->mt->buffer_DFTL->buffer_head->group){

                        ssdsim_msg("compression write request  moving the node's value %d to DFTL_buffer headr is wrong ", buffer_node->group);
                        while(1){

                        }
                    }


                    }else{//hit in uncompression area buffer.
                        if(sign != 2){
                            ssdsim_msg("wrong in value read request parameter");
                            while(1){

                            }
                        }

                        If_hitInBUffer_move_theNode_ahead_in_writerequest(ssd, ssd->dram->mt->buffer_DFTL_UNCOM, buffer_node);//this function has finished the hit add in buffer_DFTL_UNCOM
                    //here, just check the moving result.
                    if(buffer_node->group != ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head->group){

                        ssdsim_msg("write request  moving the node's value %d to DFTL_buffer headr is wrong ", buffer_node->group);
                        while(1){

                        }
                    }

                        //uncompression part, just need to move ahead and then record the hit number
                        req->hit_in_uncompression_buffer_number ++;//this is only for the evict page number.
                        //new compression
                        ssd->dram->mt->buffer_DFTL_UNCOM->write_hit ++;

                    }

                }

                if(temp_node == NULL){
                    printf("bi_list wrong in read_value_request_DFTL_parameters, missing_in_data_buffer_lpn read wrong, then stop\n");
                    while(1){

                    }
                }


                //temp_node = temp_node->nextNode;
                temp_node = temp_node->prevNode; //tail ---->head here.

            }//end of for loop


        }//end of step2

        //step3, after getting all the missing in the mappingtable buffer, then get the evict lpn 
        temp_list = req->missing_in_DFTL_buffer_lpn_data;
        temp_node = temp_list->node_head;//here, there is no affect in head or tail
        //since compression ftl contains not only missing_in_mappingtable buffer, but also related with uncompression, so we should do it at any time, not only depends on temp_list->len.

        //!!!here is very important!!   new!!! only current_time can be changed.
        //new compression  we should know that, when there existing fetching or evicting, then we can ignore the compression and decompression.
        if(temp_list->len == 0){
            if( (req->compression_to_uncompression->len + req->missing_in_DFTL_buffer_lpn_data->len) > 0){
                if(ssd->current_time == req->begin_time){//this current_time == begin_time is very important here.
                    ssd->current_time = ssd->current_time + (req->compression_to_uncompression->len + req->missing_in_DFTL_buffer_lpn_data->len) * COMPRESSION_COST_TIME_ONE_PAGE;

                }
            }
        }



		if(temp_list->len != 0){//the begin of step3
			//compression_compare_DFTL_buffer_remainder_space_with_compression_area_coming(ssd, req);
            //new compression
            new_compression_compare_DFTL_buffer_remainder_space_with_compression_area_coming(ssd, req);
			//            compare_DFTL_buffer_remainder_space_with_missing_in_DFTL_buffer(ssd, req);
		}//end of the step3



        //step4, according to the list values, to value other parameters.
        if(req->missing_in_DFTL_buffer_lpn_data->len != 0){

            req->total_wait_fetch_mappingtable = 1;
            ssd->missing_mapping_and_cannot_get_traceline = 1;

        }
        if(req->should_evict_lpn_data->len != 0){
            req->total_wait_evict = 1;
        }

        req->create_or_notyet_evict_subrequests = 0;
        req->create_or_notyet_fetch_subrequests = 0;

//        return ssd;



    //last step:checking the signs in logically
        if(req->missing_in_data_buffer_lpn_data->len == 0){
            if( (req->total_wait_evict != 0) || ( req->total_wait_fetch_mappingtable != 0) || (ssd->missing_mapping_and_cannot_get_traceline != 0) ){
                ssdsim_msg("1 wrong in write  value request DFTL ");
                while(1){

                }
            }
        }


        if(req->missing_in_DFTL_buffer_lpn_data->len == 0){
            if( (req->total_wait_evict != 0) || ( req->total_wait_fetch_mappingtable != 0) || (ssd->missing_mapping_and_cannot_get_traceline != 0) ){
                ssdsim_msg("2 wrong in write  value request DFTL ");
                while(1){

                }
            }
        }

        if(req->should_evict_lpn_data->len == 0){
            if(req->missing_in_DFTL_buffer_lpn_data->len != 0){
                if( (req->total_wait_evict != 0) || (ssd->missing_mapping_and_cannot_get_traceline == 0) ){
                    ssdsim_msg("3 wrong in write  value request DFTL ");
                    while(1){

                    }
                }

            }
        }




    }//end of missing in data buffer


}//end of COMPRESSION_write_value_request_DFTL_parameters

void write_value_request_DFTL_parameters(struct ssd_info* ssd, struct request* req)
{


    int i = 0;

    struct _bi_node* temp_node = NULL;
    struct _bi_node* temp_node1 = NULL;
    struct _bi_list* temp_list = NULL;
    int temp_node_lpn = -1;
    int temp_node_state = -1;

    struct buffer_group* buffer_node = NULL; 

    if(req->operation != 0){
        printf("logical error\n");
        while(1){

        }
        return;
    }

    temp_list = req->missing_in_data_buffer_lpn_data; 
    //temp_node = temp_list->node_head;// here has been wrong,,,should be node_tail here. 
    temp_node = temp_list->node_head;// just paired with next_node below. 

    if(temp_list->len == 0){
        req->total_wait_evict = 0; // 1 means needing wait, 0 do not have to wait or finishing evicting.
        req->total_wait_fetch_mappingtable = 0; // 1 means needing fetching, 0 dont have to fetch or finish fetching
        req->create_or_notyet_evict_subrequests = 0; // these signs will be restored as 0, after the finishing of the evict subrequests. the same to the fetch process. 
        req->create_or_notyet_fetch_subrequests = 0;
        req->create_or_notyet_normal_subrequests = 1;
        return;
    }else{//write miss in data already display
        //write request have finished step1, get the missing lpn in databuffer and value the parameter.
        //step2,according to the lpn in 'missing_in_data_buffer_lpn_data',check every lpn in DFTL_buffer.


        ssdsim_msg("write request address %lld, lsn %d, length in missing data buffer is %d,", req, req->lsn, temp_list->len);

        if(temp_list->len != 0){//the begin of step2

            for(i = 0; i < temp_list->len; i++){

                temp_node_lpn = temp_node->lpn_and_state[0];
                temp_node_state = temp_node->lpn_and_state[1];
                buffer_node = node_isnot_in_DFTL_buffer(ssd, temp_node_lpn);
                //temp here, should delete later

                if(buffer_node == NULL){

#ifdef SET_INTO_LIMITTED_BUFFER
                    append_on_request_missing_in_DFTL_buffer_lpn_data(req, temp_node_lpn, temp_node_state);
#endif
                    //and increase the read_miss_hit number
                    ssd->dram->mt->buffer_DFTL->write_miss_hit ++;

                }else{

#ifdef SET_INTO_LIMITTED_BUFFER

                    If_hitInBUffer_move_theNode_ahead_in_writerequest(ssd, ssd->dram->mt->buffer_DFTL, buffer_node);
                    traceEntrybuffer_msg("write request hitting, ssd just move it to header, entry lpn is %d", buffer_node->group);
                    traceTranslationbuffer_msg("write request hitting,ssd just move it to the header,  value number is %d ", buffer_node->group);
                    //here, just check the moving result.
                    if(buffer_node != ssd->dram->mt->buffer_DFTL->buffer_head){

                        ssdsim_msg("write request  moving the node's value %d to DFTL_buffer headr is wrong ", buffer_node->group);
                        while(1){

                        }
                    }
#endif
                }


                //just checking the bi_list's validation here.
                if(temp_node == NULL){
                    printf("write request bi_list wrong in read_value_request_DFTL_parameters, missing_in_data_buffer_lpn read wrong, then stop\n");
                    while(1){

                    }
                }

                temp_node = temp_node->nextNode; // head ---> tail here. 

            }// end of for loop
            if(temp_node != NULL){
                printf("bi_list wrong in read_value_request_DFTL_parameters, then stop\n");
                while(1){

                }
            }

        }//end of step2

        else{//never go into here, just for logical checking. since at the begining, we has judged the 
            ssdsim_msg("here will be never happen in write_value_request_DFTL_parameters");
            while(1){

            }

        }


        //step3, after getting all the missing in the mappingtable buffer, then get the evict lpn 
        temp_list = req->missing_in_DFTL_buffer_lpn_data;
        temp_node = temp_list->node_head;

        if(temp_list->len != 0){//the begin of step3
            //            int translation_page_temp[temp_list->len];
            //
            //            for(i = 0; i < temp_list->len; i++ ){
            //                translation_page_temp[i] = -1;
            //            }
            //                for(i = 0; i < temp_list->len; i++ ){
            //                    temp_node1 = temp_node
            //                        temp_node_lpn = temp_node1->lpn_and_state[0];
            //                    temp_node_state = temp_node1->lpn_and_state[1];
            //
            //                    get_translation_page_number(translation_page_temp, i, temp_node_lpn);
            //
            //                    temp_node = temp_node->nextNode;
            //
            //                }

            compare_DFTL_buffer_remainder_space_with_missing_in_DFTL_buffer(ssd, req);
        }//end of the step3



        //step4, according to the list values, to value other parameters.
        if(req->missing_in_DFTL_buffer_lpn_data->len != 0){

            req->total_wait_fetch_mappingtable = 1;
            ssd->missing_mapping_and_cannot_get_traceline = 1;

        }
        if(req->should_evict_lpn_data->len != 0){
            req->total_wait_evict = 1;
        }

        req->create_or_notyet_evict_subrequests = 0;
        req->create_or_notyet_fetch_subrequests = 0;

//        return ssd;

    //last step:checking the signs in logically
        if(req->missing_in_data_buffer_lpn_data->len == 0){
            if( (req->total_wait_evict != 0) || ( req->total_wait_fetch_mappingtable != 0) || (ssd->missing_mapping_and_cannot_get_traceline != 0) ){
                ssdsim_msg("1 wrong in write  value request DFTL ");
                while(1){

                }
            }
        }


        if(req->missing_in_DFTL_buffer_lpn_data->len == 0){
            if( (req->total_wait_evict != 0) || ( req->total_wait_fetch_mappingtable != 0) || (ssd->missing_mapping_and_cannot_get_traceline != 0) ){
                ssdsim_msg("2 wrong in write  value request DFTL ");
                while(1){

                }
            }
        }

        if(req->should_evict_lpn_data->len == 0){
            if(req->missing_in_DFTL_buffer_lpn_data->len != 0){
                if( (req->total_wait_evict != 0) || (ssd->missing_mapping_and_cannot_get_traceline == 0) ){
                    ssdsim_msg("3 wrong in write  value request DFTL ");
                    while(1){

                    }
                }

            }
        }


    }//end of missing in data buffer

}//end of write_value_request_DFTL_parameters


//lxc for compression

void printf_requests_basic_paramters_value (struct ssd_info* ssd,struct request* new_request)
{
    int i, len;
    struct _bi_node* temp_node = NULL;
    struct _bi_list* temp_list = NULL;
    printf("after get all the request parameters value\n");


    printf("/*************whether missing_in_data_buffer_lpn_data part************/\n");
    temp_list = new_request->missing_in_data_buffer_lpn_data; 
    temp_node = temp_list->node_head; 

    if(temp_list->len != 0){

        printf("%d request adress: %lld, has no hit, nohit number is %d\n", new_request->operation, new_request, temp_list->len);
        for(i = 0; i < temp_list->len; i ++){
            printf("%d  ",temp_node->lpn_and_state[0]);
            if(temp_node == NULL){
                printf("wrong in printf_requests_basic_paramters_value \n");
                while(1){

                }
            }

            temp_node = temp_node->nextNode;
        }
        printf("\n");
    }else{
        printf("%d request adress: %lld, no missing in data_buffer\n",new_request->operation, new_request);
    }




    printf("/*************whether missing_in_DFTL_buffer_lpn_data************/\n");

    temp_list = new_request->missing_in_DFTL_buffer_lpn_data; 
    temp_node = temp_list->node_head; 

    if(temp_list->len != 0){

        printf("%d request adress: %lld, has no hit, nohit number is %d\n",new_request->operation, new_request, temp_list->len);
        for(i = 0; i < temp_list->len; i ++){
            printf("%d  ",temp_node->lpn_and_state[0]);
            if(temp_node == NULL){
                printf("wrong in printf_requests_basic_paramters_value \n");
                while(1){

                }
            }

            temp_node = temp_node->nextNode;
        }
        printf("\n");
    }else{
        printf("%d request adress: %lld, no missing in DFTL_buffer\n",new_request->operation, new_request);
    }




    printf("/*************whether should_evict_lpn_data*************/\n");

    temp_list = new_request->should_evict_lpn_data; 
    temp_node = temp_list->node_head; 

    if(temp_list->len != 0){

        printf("%d request adress: %lld, has no hit, nohit number is %d\n",new_request->operation, new_request, temp_list->len);
        for(i = 0; i < temp_list->len; i ++){
            printf("%d  ",temp_node->lpn_and_state[0]);
            if(temp_node == NULL){
                printf("printf_requests_basic_paramters_value \n");
                while(1){

                }
            }

            temp_node = temp_node->nextNode;
        }
        printf("\n");
    }else{
        printf("%d request adress: %lld, no evict lpn should do in DFTL buffer\n",new_request->operation, new_request);
    }


}//end of printf_requests_basic_paramters_value 



//lxc for compression
// this function is to value 'four DFTL parametersi(4 numbers)', 'missing_mapping_and_cannot_get_traceline' , 'missing_in_data_buffer_lpn, missing_in_DFTL_buffer_lpn'  by the value of need_distr_flag.  'value_request_DFTL_parameters_for_distribute_subrequests' function  is preparing for 'DFTL_deal_in_distributing_subrequests'function. 
// value total 7 parameters for DFTL
 struct ssd_info* value_request_DFTL_parameters_for_distribute_subrequests(struct ssd_info* ssd)
{
    struct request* req;
    struct sub_request* sub;
    req = ssd->request_tail;

    if(req == NULL){
        printf("now no any request in ssd\n");
        //return ssd;
        while(1){

        }
    }


    if(req->operation == READ ){// value 4 read request parameters and 2 lists.

#ifdef COMPRESSION_MAPPING 
        COMPRESSION_read_value_request_DFTL_parameters(ssd, req);
#else
        read_value_request_DFTL_parameters(ssd, req);
#endif


        ssdsim_msg("outside read request address %lld, lsn %d, length in missing data buffer is %d,", req, req->lsn, req->missing_in_data_buffer_lpn_data->len);
#ifdef compression_relevant
        printf_requests_basic_paramters_value(ssd, req);
#endif

    }else{//now is write request,  value 4 read request parameters and 2 lists.

#ifdef COMPRESSION_MAPPING 
        COMPRESSION_write_value_request_DFTL_parameters(ssd, req);
#else
        write_value_request_DFTL_parameters(ssd, req);
#endif

#ifdef compression_relevant
        printf_requests_basic_paramters_value(ssd, req);
#endif
    }


    return ssd;

    
}//value_request_DFTL_parameters_for_distribute_subrequests


//lxc for compression
//return deleted subs numbers.
int delete_subs_in_this_req(struct ssd_info* ssd, struct request* req)
{

	struct sub_request * temp_sub = NULL, *temp_sub1 = NULL;
	int i = 0, j= 0, k = 0;

	//for new mapping table part , write requests, before deleting, we should update all the write request related mapping table!!!
	//*********************************************//

	int* temp_arra = NULL;
	int* temp_arra1 = NULL;
    int page_total_number = ssd->parameter->page_block * ssd->parameter->block_plane * ssd->parameter->plane_die * ssd->parameter->die_chip * ssd->parameter->chip_num;
	int before_compression_size = ssd->parameter->page_capacity; 

    int original_related_size = 0, after_updating_adding_size = 0;

#ifdef COMPRESSION_MAPPING 
	unsigned int*  translation_page_data_before_compression = (unsigned int*) malloc (sizeof(unsigned int)* NUM_ENTRIES_PER_DIR);
	unsigned int*    translation_page_data_after_compression = (unsigned int*) malloc (sizeof(unsigned int)* NUM_ENTRIES_PER_DIR);
	memset(translation_page_data_before_compression , 0, sizeof(unsigned int)* NUM_ENTRIES_PER_DIR );
	memset(translation_page_data_after_compression , 0, sizeof(unsigned int)* NUM_ENTRIES_PER_DIR );


///******************************************updating the mapping table after every request operation*********************//
    if(req->operation == 0){	
        if(req->subs != NULL){
            if(req->subs->wait_fetch_mappingtable_sign != 1){
                //step1, get subrequest's lpn number
                temp_sub = req->subs;
                while(temp_sub != NULL){

                    i++;
                    temp_sub = temp_sub->next_subs;
                }

                //step2, get subrequest's lpn 
                if(i > 0){
                    temp_arra = (int*)malloc(sizeof(int)*i);
                    temp_arra1 = (int*)malloc(sizeof(int)*i);
                    memset(temp_arra, -1, sizeof(int)*i);
                    memset(temp_arra1,0 , sizeof(int)*i);
                    temp_sub = req->subs;
                    for(j = 0; j < i; j++){
                        temp_arra[j] = temp_sub->lpn;
                        temp_sub = temp_sub->next_subs;
                    }

                }
                //step 3, get no repeat translation page number  NUM_ENTRIES_PER_DIR 

                if(i > 0){
                    //temp_arra1[0] = temp_arra[0] /  ssd->parameter->page_capacity;
                    temp_arra1[0] = temp_arra[0] / NUM_ENTRIES_PER_DIR;
                    if(temp_arra1[0] > page_total_number ){
                        ssdsim_msg("1 compression itself wrong in delte_subs_in_this_request")
                            while(1){

                            }
                    }

                }
                if(i > 1){
                    for(j = 0; j < i; j++){
                        int temp_value = temp_arra[j] / NUM_ENTRIES_PER_DIR;
                        int flag = 0;
                        if(temp_value > page_total_number ){
                            ssdsim_msg("2 compression itself wrong in delte_subs_in_this_request")
                                while(1){

                                }
                        }

                        for(k = 0; k < j; k++){
                            if(temp_value == temp_arra[k]/ NUM_ENTRIES_PER_DIR){
                                flag = 1;
                                break;
                            }
                        }
                        if(flag == 1){
                            temp_arra1[j] = -1;
                            ssdsim_msg("the  %d number -1 value is %d", j, temp_arra1[j]);
                        }else{

                            temp_arra1[j] = temp_arra[j] /NUM_ENTRIES_PER_DIR;
                            if(temp_arra1[j] > page_total_number ){
                                ssdsim_msg("5 compression itself wrong in delte_subs_in_this_request")
                                    while(1){

                                    }
                            }
                        }
                    }
                }//end of if i> 1


                //step4, update real compression size;
                for(j = 0; j < i; j ++){
                    if(temp_arra1[j] != -1){
                        //unsigned int* translation_page_data_before_compression;
                        if(temp_arra1[j] > page_total_number ){
                            ssdsim_msg("4 compression itself wrong in delte_subs_in_this_request")
                                while(1){

                                }
                        }
                        original_related_size += ssd->dram->mt->compression_size[temp_arra1[j]];

                        for(k = 0; k < NUM_ENTRIES_PER_DIR; k ++){
                            *(translation_page_data_before_compression +  k) = ssd->dram->map->map_entry[(NUM_ENTRIES_PER_DIR * j + k)].pn;
                        }
                        ssd->dram->mt->compression_size[temp_arra1[j]] = xmatchCompress((unsigned char*)translation_page_data_before_compression, before_compression_size, (unsigned char*)translation_page_data_after_compression);

                        if(ssd->dram->mt->compression_size[temp_arra1[j]] > ssd->parameter->page_capacity){
                            ssdsim_msg("3 compression itself wrong here!!!!in deletesubs in this request");
                            while(1){

                            }

                        }

                        after_updating_adding_size += ssd->dram->mt->compression_size[temp_arra1[j]] ;
                    }

                }//end of step4's for(j = 0;  compression

                if(temp_arra != NULL){
                    free(temp_arra);
                }

                if(temp_arra1 != NULL){
                    free(temp_arra1);
                }


                free(translation_page_data_before_compression);
                free(translation_page_data_after_compression);
                ssdsim_msg("here after request lsn %lld, size %d, begin_time %lld, operation %d, after updating size is %d, before this request total size is %d", req->lsn, req->size, req->begin_time, req->operation, after_updating_adding_size, original_related_size);
                if( (after_updating_adding_size - original_related_size) >= 0 ){

                    ssd->dram->mt->now_cached_dir_slots_sizein_byte += (after_updating_adding_size - original_related_size);
                    if(ssd->dram->mt->now_cached_dir_slots_sizein_byte >  ssd->dram->mt->max_cached_dir_slots_sizein_byte){
                        ssdsim_msg("next time, we should know the cutting size next is %d", (ssd->dram->mt->now_cached_dir_slots_sizein_byte >  ssd->dram->mt->max_cached_dir_slots_sizein_byte));

                    }
                }else{
                    ssd->dram->mt->now_cached_dir_slots_sizein_byte -= (original_related_size - after_updating_adding_size);

                }
                ssd->changing_size_during_the_runnig_size_in_byte_mostshouldbeadding += (after_updating_adding_size - original_related_size);
            }//end of the first three if only for compression mapping table
        }
    }

#endif

///***********************************for new mapping table part compression*******updating the mapping table after every request operation*********************//







//	//for write requests, before deleting, we should update all the write request related mapping table!!!
//	//*********************************************//
//
//	int* temp_arra = NULL;
//	int* temp_arra1 = NULL;
//    int page_total_number = ssd->parameter->page_block * ssd->parameter->block_plane * ssd->parameter->plane_die * ssd->parameter->die_chip * ssd->parameter->chip_num;
//	int before_compression_size = ssd->parameter->page_capacity; 
//
//#ifdef COMPRESSION_MAPPING 
//	unsigned int*  translation_page_data_before_compression = (unsigned int*) malloc (sizeof(unsigned int)* NUM_ENTRIES_PER_DIR);
//	unsigned int*    translation_page_data_after_compression = (unsigned int*) malloc (sizeof(unsigned int)* NUM_ENTRIES_PER_DIR);
//	memset(translation_page_data_before_compression , 0, sizeof(unsigned int)* NUM_ENTRIES_PER_DIR );
//	memset(translation_page_data_after_compression , 0, sizeof(unsigned int)* NUM_ENTRIES_PER_DIR );
//
//
/////******************************************updating the mapping table after every request operation*********************//
//    if(req->operation == 0){	
//        if(req->subs != NULL){
//            if(req->subs->wait_fetch_mappingtable_sign != 1){
//                //step1, get subrequest's lpn number
//                temp_sub = req->subs;
//                while(temp_sub != NULL){
//
//                    i++;
//                    temp_sub = temp_sub->next_subs;
//                }
//
//                //step2, get subrequest's lpn 
//                if(i > 0){
//                    temp_arra = (int*)malloc(sizeof(int)*i);
//                    temp_arra1 = (int*)malloc(sizeof(int)*i);
//                    memset(temp_arra, -1, sizeof(int)*i);
//                    memset(temp_arra1,0 , sizeof(int)*i);
//                    temp_sub = req->subs;
//                    for(j = 0; j < i; j++){
//                        temp_arra[j] = temp_sub->lpn;
//                        temp_sub = temp_sub->next_subs;
//                    }
//
//                }
//                //step 3, get no repeat translation page number  NUM_ENTRIES_PER_DIR 
//
//                if(i > 0){
//                    //temp_arra1[0] = temp_arra[0] /  ssd->parameter->page_capacity;
//                    temp_arra1[0] = temp_arra[0] / NUM_ENTRIES_PER_DIR;
//                    if(temp_arra1[0] > page_total_number ){
//                        ssdsim_msg("1 compression itself wrong in delte_subs_in_this_request")
//                            while(1){
//
//                            }
//                    }
//
//                }
//                if(i > 1){
//                    for(j = 0; j < i; j++){
//                        int temp_value = temp_arra[j] / NUM_ENTRIES_PER_DIR;
//                        int flag = 0;
//                        if(temp_value > page_total_number ){
//                            ssdsim_msg("2 compression itself wrong in delte_subs_in_this_request")
//                                while(1){
//
//                                }
//                        }
//
//                        for(k = 0; k < j; k++){
//                            if(temp_value == temp_arra[k]/ NUM_ENTRIES_PER_DIR){
//                                flag = 1;
//                                break;
//                            }
//                        }
//                        if(flag == 1){
//                            temp_arra1[j] = -1;
//                            ssdsim_msg("the  %d number -1 value is %d", j, temp_arra1[j]);
//                        }else{
//
//                            temp_arra1[j] = temp_arra[j] /NUM_ENTRIES_PER_DIR;
//                            if(temp_arra1[j] > page_total_number ){
//                                ssdsim_msg("5 compression itself wrong in delte_subs_in_this_request")
//                                    while(1){
//
//                                    }
//                            }
//                        }
//                    }
//                }//end of if i> 1
//
//
//                //step4, update real compression size;
//                for(j = 0; j < i; j ++){
//                    if(temp_arra1[j] != -1){
//                        //unsigned int* translation_page_data_before_compression;
//                        if(temp_arra1[j] > page_total_number ){
//                            ssdsim_msg("4 compression itself wrong in delte_subs_in_this_request")
//                                while(1){
//
//                                }
//                        }
//
//                        for(k = 0; k < NUM_ENTRIES_PER_DIR; k ++){
//                            *(translation_page_data_before_compression +  k) = ssd->dram->map->map_entry[(NUM_ENTRIES_PER_DIR * j + k)].pn;
//                        }
//                        ssd->dram->mt->compression_size[temp_arra1[j]] = xmatchCompress((unsigned char*)translation_page_data_before_compression, before_compression_size, (unsigned char*)translation_page_data_after_compression);
//
//                        if(ssd->dram->mt->compression_size[temp_arra1[j]] > ssd->parameter->page_capacity){
//                            ssdsim_msg("3 compression itself wrong here!!!!in deletesubs in this request");
//                            while(1){
//
//                            }
//                        }
//                    }
//
//                }//end of step4's for(j = 0;  compression
//
//                if(temp_arra != NULL){
//                    free(temp_arra);
//                }
//
//                if(temp_arra1 != NULL){
//                    free(temp_arra1);
//                }
//
//
//                free(translation_page_data_before_compression);
//                free(translation_page_data_after_compression);
//            }//end of the first three if only for compression mapping table
//        }
//    }
//
//#endif
//
/////******************************************updating the mapping table after every request operation*********************//
	temp_sub = req->subs;
	i = 0;
	while(temp_sub != NULL){
		temp_sub1 = temp_sub->next_subs;

		if(temp_sub->update != NULL){
			free(temp_sub->update->location);
			temp_sub->update->location = NULL;
			free(temp_sub->update);
			temp_sub->update = NULL;
		}
		free(temp_sub->location);
		temp_sub->location = NULL;
		free(temp_sub);
		temp_sub = NULL;
		temp_sub = temp_sub1;

		i++;
	}//end of while
	req->subs = NULL;
traceError_msg("now, finish the deleting of req->subs %lld", req->subs);

	return i;
}//end of delete_subs_in_this_req

//lxc for compression
// return the should_evict_lpn_data's number 

int delete_subs_bilist_in_this_req(struct request* req, struct _bi_list* name)
{
    int number = 0;
    number = (name)->len;
    if(( name)->len != 0){
        del_bi_list(name);
//    if(name != NULL){
//		traceError_msg("since no NULL bilist, so wrong");
//        while(1){
//        }
//    }//name = NULL;
		traceError_msg("now finish the deleting of subbilist %lld, len is 0?", req);
        return number;
    }else{
        ssdsim_msg("wrong in deleting the subs_bilist");
        while(1){
        }

    }
}//end of delete_subs_bilist_in_this_req




//lxc for compression
//this is to delete fetch subrequests from this req, the return value is the evict subrequests number
int delete_fetch_subrequest_from_this_req(struct ssd_info* ssd)
{
    int subs_number1 = 0;
    int subs_number2 = 0;
    struct request *req;
    struct sub_request *sub = NULL;
    struct sub_request * temp_sub = NULL;
    req = ssd->request_tail;
    if(req == NULL){
        ssdsim_msg("wrong in running delete_fetch_subrequest_from_this_req");
        while(1){
        }
    }

if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 1) ){

//step1, delete evict subrequests from req->subs
    subs_number1 = delete_subs_in_this_req(ssd, req);
    if(req->subs != NULL){
        ssdsim_msg("wrong in deleting subs in this req ");
        while(1){

        }
    }

    //step2, delete the req's evict bi_list content.
    subs_number2 = delete_subs_bilist_in_this_req(req,req->missing_in_DFTL_buffer_lpn_data);
    free(req->missing_in_DFTL_buffer_lpn_data);
    req->missing_in_DFTL_buffer_lpn_data = NULL;
    //since the bilist has been deleted into NULL, so we cannot visit the member of deleted bilist
    if(req->missing_in_DFTL_buffer_lpn_data != NULL){
		printf("now after deleting, wrong in deleting missing\n");
		while(1){

		}
	}


//    if(subs_number1 != subs_number2){
//        ssdsim_msg("wrong in deleting the subs");
//        while(1){
//        }
//    }
}

    return subs_number1;
}//end of delete_fetch_subrequest_from_this_req





//lxc for compression
//this is to delete evict subrequests from this req, the return value is the evict subrequests number
int delete_evict_subrequest_from_this_req(struct ssd_info* ssd)
{
    int subs_number1 = 0;
    int subs_number2 = 0;
struct request *req;
    struct sub_request *sub = NULL;
    struct sub_request * temp_sub = NULL;
    req = ssd->request_tail;
    if(req == NULL){
        ssdsim_msg("wrong in running");
        while(1){
        }
    }

if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 1))&&((req->create_or_notyet_evict_subrequests == 1))&&(req->create_or_notyet_fetch_subrequests == 0) ){

//step1, delete evict subrequests from req->subs
    subs_number1 = delete_subs_in_this_req(ssd, req);
    
//step2, delete the req's evict bi_list content.
    subs_number2 = delete_subs_bilist_in_this_req(req,req->should_evict_lpn_data);
    free(req->should_evict_lpn_data);
    req->should_evict_lpn_data = NULL;
if(req->should_evict_lpn_data != NULL){
		printf("now after deleting, wrong in deleting should evict \n");
		while(1){

		}
	}

//    if(subs_number1 != subs_number2){
//        ssdsim_msg("wrong in deleting the subs");
//        while(1){
//        }
//    }
}

    return subs_number1;
}//end of delete_evict_subrequest_from_this_req

//lxc for compression  1 means finished.
int polling_whether_finish_evict_subrequests(struct ssd_info* ssd)
{
    struct request *req;
    struct sub_request *sub;
    int temp_finish_flag = 1;
    req = ssd->request_tail;
    if(req == NULL){
        ssdsim_msg("wrong in running");
        while(1){
            
        };
    }
    if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 1))&&((req->create_or_notyet_evict_subrequests == 1))&&(req->create_or_notyet_fetch_subrequests == 0) ){
        sub = req->subs;
        while(sub != NULL){
            if( (sub->wait_evict_sign_finished == 0)&&(sub->wait_evict_sign == 1)){
                temp_finish_flag = 0;
                break;
            }
            sub = sub->next_subs;

        }//end of while
        if(temp_finish_flag == 1){
            ssdsim_msg("polling_polling_whether_finish_evict_subrequests finished!!can go next step");
        }
        return temp_finish_flag;
    }//end of if
    else{

        ssdsim_msg("wrong in polling_whether_finish_evict_subrequests running");
        while(1){
        }
    }

}//end of polling_whether_finish_evict_subrequests


//lxc for compression
//function for the finish of fetch mappingpage. 1 means finish.
int polling_whether_finish_fetch_subrequests(struct ssd_info* ssd)
{
    struct request *req;
    struct sub_request *sub;
    int temp_finish_flag = 1, i;
    req = ssd->request_tail;
    if(req == NULL){
        ssdsim_msg("wrong in running polling_whether_finish_fetch");
        while(1){

        }
    }


    if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 1) ){

        sub = req->subs;
        for(i = 0; i < req->entry_fetch_page_number; i++){
            // while(sub != NULL){
            if( (sub->wait_fetch_mappingtable_sign_finished == 0)&&(sub->wait_fetch_mappingtable_sign == 1)){
                temp_finish_flag = 0;
                break;
            }
            if(sub == NULL){
                ssdsim_msg("wrong in running polling_whether_finish_fetch");
                while(1){

                }

            }
            sub = sub->next_subs;

        }//end of for loop
        if(temp_finish_flag == 1){
            ssdsim_msg("polling_whether_finish_fetch_subrequests finished!!can go next step");
        }

        return temp_finish_flag;
        }//end of if
        else{

            ssdsim_msg("wrong in polling_whether_finish_evict_subrequests running");
            while(1){
            }
        }

    }//end of polling_whether_finish_fetch_subrequests




struct ssd_info* create_fetch_mappingtable_subrequests(struct ssd_info* ssd)
{
    //how to create readsubrequests, and append them on request.
    struct request *req;
    struct sub_request *sub;
    int temp_i = 0;
    req = ssd->request_tail;
    struct _bi_node* temp_node = NULL;
    struct _bi_list* temp_bilist = NULL;
    if(req == NULL){
        ssdsim_msg("wrong in running in fetch_mappingtable");
        while(1){
        }
    }
    if(req->subs != NULL){
        ssdsim_msg("req subs should be NULL before fetching mappingtable");
        while(1){
        }
    }

    if(req->missing_in_DFTL_buffer_lpn_data->len == 0){
        ssdsim_msg(" wrong in running missing_in_DFTL_buffer_lpn_data");
        while(1){
        }
    }


    //for TRANSLATION_PAGE_UNIT 
#ifdef TRANSLATION_PAGE_UNIT 

    temp_bilist = req->missing_in_DFTL_buffer_lpn_data;
    temp_node = temp_bilist->node_head;

    for(temp_i = 0; temp_i < temp_bilist->len; temp_i++){

        //size is the number of subpages number. for evict mappingtable pages. And fixed sub_size and sub_state.
        creat_sub_request(ssd,temp_node->lpn_and_state[0],4, 0xf, req, MAPPINGTABLE_READ);

        ssdsim_msg("mapppnig read translation value is %d", temp_node->lpn_and_state[0]);
        temp_node = temp_node->nextNode;

		req->entry_fetch_page_number ++;
        ssd->read_count ++;
    }


#endif





    //for ENTRY_UNIT //comments: !!!!!
#ifdef ENTRY_UNIT 

    temp_bilist = req->missing_in_DFTL_buffer_lpn_data;
    temp_node = temp_bilist->node_head;

    int* temp_output_array = (int*)malloc(sizeof(int)*(temp_bilist->len));
    memset(temp_output_array, -1, (sizeof(int)*temp_bilist->len) );
    //step1, get only no-repeating pages.
    get_no_repeat_pages_number_from_lpns(ssd, req, temp_bilist, temp_output_array);


    //step2, creat real page number instead of entry lpn...
    for(temp_i = 0; temp_i < temp_bilist->len; temp_i++){

        //size is the number of subpages number. for evict mappingtable pages. And fixed sub_size and sub_state.
        if(temp_output_array[temp_i] != -1){
            creat_sub_request(ssd, temp_output_array[temp_i], 4, 0xf, req, MAPPINGTABLE_READ);
            req->entry_fetch_page_number ++;
            ssd->read_count ++;
        }
    }


free(temp_output_array);

#endif




}//end of create_fetch_mappingtable_subrequests



//lxc for compression
//return 1 means there are normal_subrequests, 0 means has been serviced by databuffer. the mixture of read and write subrequests creating.
int create_normal_subrequests(struct ssd_info* ssd)
{
    struct request* req;
    req = ssd->request_tail;


    struct _bi_node* temp_node = NULL;
    struct _bi_list* temp_list = NULL;
    int i = 0;

    if(req->subs != NULL){
        ssdsim_msg("req address %lld, subs address %lld, subs should be NULL before creating normal subrequest, not mappingtable pages", req, req->subs);
        while(1){
        }
    }


    traceError_msg("now request %lld response_time %lld", req, req->response_time);
    if(req->response_time  == 0){

        if(req->operation == READ){

            distribute(ssd);

        }else{

            if( (req->missing_in_data_buffer_lpn_data)->len != 0){
                temp_list = req->missing_in_data_buffer_lpn_data;
                temp_node = temp_list->node_head;
                for(i = 0; i < temp_list->len; i++){
                    creat_sub_request(ssd, temp_node->lpn_and_state[0], size(temp_node->lpn_and_state[1]), temp_node->lpn_and_state[1], req, WRITE);
                    if(temp_node == NULL){
                        ssdsim_msg("wrong in create_normal_subrequests");
                        while(1){
                        }
                    }
                    temp_node = temp_node->nextNode;
                }
            }
        }
        return 1;
    }else{
        traceError_msg("now request %lld serviced in databuffer directly", req);
        return 0;

    }
}//end of create_normal_subrequests


//lxc for compression

void get_no_repeat_pages_number_from_lpns(struct ssd_info* ssd, struct request* req,struct _bi_list*  temp_bilist, int* input)
{
    struct _bi_node * temp_sub = NULL, *temp_sub1 = NULL;
    int i = 0, j= 0, k = 0;

    i = temp_bilist->len;
    int* temp_arra = (int*)malloc(sizeof(int)*i);
    int* temp_arra1 = (int*)malloc(sizeof(int)*i);
    //step1, get subrequest's lpn 
    temp_sub = temp_bilist->node_head;

    for(j = 0; j < i; j++){
        temp_arra[j] = temp_sub->lpn_and_state[0];
        temp_sub = temp_sub->nextNode;
    }

    //step2, get no repeat translation page number  
    //input[0] = temp_arra[0] /ssd->parameter->page_capacity;
    input[0] = temp_arra[0] /NUM_ENTRIES_PER_DIR;
    if(temp_bilist->len > 1){
        for(j = 1; j < i; j++){
            int temp_value = temp_arra[j] /NUM_ENTRIES_PER_DIR;
            int flag = 0;

            for(k = 0; k < j; k++){
                if(temp_value == temp_arra[k]/NUM_ENTRIES_PER_DIR){
                    flag = 1;
                    break;
                }
            }
            if(flag == 1){
                // temp_arra1[j] = -1;
                input[j] = -1;

            }else{

                // temp_arra1[j] = temp_arra[j] /ssd->parameter->page_capacity;
                input[j] =  temp_arra[j] /NUM_ENTRIES_PER_DIR;
            }
        }
    }//end of if

	free(temp_arra);
	free(temp_arra1);
}//end of get_no_repeat_pages_number_from_lpns

		

//lxc for compression
//this is for write subrequest..
struct ssd_info* create_evict_mappingtable_subrequests(struct ssd_info* ssd)
{
    //how to create writesubrequest, and append them on request.
    struct request *req;
    struct sub_request *sub;
    int temp_i = 0;
    req = ssd->request_tail;
    struct _bi_node* temp_node = NULL;
    struct _bi_list* temp_bilist = NULL;
    if(req == NULL){
        ssdsim_msg("wrong in running evict_mappingtable");
        while(1){
        }
    }
    if(req->should_evict_lpn_data->len == 0){
        ssdsim_msg("2 wrong in running evict_mappingtable");
        while(1){
        }




    }
    //for TRANSLATION_PAGE_UNIT 
#ifdef TRANSLATION_PAGE_UNIT 

    temp_bilist = req->should_evict_lpn_data;
    temp_node = temp_bilist->node_head;

    for(temp_i = 0; temp_i < temp_bilist->len; temp_i++){

        //size is the number of subpages number. for evict mappingtable pages. And fixed sub_size and sub_state.
        creat_sub_request(ssd,temp_node->lpn_and_state[0],4, 0xf, req, MAPPINGTABLE_WRITE);
        temp_node = temp_node->nextNode;
        ssd->program_count ++;

		req->entry_evict_page_number ++;
    }

#endif





    //for ENTRY_UNIT //comments: firstly, get the translation page instead of entries lpn page!!!!!
#ifdef ENTRY_UNIT 


    temp_bilist = req->should_evict_lpn_data;
    temp_node = temp_bilist->node_head;
    int* temp_output_array = (int*)malloc(sizeof(int)*(temp_bilist->len));
    memset(temp_output_array, -1, (sizeof(int)*temp_bilist->len) );
    //step1, get only no-repeating pages.
    get_no_repeat_pages_number_from_lpns(ssd, req, temp_bilist, temp_output_array);


    //step2, creat real page number instead of entry lpn...
    for(temp_i = 0; temp_i < temp_bilist->len; temp_i++){

        //size is the number of subpages number. for evict mappingtable pages. And fixed sub_size and sub_state.
        if(temp_output_array[temp_i] != -1){
            creat_sub_request(ssd, temp_output_array[temp_i], 4, 0xf, req, MAPPINGTABLE_WRITE);
            req->entry_evict_page_number ++;

            ssd->program_count ++;
        }
    }

	free(temp_output_array); 
#endif
}//end of create_evict_mappingtable_subrequests
//lxc for compression
// return real size if for translation page here.

int get_realsize_inbyte_byposition_in_mt(struct ssd_info* ssd, int lpn) 
{

int size = 0;

#ifdef ENTRY_UNIT
return ssd->dram->mt->mapping_entry_size;

#endif

// TRANSLATION_PAGE_UNIT and COMPRESSION_MAPPING  should be together
#ifdef TRANSLATION_PAGE_UNIT 


#ifdef COMPRESSION_MAPPING 
//step 1 get the lpn's value.

size = ssd->dram->mt->compression_size[lpn];
       if(size > ssd->parameter->page_capacity){
            ssdsim_msg("wrong in  get realsize in size");
            while(1){

            }
        }

//step 2, then compression it to get the size.
return size;
#endif


return ssd->parameter->page_capacity; 


#endif



}//end of get_realsize_inbyte_byposition_in_mt


//lxc for compression
//this is special for insert mappingtable buffer_UNCOM.
int insert2DFTL_UNCOM_buffer(struct ssd_info* ssd, struct request* req)
{

    struct buffer_group *buffer_node = NULL, *pt, *new_node = NULL, key;
    struct _bi_node* temp_node = NULL;
    struct _bi_list* temp_bilist = NULL;
    int i;
    temp_bilist = req->missing_in_DFTL_buffer_lpn_data;
    temp_node = temp_bilist->node_head;

    if(temp_bilist->len == 0){
        ssdsim_msg("no any inserted subrequest, why do insert2DFTL_buffer");

        while(1){
        }
    }
    //for TRANSLATION_PAGE_UNIT 
#ifdef TRANSLATION_PAGE_UNIT 
    for(i = 0; i < temp_bilist->len; i ++){
        //step1 content insert

        /******************************************************************************
         *生成一个buffer node，根据这个页的情况分别赋值个各个成员，添加到队首和二叉树中
         *******************************************************************************/
        new_node = NULL;
        new_node = (struct buffer_group *) malloc(sizeof(struct buffer_group));
        alloc_assert(new_node, "buffer_group_node");
        memset(new_node, 0, sizeof(struct buffer_group));

        new_node->group = temp_node->lpn_and_state[0];
        new_node->stored = temp_node->lpn_and_state[1];
        new_node->dirty_clean = temp_node->lpn_and_state[1];
        new_node->LRU_link_pre = NULL;
        new_node->LRU_link_next = ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head;
        if (ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head != NULL) {
            ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head->LRU_link_pre = new_node;
        } else {
            ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail = new_node;
        }
        ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head = new_node;
        new_node->LRU_link_pre = NULL;
        //above operations are all for lru queue, next function is for Tree.
        avlTreeAdd(ssd->dram->mt->buffer_DFTL_UNCOM, (TREE_NODE *) new_node);

        //step2 size change . the size is not only in DFTL_buffer but in GTD structure.

        ssd->dram->mt->buffer_DFTL_UNCOM->buffer_sector_count += 1 ;// once only one total translation page here.
        ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte += ssd->parameter->page_capacity;
        if(ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte % ssd->parameter->page_capacity != 0 ){
            ssdsim_msg("uncompression buffer size is not the integer");
            while(1){

            }
        }
        //step3 , loop circulation

        temp_node = temp_node->nextNode;

    }//end of for loop
#endif


}//insert2DFTL_UNCOM_buffer

//lxc for compression
//this is special for insert mappingtable buffer.
int new_insert2DFTL_buffer(struct ssd_info* ssd, struct request* req)
{

    struct buffer_group *buffer_node = NULL, *pt, *new_node = NULL, key;
    struct _bi_node* temp_node = NULL;
    struct _bi_list* temp_bilist = NULL;
    int i;
    temp_bilist = req->missing_in_DFTL_buffer_lpn_data;
    temp_node = temp_bilist->node_head;

    if(temp_bilist->len == 0){
        ssdsim_msg("no any inserted subrequest, why do insert2DFTL_buffer");

        while(1){
        }
    }
    //for TRANSLATION_PAGE_UNIT 
#ifdef TRANSLATION_PAGE_UNIT 
    for(i = 0; i < temp_bilist->len; i ++){
        //step1 content insert

        /******************************************************************************
         *生成一个buffer node，根据这个页的情况分别赋值个各个成员，添加到队首和二叉树中
         *******************************************************************************/
        new_node = NULL;
        new_node = (struct buffer_group *) malloc(sizeof(struct buffer_group));
        alloc_assert(new_node, "buffer_group_node");
        memset(new_node, 0, sizeof(struct buffer_group));

        new_node->group = temp_node->lpn_and_state[0];
        new_node->stored = temp_node->lpn_and_state[1];
        new_node->dirty_clean = temp_node->lpn_and_state[1];
        new_node->LRU_link_pre = NULL;
        new_node->LRU_link_next = ssd->dram->mt->buffer_DFTL->buffer_head;
        if (ssd->dram->mt->buffer_DFTL->buffer_head != NULL) {
            ssd->dram->mt->buffer_DFTL->buffer_head->LRU_link_pre = new_node;
        } else {
            ssd->dram->mt->buffer_DFTL->buffer_tail = new_node;
        }
        ssd->dram->mt->buffer_DFTL->buffer_head = new_node;
        new_node->LRU_link_pre = NULL;
        //above operations are all for lru queue, next function is for Tree.
        avlTreeAdd(ssd->dram->mt->buffer_DFTL, (TREE_NODE *) new_node);

        //step2 size change . the size is not only in DFTL_buffer but in GTD structure.

        ssd->dram->mt->buffer_DFTL->buffer_sector_count += 1 ;// once only one total translation page here.
        ssd->dram->mt->now_cached_dir_slots_sizein_byte += get_realsize_inbyte_byposition_in_mt(ssd,temp_node->lpn_and_state[0]);


        traceTranslationbuffer_msg("translation page size once adding size is %d, now cached size is %d, all in byte", get_realsize_inbyte_byposition_in_mt(ssd,temp_node->lpn_and_state[0]) , ssd->dram->mt->now_cached_dir_slots_sizein_byte );

        //step3 , loop circulation

        temp_node = temp_node->nextNode;

    }//end of for loop



    return temp_bilist->len;
#endif


}//end of new_insert2DFTL_buffer







//lxc for compression
//this is special for insert mappingtable buffer.
int insert2DFTL_buffer(struct ssd_info* ssd, struct request* req)
{

    struct buffer_group *buffer_node = NULL, *pt, *new_node = NULL, key;
    struct _bi_node* temp_node = NULL;
    struct _bi_list* temp_bilist = NULL;
    int i;
    temp_bilist = req->missing_in_DFTL_buffer_lpn_data;
    temp_node = temp_bilist->node_head;

    if(temp_bilist->len == 0){
        ssdsim_msg("no any inserted subrequest, why do insert2DFTL_buffer");

        while(1){
        }
    }
    //for TRANSLATION_PAGE_UNIT 
#ifdef TRANSLATION_PAGE_UNIT 
    for(i = 0; i < temp_bilist->len; i ++){
        //step1 content insert

        /******************************************************************************
         *生成一个buffer node，根据这个页的情况分别赋值个各个成员，添加到队首和二叉树中
         *******************************************************************************/
        new_node = NULL;
        new_node = (struct buffer_group *) malloc(sizeof(struct buffer_group));
        alloc_assert(new_node, "buffer_group_node");
        memset(new_node, 0, sizeof(struct buffer_group));

        new_node->group = temp_node->lpn_and_state[0];
        new_node->stored = temp_node->lpn_and_state[1];
        new_node->dirty_clean = temp_node->lpn_and_state[1];
        new_node->LRU_link_pre = NULL;
        new_node->LRU_link_next = ssd->dram->mt->buffer_DFTL->buffer_head;
        if (ssd->dram->mt->buffer_DFTL->buffer_head != NULL) {
            ssd->dram->mt->buffer_DFTL->buffer_head->LRU_link_pre = new_node;
        } else {
            ssd->dram->mt->buffer_DFTL->buffer_tail = new_node;
        }
        ssd->dram->mt->buffer_DFTL->buffer_head = new_node;
        new_node->LRU_link_pre = NULL;
        //above operations are all for lru queue, next function is for Tree.
        avlTreeAdd(ssd->dram->mt->buffer_DFTL, (TREE_NODE *) new_node);

        //step2 size change . the size is not only in DFTL_buffer but in GTD structure.

        ssd->dram->mt->buffer_DFTL->buffer_sector_count += 1 ;// once only one total translation page here.
        ssd->dram->mt->now_cached_dir_slots_sizein_byte += get_realsize_inbyte_byposition_in_mt(ssd,temp_node->lpn_and_state[0]);


        traceTranslationbuffer_msg("translation page size once adding size is %d, now cached size is %d, all in byte", get_realsize_inbyte_byposition_in_mt(ssd,temp_node->lpn_and_state[0]) , ssd->dram->mt->now_cached_dir_slots_sizein_byte );

        //step3 , loop circulation

        temp_node = temp_node->nextNode;

    }//end of for loop



    return temp_bilist->len;
#endif

    //for ENTRY_UNIT //comments: firstly, get the translation page instead of entries lpn page!!!!!
#ifdef ENTRY_UNIT 

    for(i = 0; i < temp_bilist->len; i ++){
        //step1 content insert

        /******************************************************************************
         *生成一个buffer node，根据这个页的情况分别赋值个各个成员，添加到队首和二叉树中
         *******************************************************************************/
        new_node = NULL;
        new_node = (struct buffer_group *) malloc(sizeof(struct buffer_group));
        alloc_assert(new_node, "buffer_group_node");
        memset(new_node, 0, sizeof(struct buffer_group));

        new_node->group = temp_node->lpn_and_state[0];
        new_node->stored = temp_node->lpn_and_state[1];
        new_node->dirty_clean = temp_node->lpn_and_state[1];
        new_node->LRU_link_pre = NULL;
        new_node->LRU_link_next = ssd->dram->mt->buffer_DFTL->buffer_head;
        if (ssd->dram->mt->buffer_DFTL->buffer_head != NULL) {
            ssd->dram->mt->buffer_DFTL->buffer_head->LRU_link_pre = new_node;
        } else {
            ssd->dram->mt->buffer_DFTL->buffer_tail = new_node;
        }
        ssd->dram->mt->buffer_DFTL->buffer_head = new_node;
        new_node->LRU_link_pre = NULL;
        //above operations are all for lru queue, next function is for Tree.
        avlTreeAdd(ssd->dram->mt->buffer_DFTL, (TREE_NODE *) new_node);

        //step2 size change . the size is not only in DFTL_buffer but in GTD structure.

        ssd->dram->mt->buffer_DFTL->buffer_sector_count += 1 ;// once only one total translation page here.
        ssd->dram->mt->now_cached_entries_sizein_byte += get_realsize_inbyte_byposition_in_mt(ssd,temp_node->lpn_and_state[0]);


        traceTranslationbuffer_msg("entry size once adding size is %d, now cached size is %d, all in byte",get_realsize_inbyte_byposition_in_mt(ssd,temp_node->lpn_and_state[0]), ssd->dram->mt->now_cached_entries_sizein_byte);
        //step3 , loop circulation

        temp_node = temp_node->nextNode;

    }//end of for loop

    return temp_bilist->len;

#endif

}//end of insert2DFTL_buffer

//lxc for compression
//this function is for pop in the DFTL buffer, and get the evict number or size?? now page number is enough

int Pop_DFTL_buffer(struct ssd_info* ssd, struct request* req)
{


    struct buffer_group *buffer_node = NULL, *pt, *new_node = NULL, key;
    struct _bi_node* temp_node = NULL;
    struct _bi_list* temp_bilist = NULL;
    int i;

    int page_num = ssd->parameter->page_block * ssd->parameter->block_plane * ssd->parameter->plane_die * ssd->parameter->die_chip * ssd->parameter->chip_num;


    temp_bilist = req->should_evict_lpn_data;
    temp_node = temp_bilist->node_tail;

    if(temp_bilist->len == 0){
        ssdsim_msg("no any inserted subrequest, why do pop_DFTL_buffer");

        while(1){
        }
    }


    //for TRANSLATION_PAGE_UNIT 
#ifdef TRANSLATION_PAGE_UNIT 
    for(i = 0; i < temp_bilist->len; i ++){//this is for all the evict_subrequests in DFTL_buffer

        if( (ssd->dram->mt->buffer_DFTL->buffer_head == NULL )&&(ssd->dram->mt->buffer_DFTL->buffer_tail = NULL) ){
            ssdsim_msg("no any subrequests in DFTL_buffer, why do pop_DFTL_buffer");

            while(1){
            }
        }

        traceTranslationbuffer_msg("delete the DFTL_buffer translation page number is %d", ssd->dram->mt->buffer_DFTL->buffer_tail->group);
        //step1 content evict
        pt = ssd->dram->mt->buffer_DFTL->buffer_tail;
        //judge the should_evict and buffer_tail
        if(pt->group != temp_node->lpn_and_state[0]){
            ssdsim_msg("the record evict cannot match the buffer tail");

            while(1){
            }
        }

        avlTreeDel(ssd->dram->mt->buffer_DFTL, (TREE_NODE *) pt);
        if (ssd->dram->mt->buffer_DFTL->buffer_head->LRU_link_next == NULL) {
            ssd->dram->mt->buffer_DFTL->buffer_head = NULL;
            ssd->dram->mt->buffer_DFTL->buffer_tail = NULL;
        } else {
            ssd->dram->mt->buffer_DFTL->buffer_tail = ssd->dram->mt->buffer_DFTL->buffer_tail->LRU_link_pre;
            ssd->dram->mt->buffer_DFTL->buffer_tail->LRU_link_next = NULL;
        }
        pt->LRU_link_next = NULL;
        pt->LRU_link_pre = NULL;
        AVL_TREENODE_FREE(ssd->dram->mt->buffer_DFTL, (TREE_NODE *) pt);
        pt = NULL;






        //step2 size change . the size is not only in DFTL_buffer but in GTD structure.
        //size contains two parts, buffer number and real size in GTD.

        //        ssd->dram->buffer->buffer_sector_count -= 1 ;// once only one total translation page here.
        ssd->dram->mt->now_cached_dir_slots_sizein_byte -= get_realsize_inbyte_byposition_in_mt(ssd,temp_node->lpn_and_state[0]);
traceTranslationbuffer_msg("once deletion size is %d, now cached size is %d, all in byte", get_realsize_inbyte_byposition_in_mt(ssd,temp_node->lpn_and_state[0]) , ssd->dram->mt->now_cached_dir_slots_sizein_byte );

 //checking the lpn number
        if(temp_node->lpn_and_state[0] > page_num){
            ssdsim_msg("wrong in  wrong lpn number");
            while(1){

            }
        }

        //step3, move to another buffer_node
        if(temp_node == NULL){
            ssdsim_msg("wrong in pop_DFTL_buffer");
            while(1){

            }
        }
        temp_node = temp_node->prevNode;

    }//end of for loop

    return temp_bilist->len;
#endif


    //for ENTRY_UNIT //comments: firstly!!!!!
#ifdef ENTRY_UNIT 

    for(i = 0; i < temp_bilist->len; i ++){//this is for all the evict_subrequests in DFTL_buffer

        if( (ssd->dram->mt->buffer_DFTL->buffer_head == NULL )&&(ssd->dram->mt->buffer_DFTL->buffer_tail = NULL) ){
            ssdsim_msg("no any subrequests in DFTL_buffer, why do pop_DFTL_buffer");

            while(1){
            }
        }

        traceEntrybuffer_msg("delete the DFTL_buffer entry lpn  is %d", ssd->dram->mt->buffer_DFTL->buffer_tail->group);
        //step1 content evict
        pt = ssd->dram->mt->buffer_DFTL->buffer_tail;
        //judge the should_evict and buffer_tail
        if(pt->group != temp_node->lpn_and_state[0]){
            ssdsim_msg("the record evict cannot match the buffer tail");

            while(1){
            }
        }

        avlTreeDel(ssd->dram->mt->buffer_DFTL, (TREE_NODE *) pt);
        if (ssd->dram->mt->buffer_DFTL->buffer_head->LRU_link_next == NULL) {
            ssd->dram->mt->buffer_DFTL->buffer_head = NULL;
            ssd->dram->mt->buffer_DFTL->buffer_tail = NULL;
        } else {
            ssd->dram->mt->buffer_DFTL->buffer_tail = ssd->dram->mt->buffer_DFTL->buffer_tail->LRU_link_pre;
            ssd->dram->mt->buffer_DFTL->buffer_tail->LRU_link_next = NULL;
        }
        pt->LRU_link_next = NULL;
        pt->LRU_link_pre = NULL;
        AVL_TREENODE_FREE(ssd->dram->mt->buffer_DFTL, (TREE_NODE *) pt);
        pt = NULL;






        //step2 size change . the size is not only in DFTL_buffer but in GTD structure.
        //size contains two parts, buffer number and real size in GTD.

        //        ssd->dram->buffer->buffer_sector_count -= 1 ;// once only one total translation page here.
        ssd->dram->mt->now_cached_entries_sizein_byte -= get_realsize_inbyte_byposition_in_mt(ssd,temp_node->lpn_and_state[0]);

        ssd->dram->mt->buffer_DFTL->buffer_sector_count += 1 ;// once only one total translation page here.
        traceTranslationbuffer_msg("entry size once deletion size is %d, now cached size is %d, all in byte",get_realsize_inbyte_byposition_in_mt(ssd,temp_node->lpn_and_state[0]), ssd->dram->mt->now_cached_entries_sizein_byte);

        //step3, move to another buffer_node
        if(temp_node == NULL){
            ssdsim_msg("wrong in pop_DFTL_buffer");
            while(1){

            }
        }
        temp_node = temp_node->prevNode;

    }//end of for loop

    return temp_bilist->len;

#endif


}//end of Pop_DFTL_buffer

//lxc for compression
int move_pages_from_uncompression_to_compression(struct ssd_info* ssd, struct request* request)
{
    int i, temp_lpn, temp_state;
    struct buffer_group* temp_tail, *new_node = NULL, *temp_tail_next ;


    


    temp_tail = ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail;
    if(request->the_number_of_evicting_from_uncom_to_com != 0){

        for(i = 0; i < request->the_number_of_evicting_from_uncom_to_com; i++){


            //step1, move them to the head of the compression. the costing time is not here.and we always update the newest translation pages size here, so no any compression here.
            /******************************************************************************
             *create a new buffer node，and add to list head and avlTree
             *******************************************************************************/

            new_node = NULL;
            new_node = (struct buffer_group *) malloc(sizeof(struct buffer_group));
            alloc_assert(new_node, "buffer_group_node");
            memset(new_node, 0, sizeof(struct buffer_group));

            new_node->group = temp_tail->group;
            new_node->stored = temp_tail->stored;
            new_node->dirty_clean = temp_tail->dirty_clean;
            new_node->LRU_link_pre = NULL;
            new_node->LRU_link_next = ssd->dram->mt->buffer_DFTL->buffer_head;
            if (ssd->dram->mt->buffer_DFTL->buffer_head != NULL) {
                ssd->dram->mt->buffer_DFTL->buffer_head->LRU_link_pre = new_node;
            } else {
                ssd->dram->mt->buffer_DFTL->buffer_tail = new_node;
            }
            ssd->dram->mt->buffer_DFTL->buffer_head = new_node;
            new_node->LRU_link_pre = NULL;
            //checking the queue
                if( ssd->dram->mt->buffer_DFTL->buffer_tail != new_node){
                    if(new_node->LRU_link_next == NULL){
                        ssdsim_msg("wrong in adding node in queue 1");
                        while(1){

                        }
                    }
            }



            //above operations are all for lru queue, next function is for Tree.
            avlTreeAdd(ssd->dram->mt->buffer_DFTL, (TREE_NODE *) new_node);

            //step1.1 size change . the size is not only in DFTL_buffer but in GTD structure.

            ssd->dram->mt->buffer_DFTL->buffer_sector_count += 1 ;// once only one total translation page here.
            ssd->dram->mt->now_cached_dir_slots_sizein_byte += ssd->dram->mt->compression_size[temp_tail->group];

            //step2, delete the tail nodes in uncompression part.
            temp_tail_next = temp_tail->LRU_link_pre;

            avlTreeDel(ssd->dram->mt->buffer_DFTL_UNCOM, (TREE_NODE *) temp_tail);
            if (ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head->LRU_link_next == NULL) {
                ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head = NULL;
                ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail = NULL;
            } else {
                ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail = ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail->LRU_link_pre;
                ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail->LRU_link_next = NULL;
            }
            temp_tail->LRU_link_next = NULL;
            temp_tail->LRU_link_pre = NULL;
            AVL_TREENODE_FREE(ssd->dram->mt->buffer_DFTL_UNCOM, (TREE_NODE *) temp_tail);
            //step2.1 change size.

            ssd->dram->mt->buffer_DFTL_UNCOM->buffer_sector_count -= 1 ;// once only one total translation page here.
            ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte -= ssd->parameter->page_capacity;



            temp_tail = temp_tail_next;
        }//end of for loop
    }
    return i;
}//end of move_pages_from_uncompression_to_compression




//lxc for compression

void new_compression_evict_uncompression_from_tail_part(struct ssd_info* ssd, int evict_size)
{
    int i, temp_lpn, temp_state;
    struct buffer_group* temp_tail, *new_node = NULL, *temp_tail_next ;
    int lpn_number = evict_size / MAPPING_ENTRY_SIZE;
    if(evict_size > ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte){
        ssdsim_msg("wrong in pop the uncompression part");
        while(1){

        }

    }
    for(i = 0; i < lpn_number; i ++){

        temp_tail = ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail;

        //delete the tail part.

        avlTreeDel(ssd->dram->mt->buffer_DFTL_UNCOM, (TREE_NODE *) temp_tail);
        if (ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head->LRU_link_next == NULL) {
                ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head = NULL;
                ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail = NULL;
            } else {
                ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail = ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail->LRU_link_pre;
                ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail->LRU_link_next = NULL;
            }
            temp_tail->LRU_link_next = NULL;
            temp_tail->LRU_link_pre = NULL;
            AVL_TREENODE_FREE(ssd->dram->mt->buffer_DFTL_UNCOM, (TREE_NODE *) temp_tail);
            //step2.1 change size.

            ssd->dram->mt->buffer_DFTL_UNCOM->buffer_sector_count -= 1 ;// once only one total translation page here.
            ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte -= MAPPING_ENTRY_SIZE;

}
}//end of new_compression_evict_uncompression_from_tail_part


//lxc for compression
//this new move pages from compression to uncompression is not pages, just get the new entry nodes to uncompression part. only size changing inuncompression part no size changing in compression part.
int new_move_pages_from_compression_to_uncompression(struct ssd_info* ssd, struct request* request)
{

    int i, j;
    struct buffer_group key,  *buffer_node = NULL, *new_node ;
    struct _bi_node* temp_binode = NULL, *temp_binode_pre = NULL;
    struct _bi_list* temp_bilist = NULL;
    temp_bilist = request->missing_in_data_buffer_lpn_data;
    temp_binode = temp_bilist->node_tail;
    int evict_size =  ( (ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte + temp_bilist->len * MAPPING_ENTRY_SIZE) - ssd->dram->mt->uncompression_max_cached_dir_slots_sizein_byte );
//step0, calculate the size of incoming lpn for uncompression part.
//for(j = 0; j < temp_bilist=){
//
//}

        //step0   if there is no enough space in uncompression, deleting the tail in uncompression
    if(temp_bilist->len > 0){
            if(evict_size > 0){
                //begin delete now:
                new_compression_evict_uncompression_from_tail_part(ssd, evict_size);

            }


    }else{
        return 0;

    }



    for(i = 0; i < temp_bilist->len; i ++){
        key.group = temp_binode->lpn_and_state[0];
        buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL,
                (TREE_NODE *) &key);
        if(buffer_node != NULL){

            temp_binode = temp_binode->prevNode;
            continue;


        }else{
            //step1, if there is no enough space for the uncompression part.
            if(ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte > ssd->dram->mt->uncompression_max_cached_dir_slots_sizein_byte){
                ssdsim_msg("wrong in uncompression part");
                while(1){

                }
            }
    
            //step2,add nodes to  uncompression part 
            /******************************************************************************
             *create a new buffer node，and add to list head and avlTree
             *******************************************************************************/

            new_node = NULL;
            new_node = (struct buffer_group *) malloc(sizeof(struct buffer_group));
            alloc_assert(new_node, "buffer_group_node");
            memset(new_node, 0, sizeof(struct buffer_group));

            new_node->group = temp_binode->lpn_and_state[0];
            new_node->stored = temp_binode->lpn_and_state[1];
            new_node->dirty_clean = temp_binode->lpn_and_state[1];
            new_node->LRU_link_pre = NULL;
            new_node->LRU_link_next = ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head;
            if (ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head != NULL) {
                ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head->LRU_link_pre = new_node;
            } else {
                ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail = new_node;
            }
            ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head = new_node;
            new_node->LRU_link_pre = NULL;
            //above operations are all for lru queue, next function is for Tree.
            avlTreeAdd(ssd->dram->mt->buffer_DFTL_UNCOM, (TREE_NODE *) new_node);

            //step1.1 size change . the size is not only in DFTL_buffer but in GTD structure.

            ssd->dram->mt->buffer_DFTL_UNCOM->buffer_sector_count += 1 ;// once only one total translation page here.
            //ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte += ssd->parameter->page_capacity;
            ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte += MAPPING_ENTRY_SIZE;

            //        //step2, delete the appointed nodes instead of tail nodes!!!!!!!!! in compression part.
            //       yy temp_binode_pre = temp_binode->prevNode ;
            //
            //        key.group = temp_binode->lpn_and_state[0];
            //        buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL,
            //                (TREE_NODE *) &key);
            //        if(buffer_node == NULL){
            //            ssdsim_msg("wrong in move pages from compression to uncompression");
            //            while(1){
            //
            //            }
            //        }
            //
            //        avlTreeDel(ssd->dram->mt->buffer_DFTL, (TREE_NODE *) buffer_node);
            //        if (ssd->dram->mt->buffer_DFTL->buffer_head->LRU_link_next == NULL) {
            //            ssd->dram->mt->buffer_DFTL->buffer_head = NULL;
            //            ssd->dram->mt->buffer_DFTL->buffer_tail = NULL;
            //        } else if(buffer_node == ssd->dram->mt->buffer_DFTL->buffer_tail) {
            //
            //            ssd->dram->mt->buffer_DFTL->buffer_tail = ssd->dram->mt->buffer_DFTL->buffer_tail->LRU_link_pre;
            //            ssd->dram->mt->buffer_DFTL->buffer_tail->LRU_link_next = NULL;
            //        }else{
            //
            //            buffer_node->LRU_link_next->LRU_link_pre =  buffer_node->LRU_link_pre;
            //            buffer_node->LRU_link_pre->LRU_link_next =  buffer_node->LRU_link_next;
            //
            //        }
            //        buffer_node->LRU_link_next = NULL;
            //        buffer_node->LRU_link_pre = NULL;
            //        AVL_TREENODE_FREE(ssd->dram->mt->buffer_DFTL, (TREE_NODE *)buffer_node);
            //        //step2.1 change size.
            //
            //        ssd->dram->mt->buffer_DFTL->buffer_sector_count -= 1 ;// once only one total translation page here.
            //        ssd->dram->mt->now_cached_dir_slots_sizein_byte -=ssd->dram->mt->compression_size[temp_binode->lpn_and_state[0]];
            //
            //

            //temp_binode = temp_binode_pre;
            temp_binode = temp_binode->prevNode;
        }

    }//end of for loop
//step2 checking the uncompression size whether overflow.
if((ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte % MAPPING_ENTRY_SIZE) != 0){
    ssdsim_msg("wrong the size of uncompression part");
    while(1){

    }
}


if(ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte > ssd->dram->mt->uncompression_max_cached_dir_slots_sizein_byte){

    ssdsim_msg("wrong the size of uncompression part, size overflow");
    while(1){

    }
}

    return i;
}//end of new_move_pages_from_compression_to_uncompression








//lxc for compression

int move_pages_from_compression_to_uncompression(struct ssd_info* ssd, struct request* request)
{

    int i;
    struct buffer_group key,  *buffer_node = NULL, *new_node ;
    struct _bi_node* temp_binode = NULL, *temp_binode_pre = NULL;
    struct _bi_list* temp_bilist = NULL;
    temp_bilist = request->compression_to_uncompression;
    temp_binode = temp_bilist->node_tail;
//pay attention it, this buffernode should be buffer_DFTL's head node, but covered by the coming uncompression part!!!!

    for(i = 0; i < temp_bilist->len; i ++){
        key.group = temp_binode->lpn_and_state[0];
        buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL,
                (TREE_NODE *) &key);
        if(buffer_node == NULL){
            ssdsim_msg("wrong in move pages from compression to uncompression");
            while(1){

            }
        }




        //step1,add nodes to  uncompression part 
        /******************************************************************************
         *create a new buffer node，and add to list head and avlTree
         *******************************************************************************/

        new_node = NULL;
        new_node = (struct buffer_group *) malloc(sizeof(struct buffer_group));
        alloc_assert(new_node, "buffer_group_node");
        memset(new_node, 0, sizeof(struct buffer_group));

        new_node->group = temp_binode->lpn_and_state[0];
        new_node->stored = temp_binode->lpn_and_state[1];
        new_node->dirty_clean = temp_binode->lpn_and_state[1];
        new_node->LRU_link_pre = NULL;
        new_node->LRU_link_next = ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head;
        if (ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head != NULL) {
            ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head->LRU_link_pre = new_node;
        } else {
            ssd->dram->mt->buffer_DFTL_UNCOM->buffer_tail = new_node;
        }
        ssd->dram->mt->buffer_DFTL_UNCOM->buffer_head = new_node;
        new_node->LRU_link_pre = NULL;
        //above operations are all for lru queue, next function is for Tree.
        avlTreeAdd(ssd->dram->mt->buffer_DFTL_UNCOM, (TREE_NODE *) new_node);

        //step1.1 size change . the size is not only in DFTL_buffer but in GTD structure.

        ssd->dram->mt->buffer_DFTL_UNCOM->buffer_sector_count += 1 ;// once only one total translation page here.
        ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte += ssd->parameter->page_capacity;

        //step2, delete the appointed nodes instead of tail nodes!!!!!!!!! in compression part.
        temp_binode_pre = temp_binode->prevNode ;

        key.group = temp_binode->lpn_and_state[0];
        buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->mt->buffer_DFTL,
                (TREE_NODE *) &key);
        if(buffer_node == NULL){
            ssdsim_msg("wrong in move pages from compression to uncompression");
            while(1){

            }
        }

        avlTreeDel(ssd->dram->mt->buffer_DFTL, (TREE_NODE *) buffer_node);
        if (ssd->dram->mt->buffer_DFTL->buffer_head->LRU_link_next == NULL) {
            ssd->dram->mt->buffer_DFTL->buffer_head = NULL;
            ssd->dram->mt->buffer_DFTL->buffer_tail = NULL;
        } else if(buffer_node == ssd->dram->mt->buffer_DFTL->buffer_tail) {
            
            ssd->dram->mt->buffer_DFTL->buffer_tail = ssd->dram->mt->buffer_DFTL->buffer_tail->LRU_link_pre;
            ssd->dram->mt->buffer_DFTL->buffer_tail->LRU_link_next = NULL;
        }else{

        buffer_node->LRU_link_next->LRU_link_pre =  buffer_node->LRU_link_pre;
        buffer_node->LRU_link_pre->LRU_link_next =  buffer_node->LRU_link_next;

        }
        buffer_node->LRU_link_next = NULL;
        buffer_node->LRU_link_pre = NULL;
        AVL_TREENODE_FREE(ssd->dram->mt->buffer_DFTL, (TREE_NODE *)buffer_node);
        //step2.1 change size.

        ssd->dram->mt->buffer_DFTL->buffer_sector_count -= 1 ;// once only one total translation page here.
        ssd->dram->mt->now_cached_dir_slots_sizein_byte -=ssd->dram->mt->compression_size[temp_binode->lpn_and_state[0]];




        temp_binode = temp_binode_pre;


    }//end of for loop
    return i;
}//end of move_pages_from_compression_to_uncompression


//lxc for compression
//only write requests can incures compression size update!!and the write requests!!!these two parts
//but movement can incur possible read overhead than SFTL.
//this part is only for the compression to uncompression's entry size
void new_move_between_compression_and_uncompression(struct ssd_info* ssd, struct request* request)
{
    //step1, uncompression---->compression. since now from the evicting operation, only compression area contains void space.
//    if(request->the_number_of_evicting_from_uncom_to_com != 0){
//        if(move_pages_from_uncompression_to_compression(ssd, request) != request->the_number_of_evicting_from_uncom_to_com){
//            ssdsim_msg("wrong in move_between_compression_and_uncompression");
//            while(1){
//
//            }
//        }
//    }
    //update_compression_size();//that move_pages_from_uncompression_to_compression already finish that two buffers size.


    //step2, compression-->uncompression
    if(request->missing_in_data_buffer_lpn_data->len != 0){
        if(new_move_pages_from_compression_to_uncompression(ssd, request) != request->missing_in_data_buffer_lpn_data->len){
            ssdsim_msg("wrong in new_move_between_compression_and_uncompression");
            while(1){

            }
        }
    }

}//end of new_move_between_compression_and_uncompression







//lxc for compression
//only write requests can incures compression size update!!and the write requests!!!these two parts
//but movement can incur possible read overhead than SFTL.
void move_between_compression_and_uncompression(struct ssd_info* ssd, struct request* request)
{
    //step1, uncompression---->compression. since now from the evicting operation, only compression area contains void space.
    if(request->the_number_of_evicting_from_uncom_to_com != 0){
        if(move_pages_from_uncompression_to_compression(ssd, request) != request->the_number_of_evicting_from_uncom_to_com){
            ssdsim_msg("wrong in move_between_compression_and_uncompression");
            while(1){

            }
        }
    }
    //update_compression_size();//that move_pages_from_uncompression_to_compression already finish that two buffers size.


    //step2, compression-->uncompression
    if(request->compression_to_uncompression->len != 0){
        if(move_pages_from_compression_to_uncompression(ssd, request) != request->compression_to_uncompression->len){
            ssdsim_msg("wrong in move_between_compression_and_uncompression");
            while(1){

            }
        }
    }

}//end of move_between_compression_and_uncompression

//lxc for compression

//lxc for compression

struct ssd_info* COMPRESSION_DFTL_deal_in_distributing_subrequests(struct ssd_info* ssd)
{
	//compression DFTL means:
	//after getting evicting pages, then,we should consider compression cost time. if there is evict pages, then during the evicting process, there is no compression time costing...and during the fetching process, we can also ignore all the compression costing time 
	struct request *req;
	struct sub_request *sub;
	int  evict_number, fetch_number, temp_number_now; 
	int compression_cost_sign_in_subpage;// 1 means need to take the compression time into consider.
	//int finished_movement_between_com_and_uncom_flag = 0;//1 means finished, since we can make sure whether evicting or fetching can happen.
	req = ssd->request_tail;
	if(req == NULL){

		ssdsim_msg("now no any request in ssd");
		if(ssd->request_queue_length != 0){
			ssdsim_msg("wrong in dealing the ssd's request here. in ssd");

			while(1){
			}
		}
		if(ssd->missing_mapping_and_cannot_get_traceline != 0){
			ssdsim_msg("no any request, but cannot get new request here");
			while(1){
			}
		}
		return ssd;
	}


 if(req->missing_in_data_buffer_lpn_data->len == 0){
        ssdsim_msg("req lsn %d, req address %lld, there is no missing in databuffer, so dont need any distributing subrequests here.", req->lsn, req);

        if( (req->total_wait_fetch_mappingtable == 1) || (req->total_wait_evict == 1) ){

            ssdsim_msg("serious wrong in logically");
            while(1){

            }
        }
        return ssd;
    }

 //this part is very important. after distributing the normal subrequests, then we should jump out this function
    //since the above judgement has decided if there is no missing_in_databuffer, then we just jump out here, after the above judgement, we can eliminate the incoming new request with no-missing-in-databuffer, after the elimination, if there is another requst, which has created normal subrequests sign, we should jump out for next process() function, but before it, if it has response_time simutaniously. that means we deleted the finished requests in trace_output() function wrongly....

    if(req->create_or_notyet_normal_subrequests == 1){
        if(ssd->request_queue != NULL){
            struct request* temp = ssd->request_queue;
            while(temp != NULL){
                ssdsim_msg("go out here, since finish the creating of normal subrequest, req lsn %d, req %lld, req->operation %d , whether can get new requests %d, req's responsetime %lld", temp->lsn, temp, temp->operation, ssd-> missing_mapping_and_cannot_get_traceline, temp->response_time);
                if(temp->response_time != 0){
                    ssdsim_msg("stop here, logical error happening!,after traceoutput, there is no finished request existing here");
                    while(1){

                    }
                }
                temp = temp->next_node;
                //ssdsim_msg("go out here, since finish the creating of normal subrequest, req lsn %d, req %lld, req->operation %d , whether can get new requests %d", req->lsn, req, req->operation, ssd-> missing_mapping_and_cannot_get_traceline);

            }

        }
        return ssd;
    }




//above all, are checking, next is function

if( (req->missing_in_data_buffer_lpn_data != NULL) && ( req->missing_in_DFTL_buffer_lpn_data != NULL) && ( req->should_evict_lpn_data !=NULL) ){
	ssdsim_msg("before distributing subrequests, everyvalue: requestaddress %lld, has total_wait_fetch_mappingtable is %d, total_wait_evict is %d, create_or_notyet_fetch is %d, create_or_notyet_evict is %d, missing_data_buffer_length %d, missing_DFTL_buffer_length %d, should_evict_lpn number %d", req, req->total_wait_fetch_mappingtable,req->create_or_notyet_fetch_subrequests, req->create_or_notyet_evict_subrequests, req->missing_in_data_buffer_lpn_data->len, req->missing_in_DFTL_buffer_lpn_data->len, req->should_evict_lpn_data->len);

}
	//this illustration is so important, so important!!step 0,!!!!!!!!!!!!!only can be done after evicting process, then we can make sure the compresson area has void space!!!!!!!!!!!!!!!!!!! cost time sign for sub_request.
	//if(){//there are evicting pages or fetching pages 
	//if(){//if there is movement between compression and uncompression
	//move_between_compression_and_uncompression();
	//}
	//
	//}else{//there are not evicting pages or fetching pages
	//if(){//if there is movement between compression and uncompression
	//move_between_compression_and_uncompression();
	//add_compression_cost_time_in_process();
	//}
	//
	//}


	//if_need_movement_between_com_and_uncom_flag_and_page_numbers = whether_need_movement();

	///*************step1, before this founction, request has been valued in 4 parameters, total_wait_evict, total_wait_fetch_mappingtable, create_or_notyet_evict_subrequests, create_or_notyet_fetch_subrequests. so this step is for creat_evict_subrequests or creat_fetch_subrequests and then waiting for getting missing mappingtable*****************************///



	if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 1))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 0) ){

		create_evict_mappingtable_subrequests(ssd);
		req->create_or_notyet_evict_subrequests = 1;


	}//end of step1

	///****************step2, polling and checking whether evict mapping table finished under this if condition.*****************************///

	if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 1))&&((req->create_or_notyet_evict_subrequests == 1))&&(req->create_or_notyet_fetch_subrequests == 0) ){

		if(polling_whether_finish_evict_subrequests(ssd)){


			ssdsim_msg("compression here has finished the evict write subrequest, current_time is %d\n", ssd->current_time);
			evict_number = Pop_DFTL_buffer(ssd, req);

			temp_number_now = delete_evict_subrequest_from_this_req(ssd);//if evict subrequests finished, then delete it before next two value sentences.

		if(req->subs != NULL){
				traceError_msg("compression deleting in evict subs wrong");
				while(1){

				}
			}


			req->create_or_notyet_evict_subrequests = 0;
			req->total_wait_evict = 0;
			// DFTLmanagement1 of DFTL buffer . DFTL buffer management contains 2 steps, content and size


		//	if(evict_number != temp_number_now){
		//		ssdsim_msg("evict number in DFTL buffer is wrong with delete");
		//		while(1){
		//		}
		//	}
		}
		//now,here,  it means, there is chance to complete movement between compression and uncompression part.

		if((req->finished_movement_between_com_and_uncom_flag  == 0) && (req->if_need_movement_between_com_and_uncom_flag_and_page_numbers !=0)){
			move_between_compression_and_uncompression(ssd, req);
			req->finished_movement_between_com_and_uncom_flag  = 1;
		}
	}//end of step2

	///***************step3, after evict,or not any evict actions at all, then create fetching mapping table subrequests***************************/// 

	if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 0) ){

		create_fetch_mappingtable_subrequests(ssd);
		req->create_or_notyet_fetch_subrequests = 1;


	}//end of step3


	///****************step4, polling and checking whether fetching mapping table finished under this if condition.*****************************///

	if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 1) ){

		if(polling_whether_finish_fetch_subrequests(ssd)){

			// first insert and then delete subrequest and bi_list. this is the same in step2: before deleting ,we should deal DFTL_buffer firstly according the bi_list content.

			//fetch_number = insert2DFTL_buffer(ssd, req);
			//fetch_number = insert2DFTL_UNCOM_buffer(ssd, req);
            //new compression
			//fetch_number = new_insert2DFTL_buffer(ssd, req);
			fetch_number = insert2DFTL_buffer(ssd, req);

			temp_number_now = delete_fetch_subrequest_from_this_req(ssd);//if fetch subrequests finished, then delete it before next two value sentences.
			if(req->subs != NULL){
				traceError_msg("compression deleting in fetch_subrequest wrong");
				while(1){

				}
			}


			req->create_or_notyet_fetch_subrequests = 0;
			req->total_wait_fetch_mappingtable = 0;
			// DFTLmanagement2 of DFTL buffer . DFTL buffer management contains 2 steps, content and size

//			if(fetch_number != temp_number_now){
//				ssdsim_msg("fetch number in DFTL buffer is wrong with delete");
//				while(1){
//				}
//			}
		}
		if((req->finished_movement_between_com_and_uncom_flag  == 0) && (req->if_need_movement_between_com_and_uncom_flag_and_page_numbers !=0)){
			//move_between_compression_and_uncompression(ssd, req);
			//new compression
            new_move_between_compression_and_uncompression(ssd, req);
			req->finished_movement_between_com_and_uncom_flag  = 1;


		}
	}//end of step4


	///****************step5, not any missing mappingtable or already got the mapping table, then do normal creat_sub_request.*****************************///

	if( (req->total_wait_fetch_mappingtable == 0)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 0)&& req->create_or_notyet_normal_subrequests == 0 ){

		if((req->finished_movement_between_com_and_uncom_flag  == 0) && (req->if_need_movement_between_com_and_uncom_flag_and_page_numbers !=0)){
            //new compression
			new_move_between_compression_and_uncompression(ssd, req);
			req->finished_movement_between_com_and_uncom_flag  = 1;

			//biggest question ,how to add adding time, now we have not added it!!!!!

        //so we have to cancel this sentence in creat_sub_request function.  sub->begin_time = ssd->current_time + sub->compression_cost_add_flag;

		}


		if(req->missing_in_data_buffer_lpn_data->len == 0){
		return ssd;
	}

		if( (create_normal_subrequests(ssd) == 1) && (req->subs != NULL  )){//after this function, this request must must must must be removed from ssd->request, incase of repeat deal!!!!!!!so important!!!!
			req->create_or_notyet_normal_subrequests = 1; 
		}



//
//		if(req->finished_movement_between_com_and_uncom_flag  == 1){
//			req->if_need_movement_between_com_and_uncom_flag_and_page_numbers = 0;
//				create_normal_subrequests(ssd);//after this function, this request must must must must be removed from ssd->request, incase of repeat deal!!!!!!!so important!!!!
//		}else{
//			//here else, means not finished movement, that means no no fetching or evicting time to uncompression, so if req->if_need_movement_between_com_and_uncom_flag_and_page_numbers has values, then create_normal_subrequests function  can deal with it in create_sub_request(). changing every subrequests's begin_time, and!!!!!and we set subrequest's write and read begin under sub->begin_time <= ssd->current_time. is enough!!
//			//but if ssd->current_time cannot move award,so how to move current_time award? no way, so how to? have to just add compression time in last request time.no any schedule relation for subrequests.
//        //so we have to cancel this sentence in creat_sub_request function.  sub->begin_time = ssd->current_time + sub->compression_cost_add_flag;
//			create_normal_subrequests(ssd);//after this function, this request must must must must be removed from ssd->request, incase of repeat deal!!!!!!!so important!!!!
//		}
//


if(ssd->missing_mapping_and_cannot_get_traceline == 1){
            ssd->missing_mapping_and_cannot_get_traceline = 0;
            traceError_msg("compression after dealing with mappingtables of request address: %lld, we allow to get new request by value missing_mapping_and_cannot_xx to 0 again", req);
        }

	}//end of step5


}//end of COMPRESSION_DFTL_deal_in_distributing_subrequests


//lxc for compression this function is for all requests's sub_requests assignment. functions before this funciton has been changed to get only request parameter value.

struct ssd_info* DFTL_deal_in_distributing_subrequests(struct ssd_info* ssd)
{
    struct request *req;
    struct sub_request *sub;
    int  evict_number, fetch_number, temp_number_now; 
    req = ssd->request_tail;
	if(req == NULL){
		ssdsim_msg("now no any request in ssd");
		if(ssd->missing_mapping_and_cannot_get_traceline != 0){
			ssdsim_msg("no any request, but cannot get new request here");
			while(1){
			}
		}
		return ssd;
	}

    if(req->missing_in_data_buffer_lpn_data->len == 0){
        ssdsim_msg("req lsn %d, req address %lld, there is no missing in databuffer, so dont need any distributing subrequests here.", req->lsn, req);

        if( (req->total_wait_fetch_mappingtable == 1) || (req->total_wait_evict == 1) ){

            ssdsim_msg("serious wrong in logically");
            while(1){

            }
        }
        return ssd;
    }
    //this part is very important. after distributing the normal subrequests, then we should jump out this function
    //since the above judgement has decided if there is no missing_in_databuffer, then we just jump out here, after the above judgement, we can eliminate the incoming new request with no-missing-in-databuffer, after the elimination, if there is another requst, which has created normal subrequests sign, we should jump out for next process() function, but before it, if it has response_time simutaniously. that means we deleted the finished requests in trace_output() function wrongly....

    if(req->create_or_notyet_normal_subrequests == 1){
        if(ssd->request_queue != NULL){
            struct request* temp = ssd->request_queue;
            while(temp != NULL){
                ssdsim_msg("go out here, since finish the creating of normal subrequest, req lsn %d, req %lld, req->operation %d , whether can get new requests %d, req's responsetime %lld", temp->lsn, temp, temp->operation, ssd-> missing_mapping_and_cannot_get_traceline, temp->response_time);
                if(temp->response_time != 0){
                    ssdsim_msg("stop here, logical error happening!,after traceoutput, there is no finished request existing here");
                    while(1){

                    }
                }
                temp = temp->next_node;
                //ssdsim_msg("go out here, since finish the creating of normal subrequest, req lsn %d, req %lld, req->operation %d , whether can get new requests %d", req->lsn, req, req->operation, ssd-> missing_mapping_and_cannot_get_traceline);

            }

        }
        return ssd;
    }



    if( (req->missing_in_data_buffer_lpn_data != NULL) && ( req->missing_in_DFTL_buffer_lpn_data != NULL) && ( req->should_evict_lpn_data !=NULL) ){

        ssdsim_msg(" %d request ,before distributing subrequests, everyvalue: requestaddress %lld, has total_wait_fetch_mappingtable is %d, total_wait_evict is %d, create_or_notyet_fetch is %d, create_or_notyet_evict is %d, missing_data_buffer_length %d, missing_DFTL_buffer_length %d, should_evict_lpn number %d", req->operation,req,  req->total_wait_fetch_mappingtable, req->total_wait_evict, req->create_or_notyet_fetch_subrequests, req->create_or_notyet_evict_subrequests, req->missing_in_data_buffer_lpn_data->len, req->missing_in_DFTL_buffer_lpn_data->len, req->should_evict_lpn_data->len);
    }
    ///*************step1, before this founction, request has been valued in 4 parameters, total_wait_evict, total_wait_fetch_mappingtable, create_or_notyet_evict_subrequests, create_or_notyet_fetch_subrequests. so this step is for creat_evict_subrequests or creat_fetch_subrequests and then waiting for getting missing mappingtable*****************************///



    if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 1))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 0) ){

        create_evict_mappingtable_subrequests(ssd);
#ifdef compression_relevant
        //very improtant, please be attention here!!, this founction must be before the sign's change
        printf_mappingtable_relevant_subrequest_in_different_process(ssd, req);
#endif
        req->create_or_notyet_evict_subrequests = 1;

    }//end of step1

    ///****************step2, polling and checking whether evict mapping table finished under this if condition.*****************************///

    if( (req->total_wait_fetch_mappingtable == 1)&&(req->total_wait_evict == 1)&&(req->create_or_notyet_evict_subrequests == 1)&&(req->create_or_notyet_fetch_subrequests == 0) ){

        if(polling_whether_finish_evict_subrequests(ssd)){

            ssdsim_msg("here has finished the evict write subrequest, current_time is %d\n", ssd->current_time);
            evict_number = Pop_DFTL_buffer(ssd, req);

            temp_number_now = delete_evict_subrequest_from_this_req(ssd);//if evict subrequests finished, then delete it before next two value sentences.
		if(req->subs != NULL){
				traceError_msg("deleting in evict subs wrong");
				while(1){

				}
			}

#ifdef compression_relevant
            //very improtant, please be attention here!!, this founction must be before the sign's change, but after the deletion function
            printf_mappingtable_relevant_subrequest_in_different_process(ssd, req);
#endif

            req->create_or_notyet_evict_subrequests = 0;
            req->total_wait_evict = 0;
            // DFTLmanagement1 of DFTL buffer . DFTL buffer management contains 2 steps, content and size


//            if(evict_number != temp_number_now){
//                ssdsim_msg("evict number in DFTL buffer is wrong with delete");
//                while(1){
//                }
//            }
        }

    }//end of step2

    ///***************step3, after evict,or not any evict actions at all, then create fetching mapping table subrequests***************************/// 

    if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 0) ){

        create_fetch_mappingtable_subrequests(ssd);
#ifdef compression_relevant
        //very improtant, please be attention here!!, this founction must be before the sign's change
        printf_mappingtable_relevant_subrequest_in_different_process(ssd, req);
#endif

        req->create_or_notyet_fetch_subrequests = 1;


    }//end of step3


    ///****************step4, polling and checking whether fetching mapping table finished under this if condition.*****************************///

    if( (req->total_wait_fetch_mappingtable == 1)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 1) ){

        if(polling_whether_finish_fetch_subrequests(ssd)){


            // first insert and then delete subrequest and bi_list. this is the same in step2: before deleting ,we should deal DFTL_buffer firstly according the bi_list content.

            fetch_number = insert2DFTL_buffer(ssd, req);
            temp_number_now = delete_fetch_subrequest_from_this_req(ssd);//if fetch subrequests finished, then delete it before next two value sentences.
			if(req->subs != NULL){
				traceError_msg("deleting in fetch_subrequest wrong");
				while(1){

				}
			}
#ifdef compression_relevant
            //very improtant, please be attention here!!, this founction must be before the sign's change, but after the deletion function
            printf_mappingtable_relevant_subrequest_in_different_process(ssd, req);
#endif



            req->create_or_notyet_fetch_subrequests = 0;
            req->total_wait_fetch_mappingtable = 0;
            // DFTLmanagement2 of DFTL buffer . DFTL buffer management contains 2 steps, content and size

//            if(fetch_number != temp_number_now){
//                ssdsim_msg("fetch number in DFTL buffer is wrong with delete");
//                while(1){
//                }
//            }
        }
    }//end of step4


    ///****************step5, not any missing mappingtable or already got the mapping table, then do normal creat_sub_request.*****************************///

    if( (req->total_wait_fetch_mappingtable == 0)&&((req->total_wait_evict == 0))&&((req->create_or_notyet_evict_subrequests == 0))&&(req->create_or_notyet_fetch_subrequests == 0) && req->create_or_notyet_normal_subrequests == 0 ){

    if(req->missing_in_data_buffer_lpn_data->len == 0){
		return ssd;
	}
		
		//here create_normal_subreques return 1. refers that there are normal subrequests here.
        if( (create_normal_subrequests(ssd) == 1) && (req->subs != NULL  )){//after this function, this request must must must must be removed from ssd->request, incase of repeat deal!!!!!!!so important!!!!
			req->create_or_notyet_normal_subrequests = 1; 
#ifdef compression_relevant
            //very improtant, please be attention here!!, this founction must be before the sign's change
            printf_mappingtable_relevant_subrequest_in_different_process(ssd, req);
#endif
        }
        if(ssd->missing_mapping_and_cannot_get_traceline == 1){
            ssd->missing_mapping_and_cannot_get_traceline = 0;
            traceError_msg("after dealing with mappingtables of request address: %lld, we allow to get new request by value missing_mapping_and_cannot_xx to 0 again", req);
        }
    }//end of step5






}//end of DFTL_deal_in_subrequests
/////////////add for status  debug lxc ////////////

//lxc for  compression

void get_compression_translation_page_size(struct ssd_info* ssd)
{
    int i,j, page_num, translation_page_num, before_compression_size;
    unsigned int one_page_compression_size;
    unsigned int* translation_page_data_before_compression, *translation_page_data_after_compression;
    page_num = ssd->parameter->page_block * ssd->parameter->block_plane * ssd->parameter->plane_die * ssd->parameter->die_chip * ssd->parameter->chip_num;
    before_compression_size = ssd->parameter->page_capacity; 
    ssdsim_msg("one page size is %d",  ssd->parameter->page_capacity);
    translation_page_num = page_num / NUM_ENTRIES_PER_DIR; 
    translation_page_data_before_compression = (unsigned int*) malloc (sizeof(unsigned int)* NUM_ENTRIES_PER_DIR);
    translation_page_data_after_compression = (unsigned int*) malloc (sizeof(unsigned int)* NUM_ENTRIES_PER_DIR);
    memset(translation_page_data_before_compression , 0, sizeof(unsigned int)* NUM_ENTRIES_PER_DIR );
    memset(translation_page_data_after_compression , 0, sizeof(unsigned int)* NUM_ENTRIES_PER_DIR );

    for(i = 0; i < translation_page_num; i ++){
        for(j = 0;j < NUM_ENTRIES_PER_DIR; j ++){
            //*(translation_page_data_before_compression + (NUM_ENTRIES_PER_DIR * i + j)) = ssd->dram->map->map_entry[(NUM_ENTRIES_PER_DIR * i + j)].pn;
            translation_page_data_before_compression[j] = ssd->dram->map->map_entry[(NUM_ENTRIES_PER_DIR * i + j)].pn;
        }
        //this is for compression and value GTD's page size.
        one_page_compression_size = xmatchCompress((unsigned char*)translation_page_data_before_compression, before_compression_size, (unsigned char*)translation_page_data_after_compression);
        if(one_page_compression_size != 16){
        ssdsim_msg("translation page %d compression size is %d", i, one_page_compression_size);
        }
 //       ssd->dram->mt->compression_size[i] = one_page_compression_size ;
  //      ssd->dram->mt->compression_state[i] = 1;//1 means compressed.

    }
    if(page_num % NUM_ENTRIES_PER_DIR != 0){
        for(j = (page_num - (page_num % NUM_ENTRIES_PER_DIR)); j < page_num; j ++){
            *(translation_page_data_before_compression + j) = ssd->dram->map->map_entry[j].pn;
        }


        one_page_compression_size = xmatchCompress((unsigned char*)translation_page_data_before_compression, before_compression_size, (unsigned char*)translation_page_data_after_compression);

        ssd->dram->mt->compression_size[translation_page_num] = one_page_compression_size ;
        ssd->dram->mt->compression_state[translation_page_num]  = 1;//1 means compressed.

    }

    free(translation_page_data_before_compression);
    free(translation_page_data_after_compression);
    ssdsim_msg("here finish the compression");

}//end of get_compression_translation_page_size



/********************************************************************************************************************************
1，main函数中initiatio()函数用来初始化ssd,；2，make_aged()函数使SSD成为aged，aged的ssd相当于使用过一段时间的ssd，里面有失效页，
non_aged的ssd是新的ssd，无失效页，失效页的比例可以在初始化参数中设置；3，pre_process_page()函数提前扫一遍读请求，把读请求
的lpn<--->ppn映射关系事先建立好，写请求的lpn<--->ppn映射关系在写的时候再建立，预处理trace防止读请求是读不到数据；4，simulate()是
核心处理函数，trace文件从读进来到处理完成都由这个函数来完成；5，statistic_output()函数将ssd结构中的信息输出到输出文件，输出的是
统计数据和平均数据，输出文件较小，trace_output文件则很大很详细；6，free_all_node()函数释放整个main函数中申请的节点
*********************************************************************************************************************************/
int main() {
    unsigned int i, j, k;
    struct ssd_info *ssd;




    FILE* fp_number_subrequests_txt = fopen("number_subrequests.txt", "w");

#ifdef DEBUG
    printf("enter main\n");
    printf("for test\n");
#endif
    ssd = (struct ssd_info *) malloc(sizeof(struct ssd_info));
    alloc_assert(ssd, "ssd");
    memset(ssd, 0, sizeof(struct ssd_info));
    ssd = initiation(ssd);
#ifdef DEBUG
    printf("this is in main()");
    printfsub_request_status(ssd); 
#endif
    make_aged(ssd);
    pre_process_page(ssd);
    //lxc for compression
#ifdef  COMPRESSION_MAPPING 
    get_compression_translation_page_size(ssd);
#endif

    for (i = 0; i < ssd->parameter->channel_number; i++) {
        for (j = 0; j < ssd->parameter->die_chip; j++) {
            for (k = 0; k < ssd->parameter->plane_die; k++) {
                printf("%d,0,%d,%d:  %5d\n", i, j, k,
                       ssd->channel_head[i].chip_head[0].die_head[j].plane_head[k].free_page);
            }
        }
    }
    fprintf(ssd->outputfile, "\t\t\t\t\t\t\t\t\tOUTPUT\n");
    fprintf(ssd->outputfile, "****************** TRACE INFO ******************\n");

    ssd = simulate(ssd);

    
    statistic_output(ssd);
/*	free_all_node(ssd);*/

    printf("\n");
    printf("the simulation is completed!\n");


    return 1;
/* 	_CrtDumpMemoryLeaks(); */
}


/******************simulate() *********************************************************************
*simulate()是核心处理函数，主要实现的功能包括
*1,从trace文件中获取一条请求，挂到ssd->request
*2，根据ssd是否有dram分别处理读出来的请求，把这些请求处理成为读写子请求，挂到ssd->channel或者ssd上
*3，按照事件的先后来处理这些读写子请求。
*4，输出每条请求的子请求都处理完后的相关信息到outputfile文件中
**************************************************************************************************/
struct ssd_info *simulate(struct ssd_info *ssd) {
    int flag = 1, flag1 = 0;
    double output_step = 0;
    unsigned int a = 0, b = 0;
    //errno_t err;

    printf("\n");
    printf("begin simulating.......................\n");
    printf("\n");
    printf("\n");
    printf("   ^o^    OK, please wait a moment, and enjoy music and coffee   ^o^    \n");

    ssd->tracefile = fopen(ssd->tracefilename, "r");
    if (ssd->tracefile == NULL) {
        printf("the trace file can't open\n");
        return NULL;
    }

    fprintf(ssd->outputfile,
            "      arrive           lsn     size ope     begin time    response time    process time\n");
    fflush(ssd->outputfile);
    int i = 0;
    while (flag != 100) {


        //just has GC, then printf begin. 
        if(ssd->debug_sign == 1){
            //            printf("every get request can debug now\n");
            //#define DEBUG 
            //#define DEBUGQUEUE 
            //#define DEBUGSUSPEND
            //#define DEBUGTRACE_OUT 
            //#define DEBUG_CURR_TIME
        }


        //printf("%dth\n", i++);
        flag = get_requests(ssd);//lxcv1  get_requests means get trace io and then creat request then insert it into aad->request_queue。
        /*********************************todo**************************************/

        traceError_msg( "now flag is %d, 1 means getting new request \n" ,flag);
#ifdef DEBUG //lxcv1
        printf( "flag is %d\n" ,flag);
#endif
        if (flag == 1) {// for DFTL, here we just get read requsts's distrib_value and write requests in missing databuffer subrequests, and!!!we already get the basic missing mapping requests information for future distributing.

            traceError_msg("now we can directly distribute new request adress is %lld", ssd->request_tail);
            //printf("once\n");

            //printf_every_chip_static_subrequest(ssd);
            if (ssd->parameter->dram_capacity != 0) {

#ifdef PFTL
                buffer_management(ssd);
                distribute(ssd);
#endif
#ifdef DFTL
                DFTL_buffer_management(ssd);
                //DFTL_distribute(ssd);
#endif
            } else {
                no_buffer_distribute(ssd);
            }
        }else{//if is when to get new request.

            traceError_msg(":last request need get mappingtable, here cant get new request dealing last request is %lld", ssd->request_tail);
        }
        //lxc for compression
#ifdef DFTL

#ifdef COMPRESSION_MAPPING 
        COMPRESSION_DFTL_deal_in_distributing_subrequests(ssd);
#else
        DFTL_deal_in_distributing_subrequests(ssd);
#endif
#endif
        /*******************************todo****************************************/
        //lxcprogram_gc statistic here
        //printf_every_chip_static_subrequest(ssd);
        process(ssd);
        trace_output(ssd);
        test_finish_deletion(ssd);
        ssdsim_msg("get out from trace_output,  current time:%lld\n",ssd->current_time);

        //printf("flag = %d, ssd->request_queue = %d\n", flag, ssd->request_queue_length);
        if (flag == 0 )
            flag = 100;
    }

    fclose(ssd->tracefile);
    return ssd;
}


/********    get_request    ******************************************************
*	1.get requests that arrived already
*	2.add those request node to ssd->reuqest_queue
*	return	0: reach the end of the trace
*			-1: no request has been added
*			1: add one request to list
*SSD模拟器有三种驱动方式:时钟驱动(精确，太慢) 事件驱动(本程序采用) trace驱动()，
*两种方式推进事件：channel/chip状态改变、trace文件请求达到。
*channel/chip状态改变和trace文件请求到达是散布在时间轴上的点，每次从当前状态到达
*下一个状态都要到达最近的一个状态，每到达一个点执行一次process
********************************************************************************/
int get_requests(struct ssd_info *ssd) {
	char buffer[200];

	int64_t large_lsn, lsn = 0;
	int device, size, ope, i = 0, j = 0;
	struct request *request1;
	struct sub_request *temp_sub;
	int flag = 1;
	long filepoint;

	int64_t lsn_for_record = 0;
	int64_t time_t = 0;
	int64_t nearest_event_time;
#ifdef DEBUG_CURR_TIME 
	printf("DEBUG_CURR_TIME is %lld in get_request function\n",ssd->current_time);
#endif



	//lxc for compression . here so important.
	if(ssd->missing_mapping_and_cannot_get_traceline == 1){
		traceError_msg("1: in getting mapping table sub-requests, now waiting for the missing of translation mapping pages!");
		//            fseek(ssd->tracefile, filepoint, 0);
		//here, we should notice that, even we cannot get new request, we should update ssd->current_time now!!!

		nearest_event_time = find_nearest_event(ssd);
		if (nearest_event_time < MAX_INT64){

			ssd->current_time = nearest_event_time;
			if(ssd->current_time > nearest_event_time){
				ssdsim_msg("missing_mapping_and_cannot_get_traceline wrong in get requests now");
				while(1){
				}
			}
		}

		return -1;
	}//end of missing_mapping_and_cannot_get_traceline.


    if (feof(ssd->tracefile)){
        if(ssd->request_queue_length > 0){
            printf("here is dealing end of trace\n");
            nearest_event_time = find_nearest_event(ssd);

            if (nearest_event_time == MAX_INT64) {
                return -1;
            }
            if (nearest_event_time < MAX_INT64){

                ssd->current_time = nearest_event_time;
                return -1;
            }
        }

        return 0;
    }
    filepoint = ftell(ssd->tracefile);
    fgets(buffer, 200, ssd->tracefile);
    sscanf(buffer, "%lld %d %lld %d %d", &time_t, &device, &lsn, &size, &ope);

    if ((device < 0) && (lsn < 0) && (size < 0) && (ope < 0)) {
        return 100;
    }

    lsn_for_record = lsn;
#ifdef DEBUG //lxcv1
    printf("enter get_requests,  ssd->min_lsn %d, ssd->max_lsn %d\n",ssd->min_lsn,ssd->max_lsn);

    printf( "time_t %lld, device %d, lsn  %d,size %d ,ope %d\n" , time_t, device, lsn, size, ope);
#endif


    if (lsn < ssd->min_lsn)
        ssd->min_lsn = lsn;
    if (lsn > ssd->max_lsn)
        ssd->max_lsn = lsn;
    /******************************************************************************************************
    *上层文件系统发送给SSD的任何读写命令包括两个部分（LSN，size） LSN是逻辑扇区号，对于文件系统而言，它所看到的存
    *储空间是一个线性的连续空间。例如，读请求（260，6）表示的是需要读取从扇区号为260的逻辑扇区开始，总共6个扇区。
    *large_lsn: channel下面有多少个subpage，即多少个sector。overprovide系数：SSD中并不是所有的空间都可以给用户使用，
    *比如32G的SSD可能有10%的空间保留下来留作他用，所以乘以1-provide
    ***********************************************************************************************************/
    large_lsn = (long int) ((ssd->parameter->subpage_page * ssd->parameter->page_block * ssd->parameter->block_plane *
                        ssd->parameter->plane_die * ssd->parameter->die_chip * ssd->parameter->chip_num) *
                       (1 - ssd->parameter->overprovide));
    lsn = lsn % large_lsn;

    nearest_event_time = find_nearest_event(ssd);
#ifdef DEBUG //lxcv1
    printf("nearest_event_time is %lld\n",nearest_event_time );
#endif

    if (nearest_event_time == MAX_INT64) {


        if(ssd->request_queue_length >0){
            printf("only gc processing can happen\n");
        }
        if(ssd->request_queue_length == ssd->parameter->queue_length){
//  if(ssd->request_queue_length >0){
//printf("ssd first request time is %lld\n",  );
            printf("full of requests in queue now!!\n");
            fseek(ssd->tracefile, filepoint, 0);

//                for (i = 0; i < ssd->parameter->channel_number; i++) {
//
//        printf("channel[%d].next_state is 0x%x,ssd->channel_head[%d].next_state_predict_time is %lld, channel[%d].current_state is 0x%x,ssd->channel_head[%d].current_time is %lld\n",i,ssd->channel_head[i].next_state,i,ssd->channel_head[i].next_state_predict_time  ,i,ssd->channel_head[i].current_state,i,ssd->channel_head[i].current_time  );
//
//        for (j = 0; j < ssd->parameter->chip_channel[i]; j++) {
//
//            printf("ssd->channel_head[%d].chip_head[%d].next_state_predict_time is %lld,  ssd->channel_head[%d].chip_head[%d].next_state is 0x%x  ssd->channel_head[%d].chip_head[%d].current_time is %lld,  ssd->channel_head[%d].chip_head[%d].current_state is 0x%x \n", i,j,ssd->channel_head[i].chip_head[j].next_state_predict_time ,i,j,ssd->channel_head[i].chip_head[j].next_state , i,j,ssd->channel_head[i].chip_head[j].current_time ,i,j,ssd->channel_head[i].chip_head[j].current_state  );
//
//        }
//
//    }
//
//                printf_ssd_request_queue(ssd);
//                for (i = 0; i < ssd->parameter->channel_number; i++){
//                    temp_sub = ssd->channel_head[i].subs_w_head;
//                    while (temp_sub != NULL){
//                        printf("channel %d sub is %lld, sub current_state is %d\n", i, temp_sub, temp_sub->current_state);
//                        temp_sub = temp_sub->next_subs;
//
//                    }
//                    temp_sub = ssd->channel_head[i].subs_r_head;
//                    while (temp_sub != NULL){
//                        printf("channel %d sub is %lld\n", i, temp_sub);
//                        temp_sub = temp_sub->next_subs;
//                    }
//
//
//                }// end of channel loop
                return -1;
        }
// lxc for compression
if(ssd->missing_mapping_and_cannot_get_traceline == 1){
            traceError_msg("1: in getting mapping table sub-requests, now waiting for the missing of translation mapping pages!");
            fseek(ssd->tracefile, filepoint, 0);
                return -1;
        }
        ssd->current_time = time_t;
//        if(ssd->request_queue_length > 0){// this is wrong, for if gc happen, there will no write. so request will stay here.and maybe add another program.
//            printf("error is too biggest!!!\n");
//        }

    //printf("ssdcurrent_time  nter nearest_event_time is MAX_INIT64 and ssd->current_time is %lld\n", ssd->current_time );
#ifdef DEBUG //lxcv1
    //printf("ssdcurrent_time  nter nearest_event_time is MAX_INIT64 and ssd->current_time is %lld\n", ssd->current_time );
#endif

        //if (ssd->request_queue_length>ssd->parameter->queue_length)    //如果请求队列的长度超过了配置文件中所设置的长度
        //{
        //printf("error in get request , the queue length is too long\n");
        //}
    } else {
        if (nearest_event_time < time_t) {
            /*******************************************************************************
            *回滚，即如果没有把time_t赋给ssd->current_time，则trace文件已读的一条记录回滚
            *filepoint记录了执行fgets之前的文件指针位置，回滚到文件头+filepoint处
            *int fseek(FILE *stream, long offset, int fromwhere);函数设置文件指针stream的位置。
            *如果执行成功，stream将指向以fromwhere（偏移起始位置：文件头0，当前位置1，文件尾2）为基准，
            *偏移offset（指针偏移量）个字节的位置。如果执行失败(比如offset超过文件自身大小)，则不改变stream指向的位置。
            *文本文件只能采用文件头0的定位方式，本程序中打开文件方式是"r":以只读方式打开文本文件
            **********************************************************************************/
#ifdef DEBUG //lxcv1
    printf("enter nearest_event_time <time_t\n" );
#endif

            fseek(ssd->tracefile, filepoint, 0);
            if (ssd->current_time <= nearest_event_time){

//                printf("trace line is later, dont have to add. ssd->CURR_TIME is %lld, nearest_event_time is %lld in get_requests function\n",ssd->current_time, nearest_event_time);
                ssd->current_time = nearest_event_time;

#ifdef DEBUG_CURR_TIME 
                printf("ssdcurrenttime DEBUG_CURR_TIME is %lld in get_requests function\n",ssd->current_time);
#endif

            }
            return -1;
        } else {
#ifdef DEBUG //lxcv1
    printf("enter nearest_event_time > time_t\n" );
#endif

            if (ssd->request_queue_length >= ssd->parameter->queue_length) {
                //printf("here request long enough!!!, length is %d\n", ssd->request_queue_length);

                fseek(ssd->tracefile, filepoint, 0);
                ssd->current_time = nearest_event_time;
#ifdef DEBUG_CURR_TIME 
                printf("ssdcurrenttime  DEBUG_CURR_TIME is %lld in get_requests function and request_queue>=parameter queue_length\n",ssd->current_time);
#endif



//
//
//            ///*****************judging for times by queue stuffing with only write requests***********************///
//            // judging for times stuffing by full queue only with write request.
//            unsigned int temp_i;
//            struct request *request1;
//            struct gc_operation *temp_gc_node;
//            unsigned int temp_gc_sign = 0;
//            if(ssd->request_queue != NULL){
//                request1 = ssd->request_queue;
//            }
//            for(temp_i = 0; temp_i < ssd->request_queue_length; temp_i ++){
//
//                if(request1->operation != 1 ){
//                    request1 = request1->next_node;
//                }else{
//
//                    break;
//                }
//            }//end of for
//            if(temp_i == ssd->request_queue_length){
//                if(ssd->temp_lba != lsn){
//                    ssd->temp_lba = lsn;
//
//                    ssd->interference_stuffed_length ++;
//                    printf("deal inner subrequest, current_time %lld, temp_lba = %d, interference_stuffed_length is %d \n", ssd->current_time, ssd->temp_lba, ssd->interference_stuffed_length);
//
//                    for(temp_i = 0; temp_i < ssd->parameter->channel_number; temp_i ++ ){
//
//                        temp_gc_node = ssd->channel_head[i].gc_command;
//                        if(temp_gc_node != NULL){
//                            temp_gc_sign = 1;
//                            break;
//                        }
//                    }
//                    if(temp_gc_sign == 1){
//                        ssd->interference_stuffed_length_during_gc ++;
//                        temp_gc_sign = 0;
//                        printf("stuffed during gc is %d\n", ssd->interference_stuffed_length_during_gc);
//                    }
//                }
//            }
//
//
//            ///*****************judging for times by queue stuffing with only write requests***********************///
        ///*****************judging for times by queue stuffing with only write requests***********************///
            // judging for times stuffing by full queue only with write request.
//            unsigned int temp_i;
//            struct request *request1;
//            struct gc_operation *temp_gc_node;
//            unsigned int temp_gc_sign = 0;
//            if(ssd->request_queue != NULL){
//                request1 = ssd->request_queue;
//            }
//            for(temp_i = 0; temp_i < ssd->request_queue_length; temp_i ++){
//
//                if(request1->operation != 0 ){
//                    request1 = request1->next_node;
//                }else{
//
//                    break;
//                }
//            }//end of for
//            if(temp_i == ssd->request_queue_length){
//                if(ssd->temp_lba != lsn_for_record ){
//                    ssd->temp_lba = lsn_for_record ;
//
//                    ssd->interference_stuffed_length ++;
//                    printf("fullreaddeal inner subrequest, current_time %lld, temp_lba = %lld, interference_stuffed_length is %d \n", ssd->current_time, ssd->temp_lba, ssd->interference_stuffed_length);
//
//                    for(temp_i = 0; temp_i < ssd->parameter->channel_number; temp_i ++ ){
//
//                        temp_gc_node = ssd->channel_head[i].gc_command;
//                        if(temp_gc_node != NULL){
//                            temp_gc_sign = 1;
//                            break;
//                        }
//                    }
//                    if(temp_gc_sign == 1){
//                        ssd->interference_stuffed_length_during_gc ++;
//                        temp_gc_sign = 0;
//                        printf("fullreadrequeststuffed during gc is %d\n", ssd->interference_stuffed_length_during_gc);
//                    }
//                }
//            }
//
//
//            ///*****************judging for times by queue stuffing with only write requests***********************///
//
//
//
//
//            ///*****************judging for times by queue stuffing with only write requests***********************///
//            // judging for times stuffing by full queue only with write request.
//          //  unsigned int temp_i;
//          //  struct request *request1;
//          //  struct gc_operation *temp_gc_node;
//          //  unsigned int temp_gc_sign = 0;
//            if(ssd->request_queue != NULL){
//                request1 = ssd->request_queue;
//            }
//            for(temp_i = 0; temp_i < ssd->request_queue_length; temp_i ++){
//
//                if(request1->operation != 1 ){
//                    request1 = request1->next_node;
//                }else{
//
//                    break;
//                }
//            }//end of for
//            if(temp_i == ssd->request_queue_length){
//                if(ssd->temp_lba != lsn_for_record ){
//                    ssd->temp_lba = lsn_for_record ;
//
//                    ssd->interference_stuffed_length ++;
//                    printf("fullwritedeal inner subrequest, current_time %lld, temp_lba = %lld, interference_stuffed_length is %d \n", ssd->current_time, ssd->temp_lba, ssd->interference_stuffed_length);
//
//                    for(temp_i = 0; temp_i < ssd->parameter->channel_number; temp_i ++ ){
//
//                        temp_gc_node = ssd->channel_head[i].gc_command;
//                        if(temp_gc_node != NULL){
//                            temp_gc_sign = 1;
//                            break;
//                        }
//                    }
//                    if(temp_gc_sign == 1){
//                        ssd->interference_stuffed_length_during_gc ++;
//                        temp_gc_sign = 0;
//                        printf("fullwritestuffed during gc is %d\n", ssd->interference_stuffed_length_during_gc);
//                    }
//                }
//            }
//
//
//            ///*****************judging for times by queue stuffing with only write requests***********************///







                return -1;
            }
            
            // lxc for compression
            if(ssd->missing_mapping_and_cannot_get_traceline == 1){
                traceError_msg("2: in getting requests, now waiting for the missing of translation mapping pages!!");
                fseek(ssd->tracefile, filepoint, 0);
                return -1;
            }
            
    //        else {
                ssd->current_time = time_t;
#ifdef DEBUG_CURR_TIME 
                printf("ssdcurrenttime  DEBUG_CURR_TIME is %lld in get_requests function request_queue<=parameter queue_length \n",ssd->current_time);
#endif

      //      }
        }
    }

    if (time_t < 0) {
        printf("error!\n");
        while (1) {}
    }

    if (feof(ssd->tracefile)) {
        request1 = NULL;
        return 0;
    }

    request1 = (struct request *) malloc(sizeof(struct request));
    alloc_assert(request1, "request");
    memset(request1, 0, sizeof(struct request));

    request1->time = time_t;
    request1->lsn = lsn;
    request1->size = size;
    request1->operation = ope;
    request1->begin_time = time_t;
    request1->response_time = 0;
    request1->energy_consumption = 0;
    request1->next_node = NULL;
    request1->distri_flag = 0;              // indicate whether this request has been distributed already
    request1->subs = NULL;
    request1->need_distr_flag = NULL;
    request1->complete_lsn_count = 0;         //record the count of lsn served by buffer
    filepoint = ftell(ssd->tracefile);        // set the file point
    //lxc for compression
    request1->total_wait_evict = 0; // 1 means needing wait, 0 do not have to wait or finishing evicting.
    request1->total_wait_fetch_mappingtable = 0; // 1 means needing fetching, 0 dont have to fetch or finish fetching

    request1->entry_evict_page_number = 0;
    request1->entry_fetch_page_number = 0;

    request1->should_evict_lpn_data = create_and_init_bi_list();
    request1->missing_in_data_buffer_lpn_data = create_and_init_bi_list();
	if(request1->missing_in_data_buffer_lpn_data == NULL){
		ssdsim_msg("wrong in create missing_in_data_buffer");
		while(1){

		}
	}
    request1->missing_in_DFTL_buffer_lpn_data = create_and_init_bi_list();
    request1->create_or_notyet_evict_subrequests = 0;
    request1->create_or_notyet_fetch_subrequests = 0;
#ifdef COMPRESSION_MAPPING 
    request1->if_need_movement_between_com_and_uncom_flag_and_page_numbers = 0;
    request1->the_number_of_evicting_from_uncom_to_com = 0;
    request1->hit_in_uncompression_buffer_number = 0;
    request1->compression_to_uncompression = create_and_init_bi_list();
#else

    request1->if_need_movement_between_com_and_uncom_flag_and_page_numbers = 0;
    request1->the_number_of_evicting_from_uncom_to_com = 0;
    request1->hit_in_uncompression_buffer_number = 0;
    request1->compression_to_uncompression = NULL;


#endif

#ifdef DEBUG
    printf("before insert into ssd-request queue\n");
    printf_ssd_request_queue(ssd);
#endif


 //  printf("ssd request_queue , lsn : %d, size : %d, operation :%d   ***1read,0write,complete_lsn_count :%d, distri_flag :%d, begin_time :%llu\n",request1->lsn,request1->size,request1->operation,request1->complete_lsn_count,request1->distri_flag,request1->begin_time);
   traceError_msg("at ssdcurrent time %d, getting new request_queue address:%lld, lsn : %d, size : %d, operation :%d   ***1read,0write,complete_lsn_count :%d, distri_flag :%d, begin_time :%llu",ssd->current_time,request1, request1->lsn,request1->size,request1->operation,request1->complete_lsn_count,request1->distri_flag,request1->begin_time);

    if (ssd->request_queue == NULL)          //The queue is empty
	{
		if(ssd->request_queue_length != 0){
			ssdsim_msg("1 wrong in adding ssd's request ");
			while(1){
			}
		}
        if(ssd->request_tail != NULL){
			ssdsim_msg("2 wrong in adding ssd's request ");
			while(1){
			}
        }
		ssd->request_queue = request1;
		ssd->request_tail = request1;
		ssd->request_queue_length++;
	} else {
        (ssd->request_tail)->next_node = request1;
        ssd->request_tail = request1;
        ssd->request_queue_length++;
    }
//lxcprogram_for_adjust_write_suspension_number

//    no_greedy_RF_suspension(ssd);

//lxcprogram_for_adjust_write_suspension_number


#ifdef DEBUG
    printf("after inserted into ssd-request queue\n");
    printf_ssd_request_queue(ssd);
#endif
    if (request1->operation == 1)             //计算平均请求大小 1为读 0为写
    {
        ssd->ave_read_size =
                (ssd->ave_read_size * ssd->read_request_count + request1->size) / (ssd->read_request_count + 1);
    } else {
        ssd->ave_write_size =
                (ssd->ave_write_size * ssd->write_request_count + request1->size) / (ssd->write_request_count + 1);
    }


    filepoint = ftell(ssd->tracefile);
    fgets(buffer, 200, ssd->tracefile);    //寻找下一条请求的到达时间
    sscanf(buffer, "%lld %d %lld %d %d", &time_t, &device, &lsn, &size, &ope);
    ssd->next_request_time = time_t;
    fseek(ssd->tracefile, filepoint, 0);

    return 1;
}

int no_greedy_RF_suspension(struct ssd_info *ssd)
{

//lxcprogram_for_adjust_write_suspension_number

//unsigned int chip_write_record_begin;
//unsigned int chip_write_record_end;
//unsigned int chip_read_stop;
    unsigned int temp_channel,temp_chip, temp_i, temp_j, temp_num_write;
    struct sub_request *temp_sub_for_write = NULL;
    struct gc_operation* temp_gc_node;
    unsigned int chip_has_write = 0;
    unsigned int temp_length = 0;
    temp_channel = ssd->parameter->channel_number;
    temp_chip = ssd->parameter->chip_num / ssd->parameter->channel_number;
    temp_length =  WRITE_RATIO_IN_REQUEST_QUEUE * ssd->parameter->queue_length;


//    if(request->operation == 0){
        // statistic for write requests number
        temp_num_write = statistic_write_request_number(ssd);
        if(temp_num_write == ssd->request_queue_length){
            return 1;
        }
        printf("temp_num_write is %d, and setting write length is %d, and longest length is %d\n", temp_num_write, temp_length, ssd->parameter->queue_length);
        if( temp_num_write >= temp_length ){

            //1)only  the first serviced write corresponding  chips should be set as chip_read_stop for further schedule, and record every chip's write record begin.  // here the most important is that the one chip write subrequest serviced sequence is depend on the chip write sequence. but the original design set the service sequence as the die first. 
            //2) if there is gc, then also dont set the chip_read_stop sign value. this setting is only for normal write instead of GC process.

            // step1, now in order to move write faster, so here choose all chips's first write sub-request to serviced. not only choose the being serviced write request's corresponding chips. 

            for(temp_i = 0; temp_i < temp_channel; temp_i ++ ){

                temp_j = 0;
                temp_gc_node = ssd->channel_head[temp_i].gc_command;

                // if there is gc then skip the write adjusting.
                if(temp_gc_node != NULL){
                    continue;
                }

                while (temp_j < temp_chip){

                    temp_sub_for_write  = ssd->channel_head[temp_i].subs_w_head;
                    printf("chip is %d\n", temp_j);
                    while(temp_sub_for_write != NULL){
                        //if chip was already set, then jump out this chip. 
                        if(ssd->channel_head[temp_i].chip_head[temp_j].chip_read_stop == 1){
                            chip_has_write = 1;
                            temp_j ++;
                            break;

                        }

                        //normally set write interval.
                        if(temp_sub_for_write->location->chip == temp_j){

                            //step2, after finding out the destination chip, then set the chip's chip_write_record_begin and add temp_j number
                            ssd->channel_head[temp_i].chip_head[temp_j].chip_write_record_begin = ssd->channel_head[temp_i].chip_head[temp_j].num_w_cycle;
                            ssd->channel_head[temp_i].chip_head[temp_j].chip_read_stop = 1;
                            chip_has_write = 1;
                            printf("channel %d, chip %d, has record begin is %d\n", temp_i, temp_j, ssd->channel_head[temp_i].chip_head[temp_j].num_w_cycle);


                            temp_j ++;

                            break;
                        }else{
                            temp_sub_for_write = temp_sub_for_write->next_node;

                        }

                        printf("2222\n");
                    }//end of while

                    if(chip_has_write == 1){
                        chip_has_write = 0;
                    }else{
                        temp_j ++;
                    }
                }//end of chip number

                printf("11111\n");
            }//end channel for loop





    }//end of begin of adjusting.


//lxcprogram_for_adjust_write_suspension_number


return 1;

}

//lxcprogram_for_adjust_write_suspension_number
int  statistic_write_request_number(struct ssd_info *ssd)
{
    struct request* temp_request;
    unsigned int temp_num_write_request = 0;
//    unsigned int temp_i = 0;
    temp_request = ssd->request_queue;

    if(ssd->request_queue_length != 0){

        while(temp_request != NULL){
          
            if(temp_request->operation == 0){

                temp_num_write_request ++;
                if(temp_request == ssd->request_tail){
                    break;
                }

                temp_request = temp_request->next_node;

            }else{
                if(temp_request == ssd->request_tail){
                    break;
                }
                temp_request = temp_request->next_node;
            }
      

            printf("3333\n");
        }//end of while
    }
    return temp_num_write_request;
}


//lxc for compression
//this function for hitting in compression part and then move compression  lpn to uncompression part later.
int append_on_request_compression_to_uncompression_lpn_data(struct request* req, int lpn, int state)
{



    int temp_translation_page = lpn /  NUM_ENTRIES_PER_DIR;
    //append from head point and incase of the repeat value.
    // ignore the repeat value
    if(isnot_repeat_in_bilist(req->compression_to_uncompression, temp_translation_page)){

    lpush(req->compression_to_uncompression, temp_translation_page, state);
    traceTranslationbuffer_msg("compression hitting ,but need moving, here.translation page  is %d in the future", temp_translation_page);

}


    return 0;
}




//lxc for compression
//this function for write requests's mappingtable evict lpn
int append_on_request_evict_in_DFTL_buffer_lpn_data(struct request* req, int lpn, int state)
{
    //append from head point.
    lpush(req->should_evict_lpn_data, lpn, state);

    return 0;
}




//lxc for compression
//this function for write requests's data buffer missing subrequests
int append_on_request_missing_in_data_buffer_lpn_data(struct request* req, int lpn, int state)
{
    //append from head point.
    lpush(req->missing_in_data_buffer_lpn_data, lpn, state);
    return 0;
}
//lxc for compression
//this function is to judge whether repeating the number in bilist. 1 means isnot repeated.
int isnot_repeat_in_bilist(struct _bi_list* bilist, int lpn)
{
    struct _bi_node* temp_node = bilist->node_head;
    int i = 0;
    if(bilist->len > 0){
        for(i = 0; i < bilist->len; i++){
            if(temp_node->lpn_and_state[0] == lpn){
                return 0;
            }else{
                temp_node = temp_node->nextNode;
            }
        }
        return 1;
    }else{

        return 1;
    }

}

//this function for write requests's data buffer missing subrequests
int append_on_request_missing_in_DFTL_buffer_lpn_data(struct request* req, int lpn, int state)
{

// for translation, the missing lpn in the DFTL_buffer is translation page lpn,
#ifdef TRANSLATION_PAGE_UNIT 

    int temp_translation_page = lpn /  NUM_ENTRIES_PER_DIR;
    //append from head point and incase of the repeat value.
    // ignore the repeat value
    if(isnot_repeat_in_bilist(req->missing_in_DFTL_buffer_lpn_data, temp_translation_page)){

    lpush(req->missing_in_DFTL_buffer_lpn_data, temp_translation_page, state);
    traceTranslationbuffer_msg("no hitting,ssd buffer just will add buffer value number is %d in the future", temp_translation_page);

}
    return 0;
#endif

// for entry, the missing lpn in the DFTL_buffer is entry lpn,
#ifdef ENTRY_UNIT
//if(isnot_repeat_in_bilist(req->missing_in_DFTL_buffer_lpn_data, lpn)){

    lpush(req->missing_in_DFTL_buffer_lpn_data, lpn, state);
    traceEntrybuffer_msg("no hitting, ssd buffer just will add buffer entry lpn is %d, in the future", lpn);
//}
    return 0;


#endif
}




//lxc for compression
// this is special for write request. not getting write request's
// need_distr_flag value, but getting missing_in_data_buffer_lpn_data !!! and we should deal with the data_buffer part
struct ssd_info* write_request_get_missing_in_databuffer(struct ssd_info* ssd, unsigned int lpn, int state, struct request* req)
{
    int write_back_count, flag = 0;                                                             /*flag表示为写入新数据腾空间是否完成，0表示需要进一步腾，1表示已经腾空*/
    unsigned int i, lsn, hit_flag, add_flag, sector_count, active_region_flag = 0, free_sector = 0;
    struct buffer_group *buffer_node = NULL, *pt, *new_node = NULL, key;
    struct sub_request *sub_req = NULL, *update = NULL;
    unsigned int sub_req_state = 0, sub_req_size = 0, sub_req_lpn = 0;

////////definition above/////

    sector_count = size(state);                                                                /*需要写到buffer的sector个数*/
    key.group = lpn;
    buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->buffer,
                                                      (TREE_NODE *) &key);    /*在平衡二叉树中寻找buffer node*/



/************************************************************************************************
	*没有命中。
	*第一步根据这个lpn有多少子页需要写到buffer，去除已写回的lsn，为该lpn腾出位置，
	*首先即要计算出free sector（表示还有多少可以直接写的buffer节点）。
	*如果free_sector>=sector_count，即有多余的空间够lpn子请求写，不需要产生写回请求
	*否则，没有多余的空间供lpn子请求写，这时需要释放一部分空间，产生写回请求。就要creat_sub_request()
	*************************************************************************************************/
#ifdef DEBUG
    printf("buffer_node is %d\n",buffer_node);
#endif
    if (buffer_node == NULL) {
        
        ssdsim_msg("for write request %lld, buffer_node has not been hitted in databuffer", req);
#ifdef DEBUG
        printf("buffer_node has not been hitted\n");
#endif

        free_sector = ssd->dram->buffer->max_buffer_sector - ssd->dram->buffer->buffer_sector_count;
        if (free_sector >= sector_count) {
            flag = 1;
        }
        if (flag == 0) {
#ifdef DEBUG
            printf("buffer not hit, free_sector remained is %d,need write into sector_count is %d\n",free_sector, sector_count);
#endif

            write_back_count = sector_count - free_sector;
            if(write_back_count > 0){
                ssd->dram->buffer->write_miss_hit = ssd->dram->buffer->write_miss_hit + write_back_count;
            }
            while (write_back_count > 0) {
                sub_req = NULL;
                sub_req_state = ssd->dram->buffer->buffer_tail->stored;
#ifdef DEBUG
                printf("next is write back buffer size amount.\n");
#endif
                sub_req_size = size(ssd->dram->buffer->buffer_tail->stored);
                sub_req_lpn = ssd->dram->buffer->buffer_tail->group;
//lxc for compression
//this function for write requests's data buffer missing subrequests

				append_on_request_missing_in_data_buffer_lpn_data(req,ssd->dram->buffer->buffer_tail->group, ssd->dram->buffer->buffer_tail->stored);
                //sub_req = creat_sub_request(ssd, sub_req_lpn, sub_req_size, sub_req_state, req, WRITE);
                ssdsim_msg("now, for writesubrequest in databuffer appending lpn is: %d", sub_req_lpn);

                /**********************************************************************************
				*req不为空，表示这个insert2buffer函数是在buffer_management中调用，传递了request进来
				*req为空，表示这个函数是在process函数中处理一对多映射关系的读的时候，需要将这个读出
				*的数据加到buffer中，这可能产生实时的写回操作，需要将这个实时的写回操作的子请求挂在
				*这个读请求的总请求上
                注意在buffer_management中的sub是NULL。
				***********************************************************************************/
                if (req != NULL) {
                } else {
#ifdef DEBUG
                    printf("opps, never happen \n");
#endif
                    while(1){
                    
                    }
                   // sub_req->next_subs = sub->next_subs;//sub与req的区别。
                   // sub->next_subs = sub_req;
                }

                /*********************************************************************
				*写请求插入到了平衡二叉树，这时就要修改dram的buffer_sector_count；
				*维持平衡二叉树调用avlTreeDel()和AVL_TREENODE_FREE()函数；维持LRU算法；
				**********************************************************************/
                //ssd->dram->buffer->buffer_sector_count = ssd->dram->buffer->buffer_sector_count - sub_req->size;
                //lxcprogram_gc original wrong.
                ssd->dram->buffer->buffer_sector_count = ssd->dram->buffer->buffer_sector_count - sub_req_size;

                pt = ssd->dram->buffer->buffer_tail;
                avlTreeDel(ssd->dram->buffer, (TREE_NODE *) pt);
                if (ssd->dram->buffer->buffer_head->LRU_link_next == NULL) {
                    ssd->dram->buffer->buffer_head = NULL;
                    ssd->dram->buffer->buffer_tail = NULL;
                } else {
                    ssd->dram->buffer->buffer_tail = ssd->dram->buffer->buffer_tail->LRU_link_pre;
                    ssd->dram->buffer->buffer_tail->LRU_link_next = NULL;
                }
                pt->LRU_link_next = NULL;
                pt->LRU_link_pre = NULL;
                AVL_TREENODE_FREE(ssd->dram->buffer, (TREE_NODE *) pt);
                pt = NULL;

                write_back_count =
//                        write_back_count - sub_req->size;                            /*因为产生了实时写回操作，需要将主动写回操作区域增加*///lxcprogram_gc original wrong
                        write_back_count - sub_req_size;                            /*因为产生了实时写回操作，需要将主动写回操作区域增加*/
#ifdef DEBUG
                printf("once write insert into buffer but writeback first,  all need write into sectore is %d, but this time writeback into sector is %d \n", write_back_count, sub_req_size);
#endif
            }
        }
        else{
            ssdsim_msg("enough data buffer space for the coming write sub_request, so no adding in databuffer hit missing");
        }

        /******************************************************************************
		*生成一个buffer node，根据这个页的情况分别赋值个各个成员，添加到队首和二叉树中
		*******************************************************************************/
        ssdsim_msg("in write process, we every time ,the databuffer, here make a buffer-node and insert into buffer tree. should printf here, %d \n", lpn);
        new_node = NULL;
        new_node = (struct buffer_group *) malloc(sizeof(struct buffer_group));
        alloc_assert(new_node, "buffer_group_node");
        memset(new_node, 0, sizeof(struct buffer_group));

        new_node->group = lpn;
        new_node->stored = state;
        new_node->dirty_clean = state;
        new_node->LRU_link_pre = NULL;
        new_node->LRU_link_next = ssd->dram->buffer->buffer_head;
        if (ssd->dram->buffer->buffer_head != NULL) {
            ssd->dram->buffer->buffer_head->LRU_link_pre = new_node;
        } else {
            ssd->dram->buffer->buffer_tail = new_node;
        }
        ssd->dram->buffer->buffer_head = new_node;
        new_node->LRU_link_pre = NULL;
        avlTreeAdd(ssd->dram->buffer, (TREE_NODE *) new_node);
        ssd->dram->buffer->buffer_sector_count += sector_count;
    }
        /****************************************************************************************
	*在buffer中命中的情况
	*算然命中了，但是命中的只是lpn，有可能新来的写请求，只是需要写lpn这一page的某几个sub_page
	*这时有需要进一步的判断
	*****************************************************************************************/
    else {
#ifdef DEBUG
        printf("buffer_node hit\n");
#endif

        for (i = 0; i < ssd->parameter->subpage_page; i++) {
            /*************************************************************
			*判断state第i位是不是1
			*并且判断第i个sector是否存在buffer中，1表示存在，0表示不存在。
			**************************************************************/
            if ((state >> i) % 2 != 0) {
                lsn = lpn * ssd->parameter->subpage_page + i;
                hit_flag = 0;
                hit_flag = (buffer_node->stored) & (0x00000001 << i);

                if (hit_flag !=
                    0)                                                          /*命中了，需要将该节点移到buffer的队首，并且将命中的lsn进行标记*/
                {
                    active_region_flag = 1;                                             /*用来记录在这个buffer node中的lsn是否被命中，用于后面对阈值的判定*/

                    if (req != NULL) {
                        if (ssd->dram->buffer->buffer_head != buffer_node) {
                            if (ssd->dram->buffer->buffer_tail == buffer_node) {
                                ssd->dram->buffer->buffer_tail = buffer_node->LRU_link_pre;
                                buffer_node->LRU_link_pre->LRU_link_next = NULL;
                            } else if (buffer_node != ssd->dram->buffer->buffer_head) {
                                buffer_node->LRU_link_pre->LRU_link_next = buffer_node->LRU_link_next;
                                buffer_node->LRU_link_next->LRU_link_pre = buffer_node->LRU_link_pre;
                            }
                            buffer_node->LRU_link_next = ssd->dram->buffer->buffer_head;
                            ssd->dram->buffer->buffer_head->LRU_link_pre = buffer_node;
                            buffer_node->LRU_link_pre = NULL;
                            ssd->dram->buffer->buffer_head = buffer_node;
                        }
                        ssd->dram->buffer->write_hit++;
                        req->complete_lsn_count++;                                        /*关键 当在buffer中命中时 就用req->complete_lsn_count++表示往buffer中写了数据。*/
                    } else {
                    }
                } else {
                    /************************************************************************************************************
					*该lsn没有命中，但是节点在buffer中，需要将这个lsn加到buffer的对应节点中
					*从buffer的末端找一个节点，将一个已经写回的lsn从节点中删除(如果找到的话)，更改这个节点的状态，同时将这个新的
					*lsn加到相应的buffer节点中，该节点可能在buffer头，不在的话，将其移到头部。如果没有找到已经写回的lsn，在buffer
					*节点找一个group整体写回，将这个子请求挂在这个请求上。可以提前挂在一个channel上。
					*第一步:将buffer队尾的已经写回的节点删除一个，为新的lsn腾出空间，这里需要修改队尾某节点的stored状态这里还需要
					*       增加，当没有可以之间删除的lsn时，需要产生新的写子请求，写回LRU最后的节点。
					*第二步:将新的lsn加到所述的buffer节点中。
					*************************************************************************************************************/
                    ssd->dram->buffer->write_miss_hit++;

                    if (ssd->dram->buffer->buffer_sector_count >= ssd->dram->buffer->max_buffer_sector) {
                        if (buffer_node ==
                            ssd->dram->buffer->buffer_tail)                  /*如果命中的节点是buffer中最后一个节点，交换最后两个节点*/
                        {
                            pt = ssd->dram->buffer->buffer_tail->LRU_link_pre;
                            ssd->dram->buffer->buffer_tail->LRU_link_pre = pt->LRU_link_pre;
                            ssd->dram->buffer->buffer_tail->LRU_link_pre->LRU_link_next = ssd->dram->buffer->buffer_tail;
                            ssd->dram->buffer->buffer_tail->LRU_link_next = pt;
                            pt->LRU_link_next = NULL;
                            pt->LRU_link_pre = ssd->dram->buffer->buffer_tail;
                            ssd->dram->buffer->buffer_tail = pt;

                        }
                        sub_req = NULL;
                        sub_req_state = ssd->dram->buffer->buffer_tail->stored;
                        sub_req_size = size(ssd->dram->buffer->buffer_tail->stored);
                        sub_req_lpn = ssd->dram->buffer->buffer_tail->group;
//lxc for compression
//this function for write requests's data buffer missing subrequests
                        append_on_request_missing_in_data_buffer_lpn_data(req, sub_req_lpn, sub_req_state);

                        ssdsim_msg("now,in hit part,no hitmiss part, for writesubrequest in databuffer appending lpn is: %d", sub_req_lpn);

//                        sub_req = creat_sub_request(ssd, sub_req_lpn, sub_req_size, sub_req_state, req, WRITE);

                        if (req != NULL) {

                        } else if (req == NULL) {
                            while(1){
                            
                            }
                           // sub_req->next_subs = sub->next_subs;
                           // sub->next_subs = sub_req;
                        }

                //        ssd->dram->buffer->buffer_sector_count = ssd->dram->buffer->buffer_sector_count - sub_req->size;
                        ssd->dram->buffer->buffer_sector_count = ssd->dram->buffer->buffer_sector_count - sub_req_size;
                        pt = ssd->dram->buffer->buffer_tail;
                        avlTreeDel(ssd->dram->buffer, (TREE_NODE *) pt);

                        /************************************************************************/
                        /* 改:  挂在了子请求，buffer的节点不应立即删除，						*/
                        /*			需等到写回了之后才能删除									*/
                        /************************************************************************/
                        if (ssd->dram->buffer->buffer_head->LRU_link_next == NULL) {
                            ssd->dram->buffer->buffer_head = NULL;
                            ssd->dram->buffer->buffer_tail = NULL;
                        } else {
                            ssd->dram->buffer->buffer_tail = ssd->dram->buffer->buffer_tail->LRU_link_pre;
                            ssd->dram->buffer->buffer_tail->LRU_link_next = NULL;
                        }
                        pt->LRU_link_next = NULL;
                        pt->LRU_link_pre = NULL;
                        AVL_TREENODE_FREE(ssd->dram->buffer, (TREE_NODE *) pt);
                        pt = NULL;
                    }

                    /*第二步:将新的lsn加到所述的buffer节点中*/
                    add_flag = 0x00000001 << (lsn % ssd->parameter->subpage_page);

                    if (ssd->dram->buffer->buffer_head !=
                        buffer_node)                      /*如果该buffer节点不在buffer的队首，需要将这个节点提到队首*/
                    {
                        if (ssd->dram->buffer->buffer_tail == buffer_node) {
                            buffer_node->LRU_link_pre->LRU_link_next = NULL;
                            ssd->dram->buffer->buffer_tail = buffer_node->LRU_link_pre;
                        } else {
                            buffer_node->LRU_link_pre->LRU_link_next = buffer_node->LRU_link_next;
                            buffer_node->LRU_link_next->LRU_link_pre = buffer_node->LRU_link_pre;
                        }
                        buffer_node->LRU_link_next = ssd->dram->buffer->buffer_head;
                        ssd->dram->buffer->buffer_head->LRU_link_pre = buffer_node;
                        buffer_node->LRU_link_pre = NULL;
                        ssd->dram->buffer->buffer_head = buffer_node;
                    }
                    buffer_node->stored = buffer_node->stored | add_flag;
                    buffer_node->dirty_clean = buffer_node->dirty_clean | add_flag;
                    ssd->dram->buffer->buffer_sector_count++;
                }

            }
        }//end of for subpage's for loop
    }

    return ssd;






}//end of write_request_get_missing_in_databuffer

//lxc for compression

void printf_read_request_need_distr_flag_value_and_illustion(struct ssd_info* ssd, struct request* req) 
{

    unsigned int i, lsn, lpn,last_lpn, first_lpn, index;
    unsigned int need_distb_flag;
    lsn = req->lsn;
    lpn = req->lsn / ssd->parameter->subpage_page;

    last_lpn = (req->lsn + req->size - 1) / ssd->parameter->subpage_page;
    first_lpn = req->lsn / ssd->parameter->subpage_page;


    printf("read address: %lld  need_distr_flag is from tail to head,%d, begin lsn is  %d \n", req, req->lsn);
    while(lpn <= last_lpn){

        unsigned int * temp;
        index = (lpn - first_lpn) / (32 / ssd->parameter->subpage_page);

            need_distb_flag = req->need_distr_flag[index] & ( (0x0000000f << ( ((lpn - first_lpn) %(32 /ssd->parameter->subpage_page)) * ssd->parameter->subpage_page) ));
            printf("only last 4 bits: %x, index %d, lpn %d\n", need_distb_flag, index, lpn);

        lpn++;
    }//end of while
    printf("\n and req total lpn number is %d , 0 means hit,  needing distributing\n", last_lpn - first_lpn + 1);
}//end of printf_read_request_need_distr_flag_value_and_illustion


//lxc for compression

void printf_write_request_need_distr_flag_value_and_illustion(struct ssd_info* ssd,struct request* new_request)
{
    int i, len;
    struct _bi_node* temp_node = NULL;
    temp_node = new_request->missing_in_data_buffer_lpn_data->node_head; 
    if(new_request->missing_in_data_buffer_lpn_data->len != 0){

        printf("write request adress: %lld, has no hit, nohit number is %d\n", new_request, new_request->missing_in_data_buffer_lpn_data->len);
        for(i = 0; i < new_request->missing_in_data_buffer_lpn_data->len; i ++){
            printf("%d  ",temp_node->lpn_and_state[0]);
            if(temp_node == NULL){
                printf("wrong in printf_write_request_need_distr_flag\n");
                while(1){

                }
            }
            temp_node = temp_node->nextNode;
        }//end of for loop
        printf("\n");
    }else{
        printf("write request adress: %lld, all hit in data buffer\n", new_request);
    }

}//end of printf_write_request_need_distr_flag_value_and_illustion


//lxc for compression
///****************this is to get every requests need_distr_flag value here.
struct ssd_info *DFTL_buffer_management(struct ssd_info *ssd) 
{
    unsigned int j, lsn, lpn, last_lpn, first_lpn, index, complete_flag = 0, state, full_page;
    unsigned int flag = 0, need_distb_flag, lsn_flag, flag1 = 1, active_region_flag = 0;
    unsigned int in_index_offset = 0;
    struct request *new_request;
    struct buffer_group *buffer_node, key;
    unsigned int mask = 0, offset1 = 0, offset2 = 0;
#ifdef DEBUG_CURR_TIME 
    printf("DEBUG_CURR_TIME is %lld in buffer_management function \n",ssd->current_time);
#endif

    ssd->dram->current_time = ssd->current_time;
    full_page = ~(0xffffffff << ssd->parameter->subpage_page);//目前是1111.

    new_request = ssd->request_tail;
    if(new_request == NULL){
        printf("now no requests in ssd\n");
        return ssd;
    }
    lsn = new_request->lsn;
    lpn = new_request->lsn / ssd->parameter->subpage_page;
    last_lpn = (new_request->lsn + new_request->size - 1) / ssd->parameter->subpage_page;
    first_lpn = new_request->lsn / ssd->parameter->subpage_page;

    new_request->need_distr_flag = (unsigned int *) malloc(
            sizeof(unsigned int) * ((last_lpn - first_lpn + 1) * ssd->parameter->subpage_page / 32 + 1));
    //    printf("unsigned int length is %d\n",sizeof(unsigned int));
    alloc_assert(new_request->need_distr_flag, "new_request->need_distr_flag");
    memset(new_request->need_distr_flag, 0,
            sizeof(unsigned int) * ((last_lpn - first_lpn + 1) * ssd->parameter->subpage_page / 32 + 1));
#ifdef DEBUG
    printf("yes, here is the request_tail for new_request ,its lsn is %d, size is %d, operation is %d\n",new_request->lsn, new_request->size, new_request->operation);
#endif

    if (new_request->operation == READ) {
        while (lpn <= last_lpn) {
            /************************************************************************************************
             *need_distb_flag表示是否需要执行distribution函数，1表示需要执行，buffer中没有，0表示不需要执行
             *即1表示需要分发，0表示不需要分发，对应点初始全部赋为1
             *************************************************************************************************/
            ssdsim_msg("in read request find in databuffer, lpn is %d", lpn);
            need_distb_flag = full_page;
            key.group = lpn;
            buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->buffer,
                    (TREE_NODE *) &key);        // buffer node

            ssdsim_msg("in read, buffer_node is %lld", buffer_node);

            while ((buffer_node != NULL) && (lsn < (lpn + 1) * ssd->parameter->subpage_page) &&
                    (lsn <= (new_request->lsn + new_request->size - 1))) {
#ifdef DEBUG //lxcv1
                printf("buffer_node is not null, find in Tree the buffer_node information is :\n");

                printf("buffer_node. group %d, stored %d, dirty_clean %d, flag %d\n", buffer_node->group, buffer_node->stored, buffer_node->dirty_clean, buffer_node->flag);

#endif

                lsn_flag = full_page;
                mask = 1 << (lsn % ssd->parameter->subpage_page);
                if (mask > 31) {//此处一定是错误的。。lxc。
                    printf("the subpage number is larger than 32!add some cases");
                    getchar();
                } else if ((buffer_node->stored & mask) == mask) {
                    flag = 1;
                    lsn_flag = lsn_flag & (~mask);
#ifdef DEBUG //lxcv1
                    printf("lsn_flag is 0x%x, buffer_node->stored？？？where is hole is 0x%x, mask is 0x%x\n",lsn_flag,  buffer_node->stored,mask);
                    //注意观察其中的stored是否有空洞？？？应该没有，都是整体没有的。
#endif

                }

                if (flag == 1) {    //如果该buffer节点不在buffer的队首，需要将这个节点提到队首，实现了LRU算法，这个是一个双向队列。
                    if (ssd->dram->buffer->buffer_head != buffer_node) {
                        if (ssd->dram->buffer->buffer_tail == buffer_node) {
                            buffer_node->LRU_link_pre->LRU_link_next = NULL;
                            ssd->dram->buffer->buffer_tail = buffer_node->LRU_link_pre;
                        } else {
                            buffer_node->LRU_link_pre->LRU_link_next = buffer_node->LRU_link_next;
                            buffer_node->LRU_link_next->LRU_link_pre = buffer_node->LRU_link_pre;
                        }
                        buffer_node->LRU_link_next = ssd->dram->buffer->buffer_head;
                        ssd->dram->buffer->buffer_head->LRU_link_pre = buffer_node;
                        buffer_node->LRU_link_pre = NULL;
                        ssd->dram->buffer->buffer_head = buffer_node;
                    }
                    ssd->dram->buffer->read_hit++;
                    new_request->complete_lsn_count++;
                } else if (flag == 0) {
                    ssd->dram->buffer->read_miss_hit++;
#ifdef DEBUG

                    printf("buffer->read_miss_hit++ is %d\n",ssd->dram->buffer->read_miss_hit);
#endif
                }

                need_distb_flag = need_distb_flag & lsn_flag;

                flag = 0;
                lsn++;
            }

            index = (lpn - first_lpn) / (32 / ssd->parameter->subpage_page);
            new_request->need_distr_flag[index] = new_request->need_distr_flag[index] | (need_distb_flag
                    << (((lpn - first_lpn) % (32 / ssd->parameter->subpage_page)) * ssd->parameter->subpage_page));

            traceError_msg("need_distb_flag is %d", need_distb_flag);
            lpn++;

        }
        //this function is just for debug

#ifdef compression_relevant
        printf_read_request_need_distr_flag_value_and_illustion(ssd, new_request);
#endif

    } else if (new_request->operation == WRITE) {
        //index = (lpn - first_lpn) / (32 / ssd->parameter->subpage_page);

        //in_index_offset = (((lpn - first_lpn) % (32 / ssd->parameter->subpage_page)) * ssd->parameter->subpage_page);


        while (lpn <= last_lpn) {
            need_distb_flag = full_page;
            mask = ~(0xffffffff << (ssd->parameter->subpage_page));
            state = mask;

            if (lpn == first_lpn) {
                offset1 = ssd->parameter->subpage_page - ((lpn + 1) * ssd->parameter->subpage_page - new_request->lsn);
                state = state & (0xffffffff << offset1);
            }
            if (lpn == last_lpn) {
                offset2 = ssd->parameter->subpage_page -
                    ((lpn + 1) * ssd->parameter->subpage_page - (new_request->lsn + new_request->size));
                state = state & (~(0xffffffff << offset2));
            }
#ifdef  DEBUG
            printf("write lpn state is 0x%x, offset1 is 0x%x, offset2 is 0x%x\n",state,offset1, offset2 );
#endif

            //lxc compression  
            //ssd = insert2buffer(ssd, lpn, state, NULL, new_request);
            ssdsim_msg("before the judging of appending of the missing in databuffer, lpn is %d", lpn);
            write_request_get_missing_in_databuffer(ssd, lpn, state, new_request);

            lpn++;

        }// end of lpn count
        //this function is just for debug

#ifdef compression_relevant
        printf_write_request_need_distr_flag_value_and_illustion(ssd, new_request);
#endif

    }//end of write request

    //arrange the response_time for read request
    if(new_request->operation == READ){
    complete_flag = 1;
    for (j = 0; j <= (last_lpn - first_lpn + 1) * ssd->parameter->subpage_page / 32; j++) {
        if (new_request->need_distr_flag[j] != 0) {
            complete_flag = 0;
        }
    }//end of for loop
    if (complete_flag == 1)  {
        //new_request->begin_time = ssd->current_time;
        new_request->response_time = ssd->current_time + 1000;
    }

    traceError_msg("read request %lld response_time %lld", new_request, new_request->response_time);
    }//end of judgement for read request
    if(new_request->operation == WRITE){
        //arrange the response_time for write request
traceError_msg(" write request %lld, missing_in_data_buffer_lpn_data->len is %d", new_request, new_request->missing_in_data_buffer_lpn_data->len);

        if (new_request->missing_in_data_buffer_lpn_data->len == 0) {

            //new_request->begin_time = ssd->current_time;
            new_request->response_time = ssd->current_time + 1000;
        }else{

            traceError_msg(" write request %lld, xxxxxxx_missing_in_data_buffer_lpn_data->len is %d", new_request, new_request->missing_in_data_buffer_lpn_data->len);
        }

    traceError_msg("write request %lld response_time %lld", new_request, new_request->response_time);

    }//end of judgement for write reuqest


#ifdef DEBUG
    printf("complete_flag is %d, (0 indicates that not be totally seviced by buffer directely)\n", complete_flag);
#endif
    /*************************************************************
     *如果请求已经被全部由buffer服务，该请求可以被直接响应，输出结果
     *这里假设dram的服务时间为1000ns
     **************************************************************/
    //lxc for compression
    // judgement below shows read and write request
    //if ((complete_flag == 1) && (new_request->subs == NULL)) {
//    if ((complete_flag == 1) && (new_request->missing_in_data_buffer_lpn_data->len = 0)) {
//        new_request->begin_time = ssd->current_time;
//        new_request->response_time = ssd->current_time + 1000;
//    }



    //lxc for compression
    //if there is mappint table missing then jump out from distribute. 
//#ifdef COMPRESSION_MAPPING 
	//COMPRESSION_value_request_DFTL_parameters_for_distribute_subrequests(ssd);
    value_request_DFTL_parameters_for_distribute_subrequests(ssd);

//#endif

    fprintf(ssd->fp_number_subrequests_txt, "%d request lsn %d, subrequestnubmer is %d\n", new_request->operation, new_request->lsn, new_request->missing_in_data_buffer_lpn_data->len);


return ssd;
}//end of DFTL_buffer_management



/**********************************************************************************************************************************************
*首先buffer是个写buffer，就是为写请求服务的，因为读flash的时间tR为20us，写flash的时间tprog为200us，所以为写服务更能节省时间
*  读操作：如果命中了buffer，从buffer读，不占用channel的I/O总线，没有命中buffer，从flash读，占用channel的I/O总线，但是不进buffer了
*  写操作：首先request分成sub_request子请求，如果是动态分配，sub_request挂到ssd->sub_request上，因为不知道要先挂到哪个channel的sub_request上
*          如果是静态分配则sub_request挂到channel的sub_request链上,同时不管动态分配还是静态分配sub_request都要挂到request的sub_request链上
*		   因为每处理完一个request，都要在traceoutput文件中输出关于这个request的信息。处理完一个sub_request,就将其从channel的sub_request链
*		   或ssd的sub_request链上摘除，但是在traceoutput文件输出一条后再清空request的sub_request链。
*		   sub_request命中buffer则在buffer里面写就行了，并且将该sub_page提到buffer链头(LRU)，若没有命中且buffer满，则先将buffer链尾的sub_request
*		   写入flash(这会产生一个sub_request写请求，挂到这个请求request的sub_request链上，同时视动态分配还是静态分配挂到channel或ssd的
*		   sub_request链上),在将要写的sub_page写入buffer链头
***********************************************************************************************************************************************/
struct ssd_info *buffer_management(struct ssd_info *ssd) {
    unsigned int j, lsn, lpn, last_lpn, first_lpn, index, complete_flag = 0, state, full_page;
    unsigned int flag = 0, need_distb_flag, lsn_flag, flag1 = 1, active_region_flag = 0;
    struct request *new_request;
    struct buffer_group *buffer_node, key;
    unsigned int mask = 0, offset1 = 0, offset2 = 0;
#ifdef DEBUG_CURR_TIME 
    printf("DEBUG_CURR_TIME is %lld in buffer_management function \n",ssd->current_time);
#endif

    ssd->dram->current_time = ssd->current_time;
    full_page = ~(0xffffffff << ssd->parameter->subpage_page);//目前是1111.

    new_request = ssd->request_tail;
    lsn = new_request->lsn;
    lpn = new_request->lsn / ssd->parameter->subpage_page;
    last_lpn = (new_request->lsn + new_request->size - 1) / ssd->parameter->subpage_page;
    first_lpn = new_request->lsn / ssd->parameter->subpage_page;

    new_request->need_distr_flag = (unsigned int *) malloc(
            sizeof(unsigned int) * ((last_lpn - first_lpn + 1) * ssd->parameter->subpage_page / 32 + 1));
//    printf("unsigned int length is %d\n",sizeof(unsigned int));
    alloc_assert(new_request->need_distr_flag, "new_request->need_distr_flag");
    memset(new_request->need_distr_flag, 0,
           sizeof(unsigned int) * ((last_lpn - first_lpn + 1) * ssd->parameter->subpage_page / 32 + 1));
#ifdef DEBUG
    printf("yes, here is the request_tail for new_request ,its lsn is %d, size is %d, operation is %d\n",new_request->lsn, new_request->size, new_request->operation);
#endif

    if (new_request->operation == READ) {
        while (lpn <= last_lpn) {
            /************************************************************************************************
             *need_distb_flag表示是否需要执行distribution函数，1表示需要执行，buffer中没有，0表示不需要执行
             *即1表示需要分发，0表示不需要分发，对应点初始全部赋为1
            *************************************************************************************************/
            need_distb_flag = full_page;
            key.group = lpn;
            buffer_node = (struct buffer_group *) avlTreeFind(ssd->dram->buffer,
                                                              (TREE_NODE *) &key);        // buffer node



            while ((buffer_node != NULL) && (lsn < (lpn + 1) * ssd->parameter->subpage_page) &&
                   (lsn <= (new_request->lsn + new_request->size - 1))) {
#ifdef DEBUG //lxcv1
            printf("buffer_node is not null, find in Tree the buffer_node information is :\n");

            printf("buffer_node. group %d, stored %d, dirty_clean %d, flag %d\n", buffer_node->group, buffer_node->stored, buffer_node->dirty_clean, buffer_node->flag);

#endif

                lsn_flag = full_page;
                mask = 1 << (lsn % ssd->parameter->subpage_page);
                if (mask > 31) {//此处一定是错误的。。lxc。
                    printf("the subpage number is larger than 32!add some cases");
                    getchar();
                } else if ((buffer_node->stored & mask) == mask) {
                    flag = 1;
                    lsn_flag = lsn_flag & (~mask);
#ifdef DEBUG //lxcv1
            printf("lsn_flag is 0x%x, buffer_node->stored？？？where is hole is 0x%x, mask is 0x%x\n",lsn_flag,  buffer_node->stored,mask);
//注意观察其中的stored是否有空洞？？？应该没有，都是整体没有的。
#endif

                }

                if (flag == 1) {    //如果该buffer节点不在buffer的队首，需要将这个节点提到队首，实现了LRU算法，这个是一个双向队列。
                    if (ssd->dram->buffer->buffer_head != buffer_node) {
                        if (ssd->dram->buffer->buffer_tail == buffer_node) {
                            buffer_node->LRU_link_pre->LRU_link_next = NULL;
                            ssd->dram->buffer->buffer_tail = buffer_node->LRU_link_pre;
                        } else {
                            buffer_node->LRU_link_pre->LRU_link_next = buffer_node->LRU_link_next;
                            buffer_node->LRU_link_next->LRU_link_pre = buffer_node->LRU_link_pre;
                        }
                        buffer_node->LRU_link_next = ssd->dram->buffer->buffer_head;
                        ssd->dram->buffer->buffer_head->LRU_link_pre = buffer_node;
                        buffer_node->LRU_link_pre = NULL;
                        ssd->dram->buffer->buffer_head = buffer_node;
                    }
                    ssd->dram->buffer->read_hit++;
                    new_request->complete_lsn_count++;
                } else if (flag == 0) {
                    ssd->dram->buffer->read_miss_hit++;
#ifdef DEBUG

                    printf("buffer->read_miss_hit++ is %d\n",ssd->dram->buffer->read_miss_hit);
#endif
                }

                need_distb_flag = need_distb_flag & lsn_flag;

                flag = 0;
                lsn++;
            }

            index = (lpn - first_lpn) / (32 / ssd->parameter->subpage_page);
            new_request->need_distr_flag[index] = new_request->need_distr_flag[index] | (need_distb_flag
                    << (((lpn - first_lpn) % (32 / ssd->parameter->subpage_page)) * ssd->parameter->subpage_page));
            lpn++;

        }
    } else if (new_request->operation == WRITE) {
        while (lpn <= last_lpn) {
            need_distb_flag = full_page;
            mask = ~(0xffffffff << (ssd->parameter->subpage_page));
            state = mask;

            if (lpn == first_lpn) {
                offset1 = ssd->parameter->subpage_page - ((lpn + 1) * ssd->parameter->subpage_page - new_request->lsn);
                state = state & (0xffffffff << offset1);
            }
            if (lpn == last_lpn) {
                offset2 = ssd->parameter->subpage_page -
                          ((lpn + 1) * ssd->parameter->subpage_page - (new_request->lsn + new_request->size));
                state = state & (~(0xffffffff << offset2));
            }
#ifdef  DEBUG
            printf("write lpn state is 0x%x, offset1 is 0x%x, offset2 is 0x%x\n",state,offset1, offset2 );
#endif
            ssd = insert2buffer(ssd, lpn, state, NULL, new_request);
            lpn++;
        }
    }
    complete_flag = 1;
    for (j = 0; j <= (last_lpn - first_lpn + 1) * ssd->parameter->subpage_page / 32; j++) {
        if (new_request->need_distr_flag[j] != 0) {
            complete_flag = 0;
        }
    }
#ifdef DEBUG
    printf("complete_flag is %d, (0 indicates that not be totally seviced by buffer directely)\n", complete_flag);
#endif
    /*************************************************************
    *如果请求已经被全部由buffer服务，该请求可以被直接响应，输出结果
    *这里假设dram的服务时间为1000ns
    **************************************************************/
    if ((complete_flag == 1) && (new_request->subs == NULL)) {
        new_request->begin_time = ssd->current_time;
        new_request->response_time = ssd->current_time + 1000;
    }

    return ssd;
}

/*****************************
*lpn向ppn的转换
******************************/
unsigned int lpn2ppn(struct ssd_info *ssd, unsigned int lsn) {
    int lpn, ppn;
    struct entry *p_map = ssd->dram->map->map_entry;
#ifdef DEBUG_CURR_TIME 
    printf("enter lpn2ppn,  current time:%lld\n",ssd->current_time);
#endif

    lpn = lsn / ssd->parameter->subpage_page;            //lpn
    ppn = (p_map[lpn]).pn;
    return ppn;
}

/**********************************************************************************
*读请求分配子请求函数，这里只处理读请求，写请求已经在buffer_management()函数中处理了
*根据请求队列和buffer命中的检查，将每个请求分解成子请求，将子请求队列挂在channel上，
*不同的channel有自己的子请求队列
**********************************************************************************/

struct ssd_info * distribute(struct ssd_info *ssd) {
    unsigned int start, end, first_lsn, last_lsn, lpn, flag = 0, flag_attached = 0, full_page;
    unsigned int j, k, sub_size;
    int i = 0;
    struct request *req;
    struct sub_request *sub;
    int *complt;

#ifdef DEBUG_CURR_TIME 
    printf("enter distribute,  current time:%lld\n",ssd->current_time);
    printf_ssd_request_queue(ssd);
#endif
    full_page = ~(0xffffffff << ssd->parameter->subpage_page);

    req = ssd->request_tail;
    if (req->response_time != 0) {
#ifdef DEBUG
        printf("req->response_time !=0\n");
#endif
        return ssd;
    }
    if (req->operation == WRITE) {
#ifdef DEBUG
        printf("ssd->req_tail's operation is write\n");
#endif
        return ssd;
    }

    if (req != NULL) {
        if (req->distri_flag == 0) {
            //如果还有一些读请求需要处理
            if (req->complete_lsn_count != ssd->request_tail->size) {
                first_lsn = req->lsn;
                last_lsn = first_lsn + req->size;
                complt = req->need_distr_flag;
                start = first_lsn - first_lsn % ssd->parameter->subpage_page;
                end = (last_lsn / ssd->parameter->subpage_page + 1) * ssd->parameter->subpage_page;
                i = (end - start) / 32;

                //lxc for compression

                //get_one_request_missing_part();

                while (i >= 0) {
                    /*************************************************************************************
                    *一个32位的整型数据的每一位代表一个子页，32/ssd->parameter->subpage_page就表示有多少页，
                    *这里的每一页的状态都存放在了 req->need_distr_flag中，也就是complt中，通过比较complt的
                    *每一项与full_page，就可以知道，这一页是否处理完成。如果没处理完成则通过creat_sub_request
                    函数创建子请求。
                    *************************************************************************************/
                    for (j = 0; j < 32 / ssd->parameter->subpage_page; j++) {
                        k = (complt[((end - start) / 32 - i)] >> (ssd->parameter->subpage_page * j)) & full_page;
                        if (k != 0) {
                            lpn = start / ssd->parameter->subpage_page +
                                  ((end - start) / 32 - i) * 32 / ssd->parameter->subpage_page + j;
                            sub_size = transfer_size(ssd, k, lpn, req);
                            if (sub_size == 0) {
                                continue;
                            } else {
                                sub = creat_sub_request(ssd, lpn, sub_size, 0, req, req->operation);


                            }
                        }
                    }
                    i = i - 1;
                }

                //lxcprogram_de-prioritize.
                //1、 req's every sub_request's slack_time.
                //2、 insert the new sub_requests into channel_queue's respective chip
        //        calc_req_slack_time(req);
        //        reordering_req(req);


            } else {
                req->begin_time = ssd->current_time;
				req->response_time = ssd->current_time + 1000;
            }

        }
    }
    return ssd;
}


//lxcprogram_de-prioritize
int calc_req_slack_time(struct request * req)
{
    unsigned int channel_max, chip_max;
    struct sub_request *sub;
    struct sub_request *temp_sub;
    int64_t temp_time;
    sub = req->subs;
    temp_time = 0;

    while (sub != NULL){
        if(temp_time < sub->serviced_time){

            temp_time = sub->serviced_time;
            channel_max = sub->location->channel;
            chip_max = sub->location->chip ;
            temp_sub = sub;

            sub = sub->next_subs;
        }

    }

    temp_sub->last_sign = 1;

    sub = req->subs;
    while (sub != NULL){

        sub->slack_time = temp_time - sub->serviced_time;
#ifdef DEBUG 
         printf("req %u, sub slack_time is %d\n", req, sub->slack_time);
#endif

        sub = sub->next_subs;
#ifdef DEBUG 
        if(sub->slack_time == 0) {
            
            printf("channel_max %d, chip_max %d, sub->location->channel %d, sub->location->chip %d, they are should be the same\n", channel_max, chip_max, sub->location->channel, sub->location->chip);
        }
#endif

    }
    return 0;
}




int reordering_req(struct request * req )
{

//    unsigned int channel, chip, req_reorder_sign;
//    unsigned int temp_slack[1000];
//    struct sub_request *sub, temp_sub;
//    int64_t one_read_time;
//    req_reorder_sign = 1;
//    one_read_time = (ssd->parameter->time_characteristics.tR + 7 * ssd->parameter->time_characteristics.tWC + (sub->size * ssd->parameter->subpage_capacity) * ssd->parameter->time_characteristics.tWC);
//    //find req's last sub-request.then get the channel and chip.
//    sub = req->subs;
//    while (sub != NULL){
//
//        if(sub->last_sign == 1){
//            break;
//        }
//        sub = sub->next_subs;
//    }
//
//channel = sub->location->channel;
//chip = sub->location->chip;
////while (req_reorder_sign == 1){
//
//
//// insert last_sign at first, remember to change moved sub-requests's slack_time.
//sub = ssd->channel_head[channel]->subs_r_head;//here the subs_r_tail will not be NULL, since just adding the req request.
//while (sub->location->chip == chip ){
//
//if(sub->slack_time > one_read_time){
//sub = sub->next_node
//
//}


}
// recalculate req 's new slack_time
//find the last but one slack_time as 

//}//end of req_reorder_sign
// then due to change, so find all other (slack_time = 0)'s possible schedule. 



//lxc for compression

void delete_request_without_subs(struct request* req)
{
    if(req->subs != NULL){
        ssdsim_msg("wrong in deleting the request part, before deleting, there should no subs");
        while(1){
        }
    }
    //steps: delete all malloc spaces in request
    //step1 delete need_distr_flag;
    free(req->need_distr_flag);
    req->need_distr_flag = NULL;
    //step2 delete evict_sub_request
    if(req->evict_sub_request != NULL){
        ssdsim_msg("wrong in deleting evict_sub_request.");
        while(1){
        }
    }

    //step3 delete fetch_sub_request
    if(req->fetch_sub_request != NULL){
        ssdsim_msg("wrong in deleting fetch_sub_request.");
        while(1){
        }
    }


    //step4 delete should_evict_lpn_data

    if(req->should_evict_lpn_data != NULL){ // since if there are should_evict lpn, we have delete, if not NULL, then means new bi_list, so there is no any bi_nodes, so just del_bi_list(this new _bi_list) is ok.
        if(req->should_evict_lpn_data->len == 0){
            del_bi_list(req->should_evict_lpn_data);
            req->should_evict_lpn_data = NULL;

        }else{
            ssdsim_msg("wrong in deleting should_evict_lpn_data.");

            while(1){
            }
        }


    }

    //step5 delete missing_in_DFTL_buffer_lpn_data
    if(req->missing_in_DFTL_buffer_lpn_data != NULL){

        if(req->missing_in_DFTL_buffer_lpn_data->len == 0){
            del_bi_list(req->missing_in_DFTL_buffer_lpn_data);
            req->missing_in_DFTL_buffer_lpn_data = NULL;

        }else{
            ssdsim_msg("wrong in deleting missing_in_DFTL_buffer_lpn_data length is %d",req->missing_in_DFTL_buffer_lpn_data->len);

            while(1){
            }
        }

    }

    //step6 delete missing_in_data_buffer_lpn_data

    if(req->missing_in_data_buffer_lpn_data != NULL){

        del_bi_list(req->missing_in_data_buffer_lpn_data);
        req->missing_in_data_buffer_lpn_data = NULL;

    }




    free(req);

}//end of delete_request




void test_finish_deletion(struct ssd_info* ssd)
{
    int flag = 1;
    int64_t start_time, end_time;
    struct request *req, *pre_node;
    struct sub_request *sub, *tmp;


    ssdsim_msg("enter test_finish_deletion.current_time:%lld\n",ssd->current_time);
    req = ssd->request_queue;
    pre_node = req;
    start_time = 0;
    end_time = 0;

    if (req == NULL){
#ifdef DEBUG
        printf("ssd->request_queue is NULL\n");
#endif
        return;
    }
    //**************************step0********************************//
    // begin the while loop for every req
    while (req != NULL) {//this NULL must be made sure by request_tail->next_node value!!!!

        //**************************step1********************************//
        //every request should withstand the check by logic

        //lxc for compression
        //here must escape from the mappingtable dealing request.
        if((req->total_wait_fetch_mappingtable == 1 )||(req->total_wait_evict == 1) || (req->create_or_notyet_normal_subrequests == 0)){
            if(ssd->request_queue_length == 1){
                return;
            }
            pre_node = req;
            req = req->next_node;
            continue;

        }

        //here, judge the logic again.
        if(req->missing_in_data_buffer_lpn_data->len != 0){
            if(req->response_time != 0){
                ssdsim_msg("wrong in request response time value req operation %d, address is %lld", req->operation, req);
                while(1){

                }

                //if(req->operation == 0){
                //    while(1){

                //    }
                //}
            }
        }



        //**************************end of step1********************************//

        if (req->response_time != 0) {
//
//            //**************************step2********************************//
//            // begin judge the request which in response_time is not 0
//
//
//            ssdsim_msg("xxxxxxxdebugxxxxxx%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size, req->operation, req->begin_time, req->response_time, req->response_time - req->time);
//            //**************************step2.1********************************//
//            //begin the fprtintf dealing in ex.out   and the calculation of ssd information, such as, write/read_request_count, write/read average, request_queue_length and so on.    
//
//
//            //            printf("responding time has response_time here. ssd->request_queue is 0x%x\n", ssd->request_queue);
//#ifdef DEBUGTRACE_OUT
//            printf("only has entire req->response_time one time,dont care req's subrequest. \n");
//#endif
//
//
//
//#ifdef DEBUGSUSPEND
//            printf("%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size, req->operation, req->begin_time, req->response_time, req->response_time - req->time);
//
//#endif
//            fprintf(ssd->outputfile, "%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size,
//                    req->operation, req->begin_time, req->response_time, req->response_time - req->time);
//            fflush(ssd->outputfile);
//
//            if (req->response_time - req->begin_time == 0) {
//                printf("the response time is 0?? \n");
//                getchar();
//                while(1){
//
//                }
//            }
//
//            if (req->operation == READ) {
//                ssd->read_request_count++;
//                ssd->read_avg = ssd->read_avg + (req->response_time - req->time);
//            } else {
//                ssd->write_request_count++;
//                ssd->write_avg = ssd->write_avg + (req->response_time - req->time);
//            }
//
//
//
//            //**************************step2.2********************************//
//            //begin to delete. not only deletion, but also get the right req value to do next while(req != NULL) judgement.
//
//            // when the first request delete
//            if(req == ssd->request_queue){
//
//                //dealing pre_node should before the free(req)
//                pre_node =pre_node->next_node;
//                ssd->request_queue = pre_node;
//
//                free(req->need_distr_flag);
//                req->need_distr_flag = NULL;
//                free(req);
//                ssd->request_queue_length--;
//
//
//                req = pre_node;
//                //here we also need to cut down the request_tail in case of request_tail is not NULL 
//                if(ssd->request_queue == NULL){
//                    ssd->request_tail = NULL;
//
//                }
//
//                continue;
//
//            }else {
//
//
//
//                if (req == ssd->request_tail){
//
//                    pre_node->next_node = req->next_node;
//                    if(pre_node->next_node != NULL){
//                        ssdsim_msg("1 logic wrong in request tail");
//                        while(1){
//
//                        }
//                    }
//                    ssd->request_tail = pre_node;
//
//                    free(req->need_distr_flag);
//                    req->need_distr_flag = NULL;
//                    free(req);
//                    req = NULL;
//
//                    ssd->request_queue_length--;
//                    // break; // here jump out all the requesting.
//                    continue; // we believe the operation than break. even break is right.
//
//                }else {
//
//
//                    pre_node->next_node = req->next_node;
//
//                    free(req->need_distr_flag);
//                    req->need_distr_flag = NULL;
//                    free(req);
//
//                    req = pre_node->next_node;
//                    ssd->request_queue_length--;
//                    continue;
//
//                }
//            }//end of no begin of the request.
//            // end of request 1000 
//
//            //**************************step2.3********************************//
//            //after every deletion, we should check the list itself
//            if(ssd->request_queue == NULL){
//                if(ssd->request_tail != NULL){
//                    ssdsim_msg("1 deletion in requests, no subrequests' requests wrong");
//                    while(1){
//
//                    }
//                }
//
//            }
//            if(ssd->request_tail != NULL){
//                if(ssd->request_tail->next_node != NULL){
//                    ssdsim_msg("2 deletion in requests, no subrequests' requests wrong");
//                    while(1){
//
//                    }
//
//                }
//            }
//            if(ssd->request_queue_length == 0){
//                if( (ssd->request_queue != NULL) || (ssd->request_tail != NULL) ){
//                    ssdsim_msg("3 deletion in requests, no subrequests' requests wrong");
//                    while(1){
//
//                    }
//
//                }
//
//            }
//
//            if(ssd->request_queue_length != 0){
//                if( (ssd->request_queue == NULL) || (ssd->request_tail == NULL) ){
//                    ssdsim_msg("4 deletion in requests, no subrequests' requests wrong");
//                    while(1){
//
//                    }
//
//                }
//
//            }
//
ssdsim_msg("now stop in test, req %lld", req);
while(1){

}
        }//end of request 1000


        //**************************end of step2********************************//
        //end of the request has response time

        //**************************step3********************************//
        //begin the request has no response time

        else {
            //before the step 3, we should initilize every request's start_time and end_time value.

            ssdsim_msg("enter hassubrequest part now,  current time:%lld\n",ssd->current_time);
            flag = 1;// 1 means already finished.
            start_time = 0;
            end_time = 0;


            sub = req->subs;

//            //            printf("responding time has no value enter else,have to consider the sub request here. ssd->request_queue is 0x%x\n", ssd->request_queue);
//#ifdef DEBUGSUSPEND
//            printf("responding time has no value enter else,have to consider the sub request here.\n");
//            printf("sub begin_time is %lld, complete_time is %lld,sub->next state %d, sub_next_time %lld,  this request's sub, only little one sub, operation[%d] 0 is write,lpn [%d],channel[%d], chip[%d], die[%d], block[%d], page[%d]\n", sub->begin_time, sub->complete_time, sub->next_state, sub->next_state_predict_time,  sub->operation, sub->lpn, sub->location->channel, sub->location->chip, sub->location->die, sub->location->block, sub->location->page);
//#endif
//
//
//            //**************************step3.1********************************//
            //shoule inquire all the subrequests of one request to make sure the completion state.
            while (sub != NULL) {//we should be aware of every sub's next_subs(not next_node) should be NULL at initiation time, no problem in creat_sub_request()function
                if (start_time == 0)
                    start_time = sub->begin_time;
                if (start_time > sub->begin_time)
                    start_time = sub->begin_time;
                if (end_time < sub->complete_time)
                    end_time = sub->complete_time;
                if ((sub->current_state == SR_COMPLETE) || ((sub->next_state == SR_COMPLETE) &&
                            (sub->next_state_predict_time <=
                             ssd->current_time)))    // if any sub-request is not completed, the request is not completed
                {
#ifdef DEBUGSUSPEND
                    printf("sub begin_time is %lld, complete_time is %lld,sub->next state %d, sub_next_time %lld, this request's sub, only little one sub, operation[%d] 0 is write,lpn [%d],channel[%d], chip[%d], die[%d], block[%d], page[%d],here has been finished\n",sub->begin_time, sub->complete_time, sub->next_state, sub->next_state_predict_time, sub->operation, sub->lpn, sub->location->channel, sub->location->chip, sub->location->die, sub->location->block, sub->location->page);
#endif

                    sub = sub->next_subs;
                } else {
                    flag = 0;
                    break;
                }

            }//end of while for subs.


            //**************************step3.2********************************//
            //now begin to do sth calculation.
#ifdef DEBUGSUSPEND
            printf("flag here is %d, *** 1 means finished.0 no\n",flag);
#endif
            if (flag == 1) {
//                //fprintf(ssd->outputfile,"%10I64u %10u %6u %2u %16I64u %16I64u %10I64u\n",req->time,req->lsn, req->size, req->operation, start_time, end_time, end_time-req->time);
//
//#ifdef DEBUGSUSPEND
//                printf("now begin to clear the ssd->req. %16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size,
//                        req->operation, start_time, end_time, end_time - req->time);
//#endif
//
//                fprintf(ssd->outputfile, "%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size,
//                        req->operation, start_time, end_time, end_time - req->time);
//                fflush(ssd->outputfile);
//
//                if (end_time - start_time == 0) {
//                    printf("the response time is 0?? \n");
//                    while(1){
//
//                    }
//                }
//
//                if (req->operation == READ) {
//                    ssd->read_request_count++;
//                    ssd->read_avg = ssd->read_avg + (end_time - req->time);
//                } else {
//                    ssd->write_request_count++;
//                    ssd->write_avg = ssd->write_avg + (end_time - req->time);
//                }
//
//
//                //**************************step3.3********************************//
//                //now begin to delete.
//                // when the first request delete
//                if(req == ssd->request_queue){
//                    //we should know that request queue assigned before deletion.
//                    pre_node =pre_node->next_node;
//                    ssd->request_queue = pre_node;
//
//
//                    free(req->need_distr_flag);
//                    req->need_distr_flag = NULL;
//                    free(req);
//                    ssd->request_queue_length--;
//
//                    req = pre_node;
//                    //here we also need to cut down the request_tail in case of request_tail is not NULL 
//                    if(ssd->request_queue == NULL){
//                        ssd->request_tail = NULL;
//
//                    }
//
//                    continue;
//                }else {
//
//                    if (req == ssd->request_tail){
//
//                        pre_node->next_node = req->next_node;
//                        if(pre_node->next_node != NULL){
//                            ssdsim_msg("1 logic wrong in request tail");
//                            while(1){
//
//                            }
//                        }
//
//                        ssd->request_tail = pre_node;
//
//                        free(req->need_distr_flag);
//                        req->need_distr_flag = NULL;
//                        free(req);
//                        req = NULL;
//                        ssd->request_queue_length--;
//                        //break; // here jump out all the requesting.
//                        continue;
//
//                    }else {
//
//                        pre_node->next_node = req->next_node;
//
//                        free(req->need_distr_flag);
//                        req->need_distr_flag = NULL;
//                        free(req);
//
//                        req = pre_node->next_node;
//                        ssd->request_queue_length--;
//                        continue;
//
//                    }
//                }//end of no begin of the request.
//
//                //**************************step3.4********************************//
//                //now, no deletion happens. just go to next request.delete nothing.

                ssdsim_msg("stop here 2 wrong happens, req %lld", req);
                while(1){

                }
            } else {//除了上述两种情况，只剩下带有sub且没有完成的了。当没有完成，就敢于往后移动。只有删除的时候需要考虑是否是末尾。 // if it is not finished, then go next req.

                //#ifdef DEBUGTRACE_OUT
                //printf("have to wait for furthure finishment and judge another request . subrequest's next_node req \n");
                //#endif
            //    pre_node = req;
                req = req->next_node;

            }

            //**************************step3.5********************************//
            //after every deletion or just like step 3.5 , we should check the list itself

            ssdsim_msg("enter trace_output,step 3.5 check now,  current time:%lld\n",ssd->current_time);
            if(ssd->request_queue == NULL){
                if(ssd->request_tail != NULL){
                    ssd->request_tail != NULL;
                    ssdsim_msg("1 deletion in requests, has subrequests' requests wrong");
                    while(1){

                    }
                }

            }
            if(ssd->request_tail != NULL){
                if(ssd->request_tail->next_node != NULL){
                    ssdsim_msg("2 deletion in requests, has subrequests' requests wrong");
                    while(1){

                    }
                }
            }
            if(ssd->request_queue_length == 0){
                if( (ssd->request_queue != NULL) || (ssd->request_tail != NULL) ){
                    ssdsim_msg("3 deletion in requests, has subrequests' requests wrong");
                    while(1){

                    }

                }

            }

            if(ssd->request_queue_length != 0){
                if( (ssd->request_queue == NULL) || (ssd->request_tail == NULL) ){
                    ssdsim_msg("4 deletion in requests, has subrequests' requests wrong");
                    while(1){

                    }

                }

            }

        }//end of request has sub_requests judegement.

        /////***************************end of step3*******************************///

    }//end of request for while loop

    ssdsim_msg("now go out test_finish_deletion.current_time:%lld\n",ssd->current_time);
}//end of test for delete_finishment.








/**********************************************************************
*trace_output()函数是在每一条请求的所有子请求经过process()函数处理完后，
*打印输出相关的运行结果到outputfile文件中，这里的结果主要是运行的时间
**********************************************************************/


void trace_output(struct ssd_info *ssd) {
    int flag = 1;
    int64_t start_time, end_time;
    struct request *req, *pre_node;
    struct sub_request *sub, *tmp;


    ssdsim_msg("enter trace_output,  current time:%lld\n",ssd->current_time);
    req = ssd->request_queue;
    pre_node = req;
    start_time = 0;
    end_time = 0;

    if (req == NULL){
#ifdef DEBUG
        printf("ssd->request_queue is NULL\n");
#endif
        return;
    }
    //**************************step0********************************//
    // begin the while loop for every req
    while (req != NULL) {//this NULL must be made sure by request_tail->next_node value!!!!
        ssdsim_msg("one entire request begin judging whether finishment here ,req address is %lld",req);

        //**************************step1********************************//
        //every request should withstand the check by logic

        //lxc for compression
        //here must escape from the mappingtable dealing request.
        if((req->total_wait_fetch_mappingtable == 1 )||(req->total_wait_evict == 1) || (req->create_or_notyet_normal_subrequests == 0)){
            if(ssd->request_queue_length == 1){
                return;
            }
            pre_node = req;
            req = req->next_node;
            continue;

        }

        //here, judge the logic again.
        if(req->missing_in_data_buffer_lpn_data->len != 0){
            if(req->response_time != 0){
                ssdsim_msg("wrong in request response time value req operation %d, address is %lld", req->operation, req);
                while(1){

                }

                //if(req->operation == 0){
                //    while(1){

                //    }
                //}
            }
        }



        //**************************end of step1********************************//

        if (req->response_time != 0) {

            //**************************step2********************************//
            // begin judge the request which in response_time is not 0


            ssdsim_msg("xxxxxxxdebugxxxxxx%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size, req->operation, req->begin_time, req->response_time, req->response_time - req->time);
            //**************************step2.1********************************//
            //begin the fprtintf dealing in ex.out   and the calculation of ssd information, such as, write/read_request_count, write/read average, request_queue_length and so on.    


            //            printf("responding time has response_time here. ssd->request_queue is 0x%x\n", ssd->request_queue);
#ifdef DEBUGTRACE_OUT
            printf("only has entire req->response_time one time,dont care req's subrequest. \n");
#endif



#ifdef DEBUGSUSPEND
            printf("%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size, req->operation, req->begin_time, req->response_time, req->response_time - req->time);

#endif
            fprintf(ssd->outputfile, "%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size,
                    req->operation, req->begin_time, req->response_time, req->response_time - req->time);
            fflush(ssd->outputfile);

            if (req->response_time - req->begin_time == 0) {
                printf("the response time is 0?? \n");
                getchar();
                while(1){

                }
            }

            if (req->operation == READ) {
                ssd->read_request_count++;
                ssd->read_avg = ssd->read_avg + (req->response_time - req->time);
            } else {
                ssd->write_request_count++;
                ssd->write_avg = ssd->write_avg + (req->response_time - req->time);
            }



            //**************************step2.2********************************//
            //begin to delete. not only deletion, but also get the right req value to do next while(req != NULL) judgement.

            // when the first request delete
            if(req == ssd->request_queue){

                //dealing pre_node should before the free(req)
                pre_node =pre_node->next_node;
                ssd->request_queue = pre_node;

                free(req->need_distr_flag);
                req->need_distr_flag = NULL;
                free(req);
                ssd->request_queue_length--;


                req = pre_node;
                //here we also need to cut down the request_tail in case of request_tail is not NULL 
                if(ssd->request_queue == NULL){
                    ssd->request_tail = NULL;

                }

                continue;

            }else {



                if (req == ssd->request_tail){

                    pre_node->next_node = req->next_node;
                    if(pre_node->next_node != NULL){
                        ssdsim_msg("1 logic wrong in request tail");
                        while(1){

                        }
                    }
                    ssd->request_tail = pre_node;

                    free(req->need_distr_flag);
                    req->need_distr_flag = NULL;
                    free(req);
                    req = NULL;

                    ssd->request_queue_length--;
                    // break; // here jump out all the requesting.
                    continue; // we believe the operation than break. even break is right.

                }else {


                    pre_node->next_node = req->next_node;

                    free(req->need_distr_flag);
                    req->need_distr_flag = NULL;
                    free(req);

                    req = pre_node->next_node;
                    ssd->request_queue_length--;
                    continue;

                }
            }//end of no begin of the request.
            // end of request 1000 

            //**************************step2.3********************************//
            //after every deletion, we should check the list itself
            if(ssd->request_queue == NULL){
                if(ssd->request_tail != NULL){
                    ssdsim_msg("1 deletion in requests, no subrequests' requests wrong");
                    while(1){

                    }
                }

            }
            if(ssd->request_tail != NULL){
                if(ssd->request_tail->next_node != NULL){
                    ssdsim_msg("2 deletion in requests, no subrequests' requests wrong");
                    while(1){

                    }

                }
            }
            if(ssd->request_queue_length == 0){
                if( (ssd->request_queue != NULL) || (ssd->request_tail != NULL) ){
                    ssdsim_msg("3 deletion in requests, no subrequests' requests wrong");
                    while(1){

                    }

                }

            }

            if(ssd->request_queue_length != 0){
                if( (ssd->request_queue == NULL) || (ssd->request_tail == NULL) ){
                    ssdsim_msg("4 deletion in requests, no subrequests' requests wrong");
                    while(1){

                    }

                }

            }


        }//end of request 1000


        //**************************end of step2********************************//
        //end of the request has response time

        //**************************step3********************************//
        //begin the request has no response time

        else {
            //before the step 3, we should initilize every request's start_time and end_time value.

            ssdsim_msg("enter trace_output,step 3 now,  current time:%lld\n",ssd->current_time);
            flag = 1;// 1 means already finished.
            start_time = 0;
            end_time = 0;


            sub = req->subs;

            //            printf("responding time has no value enter else,have to consider the sub request here. ssd->request_queue is 0x%x\n", ssd->request_queue);
#ifdef DEBUGSUSPEND
            printf("responding time has no value enter else,have to consider the sub request here.\n");
            printf("sub begin_time is %lld, complete_time is %lld,sub->next state %d, sub_next_time %lld,  this request's sub, only little one sub, operation[%d] 0 is write,lpn [%d],channel[%d], chip[%d], die[%d], block[%d], page[%d]\n", sub->begin_time, sub->complete_time, sub->next_state, sub->next_state_predict_time,  sub->operation, sub->lpn, sub->location->channel, sub->location->chip, sub->location->die, sub->location->block, sub->location->page);
#endif


            //**************************step3.1********************************//
            //shoule inquire all the subrequests of one request to make sure the completion state.
            while (sub != NULL) {//we should be aware of every sub's next_subs(not next_node) should be NULL at initiation time, no problem in creat_sub_request()function
                if (start_time == 0)
                    start_time = sub->begin_time;
                if (start_time > sub->begin_time)
                    start_time = sub->begin_time;
                if (end_time < sub->complete_time)
                    end_time = sub->complete_time;
                if ((sub->current_state == SR_COMPLETE) || ((sub->next_state == SR_COMPLETE) &&
                            (sub->next_state_predict_time <=
                             ssd->current_time)))    // if any sub-request is not completed, the request is not completed
                {
#ifdef DEBUGSUSPEND
                    printf("sub begin_time is %lld, complete_time is %lld,sub->next state %d, sub_next_time %lld, this request's sub, only little one sub, operation[%d] 0 is write,lpn [%d],channel[%d], chip[%d], die[%d], block[%d], page[%d],here has been finished\n",sub->begin_time, sub->complete_time, sub->next_state, sub->next_state_predict_time, sub->operation, sub->lpn, sub->location->channel, sub->location->chip, sub->location->die, sub->location->block, sub->location->page);
#endif

                    sub = sub->next_subs;
                } else {
                    flag = 0;
                    break;
                }

            }//end of while for subs.


            //**************************step3.2********************************//
            //now begin to do sth calculation.
#ifdef DEBUGSUSPEND
            printf("flag here is %d, *** 1 means finished.0 no\n",flag);
#endif
            if (flag == 1) {
                //fprintf(ssd->outputfile,"%10I64u %10u %6u %2u %16I64u %16I64u %10I64u\n",req->time,req->lsn, req->size, req->operation, start_time, end_time, end_time-req->time);

                ssdsim_msg("now begin to clear has subrequests' ssd->req. %16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size,
                        req->operation, start_time, end_time, end_time - req->time);

                fprintf(ssd->outputfile, "%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size,
                        req->operation, start_time, end_time, end_time - req->time);
                fflush(ssd->outputfile);

                if (end_time - start_time == 0) {
                    printf("the response time is 0?? \n");
                    while(1){

                    }
                }

                if (req->operation == READ) {
                    ssd->read_request_count++;
                    ssd->read_avg = ssd->read_avg + (end_time - req->time);
                } else {
                    ssd->write_request_count++;
                    ssd->write_avg = ssd->write_avg + (end_time - req->time);
                }


                //**************************step3.3********************************//
                //now begin to delete.
                //this delete function is not only deleting subs, but also dealing the compression mapping table relatived things.
                delete_subs_in_this_req(ssd, req);
                // when the first request delete
                if(req == ssd->request_queue){
                    //we should know that request queue assigned before deletion.
                    pre_node =pre_node->next_node;
                    ssd->request_queue = pre_node;


                    free(req->need_distr_flag);
                    req->need_distr_flag = NULL;
                    free(req);
                    ssd->request_queue_length--;

                    req = pre_node;
                    //here we also need to cut down the request_tail in case of request_tail is not NULL 
                    if(ssd->request_queue == NULL){
                        ssd->request_tail = NULL;

                    }

                    continue;
                }else {

                    if (req == ssd->request_tail){

                        pre_node->next_node = req->next_node;
                        if(pre_node->next_node != NULL){
                            ssdsim_msg("1 logic wrong in request tail");
                            while(1){

                            }
                        }

                        ssd->request_tail = pre_node;

                        free(req->need_distr_flag);
                        req->need_distr_flag = NULL;
                        free(req);
                        req = NULL;
                        ssd->request_queue_length--;
                        //break; // here jump out all the requesting.
                        continue;

                    }else {

                        pre_node->next_node = req->next_node;

                        free(req->need_distr_flag);
                        req->need_distr_flag = NULL;
                        free(req);

                        req = pre_node->next_node;
                        ssd->request_queue_length--;
                        continue;

                    }
                }//end of no begin of the request.

                //**************************step3.4********************************//
                //now, no deletion happens. just go to next request.delete nothing.

            } else {//除了上述两种情况，只剩下带有sub且没有完成的了。当没有完成，就敢于往后移动。只有删除的时候需要考虑是否是末尾。 // if it is not finished, then go next req.

                //#ifdef DEBUGTRACE_OUT
                //printf("have to wait for furthure finishment and judge another request . subrequest's next_node req \n");
                //#endif
                pre_node = req;
                req = req->next_node;

            }

            //**************************step3.5********************************//
            //after every deletion or just like step 3.5 , we should check the list itself

            ssdsim_msg("enter trace_output,step 3.5 check now,  current time:%lld\n",ssd->current_time);
            if(ssd->request_queue == NULL){
                if(ssd->request_tail != NULL){
                    ssd->request_tail != NULL;
                    ssdsim_msg("1 deletion in requests, has subrequests' requests wrong");
                    while(1){

                    }
                }

            }
            if(ssd->request_tail != NULL){
                if(ssd->request_tail->next_node != NULL){
                    ssdsim_msg("2 deletion in requests, has subrequests' requests wrong");
                    while(1){

                    }
                }
            }
            if(ssd->request_queue_length == 0){
                if( (ssd->request_queue != NULL) || (ssd->request_tail != NULL) ){
                    ssdsim_msg("3 deletion in requests, has subrequests' requests wrong");
                    while(1){

                    }

                }

            }

            if(ssd->request_queue_length != 0){
                if( (ssd->request_queue == NULL) || (ssd->request_tail == NULL) ){
                    ssdsim_msg("4 deletion in requests, has subrequests' requests wrong");
                    while(1){

                    }

                }

            }

        }//end of request has sub_requests judegement.

        /////***************************end of step3*******************************///

    }//end of request for while loop
}//end of trace_output.









//void trace_output(struct ssd_info *ssd) {
//    int flag = 1;
//    int64_t start_time, end_time;
//    struct request *req, *pre_node;
//    struct sub_request *sub, *tmp;
//
//#ifdef DEBUG_CURR_TIME 
//    printf("enter trace_output,  current time:%lld\n",ssd->current_time);
//#endif
//
//    req = ssd->request_queue;
//    pre_node = req;
//    start_time = 0;
//    end_time = 0;
//
//    if (req == NULL){
//#ifdef DEBUG
//        printf("ssd->request_queue is NULL\n");
//#endif
//        return;
//    }
//    while (req != NULL) {
////lxc for compression
////here must escape from the mappingtable dealing request.
//        if((req->total_wait_fetch_mappingtable == 1 )||(req->total_wait_evict == 1)){
//            if(ssd->request_queue_length == 1){
//                return;
//            }
//            pre_node = req;
//            req = req->next_node;
//            continue;
//
//        }
//        if(req->missing_in_data_buffer_lpn_data->len != 0){
//            if(req->response_time != 0){
//                ssdsim_msg("wrong in request response time value req operation %d, address is %lld", req->operation, req);
//                if(req->operation == 0){
//                    while(1){
//
//                    }
//                }
//
//            }
//
//        }
//if( (req->create_or_notyet_normal_subrequests == 0) && (req->response_time == 0) ){
//    if(ssd->request_queue_length == 1){
//                return;
//            }
//            pre_node = req;
//            req = req->next_node;
//            continue;
//
//}
//#ifdef DEBUGTRACE_OUT
//            printf("one entire request begin judging whether finishment here ,req address is %lld\n",req);
//#endif
//
//
////    printf("request queue length is %d\n", ssd->request_queue_length);
//
//        flag = 1;
//        start_time = 0;
//        end_time = 0;
//
///////***************************begin the request has response time*******************************///
//        if (req->response_time != 0) {
//
////            printf("responding time has response_time here. ssd->request_queue is 0x%x\n", ssd->request_queue);
//#ifdef DEBUGTRACE_OUT
//            printf("only has entire req->response_time one time,dont care req's subrequest. \n");
//#endif
//
//
//
//#ifdef DEBUGSUSPEND
//            printf("%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size,
//                    req->operation, req->time, req->response_time, req->response_time - req->time);
//
//#endif
//if(req->if_need_movement_between_com_and_uncom_flag_and_page_numbers != 0){
//    traceError_msg("no subrequestssss fprintf in compressionmappingtable");
//                fprintf(ssd->outputfile, "%16lld %10d %6d %2d %16lld %16lld %10lld  compression delay\n", req->time, req->lsn, req->size,
//                        req->operation, req->time , req->response_time, req->response_time - req->time + req->if_need_movement_between_com_and_uncom_flag_and_page_numbers * COMPRESSION_COST_TIME_ONE_PAGE );
//}else{
//            fprintf(ssd->outputfile, "%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size,
//                        req->operation, req->time, req->response_time, req->response_time - req->time);
//
//
//
//}
//
//
//            //fprintf(ssd->outputfile, "%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size, req->operation, req->begin_time, req->response_time, req->response_time - req->time);
//            fflush(ssd->outputfile);
//
//            if (req->response_time - req->begin_time == 0) {
//                printf("the response time is 0?? \n");
//                getchar();
//            }
//
//
//                traceError_msg("no no no hasing no sub_requests's now begin to clear the  ssd->req.address %lld,  %lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size, req->operation, start_time, end_time, end_time - req->time);
//            if (req->operation == READ) {
//                ssd->read_request_count++;
//                ssd->read_avg = ssd->read_avg + (req->response_time - req->time);
//            } else {
//                ssd->write_request_count++;
//                ssd->write_avg = ssd->write_avg + (req->response_time - req->time);
//            }
//            // when the first request delete
//if(req == ssd->request_queue){
//	if(req == ssd->request_tail){
//
//		if(ssd->request_queue_length != 1){
//			ssdsim_msg("wrong in deleting request, nonononono subrequest, ssd->request_queue_length is %d", ssd->request_queue_length);
//			while(1){
//
//			}
//		}
//		ssd->request_queue_length--;
//		delete_request_without_subs(req);
//		req = NULL;	
//		ssd->request_queue = NULL;
//		ssd->request_tail = NULL;
//		continue;
//	}
//
//	//free(req->need_distr_flag);
//	//req->need_distr_flag = NULL;
//	//lxc for compression
//	//free(req);
//	ssd->request_queue_length--;
//
//	ssd->request_queue = pre_node->next_node;
//	pre_node =pre_node->next_node;
//	//please be carful for this delete function position. must be after the new pre_node for next req 
//	delete_request_without_subs(req);
//	req = pre_node;
//
//	continue;
//
//}else {
//                
//                
//                
//                if (req == ssd->request_tail){
//
//                    pre_node->next_node = req->next_node;
//					if(req->next_node != NULL){
//						ssdsim_msg("wrong in deleting request in no-subrequest part ");
//						while(1){
//
//						}
//					}
//                    ssd->request_tail = pre_node;
//
//                    //free(req->need_distr_flag);
//                    //req->need_distr_flag = NULL;
//                    //free(req);
//
//                    //lxc for compression
//                    delete_request_without_subs(req);
//                    req = NULL;
//                    ssd->request_queue_length--;
//                    break; // here jump out all the requesting.
//
//                }else {
//
//
//                    pre_node->next_node = req->next_node;
//                    //free(req->need_distr_flag);
//                    //req->need_distr_flag = NULL;
//                    //free(req);
//                    //lxc for compression
//                    delete_request_without_subs(req);
//
//                    req = pre_node->next_node;
//                    ssd->request_queue_length--;
//                    continue;
//
//                }
//            }//end of no begin of the request.
//        }//end of request 1000
//        // end of request 1000 
//
///////***************************end of the request has response time*******************************///
//
///////***************************begin the request has no response time*******************************///
//        // begin the request has sub_request judgement.
//        else {
//
//            flag = 1;// 1 means already finished.
//            sub = req->subs;
//
////            printf("responding time has no value enter else,have to consider the sub request here. ssd->request_queue is 0x%x\n", ssd->request_queue);
//#ifdef DEBUGSUSPEND
//            printf("responding time has no value enter else,have to consider the sub request here.\n");
//            printf("sub begin_time is %lld, complete_time is %lld,sub->next state %d, sub_next_time %lld,  this request's sub, only little one sub, operation[%d] 0 is write,lpn [%d],channel[%d], chip[%d], die[%d], block[%d], page[%d]\n", sub->begin_time, sub->complete_time, sub->next_state, sub->next_state_predict_time,  sub->operation, sub->lpn, sub->location->channel, sub->location->chip, sub->location->die, sub->location->block, sub->location->page);
//#endif
//
//
//            while (sub != NULL) {
////lxc for compression
//				if(sub->wait_fetch_mappingtable_sign == 1){  
//                    traceError_msg("sub %lld, wait_fetch_mappingtable_sign is 1, so break out in trace_output", sub);
//					flag = 0;
//					break;
//				}
//                if (start_time == 0)
//                   // start_time = sub->begin_time;//but start time is just for the begin service time. it has no effectiveness for average responsetime...
//                   //lxc for compression changed here, since mappingtable related subs has been deleted in other spaces
//                    start_time = req->time;
//                if (start_time > sub->begin_time)
//                    start_time = sub->begin_time;
//                if (end_time < sub->complete_time)
//                    end_time = sub->complete_time;
//                //lxc for compression
//                if ((sub->wait_fetch_mappingtable_sign != 1) && (sub->current_state == SR_COMPLETE) || ((sub->next_state == SR_COMPLETE) &&
//                                                            (sub->next_state_predict_time <=
//                                                             ssd->current_time)))    // if any sub-request is not completed, the request is not completed
//                {
//#ifdef DEBUGSUSPEND
//            printf("sub begin_time is %lld, complete_time is %lld,sub->next state %d, sub_next_time %lld, this request's sub, only little one sub, operation[%d] 0 is write,lpn [%d],channel[%d], chip[%d], die[%d], block[%d], page[%d],here has been finished\n",sub->begin_time, sub->complete_time, sub->next_state, sub->next_state_predict_time, sub->operation, sub->lpn, sub->location->channel, sub->location->chip, sub->location->die, sub->location->block, sub->location->page);
//#endif
//
//                    sub = sub->next_subs;
//                } else {
//                    flag = 0;
//                    break;
//                }
//
//            }
//#ifdef DEBUGSUSPEND
//printf("flag here is %d, *** 1 means finished.0 no\n",flag);
//#endif
//            if (flag == 1) {
//                //fprintf(ssd->outputfile,"%10I64u %10u %6u %2u %16I64u %16I64u %10I64u\n",req->time,req->lsn, req->size, req->operation, start_time, end_time, end_time-req->time);
//
//                traceError_msg("hasing sub_requests's now begin to clear the  ssd->req.address %lld,  %lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size, req->operation, start_time, end_time, end_time - req->time);
//
//if(req->if_need_movement_between_com_and_uncom_flag_and_page_numbers != 0){
//    traceError_msg("has subrequestssss fprintf in compressionmappingtable");
//                fprintf(ssd->outputfile, "%16lld %10d %6d %2d %16lld %16lld %10lld  compression delay\n", req->time, req->lsn, req->size,
//                        req->operation, start_time , end_time, end_time - req->time + req->if_need_movement_between_com_and_uncom_flag_and_page_numbers * COMPRESSION_COST_TIME_ONE_PAGE );
//}else{
//            fprintf(ssd->outputfile, "%16lld %10d %6d %2d %16lld %16lld %10lld\n", req->time, req->lsn, req->size,
//                        req->operation, start_time, end_time, end_time - req->time);
//
//
//
//}
//                fflush(ssd->outputfile);
//
//                if (end_time - start_time == 0) {
//                    printf("the response time is 0?? \n");
//                    while(1){
//
//                    }
//                }
//
//                if (req->operation == READ) {
//                    ssd->read_request_count++;
//					if(req->if_need_movement_between_com_and_uncom_flag_and_page_numbers != 0){
//                        ssdsim_msg("get read_avg with compression overhead once");
//
//                    ssd->read_avg = ssd->write_avg + (end_time - req->time + req->if_need_movement_between_com_and_uncom_flag_and_page_numbers * COMPRESSION_COST_TIME_ONE_PAGE);
//					}else{
//
//                        ssdsim_msg("get read_avg dont consider compression overhead once");
//                    ssd->read_avg = ssd->read_avg + (end_time - req->time);
//					}
//                } else {
//                    ssd->write_request_count++;
//					if(req->if_need_movement_between_com_and_uncom_flag_and_page_numbers != 0){
//
//                        ssdsim_msg("get write_avg with compression overhead once");
//                    ssd->write_avg = ssd->write_avg + (end_time - req->time + req->if_need_movement_between_com_and_uncom_flag_and_page_numbers * COMPRESSION_COST_TIME_ONE_PAGE);
//					}else{
//
//                        ssdsim_msg("get write_avg dont consider compression overhead once");
//                    ssd->write_avg = ssd->read_avg + (end_time - req->time);
//					}
//                }
////lxc for compression original wrong
//                delete_subs_in_this_req(ssd, req);
//      // when the first request delete
//            if(req == ssd->request_queue){
//
//                //free(req->need_distr_flag);
//                //req->need_distr_flag = NULL;
//                //free(req);
//
//                //lxc for compression
//				if(req == ssd->request_tail){
//					if(ssd->request_queue_length != 1){
//						ssdsim_msg("wrong in deleting request, has subrequest, ssd->request_queue_length is %d", ssd->request_queue_length);
//						while(1){
//
//						}
//					}
//					ssd->request_queue_length--;
//					delete_request_without_subs(req);
//					req = NULL;	
//					ssd->request_queue = NULL;
//					ssd->request_tail = NULL;
//
//					continue;
//				}
//
//                ssd->request_queue_length--;
//
//                ssd->request_queue = pre_node->next_node;
//                pre_node =pre_node->next_node;
//          
//
//                delete_request_without_subs(req);
//                req = pre_node;
//
//
//                continue;
//            }else {
//                
//                
//                
//                if (req == ssd->request_tail){
//
//                    pre_node->next_node = req->next_node;
//                    ssd->request_tail = pre_node;
//
//                    //free(req->need_distr_flag);
//                    //req->need_distr_flag = NULL;
//                    //free(req);
//
//                    //lxc for compression
//                    delete_request_without_subs(req);
//
//                    req = NULL;
//                    ssd->request_queue_length--;
//                    break; // here jump out all the requesting.
//
//                }else {
//
//
//                    pre_node->next_node = req->next_node;
//                   // free(req->need_distr_flag);
//                   // req->need_distr_flag = NULL;
//                   // free(req);
//
//                   //lxc for compression
//                    delete_request_without_subs(req);
//
//                    req = pre_node->next_node;
//                    ssd->request_queue_length--;
//                    continue;
//
//                }
//            }//end of no begin of the request.
//
//            } else {//除了上述两种情况，只剩下带有sub且没有完成的了。当没有完成，就敢于往后移动。只有删除的时候需要考虑是否是末尾。 // if it is not finished, then go next req.
//
////#ifdef DEBUGTRACE_OUT
////printf("have to wait for furthure finishment and judge another request . subrequest's next_node req \n");
////#endif
//                pre_node = req;
//                req = req->next_node;
//            }
//        }//end of request has sub_requests judegement.
//
///////***************************end of the request has no response time*******************************///
//
//    }//end of request for loop
//}


/*******************************************************************************
*statistic_output()函数主要是输出处理完一条请求后的相关处理信息。
*1，计算出每个plane的擦除次数即plane_erase和总的擦除次数即erase
*2，打印min_lsn，max_lsn，read_count，program_count等统计信息到文件outputfile中。
*3，打印相同的信息到文件statisticfile中
*******************************************************************************/
void statistic_output(struct ssd_info *ssd) {
    unsigned int lpn_count = 0, i, j, k, l, m, erase = 0, plane_erase = 0, temp_chip_channel = 0;
    double gc_energy = 0.0;
#ifdef DEBUG_CURR_TIME  
    printf("enter statistic_output,  current time:%lld\n",ssd->current_time);
#endif
temp_chip_channel = ssd->parameter->chip_num / ssd->parameter->channel_number;


for (i = 0; i < ssd->parameter->channel_number; i++) {
    for (l = 0; l < temp_chip_channel; l++) {
        for (j = 0; j < ssd->parameter->die_chip; j++) {
            for (k = 0; k < ssd->parameter->plane_die; k++) {
                plane_erase = 0;
                for (m = 0; m < ssd->parameter->block_plane; m++) {
                    if (ssd->channel_head[i].chip_head[l].die_head[j].plane_head[k].blk_head[m].erase_count > 0) {
                        erase = erase +
                            ssd->channel_head[i].chip_head[l].die_head[j].plane_head[k].blk_head[m].erase_count;
                        plane_erase += ssd->channel_head[i].chip_head[l].die_head[j].plane_head[k].blk_head[m].erase_count;
                    }
                }
                fprintf(ssd->outputfile, "the %d channel, %d chip, %d die, %d plane has : %13d erase operations\n", i, l, j, k, plane_erase);
                fprintf(ssd->statisticfile, "the %d channel, %d chip, %d die, %d plane has : %13d erase operations\n",
                        i, l, j, k, plane_erase);
            }
        }
    }
}



flush_mappingtable_into_files(ssd, "map_after_run.txt");

    fclose(ssd->fp_number_subrequests_txt);

    fprintf(ssd->outputfile, "\n");
    fprintf(ssd->outputfile, "\n");
    fprintf(ssd->outputfile, "---------------------------statistic data---------------------------\n");
    fprintf(ssd->outputfile, "gc suspend times: %13d\n", ssd->gc_suspend_times);
    fprintf(ssd->outputfile, "write suspend times: %13d\n", ssd->write_suspend_times);


    fprintf(ssd->outputfile, "min lsn: %13d\n", ssd->min_lsn);
    fprintf(ssd->outputfile, "max lsn: %13d\n", ssd->max_lsn);
    fprintf(ssd->outputfile, "read count: %13d\n", ssd->read_count);
    fprintf(ssd->outputfile, "program count: %13d", ssd->program_count);
    fprintf(ssd->outputfile, "                        include the flash write count leaded by read requests\n");
    fprintf(ssd->outputfile, "the read operation leaded by un-covered update count: %13d\n", ssd->update_read_count);
    fprintf(ssd->outputfile, "erase count: %13d\n", ssd->erase_count);
    fprintf(ssd->outputfile, "direct erase count: %13d\n", ssd->direct_erase_count);
    fprintf(ssd->outputfile, "copy back count: %13d\n", ssd->copy_back_count);
    fprintf(ssd->outputfile, "multi-plane program count: %13d\n", ssd->m_plane_prog_count);
    fprintf(ssd->outputfile, "multi-plane read count: %13d\n", ssd->m_plane_read_count);
    fprintf(ssd->outputfile, "interleave write count: %13d\n", ssd->interleave_count);
    fprintf(ssd->outputfile, "interleave read count: %13d\n", ssd->interleave_read_count);
    fprintf(ssd->outputfile, "interleave two plane and one program count: %13d\n", ssd->inter_mplane_prog_count);
    fprintf(ssd->outputfile, "interleave two plane count: %13d\n", ssd->inter_mplane_count);
    fprintf(ssd->outputfile, "gc copy back count: %13d\n", ssd->gc_copy_back);
    fprintf(ssd->outputfile, "write flash count: %13d\n", ssd->write_flash_count);
    fprintf(ssd->outputfile, "interleave erase count: %13d\n", ssd->interleave_erase_count);
    fprintf(ssd->outputfile, "multiple plane erase count: %13d\n", ssd->mplane_erase_conut);
    fprintf(ssd->outputfile, "interleave multiple plane erase count: %13d\n", ssd->interleave_mplane_erase_count);
    fprintf(ssd->outputfile, "read request count: %13d\n", ssd->read_request_count);
    fprintf(ssd->outputfile, "write request count: %13d\n", ssd->write_request_count);
    fprintf(ssd->outputfile, "read request average size: %13f\n", ssd->ave_read_size);
    fprintf(ssd->outputfile, "write request average size: %13f\n", ssd->ave_write_size);
    fprintf(ssd->outputfile, "read request average response time: %lld\n", ssd->read_avg / ssd->read_request_count);
    fprintf(ssd->outputfile, "write request average response time: %lld\n", ssd->write_avg / ssd->write_request_count);
#ifdef DFTL




#ifdef TRANSLATION_PAGE_UNIT 

#ifdef COMPRESSION_MAPPING 
    fprintf(ssd->outputfile, "translation, compression part buffer_DFTL readrequest hits: %13d\n", ssd->dram->mt->buffer_DFTL->read_hit);

    fprintf(ssd->outputfile, "translation, compression part buffer_DFTL readrequest miss: %13d\n", ssd->dram->mt->buffer_DFTL->read_miss_hit);
    fprintf(ssd->outputfile, "translation, compression part buffer_DFTL writerequest hits: %13d\n", ssd->dram->mt->buffer_DFTL->write_hit);

    fprintf(ssd->outputfile, "translation, compression part buffer_DFTL writerequest miss: %13d\n", ssd->dram->mt->buffer_DFTL->write_miss_hit);

    fprintf(ssd->outputfile, "entry uncompression part,buffer_DFTL readrequest hits: %13d\n", ssd->dram->mt->buffer_DFTL_UNCOM->read_hit);
    fprintf(ssd->outputfile, "entry uncompression part,buffer_DFTL writerequest hits: %13d\n", ssd->dram->mt->buffer_DFTL_UNCOM->write_hit);

    fprintf(ssd->outputfile, "compression method increasing the size during the writting process is in bytes(no gc now)  %13d\n", ssd->changing_size_during_the_runnig_size_in_byte_mostshouldbeadding);

#else//else means normal translation page. 


    fprintf(ssd->outputfile, "now the hitting granualarity is translation pages , so the times will be smaller than entries part\n");

    fprintf(ssd->outputfile, "translation page, buffer_DFTL readrequest hits: %13d\n", ssd->dram->mt->buffer_DFTL->read_hit);
    fprintf(ssd->outputfile, "translation page, buffer_DFTL readrequest miss: %13d\n", ssd->dram->mt->buffer_DFTL->read_miss_hit);
    fprintf(ssd->outputfile, "translation page, buffer_DFTL writerequest hits: %13d\n", ssd->dram->mt->buffer_DFTL->write_hit);
    fprintf(ssd->outputfile, "translation page, buffer_DFTL writerequest miss: %13d\n", ssd->dram->mt->buffer_DFTL->write_miss_hit);

#endif //end of COMPRESSION_MAPPING 

#endif //end of TRANSLATION_PAGE_UNIT 


#ifdef ENTRY_UNIT 


    fprintf(ssd->outputfile, "now the hitting granualarity is entries, so the times will be bigger than translation page\n");

    fprintf(ssd->outputfile, "entry, buffer_DFTL readrequest hits: %13d\n", ssd->dram->mt->buffer_DFTL->read_hit);
    fprintf(ssd->outputfile, "entry, buffer_DFTL readrequest miss: %13d\n", ssd->dram->mt->buffer_DFTL->read_miss_hit);
    fprintf(ssd->outputfile, "entry, buffer_DFTL writerequest hits: %13d\n", ssd->dram->mt->buffer_DFTL->write_hit);
    fprintf(ssd->outputfile, "entry, buffer_DFTL writerequest miss: %13d\n", ssd->dram->mt->buffer_DFTL->write_miss_hit);


#endif //end of ENTRY_UNIT 



#endif// end of DFTL


    fprintf(ssd->outputfile, "total now cached translation page buffer_size in Byte is : %13d\n", ssd->dram->mt->now_cached_dir_slots_sizein_byte);
    fprintf(ssd->outputfile, "total now cached uncompression  buffer_size in Byte is : %13d\n", ssd->dram->mt->uncompression_now_cached_dir_slots_sizein_byte);
    fprintf(ssd->outputfile, "total now cached entry buffer_size in Byte is : %13d\n", ssd->dram->mt->now_cached_entries_sizein_byte);

    fprintf(ssd->outputfile, "data buffer read hits: %13d\n", ssd->dram->buffer->read_hit);
    fprintf(ssd->outputfile, "data buffer read miss: %13d\n", ssd->dram->buffer->read_miss_hit);
    fprintf(ssd->outputfile, "data buffer write hits: %13d\n", ssd->dram->buffer->write_hit);
    fprintf(ssd->outputfile, "data buffer write miss: %13d\n", ssd->dram->buffer->write_miss_hit);
    fprintf(ssd->outputfile, "erase: %13d\n", erase);

    fprintf(ssd->outputfile, "diff between FCFS and RF is %13d\n", ssd->diff_between_FCFS_RF);
    fprintf(ssd->outputfile, "original total pages is %13d\n", ssd->original_total_pages_number);
    fprintf(ssd->outputfile, "only pre_processed pages number is %13d\n", ssd->after_pre_process_pages_number);
    fprintf(ssd->outputfile, "after prep rocessed read part and aged pages , then left the available number is %13d\n", ssd->after_pre_processe_and_aged_pages_number);

    for (i = 0; i < ssd->parameter->channel_number; i++){
        for (l = 0; l < temp_chip_channel; l++) {
            for (j = 0; j < ssd->parameter->die_chip; j++){
                for (k = 0; k < ssd->parameter->plane_die; k++) {
                    fprintf(ssd->outputfile, "channel %d, chip:%d, die:%d, plane:%d have free page: %d\n", i, l, j, k,
                            ssd->channel_head[i].chip_head[l].die_head[j].plane_head[k].free_page);
                    ssd->last_total_available_pages_number +=  ssd->channel_head[i].chip_head[l].die_head[j].plane_head[k].free_page;
                }
            }
        }
    }

 //   fprintf(ssd->outputfile, "only write used pages number is %13d\n",  ssd->after_pre_process_pages_number - ssd->last_total_available_pages_number);
    fprintf(ssd->outputfile, "last_total_available_pages number is %13d\n", ssd->last_total_available_pages_number);






    fflush(ssd->outputfile);

    fclose(ssd->outputfile);


    fprintf(ssd->statisticfile, "\n");
    fprintf(ssd->statisticfile, "\n");
    fprintf(ssd->statisticfile, "---------------------------statistic data---------------------------\n");
    fprintf(ssd->statisticfile, "min lsn: %13d\n", ssd->min_lsn);
    fprintf(ssd->statisticfile, "max lsn: %13d\n", ssd->max_lsn);
    fprintf(ssd->statisticfile, "read count: %13d\n", ssd->read_count);
    fprintf(ssd->statisticfile, "program count: %13d", ssd->program_count);
    fprintf(ssd->statisticfile, "                        include the flash write count leaded by read requests\n");
    fprintf(ssd->statisticfile, "the read operation leaded by un-covered update count: %13d\n", ssd->update_read_count);
    fprintf(ssd->statisticfile, "erase count: %13d\n", ssd->erase_count);
    fprintf(ssd->statisticfile, "direct erase count: %13d\n", ssd->direct_erase_count);
    fprintf(ssd->statisticfile, "copy back count: %13d\n", ssd->copy_back_count);
    fprintf(ssd->statisticfile, "multi-plane program count: %13d\n", ssd->m_plane_prog_count);
    fprintf(ssd->statisticfile, "multi-plane read count: %13d\n", ssd->m_plane_read_count);
    fprintf(ssd->statisticfile, "interleave count: %13d\n", ssd->interleave_count);
    fprintf(ssd->statisticfile, "interleave read count: %13d\n", ssd->interleave_read_count);
    fprintf(ssd->statisticfile, "interleave two plane and one program count: %13d\n", ssd->inter_mplane_prog_count);
    fprintf(ssd->statisticfile, "interleave two plane count: %13d\n", ssd->inter_mplane_count);
    fprintf(ssd->statisticfile, "gc copy back count: %13d\n", ssd->gc_copy_back);
    fprintf(ssd->statisticfile, "write flash count: %13d\n", ssd->write_flash_count);
    fprintf(ssd->statisticfile, "waste page count: %13d\n", ssd->waste_page_count);
    fprintf(ssd->statisticfile, "interleave erase count: %13d\n", ssd->interleave_erase_count);
    fprintf(ssd->statisticfile, "multiple plane erase count: %13d\n", ssd->mplane_erase_conut);
    fprintf(ssd->statisticfile, "interleave multiple plane erase count: %13d\n", ssd->interleave_mplane_erase_count);
    fprintf(ssd->statisticfile, "read request count: %13d\n", ssd->read_request_count);
    fprintf(ssd->statisticfile, "write request count: %13d\n", ssd->write_request_count);
    fprintf(ssd->statisticfile, "read request average size: %13f\n", ssd->ave_read_size);
    fprintf(ssd->statisticfile, "write request average size: %13f\n", ssd->ave_write_size);
    fprintf(ssd->statisticfile, "read request average response time: %lld\n", ssd->read_avg / ssd->read_request_count);
    fprintf(ssd->statisticfile, "write request average response time: %lld\n",
            ssd->write_avg / ssd->write_request_count);
    fprintf(ssd->statisticfile, "buffer read hits: %13d\n", ssd->dram->buffer->read_hit);
    fprintf(ssd->statisticfile, "buffer read miss: %13d\n", ssd->dram->buffer->read_miss_hit);
    fprintf(ssd->statisticfile, "buffer write hits: %13d\n", ssd->dram->buffer->write_hit);
    fprintf(ssd->statisticfile, "buffer write miss: %13d\n", ssd->dram->buffer->write_miss_hit);
    fprintf(ssd->statisticfile, "erase: %13d\n", erase);


    fflush(ssd->statisticfile);

    fclose(ssd->statisticfile);
}


/***********************************************************************************
*根据每一页的状态计算出每一需要处理的子页的数目，也就是一个子请求需要处理的子页的页数
************************************************************************************/
unsigned int size(unsigned int stored) {
    unsigned int i, total = 0, mask = 0x80000000;

#ifdef DEBUG
   printf("enter size\n");
#endif
    for (i = 1; i <= 32; i++) {
        if (stored & mask) total++;
        stored <<= 1;
    }
#ifdef DEBUG
    printf("leave size total lsn number in request : %d\n",total);
#endif
    return total;
}


/*********************************************************
*transfer_size()函数的作用就是计算出子请求的需要处理的size
*函数中单独处理了first_lpn，last_lpn这两个特别情况，因为这
*两种情况下很有可能不是处理一整页而是处理一页的一部分，因
*为lsn有可能不是一页的第一个子页。
*********************************************************/
unsigned int transfer_size(struct ssd_info *ssd, int need_distribute, unsigned int lpn, struct request *req) {
    unsigned int first_lpn, last_lpn, state, trans_size;
    unsigned int mask = 0, offset1 = 0, offset2 = 0;

    first_lpn = req->lsn / ssd->parameter->subpage_page;
    last_lpn = (req->lsn + req->size - 1) / ssd->parameter->subpage_page;

    mask = ~(0xffffffff << (ssd->parameter->subpage_page));
    state = mask;
    if (lpn == first_lpn) {
        offset1 = ssd->parameter->subpage_page - ((lpn + 1) * ssd->parameter->subpage_page - req->lsn);
        state = state & (0xffffffff << offset1);
    }
    if (lpn == last_lpn) {
        offset2 = ssd->parameter->subpage_page - ((lpn + 1) * ssd->parameter->subpage_page - (req->lsn + req->size));
        state = state & (~(0xffffffff << offset2));
    }

    trans_size = size(state & need_distribute);

    return trans_size;
}


/**********************************************************************************************************  
*int64_t find_nearest_event(struct ssd_info *ssd)       
*寻找所有子请求的最早到达的下个状态时间,首先看请求的下一个状态时间，如果请求的下个状态时间小于等于当前时间，
*说明请求被阻塞，需要查看channel或者对应die的下一状态时间。Int64是有符号 64 位整数数据类型，值类型表示值介于
*-2^63 ( -9,223,372,036,854,775,808)到2^63-1(+9,223,372,036,854,775,807 )之间的整数。存储空间占 8 字节。
*channel,die是事件向前推进的关键因素，三种情况可以使事件继续向前推进，channel，die分别回到idle状态，die中的
*读数据准备好了
***********************************************************************************************************/
int64_t find_nearest_event(struct ssd_info *ssd) {
    unsigned int i, j;
    int64_t time = MAX_INT64;
    int64_t time1 = MAX_INT64;
    int64_t time2 = MAX_INT64;
#ifdef DEBUG_CURR_TIME  //lxcv1
    printf("enter find_nearest_event, ssd->current_time is %lld\n",ssd->current_time);

    for (i = 0; i < ssd->parameter->channel_number; i++) {

        printf("channel[%d].next_state is 0x%x,ssd->channel_head[%d].next_state_predict_time is %lld, channel[%d].current_state is 0x%x,ssd->channel_head[%d].current_time is %lld\n",i,ssd->channel_head[i].next_state,i,ssd->channel_head[i].next_state_predict_time  ,i,ssd->channel_head[i].current_state,i,ssd->channel_head[i].current_time  );

        for (j = 0; j < ssd->parameter->chip_channel[i]; j++) {

            printf("ssd->channel_head[%d].chip_head[%d].next_state_predict_time is %lld,  ssd->channel_head[%d].chip_head[%d].next_state is 0x%x  ssd->channel_head[%d].chip_head[%d].current_time is %lld,  ssd->channel_head[%d].chip_head[%d].current_state is 0x%x \n", i,j,ssd->channel_head[i].chip_head[j].next_state_predict_time ,i,j,ssd->channel_head[i].chip_head[j].next_state , i,j,ssd->channel_head[i].chip_head[j].current_time ,i,j,ssd->channel_head[i].chip_head[j].current_state  );

        }

    }

#endif


    for (i = 0; i < ssd->parameter->channel_number; i++) {
        if (ssd->channel_head[i].next_state == CHANNEL_IDLE)
            if (time1 > ssd->channel_head[i].next_state_predict_time)
                if (ssd->channel_head[i].next_state_predict_time > ssd->current_time)
                    time1 = ssd->channel_head[i].next_state_predict_time;
        for (j = 0; j < ssd->parameter->chip_channel[i]; j++) {

            //lxc_programgc orignal wrong
            //    if ((ssd->channel_head[i].chip_head[j].next_state == CHIP_IDLE) ||
            //          (ssd->channel_head[i].chip_head[j].next_state == CHIP_DATA_TRANSFER))
            if (time2 > ssd->channel_head[i].chip_head[j].next_state_predict_time)
                if (ssd->channel_head[i].chip_head[j].next_state_predict_time > ssd->current_time)
                        time2 = ssd->channel_head[i].chip_head[j].next_state_predict_time;
        }
    }

    /*****************************************************************************************************
     *time为所有 A.下一状态为CHANNEL_IDLE且下一状态预计时间大于ssd当前时间的CHANNEL的下一状态预计时间
     *           B.下一状态为CHIP_IDLE且下一状态预计时间大于ssd当前时间的DIE的下一状态预计时间
     *		     C.下一状态为CHIP_DATA_TRANSFER且下一状态预计时间大于ssd当前时间的DIE的下一状态预计时间
     *CHIP_DATA_TRANSFER读准备好状态，数据已从介质传到了register，下一状态是从register传往buffer中的最小值
     *注意可能都没有满足要求的time，这时time返回0x7fffffffffffffff 。
     *****************************************************************************************************/
#ifdef DEBUG //lxcv1
    printf("result time1 is %lld, time2 is %lld\n", time1, time2);

#endif

    time = (time1 > time2) ? time2 : time1;
    return time;
}

/***********************************************
*free_all_node()函数的作用就是释放所有申请的节点
************************************************/
void free_all_node(struct ssd_info *ssd) {
    unsigned int i, j, k, l, n;
    struct buffer_group *pt = NULL;
    struct direct_erase *erase_node = NULL;
    for (i = 0; i < ssd->parameter->channel_number; i++) {
        for (j = 0; j < ssd->parameter->chip_channel[0]; j++) {
            for (k = 0; k < ssd->parameter->die_chip; k++) {
                for (l = 0; l < ssd->parameter->plane_die; l++) {
                    for (n = 0; n < ssd->parameter->block_plane; n++) {
                        free(ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[n].page_head);
                        ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[n].page_head = NULL;
                    }
                    free(ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head);
                    ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head = NULL;
                    while (ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].erase_node != NULL) {
                        erase_node = ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].erase_node;
                        ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].erase_node = erase_node->next_node;
                        free(erase_node);
                        erase_node = NULL;
                    }
                }

                free(ssd->channel_head[i].chip_head[j].die_head[k].plane_head);
                ssd->channel_head[i].chip_head[j].die_head[k].plane_head = NULL;
            }
            free(ssd->channel_head[i].chip_head[j].die_head);
            ssd->channel_head[i].chip_head[j].die_head = NULL;
        }
        free(ssd->channel_head[i].chip_head);
        ssd->channel_head[i].chip_head = NULL;
    }
    free(ssd->channel_head);
    ssd->channel_head = NULL;

    avlTreeDestroy(ssd->dram->buffer);
    ssd->dram->buffer = NULL;
//lxc for compression

    avlTreeDestroy(ssd->dram->mt->buffer_DFTL);
    ssd->dram->mt->buffer_DFTL = NULL;
    avlTreeDestroy(ssd->dram->mt->buffer_DFTL_UNCOM);

    free(ssd->dram->map->map_entry);
    ssd->dram->map->map_entry = NULL;
    free(ssd->dram->map);
    ssd->dram->map = NULL;
    free(ssd->dram);
    ssd->dram = NULL;
    free(ssd->parameter);
    ssd->parameter = NULL;

    free(ssd);
    ssd = NULL;
}


/*****************************************************************************
*make_aged()函数的作用就死模拟真实的用过一段时间的ssd，
*那么这个ssd的相应的参数就要改变，所以这个函数实质上就是对ssd中各个参数的赋值。
******************************************************************************/
struct ssd_info *make_aged(struct ssd_info *ssd) {
    unsigned int i, j, k, l, m, n, ppn;
    int threshould, flag = 0;

    if (ssd->parameter->aged == 1) {
        //threshold表示一个plane中有多少页需要提前置为失效
        threshould = (int) (ssd->parameter->block_plane * ssd->parameter->page_block * ssd->parameter->aged_ratio);
        for (i = 0; i < ssd->parameter->channel_number; i++)
            for (j = 0; j < ssd->parameter->chip_channel[i]; j++)
                for (k = 0; k < ssd->parameter->die_chip; k++)
                    for (l = 0; l < ssd->parameter->plane_die; l++) {
                        flag = 0;
                        for (m = 0; m < ssd->parameter->block_plane; m++) {
                            if (flag >= threshould) {
                                break;
                            }
                            for (n = 0; n < (ssd->parameter->page_block * ssd->parameter->aged_ratio + 1); n++) {
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].page_head[n].valid_state = 0;        //表示某一页失效，同时标记valid和free状态都为0
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].page_head[n].free_state = 0;         //表示某一页失效，同时标记valid和free状态都为0
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].page_head[n].lpn = 0;  //把valid_state free_state lpn都置为0表示页失效，检测的时候三项都检测，单独lpn=0可以是有效页
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].free_page_num--;
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].invalid_page_num++;
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].blk_head[m].last_write_page++;
                                ssd->channel_head[i].chip_head[j].die_head[k].plane_head[l].free_page--;
                                flag++;

                                ppn = find_ppn(ssd, i, j, k, l, m, n);
#ifdef DEBUGONLYGC
                   //             printf("channel %d, chip %d, die %d, plane %d, block %d, page %d\n", i, j, k, l, m, n);
#endif

                            }
                        }
                    }
    } else {
        return ssd;
    }

    return ssd;
}


/*********************************************************************************************
*no_buffer_distribute()函数是处理当ssd没有dram的时候，
*这是读写请求就不必再需要在buffer里面寻找，直接利用creat_sub_request()函数创建子请求，再处理。
*********************************************************************************************/
struct ssd_info *no_buffer_distribute(struct ssd_info *ssd) 
{
    unsigned int lsn, lpn, last_lpn, first_lpn, complete_flag = 0, state;
    unsigned int flag = 0, flag1 = 1, active_region_flag = 0;           //to indicate the lsn is hitted or not
    struct request *req = NULL;
    struct sub_request *sub = NULL, *sub_r = NULL, *update = NULL;
    struct local *loc = NULL;
    struct channel_info *p_ch = NULL;
    unsigned int mask = 0;
    unsigned int offset1 = 0, offset2 = 0;
    unsigned int sub_size = 0;
    unsigned int sub_state = 0;
    ssd->dram->current_time = ssd->current_time;

#ifdef DEBUG_CURR_TIME  //lxcv1
printf("current_time is %lld in no_buffer_distribute function\n", ssd->current_time);
#endif
    req = ssd->request_tail;
    lsn = req->lsn;
    lpn = req->lsn / ssd->parameter->subpage_page;
    last_lpn = (req->lsn + req->size - 1) / ssd->parameter->subpage_page;
    first_lpn = req->lsn / ssd->parameter->subpage_page;

    if (req->operation == READ) {
        while (lpn <= last_lpn) {
            sub_state = (ssd->dram->map->map_entry[lpn].state & 0x7fffffff);
            sub_size = size(sub_state);
            sub = creat_sub_request(ssd, lpn, sub_size, sub_state, req, req->operation);
            lpn++;
        }
    } else if (req->operation == WRITE) {
        while (lpn <= last_lpn) {
            mask = ~(0xffffffff << (ssd->parameter->subpage_page));
            state = mask;
            if (lpn == first_lpn) {
                offset1 = ssd->parameter->subpage_page - ((lpn + 1) * ssd->parameter->subpage_page - req->lsn);
                state = state & (0xffffffff << offset1);
            }
            if (lpn == last_lpn) {
                offset2 = ssd->parameter->subpage_page -
                          ((lpn + 1) * ssd->parameter->subpage_page - (req->lsn + req->size));
                state = state & (~(0xffffffff << offset2));
            }
            sub_size = size(state);

            sub = creat_sub_request(ssd, lpn, sub_size, state, req, req->operation);
            lpn++;
        }
    }

    return ssd;
}


