import glob
import os
import sys

from PIL import Image

EXTS = 'png', 'PNG'

def averageHash(im):
    if not isinstance(im, Image.Image):
        im = Image.open(im)
    im = im.resize((8, 8), Image.ANTIALIAS).convert('L')
    avg = reduce(lambda x, y: x + y, im.getdata()) / 64.
    return reduce(lambda x, (y, z): x | (z << y),
                  enumerate(map(lambda i: 0 if i < avg else 1, im.getdata())),
                  0)

def hammingDistance(h1, h2):
    h, d = 0, h1 ^ h2
    while d:
        h += 1
        d &= d - 1
    return h

if __name__ == '__main__':
    images = []
    for ext in EXTS:
        images.extend(glob.glob('*.%s' % ext))

    if len(sys.argv) <= 1 or len(sys.argv) > 3:
        print "average hash:"
        progressbar = int(len(images) > 50 and sys.stdout.isatty())

        for f in images:
            print "%s,%s" % (f, averageHash(f))
    else:
        im, wd = sys.argv[1], '.' if len(sys.argv) < 3 else sys.argv[2]
        h = averageHash(im)
        print "average hash:\n %s\t%s\n" % (im, h)

        seq = []
        progressbar = int(len(images) > 50 and sys.stdout.isatty())
        for f in images:
            seq.append( (f, averageHash(f), hammingDistance(averageHash(f), h)) )

            #progessbar
            if progressbar:
                perc = 100. * progressbar / len(images)
                x = int(2 * perc / 5)
                print '\rCalculating... [' + '#' * x + ' ' * (40 - x) + ']',
                print '%.2f%%' % perc, '(%d/%d)' % (progressbar, len(images)),
                sys.stdout.flush()
                progressbar += 1

        if progressbar: print
        print "hamming distance:"
        for f in sorted(seq, key=lambda i: i[2]):
            print "%s\t%s\t%s" % (f[2], f[0] ,f[1])
