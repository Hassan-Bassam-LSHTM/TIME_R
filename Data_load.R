### Loads all external data sources and where appropriate creates forcing functions

# Used to pick the country from cn
c_list <- c("Bangladesh","Ghana","South_Africa")
cntry <- c_list[cn]

# Define number of TB, HIV and ART states
n_HIV <- 7
n_ART <- 3
n_dis <- 15

## UN population data ###########################################################################################################

# Single year age groups for 1970 - used as initial population
UN_pop_start <- as.data.frame(read.table(paste("Demog/Initial_Pop_single_age.txt",sep=""),header=FALSE)) # Load UN Population data
# Load number of births
# Add total, births and deaths to pop size data
UN_pop_start_t <- cbind(UN_pop_start[,1:83],rowSums(UN_pop_start[,2:82]))
UN_pop_start_t <- UN_pop_start_t[UN_pop_start_t[,1]==cntry,]
colnames(UN_pop_start_t) <- c("Country","Year",colnames(UN_pop_start[3:83]),"Total")

## UN indicators - Crude birth rate (CBR), births, deaths #######################################################################
UN_ind <- as.data.frame(read.table("Demog/UN_indicators_all.txt",header=TRUE))
UN_ind <- UN_ind[UN_ind$Country==cntry,]
# Convert CBR into forcing function to be used in C code
birth_rate <- cbind(seq(1970,2050),approx(UN_ind$Year,UN_ind$CBR,seq(1970,2050),rule=2)$y)
# births and deaths will be used in plots later

## Mortality ####################################################################################################################

# Age specific mortality taken from demproj outputs
mort_age <- as.data.frame(read.table(paste("Demog/",cntry,"/",cntry,"_mort_age.txt",sep=""),header=FALSE)) 

mort_age[,82] <-mort_age[,82]/2 

# Convert into forcing functions to be used in C code
for (i in 1:81){  
  assign(paste("s",i,sep=""), cbind(seq(1970,2050),approx(mort_age[,1],mort_age[,i+1],seq(1970,2050),rule=2)$y))
}

## MIGRATION ####################################################################################################################

# Migration data taken from DemProj - 5 year age bins replicated in C code for single year age bins
mig <- as.data.frame(read.table(paste("Demog/",cntry,"/",cntry,"_migrants_age.txt",sep=""),header=FALSE))
temp <- c(0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80)
for (i in 1:17){
  assign(paste("mig",temp[i],sep=""),cbind(seq(1970,2050),approx(mig[,1],mig[,i+1]/1000,seq(1970,2050),rule=2)$y))
}

## MORTALITY ADJUSTMENT #########################################################################################################

# Pop adjust - to turn off population adjustment for TB/HIV deaths from 2015 onwards
pop_ad <- cbind(seq(1970,2050),c(rep(1,45),rep(0,35),0))


## HIV ##########################################################################################################################

# HIV Incidence by age and year - taken from AIM (rate per 1000)
temp <- as.data.frame(read.table(paste("HIV/",cntry,"/",cntry,"_HIV_Inc_age.txt",sep=""),header=TRUE,fill=TRUE))
# Re-arrage to get in year vs age format
HIV_Inc_age <- mat.or.vec(81,18)
HIV_Inc_age[,1] <- seq(1970,2050)
for (i in 1:81){
  j <- (i-1)*19+4
  HIV_Inc_age[i,2:18]=as.numeric(levels(temp[j:(j+16),2]))[temp[j:(j+16),2]]
}
# Convert into forcing functions - 5 year age bins replicated in C code for single year age bins
temp <- c(0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80)
for (i in 1:17){
  assign(paste("h",temp[i],sep=""),cbind(seq(1970,2050),approx(HIV_Inc_age[,1],HIV_Inc_age[,i+1]/1000,seq(1970,2050),rule=2)$y))
}

## ART ##########################################################################################################################

# Take "need for" and "number on" by age and year from AIM, convert to % of eligible on ART 

# ART need (by age and year taken from AIM)
temp <- as.data.frame(read.table(paste("HIV/",cntry,"/",cntry,"_ART_need_age.txt",sep=""),header=TRUE,fill=TRUE))
# Re-arrage to get in year vs age format
ART_need_age <- mat.or.vec(81,18)
ART_need_age[,1] <- seq(1970,2050)
for (i in 1:81){
  j <- (i-1)*19+4
  ART_need_age[i,2:18]=as.numeric(levels(temp[j:(j+16),2]))[temp[j:(j+16),2]]
}

# ART numbers (by age and year taken from AIM)
temp <- as.data.frame(read.table(paste("HIV/",cntry,"/",cntry,"_ART_numbers_age.txt",sep=""),header=TRUE,fill=TRUE))
# Re-arrage to get in year vs age format
ART_number_age <- mat.or.vec(81,18)
ART_number_age[,1] <- seq(1970,2050)
for (i in 1:81){
  j <- (i-1)*19+4
  ART_number_age[i,2:18]=as.numeric(levels(temp[j:(j+16),2]))[temp[j:(j+16),2]]
}

# Calculate % coverage
ART_percent <- cbind(seq(1970,2050),ART_number_age[,2:18]/ART_need_age[,2:18])
# Correct for "nan" if "need" is zero
ART_percent[is.nan(ART_percent)] <- 0

# Convert into forcing functions - 5 year age bins replicated in C code for single year age bins
temp <- c(0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80)
for (i in 1:17){
  assign(paste("A",temp[i],sep=""),cbind(seq(1970,2050),approx(seq(1970,2050),ART_percent[,i+1],seq(1970,2050),rule=2)$y))
}

# ART eligibility threshold (in terms of model CD4 categories)
ART_data <- as.data.frame(read.table(paste("HIV/",cntry,"/",cntry,"_ART_data.txt",sep=""),header=TRUE,fill=TRUE))
# ART eligibility threshold
Athresh <- cbind(ART_data[,"Year"],ART_data[,"CD4_cat"])

#######################################################################################################################################
# Mortality from HIV when on ART - region, age, sex, CD4 and time on ART specific values from AIM. <15 values mapped to adult CD4.
# Use numbers on ART by sex to calculate average, take average over calendar time (varies little by time)

temp_m <- as.data.frame(read.table(paste("HIV/",cntry,"/",cntry,"_ART_mort.txt",sep=""),header=TRUE,fill=TRUE))

# ART numbers (by age and year taken from AIM)
temp_a <- as.data.frame(read.table(paste("HIV/",cntry,"/",cntry,"_ART_numbers_age.txt",sep=""),header=TRUE,fill=TRUE))
# Re-arrage to get in year vs age format for male numbers
ART_m <- mat.or.vec(81,17)
for (i in 1:81){
  j <- (i-1)*19+4
  ART_m[i,1:17]=as.numeric(levels(temp_a[j:(j+16),3]))[temp_a[j:(j+16),3]]
}
# then calculate % male and % female
m_per <- ART_m/ART_number_age[,2:18]
m_per[is.nan(m_per)] <- 1
f_per <- 1-m_per
# now calculate weighted mortality rates and store
temp_age <- c(1,2,3,4,4,5,5,6,6,7,7,7,7,7,7,7,7)
temp_cd4 <- rbind(c(2,3,4,5,6,7,8),c(10,11,12,13,14,15,16),c(18,19,20,21,22,23,24))
k<-1
temp_list <- c(0,17*7*3)
for (i in 1:17){
  for (j in 1:7){
    for (l in 1:3){
      temp_y <- (m_per[,i]*temp_m[temp_cd4[l,j],temp_age[i]+1] + f_per[,i]*temp_m[temp_cd4[l,j],temp_age[i]+8])
      temp_list[k] <- mean(temp_y[37:81]) # calculate mean between 2006 and 2050
      k <- k+1
    }
  }
}

#######################################################################################################################################
# Set up the RR structure for the paediatric TB parameters
# These values are used to multiply the RR for 

RR_a_0 <- 4.72
RR_a_5 <- 2.51
RR_sig_0 <- 0.38
RR_sig_5 <- 0.23
RR_sig_10 <- 0.47
RR_mu_0 <- 4.26








