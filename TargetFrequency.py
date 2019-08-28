""" 
TargetFrequency.py

Author : A. Murphy
Email  : almurp20@g.holycross.edu
Updated: 7/8/2019

"""

targetFrequency = 1 / 86400
Tobs = 496609.7333 # total continuous observation time of a telescope recording.


# Here's how it works.
# The maximum time that we calculate is going to be the inverse of the folding frequency,
# or change in frequency between adjacent data points on the FFT.
# AKA: 1/value = fmin where value is an adjusted observation time based upon the bin duration and amount.

# We want to pick Tobs such that targetFrequency = k*fmin where k is an integer constant between 1 and n.
# Also, we want to maximize Tobs at the same time.

# Tobs on its own is going to be equal to n*BinDuration, where we have predetermined n.
# However, the calculations we perform will be completely independent of the binduration because
# we can do calculations directly from frequency. 

# The equation for value is then value = 1/fmin = k/targetFrequency

numberList = [2**n for n in range(4,14)]  
for n in numberList:
    # minimize k such that ftarg*n/k ~ Tobs but not greater than Tobs
    k = 1
    value = k / targetFrequency
    while k < n and value < Tobs:
        BinDuration = value / n
        entry = [BinDuration, n, value]
        k += 1
        value = k / targetFrequency
        
    for i in entry:
        print(i, end=", ")
    print("")
    

# After executing this program on different values, it becomes apparent that the observation time must be a factor
# of the period corresponding to the target frequency, or else the target frequency cannot be exactly represented
# by the FFT.
