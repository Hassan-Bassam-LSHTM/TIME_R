# Create forcing functions - these are time varying inputs used in the C code ####################################

# They need to consist of x,y pairs where x is the year and y is the value in that year
# They can be entered as numeric values or using the "logcurve" function to simulate a logistic curve:

# logcurve(base, target, base_year, target_year, growth, shape)
# "base" and "target" are the initial and final values
# "shape" determines the symmetry of the curve
# with "shape=1" the curve is symmetric around the midpoint of "base_year" and "target_year"
# "growth" determines the steepness of the curve
# "ss" determines how often to output values

# BCG coverage - currently assume 100% at all times
BCG_cov <- cbind(seq(0,2050),1)

# Case detection rate by HIV status (neg/pos)
# 100 equates to average of one year from disease to presentation at health services (in the absence of competing events)
kneg <- logcurve(130,140,1998,2010,2,2)
kpos <- logcurve(130,140,1998,2010,2,2)

# Relative rate of detection of smear neg - assume those with smear negative disease are less likely to present to health services
rel_d <- cbind(seq(1970,2050),0.8) 

# Relative rate of presentation of healthy individuals 
health <- cbind(seq(1970,2050),0.03) 

# DST coverage among new and previously treated cases (proportion of diagnosed given DST)
# Based on targets values - Assume no difference by HIV. 2004 values based on Dowdy et al (taken from data in Northern Cape) with linear scale up from 1980. 
# 2011/12 values based on data from Kate (post LPA). Then scale up to Xpert in 2014

dstneg_n <- cbind(c(1970,2009,2010,2011,2012,2013,2014,2050),c(0,0,50,50,50,72.5,95,95)/100) 
dstpos_p <- cbind(c(1970,1979,seq(1980,2014),2050),c(0,0,0.068252,1.607075,3.145898,4.684721,6.223543,7.762366,9.301189,10.840012,12.378835,13.917657,15.45648,16.995303,18.534126,20.072949,21.611772,23.150595,
                                                     24.689417,26.22824,27.767063,29.305886,30.844709,32.383531,33.922354,35.461177,37,37,39.6,42.2,44.8,47.4,50,50,50,72.5,95,95)/100)
dstpos_n <- cbind(c(1970,2009,2010,2011,2012,2013,2014,2050),c(0,0,50,50,50,72.5,95,95)/100)
dstpos_p <- cbind(c(1970,1979,seq(1980,2014),2050),c(0,0,0.068252,1.607075,3.145898,4.684721,6.223543,7.762366,9.301189,10.840012,12.378835,13.917657,15.45648,16.995303,18.534126,20.072949,21.611772,23.150595,
                                     24.689417,26.22824,27.767063,29.305886,30.844709,32.383531,33.922354,35.461177,37,37,39.6,42.2,44.8,47.4,50,50,50,72.5,95,95)/100)

# Sens (se) and spec (sp) of algorithms for sm+ (I), sm- (N) and DST testing (m)
se_I_neg <- cbind(c(1970,2011,2015,2050),c(0.84,0.84,0.82,0.82)) 
se_N_neg <- cbind(c(1970,2011,2015,2050),c(0.2,0.2,0.57,0.57)) 
se_m_neg <- logcurve(100,100,1970,2010,1,2)
sp_I_neg <- cbind(c(1970,2011,2015,2050),c(0.99,0.99,0.99,0.99)) 
sp_N_neg <- cbind(c(1970,2011,2015,2050),c(0.98,0.98,0.99,0.99))
sp_m_neg <- logcurve(100,100,1970,2010,1,2)

se_I_pos <- cbind(c(1970,2011,2015,2050),c(0.84,0.84,0.82,0.82)) 
se_N_pos <- cbind(c(1970,2011,2015,2050),c(0.2,0.2,0.4,0.4)) 
se_m_pos <- logcurve(100,100,1970,2010,1,2)
sp_I_pos <- cbind(c(1970,2011,2015,2050),c(0.99,0.99,0.99,0.99)) 
sp_N_pos <- cbind(c(1970,2011,2015,2050),c(0.98,0.98,0.99,0.99))
sp_m_pos <- logcurve(100,100,1970,2010,1,2)

# Linkage to care (i.e. 1-pre-treatment loss to follow-up)
l_s <- cbind(seq(1970,2050),0.83)  # Based on .........
l_m <- cbind(seq(1970,2050),0.7) 

# Treatment success by HIV (neg, pos no ART, pos on ART) and susceptibility 
tneg_s <- cbind(seq(1970,2050),0.76) 
tpos_s <- cbind(seq(1970,2050),0.76)
tART_s <- cbind(seq(1970,2050),0.76)
tneg_m <- cbind(seq(1970,2050),0.5) 
tpos_m <- cbind(seq(1970,2050),0.5)
tART_m <- cbind(seq(1970,2050),0.5)

# Coverage of HIV testing and (subsequent) ART initiation in notified TB cases
HIV_test <- cbind(seq(1970,2050),0)
ART_link <- cbind(seq(1970,2050),0)

# Set up TB parameters ###########################################################################################

# Fitness of MDR (fit_cost), used to calculate parameter for superinfections (g)
# Both of these are passed into "parms" together with the MDR acquisition rate (e)
fit_cost=0.85
g = fit_cost/(1+fit_cost) # superinfections 
e = 0.01

# beta = contact rate; a = proportion developing primary TB; p = protection due to previous infection; v = reactivation rate; 
# sig = proportion of cases developing smear positive TB; rel_inf = relative infectiousness of smear negative cases;
# theta = rate of conversion from smear negative to smear positive; r = self cure rate; 
# mu_N = mortality for smear negative cases; mu_I = mortality for smear positive cases; 
# eff_n/p = relative efficacy of first line treatment in new/previously treated MDR cases; 

# _H indicates values for HIV+ (mu_N, mu_I, sig, r, theta and rel_inf)
# other natural history parameters for HIV+ (a,v,p) are adjusted using the rate ratio parameters RR1a etc
# ART modifies a,v,p, muN_H and muI_H by ART_TB1 etc 

# proportion primary (a), proportion smear pos (sig) and mortality rates (muN and muI) take different values for 
# adults (>15) (_a), 0-4 (_0), 5-9 (_5) and 10-14 (_10)

a_a = 0.13
sig_a = 0.4
mu_N = 0.18
mu_I = 0.2
# This RR value can be adjusted between 0.4 and 0.69 to change the burden of childhood TB
# It directly controls the proportion primary in 10-14 year olds (a_10)
# and indirectly controls all other age specific parameters using the RRs defined in "Data_load.R"
RR_a_10 = 0.4

parms <- c(beta = 22.5, 
           a_a = a_a, a0 = a_a*RR_a_10*RR_a_0, a5 = a_a*RR_a_10*RR_a_5, a10 = a_a*RR_a_10,
           v = 0.001, 
           p = 0.65, 
           sig_a = sig_a, sig0 = sig_a*RR_a_10*RR_sig_0, sig5 = sig_a*RR_a_10*RR_sig_5, sig10 = sig_a*RR_a_10*RR_sig_10, 
           rel_inf = 0.25, theta = 0.02, r = 0.25,
           mu_N = mu_N, mu_N0 = mu_N*RR_a_10*RR_mu_0, mu_N5 = mu_N, mu_N10 = mu_N,
           mu_I = mu_I, mu_I0 = mu_I*RR_a_10*RR_mu_0, mu_I5 = mu_I, mu_I10 = mu_I, 
           fit_cost = fit_cost, e = e, g=g,
           eff_n = 0.61, eff_p = 0.45, 
           muN_H = 0.40, muI_H = 0.6, RR1a = 3.2, RR2a = 1.42, RR1v = 3.2, RR2v = 1.42, RR1p = 0.6, RR2p = 1.0,
           ART_TB1 = 0.16, ART_TB2 = 0.43, ART_TB3 = 0.54, ART_mort1 = 0.15, ART_mort2 = 0.6, ART_mort3 = 0.7,
           BCG_eff = 0.72,
           sig_H = 0.3,r_H=0.16,rel_inf_H=0.1,theta_H=0.015)




