# Time_R, Version 1.0
# Tom Sumner, 02/02/2016

# Research Version of TIME

# This version replicates the core dynamic transmission model of TIME as implemented in the Spectrum Software Suite
 
# The model is implemented in R, but uses compiled C to code the equations (for computational efficiency)
# The model can be run in two age structure configurations: single year age bins; 5 year age bins (0-4,5-9,...)
 
# The model consists of the following files:
 
# Main.R - main script for running the model, the user must define the file path (containing model files and input folders (Demog, HIV, TB)), the country and the age structure to use
# Libraries_and_dll.R - script to load libraries and compile the C code into a dynamic linked library 
# TB_model.c - the equations implemented in C
# logcurve.R - function for defining generalised logistic function
# Data_load.R - loads and processes required input data
# Para_cn.R (where cn is the country) - file to define the parameters for the current model run
# Run_model.R - uses R package "desolve" to solve the equations and return outputs
# Plots.R - generates plots of demography (compared to UN population projections) and TB burden (compared to WHO estimates) 

# Libraries_and_dll.R, TB_model.c, Data_load.R, Run_model.R and Plots.R have separate versions for the 5 year age bin model (postscript _5ry). 
# The correct version is called based on the age structure defined in Main.R

# There are also a set of input files that are used by the model. They are organised into 3 sub-folders:

# Demog - initial population, migration and mortality rates
# HIV - ART data, HIV incidence
# TB - WHO TB estimates 

# These folders must be in the same directory as the model files

# The model also uses the following R packages which must be installed on the computer (note they are loaded when the model is called):

# deSolve
# reshape2
# ggplot2



