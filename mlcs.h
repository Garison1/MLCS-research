#ifndef MLCS_H
#define MLCS_H
#include "shrhelp.h"

#ifdef  __cplusplus
extern "C" {
#endif
	EXPORTED_FUNCTION short ComputeMCLSs(unsigned short* seqData, unsigned long* seqLength, unsigned long seqNumber, bool preprocessing, unsigned long subCount);
	EXPORTED_FUNCTION unsigned short* get_subsequences(unsigned long **sseqLengths, unsigned long *sseqNumber);

#ifdef  __cplusplus
}
#endif

#endif