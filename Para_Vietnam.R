# Create forcing functions - these are time varying inputs used in the C code ####################################

# They need to consist of x,y pairs where x is the year and y is the value in that year
# They can be entered as numeric values or using the "logcurve" function to simulate a logistic curve:

# logcurve(base, target, base_year, target_year, growth, shape, ss)
# "base" and "target" are the initial and final values
# "shape" determines the symmetry of the curve
# with "shape=1" the curve is symmetric around the midpoint of "base_year" and "target_year"
# "growth" determines the steepness of the curve
# "ss" determine show often to output values

# BCG coverage - currently assume 100% at all times
BCG_cov <- cbind(seq(0,2050),1)

# Case detection rate by HIV (neg/pos)
kneg <- logcurve(50,100,1985,2005,0.8,2)
kpos <- logcurve(50,100,1985,2005,0.8,2) 

# Relative detection smear neg
rel_d <- cbind(seq(1970,2050),0.4)

# Relative presentation healthy 
health <- cbind(seq(1970,2050),0.015)

# DST coverage among new and previously treated cases 
dstneg_n <- logcurve(0,10,1985,2014,1,2)
dstneg_p <- logcurve(0,99,1985,2014,1,2)
dstpos_n <- logcurve(0,10,1985,2014,1,2)
dstpos_p <- logcurve(0,99,1985,2014,1,2)

# Sens (se) and spec (sp) of algorithms for sm+ (I), sm- (N) and DST testing (m)
se_I_neg <- logcurve(100,100,1970,2010,1,2)
se_N_neg <- logcurve(100,100,1970,2010,1,2)
se_m_neg <- logcurve(100,100,1970,2010,1,2)
sp_I_neg <- logcurve(100,100,1970,2010,1,2)
sp_N_neg <- logcurve(100,100,1970,2010,1,2)
sp_m_neg <- logcurve(100,100,1970,2010,1,2)

se_I_pos <- logcurve(100,100,1970,2010,1,2)
se_N_pos <- logcurve(100,100,1970,2010,1,2)
se_m_pos <- logcurve(100,100,1970,2010,1,2)
sp_I_pos <- logcurve(100,100,1970,2010,1,2)
sp_N_pos <- logcurve(100,100,1970,2010,1,2)
sp_m_pos <- logcurve(100,100,1970,2010,1,2)

# Linkage to care
l_s <- cbind(seq(1970,2050),0.5)
l_m <- cbind(seq(1970,2050),0.5)

# Treatment success by HIV (neg, pos no ART, pos on ART) and susceptibility
tneg_s <- cbind(seq(1970,2050),0.5)
tpos_s <- cbind(seq(1970,2050),0.5)
tART_s <- cbind(seq(1970,2050),0.5)
tneg_m <- cbind(seq(1970,2050),0.5)
tpos_m <- cbind(seq(1970,2050),0.5)
tART_m <- cbind(seq(1970,2050),0.5)

# Set up TB parameters ###########################################################################################

# Fitness of MDR (fit_cost), used to calculate parameter for superinfections (g)
# Both of these are passed into "parms" together with the MDR acquisition rate (e)
fit_cost=0.73
g = fit_cost/(1+fit_cost) # superinfections 
e = 0.014

# beta = contact rate; a = proportion developing primary TB; p = protection due to previous infection; v = reactivation rate; 
# sig = proportion of cases developing smear positive TB; rel_inf = relative infectiousness of smear negative cases;
# theta = rate of conversion from smear negative to smear positive; r = self cure rate; 
# mu_N = mortality for smear negative cases; mu_I = mortality for smear positive cases; 
# eff_n/p = relative efficacy of first line treatment in new/previously treated MDR cases; 

# _H indicates values for HIV+ (mu_N, mu_I, sig, r and rel_inf)
# other natural history parameters for HIV+ (a,v,p) are adjusted using the rate ratio parameters RR1a etc
# ART modifies a,v,p, muN_H and muI_H by ART_TB1 etc 

# proportion primary (a), proportion smear pos (sig) and mortality rates (muN and muI) take different values for 
# adults (>15) (_a), 0-4 (_0), 5-9 (_5) and 10-14 (_10)

a_a = 0.115
sig_a = 0.45
mu_N = 0.21
mu_I = 0.3
# This RR value can be adjusted between 0.4 and 0.69 to change the burden of childhood TB
# It directly controls the proportion primary in 10-14 year olds (a_10)
# and indirectly controls all other age specific parameters using the RRs defined in "Data_load.R"
RR_a_10 = 0.47

parms <- c(beta = 15, 
           a_a = a_a, a0 = a_a*RR_a_10*RR_a_0, a5 = a_a*RR_a_10*RR_a_5, a10 = a_a*RR_a_10,
           v = 0.001, 
           p = 0.65, 
           sig_a = sig_a, sig0 = sig_a*RR_a_10*RR_sig_0, sig5 = sig_a*RR_a_10*RR_sig_5, sig10 = sig_a*RR_a_10*RR_sig_10,
           rel_inf = 0.22, theta = 0.015, r = 0.2,
           mu_N = mu_N, mu_N0 = mu_N*RR_a_10*RR_mu_0, mu_N5 = mu_N, mu_N10 = mu_N,
           mu_I = mu_I, mu_I0 = mu_I*RR_a_10*RR_mu_0, mu_I5 = mu_I, mu_I10 = mu_I, 
           fit_cost = fit_cost, e = e, g=g,
           eff_n = 0.61, eff_p = 0.45, 
           muN_H = 0.42, muI_H = 0.60, RR1a = 2.6, RR2a = 1.36, RR1v = 2.6, RR2v = 1.36, RR1p = 0.8, RR2p = 1.3,
           ART_TB1 = 0.204, ART_TB2 = 0.554, ART_TB3 = 0.7, ART_mort1 = 0.232, ART_mort2 = 0.629, ART_mort3 = 0.795,
           BCG_eff = 0.56,
           sig_H = 0.327,r_H=0.1,rel_inf_H=0.22,theta_H=0.0225)

