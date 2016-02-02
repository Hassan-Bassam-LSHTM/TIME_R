/* TB model in C code to call from R */

/* Single year age bin version */

/* Can be compiled within R with system("R CMD SHLIB TB_model.c") */
/* This creates a dynamic linked library (.dll) which can be loaded (dyn.load(TB_model.dll)) into R and used as the model function in a call to desolve */

/* C libraries needed */
#include <R.h>
#include <math.h>

/* You need to define number of parameters and forcing functions passed to the model here */
/* These must match number in intializer functions below */
static double parms[404];
static double forc[166];

/* ###### A TRICK TO KEEP UP WITH THE PARAMETERS AND FORCINGS ###### */

/* !!!!!!! NOTE IN C INDEXING STARTS AT 0 AND NOT 1 (i.e. parms[0] is the first value in parms) !!!!!! */

/* Parameters are constant over time */
#define beta parms[0]       /* effective contact rate */
#define a_a parms[1]        /* proportion developing primary disease in adults (>15) */
#define a0 parms[2]         /* proportion developing primary disease in 0-4 year olds */ 
#define a5 parms[3]         /* proportion developing primary disease in 5-9 year olds */
#define a10 parms[4]        /* proportion developing primary disease in 10-14 year olds */
#define v parms[5]          /* reactivation rate */
#define p parms[6]          /* protection against disease due to prior infection */
#define sig_a parms[7]      /* proportion developing smear positive disease in adults (>15) */
#define sig0 parms[8]       /* proportion developing smear positive disease in 0-4 year olds */
#define sig5 parms[9]       /* proportion developing smear positive disease in 5-9 year olds */
#define sig10 parms[10]     /* proportion developing smear positive disease in 10-14 year olds */
#define rel_inf parms[11]   /* relative infectiousness of smear negative TB */
#define theta parms[12]     /* rate of conversion from smear negative to smear positive TB */
#define r parms[13]         /* rate of self cure from TB */
#define mu_N parms[14]      /* mortality rate from smear negative TB (>15 years old) */
#define mu_N0 parms[15]     /* mortality rate from smear negative TB in 0-4 year olds */
#define mu_N5 parms[16]     /* mortality rate from smear negative TB in 5-9 year olds */
#define mu_N10 parms[17]    /* mortality rate from smear negative TB in 10-14 year olds */
#define mu_I parms[18]      /* mortality rate from smear positive TB (>15 years old) */
#define mu_I0 parms[19]     /* mortality rate from smear positive TB in 0-4 year olds */
#define mu_I5 parms[20]     /* mortality rate from smear positive TB in 5-9 year olds */
#define mu_I10 parms[21]    /* mortality rate from smear positive TB in 10-14 year olds */
#define fit_cost parms[22]  /* relative fitness of MDR strains (transmission only) */
#define e parms[23]         /* rate of acquisition of MDR */
#define g parms[24]         /* superinfections */
#define eff_n parms[25]     /* efficacy of first line drugs in treating new MDR cases */
#define eff_p parms[26]     /* efficacy of first line drugs in treating retreatment MDR cases */
#define muN_H parms[27]     /* mortaltiy rate from smear negative TB in HIV+ */
#define muI_H parms[28]     /* mortaltiy rate from smear positive TB in HIV+ */
#define RR1a parms[29]      /* Relative risk for primary disease following HIV infection */
#define RR2a parms[30]      /* Relative risk for primary disease by CD4 */
#define RR1v parms[31]      /* Relative risk for reactivation following HIV infection */
#define RR2v parms[32]      /* Relative risk for reactivation by CD4 */
#define RR1p parms[33]      /* Relative risk of protection against reinfection following HIV infection */
#define RR2p parms[34]      /* Relative risk of protection against reinfection by CD4 */
#define ART_TB1 parms[35]   /* Reduction in TB parameters on ART (<6m) */
#define ART_TB2 parms[36]   /* Reduction in TB parameters on ART (7-12m) */
#define ART_TB3 parms[37]   /* Reduction in TB parameters on ART (>12m) */
#define ART_mort1 parms[38] /* Reduction in TB mortality on ART (<6m) */
#define ART_mort2 parms[39] /* Reduction in TB mortality on ART (7-12m) */
#define ART_mort3 parms[40] /* Reduction in TB mortality on ART (>12m) */
#define BCG_eff parms[41]   /* Efficacy of BCG (reduces primary and reactivation risks) */
#define sig_H parms[42]     /* proportion developing smear positive disease in HIV+ */
#define r_H parms[43]       /* rate of self cure from TB in HIV+ */
#define rel_inf_H parms[44] /* relative infectiousness of smear negative cases in HIV+ */
#define theta_H parms[45]   /* rate of conversion from smear negative to smear positive TB in HIV+ */ 

#define HIV_run parms[46]   /* used to skip HIV equations in equilibrium run */

/* Forcings are time dependant functions e.g. mortality rates */
#define birth_rate forc[0] /* Birth rate */

#define s1 forc[1]         /* Survival to age x */
#define s2 forc[2] 
#define s3 forc[3] 
#define s4 forc[4] 
#define s5 forc[5]
#define s6 forc[6]
#define s7 forc[7]
#define s8 forc[8]
#define s9 forc[9]
#define s10 forc[10]
#define s11 forc[11]
#define s12 forc[12]
#define s13 forc[13]
#define s14 forc[14]
#define s15 forc[15]
#define s16 forc[16]
#define s17 forc[17]
#define s18 forc[18]       
#define s19 forc[19] 
#define s20 forc[20] 
#define s21 forc[21] 
#define s22 forc[22]
#define s23 forc[23]
#define s24 forc[24]
#define s25 forc[25]
#define s26 forc[26]
#define s27 forc[27]
#define s28 forc[28]
#define s29 forc[29]
#define s30 forc[30]
#define s31 forc[31]
#define s32 forc[32]
#define s33 forc[33]
#define s34 forc[34]
#define s35 forc[35]
#define s36 forc[36]
#define s37 forc[37]
#define s38 forc[38]         
#define s39 forc[39] 
#define s40 forc[40] 
#define s41 forc[41] 
#define s42 forc[42]
#define s43 forc[43]
#define s44 forc[44]
#define s45 forc[45]
#define s46 forc[46]
#define s47 forc[47]
#define s48 forc[48]
#define s49 forc[49]
#define s50 forc[50]
#define s51 forc[51]
#define s52 forc[52]
#define s53 forc[53]
#define s54 forc[54]
#define s55 forc[55]
#define s56 forc[56]
#define s57 forc[57]
#define s58 forc[58]        
#define s59 forc[59] 
#define s60 forc[60] 
#define s61 forc[61] 
#define s62 forc[62]
#define s63 forc[63]
#define s64 forc[64]
#define s65 forc[65]
#define s66 forc[66]
#define s67 forc[67]
#define s68 forc[68]
#define s69 forc[69]
#define s70 forc[70]
#define s71 forc[71] 
#define s72 forc[72]
#define s73 forc[73]
#define s74 forc[74]
#define s75 forc[75]
#define s76 forc[76]
#define s77 forc[77]
#define s78 forc[78]
#define s79 forc[79]
#define s80 forc[80]
#define s81 forc[81]

#define h0 forc[82]  /* HIV incidence at age x */
#define h5 forc[83]
#define h10 forc[84]
#define h15 forc[85]
#define h20 forc[86]
#define h25 forc[87]
#define h30 forc[88]
#define h35 forc[89]
#define h40 forc[90]
#define h45 forc[91]
#define h50 forc[92]
#define h55 forc[93]
#define h60 forc[94]
#define h65 forc[95]
#define h70 forc[96]
#define h75 forc[97]
#define h80 forc[98]

#define BCG_cov forc[99] /* BCG coverage */

#define pop_ad forc[100]  /* used to turn on/off adjustment of population to account for disease induced mortality - idea is to turn it off from 2015 onwards*/

#define kneg forc[101]  /* detection rate */
#define kpos forc[102]

#define rel_d forc[103] /* relative detection of smear negatives */

#define dstneg_n forc[104]   /* dst coverage in new cases */
#define dstneg_p forc[105]   /* dst coverage in previoulsy treated cases */
#define dstpos_n forc[106]  
#define dstpos_p forc[107] 

#define l_s forc[108]  /* linkage to care */
#define l_m forc[109]

#define tneg_s forc[110]  /* treatment success */
#define tpos_s forc[111]
#define tART_s forc[112]
#define tneg_m forc[113]
#define tpos_m forc[114]
#define tART_m forc[115]

#define mig0 forc[116]  /* migration by age x */
#define mig5 forc[117]
#define mig10 forc[118]
#define mig15 forc[119]
#define mig20 forc[120]
#define mig25 forc[121]
#define mig30 forc[122]
#define mig35 forc[123]
#define mig40 forc[124]
#define mig45 forc[125]
#define mig50 forc[126]
#define mig55 forc[127]
#define mig60 forc[128]
#define mig65 forc[129]
#define mig70 forc[130]
#define mig75 forc[131]
#define mig80 forc[132]

/* sens and spec for tests */
#define se_I_neg forc[133]
#define se_N_neg forc[134]
#define se_m_neg forc[135]

#define sp_I_neg forc[136]
#define sp_N_neg forc[137]
#define sp_m_neg forc[138]

#define se_I_pos forc[139]
#define se_N_pos forc[140]
#define se_m_pos forc[141]

#define sp_I_pos forc[142]
#define sp_N_pos forc[143]
#define sp_m_pos forc[144]

/* RR for presentation for healthy individuals */
#define health forc[145]

/* ART coverage (% of those eligible by age) */
#define A0 forc[146]
#define A5 forc[147]
#define A10 forc[148]
#define A15 forc[149]
#define A20 forc[150]
#define A25 forc[151]
#define A30 forc[152]
#define A35 forc[153]
#define A40 forc[154]
#define A45 forc[155]
#define A50 forc[156]
#define A55 forc[157]
#define A60 forc[158]
#define A65 forc[159]
#define A70 forc[160]
#define A75 forc[161]
#define A80 forc[162]

#define Athresh forc[163] /* ART eligibility threshold (CD4 category) */

#define HIV_test forc[164] /* proportion of notifed TB cases tested for HIV */
#define ART_link forc[165] /* proportion of those tested positive linked to ART */

/* ###### FUNCTION TO SUM ARRAY FROM ELEMENT i_start TO i_end ###### */
double sumsum(double ar[], int i_start, int i_end)
{
   int i=0;
   double sum=0;
   for (i=i_start; i<=i_end; i++)
   {
    sum = sum + ar[i];
   }
   return(sum);
}

/* ###### FUNCTION TO INITIALIZE PARAMETERS PASSED FROM R - if the number of parameters is changed you must update N here ###### */
void parmsc(void (* odeparms)(int *, double *))
{
    int N=404;
    odeparms(&N, parms);
}

/* ###### FUNCTION TO INITIALIZE FORCINGS PASSED FROM R - if the number of parameters is changed you must update N here ###### */
void forcc(void (* odeforcs)(int *, double *))
{
    int N=166;
    odeforcs(&N, forc);
}

/* ##### EVENTS ARE USED TO ADD BIRTHS AND SHIFT THE POPULATION BY AGE - EQUIVALENT TO THE METHOD OF SCHENZLE ###### */

void event(int *n, double *t, double *y) 
{
  int i;
  
  /* Store current population in temp and shift every age group forward one */
  double temp[35235];
  temp[0] = y[0];
  for (i=1; i<35235; i++){
    temp[i] = y[i];
    y[i] = temp[i-1];
  }
  /* Set every 0 age group to zero and every >80 age group to the previous age group plus those still surviving  */
  for (i=0; i<35235; i+=81) {
    y[i] = 0;
    y[i+80] = temp[i+80] + temp[i+79];  
  }
  /* Then add births into group 0 - only susceptibles get born */ 
  y[0] = birth_rate*sumsum(temp,0,35234)/1000;
}

/* ###### DERIVATIVE FUNCTIONS - THIS IS THE MODEL ITSELF ###### */

void derivs1(int *neq, double *t, double *y, double *ydot, double *yout, int *ip)
{
    if (ip[0] <2) error("nout should be at least 2");
    
    /* Expand state variables so can use more meaningful names than y and ydot (the variables and rates of change are vectors y and ydot here) */
    /* There are 81 age groups, 7 HIV positive categories and 3 times on ART */
    /* _H = HIV+; _A = HIV+ on ART */
    
    /* These are the variables */
    double S[81]={0};   double S_H[81][7]={{0}};   double S_A[81][7][3]={{{0}}};      /* Susceptible */
    double Lsn[81]={0}; double Lsn_H[81][7]={{0}}; double Lsn_A[81][7][3]={{{0}}};    /* Latent, DS, new */
    double Lsp[81]={0}; double Lsp_H[81][7]={{0}}; double Lsp_A[81][7][3]={{{0}}};    /* Latent, DS, previous */
    double Lmn[81]={0}; double Lmn_H[81][7]={{0}}; double Lmn_A[81][7][3]={{{0}}};    /* Latent, DR, new */
    double Lmp[81]={0}; double Lmp_H[81][7]={{0}}; double Lmp_A[81][7][3]={{{0}}};    /* Latent, DR, previous */
    double Nsn[81]={0}; double Nsn_H[81][7]={{0}}; double Nsn_A[81][7][3]={{{0}}};    /* Smear negative, DS, new */
    double Nsp[81]={0}; double Nsp_H[81][7]={{0}}; double Nsp_A[81][7][3]={{{0}}};    /* Smear negative, DS, previous */
    double Nmn[81]={0}; double Nmn_H[81][7]={{0}}; double Nmn_A[81][7][3]={{{0}}};    /* Smear negative, DR, new */
    double Nmp[81]={0}; double Nmp_H[81][7]={{0}}; double Nmp_A[81][7][3]={{{0}}};    /* Smear negative, DR, previous */
    double Isn[81]={0}; double Isn_H[81][7]={{0}}; double Isn_A[81][7][3]={{{0}}};    /* Smear positive, DS, new */
    double Isp[81]={0}; double Isp_H[81][7]={{0}}; double Isp_A[81][7][3]={{{0}}};    /* Smear positive, DS, previous */
    double Imn[81]={0}; double Imn_H[81][7]={{0}}; double Imn_A[81][7][3]={{{0}}};    /* Smear positive, DR, new */
    double Imp[81]={0}; double Imp_H[81][7]={{0}}; double Imp_A[81][7][3]={{{0}}};    /* Smear positive, DR, previous */
    double PTn[81]={0}; double PTn_H[81][7]={{0}}; double PTn_A[81][7][3]={{{0}}};     /* Post PT, new - also move people here if they are false positive for TB and receive Rx */
    double PTp[81]={0}; double PTp_H[81][7]={{0}}; double PTp_A[81][7][3]={{{0}}};     /* Post PT, previous - also move people here if they are false positive for TB and receive Rx */

    /* These are the rates of change (same names but prefixed with d) */
    double dS[81]={0};   double dS_H[81][7]={{0}};   double dS_A[81][7][3]={{{0}}};
    double dLsn[81]={0}; double dLsn_H[81][7]={{0}}; double dLsn_A[81][7][3]={{{0}}};
    double dLsp[81]={0}; double dLsp_H[81][7]={{0}}; double dLsp_A[81][7][3]={{{0}}};
    double dLmn[81]={0}; double dLmn_H[81][7]={{0}}; double dLmn_A[81][7][3]={{{0}}};
    double dLmp[81]={0}; double dLmp_H[81][7]={{0}}; double dLmp_A[81][7][3]={{{0}}};
    double dNsn[81]={0}; double dNsn_H[81][7]={{0}}; double dNsn_A[81][7][3]={{{0}}};
    double dNsp[81]={0}; double dNsp_H[81][7]={{0}}; double dNsp_A[81][7][3]={{{0}}};
    double dNmn[81]={0}; double dNmn_H[81][7]={{0}}; double dNmn_A[81][7][3]={{{0}}};
    double dNmp[81]={0}; double dNmp_H[81][7]={{0}}; double dNmp_A[81][7][3]={{{0}}};
    double dIsn[81]={0}; double dIsn_H[81][7]={{0}}; double dIsn_A[81][7][3]={{{0}}};
    double dIsp[81]={0}; double dIsp_H[81][7]={{0}}; double dIsp_A[81][7][3]={{{0}}};
    double dImn[81]={0}; double dImn_H[81][7]={{0}}; double dImn_A[81][7][3]={{{0}}};
    double dImp[81]={0}; double dImp_H[81][7]={{0}}; double dImp_A[81][7][3]={{{0}}};
    double dPTn[81]={0}; double dPTn_H[81][7]={{0}}; double dPTn_A[81][7][3]={{{0}}}; 
    double dPTp[81]={0}; double dPTp_H[81][7]={{0}}; double dPTp_A[81][7][3]={{{0}}}; 

    /* intergers to use as counters */ 
    int i,j,l,ij,iz;
    /* used to index the correct forcing functions by age */
    int iii[81] = {0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8,9,9,9,9,9,
                   10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,13,13,13,13,13,14,14,14,14,14,15,15,15,15,15,16};  
     
    /* Then need to assign the variables to the correct bit of y (we do the same for the rates of change after solving the equations) */
     
    /* HIV- */ 
    
    int n_age = 81;     /* Number of age groups */
    int n_HIV = 7;      /* Number of HIV pos groups */
    int n_ART = 3;      /* Number of ART groups */
    int n_disease = 15; /* Number of disease states */
    
    for (i=0; i<n_age; i++) S[i] = y[i];             
    for (i=n_age; i<n_age*2; i++) Lsn[i-n_age] = y[i];       
    for (i=n_age*2; i<n_age*3; i++) Lsp[i-n_age*2] = y[i];       
    for (i=n_age*3; i<n_age*4; i++) Lmn[i-n_age*3] = y[i];    
    for (i=n_age*4; i<n_age*5; i++) Lmp[i-n_age*4] = y[i];     
    for (i=n_age*5; i<n_age*6; i++) Nsn[i-n_age*5] = y[i];      
    for (i=n_age*6; i<n_age*7; i++) Nsp[i-n_age*6] = y[i];    
    for (i=n_age*7; i<n_age*8; i++) Nmn[i-n_age*7] = y[i];    
    for (i=n_age*8; i<n_age*9; i++) Nmp[i-n_age*8] = y[i];    
    for (i=n_age*9; i<n_age*10; i++) Isn[i-n_age*9] = y[i];    
    for (i=n_age*10; i<n_age*11; i++) Isp[i-n_age*10] = y[i];    
    for (i=n_age*11; i<n_age*12; i++) Imn[i-n_age*11] = y[i];    
    for (i=n_age*12; i<n_age*13; i++) Imp[i-n_age*12] = y[i];  
    for (i=n_age*13; i<n_age*14; i++) PTn[i-n_age*13] = y[i];
    for (i=n_age*14; i<n_age*15; i++) PTp[i-n_age*14] = y[i];
    /* HIV+ */
    ij = n_age*n_disease;  
    for (j=0; j<n_HIV; j++){
      for (i=0; i<n_age; i++){  
        S_H[i][j] = y[ij];
        Lsn_H[i][j] = y[ij+(n_age*n_HIV)];
        Lsp_H[i][j] = y[ij+(2*n_age*n_HIV)];
        Lmn_H[i][j] = y[ij+(3*n_age*n_HIV)];
        Lmp_H[i][j] = y[ij+(4*n_age*n_HIV)];  
        Nsn_H[i][j] = y[ij+(5*n_age*n_HIV)];
        Nsp_H[i][j] = y[ij+(6*n_age*n_HIV)];
        Nmn_H[i][j] = y[ij+(7*n_age*n_HIV)];
        Nmp_H[i][j] = y[ij+(8*n_age*n_HIV)];
        Isn_H[i][j] = y[ij+(9*n_age*n_HIV)];
        Isp_H[i][j] = y[ij+(10*n_age*n_HIV)];
        Imn_H[i][j] = y[ij+(11*n_age*n_HIV)];
        Imp_H[i][j] = y[ij+(12*n_age*n_HIV)];
        PTn_H[i][j] = y[ij+(13*n_age*n_HIV)];
        PTp_H[i][j] = y[ij+(14*n_age*n_HIV)];
        ij = ij+1;
      }
    }
    /* HIV+, on ART */
    ij = (n_HIV+1)*n_age*n_disease;   
    for (l=0; l<n_ART; l++){                          
      for (j=0; j<n_HIV; j++){
        for (i=0; i<n_age; i++){
          S_A[i][j][l] = y[ij];
          Lsn_A[i][j][l] = y[ij+(n_age*n_ART*n_HIV)];
          Lsp_A[i][j][l] = y[ij+(2*n_age*n_ART*n_HIV)];
          Lmn_A[i][j][l] = y[ij+(3*n_age*n_ART*n_HIV)];
          Lmp_A[i][j][l] = y[ij+(4*n_age*n_ART*n_HIV)];  
          Nsn_A[i][j][l] = y[ij+(5*n_age*n_ART*n_HIV)];
          Nsp_A[i][j][l] = y[ij+(6*n_age*n_ART*n_HIV)];
          Nmn_A[i][j][l] = y[ij+(7*n_age*n_ART*n_HIV)];
          Nmp_A[i][j][l] = y[ij+(8*n_age*n_ART*n_HIV)];
          Isn_A[i][j][l] = y[ij+(9*n_age*n_ART*n_HIV)];
          Isp_A[i][j][l] = y[ij+(10*n_age*n_ART*n_HIV)];
          Imn_A[i][j][l] = y[ij+(11*n_age*n_ART*n_HIV)];
          Imp_A[i][j][l] = y[ij+(12*n_age*n_ART*n_HIV)];
          PTn_A[i][j][l] = y[ij+(13*n_age*n_ART*n_HIV)];
          PTp_A[i][j][l] = y[ij+(14*n_age*n_ART*n_HIV)];
          ij = ij+1;
        }
      }
    }
      
    /* Adjust TB model parameters for age, HIV and ART */

    /* Create vectors of disease parameters (by age - now single year bins) to use in derivatives - includes BCG effect on risk of primary disease */
    double a_age[81];
    double sig_age[81];
    double v_age[81];
    double muN_age[81];
    double muI_age[81];
    double bcg = (BCG_cov*(1-BCG_eff)+(1-BCG_cov));  /* those on bcg (BCG_COV) have RR of (1-BCG_eff) */

    for (i=0; i<5; i++){
      a_age[i] = a0*bcg;
      a_age[i+5] = a5*bcg;
      a_age[i+10] = a10*bcg;
      
      sig_age[i] = sig0;
      sig_age[i+5] = sig5;
      sig_age[i+10] = sig10;
      
      v_age[i] = v;
      v_age[i+5] = v;
      v_age[i+10] = v;
      
      muN_age[i] = mu_N0;
      muN_age[i+5] = mu_N5;
      muN_age[i+10] = mu_N10;
      
      muI_age[i] = mu_I0;
      muI_age[i+5] = mu_I5;
      muI_age[i+10] = mu_I10;
    }
    for (i=15; i<81; i++){
      a_age[i] = a_a;
      sig_age[i] = sig_a;
      v_age[i] = v; 
      muN_age[i] = mu_N;
      muI_age[i] = mu_I;
    }
    
    /* Now adjust parameters for HIV and ART */
    /* HIV mortality rates are passed in directly and ART mortality reduction is implemented in the derivatives */

    double mid_CD4[7] = {500,425,300,225,150,75,25};       /* mid points of CD4 categories */
    double ART_TB[3] = {ART_TB1,ART_TB2,ART_TB3};          /* vector of ART relative risks for TB disease */
    double ART_mort[3] = {ART_mort1,ART_mort2,ART_mort3};  /* vector of ART relative risks for TB mortlaity */
    /* then adjust parameters */
    double a_age_H[81][7];
    double p_H[7];
    double v_age_H[81][7];
    double a_age_A[81][7][3];
    double p_A[7][3];
    double v_age_A[81][7][3];
    double muN_H_A[81][3];
    double muI_H_A[81][3];
    for (j=0; j<n_HIV; j++){
      p_H[j] = p*RR1p*pow(RR2p,-1*(500-mid_CD4[j])/100);
      for (i=0; i<n_age; i++){
        a_age_H[i][j] = fmin(a_age[i]*RR1a*pow(RR2a,(500-mid_CD4[j])/100),1); /* fmin ensures proportion developing active disease can't go above 1 - assume this cap is also applied in TIME */
        v_age_H[i][j] = v_age[i]*RR1v*pow(RR2v,(500-mid_CD4[j])/100);
        for (l=0; l<n_ART; l++){
          a_age_A[i][j][l] = fmax(a_age_H[i][j]*(1-ART_TB[l]),a_age[i]);   /* fmax or fmin ensures being on ART can't be better than being HIV- */
          v_age_A[i][j][l] = fmax(v_age_H[i][j]*(1-ART_TB[l]),v_age[i]);
          
          p_A[j][l] = fmin(1-(1-p_H[j])*(1-ART_TB[l]),p);                        /* protection term gets higher as ART is taken */

          muN_H_A[i][l] = fmax(muN_H*(1-ART_mort[l]),muN_age[i]); /* make sure mortality can't go lower than HIV- */ 
          muI_H_A[i][l] = fmax(muI_H*(1-ART_mort[l]),muI_age[i]);
        
        }
      }
    }
  
    /* Set up parameters for HIV model - these are taken from AIM */

    double H_CD4[7][81] = {{0},{0},{0},{0},{0},{0},{0}}; /* Distribution of new HIV infections (age,CD4) - assume distribution of new child infections mirrors adults */
    for (i=0; i<25; i++) {
      H_CD4[0][i] = 0.643; H_CD4[1][i] = 0.357;
    }
    for (i=25; i<35; i++){
      H_CD4[0][i] = 0.607; H_CD4[1][i] = 0.393;
    }
    for (i=35; i<45; i++){
      H_CD4[0][i] = 0.585; H_CD4[1][i] = 0.415;
    }
    for (i=45; i<81; i++){
      H_CD4[0][i] = 0.552; H_CD4[1][i] = 0.448;
    }

    /* Have updated these values based on the durations in the AIM manual (rate = 1/duration) as they are different from rates in AIM editor in software */
    /* those are actually risks (i.e. 1-exp(-rate)) */
    double H_prog[8][81] = {{0},{0},{0},{0},{0},{0},{0},{0}};  /* Progression through CD4 categories (age, CD4) - has extra row to avoid progression in/out of first/last groups */
    for (i=0; i<15; i++){
      H_prog[1][i] = 0.298; H_prog[2][i] = 0.239; H_prog[3][i] = 0.183; H_prog[4][i] = 0.183; H_prog[5][i] = 0.130; H_prog[6][i] = 0.130;
    }
    for (i=15; i<25; i++){
      H_prog[1][i] = 0.117; H_prog[2][i] = 0.223; H_prog[3][i] = 0.294; H_prog[4][i] = 0.508; H_prog[5][i] = 0.214; H_prog[6][i] = 0.348;
    }
    for (i=25; i<35; i++){
      H_prog[1][i] = 0.147; H_prog[2][i] = 0.240; H_prog[3][i] = 0.452; H_prog[4][i] = 1.087; H_prog[5][i] = 0.637; H_prog[6][i] = 1.449;
    }
    for (i=35; i<45; i++){
      H_prog[1][i] = 0.183; H_prog[2][i] = 0.355; H_prog[3][i] = 0.581; H_prog[4][i] = 1.250; H_prog[5][i] = 0.676; H_prog[6][i] = 1.449;
    }
    for (i=45; i<81; i++){
      H_prog[1][i] = 0.213; H_prog[2][i] = 0.535; H_prog[3][i] = 0.855; H_prog[4][i] = 1.818; H_prog[5][i] = 0.952; H_prog[6][i] = 2.000;
    }

    double H_mort[7][81]; /* Mortality due to HIV (no ART) (age, CD4) */
    for (i=0; i<5; i++){
      H_mort[0][i] = 0.312; H_mort[1][i] = 0.382; H_mort[2][i] = 0.466; H_mort[3][i] = 0.466; H_mort[4][i] = 0.569; H_mort[5][i] = 0.569; H_mort[6][i] = 0.569;
    }
    for (i=5; i<15; i++){
      H_mort[0][i] = 0.039; H_mort[1][i] = 0.048; H_mort[2][i] = 0.058; H_mort[3][i] = 0.058; H_mort[4][i] = 0.071; H_mort[5][i] = 0.071; H_mort[6][i] = 0.071;
    }
    for (i=15; i<25; i++){
      H_mort[0][i] = 0.005; H_mort[1][i] = 0.011; H_mort[2][i] = 0.026; H_mort[3][i] = 0.061; H_mort[4][i] = 0.139; H_mort[5][i] = 0.321; H_mort[6][i] = 0.737;
    }    
    for (i=25; i<35; i++){
      H_mort[0][i] = 0.004; H_mort[1][i] = 0.010; H_mort[2][i] = 0.026; H_mort[3][i] = 0.069; H_mort[4][i] = 0.185; H_mort[5][i] = 0.499; H_mort[6][i] = 1.342;
    } 
    for (i=35; i<45; i++){
      H_mort[0][i] = 0.005; H_mort[1][i] = 0.013; H_mort[2][i] = 0.036; H_mort[3][i] = 0.096; H_mort[4][i] = 0.258; H_mort[5][i] = 0.691; H_mort[6][i] = 1.851;
    } 
    for (i=45; i<81; i++){
      H_mort[0][i] = 0.005; H_mort[1][i] = 0.013; H_mort[2][i] = 0.032; H_mort[3][i] = 0.080; H_mort[4][i] = 0.203; H_mort[5][i] = 0.513; H_mort[6][i] = 1.295;
    } 

    double A_mort[3][7][81]; /* On ART mortality (age,starting CD4, time on ART)  - this is an average of male and female values weigthed by sex of those on ART  */
        
    int kl;    
    for (i=0; i<5; i++){
      int kkk = 47;
      for (j=0; j<7; j++){
        for (l=0; l<3; l++){
          for (kl=0; kl<16; kl++){
            A_mort[l][j][i + (kl*5)] = parms[kkk + (kl*21)];   
          }
          kkk++;
        }    
      }
    }
    
    double A_prog[4] = {0,2,2,0}; /* Progression through time on ART, 6 monthly time blocks - 0 ensure no progression into first catergory and no progression out of last category*/
    double A_start[3] = {1,0,0};  /* Used to make sure ART initiations are only added to the fist time on ART box */ 
    
    /* sum up various totals */

    /* Use sumsum function to add up HIV- */
    double Total_S = sumsum(S,0,80);                        /* Total susceptible */
    double Total_Ls = sumsum(Lsn,0,80)+sumsum(Lsp,0,80);    /* Total LTBI with drug susceptible (DS) strain */
    double Total_Lm = sumsum(Lmn,0,80)+sumsum(Lmp,0,80);    /* Total LTBI with drug resistant (DR) strain */
    double Total_Ns_N = sumsum(Nsn,0,80)+sumsum(Nsp,0,80);    /* Total DS smear negative TB */
    double Total_Nm_N = sumsum(Nmn,0,80)+sumsum(Nmp,0,80);    /* Total DR smear negative TB */
    double Total_Is_N = sumsum(Isn,0,80)+sumsum(Isp,0,80);    /* Total DS smear positive TB */
    double Total_Im_N = sumsum(Imn,0,80)+sumsum(Imp,0,80);    /* Total DR smear positive TB */
    double Total_PT = sumsum(PTn,0,80)+sumsum(PTp,0,80);      /* Post PT */
    
    /* Now loop through HIV and ART and add them in */
    double Total_Ns_H =0; double Total_Nm_H = 0; double Total_Is_H = 0; double Total_Im_H =0;
    
    for (j=0; j<n_HIV; j++){
      for (i=0; i<n_age; i++){
        Total_S = Total_S + S_H[i][j];
        Total_Ls = Total_Ls + Lsn_H[i][j]+Lsp_H[i][j];
        Total_Lm = Total_Lm + Lmn_H[i][j]+Lmp_H[i][j];
        Total_Ns_H = Total_Ns_H + Nsn_H[i][j]+Nsp_H[i][j];
        Total_Nm_H = Total_Nm_H + Nmn_H[i][j]+Nmp_H[i][j];
        Total_Is_H = Total_Is_H + Isn_H[i][j]+Isp_H[i][j];
        Total_Im_H = Total_Im_H + Imn_H[i][j]+Imp_H[i][j];
        Total_PT = Total_PT + PTn_H[i][j] + PTp_H[i][j];
        for (l=0; l<n_ART; l++){
          Total_S = Total_S + S_A[i][j][l];
          Total_Ls = Total_Ls + Lsn_A[i][j][l]+Lsp_A[i][j][l];
          Total_Lm = Total_Lm + Lmn_A[i][j][l]+Lmp_A[i][j][l];
          Total_Ns_H = Total_Ns_H + Nsn_A[i][j][l]+Nsp_A[i][j][l];
          Total_Nm_H = Total_Nm_H + Nmn_A[i][j][l]+Nmp_A[i][j][l];
          Total_Is_H = Total_Is_H + Isn_A[i][j][l]+Isp_A[i][j][l];
          Total_Im_H = Total_Im_H + Imn_A[i][j][l]+Imp_A[i][j][l];
          Total_PT = Total_PT + PTn_A[i][j][l] + PTp_A[i][j][l];
        }
      }
    }
    double Total_L = Total_Ls + Total_Lm;           /* Total LTBI */
    double Total_N_N = Total_Ns_N + Total_Nm_N;     /* Total smear negative TB (HIV-) */
    double Total_N_H = Total_Ns_H + Total_Nm_H;     /* Total smear negative TB (HIV+) */
    double Total_N = Total_N_N + Total_N_H;         /* Total smear negative TB */
    double Total_I_N = Total_Is_N + Total_Im_N;     /* Total smear positive TB (HIV-) */
    double Total_I_H = Total_Is_H + Total_Im_H;     /* Total smear positive TB (HIV+) */
    double Total_I = Total_I_N + Total_I_H;         /* Total smear positive TB */
    
    double Total_DS = Total_Ns_N + Total_Ns_H + Total_Is_N + Total_Is_H;    /* Total DS TB */
    double Total_MDR = Total_Nm_N + Total_Nm_H + Total_Im_N + Total_Im_H;   /* Total DR TB */
    double Total = Total_S+Total_L+Total_N+Total_I+Total_PT; /* Total */
    
    /* Mortality calculations and adjustments */
    /* HIV mortality rates include TB deaths */
    /* Background mortality rates include HIV and TB deaths */
    /* Need to make an adjustment to both HIV and background rates to avoid double counting */
    
    /* Calculate deaths due to disease (TB and HIV) by age, CD4 and ART */
    /* work out disease induced mortality rate if pre 2015 - if after 2015 just use 2015 value */
    /* and adjust background mortality rate accordingly */
    /* Calculate total population in the same loop and prevalence of TB in HIV- */
    double TB_deaths_neg[81];
    double TB_deaths_HIV[81][7];
    double TB_deaths_ART[81][7][3];
    double TB_deaths_HIV_age[81] = {0};
    double TB_deaths_ART_age[81] = {0};
    double TB_deaths[81];
    
    double HIV_deaths_HIV[81] = {0}; ;
    double HIV_deaths_ART[81] = {0};

    double up_H_mort[7][81];
    double up_A_mort[3][7][81];
    double m_b[81];
    double rate_dis_death[81];
    
    double tot_age[81] = {0};
    double tot_age_HIV[81][7];
    double tot_age_ART[81][7][3];
    
    /*double Tot_deaths = 0;*/
    double Tot_deaths_age[81];
    double ART_deaths_age[81] = {0};
    double Tot_deaths=0;
    double tot_age_neg[81] = {0};
    
 
    for (i=0; i<n_age; i++) {
      
      /* Calculate HIV- TB deaths */
      TB_deaths_neg[i] = (Nsn[i]+Nsp[i]+Nmn[i]+Nmp[i])*muN_age[i] + (Isn[i]+Isp[i]+Imn[i]+Imp[i])*muI_age[i];
      TB_deaths[i] = TB_deaths_neg[i];
      /* Calculate size of age group */
      tot_age_neg[i] = S[i]+Lsn[i]+Lsp[i]+Lmn[i]+Lmp[i]+Nsn[i]+Nsp[i]+Nmn[i]+Nmp[i]+Isn[i]+Isp[i]+Imn[i]+Imp[i]+PTn[i]+PTp[i];
      tot_age[i] = tot_age[i] + tot_age_neg[i];
      
      for(j=0; j<n_HIV; j++){
        
        /* Calculate HIV+ TB deaths */
        TB_deaths_HIV[i][j] = (Nsn_H[i][j]+Nsp_H[i][j]+Nmn_H[i][j]+Nmp_H[i][j])*muN_H + (Isn_H[i][j]+Isp_H[i][j]+Imn_H[i][j]+Imp_H[i][j])*muI_H;
        TB_deaths_HIV_age[i] = TB_deaths_HIV_age[i] + TB_deaths_HIV[i][j];
        TB_deaths[i] = TB_deaths[i] + TB_deaths_HIV[i][j];
        /* Calculate size of HIV+ age group */                              
        tot_age_HIV[i][j] = S_H[i][j]+Lsn_H[i][j]+Lsp_H[i][j]+Lmn_H[i][j]+Lmp_H[i][j]+Nsn_H[i][j]+Nsp_H[i][j]+Nmn_H[i][j]+Nmp_H[i][j]+Isn_H[i][j]+Isp_H[i][j]+Imn_H[i][j]+Imp_H[i][j]+PTn_H[i][j]+PTp_H[i][j]; 
        /* Update size of age group */
        tot_age[i] = tot_age[i] + tot_age_HIV[i][j];
        /* Adjust HIV mortality probability to remove TB deaths (only if there is any HIV yet (otherwise we get a divide by 0 error)) */
        if (tot_age_HIV[i][j]>0.0){
          up_H_mort[j][i] = fmax(0,(H_mort[j][i] - (TB_deaths_HIV[i][j]/tot_age_HIV[i][j])));
        }
        else{
          up_H_mort[j][i] = H_mort[j][i];
        }
        
        /* Calculate HIV deaths using the updated probabilities */  
        HIV_deaths_HIV[i] = HIV_deaths_HIV[i] + up_H_mort[j][i]*(S_H[i][j]+Lsn_H[i][j]+Lsp_H[i][j]+Lmn_H[i][j]+Lmp_H[i][j]+
                                                                 Nsn_H[i][j]+Nsp_H[i][j]+Nmn_H[i][j]+Nmp_H[i][j]+
                                                                 Isn_H[i][j]+Isp_H[i][j]+Imn_H[i][j]+Imp_H[i][j]+
                                                                 PTn_H[i][j]+PTp_H[i][j]);
      
        for (l=0; l<n_ART; l++){
          
          /* Calculate ART TB deaths */
          TB_deaths_ART[i][j][l] = (Nsn_A[i][j][l]+Nsp_A[i][j][l]+Nmn_A[i][j][l]+Nmp_A[i][j][l])*muN_H_A[i][l] + 
                                   (Isn_A[i][j][l]+Isp_A[i][j][l]+Imn_A[i][j][l]+Imp_A[i][j][l])*muI_H_A[i][l]; 
          TB_deaths_ART_age[i] = TB_deaths_ART_age[i] + TB_deaths_ART[i][j][l];
          TB_deaths[i] = TB_deaths[i] + TB_deaths_ART[i][j][l];
          /* Calculate size of ART age group */
          tot_age_ART[i][j][l] = S_A[i][j][l]+Lsn_A[i][j][l]+Lsp_A[i][j][l]+Lmn_A[i][j][l]+Lmp_A[i][j][l]+Nsn_A[i][j][l]+Nsp_A[i][j][l]+
                                 Nmn_A[i][j][l]+Nmp_A[i][j][l]+Isn_A[i][j][l]+Isp_A[i][j][l]+Imn_A[i][j][l]+Imp_A[i][j][l]+PTn_A[i][j][l]+PTp_A[i][j][l];
          /* Update size of age group */
          tot_age[i] = tot_age[i] + tot_age_ART[i][j][l];
          /* Adjust ART mortality probability to remove TB deaths (only if there is any ART yet (otherwise we get a divide by 0 error)) */
          if (tot_age_ART[i][j][l]>0.0){
            up_A_mort[l][j][i] = fmax(0,A_mort[l][j][i] - (TB_deaths_ART[i][j][l]/tot_age_ART[i][j][l]));
          }
          else{
            up_A_mort[l][j][i] = A_mort[l][j][i];
          }
          
          /* Calculate ART deaths using the updated probabilites */
          HIV_deaths_ART[i] = HIV_deaths_ART[i] + up_A_mort[l][j][i]*(S_A[i][j][l]+Lsn_A[i][j][l]+Lsp_A[i][j][l]+Lmn_A[i][j][l]+Lmp_A[i][j][l]+
                                                                      Nsn_A[i][j][l]+Nsp_A[i][j][l]+Nmn_A[i][j][l]+Nmp_A[i][j][l]+ 
                                                                      Isn_A[i][j][l]+Isp_A[i][j][l]+Imn_A[i][j][l]+Imp_A[i][j][l]+
                                                                      PTn_A[i][j][l]+PTp_A[i][j][l]);    
                                                                      
          /* Add up all deaths on ART - used to put new people on ART */
          ART_deaths_age[i] = ART_deaths_age[i] + TB_deaths_ART[i][j][l] + up_A_mort[l][j][i]*(S_A[i][j][l]+Lsn_A[i][j][l]+Lsp_A[i][j][l]+Lmn_A[i][j][l]+Lmp_A[i][j][l]+
                                                                                 Nsn_A[i][j][l]+Nsp_A[i][j][l]+Nmn_A[i][j][l]+Nmp_A[i][j][l]+ 
                                                                                 Isn_A[i][j][l]+Isp_A[i][j][l]+Imn_A[i][j][l]+Imp_A[i][j][l]+
                                                                                 PTn_A[i][j][l]+PTp_A[i][j][l]);
        }                               
      }

      if (pop_ad>0) rate_dis_death[i] = (TB_deaths_neg[i]+TB_deaths_HIV_age[i]+TB_deaths_ART_age[i]+HIV_deaths_HIV[i]+HIV_deaths_ART[i])/tot_age[i];
      m_b[i] = fmax(0,forc[i+1]-rate_dis_death[i]);
      
      Tot_deaths_age[i] = m_b[i]*tot_age[i] + TB_deaths[i] + HIV_deaths_HIV[i] + HIV_deaths_ART[i];  
      Tot_deaths = Tot_deaths + Tot_deaths_age[i];
      
      /* Add in background deaths on ART - used to calculate new people to put on ART */
      for(j=0; j<n_HIV; j++){
        for (l=0; l<n_ART; l++){
          ART_deaths_age[i] = ART_deaths_age[i] + m_b[i]*(S_A[i][j][l]+Lsn_A[i][j][l]+Lsp_A[i][j][l]+Lmn_A[i][j][l]+Lmp_A[i][j][l]+Nsn_A[i][j][l]+Nsp_A[i][j][l]+
                                                  Nmn_A[i][j][l]+Nmp_A[i][j][l]+Isn_A[i][j][l]+Isp_A[i][j][l]+Imn_A[i][j][l]+Imp_A[i][j][l]+PTn_A[i][j][l]+PTp_A[i][j][l]);
        }
      }
      
    } 
    double TB_deaths_tot = sumsum(TB_deaths,0,80);
    double TB_deaths_neg_tot = sumsum(TB_deaths_neg,0,80);
    double TB_deaths_pos_tot = sumsum(TB_deaths_HIV_age,0,80) + sumsum(TB_deaths_ART_age,0,80);
    
    /* Sum up populations over CD4 categories, with and without ART and calculate rates of ART initiation by age */
    
    double ART_prop[81][7] = {{0}};     /* Proportion of CD4 category who should start ART by age */
    double CD4_dist[81][7] = {{0}};     /* Not on ART by CD4 and age */
    double CD4_dist_ART[81][7] = {{0}}; /* On ART by CD4 and age*/
    double CD4_dist_all[7] = {0};       /* Not on ART by CD4 */
    double CD4_dist_ART_all[7] = {0};   /* On ART by CD4 */
    double CD4_deaths[81][7] = {{0}};   /* Deaths by CD4 (no ART) */
    double ART_new[81] = {0};           /* Number of new people to put on ART by age */
    double ART_el[81] = {0};            /* Number who are eligible but not on ART */
    double ART_el_deaths[81] = {0};     /* Number eligible who will die */
    double ART_on[81] = {0};            /* Number who should be on ART by age */
    double Tot_ART[81] = {0};           /* Number currently on ART by age */
    
    for (i=0; i<n_age; i++){

      iz = iii[i];

      for (j=0; j<n_HIV; j++){
        
        CD4_dist[i][j] = S_H[i][j]+Lsn_H[i][j]+Lsp_H[i][j]+Lmn_H[i][j]+Lmp_H[i][j]+
                         Nsn_H[i][j]+Nsp_H[i][j]+Nmn_H[i][j]+Nmp_H[i][j]+ 
                         Isn_H[i][j]+Isp_H[i][j]+Imn_H[i][j]+Imp_H[i][j]+
                         PTn_H[i][j]+PTp_H[i][j];
                      
        CD4_dist_all[j] = CD4_dist_all[j] + CD4_dist[i][j];
        
        CD4_deaths[i][j] = H_mort[j][i]*(S_H[i][j]+Lsn_H[i][j]+Lsp_H[i][j]+Lmn_H[i][j]+Lmp_H[i][j]+
                                         Nsn_H[i][j]+Nsp_H[i][j]+Nmn_H[i][j]+Nmp_H[i][j]+ 
                                         Isn_H[i][j]+Isp_H[i][j]+Imn_H[i][j]+Imp_H[i][j]+
                                         PTn_H[i][j]+PTp_H[i][j]);
                                                          
                                                          
        for (l=0; l<n_ART; l++){
          
          CD4_dist_ART[i][j] = CD4_dist_ART[i][j]+S_A[i][j][l]+Lsn_A[i][j][l]+Lsp_A[i][j][l]+Lmn_A[i][j][l]+Lmp_A[i][j][l]+
                                                  Nsn_A[i][j][l]+Nsp_A[i][j][l]+Nmn_A[i][j][l]+Nmp_A[i][j][l]+ 
                                                  Isn_A[i][j][l]+Isp_A[i][j][l]+Imn_A[i][j][l]+Imp_A[i][j][l]+
                                                  PTn_A[i][j][l]+PTp_A[i][j][l];                
        
        } 
        
        CD4_dist_ART_all[j] = CD4_dist_ART_all[j] + CD4_dist_ART[i][j];
        
        Tot_ART[i] = Tot_ART[i] + CD4_dist_ART[i][j];  /* sum up number currently on ART */
         

         
        if (j>=Athresh) { /* If this CD4 is eligible for ART */
          ART_on[i] = ART_on[i] + (CD4_dist[i][j] + CD4_dist_ART[i][j])*forc[iz+146]; /* number who should be on ART - HIV+ population times coverage (by CD4) */
        }

      }
      ART_new[i] = fmax(0,ART_on[i] - (Tot_ART[i] - ART_deaths_age[i]));   /* number who need to start is number who should be on minus those already on plus those on ART who will die in current time */ 
        
      /* Then work out where these should go by CD4 - based on proportion of eligible population in CD4 group and proportion of deaths which occur in CD4 group */
      for (j=Athresh; j<n_HIV; j++) {
        ART_el[i] = ART_el[i] + CD4_dist[i][j];
        ART_el_deaths[i] = ART_el_deaths[i] + CD4_deaths[i][j];
      }
      
      /* check that number to be put on isn't greater than number eligbile */
      ART_new[i] = fmin(ART_el[i],ART_new[i]);
      
      if (ART_el[i] > 0){
        for (j=Athresh; j<n_HIV; j++) {
          if (CD4_dist[i][j] > 0) {
            
            ART_prop[i][j] = (((CD4_dist[i][j]/ART_el[i])+(CD4_deaths[i][j]/ART_el_deaths[i]))/2)*(ART_new[i]/CD4_dist[i][j]); /* applies weighting and size of CD4 group to work out % of CD4 group that should move */
            
          }
        }
      }
    }

    /* Total on or starting ART - if zero use it to skip running ART derivs */
    double ART_all = sumsum(Tot_ART,0,80) + sumsum(ART_new,0,80);

    /* Force of infection */
    double FS = beta*(Total_Ns_N*rel_inf + Total_Ns_H*rel_inf_H + Total_Is_N + Total_Is_H)/Total; 
    double FM = fit_cost*beta*(Total_Nm_N*rel_inf + Total_Nm_H*rel_inf_H + Total_Im_N + Total_Im_H)/Total; 
    
    /* Variables to store numbers of new cases */
    double TB_cases_age[81] = {0};
    double TB_cases_neg_age[81];
    double TB_cases_neg = 0;
    double TB_cases_pos_age[81][7];
    double TB_cases_pos = 0;
    double TB_cases_ART_age[81][7][3];
    double TB_cases_ART = 0;
        
    /* Derivatives */ 
 
    /* HIV-: loop through ages*/ 
    
    double births = birth_rate*Total/1000;

    for (i=0; i<n_age; i++){
      
      iz = iii[i];
      
      /* Calculate the disease flows here and use these in the derivatives - intention is to make the model more flexible/easier to understand */
      
      double S_to_Lsn = FS*(1-a_age[i])*S[i];                                     /* Susceptible to latent DS infection (no disease history) */
      double S_to_Nsn = FS*a_age[i]*(1-sig_age[i])*S[i];                          /* Susceptible to primary DS smear negative disease (no disease history) */
      double S_to_Isn = FS*a_age[i]*sig_age[i]*S[i];                              /* Susceptible to primary DS smear positive disease (no disease history) */
      double S_to_Lmn = FM*(1-a_age[i])*S[i];                                     /* Susceptible to latent DR infection (no disease history) */
      double S_to_Nmn = FM*a_age[i]*(1-sig_age[i])*S[i];                          /* Susceptible to primary DR smear negative disease (no disease history) */
      double S_to_Imn = FM*a_age[i]*sig_age[i]*S[i];                              /* Susceptible to primary DR smear positive disease (no disease history) */
      
      double Lsn_to_Nsn = (v_age[i] + FS*a_age[i]*(1-p))*(1-sig_age[i])*Lsn[i];   /* Latent DS to smear negative DS disease (no disease history) - reactivation and reinfection */ 
      double Lsn_to_Isn = (v_age[i] + FS*a_age[i]*(1-p))*sig_age[i]*Lsn[i];       /* Latent DS to smear positive DS disease (no disease history) - reactivation and reinfection */ 
      double Lsn_to_Nmn = FM*a_age[i]*(1-p)*(1-sig_age[i])*Lsn[i];                /* Latent DS to smear negative DR disease (no disease history) - co-infection */ 
      double Lsn_to_Imn = FM*a_age[i]*(1-p)*sig_age[i]*Lsn[i];                    /* Latent DS to smear positive DR disease (no disease history) - co-infection */
      double Lsn_to_Lmn = FM*(1-a_age[i])*(1-p)*g*Lsn[i];                         /* Latent DS to latent DR (no disease history) */
      
      double Lmn_to_Nmn = (v_age[i] + FM*a_age[i]*(1-p))*(1-sig_age[i])*Lmn[i];   /* Latent DR to smear negative DR disease (no disease history) - reactivation and reinfection */ 
      double Lmn_to_Imn = (v_age[i] + FM*a_age[i]*(1-p))*sig_age[i]*Lmn[i];       /* Latent DR to smear positive DR disease (no disease history) - reactivation and reinfection */ 
      double Lmn_to_Nsn = FS*a_age[i]*(1-sig_age[i])*(1-p)*Lmn[i];                /* Latent DR to smear negative DS disease (no disease history) - co-infection */
      double Lmn_to_Isn = FS*a_age[i]*sig_age[i]*(1-p)*Lmn[i];                    /* Latent DR to smear positive DS disease (no disease history) - co-infection */
      double Lmn_to_Lsn = FS*(1-a_age[i])*(1-p)*(1-g)*Lmn[i];                     /* Latent DR to latent DS (no disease history) */

      double Lsp_to_Nsp = (v_age[i] + FS*a_age[i]*(1-p))*(1-sig_age[i])*Lsp[i];   /* Latent DS to smear negative DS disease (prior Rx) - reactivation and reinfection */ 
      double Lsp_to_Isp = (v_age[i] + FS*a_age[i]*(1-p))*sig_age[i]*Lsp[i];       /* Latent DS to smear positive DS disease (prior Rx) - reactivation and reinfection */     
      double Lsp_to_Nmp = FM*a_age[i]*(1-p)*(1-sig_age[i])*Lsp[i];                /* Latent DS to smear negative DR disease (prior_rx) - reactivation and reinfection */ 
      double Lsp_to_Imp = FM*a_age[i]*(1-p)*sig_age[i]*Lsp[i];                    /* Latent DS to smear positive DR disease (prior_Rx) - reactivation and reinfection */
      double Lsp_to_Lmp = FM*(1-a_age[i])*(1-p)*g*Lsp[i];                         /* Latent DS to latent DR (prior Rx) */
      
      double Lmp_to_Nmp = (v_age[i] + FM*a_age[i]*(1-p))*(1-sig_age[i])*Lmp[i];   /* Latent DR to smear negative DR disease (prior Rx) - reactivation and reinfection */ 
      double Lmp_to_Imp = (v_age[i] + FM*a_age[i]*(1-p))*sig_age[i]*Lmp[i];       /* Latent DR to smear positive DR disease (prior Rx) - reactivation and reinfection */     
      double Lmp_to_Nsp = FS*a_age[i]*(1-p)*(1-sig_age[i])*Lmp[i];                /* Latent DR to smear negative DS disease (prior_rx) - reactivation and reinfection */ 
      double Lmp_to_Isp = FS*a_age[i]*(1-p)*sig_age[i]*Lmp[i];                    /* Latent DR to smear positive DS disease (prior_Rx) - reactivation and reinfection */
      double Lmp_to_Lsp = FS*(1-a_age[i])*(1-p)*(1-g)*Lmp[i];                     /* Latent DR to latent DS (prior Rx) */
      
      double PTn_to_Lsn = FS*(1-a_age[i])*(1-p)*PTn[i];                           /* Post PT to latent DS (no disease history) */
      double PTn_to_Nsn = FS*a_age[i]*(1-p)*(1-sig_age[i])*PTn[i];                /* Post PT to smear negative DS disease (no disease history) */
      double PTn_to_Isn = FS*a_age[i]*(1-p)*sig_age[i]*PTn[i];                    /* Post PT to smear positive DS disease (no disease history) */
      double PTn_to_Lmn = FM*(1-a_age[i])*(1-p)*g*PTn[i];                         /* Post PT to latent DR (no disease history) */
      double PTn_to_Nmn = FM*a_age[i]*(1-p)*(1-sig_age[i])*PTn[i];                /* Post PT to smear negative DR disease (no disease history) */
      double PTn_to_Imn = FM*a_age[i]*(1-p)*sig_age[i]*PTn[i];                    /* Post PT to smear positive DR disease (no disease history) */
      
      double PTp_to_Lsp = FS*(1-a_age[i])*(1-p)*PTp[i];                           /* Post PT to latent DS (prior Rx) */
      double PTp_to_Nsp = FS*a_age[i]*(1-p)*(1-sig_age[i])*PTp[i];                /* Post PT to smear negative DS disease (prior Rx) */
      double PTp_to_Isp = FS*a_age[i]*(1-p)*sig_age[i]*PTp[i];                    /* Post PT to smear positive DS disease (prior Rx) */
      double PTp_to_Lmp = FM*(1-a_age[i])*(1-p)*g*PTp[i];                         /* Post PT to latent DR (prior Rx) */
      double PTp_to_Nmp = FM*a_age[i]*(1-p)*(1-sig_age[i])*PTp[i];                /* Post PT to smear negative DR disease (prior Rx) */
      double PTp_to_Imp = FM*a_age[i]*(1-p)*sig_age[i]*PTp[i];                    /* Post PT to smear positive DR disease (prior Rx) */
      
      /* Calculate "care" flows here and use these in the derivatives */
      
      double false_pos = health*kneg*(1-sp_I_neg*sp_N_neg)*l_s;   /* Susceptible, latently infected and post-PT are false pos notif at this rate (includes link to Rx). */
                                                                  /* This only has any effect in those latently infected with drug sus strains. tneg_s complete Rx and move to PTn/PTp */
      /* sm-, drug sus, no Rx history */    
      double Nsn_pos = kneg*se_N_neg*rel_d*Nsn[i];            /* TB positive - move all out of Nsn */
      double Nsn_dst = Nsn_pos*dstneg_n;                      /* Get DST */
      double Nsn_dst_fpos = Nsn_dst*(1-sp_m_neg);             /* False pos on DST */
      double Nsn_first = l_s*(Nsn_pos - Nsn_dst_fpos);        /* Start first line Rx (correct) */     
      double Nsn_second = l_m*(Nsn_dst_fpos);                 /* Start second line treatment (incorrect) */
      double Nsn_lost = Nsn_pos - Nsn_first - Nsn_second;     /* Positive cases lost to follow up  - go to Nsn */ 
      double Nsn_res = Nsn_first*e;                           /* Develop resistance - go to Nmp */
      double Nsn_first_success = (Nsn_first-Nsn_res)*tneg_s;  /* First line success - go to Lsp */
      double Nsn_first_fail = (Nsn_first-Nsn_res)*(1-tneg_s); /* First line failure - go to Nsp */
      double Nsn_second_success = Nsn_second*tneg_m;          /* Second line success - go to Lsp */
      double Nsn_second_fail = Nsn_second*(1-tneg_m);         /* Second line failure - go to Nsp */

      /* sm-, drug sus, previous Rx history */    
      double Nsp_pos = kneg*se_N_neg*rel_d*Nsp[i];            /* TB positive - move all out of Nsp */
      double Nsp_dst = Nsp_pos*dstneg_p;                      /* Get DST */
      double Nsp_dst_fpos = Nsp_dst*(1-sp_m_neg);             /* False pos on DST */
      double Nsp_first = l_s*(Nsp_pos - Nsp_dst_fpos);        /* Start first line Rx (correct) */     
      double Nsp_second = l_m*(Nsp_dst_fpos);                 /* Start second line treatment (incorrect) */
      double Nsp_lost = Nsp_pos - Nsp_first - Nsp_second;     /* Positive cases lost to follow up  - go to Nsp */ 
      double Nsp_res = Nsp_first*e;                           /* Develop resistance - go to Nmp */
      double Nsp_first_success = (Nsp_first-Nsp_res)*tneg_s;  /* First line success - go to Lsp */
      double Nsp_first_fail = (Nsp_first-Nsp_res)*(1-tneg_s); /* First line failure - go to Nsp */
      double Nsp_second_success = Nsp_second*tneg_m;          /* Second line success - go to Lsp */
      double Nsp_second_fail = Nsp_second*(1-tneg_m);         /* Second line failure - go to Nsp */

      /* sm+, drug sus, no Rx history */    
      double Isn_pos = kneg*se_I_neg*Isn[i];                  /* TB positive - move all out of Isn */
      double Isn_dst = Isn_pos*dstneg_n;                      /* Get DST */
      double Isn_dst_fpos = Isn_dst*(1-sp_m_neg);             /* False pos on DST */
      double Isn_first = l_s*(Isn_pos - Isn_dst_fpos);        /* Start first line Rx (correct) */     
      double Isn_second = l_m*(Isn_dst_fpos);                 /* Start second line treatment (incorrect) */
      double Isn_lost = Isn_pos - Isn_first - Isn_second;     /* Positive cases lost to follow up  - go to Isn */ 
      double Isn_res = Isn_first*e;                           /* Develop resistance - go to Imp */
      double Isn_first_success = (Isn_first-Isn_res)*tneg_s;  /* First line success - go to Lsp */
      double Isn_first_fail = (Isn_first-Isn_res)*(1-tneg_s); /* First line failure - go to Isp */
      double Isn_second_success = Isn_second*tneg_m;          /* Second line success - go to Lsp */
      double Isn_second_fail = Isn_second*(1-tneg_m);         /* Second line failure - go to Isp */

      /* sm+, drug sus, previous Rx history */    
      double Isp_pos = kneg*se_I_neg*Isp[i];                  /* TB positive - move all out of Isp */
      double Isp_dst = Isp_pos*dstneg_p;                      /* Get DST */
      double Isp_dst_fpos = Isp_dst*(1-sp_m_neg);             /* False pos on DST */
      double Isp_first = l_s*(Isp_pos - Isp_dst_fpos);        /* Start first line Rx (correct) */     
      double Isp_second = l_m*(Isp_dst_fpos);                 /* Start second line Rx (incorrect) */
      double Isp_lost = Isp_pos - Isp_first - Isp_second;     /* Positive cases lost to follow up  - go to Isp */ 
      double Isp_res = Isp_first*e;                           /* Develop resistance - go to Imp */
      double Isp_first_success = (Isp_first-Isp_res)*tneg_s;  /* First line success - go to Lsp */
      double Isp_first_fail = (Isp_first-Isp_res)*(1-tneg_s); /* First line failure - go to Isp */
      double Isp_second_success = Isp_second*tneg_m;          /* Second line success - go to Lsp */
      double Isp_second_fail = Isp_second*(1-tneg_m);         /* Second line failure - go to Isp */

      /* sm-, MDR, no Rx history */
      double Nmn_pos = kneg*se_N_neg*rel_d*Nmn[i];            /* TB positive - move all out of Nmn */
      double Nmn_dst = Nmn_pos*dstneg_n;                      /* Get DST */
      double Nmn_dst_pos = Nmn_dst*se_m_neg;                  /* True pos on DST */
      double Nmn_first = l_s*(Nmn_pos - Nmn_dst_pos);         /* Start first line Rx (incorrect) */  
      double Nmn_second = l_m*Nmn_dst_pos;                    /* Start second line Rx (correct) */
      double Nmn_lost = Nmn_pos - Nmn_first - Nmn_second;     /* Positive cases lost to follow up - go to Nmn */
      double Nmn_first_success = Nmn_first*tneg_s*eff_n;      /* First line success - go to Lmp */
      double Nmn_first_fail = Nmn_first - Nmn_first_success;  /* First line failure - go to Nmp */
      double Nmn_second_success = Nmn_second*tneg_m;          /* Second line success - go to Lmp */
      double Nmn_second_fail = Nmn_second*(1-tneg_m);         /* Second line failure - go to Nmp */

      /* sm-, MDR, previous Rx history */
      double Nmp_pos = kneg*se_N_neg*rel_d*Nmp[i];            /* TB positive - move all out of Nmp */
      double Nmp_dst = Nmp_pos*dstneg_p;                      /* Get DST */
      double Nmp_dst_pos = Nmp_dst*se_m_neg;                  /* True pos on DST */
      double Nmp_first = l_s*(Nmp_pos - Nmp_dst_pos);         /* Start first line Rx (incorrect) */  
      double Nmp_second = l_m*Nmp_dst_pos;                    /* Start second line Rx (correct) */
      double Nmp_lost = Nmp_pos - Nmp_first - Nmp_second;     /* Positive cases lost to follow up - go to Nmp */
      double Nmp_first_success = Nmp_first*tneg_s*eff_p;      /* First line success - go to Lmp */
      double Nmp_first_fail = Nmp_first - Nmp_first_success;  /* First line failure - go to Nmp */
      double Nmp_second_success = Nmp_second*tneg_m;          /* Second line success - go to Lmp */
      double Nmp_second_fail = Nmp_second*(1-tneg_m);         /* Second line failure - go to Nmp */

      /* sm+, MDR, no Rx history */
      double Imn_pos = kneg*se_I_neg*Imn[i];                  /* TB positive - move all out of Imn */
      double Imn_dst = Imn_pos*dstneg_n;                      /* Get DST */
      double Imn_dst_pos = Imn_dst*se_m_neg;                  /* True pos on DST */
      double Imn_first = l_s*(Imn_pos - Imn_dst_pos);         /* Start first line Rx (incorrect) */  
      double Imn_second = l_m*Imn_dst_pos;                    /* Start second line Rx (correct) */
      double Imn_lost = Imn_pos - Imn_first - Imn_second;     /* Positive cases lost to follow up - go to Imn */
      double Imn_first_success = Imn_first*tneg_s*eff_n;      /* First line success - go to Lmp */
      double Imn_first_fail = Imn_first - Imn_first_success;  /* First line failure - go to Imp */
      double Imn_second_success = Imn_second*tneg_m;          /* Second line success - go to Lmp */
      double Imn_second_fail = Imn_second*(1-tneg_m);         /* Second line failure - go to Imp */

      /* sm+, MDR, previous Rx history */
      double Imp_pos = kneg*se_I_neg*Imp[i];                  /* TB positive - move all out of Imp */
      double Imp_dst = Imp_pos*dstneg_p;                      /* Get DST */
      double Imp_dst_pos = Imp_dst*se_m_neg;                  /* True pos on DST */
      double Imp_first = l_s*(Imp_pos - Imp_dst_pos);         /* Start first line Rx (incorrect) */  
      double Imp_second = l_m*Imp_dst_pos;                    /* Start second line Rx (correct) */
      double Imp_lost = Imp_pos - Imp_first - Imp_second;     /* Positive cases lost to follow up - go to Imp */
      double Imp_first_success = Imp_first*tneg_s*eff_p;      /* First line success - go to Lmp */
      double Imp_first_fail = Imp_first - Imp_first_success;  /* First line failure - go to Imp */
      double Imp_second_success = Imp_second*tneg_m;          /* Second line success - go to Lmp */
      double Imp_second_fail = Imp_second*(1-tneg_m);         /* Second line failure - go to Imp */


      /* Susceptible - NOTE BIRTHS ARE ADDED TO HERE IN THE EVENTS FUNCTION*/
      dS[i] = - (FS + FM)*S[i] - /* Infection */ 
              forc[iz+82]*S[i] - m_b[i]*S[i] + (S[i]/tot_age[i])*(forc[iz+116]/5); /* HIV, death, migration */
       
      /* Latent, ds, naive */
      dLsn[i] = - m_b[i]*Lsn[i] + /* Death */
                S_to_Lsn + Lmn_to_Lsn + PTn_to_Lsn- Lsn_to_Lmn - Lsn_to_Nsn - Lsn_to_Isn  - Lsn_to_Nmn - Lsn_to_Imn - /* Infection and disease */
                forc[iz+82]*Lsn[i] + (Lsn[i]/tot_age[i])*(forc[iz+116]/5) + r*(Isn[i] + Nsn[i])  - /* HIV, migration, self-cure */
                false_pos*tneg_s*Lsn[i]; /* False positive Rx */                   
      
      /* Latent, ds, prev */
      dLsp[i] = - m_b[i]*Lsp[i] +  /* Death */ 
                Lmp_to_Lsp + PTp_to_Lsp - Lsp_to_Lmp - Lsp_to_Nsp - Lsp_to_Isp  - Lsp_to_Nmp - Lsp_to_Imp + /* Infection and disease */   
                Nsn_first_success + Nsn_second_success + Nsp_first_success + Nsp_second_success + Isn_first_success + Isn_second_success + Isp_first_success + Isp_second_success - /* Rx */
                forc[iz+82]*Lsp[i] + (Lsp[i]/tot_age[i])*(forc[iz+116]/5) + r*(Isp[i] + Nsp[i]) - /* HIV, migration, self-cure */
                false_pos*tneg_s*Lsp[i]; /* False positive Rx */         

      /* Latent, mdr, naive */ 
      dLmn[i] = - m_b[i]*Lmn[i] + /* Death */
                S_to_Lmn + Lsn_to_Lmn + PTn_to_Lmn - Lmn_to_Lsn - Lmn_to_Nsn - Lmn_to_Isn - Lmn_to_Nmn - Lmn_to_Imn - /* Infection and disease */              
                forc[iz+82]*Lmn[i] + (Lmn[i]/tot_age[i])*(forc[iz+116]/5) + r*(Imn[i] + Nmn[i]); /* HIV, migration, self-cure */            
      
      /* Latent, mdr, prev */
      dLmp[i] = - m_b[i]*Lmp[i] + /* Death */
                Lsp_to_Lmp + PTp_to_Lmp - Lmp_to_Lsp - Lmp_to_Nsp - Lmp_to_Isp - Lmp_to_Nmp - Lmp_to_Imp + /* Infection and disease */
                Nmn_first_success + Nmn_second_success + Nmp_first_success + Nmp_second_success + Imn_first_success + Imn_second_success + Imp_first_success + Imp_second_success - /* Rx */
                forc[iz+82]*Lmp[i] + (Lmp[i]/tot_age[i])*(forc[iz+116]/5)  + r*(Imp[i] + Nmp[i]); /* HIV, migration, self-cure */  
                
      /* Smear neg, ds, new */
      dNsn[i] = - m_b[i]*Nsn[i] + /* Death */ 
                S_to_Nsn + Lsn_to_Nsn + Lmn_to_Nsn + PTn_to_Nsn - /* Disease */
                Nsn_pos + Nsn_lost - /* Diagnosis, pre Rx lost */ 
                forc[iz+82]*Nsn[i] + (Nsn[i]/tot_age[i])*(forc[iz+116]/5)  - (theta + r + muN_age[i])*Nsn[i]; /* HIV, migration, sm conversion, self-cure, TB death */ 
      
      /* Smear neg, ds, prev */                             
      dNsp[i] = - m_b[i]*Nsp[i] + /* Death */
                Lsp_to_Nsp + Lmp_to_Nsp + PTp_to_Nsp - /* Disease */
                Nsp_pos + Nsp_lost + Nsn_first_fail + Nsn_second_fail + Nsp_first_fail + Nsp_second_fail - /* Diagnosis, pre Rx lost, failed Rx */
                forc[iz+82]*Nsp[i] + (Nsp[i]/tot_age[i])*(forc[iz+116]/5) - (theta + r + muN_age[i])*Nsp[i]; /* HIV, migration, sm conversion, self-cure, TB death */  

      /* Smear neg, mdr, new */
      dNmn[i] = - m_b[i]*Nmn[i] + /* Death */ 
                S_to_Nmn + Lsn_to_Nmn + Lmn_to_Nmn + PTn_to_Nmn - /* Disease */
                Nmn_pos + Nmn_lost - /* Diagnosis, pre Rx lost */
                forc[iz+82]*Nmn[i] + (Nmn[i]/tot_age[i])*(forc[iz+116]/5)  - (theta + r + muN_age[i])*Nmn[i]; /* HIV, migration, sm conversion, self-cure, TB death */    
                   
      /* Smear neg, mdr, prev */
      dNmp[i] = - m_b[i]*Nmp[i] + /* Death */
                Lsp_to_Nmp + Lmp_to_Nmp + PTp_to_Nmp  - /* Disease */
                Nmp_pos + Nmp_lost + Nmn_first_fail + Nmn_second_fail + Nmp_first_fail + Nmp_second_fail + Nsn_res + Nsp_res - /* Diagnosis, pre Rx lost, failed Rx, acquired resistance */
                forc[iz+82]*Nmp[i] + (Nmp[i]/tot_age[i])*(forc[iz+116]/5) - (theta + r + muN_age[i])*Nmp[i];  /* HIV, migration, sm conversion, self-cure, TB death */

      /* Smear pos, ds, new */
      dIsn[i] = - m_b[i]*Isn[i] + /* Death */
                S_to_Isn + Lsn_to_Isn + Lmn_to_Isn + PTn_to_Isn - /* Disease */
                Isn_pos + Isn_lost - /* Diagnosis, pre Rx lost */  
                forc[iz+82]*Isn[i] + (Isn[i]/tot_age[i])*(forc[iz+116]/5) + theta*Nsn[i] - (r + muI_age[i])*Isn[i]; /* HIV, migration, sm conversion, self-cure, TB death */  
      
      /* Smear pos, ds, prev */
      dIsp[i] = - m_b[i]*Isp[i] + /* Death */
                Lsp_to_Isp + Lmp_to_Isp + PTp_to_Isp - /* Disease */   
                Isp_pos + Isp_lost + Isn_first_fail + Isn_second_fail + Isp_first_fail + Isp_second_fail - /* Diagnosis, pre Rx lost, failed Rx */
                forc[iz+82]*Isp[i] + (Isp[i]/tot_age[i])*(forc[iz+116]/5) + theta*Nsp[i] - (r + muI_age[i])*Isp[i]; /* HIV, migration, sm conversion, self-cure, TB death */

      /* Smear pos, mdr, new */
      dImn[i] = - m_b[i]*Imn[i] + /* Death */
                S_to_Imn + Lsn_to_Imn + Lmn_to_Imn + PTn_to_Imn - /* Disease */
                Imn_pos + Imn_lost - /* Diagnosis, pre Rx lost, failed Rx */ 
                forc[iz+82]*Imn[i] + (Imn[i]/tot_age[i])*(forc[iz+116]/5)  + theta*Nmn[i] - (r + muI_age[i])*Imn[i]; /* HIV, migration, sm conversion, self-cure, TB death */  
                
      /* Smear pos, mdr, prev */
      dImp[i] = - m_b[i]*Imp[i] /* Death */
                + Lsp_to_Imp + Lmp_to_Imp + PTp_to_Imp - /* Disease */
                Imp_pos + Imp_lost + Imn_first_fail + Imn_second_fail + Imp_first_fail + Imp_second_fail + Isn_res + Isp_res - /* Diagnosis, pre Rx lost, failed Rx, acquired resistance */
                forc[iz+82]*Imp[i] + (Imp[i]/tot_age[i])*(forc[iz+116]/5) + theta*Nmp[i] - (r + muI_age[i])*Imp[i]; /* HIV, migration, sm conversion, self-cure, TB death */ 
                
      /* Post PT, ds, new */          
      dPTn[i] = - m_b[i]*PTn[i] - /* Death */
                PTn_to_Lsn - PTn_to_Nsn - PTn_to_Isn - PTn_to_Lmn - PTn_to_Nmn - PTn_to_Imn + /* Infection and disease */
                false_pos*tneg_s*Lsn[i] - /* Incorrect Rx for latent infected */
                forc[iz+82]*PTn[i] + (PTn[i]/tot_age[i])*(forc[iz+116]/5); /* HIV, migration */
      
      /* Post PT, ds, prev */ 
      dPTp[i] = - m_b[i]*PTp[i] - /* Death */
                PTp_to_Lsp - PTp_to_Nsp - PTp_to_Isp - PTp_to_Lmp - PTp_to_Nmp - PTp_to_Imp + /* Infection and disease */
                false_pos*tneg_s*Lsp[i] - /* Incorrect Rx for latent infected */
                forc[iz+82]*PTp[i] + (PTp[i]/tot_age[i])*(forc[iz+116]/5); /* HIV, migration */
                        
      /* sum up new HIV- cases */           
      TB_cases_neg_age[i] = (v_age[i]*(1-sig_age[i]) + FS*a_age[i]*(1-p)*(1-sig_age[i]))*Lsn[i] + FS*a_age[i]*(1-sig_age[i])*(S[i] + (1-p)*(Lmn[i] + PTn[i])) + /*sneg,sus,new*/
                            (v_age[i]*(1-sig_age[i]) + FS*a_age[i]*(1-p)*(1-sig_age[i]))*Lsp[i] + FS*a_age[i]*(1-sig_age[i])*(1-p)*(Lmp[i] + PTp[i]) +          /*sneg,sus,prev*/
                            (v_age[i]*(1-sig_age[i]) + FM*a_age[i]*(1-p)*(1-sig_age[i]))*Lmn[i] + FM*a_age[i]*(1-sig_age[i])*(S[i] + (1-p)*(Lsn[i] + PTn[i])) + /*sneg,mdr,new*/
                            (v_age[i]*(1-sig_age[i]) + FM*a_age[i]*(1-p)*(1-sig_age[i]))*Lmp[i] + FM*a_age[i]*(1-sig_age[i])*(1-p)*(Lsp[i] + PTp[i]) +          /*sneg,mdr,prev*/
                            (v_age[i]*sig_age[i] + FS*a_age[i]*sig_age[i]*(1-p))*Lsn[i] + FS*a_age[i]*sig_age[i]*(S[i] + (1-p)*(Lmn[i] + PTn[i])) +             /*spos,sus,new*/
                            (v_age[i]*sig_age[i] + FS*a_age[i]*sig_age[i]*(1-p))*Lsp[i] + FS*a_age[i]*sig_age[i]*(1-p)*(Lmp[i] + PTp[i]) +                      /*spos,sus,prev*/
                            (v_age[i]*sig_age[i] + FM*a_age[i]*sig_age[i]*(1-p))*Lmn[i] + FM*a_age[i]*sig_age[i]*(S[i] + (1-p)*(Lsn[i] + PTn[i]))  +            /*spos,mdr,new*/
                            (v_age[i]*sig_age[i] + FM*a_age[i]*sig_age[i]*(1-p))*Lmp[i] + FM*a_age[i]*sig_age[i]*(1-p)*(Lsp[i] + PTp[i]);                       /*spos,mdr,prev*/
                            
      TB_cases_neg = TB_cases_neg + TB_cases_neg_age[i];
    
      TB_cases_age[i] = TB_cases_age[i] + TB_cases_neg_age[i];
    
        /* HIV+: Loop through CD4 categories */
    
        if (HIV_run>0.0){   /* don't run these for equilibrium as no HIV - save time */
    
        for (j=0; j<n_HIV; j++){      /* CD4 */

      /* Calculate the disease flows here and use these in the derivatives - intention is to make the model more flexible/easier to understand */
      
          double SH_to_LsnH = FS*(1-a_age_H[i][j])*S_H[i][j];                                /* Susceptible to latent DS infection (no disease history) */
          double SH_to_NsnH = FS*a_age_H[i][j]*(1-sig_H)*S_H[i][j];                          /* Susceptible to primary DS smear negative disease (no disease history) */
          double SH_to_IsnH = FS*a_age_H[i][j]*sig_H*S_H[i][j];                              /* Susceptible to primary DS smear positive disease (no disease history) */
          double SH_to_LmnH = FM*(1-a_age_H[i][j])*S_H[i][j];                                /* Susceptible to latent DR infection (no disease history) */
          double SH_to_NmnH = FM*a_age_H[i][j]*(1-sig_H)*S_H[i][j];                          /* Susceptible to primary DR smear negative disease (no disease history) */
          double SH_to_ImnH = FM*a_age_H[i][j]*sig_H*S_H[i][j];                              /* Susceptible to primary DR smear positive disease (no disease history) */
      
          double LsnH_to_NsnH = (v_age_H[i][j] + FS*a_age_H[i][j]*(1-p_H[j]))*(1-sig_H)*Lsn_H[i][j];   /* Latent DS to smear negative DS disease (no disease history) - reactivation and reinfection */ 
          double LsnH_to_IsnH = (v_age_H[i][j] + FS*a_age_H[i][j]*(1-p_H[j]))*sig_H*Lsn_H[i][j];       /* Latent DS to smear positive DS disease (no disease history) - reactivation and reinfection */ 
          double LsnH_to_NmnH = FM*a_age_H[i][j]*(1-p_H[j])*(1-sig_H)*Lsn_H[i][j];                     /* Latent DS to smear negative DR disease (no disease history) - co-infection */ 
          double LsnH_to_ImnH = FM*a_age_H[i][j]*(1-p_H[j])*sig_H*Lsn_H[i][j];                         /* Latent DS to smear positive DR disease (no disease history) - co-infection */
          double LsnH_to_LmnH = FM*(1-a_age_H[i][j])*(1-p_H[j])*g*Lsn_H[i][j];                         /* Latent DS to latent DR (no disease history) */
      
          double LmnH_to_NmnH = (v_age_H[i][j] + FM*a_age_H[i][j]*(1-p_H[j]))*(1-sig_H)*Lmn_H[i][j];   /* Latent DR to smear negative DR disease (no disease history) - reactivation and reinfection */ 
          double LmnH_to_ImnH = (v_age_H[i][j] + FM*a_age_H[i][j]*(1-p_H[j]))*sig_H*Lmn_H[i][j];       /* Latent DR to smear positive DR disease (no disease history) - reactivation and reinfection */ 
          double LmnH_to_NsnH = FS*a_age_H[i][j]*(1-sig_H)*(1-p_H[j])*Lmn_H[i][j];                     /* Latent DR to smear negative DS disease (no disease history) - co-infection */
          double LmnH_to_IsnH = FS*a_age_H[i][j]*sig_H*(1-p_H[j])*Lmn_H[i][j];                         /* Latent DR to smear positive DS disease (no disease history) - co-infection */
          double LmnH_to_LsnH = FS*(1-a_age_H[i][j])*(1-p_H[j])*(1-g)*Lmn_H[i][j];                     /* Latent DR to latent DS (no disease history) */

          double LspH_to_NspH = (v_age_H[i][j] + FS*a_age_H[i][j]*(1-p_H[j]))*(1-sig_H)*Lsp_H[i][j];   /* Latent DS to smear negative DS disease (prior Rx) - reactivation and reinfection */ 
          double LspH_to_IspH = (v_age_H[i][j] + FS*a_age_H[i][j]*(1-p_H[j]))*sig_H*Lsp_H[i][j];       /* Latent DS to smear positive DS disease (prior Rx) - reactivation and reinfection */     
          double LspH_to_NmpH = FM*a_age_H[i][j]*(1-p_H[j])*(1-sig_H)*Lsp_H[i][j];                     /* Latent DS to smear negative DR disease (prior_rx) - reactivation and reinfection */ 
          double LspH_to_ImpH = FM*a_age_H[i][j]*(1-p_H[j])*sig_H*Lsp_H[i][j];                         /* Latent DS to smear positive DR disease (prior_Rx) - reactivation and reinfection */
          double LspH_to_LmpH = FM*(1-a_age_H[i][j])*(1-p_H[j])*g*Lsp_H[i][j];                         /* Latent DS to latent DR (prior Rx) */
      
          double LmpH_to_NmpH = (v_age_H[i][j] + FM*a_age_H[i][j]*(1-p_H[j]))*(1-sig_H)*Lmp_H[i][j];   /* Latent DR to smear negative DR disease (prior Rx) - reactivation and reinfection */ 
          double LmpH_to_ImpH = (v_age_H[i][j] + FM*a_age_H[i][j]*(1-p_H[j]))*sig_H*Lmp_H[i][j];       /* Latent DR to smear positive DR disease (prior Rx) - reactivation and reinfection */     
          double LmpH_to_NspH = FS*a_age_H[i][j]*(1-p_H[j])*(1-sig_H)*Lmp_H[i][j];                     /* Latent DR to smear negative DS disease (prior_rx) - reactivation and reinfection */ 
          double LmpH_to_IspH = FS*a_age_H[i][j]*(1-p_H[j])*sig_H*Lmp_H[i][j];                         /* Latent DR to smear positive DS disease (prior_Rx) - reactivation and reinfection */
          double LmpH_to_LspH = FS*(1-a_age_H[i][j])*(1-p_H[j])*(1-g)*Lmp_H[i][j];                     /* Latent DR to latent DS (prior Rx) */

          double PTnH_to_LsnH = FS*(1-a_age_H[i][j])*(1-p_H[j])*PTn_H[i][j];            /* Post PT to latent DS (no disease history) */
          double PTnH_to_NsnH = FS*a_age_H[i][j]*(1-p_H[j])*(1-sig_H)*PTn_H[i][j];      /* Post PT to smear negative DS disease (no disease history) */
          double PTnH_to_IsnH = FS*a_age_H[i][j]*(1-p_H[j])*sig_H*PTn_H[i][j];          /* Post PT to smear positive DS disease (no disease history) */
          double PTnH_to_LmnH = FM*(1-a_age_H[i][j])*(1-p_H[j])*g*PTn_H[i][j];          /* Post PT to latent DR (no disease history) */
          double PTnH_to_NmnH = FM*a_age_H[i][j]*(1-p_H[j])*(1-sig_H)*PTn_H[i][j];      /* Post PT to smear negative DR disease (no disease history) */
          double PTnH_to_ImnH = FM*a_age_H[i][j]*(1-p_H[j])*sig_H*PTn_H[i][j];          /* Post PT to smear positive DR disease (no disease history) */
      
          double PTpH_to_LspH = FS*(1-a_age_H[i][j])*(1-p_H[j])*PTp_H[i][j];            /* Post PT to latent DS (prior Rx) */
          double PTpH_to_NspH = FS*a_age_H[i][j]*(1-p_H[j])*(1-sig_H)*PTp_H[i][j];      /* Post PT to smear negative DS disease (prior Rx) */
          double PTpH_to_IspH = FS*a_age_H[i][j]*(1-p_H[j])*sig_H*PTp_H[i][j];          /* Post PT to smear positive DS disease (prior Rx) */
          double PTpH_to_LmpH = FM*(1-a_age_H[i][j])*(1-p_H[j])*g*PTp_H[i][j];          /* Post PT to latent DR (prior Rx) */
          double PTpH_to_NmpH = FM*a_age_H[i][j]*(1-p_H[j])*(1-sig_H)*PTp_H[i][j];      /* Post PT to smear negative DR disease (prior Rx) */
          double PTpH_to_ImpH = FM*a_age_H[i][j]*(1-p_H[j])*sig_H*PTp_H[i][j];          /* Post PT to smear positive DR disease (prior Rx) */

          /* Calculate "care" flows here and use these in the derivatives */
          
          /* TB notification may trigger HIV test and ART initiation */
          /* Some proportion (HIV_test*HIV_link) of those notified (i.e. started on Rx) move to corresponding ART compartment */
          double HIV_ART = HIV_test*ART_link;

          double false_pos = health*kpos*(1-sp_I_pos*sp_N_pos)*l_s;   /* Susceptible, latently infected and post-PT are false pos notif at this rate (includes link to Rx). */
                                                                      /* This only has any effect in those latently infected with drug sus strains. tpos_s complete Rx and move to PTn/PTp */
          /* sm-, drug sus, no Rx history */    
          double NsnH_pos = kpos*se_N_pos*rel_d*Nsn_H[i][j];        /* TB positive - move all out of Nsn_H */
          double NsnH_dst = NsnH_pos*dstpos_n;                      /* Get DST */
          double NsnH_dst_fpos = NsnH_dst*(1-sp_m_pos);             /* False pos on DST */
          double NsnH_first = l_s*(NsnH_pos - NsnH_dst_fpos);       /* Start first line Rx (correct) */     
          double NsnH_second = l_m*(NsnH_dst_fpos);                 /* Start second line treatment (incorrect) */
          double NsnH_lost = NsnH_pos - NsnH_first - NsnH_second;   /* Positive cases lost to follow up  - go to Nsn_H */ 
          double NsnH_res = NsnH_first*e;                           /* Develop resistance - split between Nmp_H and Nmp_A */
          double NsnH_first_success = (NsnH_first-NsnH_res)*tpos_s; /* First line success - split between Lsp_H and Lsp_A */
          double NsnH_first_fail = (NsnH_first-NsnH_res)*(1-tpos_s);/* First line failure - split between Nsp_H and Nsp_A */
          double NsnH_second_success = NsnH_second*tpos_m;          /* Second line success - split between Lsp_H and Lsp_A */
          double NsnH_second_fail = NsnH_second*(1-tpos_m);         /* Second line failure - split between Nsp_H and Nsp_A */

          /* sm-, drug sus, previous Rx history */    
          double NspH_pos = kpos*se_N_pos*rel_d*Nsp_H[i][j];        /* TB positive - move all out of Nsp_H */
          double NspH_dst = NspH_pos*dstpos_p;                      /* Get DST */
          double NspH_dst_fpos = NspH_dst*(1-sp_m_pos);             /* False pos on DST */
          double NspH_first = l_s*(NspH_pos - NspH_dst_fpos);       /* Start first line Rx (correct) */     
          double NspH_second = l_m*(NspH_dst_fpos);                 /* Start second line treatment (incorrect) */
          double NspH_lost = NspH_pos - NspH_first - NspH_second;   /* Positive cases lost to follow up  - go to Nsp_H */ 
          double NspH_res = NspH_first*e;                           /* Develop resistance - split between Nmp_H and Nmp_A */
          double NspH_first_success = (NspH_first-NspH_res)*tpos_s; /* First line success - split between Lsp_H and Lsp_A */
          double NspH_first_fail = (NspH_first-NspH_res)*(1-tpos_s);/* First line failure - split between Nsp_H and Nsp_A */
          double NspH_second_success = NspH_second*tpos_m;          /* Second line success - split between Lsp_H and Lsp_A */
          double NspH_second_fail = NspH_second*(1-tpos_m);         /* Second line failure - split between Nsp_H and Nsp_A */

          /* sm+, drug sus, no Rx history */    
          double IsnH_pos = kpos*se_I_pos*Isn_H[i][j];              /* TB positive - move all out of Isn_H */
          double IsnH_dst = IsnH_pos*dstpos_n;                      /* Get DST */
          double IsnH_dst_fpos = IsnH_dst*(1-sp_m_pos);             /* False pos on DST */
          double IsnH_first = l_s*(IsnH_pos - IsnH_dst_fpos);       /* Start first line Rx (correct) */     
          double IsnH_second = l_m*(IsnH_dst_fpos);                 /* Start second line treatment (incorrect) */
          double IsnH_lost = IsnH_pos - IsnH_first - IsnH_second;   /* Positive cases lost to follow up  - go to Isn_H */ 
          double IsnH_res = IsnH_first*e;                           /* Develop resistance - split between Imp_H and Imp_A */
          double IsnH_first_success = (IsnH_first-IsnH_res)*tpos_s; /* First line success - split between Lsp_H and Lsp_A */
          double IsnH_first_fail = (IsnH_first-IsnH_res)*(1-tpos_s);/* First line failure - split between Isp_H and Isp_A */
          double IsnH_second_success = IsnH_second*tpos_m;          /* Second line success - split between Lsp_H and Lsp_A */
          double IsnH_second_fail = IsnH_second*(1-tpos_m);         /* Second line failure - split between Isp_H and Isp_A */

          /* sm+, drug sus, previous Rx history */    
          double IspH_pos = kpos*se_I_pos*Isp_H[i][j];              /* TB positive - move all out of Isp_H */
          double IspH_dst = IspH_pos*dstpos_p;                      /* Get DST */
          double IspH_dst_fpos = IspH_dst*(1-sp_m_pos);             /* False pos on DST */
          double IspH_first = l_s*(IspH_pos - IspH_dst_fpos);       /* Start first line Rx (correct) */     
          double IspH_second = l_m*(IspH_dst_fpos);                 /* Start second line Rx (incorrect) */
          double IspH_lost = IspH_pos - IspH_first - IspH_second;   /* Positive cases lost to follow up  - go to Isp_H */ 
          double IspH_res = IspH_first*e;                           /* Develop resistance - split between Imp_H and Imp_A */
          double IspH_first_success = (IspH_first-IspH_res)*tpos_s; /* First line success - split between Lsp_H and Lsp_A */
          double IspH_first_fail = (IspH_first-IspH_res)*(1-tpos_s);/* First line failure - split between Isp_H and Isp_A */
          double IspH_second_success = IspH_second*tpos_m;          /* Second line success - split between Lsp_H and Lsp_A*/
          double IspH_second_fail = IspH_second*(1-tpos_m);         /* Second line failure - split between Isp_H and Isp_A */

          /* sm-, MDR, no Rx history */
          double NmnH_pos = kpos*se_N_pos*rel_d*Nmn_H[i][j];        /* TB positive - move all out of Nmn_H */
          double NmnH_dst = NmnH_pos*dstpos_n;                      /* Get DST */
          double NmnH_dst_pos = NmnH_dst*se_m_pos;                  /* True pos on DST */
          double NmnH_first = l_s*(NmnH_pos - NmnH_dst_pos);        /* Start first line Rx (incorrect) */  
          double NmnH_second = l_m*NmnH_dst_pos;                    /* Start second line Rx (correct) */
          double NmnH_lost = NmnH_pos - NmnH_first - NmnH_second;   /* Positive cases lost to follow up - go to Nmn_H */
          double NmnH_first_success = NmnH_first*tpos_s*eff_n;      /* First line success - split between Lmp_H and Lmp_A */
          double NmnH_first_fail = NmnH_first - NmnH_first_success; /* First line failure - split between Nmp_H and Nmp_A */
          double NmnH_second_success = NmnH_second*tpos_m;          /* Second line success - split between Lmp_H and Lmp_A */
          double NmnH_second_fail = NmnH_second*(1-tpos_m);         /* Second line failure - split between Nmp_H and Nmp_A */

          /* sm-, MDR, previous Rx history */
          double NmpH_pos = kpos*se_N_pos*rel_d*Nmp_H[i][j];        /* TB positive - move all out of Nmp */
          double NmpH_dst = NmpH_pos*dstpos_p;                      /* Get DST */
          double NmpH_dst_pos = NmpH_dst*se_m_pos;                  /* True pos on DST */
          double NmpH_first = l_s*(NmpH_pos - NmpH_dst_pos);        /* Start first line Rx (incorrect) */  
          double NmpH_second = l_m*NmpH_dst_pos;                    /* Start second line Rx (correct) */
          double NmpH_lost = NmpH_pos - NmpH_first - NmpH_second;   /* Positive cases lost to follow up - go to Nmp */
          double NmpH_first_success = NmpH_first*tpos_s*eff_p;      /* First line success - split between Lmp_H and Lmp_A */
          double NmpH_first_fail = NmpH_first - NmpH_first_success; /* First line failure - split between Nmp_H and Nmp_A */
          double NmpH_second_success = NmpH_second*tpos_m;          /* Second line success - split between Lmp_H and Lmp_A */
          double NmpH_second_fail = NmpH_second*(1-tpos_m);         /* Second line failure - split between Nmp_H and Nmp_A */

          /* sm+, MDR, no Rx history */
          double ImnH_pos = kpos*se_I_pos*Imn_H[i][j];              /* TB positive - move all out of Imn_A */
          double ImnH_dst = ImnH_pos*dstpos_n;                      /* Get DST */
          double ImnH_dst_pos = ImnH_dst*se_m_pos;                  /* True pos on DST */
          double ImnH_first = l_s*(ImnH_pos - ImnH_dst_pos);        /* Start first line Rx (incorrect) */  
          double ImnH_second = l_m*ImnH_dst_pos;                    /* Start second line Rx (correct) */
          double ImnH_lost = ImnH_pos - ImnH_first - ImnH_second;   /* Positive cases lost to follow up - go to Imn_A */
          double ImnH_first_success = ImnH_first*tpos_s*eff_n;      /* First line success - split between Lmp_H and Lmp_A */
          double ImnH_first_fail = ImnH_first - ImnH_first_success; /* First line failure - split between Imp_H and Imp_A */
          double ImnH_second_success = ImnH_second*tpos_m;          /* Second line success - split between Lmp_H and Lmp_A */
          double ImnH_second_fail = ImnH_second*(1-tpos_m);         /* Second line failure - split between Imp_H and Imp_A */

          /* sm+, MDR, previous Rx history */
          double ImpH_pos = kpos*se_I_pos*Imp_H[i][j];              /* TB positive - move all out of Imp_A */
          double ImpH_dst = ImpH_pos*dstpos_p;                      /* Get DST */
          double ImpH_dst_pos = ImpH_dst*se_m_pos;                  /* True pos on DST */
          double ImpH_first = l_s*(ImpH_pos - ImpH_dst_pos);        /* Start first line Rx (incorrect) */  
          double ImpH_second = l_m*ImpH_dst_pos;                    /* Start second line Rx (correct) */
          double ImpH_lost = ImpH_pos - ImpH_first - ImpH_second;   /* Positive cases lost to follow up - go to Imp */
          double ImpH_first_success = ImpH_first*tpos_s*eff_p;      /* First line success - split between Lmp_H and Lmp_A */
          double ImpH_first_fail = ImpH_first - ImpH_first_success; /* First line failure - split between Imp_H and Imp_A */
          double ImpH_second_success = ImpH_second*tpos_m;          /* Second line success - split between Lmp_H and Lmp_A */
          double ImpH_second_fail = ImpH_second*(1-tpos_m);         /* Second line failure - split between Imp_H and Imp_A */

          dS_H[i][j] = - m_b[i]*S_H[i][j] - /* Death */
                      (FS + FM)*S_H[i][j] + /* Infection */
                      forc[iz+82]*H_CD4[j][i]*S[i] - H_prog[j+1][i]*S_H[i][j] + H_prog[j][i]*S_H[i][j-1] - /* HIV incidence and progression */
                      up_H_mort[j][i]*S_H[i][j] - ART_prop[i][j]*S_H[i][j] + (S_H[i][j]/tot_age[i])*(forc[iz+116]/5) - /* HIV death, ART inititation, migration */
                      false_pos*S_H[i][j]*HIV_ART; /* Link to ART for false positive TB cases */

          dLsn_H[i][j] = - m_b[i]*Lsn_H[i][j] + /* Death */
                        SH_to_LsnH + LmnH_to_LsnH + PTnH_to_LsnH - LsnH_to_LmnH - LsnH_to_NsnH - LsnH_to_IsnH - LsnH_to_NmnH - LsnH_to_ImnH + /* Infection and disease */
                        forc[iz+82]*H_CD4[j][i]*Lsn[i] - H_prog[j+1][i]*Lsn_H[i][j] + H_prog[j][i]*Lsn_H[i][j-1] - /* HIV incidence and progression */ 
                        up_H_mort[j][i]*Lsn_H[i][j] - ART_prop[i][j]*Lsn_H[i][j] + (Lsn_H[i][j]/tot_age[i])*(forc[iz+116]/5) + r_H*(Isn_H[i][j] + Nsn_H[i][j]) - /* HIV death, ART inititation, migration, self_cure */
                        false_pos*Lsn_H[i][j]*(HIV_ART + (1-HIV_ART)*tpos_s); /* False positive for TB, ART and Rx */    

          dLsp_H[i][j] = - m_b[i]*Lsp_H[i][j] + /*Death */
                        LmpH_to_LspH + PTpH_to_LspH - LspH_to_LmpH - LspH_to_NspH - LspH_to_IspH  - LspH_to_NmpH - LspH_to_ImpH + /* Infection and disease */ 
                        (1-HIV_ART)*(NsnH_first_success + NsnH_second_success + NspH_first_success + NspH_second_success + IsnH_first_success + IsnH_second_success + IspH_first_success + IspH_second_success) + /* Rx */          
                        forc[iz+82]*H_CD4[j][i]*Lsp[i] - H_prog[j+1][i]*Lsp_H[i][j] + H_prog[j][i]*Lsp_H[i][j-1] - /* HIV incidence and progression */
                        up_H_mort[j][i]*Lsp_H[i][j] - ART_prop[i][j]*Lsp_H[i][j] + (Lsp_H[i][j]/tot_age[i])*(forc[iz+116]/5) + r_H*(Isp_H[i][j]+Nsp_H[i][j]) - /* HIV death, ART inititation, migration, self-cure */
                        false_pos*Lsp_H[i][j]*(HIV_ART + (1-HIV_ART)*tpos_s); /* False positive for TB, ART and Rx */

          dLmn_H[i][j] = - m_b[i]*Lmn_H[i][j] + /* Death */
                        SH_to_LmnH + LsnH_to_LmnH + PTnH_to_LmnH - LmnH_to_LsnH - LmnH_to_NsnH - LmnH_to_IsnH - LmnH_to_NmnH - LmnH_to_ImnH - /* Infection and disease */ 
                        forc[iz+82]*H_CD4[j][i]*Lmn[i] - H_prog[j+1][i]*Lmn_H[i][j] + H_prog[j][i]*Lmn_H[i][j-1] - /* HIV incidence and progression */
                        up_H_mort[j][i]*Lmn_H[i][j] - ART_prop[i][j]*Lmn_H[i][j] + (Lmn_H[i][j]/tot_age[i])*(forc[iz+116]/5) + r_H*(Imn_H[i][j]+Nmn_H[i][j]) - /* HIV death, ART inititation, migration, self-cure */
                        false_pos*Lmn_H[i][j]*HIV_ART; /* Link to ART for false positive TB cases */
     
          dLmp_H[i][j] = - m_b[i]*Lmp_H[i][j] + /* Death */
                        LspH_to_LmpH + PTpH_to_LmpH - LmpH_to_LspH - LmpH_to_NspH - LmpH_to_IspH - LmpH_to_NmpH - LmpH_to_ImpH + /* Infection and disease */
                        (1-HIV_ART)*(NmnH_first_success + NmnH_second_success + NmpH_first_success + NmpH_second_success + ImnH_first_success + ImnH_second_success + ImpH_first_success + ImpH_second_success) + /* Rx */ 
                        forc[iz+82]*H_CD4[j][i]*Lmp[i] - H_prog[j+1][i]*Lmp_H[i][j] + H_prog[j][i]*Lmp_H[i][j-1] - /* HIV incidence and progression */
                        up_H_mort[j][i]*Lmp_H[i][j] - ART_prop[i][j]*Lmp_H[i][j] + (Lmp_H[i][j]/tot_age[i])*(forc[iz+116]/5) + r_H*(Imp_H[i][j]+Nmp_H[i][j]) - /* HIV death, ART inititation, migration, self-cure */
                        false_pos*Lmp_H[i][j]*HIV_ART; /* Link to ART for false positive TB cases */

          dNsn_H[i][j] = - m_b[i]*Nsn_H[i][j] + /* Death */
                        SH_to_NsnH + LsnH_to_NsnH + LmnH_to_NsnH + PTnH_to_NsnH - /* Disease */
                        NsnH_pos + NsnH_lost + /* Diagnosis, pre Rx lost */
                        forc[iz+82]*H_CD4[j][i]*Nsn[i] - H_prog[j+1][i]*Nsn_H[i][j] + H_prog[j][i]*Nsn_H[i][j-1] - /* HIV incidence and progression */ 
                        up_H_mort[j][i]*Nsn_H[i][j] - ART_prop[i][j]*Nsn_H[i][j] + (Nsn_H[i][j]/tot_age[i])*(forc[iz+116]/5) - (theta_H + r_H + muN_H)*Nsn_H[i][j]; /* HIV death, ART inititation, migration, sm conversion, self-cure, TB death */

          dNsp_H[i][j] = - m_b[i]*Nsp_H[i][j] + /* Death */
                        LspH_to_NspH + LmpH_to_NspH + PTpH_to_NspH - /* Disease */
                        NspH_pos + NspH_lost + (1-HIV_ART)*(NsnH_first_fail + NsnH_second_fail + NspH_first_fail + NspH_second_fail) + /* Diagnosis, pre Rx lost, failed Rx */                     
                        forc[iz+82]*H_CD4[j][i]*Nsp[i] - H_prog[j+1][i]*Nsp_H[i][j] + H_prog[j][i]*Nsp_H[i][j-1] - /* HIV incidence and progression */
                        up_H_mort[j][i]*Nsp_H[i][j] - ART_prop[i][j]*Nsp_H[i][j] + (Nsp_H[i][j]/tot_age[i])*(forc[iz+116]/5) -  (theta_H + r_H + muN_H)*Nsp_H[i][j]; /* HIV death, ART inititation, migration, sm conversion, self-cure, TB death */
    
          dNmn_H[i][j] = - m_b[i]*Nmn_H[i][j] + /* Death */
                        SH_to_NmnH + LsnH_to_NmnH + LmnH_to_NmnH + PTnH_to_NmnH - /* Disease */
                        NmnH_pos + NmnH_lost + /* Diagnosis, pre Rx lost */
                        forc[iz+82]*H_CD4[j][i]*Nmn[i] - H_prog[j+1][i]*Nmn_H[i][j] + H_prog[j][i]*Nmn_H[i][j-1] - /* HIV incidence and progression */
                        up_H_mort[j][i]*Nmn_H[i][j] - ART_prop[i][j]*Nmn_H[i][j] + (Nmn_H[i][j]/tot_age[i])*(forc[iz+116]/5) - (theta_H + r_H + muN_H)*Nmn_H[i][j]; /* HIV death, ART inititation, migration, sm conversion, self-cure, TB death */

          dNmp_H[i][j] = - m_b[i]*Nmp_H[i][j] + /* Death */
                        LspH_to_NmpH + LmpH_to_NmpH + PTpH_to_NmpH - /* Disease */
                        NmpH_pos + NmpH_lost + (1-HIV_ART)*(NmnH_first_fail + NmnH_second_fail + NmpH_first_fail + NmpH_second_fail + NsnH_res + NspH_res) + /* Diagnosis, pre Rx lost, failed Rx, acquired resistance */                      
                        forc[iz+82]*H_CD4[j][i]*Nmp[i] - H_prog[j+1][i]*Nmp_H[i][j] + H_prog[j][i]*Nmp_H[i][j-1] - /* HIV incidence and progression */ 
                        up_H_mort[j][i]*Nmp_H[i][j] - ART_prop[i][j]*Nmp_H[i][j] + (Nmp_H[i][j]/tot_age[i])*(forc[iz+116]/5) - (theta_H + r_H + muN_H)*Nmp_H[i][j]; /* HIV death, ART inititation, migration, sm conversion, self-cure, TB death */

          dIsn_H[i][j] = - m_b[i]*Isn_H[i][j] + /* Death */
                        SH_to_IsnH + LsnH_to_IsnH + LmnH_to_IsnH + PTnH_to_IsnH - /* Disease */
                        IsnH_pos + IsnH_lost + /* Diagnosis, pre Rx lost */
                        forc[iz+82]*H_CD4[j][i]*Isn[i] - H_prog[j+1][i]*Isn_H[i][j] + H_prog[j][i]*Isn_H[i][j-1] - /* HIV incidence and progression */ 
                        up_H_mort[j][i]*Isn_H[i][j] - ART_prop[i][j]*Isn_H[i][j] + (Isn_H[i][j]/tot_age[i])*(forc[iz+116]/5) + theta_H*Nsn_H[i][j] - (r_H + muI_H)*Isn_H[i][j]; /* HIV death, ART inititation, migration, sm conversion, self-cure, TB death */

          dIsp_H[i][j] = - m_b[i]*Isp_H[i][j] + /* Death */
                        LspH_to_IspH + LmpH_to_IspH + PTpH_to_IspH - /* Disease */  
                        IspH_pos + IspH_lost + (1-HIV_ART)*(IsnH_first_fail + IsnH_second_fail + IspH_first_fail + IspH_second_fail) + /* Diagnosis, pre Rx lost, failed Rx */
                        forc[iz+82]*H_CD4[j][i]*Isp[i] - H_prog[j+1][i]*Isp_H[i][j] + H_prog[j][i]*Isp_H[i][j-1] - /* HIV incidence and progression */ 
                        up_H_mort[j][i]*Isp_H[i][j] - ART_prop[i][j]*Isp_H[i][j] + (Isp_H[i][j]/tot_age[i])*(forc[iz+116]/5) + theta_H*Nsp_H[i][j] - (r_H + muI_H)*Isp_H[i][j]; /* HIV death, ART inititation, migration, sm conversion, self-cure, TB death */       

          dImn_H[i][j] = - m_b[i]*Imn_H[i][j] + /* Death */
                        SH_to_ImnH + LsnH_to_ImnH + LmnH_to_ImnH + PTnH_to_ImnH - /* Disease */
                        ImnH_pos + ImnH_lost + /* Diagnosis, pre Rx lost */
                        forc[iz+82]*H_CD4[j][i]*Imn[i] - H_prog[j+1][i]*Imn_H[i][j] + H_prog[j][i]*Imn_H[i][j-1] - /* HIV incidence and progression */
                        up_H_mort[j][i]*Imn_H[i][j] - ART_prop[i][j]*Imn_H[i][j] + (Imn_H[i][j]/tot_age[i])*(forc[iz+116]/5) + theta_H*Nmn_H[i][j] - (r_H + muI_H)*Imn_H[i][j]; /* HIV death, ART inititation, migration, sm conversion, self-cure, TB death */

          dImp_H[i][j] = - m_b[i]*Imp_H[i][j] + /* Death */
                        LspH_to_ImpH + LmpH_to_ImpH + PTpH_to_ImpH - /* Disease */
                        ImpH_pos + ImpH_lost + (1-HIV_ART)*(ImnH_first_fail + ImnH_second_fail + ImpH_first_fail + ImpH_second_fail + IsnH_res + IspH_res) + /* Diagnosis, pre Rx lost, failed Rx, acquired resistance */  
                        forc[iz+82]*H_CD4[j][i]*Imp[i] - H_prog[j+1][i]*Imp_H[i][j] + H_prog[j][i]*Imp_H[i][j-1] - /* HIV incidence and progression */
                        up_H_mort[j][i]*Imp_H[i][j] - ART_prop[i][j]*Imp_H[i][j] + (Imp_H[i][j]/tot_age[i])*(forc[iz+116]/5) + theta_H*Nmp_H[i][j] - (r_H + muI_H)*Imp_H[i][j]; /* HIV death, ART inititation, migration, sm conversion, self-cure, TB death */                        
                   
          dPTn_H[i][j] = -m_b[i]*PTn_H[i][j] - /* Death */
                         PTnH_to_LsnH - PTnH_to_NsnH - PTnH_to_IsnH - PTnH_to_LmnH - PTnH_to_NmnH - PTnH_to_ImnH + /* Infection and disease */
                         false_pos*(1-HIV_ART)*tpos_s*Lsn_H[i][j] - false_pos*PTn_H[i][j]*HIV_ART + /* Incorrect Rx for latent infected, linked to ART*/ 
                         forc[iz+82]*H_CD4[j][i]*PTn[i] - H_prog[j+1][i]*PTn_H[i][j] + H_prog[j][i]*PTn_H[i][j-1] - /* HIV incidence and progression */ 
                         up_H_mort[j][i]*PTn_H[i][j] - ART_prop[i][j]*PTn_H[i][j] + (PTn_H[i][j]/tot_age[i])*(forc[iz+116]/5); /* HIV death, ART inititation, migration */

          dPTp_H[i][j] = - m_b[i]*PTp_H[i][j] - /* Death */
                         PTpH_to_LspH - PTpH_to_NspH - PTpH_to_IspH - PTpH_to_LmpH - PTpH_to_NmpH - PTpH_to_ImpH + /* Infection and disease */
                         false_pos*(1-HIV_ART)*tpos_s*Lsp_H[i][j] - false_pos*PTp_H[i][j]*HIV_ART + /* Incorrect Rx for latent infected, linked to ART*/
                         forc[iz+82]*H_CD4[j][i]*PTp[i] - H_prog[j+1][i]*PTp_H[i][j] + H_prog[j][i]*PTp_H[i][j-1] - /* HIV incidence and progression */
                         up_H_mort[j][i]*PTp_H[i][j] - ART_prop[i][j]*PTp_H[i][j] + (PTp_H[i][j]/tot_age[i])*(forc[iz+116]/5); /* HIV death, ART inititation, migration */
                           
          TB_cases_pos_age[i][j] =(v_age_H[i][j]*(1-sig_H) + FS*a_age_H[i][j]*(1-p_H[j])*(1-sig_H))*Lsn_H[i][j] + FS*a_age_H[i][j]*(1-sig_H)*(S_H[i][j] + (1-p_H[j])*(Lmn_H[i][j] + PTn_H[i][j])) + 
                                  (v_age_H[i][j]*(1-sig_H) + FS*a_age_H[i][j]*(1-p_H[j])*(1-sig_H))*Lsp_H[i][j] + FS*a_age_H[i][j]*(1-sig_H)*(1-p_H[j])*(Lmp_H[i][j] + PTp_H[i][j]) +
                                  (v_age_H[i][j]*(1-sig_H) + FM*a_age_H[i][j]*(1-p_H[j])*(1-sig_H))*Lmn_H[i][j] + FM*a_age_H[i][j]*(1-sig_H)*(S_H[i][j] + (1-p_H[j])*(Lsn_H[i][j] + PTn_H[i][j])) +         
                                  (v_age_H[i][j]*(1-sig_H) + FM*a_age_H[i][j]*(1-p_H[j])*(1-sig_H))*Lmp_H[i][j] + FM*a_age_H[i][j]*(1-sig_H)*(1-p_H[j])*(Lsp_H[i][j] + PTp_H[i][j]) +
                                  (v_age_H[i][j]*sig_H + FS*a_age_H[i][j]*sig_H*(1-p_H[j]))*Lsn_H[i][j] + FS*a_age_H[i][j]*sig_H*(S_H[i][j] + (1-p_H[j])*(Lmn_H[i][j] + PTn_H[i][j]))+
                                  (v_age_H[i][j]*sig_H + FS*a_age_H[i][j]*sig_H*(1-p_H[j]))*Lsp_H[i][j] + FS*a_age_H[i][j]*sig_H*(1-p_H[j])*(Lmp_H[i][j] + PTp_H[i][j]) +
                                  (v_age_H[i][j]*sig_H + FM*a_age_H[i][j]*sig_H*(1-p_H[j]))*Lmn_H[i][j] + FM*a_age_H[i][j]*sig_H*(S_H[i][j] + (1-p_H[j])*(Lsn_H[i][j] + PTn_H[i][j]))+
                                  (v_age_H[i][j]*sig_H + FM*a_age_H[i][j]*sig_H*(1-p_H[j]))*Lmp_H[i][j] + FM*a_age_H[i][j]*sig_H*(1-p_H[j])*(Lsp_H[i][j] + PTp_H[i][j]);

          TB_cases_pos = TB_cases_pos + TB_cases_pos_age[i][j];
          
          TB_cases_age[i] = TB_cases_age[i] + TB_cases_pos_age[i][j];
       
       /* HIV+ on ART: loop through time on ART, CD4 at initiation, age */
          
          if(ART_all > 0.0){  /* if no ART yet can skip these */
          
          for (l=0; l<n_ART; l++){
        
              /* Calculate the disease flows here and use these in the derivatives - intention is to make the model more flexible/easier to understand */
      
            double SA_to_LsnA = FS*(1-a_age_A[i][j][l])*S_A[i][j][l];                                /* Susceptible to latent DS infection (no disease history) */
            double SA_to_NsnA = FS*a_age_A[i][j][l]*(1-sig_H)*S_A[i][j][l];                          /* Susceptible to primary DS smear negative disease (no disease history) */
            double SA_to_IsnA = FS*a_age_A[i][j][l]*sig_H*S_A[i][j][l];                              /* Susceptible to primary DS smear positive disease (no disease history) */
            double SA_to_LmnA = FM*(1-a_age_A[i][j][l])*S_A[i][j][l];                                /* Susceptible to latent DR infection (no disease history) */
            double SA_to_NmnA = FM*a_age_A[i][j][l]*(1-sig_H)*S_A[i][j][l];                          /* Susceptible to primary DR smear negative disease (no disease history) */
            double SA_to_ImnA = FM*a_age_A[i][j][l]*sig_H*S_A[i][j][l];                              /* Susceptible to primary DR smear positive disease (no disease history) */
      
            double LsnA_to_NsnA = (v_age_A[i][j][l] + FS*a_age_A[i][j][l]*(1-p_A[j][l]))*(1-sig_H)*Lsn_A[i][j][l];   /* Latent DS to smear negative DS disease (no disease history) - reactivation and reinfection */ 
            double LsnA_to_IsnA = (v_age_A[i][j][l] + FS*a_age_A[i][j][l]*(1-p_A[j][l]))*sig_H*Lsn_A[i][j][l];       /* Latent DS to smear positive DS disease (no disease history) - reactivation and reinfection */ 
            double LsnA_to_NmnA = FM*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H)*Lsn_A[i][j][l];                        /* Latent DS to smear negative DR disease (no disease history) - co-infection */ 
            double LsnA_to_ImnA = FM*a_age_A[i][j][l]*(1-p_A[j][l])*sig_H*Lsn_A[i][j][l];                            /* Latent DS to smear positive DR disease (no disease history) - co-infection */
            double LsnA_to_LmnA = FM*(1-a_age_A[i][j][l])*(1-p_A[j][l])*g*Lsn_A[i][j][l];                            /* Latent DS to latent DR (no disease history) */
      
            double LmnA_to_NmnA = (v_age_A[i][j][l] + FM*a_age_A[i][j][l]*(1-p_A[j][l]))*(1-sig_H)*Lmn_A[i][j][l];   /* Latent DR to smear negative DR disease (no disease history) - reactivation and reinfection */ 
            double LmnA_to_ImnA = (v_age_A[i][j][l] + FM*a_age_A[i][j][l]*(1-p_A[j][l]))*sig_H*Lmn_A[i][j][l];       /* Latent DR to smear positive DR disease (no disease history) - reactivation and reinfection */ 
            double LmnA_to_NsnA = FS*a_age_A[i][j][l]*(1-sig_H)*(1-p_A[j][l])*Lmn_A[i][j][l];                        /* Latent DR to smear negative DS disease (no disease history) - co-infection */
            double LmnA_to_IsnA = FS*a_age_A[i][j][l]*sig_H*(1-p_A[j][l])*Lmn_A[i][j][l];                            /* Latent DR to smear positive DS disease (no disease history) - co-infection */
            double LmnA_to_LsnA = FS*(1-a_age_A[i][j][l])*(1-p_A[j][l])*(1-g)*Lmn_A[i][j][l];                        /* Latent DR to latent DS (no disease history) */

            double LspA_to_NspA = (v_age_A[i][j][l] + FS*a_age_A[i][j][l]*(1-p_A[j][l]))*(1-sig_H)*Lsp_A[i][j][l];   /* Latent DS to smear negative DS disease (prior Rx) - reactivation and reinfection */ 
            double LspA_to_IspA = (v_age_A[i][j][l] + FS*a_age_A[i][j][l]*(1-p_A[j][l]))*sig_H*Lsp_A[i][j][l];       /* Latent DS to smear positive DS disease (prior Rx) - reactivation and reinfection */     
            double LspA_to_NmpA = FM*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H)*Lsp_A[i][j][l];                        /* Latent DS to smear negative DR disease (prior_rx) - reactivation and reinfection */ 
            double LspA_to_ImpA = FM*a_age_A[i][j][l]*(1-p_A[j][l])*sig_H*Lsp_A[i][j][l];                            /* Latent DS to smear positive DR disease (prior_Rx) - reactivation and reinfection */
            double LspA_to_LmpA = FM*(1-a_age_A[i][j][l])*(1-p_A[j][l])*g*Lsp_A[i][j][l];                            /* Latent DS to latent DR (prior Rx) */
      
            double LmpA_to_NmpA = (v_age_A[i][j][l] + FM*a_age_A[i][j][l]*(1-p_A[j][l]))*(1-sig_H)*Lmp_A[i][j][l];   /* Latent DR to smear negative DR disease (prior Rx) - reactivation and reinfection */ 
            double LmpA_to_ImpA = (v_age_A[i][j][l] + FM*a_age_A[i][j][l]*(1-p_A[j][l]))*sig_H*Lmp_A[i][j][l];       /* Latent DR to smear positive DR disease (prior Rx) - reactivation and reinfection */     
            double LmpA_to_NspA = FS*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H)*Lmp_A[i][j][l];                        /* Latent DR to smear negative DS disease (prior_rx) - reactivation and reinfection */ 
            double LmpA_to_IspA = FS*a_age_A[i][j][l]*(1-p_A[j][l])*sig_H*Lmp_A[i][j][l];                            /* Latent DR to smear positive DS disease (prior_Rx) - reactivation and reinfection */
            double LmpA_to_LspA = FS*(1-a_age_A[i][j][l])*(1-p_A[j][l])*(1-g)*Lmp_A[i][j][l];                        /* Latent DR to latent DS (prior Rx) */
            
            double PTnA_to_LsnA = FS*(1-a_age_A[i][j][l])*(1-p_A[j][l])*PTn_A[i][j][l];            /* Post PT to latent DS (no disease history) */
            double PTnA_to_NsnA = FS*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H)*PTn_A[i][j][l];      /* Post PT to smear negative DS disease (no disease history) */
            double PTnA_to_IsnA = FS*a_age_A[i][j][l]*(1-p_A[j][l])*sig_H*PTn_A[i][j][l];          /* Post PT to smear positive DS disease (no disease history) */
            double PTnA_to_LmnA = FM*(1-a_age_A[i][j][l])*(1-p_A[j][l])*g*PTn_A[i][j][l];          /* Post PT to latent DR (no disease history) */
            double PTnA_to_NmnA = FM*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H)*PTn_A[i][j][l];      /* Post PT to smear negative DR disease (no disease history) */
            double PTnA_to_ImnA = FM*a_age_A[i][j][l]*(1-p_A[j][l])*sig_H*PTn_A[i][j][l];          /* Post PT to smear positive DR disease (no disease history) */
      
            double PTpA_to_LspA = FS*(1-a_age_A[i][j][l])*(1-p_A[j][l])*PTp_A[i][j][l];            /* Post PT to latent DS (prior Rx) */
            double PTpA_to_NspA = FS*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H)*PTp_A[i][j][l];      /* Post PT to smear negative DS disease (prior Rx) */
            double PTpA_to_IspA = FS*a_age_A[i][j][l]*(1-p_A[j][l])*sig_H*PTp_A[i][j][l];          /* Post PT to smear positive DS disease (prior Rx) */
            double PTpA_to_LmpA = FM*(1-a_age_A[i][j][l])*(1-p_A[j][l])*g*PTp_A[i][j][l];          /* Post PT to latent DR (prior Rx) */
            double PTpA_to_NmpA = FM*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H)*PTp_A[i][j][l];      /* Post PT to smear negative DR disease (prior Rx) */
            double PTpA_to_ImpA = FM*a_age_A[i][j][l]*(1-p_A[j][l])*sig_H*PTp_A[i][j][l];          /* Post PT to smear positive DR disease (prior Rx) */
        
            /* Calculate "care" flows here and use these in the derivatives */
            
            /* sm-, drug sus, no Rx history */    
            double NsnA_pos = kpos*se_N_pos*rel_d*Nsn_A[i][j][l];     /* TB positive - move all out of Nsn_A */
            double NsnA_dst = NsnA_pos*dstpos_n;                      /* Get DST */
            double NsnA_dst_fpos = NsnA_dst*(1-sp_m_pos);             /* False pos on DST */
            double NsnA_first = l_s*(NsnA_pos - NsnA_dst_fpos);       /* Start first line Rx (correct) */     
            double NsnA_second = l_m*(NsnA_dst_fpos);                 /* Start second line treatment (incorrect) */
            double NsnA_lost = NsnA_pos - NsnA_first - NsnA_second;   /* Positive cases lost to follow up  - go to Nsn_A */ 
            double NsnA_res = NsnA_first*e;                           /* Develop resistance - go to Nmp_A */
            double NsnA_first_success = (NsnA_first-NsnA_res)*tART_s; /* First line success - go to Lsp_A */
            double NsnA_first_fail = (NsnA_first-NsnA_res)*(1-tART_s);/* First line failure - go to Nsp_A */
            double NsnA_second_success = NsnA_second*tART_m;          /* Second line success - go to Lsp_A */
            double NsnA_second_fail = NsnA_second*(1-tART_m);         /* Second line failure - go to Nsp_A */

            /* sm-, drug sus, previous Rx history */    
            double NspA_pos = kpos*se_N_pos*rel_d*Nsp_A[i][j][l];     /* TB positive - move all out of Nsp_A */
            double NspA_dst = NspA_pos*dstpos_p;                      /* Get DST */
            double NspA_dst_fpos = NspA_dst*(1-sp_m_pos);             /* False pos on DST */
            double NspA_first = l_s*(NspA_pos - NspA_dst_fpos);       /* Start first line Rx (correct) */     
            double NspA_second = l_m*(NspA_dst_fpos);                 /* Start second line treatment (incorrect) */
            double NspA_lost = NspA_pos - NspA_first - NspA_second;   /* Positive cases lost to follow up  - go to Nsp_A */ 
            double NspA_res = NspA_first*e;                           /* Develop resistance - go to Nmp_A */
            double NspA_first_success = (NspA_first-NspA_res)*tART_s; /* First line success - go to Lsp_A */
            double NspA_first_fail = (NspA_first-NspA_res)*(1-tART_s);/* First line failure - go to Nsp_A */
            double NspA_second_success = NspA_second*tART_m;          /* Second line success - go to Lsp_A */
            double NspA_second_fail = NspA_second*(1-tART_m);         /* Second line failure - go to Nsp_A */

            /* sm+, drug sus, no Rx history */    
            double IsnA_pos = kpos*se_I_pos*Isn_A[i][j][l];           /* TB positive - move all out of Isn_A */
            double IsnA_dst = IsnA_pos*dstpos_n;                      /* Get DST */
            double IsnA_dst_fpos = IsnA_dst*(1-sp_m_pos);             /* False pos on DST */
            double IsnA_first = l_s*(IsnA_pos - IsnA_dst_fpos);       /* Start first line Rx (correct) */     
            double IsnA_second = l_m*(IsnA_dst_fpos);                 /* Start second line treatment (incorrect) */
            double IsnA_lost = IsnA_pos - IsnA_first - IsnA_second;   /* Positive cases lost to follow up  - go to Isn_A */ 
            double IsnA_res = IsnA_first*e;                           /* Develop resistance - go to Imp_A */
            double IsnA_first_success = (IsnA_first-IsnA_res)*tART_s; /* First line success - go to Lsp_A */
            double IsnA_first_fail = (IsnA_first-IsnA_res)*(1-tART_s);/* First line failure - go to Isp_A */
            double IsnA_second_success = IsnA_second*tART_m;          /* Second line success - go to Lsp_A */
            double IsnA_second_fail = IsnA_second*(1-tART_m);         /* Second line failure - go to Isp_A */

            /* sm+, drug sus, previous Rx history */    
            double IspA_pos = kpos*se_I_pos*Isp_A[i][j][l];           /* TB positive - move all out of Isp_A */
            double IspA_dst = IspA_pos*dstpos_p;                      /* Get DST */
            double IspA_dst_fpos = IspA_dst*(1-sp_m_pos);             /* False pos on DST */
            double IspA_first = l_s*(IspA_pos - IspA_dst_fpos);       /* Start first line Rx (correct) */     
            double IspA_second = l_m*(IspA_dst_fpos);                 /* Start second line Rx (incorrect) */
            double IspA_lost = IspA_pos - IspA_first - IspA_second;   /* Positive cases lost to follow up  - go to Isp_A */ 
            double IspA_res = IspA_first*e;                           /* Develop resistance - go to Imp_A */
            double IspA_first_success = (IspA_first-IspA_res)*tART_s; /* First line success - go to Lsp_A */
            double IspA_first_fail = (IspA_first-IspA_res)*(1-tART_s);/* First line failure - go to Isp_A */
            double IspA_second_success = IspA_second*tART_m;          /* Second line success - go to Lsp_A */
            double IspA_second_fail = IspA_second*(1-tART_m);         /* Second line failure - go to Isp_A */

            /* sm-, MDR, no Rx history */
            double NmnA_pos = kpos*se_N_pos*rel_d*Nmn_A[i][j][l];     /* TB positive - move all out of Nmn_A */
            double NmnA_dst = NmnA_pos*dstpos_n;                      /* Get DST */
            double NmnA_dst_pos = NmnA_dst*se_m_pos;                  /* True pos on DST */
            double NmnA_first = l_s*(NmnA_pos - NmnA_dst_pos);        /* Start first line Rx (incorrect) */  
            double NmnA_second = l_m*NmnA_dst_pos;                    /* Start second line Rx (correct) */
            double NmnA_lost = NmnA_pos - NmnA_first - NmnA_second;   /* Positive cases lost to follow up - go to Nmn_A */
            double NmnA_first_success = NmnA_first*tART_s*eff_n;      /* First line success - go to Lmp_A */
            double NmnA_first_fail = NmnA_first - NmnA_first_success; /* First line failure - go to Nmp_A */
            double NmnA_second_success = NmnA_second*tART_m;          /* Second line success - go to Lmp_A */
            double NmnA_second_fail = NmnA_second*(1-tART_m);         /* Second line failure - go to Nmp_A */

            /* sm-, MDR, previous Rx history */
            double NmpA_pos = kpos*se_N_pos*rel_d*Nmp_A[i][j][l];     /* TB positive - move all out of Nmp_A */
            double NmpA_dst = NmpA_pos*dstpos_p;                      /* Get DST */
            double NmpA_dst_pos = NmpA_dst*se_m_pos;                  /* True pos on DST */
            double NmpA_first = l_s*(NmpA_pos - NmpA_dst_pos);        /* Start first line Rx (incorrect) */  
            double NmpA_second = l_m*NmpA_dst_pos;                    /* Start second line Rx (correct) */
            double NmpA_lost = NmpA_pos - NmpA_first - NmpA_second;   /* Positive cases lost to follow up - go to Nmp_A */
            double NmpA_first_success = NmpA_first*tART_s*eff_p;      /* First line success - go to Lmp_A */
            double NmpA_first_fail = NmpA_first - NmpA_first_success; /* First line failure - go to Nmp_A */
            double NmpA_second_success = NmpA_second*tART_m;          /* Second line success - go to Lmp_A */
            double NmpA_second_fail = NmpA_second*(1-tART_m);         /* Second line failure - go to Nmp_A */

            /* sm+, MDR, no Rx history */
            double ImnA_pos = kpos*se_I_pos*Imn_A[i][j][l];           /* TB positive - move all out of Imn_A */
            double ImnA_dst = ImnA_pos*dstpos_n;                      /* Get DST */
            double ImnA_dst_pos = ImnA_dst*se_m_pos;                  /* True pos on DST */
            double ImnA_first = l_s*(ImnA_pos - ImnA_dst_pos);        /* Start first line Rx (incorrect) */  
            double ImnA_second = l_m*ImnA_dst_pos;                    /* Start second line Rx (correct) */
            double ImnA_lost = ImnA_pos - ImnA_first - ImnA_second;   /* Positive cases lost to follow up - go to Imn_A */
            double ImnA_first_success = ImnA_first*tART_s*eff_n;      /* First line success - go to Lmp_A */
            double ImnA_first_fail = ImnA_first - ImnA_first_success; /* First line failure - go to Imp_A */
            double ImnA_second_success = ImnA_second*tART_m;          /* Second line success - go to Lmp_A */
            double ImnA_second_fail = ImnA_second*(1-tART_m);         /* Second line failure - go to Imp_A */

            /* sm+, MDR, previous Rx history */
            double ImpA_pos = kpos*se_I_pos*Imp_A[i][j][l];           /* TB positive - move all out of Imp_A */
            double ImpA_dst = ImpA_pos*dstpos_p;                      /* Get DST */
            double ImpA_dst_pos = ImpA_dst*se_m_pos;                  /* True pos on DST */
            double ImpA_first = l_s*(ImpA_pos - ImpA_dst_pos);        /* Start first line Rx (incorrect) */  
            double ImpA_second = l_m*ImpA_dst_pos;                    /* Start second line Rx (correct) */
            double ImpA_lost = ImpA_pos - ImpA_first - ImpA_second;   /* Positive cases lost to follow up - go to Imp_A */
            double ImpA_first_success = ImpA_first*tART_s*eff_p;      /* First line success - go to Lmp_A */
            double ImpA_first_fail = ImpA_first - ImpA_first_success; /* First line failure - go to Imp_A */
            double ImpA_second_success = ImpA_second*tART_m;          /* Second line success - go to Lmp_A */
            double ImpA_second_fail = ImpA_second*(1-tART_m);         /* Second line failure - go to Imp_A */
        

        
        
            dS_A[i][j][l] = - m_b[i]*S_A[i][j][l] - /* Death */
                            (FS + FM)*S_A[i][j][l] + /* Infection */
                            ART_prop[i][j]*A_start[l]*S_H[i][j] + A_prog[l]*S_A[i][j][l-1] - A_prog[l+1]*S_A[i][j][l] -  /* ART initation and progression */
                            up_A_mort[l][j][i]*S_A[i][j][l] + (S_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) + /* ART death and migration */
                            false_pos*S_H[i][j]*HIV_ART*A_start[l]; /* Link to ART for false positive TB cases */

            dLsn_A[i][j][l] = - m_b[i]*Lsn_A[i][j][l] + /* Death */
                            SA_to_LsnA + LmnA_to_LsnA + PTnA_to_LsnA - LsnA_to_LmnA - LsnA_to_NsnA - LsnA_to_IsnA - LsnA_to_NmnA - LsnA_to_ImnA + /* Infection and disease */ 
                            ART_prop[i][j]*A_start[l]*Lsn_H[i][j] + A_prog[l]*Lsn_A[i][j][l-1] - A_prog[l+1]*Lsn_A[i][j][l] - /* ART initation and progression */
                            up_A_mort[l][j][i]*Lsn_A[i][j][l] + (Lsn_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) + r_H*(Isn_A[i][j][l] + Nsn_A[i][j][l]) - /* ART death and migration, self-cure */
                            false_pos*tART_s*Lsn_A[i][j][l] + false_pos*Lsn_H[i][j]*HIV_ART*A_start[l]*(1-tpos_s); /* False positive, Rx and ART */ 

            dLsp_A[i][j][l] = - m_b[i]*Lsp_A[i][j][l] + /* Death */
                            LmpA_to_LspA + PTpA_to_LspA - LspA_to_LmpA - LspA_to_NspA - LspA_to_IspA - LspA_to_NmpA - LspA_to_ImpA + /* Infection and disease */                            
                            HIV_ART*A_start[l]*(NsnH_first_success + NsnH_second_success + NspH_first_success + NspH_second_success + IsnH_first_success + IsnH_second_success + IspH_first_success + IspH_second_success) + /* Rx and linked to ART */
                            NsnA_first_success + NsnA_second_success + NspA_first_success + NspA_second_success + IsnA_first_success + IsnA_second_success + IspA_first_success + IspA_second_success + /* Rx */
                            ART_prop[i][j]*A_start[l]*Lsp_H[i][j] + A_prog[l]*Lsp_A[i][j][l-1] - A_prog[l+1]*Lsp_A[i][j][l] - /* ART initation and progression */ 
                            up_A_mort[l][j][i]*Lsp_A[i][j][l] + (Lsp_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) + r_H*(Isp_A[i][j][l]+Nsp_A[i][j][l]) - /* ART death and migration, self-cure */
                            false_pos*tART_s*Lsp_A[i][j][l] + false_pos*Lsp_H[i][j]*HIV_ART*A_start[l]*(1-tpos_s); /* False positive, Rx and ART */ 

            dLmn_A[i][j][l] = - m_b[i]*Lmn_A[i][j][l] + /* Deaths */ 
                            SA_to_LmnA + LsnA_to_LmnA + PTnA_to_LmnA - LmnA_to_LsnA - LmnA_to_NsnA - LmnA_to_IsnA - LmnA_to_NmnA - LmnA_to_ImnA + /* Infection and disease */ 
                            ART_prop[i][j]*A_start[l]*Lmn_H[i][j] + A_prog[l]*Lmn_A[i][j][l-1] - A_prog[l+1]*Lmn_A[i][j][l] - /* ART initation and progression */  
                            up_A_mort[l][j][i]*Lmn_A[i][j][l] + (Lmn_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) + r_H*(Imn_A[i][j][l]+Nmn_A[i][j][l]) + /* ART death and migration, self-cure */
                            false_pos*Lmn_H[i][j]*HIV_ART*A_start[l]; /* Link to ART for false positive TB cases */

            dLmp_A[i][j][l] = - m_b[i]*Lmp_A[i][j][l] + /* Deaths */
                            LspA_to_LmpA + PTpA_to_LmpA - LmpA_to_LspA - LmpA_to_NspA - LmpA_to_IspA - LmpA_to_NmpA - LmpA_to_ImpA + /* Infection and disease */
                            HIV_ART*A_start[l]*(NmnH_first_success + NmnH_second_success + NmpH_first_success + NmpH_second_success + ImnH_first_success + ImnH_second_success + ImpH_first_success + ImpH_second_success) + /* Rx and linked to ART */
                            NmnA_first_success + NmnA_second_success + NmpA_first_success + NmpA_second_success + ImnA_first_success + ImnA_second_success + ImpA_first_success + ImpA_second_success + /* Rx */
                            ART_prop[i][j]*A_start[l]*Lmp_H[i][j] + A_prog[l]*Lmp_A[i][j][l-1] - A_prog[l+1]*Lmp_A[i][j][l] - /* ART initation and progression */  
                            up_A_mort[l][j][i]*Lmp_A[i][j][l] + (Lmp_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) + r_H*(Imp_A[i][j][l]+Nmp_A[i][j][l]) + /* ART death and migration, self-cure */
                            false_pos*Lmp_H[i][j]*HIV_ART*A_start[l]; /* Link to ART for false positive TB cases */

            dNsn_A[i][j][l] = - m_b[i]*Nsn_A[i][j][l] + /* Births */ 
                            SA_to_NsnA + LsnA_to_NsnA + LmnA_to_NsnA + PTnA_to_NsnA - /* Disease */
                            NsnA_pos + NsnA_lost + /* Diagnosis, pre Rx lost */
                            ART_prop[i][j]*A_start[l]*Nsn_H[i][j] + A_prog[l]*Nsn_A[i][j][l-1] - A_prog[l+1]*Nsn_A[i][j][l] - /* ART initation and progression */ 
                            up_A_mort[l][j][i]*Nsn_A[i][j][l] + (Nsn_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) - (theta_H + r_H + muN_H_A[i][l])*Nsn_A[i][j][l]; /* ART death and migration, sm conversion, self-cure, TB death */

            dNsp_A[i][j][l] = - m_b[i]*Nsp_A[i][j][l] + /* Deaths */ 
                            LspA_to_NspA + LmpA_to_NspA + PTpA_to_NspA - /* Disease */
                            NspA_pos + NspA_lost + NsnA_first_fail + NsnA_second_fail + NspA_first_fail + NspA_second_fail + /* Diagnosis, pre Rx lost, failed Rx */ 
                            HIV_ART*A_start[l]*(NsnH_first_fail + NsnH_second_fail + NspH_first_fail + NspH_second_fail) + /* ART inititation for Rx fail */
                            ART_prop[i][j]*A_start[l]*Nsp_H[i][j] + A_prog[l]*Nsp_A[i][j][l-1] - A_prog[l+1]*Nsp_A[i][j][l] - /* ART initation and progression */
                            up_A_mort[l][j][i]*Nsp_A[i][j][l] + (Nsp_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) - (theta_H + r_H + muN_H_A[i][l])*Nsp_A[i][j][l]; /* ART death and migration, sm conversion, self-cure, TB death */

            dNmn_A[i][j][l] = - m_b[i]*Nmn_A[i][j][l] + /* Deaths */
                            SA_to_NmnA + LsnA_to_NmnA + LmnA_to_NmnA + PTnA_to_NmnA - /* Disease */
                            NmnA_pos + NmnA_lost + /* Diagnosis, pre Rx lost */
                            ART_prop[i][j]*A_start[l]*Nmn_H[i][j] + A_prog[l]*Nmn_A[i][j][l-1] - A_prog[l+1]*Nmn_A[i][j][l] - /* ART initation and progression */
                            up_A_mort[l][j][i]*Nmn_A[i][j][l] + (Nmn_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) - (theta_H + r_H + muN_H_A[i][l])*Nmn_A[i][j][l]; /* ART death and migration, sm conversion, self-cure, TB death */

            dNmp_A[i][j][l] = - m_b[i]*Nmp_A[i][j][l] + /* Deaths */
                            LspA_to_NmpA + LmpA_to_NmpA + PTpA_to_NmpA - /* Disease */
                            NmpA_pos + NmpA_lost + NmnA_first_fail + NmnA_second_fail + NmpA_first_fail + NmpA_second_fail + NsnA_res + NspA_res + /* Diagnosis, pre Rx lost, failed Rx, acquired resistance */
                            HIV_ART*A_start[l]*(NmnH_first_fail + NmnH_second_fail + NmpH_first_fail + NmpH_second_fail + NsnH_res + NspH_res) + /* ART initation for Rx fail */     
                            ART_prop[i][j]*A_start[l]*Nmp_H[i][j] + A_prog[l]*Nmp_A[i][j][l-1] - A_prog[l+1]*Nmp_A[i][j][l] - /* ART initation and progression */ 
                            up_A_mort[l][j][i]*Nmp_A[i][j][l] + (Nmp_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) - (theta_H + r_H + muN_H_A[i][l])*Nmp_A[i][j][l]; /* ART death and migration, sm conversion, self-cure, TB death */

            dIsn_A[i][j][l] = - m_b[i]*Isn_A[i][j][l] + /* Deaths */
                            SA_to_IsnA + LsnA_to_IsnA + LmnA_to_IsnA + PTnA_to_IsnA - /* Disease */
                            IsnA_pos + IsnA_lost + /* Diagnosis, pre Rx lost */
                            ART_prop[i][j]*A_start[l]*Isn_H[i][j] + A_prog[l]*Isn_A[i][j][l-1] - A_prog[l+1]*Isn_A[i][j][l] - /* ART initation and progression */  
                            up_A_mort[l][j][i]*Isn_A[i][j][l] + (Isn_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) + theta_H*Nsn_A[i][j][l] - (r_H + muI_H_A[i][l])*Isn_A[i][j][l]; /* ART death and migration, sm conversion, self-cure, TB death */

            dIsp_A[i][j][l] = - m_b[i]*Isp_A[i][j][l] + /* Deaths */
                            LspA_to_IspA + LmpA_to_IspA + PTpA_to_IspA - /* Disease */
                            IspA_pos + IspA_lost + IsnA_first_fail + IsnA_second_fail + IspA_first_fail + IspA_second_fail + /* Diagnosis, pre Rx lost, failed Rx */
                            HIV_ART*A_start[l]*(IsnH_first_fail + IsnH_second_fail + IspH_first_fail + IspH_second_fail) + /* ART initation for Rx fail */
                            ART_prop[i][j]*A_start[l]*Isp_H[i][j] + A_prog[l]*Isp_A[i][j][l-1] - A_prog[l+1]*Isp_A[i][j][l] - /* ART initation and progression */ 
                            up_A_mort[l][j][i]*Isp_A[i][j][l] + (Isp_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) + theta_H*Nsp_A[i][j][l] - (r_H + muI_H_A[i][l])*Isp_A[i][j][l]; /* ART death and migration, sm conversion, self-cure, TB death */

            dImn_A[i][j][l] = - m_b[i]*Imn_A[i][j][l] + /* Deaths */
                            SA_to_ImnA + LsnA_to_ImnA + LmnA_to_ImnA + PTnA_to_ImnA - /* Disease */
                            ImnA_pos + ImnA_lost + /* Diagnosis, pre Rx lost */
                            ART_prop[i][j]*A_start[l]*Imn_H[i][j] + A_prog[l]*Imn_A[i][j][l-1] - A_prog[l+1]*Imn_A[i][j][l] - /* ART initation and progression */ 
                            up_A_mort[l][j][i]*Imn_A[i][j][l] + (Imn_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) + theta_H*Nmn_A[i][j][l] - (r_H + muI_H_A[i][l])*Imn_A[i][j][l]; /* ART death and migration,  sm conversion, self-cure, TB death */

            dImp_A[i][j][l] = - m_b[i]*Imp_A[i][j][l] + /* Deaths */
                            LspA_to_ImpA + LmpA_to_ImpA + PTpA_to_ImpA - /* Disease */
                            ImpA_pos + ImpA_lost + ImnA_first_fail + ImnA_second_fail + ImpA_first_fail + ImpA_second_fail + IsnA_res + IspA_res + /* Diagnosis, pre Rx lost, failed Rx, acquired resistance */
                            HIV_ART*A_start[l]*(ImnH_first_fail + ImnH_second_fail + ImpH_first_fail + ImpH_second_fail + IsnH_res + IspH_res) + /* ART initation for Rx fail */     
                            ART_prop[i][j]*A_start[l]*Imp_H[i][j] + A_prog[l]*Imp_A[i][j][l-1] - A_prog[l+1]*Imp_A[i][j][l] - /* ART initation and progression */ 
                            up_A_mort[l][j][i]*Imp_A[i][j][l] + (Imp_A[i][j][l]/tot_age[i])*(forc[iz+116]/5) + theta_H*Nmp_A[i][j][l] - (r_H + muI_H_A[i][l])*Imp_A[i][j][l]; /* ART death and migration, sm conversion, self-cure, TB death */           

            dPTn_A[i][j][l] = -m_b[i]*PTn_A[i][j][j] - /* Deaths */
                            PTnA_to_LsnA - PTnA_to_NsnA - PTnA_to_IsnA - PTnA_to_LmnA - PTnA_to_NmnA - PTnA_to_ImnA + /* Infection and disease */
                            false_pos*Lsn_A[i][j][l]*tART_s + false_pos*HIV_ART*A_start[l]*tpos_s*Lsn_H[i][j] + false_pos*PTn_H[i][j]*HIV_ART*A_start[l] + /* Incorrect Rx for latent infected, linked to ART*/ 
                            ART_prop[i][j]*A_start[l]*PTn_H[i][j] + A_prog[l]*PTn_A[i][j][l-1] - A_prog[l+1]*PTn_A[i][j][l] - /* ART initation and progression */ 
                            up_A_mort[l][j][i]*PTn_A[i][j][l] + (PTn_A[i][j][l]/tot_age[i])*(forc[iz+116]/5); /* ART death and migration */

            dPTp_A[i][j][l] = - m_b[i]*PTp_A[i][j][l] - /* Deaths */
                            PTpA_to_LspA - PTpA_to_NspA - PTpA_to_IspA - PTpA_to_LmpA - PTpA_to_NmpA - PTpA_to_ImpA + /* Infection and disease */
                            false_pos*Lsp_A[i][j][l]*tART_s + false_pos*HIV_ART*A_start[l]*tpos_s*Lsp_H[i][j] + false_pos*PTp_H[i][j]*HIV_ART*A_start[l] + /* Incorrect Rx for latent infected, linked to ART*/   
                            ART_prop[i][j]*A_start[l]*PTp_H[i][j] + A_prog[l]*PTp_A[i][j][l-1] - A_prog[l+1]*PTp_A[i][j][l] - /* ART initation and progression */
                            up_A_mort[l][j][i]*PTp_A[i][j][l] + (PTp_A[i][j][l]/tot_age[i])*(forc[iz+116]/5); /* ART death and migration */  
        
            TB_cases_ART_age[i][j][l] = (v_age_A[i][j][l]*(1-sig_H) + FS*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H))*Lsn_A[i][j][l] + FS*a_age_A[i][j][l]*(1-sig_H)*(S_A[i][j][l] + (1-p_A[j][l])*(Lmn_A[i][j][l] + PTn_A[i][j][l])) +
                                      (v_age_A[i][j][l]*(1-sig_H) + FS*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H))*Lsp_A[i][j][l] + FS*a_age_A[i][j][l]*(1-sig_H)*(1-p_A[j][l])*(Lmp_A[i][j][l] + PTp_A[i][j][l]) +
                                      (v_age_A[i][j][l]*(1-sig_H) + FM*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H))*Lmn_A[i][j][l] + FM*a_age_A[i][j][l]*(1-sig_H)*(S_A[i][j][l] + (1-p_A[j][l])*(Lsn_A[i][j][l] + PTn_A[i][j][l])) +
                                      (v_age_A[i][j][l]*(1-sig_H) + FM*a_age_A[i][j][l]*(1-p_A[j][l])*(1-sig_H))*Lmp_A[i][j][l] + FM*a_age_A[i][j][l]*(1-sig_H)*(1-p_A[j][l])*(Lsp_A[i][j][l] + PTp_A[i][j][l]) +
                                      (v_age_A[i][j][l]*sig_H + FS*a_age_A[i][j][l]*sig_H*(1-p_A[j][l]))*Lsn_A[i][j][l] + FS*a_age_A[i][j][l]*sig_H*(S_A[i][j][l] + (1-p_A[j][l])*(Lmn_A[i][j][l] + PTn_A[i][j][l])) +
                                      (v_age_A[i][j][l]*sig_H + FS*a_age_A[i][j][l]*sig_H*(1-p_A[j][l]))*Lsp_A[i][j][l] + FS*a_age_A[i][j][l]*sig_H*(1-p_A[j][l])*(Lmp_A[i][j][l] + PTp_A[i][j][l]) +
                                      (v_age_A[i][j][l]*sig_H + FM*a_age_A[i][j][l]*sig_H*(1-p_A[j][l]))*Lmn_A[i][j][l] + FM*a_age_A[i][j][l]*sig_H*(S_A[i][j][l] + (1-p_A[j][l])*(Lsn_A[i][j][l] + PTn_A[i][j][l])) +
                                      (v_age_A[i][j][l]*sig_H + FM*a_age_A[i][j][l]*sig_H*(1-p_A[j][l]))*Lmp_A[i][j][l] + FM*a_age_A[i][j][l]*sig_H*(1-p_A[j][l])*(Lsp_A[i][j][l] + PTp_A[i][j][l]);

            TB_cases_ART = TB_cases_ART + TB_cases_ART_age[i][j][l];
        
            TB_cases_age[i] = TB_cases_age[i] + TB_cases_ART_age[i][j][l];
        
          }  /* end loop on ART */
          }  /* end if on ART equations */
        }    /* end loop on HIV */
        }    /* end if on HIV equations */
    }        /* end loop on age */

    /* Put our calculated rates of change back into ydot */

    /* HIV+ */
    for (i=0; i<n_age; i++) ydot[i] = dS[i];             
    for (i=n_age; i<n_age*2; i++) ydot[i] = dLsn[i-n_age];      
    for (i=n_age*2; i<n_age*3; i++) ydot[i] = dLsp[i-n_age*2];    /* Lsp: 34-50 */
    for (i=n_age*3; i<n_age*4; i++) ydot[i] = dLmn[i-n_age*3];    /* Lmn: 51-67 */
    for (i=n_age*4; i<n_age*5; i++) ydot[i] = dLmp[i-n_age*4];    /* Lmp: 68-84 */
    for (i=n_age*5; i<n_age*6; i++) ydot[i] = dNsn[i-n_age*5];    /* Nsn: 85-101 */
    for (i=n_age*6; i<n_age*7; i++) ydot[i] = dNsp[i-n_age*6];    /* Nsp: 102-118 */
    for (i=n_age*7; i<n_age*8; i++) ydot[i] = dNmn[i-n_age*7];    /* Nmn: 119-135 */
    for (i=n_age*8; i<n_age*9; i++) ydot[i] = dNmp[i-n_age*8];    /* Nmp: 136-152 */ 
    for (i=n_age*9; i<n_age*10; i++) ydot[i] = dIsn[i-n_age*9];   /* Isn: 153-169 */
    for (i=n_age*10; i<n_age*11; i++) ydot[i] = dIsp[i-n_age*10]; /* Isp: 170-186 */
    for (i=n_age*11; i<n_age*12; i++) ydot[i] = dImn[i-n_age*11]; /* Imn: 187-203 */
    for (i=n_age*12; i<n_age*13; i++) ydot[i] = dImp[i-n_age*12]; /* Imp: 204-220 */
    for (i=n_age*13; i<n_age*14; i++) ydot[i] = dPTn[i-n_age*13]; /* Imn: 187-203 */
    for (i=n_age*14; i<n_age*15; i++) ydot[i] = dPTp[i-n_age*14]; /* Imp: 204-220 */
    
    /* HIV+ */
    ij = n_age*n_disease;                                     
    for (j=0; j<n_HIV; j++){
      for (i=0; i<n_age; i++){  
        ydot[ij] = dS_H[i][j];
        ydot[ij+(n_age*n_HIV)] = dLsn_H[i][j];
        ydot[ij+(2*n_age*n_HIV)] = dLsp_H[i][j]; 
        ydot[ij+(3*n_age*n_HIV)] = dLmn_H[i][j];
        ydot[ij+(4*n_age*n_HIV)] = dLmp_H[i][j];  
        ydot[ij+(5*n_age*n_HIV)] = dNsn_H[i][j];
        ydot[ij+(6*n_age*n_HIV)] = dNsp_H[i][j];
        ydot[ij+(7*n_age*n_HIV)] = dNmn_H[i][j];
        ydot[ij+(8*n_age*n_HIV)] = dNmp_H[i][j];
        ydot[ij+(9*n_age*n_HIV)] = dIsn_H[i][j];
        ydot[ij+(10*n_age*n_HIV)] = dIsp_H[i][j];
        ydot[ij+(11*n_age*n_HIV)] = dImn_H[i][j];
        ydot[ij+(12*n_age*n_HIV)] = dImp_H[i][j];
        ydot[ij+(13*n_age*n_HIV)] = dPTn_H[i][j];
        ydot[ij+(14*n_age*n_HIV)] = dPTp_H[i][j];
        ij = ij+1;
      }
    }
    /* HIV+, on ART */
    ij = (n_HIV+1)*n_disease*n_age;   
    for (l=0; l<n_ART; l++){                          
      for (j=0; j<n_HIV; j++){
        for (i=0; i<n_age; i++){
          ydot[ij] = dS_A[i][j][l];
          ydot[ij+(n_age*n_ART*n_HIV)] = dLsn_A[i][j][l];
          ydot[ij+(2*n_age*n_ART*n_HIV)] = dLsp_A[i][j][l];
          ydot[ij+(3*n_age*n_ART*n_HIV)] = dLmn_A[i][j][l];
          ydot[ij+(4*n_age*n_ART*n_HIV)] = dLmp_A[i][j][l];  
          ydot[ij+(5*n_age*n_ART*n_HIV)] = dNsn_A[i][j][l];
          ydot[ij+(6*n_age*n_ART*n_HIV)] = dNsp_A[i][j][l];
          ydot[ij+(7*n_age*n_ART*n_HIV)] = dNmn_A[i][j][l];
          ydot[ij+(8*n_age*n_ART*n_HIV)] = dNmp_A[i][j][l];
          ydot[ij+(9*n_age*n_ART*n_HIV)] = dIsn_A[i][j][l];
          ydot[ij+(10*n_age*n_ART*n_HIV)] = dIsp_A[i][j][l];
          ydot[ij+(11*n_age*n_ART*n_HIV)] = dImn_A[i][j][l];
          ydot[ij+(12*n_age*n_ART*n_HIV)] = dImp_A[i][j][l];
          ydot[ij+(13*n_age*n_ART*n_HIV)] = dPTn_A[i][j][l];
          ydot[ij+(14*n_age*n_ART*n_HIV)] = dPTp_A[i][j][l];
          ij = ij+1;
        }
      }
    }
    
    /* Calculate notifications, treatments etc */
    double DS_correct = 0;
    double DS_incorrect = 0; 
    double MDR_correct = 0; 
    double MDR_incorrect = 0;
    double FP = 0; 
    
    for (i=0; i<n_age; i++){
    
      /* ## DS notifications ## */
      
      /* Correct */
      DS_correct =  DS_correct + rel_d*kneg*se_N_neg*(dstneg_n*sp_m_neg + (1-dstneg_n))*l_s*Nsn[i] + /* s-, ds, new */
                                 rel_d*kneg*se_N_neg*(dstneg_p*sp_m_neg + (1-dstneg_p))*l_s*Nsp[i] + /* s-, ds, prev */                                        
                                 kneg*se_I_neg*(dstneg_n*sp_m_neg + (1-dstneg_n))*l_s*Isn[i] +       /* s-, ds, new */
                                 kneg*se_I_neg*(dstneg_p*sp_m_neg + (1-dstneg_p))*l_s*Isp[i];        /* s-, ds, prev */  
             
      /* Incorrect - should be MDR */    
      DS_incorrect = DS_incorrect + rel_d*kneg*se_N_neg*(dstneg_n*(1-se_m_neg) + (1-dstneg_n))*l_s*Nmn[i] + /* s-, dr, new */
                                    rel_d*kneg*se_N_neg*(dstneg_p*(1-se_m_neg) + (1-dstneg_p))*l_s*Nmp[i] + /* s-, dr, prev */                                        
                                    kneg*se_I_neg*(dstneg_n*(1-se_m_neg) + (1-dstneg_n))*l_s*Imn[i] +       /* s-, dr, new */
                                    kneg*se_I_neg*(dstneg_p*(1-se_m_neg) + (1-dstneg_p))*l_s*Imp[i];        /* s-, dr, prev */                            
    
      /* ## MDR notifications ## */
    
      /* Correct */ 
      MDR_correct = MDR_correct + rel_d*kneg*se_N_neg*l_m*(dstneg_n*Nmn[i] + dstneg_p*Nmp[i]) + /* s-, dr */
                                  kneg*se_I_neg*l_m*(dstneg_n*Imn[i] + dstneg_p*Imp[i]);        /* s+, dr */
                          
      /* Incorrect - should be DS */
      MDR_incorrect = MDR_incorrect + rel_d*kneg*se_N_neg*(1-sp_m_neg)*l_m*(dstneg_n*Nsn[i] + dstneg_p*Nsp[i]) + /* s-, ds */
                                      kneg*se_I_neg*(1-sp_m_neg)*l_m*(dstneg_n*Isn[i] + dstneg_p*Isp[i]);        /* s+, ds */ 
                          
      /* ## False positives - don't have TB ## */
      FP = FP + health*kneg*(1-sp_I_neg*sp_N_neg)*l_s*(S[i]+Lsn[i]+Lsp[i]+Lmn[i]+Lmp[i]+PTn[i]+PTp[i]);
               
      for (j=0; j<n_HIV; j++){
                          
          DS_correct = DS_correct + rel_d*kpos*se_N_pos*(dstpos_n*sp_m_pos + (1-dstpos_n))*l_s*Nsn_H[i][j] +
                                    rel_d*kpos*se_N_pos*(dstpos_p*sp_m_pos + (1-dstpos_p))*l_s*Nsp_H[i][j] +                                         
                                    kpos*se_I_pos*(dstpos_n*sp_m_pos + (1-dstpos_n))*l_s*Isn_H[i][j] +       
                                    kpos*se_I_pos*(dstpos_p*sp_m_pos + (1-dstpos_p))*l_s*Isp_H[i][j];
                 
                 
          DS_incorrect = DS_incorrect + rel_d*kpos*se_N_pos*(dstpos_n*(1-se_m_pos) + (1-dstpos_n))*l_s*Nmn_H[i][j] + 
                                        rel_d*kpos*se_N_pos*(dstpos_p*(1-se_m_pos) + (1-dstpos_p))*l_s*Nmp_H[i][j] +                                        
                                        kpos*se_I_pos*(dstpos_n*(1-se_m_pos) + (1-dstpos_n))*l_s*Imn_H[i][j] +       
                                        kpos*se_I_pos*(dstpos_p*(1-se_m_pos) + (1-dstpos_p))*l_s*Imp_H[i][j];  
                 
          MDR_correct = MDR_correct + rel_d*kpos*se_N_pos*l_m*(dstpos_n*Nmn_H[i][j] + dstpos_p*Nmp_H[i][j]) + 
                                      kpos*se_I_pos*l_m*(dstpos_n*Imn_H[i][j] + dstpos_p*Imp_H[i][j]);  
                                      
          MDR_incorrect = MDR_incorrect + rel_d*kpos*se_N_pos*(1-sp_m_pos)*l_m*(dstpos_n*Nsn_H[i][j] + dstpos_p*Nsp_H[i][j]) + 
                                          kpos*se_I_pos*(1-sp_m_pos)*l_m*(dstpos_n*Isn_H[i][j] + dstpos_p*Isp_H[i][j]);  
                 
          FP = FP + health*kpos*(1-sp_I_pos*sp_N_pos)*l_s*(S_H[i][j]+Lsn_H[i][j]+Lsp_H[i][j]+Lmn_H[i][j]+Lmp_H[i][j]+PTn_H[i][j]+PTp_H[i][j]);
      
          for (l=0; l<n_ART; l++){
            
            DS_correct = DS_correct + rel_d*kpos*se_N_pos*(dstpos_n*sp_m_pos + (1-dstpos_n))*l_s*Nsn_A[i][j][l] +
                                      rel_d*kpos*se_N_pos*(dstpos_p*sp_m_pos + (1-dstpos_p))*l_s*Nsp_A[i][j][l] +                                         
                                      kpos*se_I_pos*(dstpos_n*sp_m_pos + (1-dstpos_n))*l_s*Isn_A[i][j][l] +       
                                      kpos*se_I_pos*(dstpos_p*sp_m_pos + (1-dstpos_p))*l_s*Isp_A[i][j][l];
                 
                 
            DS_incorrect = DS_incorrect + rel_d*kpos*se_N_pos*(dstpos_n*(1-se_m_pos) + (1-dstpos_n))*l_s*Nmn_A[i][j][l] + 
                                          rel_d*kpos*se_N_pos*(dstpos_p*(1-se_m_pos) + (1-dstpos_p))*l_s*Nmp_A[i][j][l] +                                        
                                          kpos*se_I_pos*(dstpos_n*(1-se_m_pos) + (1-dstpos_n))*l_s*Imn_A[i][j][l] +       
                                          kpos*se_I_pos*(dstpos_p*(1-se_m_pos) + (1-dstpos_p))*l_s*Imp_A[i][j][l];  
                 
            MDR_correct = MDR_correct + rel_d*kpos*se_N_pos*l_m*(dstpos_n*Nmn_A[i][j][l] + dstpos_p*Nmp_A[i][j][l]) + 
                                        kpos*se_I_pos*l_m*(dstpos_n*Imn_A[i][j][l] + dstpos_p*Imp_A[i][j][l]);  
                                      
            MDR_incorrect = MDR_incorrect + rel_d*kpos*se_N_pos*(1-sp_m_pos)*l_m*(dstpos_n*Nsn_A[i][j][l] + dstpos_p*Nsp_A[i][j][l]) + 
                                            kpos*se_I_pos*(1-sp_m_pos)*l_m*(dstpos_n*Isn_A[i][j][l] + dstpos_p*Isp_A[i][j][l]);  
                 
            FP = FP + health*kpos*(1-sp_I_pos*sp_N_pos)*l_s*(S_A[i][j][l]+Lsn_A[i][j][l]+Lsp_A[i][j][l]+Lmn_A[i][j][l]+Lmp_A[i][j][l]+PTn_A[i][j][l]+PTp_A[i][j][l]);  
            
          }
      
      }                  
    }                          
              
    /* Finally assign the things we want to use in R (in addition to the state variables) to yout */
    /* The ode call in R needs to define the number of these and give names */
    yout[0] = Total;
    yout[1] = Total_S;
    yout[2] = Total_Ls;
    yout[3] = Total_Lm;
    yout[4] = Total_L;
    yout[5] = Total_Ns_N + Total_Ns_H;
    yout[6] = Total_Nm_N + Total_Nm_H;
    yout[7] = Total_N;
    yout[8] = Total_Is_N + Total_Is_H;
    yout[9] = Total_Im_N + Total_Im_H;
    yout[10] = Total_I;
    yout[11] = Total_DS;
    yout[12] = Total_MDR;
    yout[13] = FS*100;
    yout[14] = FM*100;
    yout[15] = CD4_dist_all[0];
    yout[16] = CD4_dist_all[1];
    yout[17] = CD4_dist_all[2];
    yout[18] = CD4_dist_all[3];
    yout[19] = CD4_dist_all[4];
    yout[20] = CD4_dist_all[5];
    yout[21] = CD4_dist_all[6];
    yout[22] = CD4_dist_ART_all[0];
    yout[23] = CD4_dist_ART_all[1];
    yout[24] = CD4_dist_ART_all[2];
    yout[25] = CD4_dist_ART_all[3];
    yout[26] = CD4_dist_ART_all[4];
    yout[27] = CD4_dist_ART_all[5];
    yout[28] = CD4_dist_ART_all[6];
    yout[29] = TB_deaths_tot;
    yout[30] = TB_deaths_neg_tot;
    yout[31] = TB_deaths_pos_tot;
    yout[32] = TB_cases_neg;
    yout[33] = TB_cases_pos;
    yout[34] = TB_cases_ART;
    yout[35] = births;
    yout[36] = Tot_deaths;
    yout[37] = DS_correct;
    yout[38] = DS_incorrect;
    yout[39] = MDR_correct;
    yout[40] = MDR_incorrect;
    yout[41] = FP;

}

