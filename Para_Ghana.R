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

# Case detection rate by HIV (neg/pos)
kneg <- logcurve(107,107,1985,2005,0.8,2)
kpos <- logcurve(250,250,1985,2005,0.8,2)

# Relative detection smear neg
rel_d <- cbind(seq(1970,2050),0.2) 

# Relative presentation healthy 
health <- cbind(seq(1970,2050),0.0035) 

# DST coverage among new and previously treated cases 
dstneg_n <- logcurve(0,0,1975,2010,1,2) 
dstneg_p <- logcurve(0,25,1975,2010,1,2) ###
dstpos_n <- logcurve(0,0,1975,2010,1,2) 
dstpos_p <- logcurve(0,25,1975,2010,1,2) ###

# Sens (se) and spec (sp) of algorithms for sm+ (I), sm- (N) and DST testing (m) - TRY AND BASE THESE ON KNOWN ALGORITHMS?
se_I_neg <- logcurve(50,50,1970,2010,1,2) 
se_N_neg <- logcurve(20.9,20.9,1970,2010,1,2)
se_m_neg <- logcurve(100,100,1970,2010,1,2)
sp_I_neg <- logcurve(99.8,99.8,1970,2010,1,2) 
sp_N_neg <- logcurve(95,95,1970,2010,1,2)
sp_m_neg <- logcurve(100,100,1970,2010,1,2)

se_I_pos <- logcurve(50,50,1970,2010,1,2)
se_N_pos <- logcurve(20.9,20.9,1970,2010,1,2)
se_m_pos <- logcurve(100,100,1970,2010,1,2)
sp_I_pos <- logcurve(99.8,99.8,1970,2010,1,2)
sp_N_pos <- logcurve(95,95,1970,2010,1,2)
sp_m_pos <- logcurve(100,100,1970,2010,1,2)

# Linkage to care
l_s <- cbind(seq(1970,2050),0.82)
l_m <- rbind(cbind(seq(1970,2013),0.4),cbind(seq(2014,2050),0.15))

# Treatment success by HIV (neg, pos no ART, pos on ART) and susceptibility
tneg_s <- cbind(seq(1970,2050),c(40.0,40.56,41.12,41.68,42.24,42.8,43.36,43.92,44.48,45.04,45.6,46.16,46.72,47.28,47.84,48.4,48.96,49.52,
                                 50.08,50.64,51.2,51.76,52.32,52.88,53.44,54.0,51.0,48.0,54.0,51.0,50.0,56.0,60.0,66.0,68.0,61.0,66.0,77.0,
                                 82.0,83.0,84.0,85.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,
                                 84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0,84.0)/100)
tpos_s <- cbind(seq(1970,2050),c(40.0,40.55,41.1,41.65,42.2,42.75,43.3,43.85,44.4,44.95,45.5,46.05,46.6,47.15,47.7,48.25,48.8,49.35,49.9,50.45,51.0,51.55,
                                 52.1,52.65,53.2,53.75,54.5,55.25,56.0,56.75,57.5,58.25,59.0,59.75,60.5,61.25,62.0,68.0,72.0,61.0,69.0,74.0,73.0,73.0,73.0,
                                 73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,
                                 73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0,73.0)/100)
tART_s <- tpos_s


tneg_m <- rbind(cbind(seq(1970,2010),0.33),cbind(seq(2011,2050),0.5))
tpos_m <- rbind(cbind(seq(1970,2010),0.33),cbind(seq(2011,2050),0.5))
tART_m <- rbind(cbind(seq(1970,2010),0.33),cbind(seq(2011,2050),0.5))

# Set up TB parameters ###########################################################################################

# Fitness of MDR (fit_cost), used to calculate parameter for superinfections (g)
# Both of these are passed into "parms" together with the MDR acquisition rate (e)
fit_cost=0.75
g = fit_cost/(1+fit_cost) # superinfections 
e = 0.017

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

parms <- c(beta = 13.1, 
           a_a = 0.115, a0 = 0.21712, a5 = 0.11546, a10 = 0.046,
           v = 0.0012, 
           p = 0.65, 
           sig_a = 0.40, sig0 = 0.0608, sig5 = 0.0368, sig10 = 0.0752, rel_inf = 0.25, theta = 0.01, r = 0.2,
           mu_N = 0.18, mu_N0 = 0.35784, mu_N5 = 0.21, mu_N10 = 0.21, 
           mu_I = 0.20, mu_I0 = 0.5112, mu_I5 = 0.30, mu_I10 = 0.30, 
           fit_cost = fit_cost, e = e, g=g,
           eff_n = 0.61, eff_p = 0.40, 
           muN_H = 0.50, muI_H = 0.82, RR1a = 2.6, RR2a = 1.42, RR1v = 3.2, RR2v = 3.5, RR1p = 0.5, RR2p = 2,
           ART_TB1 = 0.1575, ART_TB2 = 0.4275, ART_TB3 = 0.54, ART_mort1 = 0.1070, ART_mort2 = 0.5070, ART_mort3 = 0.64,
           BCG_eff = 0.72,
           sig_H = 0.327,r_H=0.1,rel_inf_H=0.25,theta_H=0.0225)


