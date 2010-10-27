#ifndef _NR_UTILS_H_
#define _NR_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#if 0
static double sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

static double dsqrarg;
#define DSQR(a) ((dsqrarg=(a)) == 0.0 ? 0.0 : dsqrarg*dsqrarg)

static double dmaxarg1,dmaxarg2;
#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
        (dmaxarg1) : (dmaxarg2))

static double dminarg1,dminarg2;
#define DMIN(a,b) (dminarg1=(a),dminarg2=(b),(dminarg1) < (dminarg2) ?\
        (dminarg1) : (dminarg2))

static double maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
        (maxarg1) : (maxarg2))

static double minarg1,minarg2;
#define FMIN(a,b) (minarg1=(a),minarg2=(b),(minarg1) < (minarg2) ?\
        (minarg1) : (minarg2))

static long lmaxarg1,lmaxarg2;
#define LMAX(a,b) (lmaxarg1=(a),lmaxarg2=(b),(lmaxarg1) > (lmaxarg2) ?\
        (lmaxarg1) : (lmaxarg2))

static long lminarg1,lminarg2;
#define LMIN(a,b) (lminarg1=(a),lminarg2=(b),(lminarg1) < (lminarg2) ?\
        (lminarg1) : (lminarg2))

static int imaxarg1,imaxarg2;
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ?\
        (imaxarg1) : (imaxarg2))

static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
        (iminarg1) : (iminarg2))
#endif

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

#if defined(__STDC__) || defined(ANSI) || defined(NRANSI)
#define _(X) X
#else
#define _(X) ()
#endif

void nrerror _((char error_text[]));
double *vector _((long nl, long nh));
int *ivector _((long nl, long nh));
unsigned char *cvector _((long nl, long nh));
unsigned long *lvector _((long nl, long nh));
double *dvector _((long nl, long nh));
double **matrix _((long nrl, long nrh, long ncl, long nch));
double **dmatrix _((long nrl, long nrh, long ncl, long nch));
int **imatrix _((long nrl, long nrh, long ncl, long nch));
double **submatrix _((double **a, long oldrl, long oldrh, long oldcl, long oldch,
        long newrl, long newcl));
double **convert_matrix _((double *a, long nrl, long nrh, long ncl, long nch));
double ***f3tensor _((long nrl, long nrh, long ncl, long nch, long ndl, long ndh));
void free_vector _((double *v, long nl, long nh));
void free_ivector _((int *v, long nl, long nh));
void free_cvector _((unsigned char *v, long nl, long nh));
void free_lvector _((unsigned long *v, long nl, long nh));
void free_dvector _((double *v, long nl, long nh));
void free_matrix _((double **m, long nrl, long nrh, long ncl, long nch));
void free_dmatrix _((double **m, long nrl, long nrh, long ncl, long nch));
void free_imatrix _((int **m, long nrl, long nrh, long ncl, long nch));
void free_submatrix _((double **b, long nrl, long nrh, long ncl, long nch));
void free_convert_matrix _((double **b, long nrl, long nrh, long ncl, long nch));
void free_f3tensor _((double ***t, long nrl, long nrh, long ncl, long nch,
        long ndl, long ndh));

#ifdef __cplusplus
}
#endif

#endif /* _NR_UTILS_H_ */
