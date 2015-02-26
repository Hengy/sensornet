import math

freq = int(input('Frequency: '))

#freq = 262.0
amplitude = 20000000

f = open('SineWave.txt', 'w')

sample = 0.0

for j in xrange(int(48000.0/freq)):
            
    sample = amplitude * math.sin(j * ((2*3.1415926)/(48000/freq)) )

    f.write(str(int(sample)) + ", ")