#ifndef EMODES_H
#define EMODES_H

#include "em_cmu.h"

/* Enter energy mode functions */
void em_EM0_Hfxo(void);
void em_EM1_Hfrco(CMU_HFRCOBand_TypeDef band);
void em_EM2_Lfxo(void);
void em_EM3(void);
void em_EM4(void);

#endif // EMODES_H
