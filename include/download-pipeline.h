//
// Created by Gabriele Gaetano Fronzé on 2020-06-03.
//

#ifndef RUCIO_FUSE_POSIX_DOWNLOAD_PIPELINE_H
#define RUCIO_FUSE_POSIX_DOWNLOAD_PIPELINE_H

#include <ELWD_Pipeline.h>
#include <ELWD_Starting_Stage_I.h>
#include <ELWD_Middle_Stage_I.h>
#include <ELWD_Ending_Stage_I.h>
#include <ELWD_Safe_Queue.h>
#include <ELWD_Load_Balanced_Thread_Pool.h>

#include "rucio-download.h"

// This thread picks from the input queue the information of rucio files to be downloaded and calls the wrapper.
// It also implements multiple trials by routing the requests back to the input queue if maximum trials are reached.
struct rucio_downloader : public ELWD_Middle_Stage_I<rucio_download_info, rucio_download_info, DummyT>{
    rucio_downloader(ELWD_Safe_Queue<rucio_download_info>* inputQueue,
                     ELWD_Safe_Queue<rucio_download_info>* outputQueue) :
                     ELWD_Middle_Stage_I(0,inputQueue,outputQueue){}

    rucio_download_info* get_input() final{
      return new rucio_download_info(fInputQ->poll_and_pinch());
    }

    rucio_download_info* process_input(rucio_download_info* input) final{
      fastlog(DEBUG, "Downloading did %s in %s.", input->fdid, input->ftmp_path);
      return rucio_download_wrapper(*input);
    }

    void handle_output(rucio_download_info* output) final{
      if (output->fdownloaded){
        fastlog(DEBUG, "Did %s downloaded in %s!", output->fdid, output->fcache_path);
        fOutputQ->append(*output);
      } else {
        fastlog(ERROR, "Did %s download failed!", output->fdid);
        if(output->freturn_code != MAX_ATTEMPTS){
          fastlog(INFO,"Trying again did %s download in %s.", output->fdid, output->fcache_path);
          fInputQ->append(*output);
        } else {
          fastlog(ERROR, "Did %s maximum download attempts reached. Aborting!", output->fdid);
          fOutputQ->append(*output);
        }
      }
    }
};

// This thread picks from the input queue and does nothing but writing to the terminal!
struct rucio_notifier : public ELWD_Ending_Stage_I<rucio_download_info, DummyT>{
    rucio_notifier(ELWD_Safe_Queue<rucio_download_info>* queue) : ELWD_Ending_Stage_I(0,queue){}

    rucio_download_info* get_input() final{
      return new rucio_download_info(fInputQ->poll_and_pinch());
    }

    DummyT* process_input(rucio_download_info* input) final{
      fastlog((input->fdownloaded)?INFO:ERROR, "%s", input->print());
      //TODO: notify to the GUI the download status...
      return nullptr;
    }
};

struct rucio_pipeline{
    ELWD_Safe_Queue<rucio_download_info> toDownload;
    ELWD_Safe_Queue<rucio_download_info> downloaded;

    ELWD_Thread_I* rucio_downloaders;
    ELWD_Thread_I* rucio_notifiers;

    ELWD_Pipeline pipeline;

    rucio_pipeline(){
      rucio_downloaders = new ELWD_Load_Balanced_Thread_Pool(rucio_downloader(&toDownload, &downloaded), 1);
      rucio_notifiers = new ELWD_Load_Balanced_Thread_Pool(rucio_notifier(&downloaded), 1);

      pipeline | rucio_downloaders | rucio_notifiers;

      pipeline.start();
    }

    ~rucio_pipeline(){
      pipeline.stop();
    }

    void append_new_download(rucio_download_info info){
      toDownload.append(std::move(info));
    }
};

static rucio_pipeline rucio_download_pipeline;

#endif //RUCIO_FUSE_POSIX_DOWNLOAD_PIPELINE_H
