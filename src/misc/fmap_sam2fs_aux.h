#ifndef TMAP_SAM2FS_AUX_H_
#define TMAP_SAM2FS_AUX_H_

#include "../io/tmap_file.h"

// TODO: document
void
tmap_sam2fs_aux_flow_align(tmap_file_t *fp, uint8_t *qseq, int32_t qseq_len, uint8_t *tseq, int32_t tseq_len, uint8_t *flow_order, int8_t strand);

#endif
