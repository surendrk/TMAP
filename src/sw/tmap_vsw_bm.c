/* Copyright (C) 2010 Ion Torrent Systems, Inc. All Rights Reserved */
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <config.h>
#include "../util/tmap_alloc.h"
#include "../util/tmap_error.h"
#include "../util/tmap_sam_print.h"
#include "../util/tmap_progress.h"
#include "../util/tmap_sort.h"
#include "../util/tmap_definitions.h"
#include "../util/tmap_rand.h"
#include "../seq/tmap_seq.h"
#include "../index/tmap_refseq.h"
#include "../index/tmap_bwt.h"
#include "../index/tmap_sa.h"
#include "../sw/tmap_sw.h"
#include "../sw/tmap_fsw.h"
#include "../sw/tmap_vsw.h"
#include "../map/util/tmap_map_opt.h"
#include "../map/util/tmap_map_util.h"

#ifdef ENABLE_TMAP_DEBUG_FUNCTIONS
void
tmap_vsw_bm_core(int32_t seq_len, int32_t tlen, int32_t n_iter)
{
  int32_t i, j, k;
  tmap_vsw_query_t *vsw_query = NULL;
  tmap_vsw_opt_t *vsw_opt = NULL;
  int32_t softclip_start, softclip_end;


  tmap_map_opt_t *opt = tmap_map_opt_init(TMAP_MAP_ALGO_NONE);

  uint8_t *seq, *target;
  tmap_rand_t *rand = tmap_rand_init(0);

  seq = tmap_malloc(sizeof(uint8_t) * seq_len, "seq");
  target = tmap_malloc(sizeof(uint8_t) * tlen, "target");

  // random sequence
  for(i=0;i<seq_len;i++) {
      seq[i] = (uint8_t)(4*tmap_rand_get(rand));
  }

  softclip_start = 1;
  softclip_end = 1;

  // initialize opt
  vsw_opt = tmap_vsw_opt_init(opt->score_match, opt->pen_mm, opt->pen_gapo, opt->pen_gape, opt->score_thr);

  // init seqs
  vsw_query = tmap_vsw_query_init(seq, seq_len, seq_len, softclip_start, softclip_end, vsw_opt);

  int32_t front = (tlen - seq_len) / 2;
  int32_t end = tlen - seq_len - front;
  for(i=0;i<n_iter;i++) {
      tmap_map_sam_t tmp_sam;
      int32_t overflow, n_best;
      for(j=k=0;j<front;j++,k++) {
          target[k] = (uint8_t)(4*tmap_rand_get(rand));
      }
      for(j=0;j<seq_len;j++,k++) {
          target[k] = seq[j];
      }
      for(j=0;j<end;j++,k++) {
          target[k] = (uint8_t)(4*tmap_rand_get(rand));
      }
      tmap_vsw_sse2(vsw_query, seq, seq_len,
                    target, tlen,
                    softclip_start, softclip_end,
                    vsw_opt, 
                    &tmp_sam.score_fwd, &tmp_sam.score_rev,
                    &tmp_sam.query_start, &tmp_sam.query_end,
                    &tmp_sam.target_start, &tmp_sam.target_end,
                    &overflow, &n_best, opt->score_thr, 0);
  }

  // free memory
  free(target);
  free(seq);
  tmap_vsw_opt_destroy(vsw_opt);
  tmap_vsw_query_destroy(vsw_query);
  tmap_map_opt_destroy(opt);
  tmap_rand_destroy(rand);
}

static int
usage(int32_t seq_len, int32_t tlen, int32_t n_iter)
{
  tmap_file_fprintf(tmap_file_stderr, "\n");
  tmap_file_fprintf(tmap_file_stderr, "Usage: %s vswbm [options]", PACKAGE);
  tmap_file_fprintf(tmap_file_stderr, "\n");
  tmap_file_fprintf(tmap_file_stderr, "Options (required):\n");
  tmap_file_fprintf(tmap_file_stderr, "         -q INT      the query length [%d]\n", seq_len);
  tmap_file_fprintf(tmap_file_stderr, "         -t INT      the target length [%d]\n", tlen);
  tmap_file_fprintf(tmap_file_stderr, "         -n INT      the number of iterations [%d]\n", n_iter);
  tmap_file_fprintf(tmap_file_stderr, "Options (optional):\n");
  tmap_file_fprintf(tmap_file_stderr, "         -h          print this message\n");
  tmap_file_fprintf(tmap_file_stderr, "\n");
  return 1;
}

int
tmap_vswbm_main(int argc, char *argv[])
{

  int32_t seq_len = 150;
  int32_t tlen = 256; 
  int32_t n_iter = 1000;
  int c;

  while((c = getopt(argc, argv, "q:t:n:h")) >= 0) {
      switch(c) {
        case 'q':
          seq_len = atoi(optarg); break;
        case 't':
          tlen = atoi(optarg); break;
        case 'n':
          n_iter = atoi(optarg); break;
        case 'h':
        default:
          return usage(seq_len, tlen, n_iter);
      }
  }
  if(argc != optind) {
      return usage(seq_len, tlen, n_iter);
  }

  tmap_progress_set_verbosity(1);
  tmap_progress_print2("starting benchmark");

  tmap_vsw_bm_core(seq_len, tlen, n_iter);
  
  tmap_progress_print2("ending benchmark");

  return 0;
}
#endif
