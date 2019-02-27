import pymadx

import halosigma as _h

# definitions
primSig = _h.primSig
secoSig = _h.secoSig
tertSig = _h.tertSig
openSig = _h.openSig
primMat = 'carbon'
secoMat = 'copper'
tertMat = 'tungsten'
filename = 'collimatorSettings.dat'

# load optics
o = pymadx.Data.Tfs("../madx/ring.tfs")
rcols = o.GetElementsOfType('RCOLLIMATOR')

# loop over collimators we find and make up sizes from optics in nsigma
coldefs = []
for col in rcols:
    name  = col['NAME']
    index = o.IndexFromName(name)
    data  = o[index-1] # element before to get the optics at the start of the collimator
    sigx  = data['SIGMAX']
    sigy  = data['SIGMAY']
    print name, sigx, 6*sigx

    horizontal = 'x' in name.lower()
    vertical   = not horizontal
    primary    = 'prim' in name.lower()
    tertiary   = 'tert' in name.lower()
    print name, 'is primary', primary

    sigval = primSig if primary else secoSig
    if tertiary:
        sigval = tertSig
    print 'sigval',sigval
    xhgap  = sigval if horizontal else openSig
    if tertiary:
        xhgap = sigval
    print 'x sigmas',xhgap
    yhgap  = sigval if vertical else openSig
    if tertiary:
        yhgap = sigval
    print 'y sigmas',yhgap
    xhgap *= sigx
    yhgap *= sigy

    print 'xhgap',xhgap
    print 'yhgap',yhgap
    mat    = primMat if primary else secoMat
    if tertiary:
        mat = tertMat
    
    result = '\t'.join([name.ljust(10), mat.ljust(15), "{0:1.8e}".format(xhgap), "{0:1.8e}".format(yhgap)]) + '\n'

    coldefs.append(result)

# write a wee text file
f = open(filename, 'w')
f.write("# Collimator Settings\n")
f.write('\t'.join(['name'.ljust(10), 'material', 'xsize[m]', 'ysize[m]'])+'\n')
for d in coldefs:
    f.write(d)
f.close()

