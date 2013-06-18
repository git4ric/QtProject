g = open('stations.txt', 'r')
out = ""
for line in g:
    elements = line.split(",")
    i = 1
    while (i < len(elements)):
        out = out + "," + elements[i]
        i = i + 1
    
g.close()
l = open('routes.txt', 'w')
l.write(out)
l.close()

