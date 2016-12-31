from PIL import Image

im = Image.open('foo.bmp').convert('1')

pixels = list(im.getdata())

width, height = im.size

pixels = [pixels[i * width : (i + 1) * width] for i in range(height)]

data = [[1 if pixel >= 100 else 0 for pixel in pixels[i]] for i in range(height)]

S = [(i, j) for j in range(width) for i in range(height) if data[i][j] == 0]

e = 2
e_sample = [None] * len(S)

e_sample[0] = S[0]
e_sample[1] = max([(i, j) \
                  for j in range(max(0, e_sample[0][1] - e), min(width,  e_sample[0][1] + e + 1)) \
                  for i in range(max(0, e_sample[0][0] - e), min(height, e_sample[0][0] + e + 1)) \
                  if data[i][j] == 0],
              key = lambda xy: (xy[1] - e_sample[0][1]) ** 2 + (xy[0] - e_sample[0][0]) ** 2)
              
c = 1
             
def fun(xy):
    return (xy[1] - e_sample[c - 1][1]) ** 2 + (xy[0] - e_sample[c - 1][0]) ** 2
    
while (e_sample[c][0] - e_sample[0][0]) ** 2 + (e_sample[c][1] - e_sample[0][1]) ** 2 >= (e / 2) ** 2:
    e_sample[c + 1] = max([(i, j) \
                          for j in range(max(0, e_sample[c][1] - e), min(width,  e_sample[c][1] + e + 1)) \
                          for i in range(max(0, e_sample[c][0] - e), min(height, e_sample[c][0] + e + 1)) \
                          if data[i][j] == 0],
                      key = fun)
    c = c + 1

output = open("foo.xy", 'w')
output.seek(0)

output.write("2\n")
for xy in e_sample:
    if xy != None: 
        output.write(str(xy[1]) + ' ' + str(xy[0]) + ' 0\n')
    else:
        break
  
output.close()
