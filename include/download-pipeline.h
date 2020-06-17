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
      fastlog(DEBUG, "Downloading did %s in %s.", input->fdid.data(), input->full_cache_path().data());
      return rucio_download_wrapper(*input);
    }

    void handle_output(rucio_download_info* output) final{
      if (output->fdownloaded){
        fastlog(DEBUG, "Did %s downloaded in %s!", output->fdid.data(), output->full_cache_path().data());
        fOutputQ->append(*output);
      } else {
        fastlog(ERROR, "Did %s download failed!", output->fdid.data());
        if(output->freturn_code != MAX_ATTEMPTS){
          fastlog(INFO,"Trying again did %s download in %s.", output->fdid.data(), output->full_cache_path().data());
//          fInputQ->append(*output);
        } else {
          fastlog(ERROR, "Did %s maximum download attempts reached. Aborting!", output->fdid.data());
          fOutputQ->append(*output);
        }
      }
    }
};

// This thread picks from the input queue and does nothing but writing to the terminal!
// This worker will serve well for sending "download completed" notifications
struct rucio_notifier : public ELWD_Ending_Stage_I<rucio_download_info, DummyT>{
    explicit rucio_notifier(ELWD_Safe_Queue<rucio_download_info>* queue) : ELWD_Ending_Stage_I(0,queue){}

    rucio_download_info* get_input() final{
      return new rucio_download_info(fInputQ->poll_and_pinch());
    }

    DummyT* process_input(rucio_download_info* input) final{
      fastlog((input->fdownloaded)?INFO:ERROR, "%s", input->print().data());
      //TODO: notify to the GUI the download status...
      return nullptr;
    }
};

// This is a struct to keep together the pieces for the rucio download pipeline and allow static instantiation.
// It contains two queues ("to do" and "done" jobs) and two thread pools with automated load-based rescaling.
// It also provides a method to append new jobs, being able to act as an opaque function with internal multithreading.
struct rucio_pipeline{
    ELWD_Safe_Queue<rucio_download_info> toDownload;
    ELWD_Safe_Queue<rucio_download_info> downloaded;

    ELWD_Thread_I* rucio_downloaders;
    ELWD_Thread_I* rucio_notifiers;

    ELWD_Pipeline pipeline;

    rucio_pipeline(){
      rucio_downloaders = new ELWD_Load_Balanced_Thread_Pool<typeof(rucio_downloader)>(rucio_downloader(&toDownload, &downloaded), 1);
      rucio_notifiers = new ELWD_Load_Balanced_Thread_Pool<typeof(rucio_notifier)>(rucio_notifier(&downloaded), 1);

      pipeline | rucio_downloaders | rucio_notifiers;

      pipeline.start();
    }

    ~rucio_pipeline(){
      pipeline.stop();
    }

    bool append_new_download(const rucio_download_info& info){
      //TODO: avoid appending multiple times the same DiD
      fastlog(DEBUG, "Download info enqueued with server name %s and settings at %s", info.fserver_name.data(), info.fserver_config->data());
      toDownload.append(info);
      return true;
    }
};

static rucio_pipeline rucio_download_pipeline;

#endif //RUCIO_FUSE_POSIX_DOWNLOAD_PIPELINE_H
