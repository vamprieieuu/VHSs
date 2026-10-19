#pragma once
#ifndef PARAM_UTILS_H
#define PARAM_UTILS_H

#include "AE_Effect.h"
#include "AE_Macros.h"

#ifndef PF_Precision_INTEGER
#define PF_Precision_INTEGER 0
#endif
#ifndef PF_Precision_TENTHS
#define PF_Precision_TENTHS 1
#endif
#ifndef PF_Precision_HUNDREDTHS
#define PF_Precision_HUNDREDTHS 2
#endif
#ifndef PF_Precision_THOUSANDTHS
#define PF_Precision_THOUSANDTHS 3
#endif

#define PF_ADD_SLIDER(NAME, MIN_VAL, MAX_VAL, SLD_MIN, SLD_MAX, DFLT, ID) \
	do { \
		AEFX_CLR_STRUCT(def); \
		def.param_type = PF_Param_SLIDER; \
		PF_STRCPY(def.name, (NAME)); \
		def.u.sd.valid_min = (MIN_VAL); \
		def.u.sd.valid_max = (MAX_VAL); \
		def.u.sd.slider_min = (SLD_MIN); \
		def.u.sd.slider_max = (SLD_MAX); \
		def.u.sd.value = (DFLT); \
		def.u.sd.dephault = (DFLT); \
		def.uu.id = (ID); \
		err = PF_ADD_PARAM(in_data, -1, &def); \
		if (err) return err; \
	} while (0)

#define PF_ADD_FLOAT_SLIDER(NAME, MIN_VAL, MAX_VAL, SLD_MIN, SLD_MAX, DFLT, PREC, ID) \
	do { \
		AEFX_CLR_STRUCT(def); \
		def.param_type = PF_Param_FLOAT_SLIDER; \
		PF_STRCPY(def.name, (NAME)); \
		def.u.fs_d.valid_min = (MIN_VAL); \
		def.u.fs_d.valid_max = (MAX_VAL); \
		def.u.fs_d.slider_min = (SLD_MIN); \
		def.u.fs_d.slider_max = (SLD_MAX); \
		def.u.fs_d.value = (DFLT); \
		def.u.fs_d.dephault = (DFLT); \
		def.u.fs_d.precision = (PREC); \
		def.u.fs_d.display_flags = 0; \
		def.u.fs_d.fs_flags = 0; \
		def.uu.id = (ID); \
		err = PF_ADD_PARAM(in_data, -1, &def); \
		if (err) return err; \
	} while (0)

#define PF_ADD_FLOAT_SLIDERX(NAME, MIN_VAL, MAX_VAL, SLD_MIN, SLD_MAX, DFLT, PREC, DISP, FLAGS, ID) \
	do { \
		AEFX_CLR_STRUCT(def); \
		def.param_type = PF_Param_FLOAT_SLIDER; \
		PF_STRCPY(def.name, (NAME)); \
		def.u.fs_d.valid_min = (MIN_VAL); \
		def.u.fs_d.valid_max = (MAX_VAL); \
		def.u.fs_d.slider_min = (SLD_MIN); \
		def.u.fs_d.slider_max = (SLD_MAX); \
		def.u.fs_d.value = (DFLT); \
		def.u.fs_d.dephault = (DFLT); \
		def.u.fs_d.precision = (PREC); \
		def.u.fs_d.display_flags = (DISP); \
		def.u.fs_d.fs_flags = (FLAGS); \
		def.uu.id = (ID); \
		err = PF_ADD_PARAM(in_data, -1, &def); \
		if (err) return err; \
	} while (0)

#define PF_ADD_CHECKBOX(NAME, STR_ON, DFLT, FLAGS, ID) \
	do { \
		AEFX_CLR_STRUCT(def); \
		def.param_type = PF_Param_CHECKBOX; \
		PF_STRCPY(def.name, (NAME)); \
		def.u.bd.u.nameptr = (STR_ON); \
		def.u.bd.value = (DFLT); \
		def.u.bd.dephault = (DFLT); \
		def.flags = (FLAGS); \
		def.uu.id = (ID); \
		err = PF_ADD_PARAM(in_data, -1, &def); \
		if (err) return err; \
	} while (0)

#define PF_ADD_POPUP(NAME, NUM_CHOICES, DFLT, CHOICES_STR, ID) \
	do { \
		AEFX_CLR_STRUCT(def); \
		def.param_type = PF_Param_POPUP; \
		PF_STRCPY(def.name, (NAME)); \
		def.u.pd.num_choices = (NUM_CHOICES); \
		def.u.pd.value = (DFLT); \
		def.u.pd.dephault = (DFLT); \
		def.u.pd.u.namesptr = (CHOICES_STR); \
		def.uu.id = (ID); \
		err = PF_ADD_PARAM(in_data, -1, &def); \
		if (err) return err; \
	} while (0)

#define PF_ADD_LAYER(NAME, DFLT, ID) \
	do { \
		AEFX_CLR_STRUCT(def); \
		def.param_type = PF_Param_LAYER; \
		PF_STRCPY(def.name, (NAME)); \
		def.u.ld.dephault = (DFLT); \
		def.uu.id = (ID); \
		err = PF_ADD_PARAM(in_data, -1, &def); \
		if (err) return err; \
	} while (0)

#define PF_ADD_TOPIC(NAME, ID) \
	do { \
		AEFX_CLR_STRUCT(def); \
		def.param_type = PF_Param_GROUP_START; \
		PF_STRCPY(def.name, (NAME)); \
		def.flags = PF_ParamFlag_START_COLLAPSED; \
		def.uu.id = (ID); \
		err = PF_ADD_PARAM(in_data, -1, &def); \
		if (err) return err; \
	} while (0)

#define PF_END_TOPIC(ID) \
	do { \
		AEFX_CLR_STRUCT(def); \
		def.param_type = PF_Param_GROUP_END; \
		def.uu.id = (ID); \
		err = PF_ADD_PARAM(in_data, -1, &def); \
		if (err) return err; \
	} while (0)

#endif // PARAM_UTILS_H
