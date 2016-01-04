## Script to run TIME research model - runs the model for a single parameter set for a defined country
## Model can be run using 1 or 5 year age bins (5 year is faster but less accurate demographically)
## See README.md for further information

## First set the working directory - these must contain all model files including input folders (Demog, HIV, TB)
setwd("C:/Users/TOM SUMMER/Documents/TIME_research/TIME_R")

## Define Country (1=Bangladesh, 2=Ghana, 3=India, 4=South_Africa, 5=Vietnam)
cn <- 1

## Define age structure of the model (5 = 5 year age bins (0-4,5-9 etc); 1 = single year age bins)
n_age <- 5

if (n_age==5) suf <- "_5yr"
if (n_age==1) suf <- ""

## Inidcate whether to generate plots or not (0=no; 1=yes)
plotting <- 1

##################################################################################################################################

## This section only needs to be run once, unless you change the age structure (n_age) or country (cn) above when it must be rerun 

# Load packages, compile model and load DLL
# Load external data sources and create forcing functions

source(paste("Libraries_and_dll",suf,".R",sep=""))
source(paste("Data_load",suf,".R",sep=""))

##################################################################################################################################

## This section needs to be rerun each time you want to generate model outputs

# Set up the forcing functions and parameters
source(paste("Para_",cntry,".R",sep=""))

# Run the model (and time it) 
system.time(source(paste("Run_model",suf,".R",sep="")))

## Generate plots of demography and TB outputs (prev, inc, mort, notif) - resulting figures are plot_pop and plot_TB (_5yr)
if (plotting==1){
  source(paste("Plots",suf,".R",sep="")) 
}


