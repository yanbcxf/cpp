import math
lines = int(input("How many lines?"))
inputs = []
reverse = True

for i in range(lines):
    inputs.append(input(str(i)+": "))

if(reverse):
	y = 0
	for i in inputs:
		i = i.replace(",", "")
		x = 0
		for number in i:
			if(number == "1"):
				print("draw_pixel(x+"+str(int(math.fabs(y-lines)))+", y+"+ str(x)+", color);")
			x+=1
		y+=1
else:
	y = 0
	for i in inputs:
		i = i.replace(",", "")
		x = 0
		for number in i:
			if(number == "1"):
				print("draw_pixel(x+"+str(x)+", y+"+ str(y)+", color);")
			x+=1
		y+=1
