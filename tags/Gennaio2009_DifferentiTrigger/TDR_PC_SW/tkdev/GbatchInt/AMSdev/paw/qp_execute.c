#define mipsFortran

/*
 *  qp_execute.c  --
 *	The main entry point for the interpreter 
 *
 *  Original: 23-Nov-1994 16:58
 *
 *  Author:   Maarten Ballintijn <Maarten.Ballintijn@cern.ch>
 *
 *  $Id: qp_execute.c,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
 *
 *  $Log: qp_execute.c,v $
 *  Revision 1.1.1.1  2007/11/13 09:56:12  zuccon
 *  First Import
 *
 *  Revision 1.1  2002/04/23 14:11:38  choutko
 *  *** empty log message ***
 *
 *  Revision 1.78  1999/07/05 15:43:33  couet
 *  - hbook_interface.h in now replaced by hbook.h in CVSCOSRC
 *
 *  Revision 1.77  1999/07/02 12:31:48  couet
 *  - hplot_interface.h is now replaced by hplot.h in CVSCOSRC
 *  - bug fixed in qp_execute.c: the zones were #define mipsFortran
 *    NT/SCAN with option SPIDER
 *
 *  Revision 1.76  1999/07/01 14:18:47  couet
 *  - higz_interface.h in now replaced by higz.h in CVSCOSRC
 *
 *  Revision 1.75  1999/06/30 15:38:23  couet
 *  - comis_interface.h is now replaced by comis.h in CVSCOSRC
 *
 *  Revision 1.74  1999/06/28 15:08:59  couet
 *  - use now cfortran.h in $CVSCOSRC
 *
 *  Revision 1.73  1997/10/23 13:12:00  mclareni
 *  NT mods
 *
 *  Revision 1.72  1997/04/03 12:48:44  couet
 *  - the variable ICHEVT was not correct in comis functions used in NTUPLE
 *     commands.
 *
 *  Revision 1.71  1996/10/17 15:19:56  couet
 *  - missing prototype for qp_exec_evt
 *
 *  Revision 1.70  1996/10/16 16:00:31  couet
 *  - We NEED <math.h> in this file !!!
 *
 *  Revision 1.69  1996/10/14 16:00:03  lecointe
 *  Replaced MAXFLOAT with FLT_MAX as it appears to be more conform to
 *  ANSI-C. "float.h" is now included.
 *
 *  Revision 1.68  1996/09/09 09:11:46  lecointe
 *  Split "qp_execute.c"in two smaller files, for optimization purpose.
 *  "qp_exec_global.h" contains the global variables that were in
 *  "qp_execute.c", and their initialization (if any)
 *
 *  Revision 1.67  1996/08/30 10:04:30  lecointe
 *  Restored Gouraud Shading in NT/PLOT
 *
 *  Revision 1.66  1996/08/27 10:11:27  lecointe
 *  Minor bug corrected in nt/scan. A string was sometimes misallocated.
 *
 *  Revision 1.65  1996/08/21 12:55:35  lecointe
 *  Restore the spider plot in ntuple/scan
 *
 *  Revision 1.64  1996/07/24 13:49:35  dinofm
 *  Just a few optimization in the qp_exe_evt function and stack handling
 *  macros. Warning, this optimizations would affect the 'hand-made'
 *  timing statistics.
 *
 *  Revision 1.63  1996/05/15 13:52:53  maartenb
 *  - Fix a problem with allocating zero length structures.
 *
 *  Revision 1.62  1996/05/12 15:13:52  maartenb
 *  - Replaced the handling of variables. The handling of variables is now
 *  fully dynamic and there is no other limit than the commonblocks to the
 *  number of var's in a query.
 *
 *  Revision 1.61  1996/05/06 09:18:10  maartenb
 *  - Introduce a consistent reporting of array bound errors
 *
 *  - Cleanup dynamic non scalar access.
 *
 *  Revision 1.60  1996/04/30 10:09:08  maartenb
 *  - Implement comis functions which return a logical
 *
 *  Revision 1.59  1996/04/23 18:38:37  maartenb
 *  - Add RCS keywords
 *
 *
 */

#include	<stdlib.h>
#include	<float.h>
#include	<math.h>
#include	<string.h>

#include	"str.h"

#include	<cfortran/cfortran.h>

#define CF_TRUE         C2FLOGICAL(1)
#define CF_FALSE        C2FLOGICAL(0)

#include	"c_pawchn.h"
#include	"c_pawidn.h"
#include	"c_hcbook.h"
#include	"c_hcntpaw.h"
#include	"c_hcunit.h"
#include	"c_pcmatw.h"
#include	"c_pcaddr.h"
#include	"cern_types.h"
#include	<cfortran/comis.h>
#include	"errors.h"
#include	"fcode.h"
#include	"kuip_interface.h"
#include        <cfortran/hplot.h>
#include	<cfortran/hbook.h>
#include	<cfortran/higz.h>
#include	"paw_interface.h"
#include	"qp_command.h"
#include	"qp_dimension.h"
#include	"qp_execute.h"
#include	"qp_hbook_if.h"
#include	"qp_query.h"
#include	"qp_report.h"
#include	"qp_value.h"
#include	"qpflags.h"

#define Extern
#include        "qp_exec_global.h"
#define INIT_STACK_TO_NULL
#include	"qp_exe_stack.h"	/* PRE_INCLUDE */
					/* all definitions for the stack */
#undef INIT_STACK_TO_NULL		/* are kept together in this file */
#include <hpaw/chain.h>
/* extern functions */
extern void qp_exec_evt( QueryExe *, long, long, QPCmd *, int *);
extern Chain * find_chain(char * cname);
typedef		char	tName[MAX_NAME_LEN+1];
tName		*namelist;
int		*offsetlist;
int		*indexlist;


int		nt_load_cnt;
bool		nt_load_from_disk;


static bool
polygon_sel( int cid, Float32 x, Float32 y )
{
	int		i, j, np;
	CutData		v = cut_data[cid]->points;
	Float32		xint;

	np = cut_data[cid]->np;
	j = 0;
	for ( i=0; i < np-1 ; i++ ) {
		if ( v[i][1] == v[i+1][1] )
			continue;
		if ( (y < v[i][1]) && (y < v[i+1][1]) )
			continue;
		if ( (y > v[i][1]) && (y > v[i+1][1]) )
			continue;
		xint = v[i][0] + (y-v[i][1])*
			(v[i+1][0]-v[i][0])/(v[i+1][1]-v[i][1]);
		if ( x < xint )
			j += 1;
	}

	if ( v[i][1] == v[0][1] )
		goto done;
	if ( (y < v[i][1]) && (y < v[0][1]) )
		goto done;
	if ( (y > v[i][1]) && (y > v[0][1]) )
		goto done;
	xint = v[i][0] + (y-v[i][1])* (v[0][0]-v[i][0])/(v[0][1]-v[i][1]);
	if ( x < xint )
		j += 1;
done:

	return j & 1 ? TRUE : FALSE;
}


static void
qp_exec_init(
	QueryExe *	qe,
	long		start_evt,
	long		nevt,
	QPCmd *		cmd,
	int *		errp
) {
	char		ntname[MAX_NAME_LEN+6];
	int		i, ierr, indx, itype, isize, ielem;
	Int32 *		addr;
	int		ibase[1], ioff, nuse;
	int		id = qe->id;
	char *		path = qe->path;
	char *		title;
	int		nbin[2],nwt[2];
	float		low[2],up[2];

	db_stack = qp_flags_get( "stack" ) != 0;
	exe_verbose = qp_flags_get( "trace" ) != 0;
	timing = qp_flags_get( "verbose" ) != 0;

	stat_opc = 0;
	stat_event = 0;
	stat_selected = 0;

	if ( stack == 0 ) {
		STACK_ALLOC;
		if ( *errp != R_NOERR ) {
			return;
		}
	}

	/* cut segments */

	for ( i=0 ; i < qe->ncuts ; i++ ) {
		cut_segment[i] = qe->cuts[i];
		cut_type[i] = qe->cuts_type[i];
		cut_has_shape[i] = qe->cuts_has_shape[i];
		cut_value[i] = malloc( sizeof(Int32) * qe->cuts_size[i] );
		if ( cut_value[i] == 0 )
			qp_abort( "qp_exec_init: Allocation error\n" );
		cut_data[i] = gcut_get_data( qe->cuts_cid[i] );
	}

	/* init ntuple & vectors */

	CWNCount = vvec_entries( qe->vv );
	/* add + 1 to fix problems with old malloc() implementation */
	CWNBlock = (RefCWN *) calloc( CWNCount + 1, sizeof( RefCWN ) );
	qp_assert( MAX_NAME_LEN == 32 ); /* see hbook.h (HGNTBF) */
	namelist = (tName *) calloc( CWNCount + 1, sizeof(tName) );
	offsetlist = (int *) calloc( CWNCount + 1, sizeof(int) );
	indexlist = (int *) calloc( CWNCount + 1, sizeof(int) );
	qp_assert(
		CWNBlock != 0 &&
		namelist != 0 &&
		offsetlist != 0 &&
		indexlist != 0
		);


	nt_load_cnt = 0;
	nt_load_from_disk = FALSE; /* some RWN/CWN var is loaded from disk */

	for ( i=0; i < CWNCount ; i++ ) {

		char		*name, *block;
		Variable	v;

		v = vvec_get( qe->vv, i );
		CWNBlock[i].name = v->name;

		if ( v->var_class == Var_CWN ) {

			name = str_toupper( v->name );
			block = str_toupper( v->block );

			HNTGET( qe->id, name, indx, itype, isize, ielem, ierr );
			if ( ierr != 0 ) {
				sf_report( "qp_exec_init: HNTGET %s failed\n",
				v->name );
				str_del( name );
				str_del( block );
				*errp = R_ALLOC_ERROR;
				return;
			}

			switch ( v->dtyp ) {	/* switch DataType */
			case D_BOOL:
			case D_UINT:
			case D_INT:
			case D_FLOAT:
				addr = (Int32 *) &PAWCR4.rvar4[indx-1];
				break;
			case D_ULONG:
			case D_LONG:
			case D_DOUBLE:
				addr = (Int32 *) &PAWCR8.rvar8[indx-1];
				break;
			case D_STR:
				addr = (Int32 *) &PAWC32.cvar32[indx-1];
				break;
			default:
				qp_assert( 0 );
				break;
			}

			HBALLOC( id, path, name, block, itype, isize, start_evt,
				nevt*ielem, ibase, ioff, nuse );

			if ( qp_flags_get( "cache" ) != 0 ) {
			  printf( "@@@ %s/%d %s:%s(%d) [%d..%d] -> ioff=%d nuse=%d\n",
				  path, id, name, block, ielem, start_evt, start_evt+nevt-1, ioff, nuse );
			}

			if ( ioff == 0 ) {
				/* Var does not fit in the cache */
				CWNBlock[i].p = addr;
				CWNBlock[i].p_cs = 0;
				CWNBlock[i].p_rwn = 0;
				CWNBlock[i].isize = v->dtyp == D_STR ? QP_STR_MAX : isize;
				CWNBlock[i].step = 0;
				CWNBlock[i].varDim = v->varDim;
				CWNBlock[i].var_base = v->var_base;
				CWNBlock[i].var_index = v->var_index;

				/* make sure we reload the index */
				/* this is needed by hbook ...   */
				if ( v->varDim ) {
					int	j, index = v->var_index;
					Variable	vi;

					qp_assert( index < i );

					vi = vvec_get( qe->vv, index );
					for (j=0; j<nt_load_cnt ;j++) {
						if ( strcmp(namelist[j], vi->name) == 0 ) {
							break;
						}
					}
					if ( j == nt_load_cnt ) {
					/* not yet in list */
					/* offset in words from iq !! */
						offsetlist[nt_load_cnt]=
						CWNBlock[index].p -
						PAWC.iq_eqv;
						strncpy(
						namelist[nt_load_cnt],
						vi->name,
						MAX_NAME_LEN );
						namelist[nt_load_cnt][MAX_NAME_LEN] =
							'\0';
						indexlist[nt_load_cnt] =
							index;
						nt_load_cnt += 1;
						nt_load_from_disk = TRUE;
					}
				}
				offsetlist[nt_load_cnt] = 0;
				strncpy(namelist[nt_load_cnt], v->name,
					MAX_NAME_LEN );
				namelist[nt_load_cnt][MAX_NAME_LEN] = '\0';
				indexlist[nt_load_cnt] = i;
				nt_load_cnt += 1;
				nt_load_from_disk = TRUE;
			} else {
				/* Var will be loaded into the cache */
				/* or Var will be used from the cache */
				CWNBlock[i].p = &ibase[ioff];
				CWNBlock[i].p_cs = addr;
				CWNBlock[i].p_rwn = 0;
				CWNBlock[i].step = v->size;
				CWNBlock[i].isize = isize;
				CWNBlock[i].varDim = v->varDim;
				CWNBlock[i].var_base = v->var_base;
				CWNBlock[i].var_index = v->var_index;


				/* Var will be loaded into the cache */
				if ( nuse == 1 ) {

					/* make sure we reload the index */
					/* this is needed by hbook ...   */
					if ( v->varDim ) {
						int	j, index = v->var_index;
						Variable	vi;

						qp_assert( index < i );

						vi = vvec_get( qe->vv, index );
						for (j=0; j<nt_load_cnt ;j++) {
							if ( strcmp(namelist[j], vi->name) == 0 ) {
								break;
							}
						}
						if ( j == nt_load_cnt ) {
						/* not yet in list */
						/* offset in words from iq !! */
							offsetlist[nt_load_cnt]=
							CWNBlock[index].p -
							PAWC.iq_eqv;
							strncpy(
							namelist[nt_load_cnt],
							vi->name,
							MAX_NAME_LEN );
							namelist[nt_load_cnt][MAX_NAME_LEN] =
								'\0';
							indexlist[nt_load_cnt] =
								index;
							nt_load_cnt += 1;
							nt_load_from_disk = TRUE;
						}
					}

					/* offset in words from iq !! */
					offsetlist[nt_load_cnt] =
						CWNBlock[i].p - PAWC.iq_eqv;
					strncpy(namelist[nt_load_cnt], v->name,
						MAX_NAME_LEN );
					namelist[nt_load_cnt][MAX_NAME_LEN] =
						'\0';
					indexlist[nt_load_cnt] = i;
					nt_load_cnt += 1;
					nt_load_from_disk = TRUE;

				}
			}

			str_del( name );
			str_del( block );

		} else if ( v->var_class == Var_RWN ) {

			name = str_toupper( v->name );

			indx = h_rwn_getIndex( qe->id, v->name );
			qp_assert( indx >= 0 );

			HBALLOC( id, path, name, "RWN", 1, 4, start_evt,
				nevt, ibase, ioff, nuse );

if ( qp_flags_get( "cache" ) != 0 ) {
printf( "@@@ %s/%d %s:%s(%d) [%d..%d] -> ioff=%d nuse=%d\n",
path, id, name, "RWN", 1, start_evt, start_evt+nevt-1, ioff, nuse );
}
			if ( ioff == 0 ) {
				/* Var does not fit in the cache */
				CWNBlock[i].p = (Int32 *) &PAWIDN.x[indx];
				CWNBlock[i].p_cs = 0;
				CWNBlock[i].p_rwn = 0;
				CWNBlock[i].step = 0;
				nt_load_from_disk = TRUE;
			} else if ( nuse == 1 ) {
				/* Var will be loaded into the cache */
				CWNBlock[i].p = (Int32 *) &PAWIDN.x[indx];
				CWNBlock[i].p_cs = 0;
				CWNBlock[i].p_rwn = (float *) &ibase[ioff];
				CWNBlock[i].step = 0;
				nt_load_from_disk = TRUE;
			} else {
				/* Var will be used from the cache */
				CWNBlock[i].p = &ibase[ioff];
				CWNBlock[i].p_cs =  (Int32 *) &PAWIDN.x[indx];
				CWNBlock[i].p_rwn = 0;
				CWNBlock[i].step = 1;
			}
			CWNBlock[i].varDim = v->varDim;
			CWNBlock[i].var_base = v->var_base;
			CWNBlock[i].var_index = v->var_index;

			str_del( name );

		} else if ( v->var_class == Var_Vec ) {

			int	llow, lhigh;

			KUVECT(v->name,llow,lhigh);
			if ( llow == 0 ) {
				sf_report( "qp_exec_init: KUVECT %s failed\n",
					v->name );
				*errp = R_ALLOC_ERROR;
				return;
			}

			CWNBlock[i].p = &PAWC.iq_eqv[llow-1];
			CWNBlock[i].p_cs = 0;
			CWNBlock[i].p_rwn = 0;
			CWNBlock[i].step = 0;
			CWNBlock[i].varDim = v->varDim;
			CWNBlock[i].var_base = v->var_base;
			CWNBlock[i].var_index = v->var_index;
		}
	}

if ( qp_flags_get( "cache" ) != 0 ) {
	int	i;

	for( i=0 ; i < nt_load_cnt ; i++ ) {
		printf( "@@@ %s [%d] @ %d\n", namelist[i], indexlist[i],
			offsetlist[i] );
	}
}

	/* init masks */

	for ( i=0 ; i < qe->nmasks ; i++ ) {
		MaskList[i] = qe->mask[i];
	}
	nmasks = qe->nmasks;

	if ( qe->mask_write != -1 ) {
		Mask	*mp = qe->mask[qe->mask_write];
		char	*p, *desc;

		desc = str_new( qe->sel_str );
		p = strrchr( desc, '>' ); qp_assert( p != 0 );
		p -= 1;
		*p = '\0';

		mask_set_desc( mp, qe->mask_ibit, desc );
	}


	/* init operation */

	switch ( cmd->command ) {	/* switch CmdType */
	case CMD_NULL:
		break;
	case CMD_MINMAX: {
		int		n;
		Extreme *	el;

		cmd->u.minmax.n = qe->nexpr;
		cmd->u.minmax.has_events = FALSE;
		el = (Extreme *) calloc( sizeof(Extreme), qe->nexpr );
		qp_assert(el != 0);
		cmd->u.minmax.el = el;

		for ( i=0 ; i < qe->nexpr ; i++ ) {
			
			el[i].dtyp =  qe->expr_type[i];
			switch ( el[i].dtyp ) {	/* switch DataType */
			case D_BOOL:
				el[i].min.bval = TRUE;
				el[i].max.bval = FALSE;
				break;
			case D_UINT:
				el[i].min.uval = UINT32_MAX;
				el[i].max.uval = 0;
				break;
			case D_ULONG:
				el[i].min.luval = UINT64_MAX;
				el[i].max.luval = 0;
				break;
			case D_INT:
				el[i].min.ival = INT32_MAX;
				el[i].max.ival = INT32_MIN;
				break;
			case D_LONG:
				el[i].min.lival = INT64_MAX;
				el[i].max.lival = INT64_MIN;
				break;
			case D_FLOAT:
				el[i].min.fval = FLOAT32_MAX;
				el[i].max.fval = -FLOAT32_MAX;
				break;
			case D_DOUBLE:
				el[i].min.lfval = FLOAT64_MAX;
				el[i].max.lfval = -FLOAT64_MAX;
				break;
			case D_STR:
				el[i].sm = smap_new( MAX_MAP_STRINGS );
				break;

			default:
				qp_abort( "CMD_MINMAX illegal datatype %d\n",
					qe->expr_type[i] );
				break;
			}
		}
		break;
	}

	case CMD_MINMAXAVG: {
		int		n;
		ExtremeAvg *	el;

		cmd->u.minmaxavg.n = qe->nexpr;
		cmd->u.minmaxavg.has_events = FALSE;
		cmd->u.minmaxavg.evt_count = 0;
		el = (ExtremeAvg *) calloc( sizeof(ExtremeAvg), qe->nexpr );
		qp_assert(el != 0);
		cmd->u.minmaxavg.el = el;

		for ( i=0 ; i < qe->nexpr ; i++ ) {
			
			el[i].dtyp =  qe->expr_type[i];
			el[i].avg  =  0.0;
			switch ( el[i].dtyp ) {	/* switch DataType */
			case D_BOOL:
				el[i].min.bval = TRUE;
				el[i].max.bval = FALSE;
				break;
			case D_UINT:
				el[i].min.uval = UINT32_MAX;
				el[i].max.uval = 0;
				break;
			case D_ULONG:
				el[i].min.luval = UINT64_MAX;
				el[i].max.luval = 0;
				break;
			case D_INT:
				el[i].min.ival = INT32_MAX;
				el[i].max.ival = INT32_MIN;
				break;
			case D_LONG:
				el[i].min.lival = INT64_MAX;
				el[i].max.lival = INT64_MIN;
				break;
			case D_FLOAT:
				el[i].min.fval = FLOAT32_MAX;
				el[i].max.fval = -FLOAT32_MAX;
				break;
			case D_DOUBLE:
				el[i].min.lfval = FLOAT64_MAX;
				el[i].max.lfval = -FLOAT64_MAX;
				break;
			case D_STR:
				el[i].sm = smap_new( MAX_MAP_STRINGS );
				break;

			default:
				qp_abort( "CMD_MINMAXAVG illegal datatype %d\n",
					qe->expr_type[i] );
				break;
			}
		}
		break;
	}

	case CMD_VFILL:
		break;

	case CMD_HFILL1: {
		QPCmdHFill1	*h;
		
		h = &cmd->u.hfill1;

		h->n = 0;

		h->step_x = 4 * datatype_size[qe->expr_type[0]];
		h->x = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->x );

		h->w = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->w );

		break;
	}

	case CMD_HFILL2: {
		QPCmdHFill2	*h;
		
		h = &cmd->u.hfill2;
		h->step_x = 4 * datatype_size[qe->expr_type[1]];
		h->step_y = 4 * datatype_size[qe->expr_type[0]];

		break;
	}

	case CMD_SCAT2: {
		QPCmdScat2	*h;
		
		h = &cmd->u.scat2;

		h->n = 0;

		h->ievt = (int*) calloc( sizeof(int), HFILL_BSIZE );
		qp_assert( h->ievt );

		h->step_x = 4 * datatype_size[qe->expr_type[1]];
		h->x = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->x );

		h->step_y = 4 * datatype_size[qe->expr_type[0]];
		h->y = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->y );

		break;
	}

	case CMD_SCAT3: {
		QPCmdScat3	*h;
		
		h = &cmd->u.scat3;

		h->n = 0;

		h->ievt = (int*) calloc( sizeof(int), HFILL_BSIZE );
		qp_assert( h->ievt );

		h->cvt_x = datatype_to_cvtcallback( qe->expr_type[2],
							(void *) 0 );
		h->step_x = 4 * datatype_size[qe->expr_type[2]];
		h->x = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->x );

		h->cvt_y = datatype_to_cvtcallback( qe->expr_type[1],
							(void *) 0 );
		h->step_y = 4 * datatype_size[qe->expr_type[1]];
		h->y = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->y );

		h->cvt_z = datatype_to_cvtcallback( qe->expr_type[0],
							(void *) 0 );
		h->step_z = 4 * datatype_size[qe->expr_type[0]];
		h->z = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->z );

		break;
	}

	case CMD_GOURAUD: {
		QPCmdGouraud	*h;
		
		h = &cmd->u.gouraud;

		GETNBINS(h->nx, h->ny, h->nz);

		h->cvt_x = datatype_to_cvtcallback( qe->expr_type[2],
							(void *) 0 );
		h->step_x = 4 * datatype_size[qe->expr_type[2]];
		h->x = (float*) calloc( sizeof(float), h->nx );
		qp_assert( h->x );

		h->cvt_y = datatype_to_cvtcallback( qe->expr_type[1],
							(void *) 0 );
		h->step_y = 4 * datatype_size[qe->expr_type[1]];
		h->y = (float*) calloc( sizeof(float), h->ny );
		qp_assert( h->y );

		h->cvt_z = datatype_to_cvtcallback( qe->expr_type[0],
							(void *) 0 );
		h->step_z = 4 * datatype_size[qe->expr_type[0]];
		h->z = (float*) calloc( sizeof(float), h->nz );
		qp_assert( h->z );

		h->value = (float*) calloc( sizeof(float), (h->nx)*(h->ny)*(h->nz) );
		qp_assert( h->value );

		/* Compute the X,Y,Z coordinates */

		h->dx = ((h->max[0])-(h->min[0])) / h->nx;
		h->dy = ((h->max[1])-(h->min[1])) / h->ny;
		h->dz = ((h->max[2])-(h->min[2])) / h->nz;
		
		h->x[0] = h->min[0] + (h->dx/2);
		h->y[0] = h->min[1] + (h->dy/2);
		h->z[0] = h->min[2] + (h->dz/2);
		
		for (i=1; i<h->nx; i++)
		  h->x[i] = h->x[i-1] + h->dx;
		for (i=1; i<h->ny; i++)
		  h->y[i] = h->y[i-1] + h->dy;
		for (i=1; i<h->nz; i++)
		  h->z[i] = h->z[i-1] + h->dz;

		break;
	}

	case CMD_SCAT4: {
		QPCmdScat4	*h;
		
		h = &cmd->u.scat4;

		h->n = 0;

		h->ievt = (int*) calloc( sizeof(int), HFILL_BSIZE );
		qp_assert( h->ievt );

		h->cvt_c = datatype_to_cvtcallback( qe->expr_type[3],
							(void *) 0 );
		h->step_c = 4 * datatype_size[qe->expr_type[3]];
		h->c = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->c );

		h->cvt_x = datatype_to_cvtcallback( qe->expr_type[2],
							(void *) 0 );
		h->step_x = 4 * datatype_size[qe->expr_type[2]];
		h->x = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->x );

		h->cvt_y = datatype_to_cvtcallback( qe->expr_type[1],
							(void *) 0 );
		h->step_y = 4 * datatype_size[qe->expr_type[1]];
		h->y = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->y );

		h->cvt_z = datatype_to_cvtcallback( qe->expr_type[0],
							(void *) 0 );
		h->step_z = 4 * datatype_size[qe->expr_type[0]];
		h->z = (float*) calloc( sizeof(float), HFILL_BSIZE );
		qp_assert( h->z );

		break;
	}

	case CMD_LOOP:
		break;

	case CMD_DUMP:
		{
			int	i;

			for ( i=0 ; i < qe->nexpr ; i++ ) {
				cmd->u.dump.f[i] = datatype_to_dumpfun(
							qe->expr_type[i]);
			}
		}
		break;

	case CMD_SCAN:
		{
			int		i;
			QPCmdScan	*c = &cmd->u.scan;

			for ( i=0 ; i < qe->nexpr ; i++ ) {
				c->f[i] = datatype_to_printfun(
							qe->expr_type[i]);
			}

			c->evt_count = 0;

			if ( cmd->u.scan.pawpp ) {
				char	buf[33];

				c->matlab = (TableCallBack) PCADDR.jmlab;
				c->matrow = (TableCallBack) PCADDR.jmrow;

				qp_assert( qe->nexpr <= MAX_OUTSTR );

				for ( i=0 ; i < qe->nexpr ; i++ ) {
					strncpy( buf, qe->expr_str[i], QP_STR_MAX );
					buf[QP_STR_MAX] = '\0';
					C2FCBSTR(buf,PCMATW.outstr[i],0);
				}

				/* How to do this in CFortran ? */
				(*c->matlab)(  &qe->nexpr );
			} else {
				String	sep, title;
				int	i, n, cl, lt, lf, l;
				char	part[128];
				char	tit[128];

				sep =   str_new( "+-------+" );
				title = str_new( "| Event |" );

				n = qe->nexpr;
				for ( i=0 ; i < n ; i++ ) {
					c->fw[i] = strlen(qe->expr_str[i]) + 5;
					lf = datatype_width[qe->expr_type[i]]+2;
					if ( lf > c->fw[i]  ) c->fw[i] = lf;
					if ( c->fw[i] > 127 ) c->fw[i] = 127;

					memset( part, '-', 127 );
					part[c->fw[i]] = '+';
					part[c->fw[i]+1] = '\0';
					sep = str_merge(sep, str_new(part),
						(char *) 0 );
					cl = c->fw[i] - 5;
					sprintf( tit, "   %-*.*s  |", cl, cl,
						qe->expr_str[i] );
					title = str_merge(title, str_new(tit),
						(char *) 0 );
				}

				cmd->u.scan.sep = sep;
				cmd->u.scan.title = title;
				l = strlen(sep)+1;
				/* allocate buffer of the right size */
				cmd->u.scan.buf = str_alloc( l > 80 ? l : 80 );
				l = ((l > 80 ? l : 80));

				if ( HCUNIT.lout != 6 ) {
					cmd->u.scan.no_prompt = TRUE; /* scan to file */
				}
			}
		}
		break;

	case CMD_SPIDERSCAN:
		{
			int		i,len_max, zonex, zoney;
			QPCmdSpiderScan	*c = &cmd->u.sp_scan;
			float           tmp_val[6];

			len_max=0;
			for ( i=0 ; i < qe->nexpr ; i++ ) {
				c->cvt[i] = datatype_to_cvtcallback(
							qe->expr_type[i], (void *) 0);
				c->t_code[i] = datatype_to_typecode(
							qe->expr_type[i]);
				if (strlen(qe->expr_str[i])>len_max) 
				  len_max=strlen(qe->expr_str[i]);
			}
			len_max++;
			c->expr_str = (char*) malloc(len_max*qe->nexpr*sizeof(char*));

			strcpy(c->expr_str,qe->expr_str[0]);
			strcat(c->expr_str," ");
			for ( i=1 ; i < qe->nexpr ; i++ ) {
			  strcat(c->expr_str,qe->expr_str[i]);
			  strcat(c->expr_str," ");
			}


			c->evt_count = 0;
			c->cur_line = 0;
		
			/* Get the number of drawing zones*/

			HPLGZO(zonex,zoney);
			c->max_line = zonex*zoney;
			c->rzone = (float) (zonex >= zoney ? zonex : zoney);

			/* Get graphic parameters to save */
			HPLSET ("?XWIN", (c->saved[0]));
			HPLSET ("?YWIN", (c->saved[1]));
			HPLSET ("?XMGL", (c->saved[2]));
			HPLSET ("?XMGR", (c->saved[3]));
			HPLSET ("?YMGL", (c->saved[4]));
			HPLSET ("?YMGU", (c->saved[5]));
                        c->saved[6] = zonex;
                        c->saved[7] = zoney;

			/* and set your own */
			tmp_val[0] = 1e-10;
			tmp_val[1] = 1e-10;
			tmp_val[2] = 0.2;
			tmp_val[3] = 0.2;
			tmp_val[4] = 0.2;
			tmp_val[5] = 0.2;

			HPLSET ("XWIN", tmp_val[0]);
			HPLSET ("YWIN", tmp_val[1]);
			HPLSET ("XMGL", tmp_val[2]);
			HPLSET ("XMGR", tmp_val[3]);
			HPLSET ("YMGL", tmp_val[4]);
			HPLSET ("YMGU", tmp_val[5]);
			
			/* set zones once again -> the size of each zone will be recomputed*/
		        HPLZON (zonex, zoney, 1, " ");

		}
		break;

	default:
		qp_abort( "Internal error, unknown Cmd_Type %d", cmd->command );
		break;
	}
}


static void
reinit_cwn( char * member, int id, int * errp )
{
	int	i, indx, itype, isize, ielem, ierr;
	char	*name;

	ierr = 0;
	for ( i=0 ; i < nt_load_cnt ; i++ ) {
		name = str_toupper( namelist[i] );

		/* a side-effect of HNTGET is setting the address of 'name' */
		HNTGET( id, name, indx, itype, isize, ielem, ierr );

		if ( ierr != 0 ) {
			sf_report( "reinit_cwn: Variable %s not found in %s\n",
				name, member );
			*errp = R_NAME_UNDEF;
			break;
		}

		if ( offsetlist[i] != 0 ) {
			/* update cache offsets ... */
			offsetlist[i] = CWNBlock[indexlist[i]].p - PAWC.iq_eqv;
		}

		str_del( name );
	}
}


/*
 *  The instruction format: See intruction.txt
 *  
 */

Category	qp_fc_to_cat[] = {
	CAT_OP, CAT_OP, CAT_OP, CAT_OP, CAT_OP, CAT_OP, CAT_OP, CAT_OP, 
	CAT_OP, CAT_OP, CAT_OP, CAT_OP, CAT_OP, CAT_OP, CAT_OP, CAT_OP, 

	CAT_CONV, CAT_CONV, CAT_CONV, CAT_CONV, 

	CAT_CONST,

	CAT_MATH_SINGLE,

	CAT_MATH_DOUBLE,

	CAT_VAR,

	CAT_LITERAL,

	CAT_BITOP,

	CAT_COMIS,

	CAT_DYN,

	CAT_MASK,
	
	CAT_NONE, CAT_NONE,

	CAT_FLOW,
};



static bool
exec_dyn( Segment *tmp_pc, UInt32 opc, long ievt, int *errp )
{
	register Segment	pc = *tmp_pc;
	bool			running = TRUE;
	RefCWN			*np;
	int			index;
	bool			info_flag;

	index = *pc++;
	qp_assert( 0<=index && index<CWNCount );
	np = &CWNBlock[index];
	info_flag = (opc & FC_DYNAMIC_INFO_BIT) != 0;

	switch ( opc & MASK_FC ) {

#include	"qp_exe_dyn.h"	/* PRE_INCLUDE */

	default:
		sf_report( "exec_dyn: Unkown Fcode ( %d )\n", opc & MASK_FC );
		*errp = R_INTERNAL_ERROR;
		running = FALSE;
		break;
	}

	/* only one point of return ! */
	*tmp_pc = pc;
	return running;
}


static void
exec_mask( Segment *tmp_pc, FCode fc, long ievt, int *errp )
{
	register Segment	pc = *tmp_pc;
	int			idx, ibit;
	UInt32			v;
	Mask			*mp;


	idx = *pc++;
	mp = MaskList[idx];

	if ( fc == FC_MASK_SET_BIT ) {
		bool	v;

		qp_assert( (frame_size[1] == 1)
			&& (frame_type[1] == D_FLOAT) );

		ibit = *pc++;
		v = *((Float32 *) &stack[frame[1]]) != 0.;
		mask_set_bit( mp, ievt, ibit, v, errp );

	} else {
		v = mask_get_word( mp, ievt, errp );
		if ( fc == FC_MASK_GET_BIT ) {
			bool		*bp;

			ibit = *pc++;
			NEW_FRAME( D_BOOL, 1, bp );
			*bp = (v & (1 << (ibit-1)) ) == 0 ?
				FALSE : TRUE ;
		} else {
			UInt32		*up;

			NEW_FRAME( D_UINT, 1, up );
			*up = v;
		}
	}

	*tmp_pc = pc;
}


static bool
exec_const( FCode fc, int *errp )
{
	register void		*r;
	bool			running = TRUE;

	switch ( fc ) {

#include	"qp_exe_fun_const.h"	/* PRE_INCLUDE */

	case FC_SUBSTR:
	{
		Int32		*o1, *r;
		char		tmp1[QP_STR_MAX];
		Int32		*o2, *o3, tmp2, tmp3;
		int		i, n = 1, d;
		int		inc_o1, inc_o2, inc_o3, inc_r;

		if ( frame_size[framep] != 1 ) {
			o1 = &stack[frame[framep]];
			inc_o1 = datatype_size[D_STR];
			n = frame_size[framep];
		} else {
			memcpy( tmp1, (char *) &stack[frame[framep]], QP_STR_MAX );
			o1 = (Int32 *) tmp1;
			inc_o1 = 0;
		}
		if ( frame_size[framep-1] != 1 ) {
			o2 = &stack[frame[framep-1]];
			inc_o2 = 1;
			qp_assert( n==1 || n==frame_size[framep-1] );
			n = frame_size[framep-1];
		} else {
			tmp2 = stack[frame[framep-1]];
			o2 = (Int32 *) &tmp2;
			inc_o2 = 0;
		}
		if ( frame_size[framep-2] != 1 ) {
			o3 = &stack[frame[framep-2]];
			inc_o3 = 1;
			qp_assert( n==1 || n==frame_size[framep-2] );
			n = frame_size[framep-2];
		} else {
			tmp3 = stack[frame[framep-2]];
			o3 = (Int32 *) &tmp3;
			inc_o3 = 0;
		}

		NEW_FRAME(D_STR,n,r);
		inc_r = datatype_size[D_STR];

		o1 += inc_o1 * (n-1);
		o2 += inc_o2 * (n-1);
		o3 += inc_o3 * (n-1);
		r += inc_r * (n-1);
		for ( i=0 ; i < n ; i++ ) {
			int	start, len;

			start = *o2;
			len = *o3;
			if ( start < 1 || start > QP_STR_MAX ) {
				sf_report( "SUBSTR: start out of range (%d)\n", start );
				*errp = R_MATH_ERROR;
				break;
			} else if ( len < 1 || start+len > QP_STR_MAX ) {
				sf_report( "SUBSTR: len out of range (%d)\n", start );
				*errp = R_MATH_ERROR;
				break;
			} else {
				memset( r, ' ', QP_STR_MAX );
				memcpy( (char *)r, ((char *)o1)+start-1, len );
			}

			o1 -= inc_o1;
			o2 -= inc_o2;
			o3 -= inc_o3;
			r -= inc_r;
		}

		SQUEEZE_FRAME( 3 );
		break;
	}
	case FC_MATCH:
	{
		Int32		*o1, *o2;
		char		tmp1[QP_STR_MAX], tmp2[QP_STR_MAX];
		int		i, n = 1, d, ilen, result;
		int		inc_o1, inc_o2;
		bool		*r;
		char		c1[33], c2[33];

		if ( frame_size[framep] != 1 ) {
			o1 = &stack[frame[framep]];
			inc_o1 = datatype_size[D_STR];
			n = frame_size[framep];
		} else {
			memcpy( tmp1, (char *) &stack[frame[framep]], QP_STR_MAX );
			o1 = (Int32 *) tmp1;
			inc_o1 = 0;
		}
		if ( frame_size[framep-1] != 1 ) {
			o2 = &stack[frame[framep-1]];
			inc_o2 = datatype_size[D_STR];
			qp_assert( n==1 || n==frame_size[framep-1] );
			n = frame_size[framep-1];
		} else {
			memcpy( tmp2, (char *) &stack[frame[framep-1]], QP_STR_MAX );
			o2 = (Int32 *) tmp2;
			inc_o2 = 0;
		}
		POP_FRAME(2);

		NEW_FRAME(D_BOOL,n,r);

		o1 += inc_o1 * (n-1);
		o2 += inc_o2 * (n-1);
		r += (n-1);
		c1[QP_STR_MAX] = '\0'; c2[QP_STR_MAX] = '\0';
		for ( i=0 ; i < n ; i++ ) {
			char	*p;

			memcpy( c1, o1, QP_STR_MAX );
			memcpy( c2, o2, QP_STR_MAX );

			for ( p=strchr(c1,'\0')-1; p>c1 && *p==' '; p-- );
			PAWILD( c1, c2, p - c1 + 1, result );
			if ( result == 1 ) {
				*r = TRUE;
			} else {
				*r = FALSE;
			}

			o1 -= inc_o1;
			o2 -= inc_o2;
			r -= 1;
		}
		break;
	}
	default:
		sf_report( "qp_exe_fun_const.h: Unkown Fcode ( %d )\n", fc );
		*errp = R_INTERNAL_ERROR;
		running = FALSE;
		break;
	}

	return running;
}


static bool
exec_math_single( FCode fc, int *errp )
{
	register Float32	*r, *o1, *o2;
	register int		n;
	Float32			o1_tmp, o2_tmp;
	int			o1_inc, o2_inc;
	bool			running = TRUE;

	switch ( fc ) {

#include	"qp_exe_fun_single_math.h"	/* PRE_INCLUDE */

	default:
		sf_report( "qp_exe_fun_single_math.h: Unkown Fcode ( %d )\n", fc );
		*errp = R_INTERNAL_ERROR;
		running = FALSE;
		break;
	}

	return running;
}


static bool
exec_math_double( FCode fc, int *errp )
{
	register Float64	*r, *o1, *o2;
	register int		n;
	Float64			o1_tmp, o2_tmp;
	int			o1_inc, o2_inc;
	bool			running = TRUE;


	switch ( fc ) {

#include	"qp_exe_fun_double_math.h"	/* PRE_INCLUDE */

	default:
		sf_report( "qp_exe_fun_double_math.h: Unkown Fcode ( %d )\n", fc );
		*errp = R_INTERNAL_ERROR;
		running = FALSE;
		break;
	}

	return running;
}


/*
 *	Implement the BITPAK bit handling routines
 */

static bool
exec_bitop( FCode fc, int *errp )
{
	bool			running = TRUE;

	switch ( fc ) {

#define	OP_TYPE		UInt32
#define	OP_DTYPE	D_UINT
#define	OP_BASE		( 0 )

#include	"qp_exe_fun_bitop_templ.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE

#define	OP_TYPE		UInt64
#define	OP_DTYPE	D_ULONG
#define	OP_BASE		( 16 )

#include	"qp_exe_fun_bitop_templ.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE

#define	OP_TYPE		Int32
#define	OP_DTYPE	D_INT
#define	OP_BASE		( 32 )

#include	"qp_exe_fun_bitop_templ.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE

#define	OP_TYPE		Int64
#define	OP_DTYPE	D_LONG
#define	OP_BASE		( 48 )

#include	"qp_exe_fun_bitop_templ.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE

	default:
		sf_report( "exec_bitop: Unkown Fcode ( %d )\n", fc );
		*errp = R_INTERNAL_ERROR;
		running = FALSE;
		break;
	}

	return running;
}


static bool
exec_conv( FCode fc, int *errp )
{
	bool			running = TRUE;
	register Int32		*r, *o1;

	switch ( fc ) {

#include	"qp_exe_conv_case.h"	/* PRE_INCLUDE */


	default:
		sf_report( "qp_exe_conv.h: Unkown Fcode ( %d )\n", fc );
		*errp = R_INTERNAL_ERROR;
		running = FALSE;
		break;
	}

	return running;
}

#if defined(_WIN32) || defined(WIN32) || defined(CERNLIB_WINNT)
#pragma optimize( "", off )
#endif
static bool
exec_op( FCode fc, UInt32 opc, int *errp )
{
	Int32		*r, *o1, *o2, *o3;
	bool			running = TRUE;
	DataType	fctype;

	fctype = ( fc >> 6 ) & 0xf;

	if ( (opc & FC_VECTOR_BIT) != 0 ) {
/*
 *  OP_TYPE	the type for declarations and casts
 *  OP_DTYPE	The Datatype value for this type
 *  OP_BASE	The base value added to the FC for this type
 */
		int		inc_r, inc_o1, inc_o2, inc_o3;

	switch ( fctype ) {

	case D_BOOL: switch ( fc ) {
#define	OP_TYPE		bool
#define	OP_DTYPE	D_BOOL
#define	OP_BASE		(D_BOOL * 64)

#include	"qp_exev_op_bool.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_UINT: switch ( fc ) {
#define	OP_TYPE		UInt32
#define	OP_DTYPE	D_UINT
#define	OP_BASE		(D_UINT * 64)

#include	"qp_exev_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exev_op_cmp.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_ULONG: switch ( fc ) {
#define	OP_TYPE		UInt64
#define	OP_DTYPE	D_ULONG
#define	OP_BASE		(D_ULONG * 64)

#include	"qp_exev_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exev_op_cmp.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_INT: switch ( fc ) {
#define	OP_TYPE		Int32
#define	OP_DTYPE	D_INT
#define	OP_BASE		(D_INT * 64)

#include	"qp_exev_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exev_op_cmp.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_LONG: switch ( fc ) {
#define	OP_TYPE		Int64
#define	OP_DTYPE	D_LONG
#define	OP_BASE		(D_LONG * 64)

#include	"qp_exev_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exev_op_cmp.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_FLOAT: switch ( fc ) {
#define	OP_TYPE		Float32
#define	OP_DTYPE	D_FLOAT
#define	OP_BASE		(D_FLOAT * 64)

#include	"qp_exev_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exev_op_cmp.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_DOUBLE: switch ( fc ) {
#define	OP_TYPE		Float64
#define	OP_DTYPE	D_DOUBLE
#define	OP_BASE		(D_DOUBLE * 64)

#include	"qp_exev_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exev_op_cmp.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_STR: switch ( fc ) {
#include	"qp_exev_op_cmp_str.h"	/* PRE_INCLUDE */

		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	default:
		sf_report( "exec_op: Unkown type ( %d )\n", fctype );
		*errp = R_INTERNAL_ERROR;
		running = FALSE;
		break;
	}
	} else {
/*
 *  OP_TYPE	the type for declarations and casts
 *  OP_DTYPE	The Datatype value for this type
 *  OP_BASE	The base value added to the FC for this type
 */
	switch ( fctype ) {

	case D_BOOL: switch ( fc ) {
#define	OP_TYPE		bool
#define	OP_DTYPE	D_BOOL
#define	OP_BASE		(D_BOOL * 64)

#include	"qp_exe_op_bool.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_UINT: switch ( fc ) {
#define	OP_TYPE		UInt32
#define	OP_TYPE_MAX	D_UINT_MAX
#define	OP_TYPE_MIN	D_UINT_MIN
#define	OP_DTYPE	D_UINT
#define	OP_BASE		(D_UINT * 64)

#include	"qp_exe_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_cmp.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_vec.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_TYPE_MAX
#undef	OP_TYPE_MIN
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_ULONG: switch ( fc ) {
#define	OP_TYPE		UInt64
#define	OP_TYPE_MAX	D_ULONG_MAX
#define	OP_TYPE_MIN	D_ULONG_MIN
#define	OP_DTYPE	D_ULONG
#define	OP_BASE		(D_ULONG * 64)

#include	"qp_exe_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_cmp.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_vec.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_TYPE_MAX
#undef	OP_TYPE_MIN
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_INT: switch ( fc ) {
#define	OP_TYPE		Int32
#define	OP_TYPE_MAX	D_INT_MAX
#define	OP_TYPE_MIN	D_INT_MIN
#define	OP_DTYPE	D_INT
#define	OP_BASE		(D_INT * 64)

#include	"qp_exe_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_cmp.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_vec.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_TYPE_MAX
#undef	OP_TYPE_MIN
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_LONG: switch ( fc ) {
#define	OP_TYPE		Int64
#define	OP_TYPE_MAX	D_LONG_MAX
#define	OP_TYPE_MIN	D_LONG_MIN
#define	OP_DTYPE	D_LONG
#define	OP_BASE		(D_LONG * 64)

#include	"qp_exe_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_cmp.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_vec.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_TYPE_MAX
#undef	OP_TYPE_MIN
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_FLOAT: switch ( fc ) {
#define	OP_TYPE		Float32
#define	OP_TYPE_MAX	D_FLOAT_MAX
#define	OP_TYPE_MIN	D_FLOAT_MIN
#define	OP_DTYPE	D_FLOAT
#define	OP_BASE		(D_FLOAT * 64)

#include	"qp_exe_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_cmp.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_vec.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_TYPE_MAX
#undef	OP_TYPE_MIN
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_DOUBLE: switch ( fc ) {
#define	OP_TYPE		Float64
#define	OP_TYPE_MAX	D_DOUBLE_MAX
#define	OP_TYPE_MIN	D_DOUBLE_MIN
#define	OP_DTYPE	D_DOUBLE
#define	OP_BASE		(D_DOUBLE * 64)

#include	"qp_exe_op_num.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_cmp.h"	/* PRE_INCLUDE */
#include	"qp_exe_op_vec.h"	/* PRE_INCLUDE */

#undef	OP_TYPE
#undef	OP_TYPE_MAX
#undef	OP_TYPE_MIN
#undef	OP_DTYPE
#undef	OP_BASE
		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;

	case D_STR: switch ( fc ) {
#include	"qp_exe_op_str.h"	/* PRE_INCLUDE */

		default:
			sf_report( "exec_op: Unkown Fcode ( %d )\n", fc );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}
		break;
	}
	}

	return running;
}

#if defined(_WIN32) || defined(WIN32) || defined(CERNLIB_WINNT)
#pragma optimize( "", on )
#endif

static bool
exec_comis( long ievt, long ievt_chain, FCode fc, int iaddr, int npar, int *errp )
{
	bool			running = TRUE;
	void			*p1=0,*p2=0,*p3=0,*p4=0,*p5=0,*p6=0,*p7=0,*p8=0, *p9=0, *p10=0;
	register Int32		*r;

	PAWIDN.idnevt = ievt;
        PAWCHN.ichevt = ievt_chain;

	switch ( npar ) {
	case 8: p8 = &stack[frame[framep-7]];
	case 7: p7 = &stack[frame[framep-6]];
	case 6: p6 = &stack[frame[framep-5]];
	case 5: p5 = &stack[frame[framep-4]];
	case 4: p4 = &stack[frame[framep-3]];
	case 3: p3 = &stack[frame[framep-2]];
	case 2: p2 = &stack[frame[framep-1]];
	case 1: p1 = &stack[frame[framep-0]];
	case 0:
		break;
	default:
		sf_report( "exec_comis: To many arguments (%d)\n", npar );
		*errp = R_INTERNAL_ERROR;
		return FALSE;
		break;
	}

	switch ( fc ) {

	case FC_CS_LFUN:	/* call comis function returning bool */
		NEW_FRAME( D_BOOL, 1, r );
		*(bool *) r =
			CSLJCL( iaddr, npar, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
			== CF_TRUE ? TRUE : FALSE;
		break;

	case FC_CS_IFUN:	/* call comis function returning integer */
		NEW_FRAME( D_INT, 1, r );
		*(int *) r =
			CSIJCL( iaddr, npar, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10);
		break;

	case FC_CS_RFUN:	/* call comis function returning real */
		NEW_FRAME( D_FLOAT, 1, r );
		*(float *) r =
			CSRJCL(iaddr, npar, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10);
		break;

	case FC_CS_DFUN:	/* call comis function returning double */
		NEW_FRAME( D_DOUBLE, 1, r );
		*(double *) r =
			CSDJCL(iaddr, npar, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10);
		break;

	case FC_CS_SFUN:	/* call comis function returning string */
		sf_report( "exec_comis: FC_CS_SFUN not implemented\n");
		*errp = R_NOT_IMPLEMENTED;
		running = FALSE;
		break;

	default:
		sf_report( "exec_comis: Unkown Fcode ( %d )\n", fc );
		*errp = R_INTERNAL_ERROR;
		running = FALSE;
		break;
	}

	if ( npar > 0 ) {
		SQUEEZE_FRAME( npar );
	}

	return running;
}


void
qp_exec_seg( Segment start, long ievt, long ievt_chain, int *errp )
{
	register Segment	pc;
	register FCode		fc;
	bool			running;
	UInt32			opc;
	Category		cat;

	cut_call_sp = 0;

	pc = start;
	for ( running=TRUE ; running ; ) {
		/* decode instruction */
		opc = *pc;
		stat_opc++;
		fc = opc & MASK_FC;
		pc += 1;
		cat = qp_fc_to_cat[ fc >> CAT_BITS ];

		switch ( cat ) {

		case CAT_NONE:
			sf_report( "qp_exec_seg: Illegal category CAT_NONE\n" );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;

		case CAT_OP:
		{
			int		nc;
			bool		ok;

			if ( (opc & FC_DYNAMIC_TEST_BIT) != 0 ) {

				nc = *pc++;

				SHAPE_CHECK_N( nc, ok );
				if ( ! ok ) {
					sf_report( "Evt %ld: Shapes of the expressions do "
						"not match.\n", ievt );
					running = FALSE;
					*errp = R_SHAPE_ERROR;
					break;	/* skip the event */
				}

				if ( (opc & FC_DYNAMIC_TEST_BIT) != 0 ) {
					SHAPE_POP( nc-1 );
				} else {
					SHAPE_POP( nc );
				}
			}

			running = exec_op( fc, opc, errp );
			break;
		}

		case CAT_CONV:
			running = exec_conv( fc, errp );
			break;

		case CAT_CONST:
			running = exec_const( fc, errp );
			break;

		case CAT_MATH_SINGLE:
		{
			int		nc;
			bool		ok;

			if ( (opc & FC_DYNAMIC_TEST_BIT) != 0 ) {

				nc = *pc++;

				SHAPE_CHECK_N( nc, ok );
				if ( ! ok ) {
					sf_report( "Evt %ld: Shapes of the expressions do "
						"not match.\n", ievt );
					running = FALSE;
					*errp = R_SHAPE_ERROR;
					break;	/* skip the event */
				}

				if ( (opc & FC_DYNAMIC_TEST_BIT) != 0 ) {
					SHAPE_POP( nc-1 );
				} else {
					SHAPE_POP( nc );
				}
			}

			running = exec_math_single( fc, errp );
			break;
		}

		case CAT_MATH_DOUBLE:
		{
			int		nc;
			bool		ok;

			if ( (opc & FC_DYNAMIC_TEST_BIT) != 0 ) {

				nc = *pc++;

				SHAPE_CHECK_N( nc, ok );
				if ( ! ok ) {
					sf_report( "Evt %ld: Shapes of the expressions do "
						"not match.\n", ievt );
					running = FALSE;
					*errp = R_SHAPE_ERROR;
					break;	/* skip the event */
				}

				if ( (opc & FC_DYNAMIC_TEST_BIT) != 0 ) {
					SHAPE_POP( nc-1 );
				} else {
					SHAPE_POP( nc );
				}
			}

			running = exec_math_double( fc, errp );
			break;
		}

		case CAT_BITOP:
		{
			int		nc;
			bool		ok;

			if ( (opc & FC_DYNAMIC_TEST_BIT) != 0 ) {

				nc = *pc++;

				SHAPE_CHECK_N( nc, ok );
				if ( ! ok ) {
					sf_report( "Evt %ld: Shapes of the expressions do "
						"not match.\n", ievt );
					running = FALSE;
					*errp = R_SHAPE_ERROR;
					break;	/* skip the event */
				}

				if ( (opc & FC_DYNAMIC_TEST_BIT) != 0 ) {
					SHAPE_POP( nc-1 );
				} else {
					SHAPE_POP( nc );
				}
			}

			running = exec_bitop( fc, errp );
			break;
		}

		case CAT_VAR:
#include	"qp_exe_var.h"	/* PRE_INCLUDE */
			break;

		case CAT_LITERAL:
#include	"qp_exe_literal.h"	/* PRE_INCLUDE */
			break;

		case CAT_COMIS:
			{
				int	iaddr, npar;

				iaddr = *pc++;
				npar = *pc++;
				running = exec_comis( ievt, ievt_chain, fc, iaddr, npar, errp );
			}
			break;

		case CAT_DYN:
			{
				/* allow pc in register */
				Segment	tmp_pc = pc;

				running = exec_dyn( &tmp_pc, opc, ievt, errp );
				pc = tmp_pc;
			}
			break;

		case CAT_MASK:
			{
				/* allow pc in register */
				Segment	tmp_pc = pc;

				exec_mask( &tmp_pc, fc, ievt, errp );
				pc = tmp_pc;
			}
			break;

		case CAT_FLOW:
#include	"qp_exe_flow.h"	/* PRE_INCLUDE */
			break;

		default:
			sf_report( "qp_exec_seg: Unkown category ( %d )\n",
				cat );
			*errp = R_INTERNAL_ERROR;
			running = FALSE;
			break;
		}

		/* tricky, because we might have to pop several frames */
		while ( opc & FC_HALT_BIT ) {
			/* keep in sync with FC_HALT in qp_exe_flow.h */
			if ( cut_call_sp > 0 ) {
				int	size, i;
				bool	shape_pop;

				i = cut_index_stack[--cut_call_sp];
				opc = cut_opc_stack[cut_call_sp];
				pc = cut_call_stack[cut_call_sp];

				size = frame_size[framep];
				cut_size[i] = size;
				cut_bsize[i] = 4 * size *
					datatype_size[cut_type[i]];
				memcpy( cut_value[i], stack+frame[framep],
					cut_bsize[i] );
				cut_valid[i] = TRUE;
				if ( cut_has_shape[i] && ((opc & FC_DYNAMIC_INFO_BIT) == 0) ){
					SHAPE_POP( 1 );
				}
			} else {
				/* ready with main segment */
				running = FALSE;
				break;
			}
		}
	}
}


static void
qp_exec_end(
	QueryExe *	qe,
	QPCmd *		cmd,
	int *		errp
) {
	int		i;


	/* Flush mask */

	for ( i=0 ; i < nmasks ; i++ ) {
		if ( MaskList[i]->dirty ) {
			mask_flush_buf( MaskList[i] );
			mask_flush_header( MaskList[i] );
		}
	}

	/* close ntuple */

	/* free buffers */

	for ( i=0 ; i < qe->ncuts ; i++ ) {
		free( cut_value[i] );
		cut_value[i] = 0;
	}

	/* command finalisation */

	switch ( cmd->command ) {	/* switch CmdType */
	case CMD_NULL:
		break;

	case CMD_MINMAX:
		if ( qp_flags_get( "verbose" ) == 1 ) {
			for ( i=0 ; i < cmd->u.minmax.n ; i++ ) {
				switch ( cmd->u.minmax.el[i].dtyp ) {
				case D_UINT:
					printf( "[z'%x',z'%x']\n",
						cmd->u.minmax.el[i].min.uval,
						cmd->u.minmax.el[i].max.uval );
					break;
				case D_ULONG:
					printf( "[z'%lx',z'%lx']\n",
						cmd->u.minmax.el[i].min.luval,
						cmd->u.minmax.el[i].max.luval );
					break;
				case D_INT:
					printf( "[%d,%d]\n",
						cmd->u.minmax.el[i].min.ival,
						cmd->u.minmax.el[i].max.ival );
					break;
				case D_LONG:
					printf( "[%ld,%ld]\n",
						cmd->u.minmax.el[i].min.lival,
						cmd->u.minmax.el[i].max.lival );
					break;
				case D_FLOAT:
					printf( "[%g,%g]\n",
						cmd->u.minmax.el[i].min.fval,
						cmd->u.minmax.el[i].max.fval );
					break;
				case D_DOUBLE:
					printf( "[%lg,%lg]\n",
						cmd->u.minmax.el[i].min.lfval,
						cmd->u.minmax.el[i].max.lfval );
					break;
				case D_STR:
				{
					int	j, n;
					SMap	label_list;

					label_list = cmd->u.minmax.el[i].sm;

					n = smap_entries(label_list);
					for( j=0 ; j < n ; j++ ) {
						printf( "label_list(%d) = '%s'\n",
						j, smap_get( label_list, j ) ); 
					}
				}
				break;
				case D_BOOL:
					printf( "[%s,%s]\n",
						cmd->u.minmax.el[i].min.bval ?
						"true" : "false",
						cmd->u.minmax.el[i].max.bval ?
						"true" : "false" );
					break;
				default: 
					qp_abort( "qp_exec_end: Internal error."
						" DataType %d unknown\n",
						cmd->u.minmax.el[i].dtyp );
					break;
				}
			}
		}
		break;

	case CMD_MINMAXAVG:
		if ( qp_flags_get( "verbose" ) == 1 ) {
			for ( i=0 ; i < cmd->u.minmaxavg.n ; i++ ) {
				switch ( cmd->u.minmaxavg.el[i].dtyp ) {
				case D_UINT:
					printf( "[z'%x',z'%x']\n",
						cmd->u.minmaxavg.el[i].min.uval,
						cmd->u.minmaxavg.el[i].max.uval );
					break;
				case D_ULONG:
					printf( "[z'%lx',z'%lx']\n",
						cmd->u.minmaxavg.el[i].min.luval,
						cmd->u.minmaxavg.el[i].max.luval );
					break;
				case D_INT:
					printf( "[%d,%d]\n",
						cmd->u.minmaxavg.el[i].min.ival,
						cmd->u.minmaxavg.el[i].max.ival );
					break;
				case D_LONG:
					printf( "[%ld,%ld]\n",
						cmd->u.minmaxavg.el[i].min.lival,
						cmd->u.minmaxavg.el[i].max.lival );
					break;
				case D_FLOAT:
					printf( "[%g,%g]\n",
						cmd->u.minmaxavg.el[i].min.fval,
						cmd->u.minmaxavg.el[i].max.fval );
					break;
				case D_DOUBLE:
					printf( "[%lg,%lg]\n",
						cmd->u.minmaxavg.el[i].min.lfval,
						cmd->u.minmaxavg.el[i].max.lfval );
					break;
				case D_STR:
				{
					int	j, n;
					SMap	label_list;

					label_list = cmd->u.minmaxavg.el[i].sm;

					n = smap_entries(label_list);
					for( j=0 ; j < n ; j++ ) {
						printf( "label_list(%d) = '%s'\n",
						j, smap_get( label_list, j ) ); 
					}
				}
				break;
				case D_BOOL:
					printf( "[%s,%s]\n",
						cmd->u.minmaxavg.el[i].min.bval ?
						"true" : "false",
						cmd->u.minmaxavg.el[i].max.bval ?
						"true" : "false" );
					break;
				default: 
					qp_abort( "qp_exec_end: Internal error."
						" DataType %d unknown\n",
						cmd->u.minmaxavg.el[i].dtyp );
					break;
				}
			}
		}

		/* compute the average */
		for (i=0; i<cmd->u.minmaxavg.n; i++) {
		  cmd->u.minmaxavg.el[i].avg /= (double)cmd->u.minmaxavg.evt_count;
		}

		break;

	case CMD_VFILL:
		break;

	case CMD_HFILL1:
		if ( cmd->u.hfill1.n != 0 ) {
			HF1N( cmd->u.hfill1.idh,
				cmd->u.hfill1.x,
				cmd->u.hfill1.w,
				cmd->u.hfill1.n );
			cmd->u.hfill1.n = 0;
		}
		free( (void *) cmd->u.hfill1.x );
		free( (void *) cmd->u.hfill1.w );
		break;

	case CMD_HFILL2:
		break;

	case CMD_SCAT2:
		if ( cmd->u.scat2.n != 0 ) {
			if ( cmd->u.scat2.line ) {
				IPL( cmd->u.scat2.n, cmd->u.scat2.x,
					cmd->u.scat2.y );
			} else {
				IPMID( cmd->u.scat2.n, cmd->u.scat2.x,
					cmd->u.scat2.y, 2, cmd->u.scat2.ievt);
			}
			cmd->u.scat2.n = 0;
		}
		free( (void *) cmd->u.scat2.ievt );
		free( (void *) cmd->u.scat2.x );
		free( (void *) cmd->u.scat2.y );
		break;

	case CMD_SCAT3:
		if ( cmd->u.scat3.n != 0 ) {
			if ( cmd->u.scat3.line ) {
				IPL3( cmd->u.scat3.n, cmd->u.scat3.x,
					cmd->u.scat3.y, cmd->u.scat3.z );
			} else {
				IPM3ID( cmd->u.scat3.n, cmd->u.scat3.x,
					cmd->u.scat3.y, cmd->u.scat3.z, 
					2, cmd->u.scat3.ievt);
			}
			cmd->u.scat3.n = 0;
		}
		free( (void *) cmd->u.scat3.ievt );
		free( (void *) cmd->u.scat3.x );
		free( (void *) cmd->u.scat3.y );
		free( (void *) cmd->u.scat3.z );
		break;

	case CMD_GOURAUD: {
		QPCmdGouraud	*h;
		register float  min, max, avg, rms, *pvalue;
		int             icell=0;

		h = &cmd->u.gouraud;

		/* Do some statistics on the 3D Histo */

		min = FLT_MAX;
		max = 0.0;
		avg = 0.0;
		rms = 0.0;
		pvalue = h->value;
		
		for (i=0; i<(h->nx)*(h->ny)*(h->nz); i++) {
		  avg += *pvalue;
		  rms += (*pvalue)*(*pvalue);
		  if (*pvalue<min) min = *pvalue;
		  if (*pvalue>max) max = *pvalue;
		  if (*pvalue++>0.5) icell++;
		}

		if (icell>0) {
		  avg /= (float)icell;
		  rms = sqrt(fabs(((double)rms/(double)icell)-(double)(avg*avg)));
		}
		else {
		  avg = 0.0;
		  rms = 0.0;
		}

		h->s[2] = (avg+rms<max ? avg+rms : avg+0.5*(max-avg));
		h->s[0] = (avg-rms>min ? avg-rms : avg-0.5*(avg-min));
		h->s[1] = avg;
		
		PADRISO (h->nx,h->ny,h->nz,h->x,h->y,h->z,h->value,h->s);
		
		free( (void *) h->x );
		free( (void *) h->y );
		free( (void *) h->z );
		break;
	}

	case CMD_SCAT4:
		if ( cmd->u.scat4.n != 0 ) {
			if ( cmd->u.scat4.line ) {
				IPL3( cmd->u.scat4.n, cmd->u.scat4.x,
					cmd->u.scat4.y, cmd->u.scat4.z );
			} else {
				int IC1,IC2[1];
				IPM4ID( cmd->u.scat4.n, cmd->u.scat4.x,
					cmd->u.scat4.y, cmd->u.scat4.z,
					cmd->u.scat4.c,
					cmd->u.scat4.col_min,
					cmd->u.scat4.col_max,
					2, cmd->u.scat4.ievt);
				IGCOLM(0.,0.,0.,0.,
					IC1,IC2,
					cmd->u.scat4.col_min,
					cmd->u.scat4.col_max,
					"CAL");
			}
			cmd->u.scat4.n = 0;
		}
		free( (void *) cmd->u.scat4.ievt );
		free( (void *) cmd->u.scat4.c );
		free( (void *) cmd->u.scat4.x );
		free( (void *) cmd->u.scat4.y );
		free( (void *) cmd->u.scat4.z );
		break;

	case CMD_LOOP:
		break;

	case CMD_DUMP:
		break;

	case CMD_SCAN:
		{
			if ( cmd->u.scan.pawpp ) {
				int	mone = -1;

				(*cmd->u.scan.matrow)(&mone);
			} else {
				if ( *errp == R_NOERR ) {
					if ( cmd->u.scan.evt_count > 0 ) {
						FTNPRN( cmd->u.scan.sep );
					}
					sprintf( cmd->u.scan.buf, "==> %d event%s satisfied the imposed "
						"cuts\n", cmd->u.scan.evt_count,
						cmd->u.scan.evt_count != 1 ? "s": "" );
					FTNPRN( cmd->u.scan.buf );
				}
			}
		}
		break;


	case CMD_SPIDERSCAN:
		{
		  QPCmdSpiderScan	*c = &cmd->u.sp_scan;
			
		  if ( *errp == R_NOERR ) {
		    char buf[100];
		    sprintf( buf, "==> %d event%s satisfied the imposed "
			     "cuts\n", c->evt_count,
			     c->evt_count != 1 ? "s": "" );
		    FTNPRN( buf );
		  }

		  /* Restore graphic parameters saved during the initialization */
		  HPLSET ("XWIN", (c->saved[0]));
		  HPLSET ("YWIN", (c->saved[1]));
		  HPLSET ("XMGL", (c->saved[2]));
		  HPLSET ("XMGR", (c->saved[3]));
		  HPLSET ("YMGL", (c->saved[4]));
		  HPLSET ("YMGU", (c->saved[5]));
                  HPLZON (c->saved[6], c->saved[7], 1, " ");

		}
		break;

	default:
		qp_abort( "Internal error, unknown CmdType %d", cmd->command );
		break;
	}

	free( (void *) CWNBlock );
	free( (void *) namelist );
	free( (void *) offsetlist );
	free( (void *) indexlist );
}


void
qp_execute(
	QueryExe *	qe,
	long		start_evt,
	long		nevt,
	QPCmd *		cmd,
	int *		errp
)
{
	bool		new_file, is_chain, is_cwn, done;
	long		ievt, ievt_chain, nlast, ievtlcl, end_evt;
	int		noffset, nrow, ierr;
        int numevt,numerr,numerr_chain;
        Entry *current_entry;
        Chain *current_chain;
        int nfiles,iskip;
        float pererr;
        char * fname;
	qp_exec_init( qe, start_evt, nevt, cmd, errp );
	if ( *errp != R_NOERR ) {
		return;
	}

	if ( timing ) {
		qptime( 0 );
	}

	is_chain = PAWCHN.chain;
	is_cwn = qe->new_ntuple;
	new_file = TRUE;
	ievt = start_evt;
	ievt_chain = start_evt;
	end_evt = start_evt + nevt - 1;

	h_hnoent( qe->id, FALSE, &nrow );
	noffset = 0;
      	done = FALSE;
        numerr=0;
        numerr_chain=0;
        numevt=0;
        nfiles=0;

	do {
		nlast = noffset + nrow;

		if ( ievt <= nlast ) {
			/* do one file */
			ievtlcl = ievt - noffset;
			for ( ; ievtlcl <= nrow ; ievtlcl++, ievt++ ) { 

				/* load the data for this event */
				if ( nt_load_from_disk ) {
					if ( is_cwn ) {
						if ( new_file ) {
							HGNTBF( qe->id,
								namelist,
								offsetlist,
								nt_load_cnt,
								ievtlcl, ierr );
							new_file = FALSE;
						} else {
							HGNTF( qe->id,
								ievtlcl, ierr );
						}
					} else {
						if ( new_file ) {
							HGNPAR( qe->id,
								"qp_execute" );
							new_file = FALSE;
						}
						HGNF( qe->id, ievtlcl,
							PAWIDN.x[0], ierr );
					}

					if ( ierr != 0 ) {
						*errp = R_INTERNAL_ERROR;
						break;
                                        
					}
				}

				/* copy data from cache to comis commonblocks */
				/* and data from RWN common to the cache */
				{
					register RefCWN *	cwnp;
					register int		i;

					cwnp = CWNBlock;
					for (i=CWNCount ; i-- > 0 ; cwnp++ ) {
						if ( cwnp->p_cs != 0 ) {
							memcpy( cwnp->p_cs,
								cwnp->p,
								4*cwnp->step );
						}
						if ( cwnp->p_rwn != 0 ) {
							*cwnp->p_rwn = *(Float32*)cwnp->p;
						}
					}
				}

				/* execute command for this event */

				qp_exec_evt( qe, ievt, ievt_chain, cmd, errp );

                                ievt_chain++;
                                numevt++;
              		        if ( *errp != R_NOERR ) {
				      break;
				}


				/* update our cache pointers */

				{
					register RefCWN *	cwnp;
					register int		i;

					cwnp = CWNBlock;
					for ( i=CWNCount ; i-- > 0 ; cwnp++ ) {
						cwnp->p += cwnp->step;
						if ( cwnp->p_rwn != 0 ) {
							cwnp->p_rwn += 1;
						}
					}
				}

				if ( ievt == end_evt ) {
					done = TRUE;
					break;
				}
			}
		}

/*		if ( *errp == R_NOERR && is_chain ) { */
		if ( is_chain ) { 
                    if( *errp != R_NOERR){


                       current_chain=find_chain(qe->path+2);
                       if(current_chain){
                             current_entry = current_chain->entries;
                             for(iskip=0;current_entry && iskip<nfiles;iskip++){
                               current_entry=current_entry->link;
                             }
                       }
                       else current_entry=NULL;
                       if(current_entry)fname=current_entry->name;
                       else fname=NULL;


                       numerr+=nlast-ievt+1;
    		       printf( " @@@ %s %s %s [%d]  \n ", "****NTUPLEHASERRORS*** ",qe->path,fname,nlast-ievt+1);
                      *errp=R_NOERR;
                      ievt=nlast+1; 
                    }
                     nfiles++;
			if ( ! done ) {
				/* Attach the next file from the chain */
				int	ndim, iend = 0;

				/* in PCNEXT *current* nrow will be added to noffset */
				/* on exit nrow will contain the *NEW* value */
				PCNEXT(qe->id, noffset, ndim, nrow, iend);
				reinit_cwn( "?member?", qe->id, errp );
				new_file = TRUE;
                                numerr_chain=0;
				done = iend != 0;
			} else {
				int	ndim, iend = 0;
				/* cycle through chain to position at the start */
				do {
					PCNEXT(qe->id, noffset, ndim, nrow, iend);
				} while ( iend == 0 );

			}
		}
                ievt_chain = 1;

	} while ( is_chain && ! done && *errp == R_NOERR );
        if(numerr ){
		printf( "@@@ %s \n Events read %d Errors %d Percent %d\n", "*******WARNING**********ChainContainsErrors*******",numevt+numerr,numerr,100*numerr/(numevt+numerr));
        }
	qp_exec_end( qe, cmd, errp );

	if (  *errp != R_NOERR ) {
		HBFREE( -1 );	/* free the buffers that were being filled */
	}


	/* optionally report timings and statistics */
	if ( timing ) {
		double	opc_avg;

		qptime( 1 );

		if ( stat_event != 0 ) {
			opc_avg = stat_opc / (double) stat_event;
		} else {
			opc_avg = 0.;
		}

		sf_report( "EVTS %7ld   SEL %7ld   OPC %8ld ( %lf / EVT )\n",
			stat_event, stat_selected, stat_opc, opc_avg );
	}

}
