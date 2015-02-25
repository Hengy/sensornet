import math

filePrefix = "C"
noteFreq = 262.0
sampleSize = 18300
amplitude = 20000000

f = open('SineWave.txt', 'w')

sample = 0.0
index = 0.0
inc = 1.0 / 183.0
 
for j in xrange(sampleSize):
            
    sample = amplitude * math.sin( 6.283 * index )

    f.write(str(int(sample)) + ", ")
    
    if (j%20 == 0 and j != 0):
        f.write("\n")
    
    if (j%1000 == 0):
        print "Done another 1000"
    
    index += inc
    
print "Done"