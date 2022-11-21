# import the necessary packages
from imutils.perspective import four_point_transform
from imutils import contours
import imutils
import cv2
import operator
import time
import numpy as np
from statistics import mean 
import csv
import matplotlib.pyplot as plt
from pylive import live_plotter
# Reading an image in default mode

records1 =[]
records2 =[]
records3 =[]

#digit precise location list
digitCnts1 = []
digitCnts2 = []
digitCnts3 = []

digitCnts1.append((92,8,12,24))
digitCnts2.append((95,104,12,24))
digitCnts2.append((111,105,12,24))
digitCnts3.append((75,44,26,58))
digitCnts3.append((115,43,26,58))
#digitCnts.append((152,20,10,20))

DIGITS_LOOKUP = {
	(1, 1, 1, 0, 1, 1, 1): 0,
	(0, 0, 1, 0, 0, 1, 0): 1,
	(1, 0, 1, 1, 1, 0, 1): 2,
	(1, 0, 1, 1, 0, 1, 1): 3,
	(0, 1, 1, 1, 0, 1, 0): 4,
	(1, 1, 0, 1, 0, 1, 1): 5,
	(1, 1, 0, 1, 1, 1, 1): 6,
	(1, 0, 1, 0, 0, 1, 0): 7,
	(1, 1, 1, 1, 1, 1, 1): 8,
	(1, 1, 1, 1, 0, 1, 1): 9
}

cap = cv2.VideoCapture("SingleABEMA.mp4")
fps = cap.get(cv2.CAP_PROP_FPS)
cap.set(cv2.CAP_PROP_POS_AVI_RATIO,1)
duration=cap.get(cv2.CAP_PROP_POS_MSEC)
frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
timeBetweenFrames=duration/frame_count
cap.set(cv2.CAP_PROP_POS_AVI_RATIO,0)

print("fps: "+str(fps))
print("number of frames: "+ str(frame_count))
print("duration: "+str(duration))
print("timeBetween2Frames: "+str(timeBetweenFrames))
#print(timeBetweenFrames*412)

#plot points
xx1=[]
yy1=[]
xx2=[]
yy2=[]
xx3=[]
yy3=[]

size = 100
x_vec = np.linspace(0,1,size+1)[0:-1]
line1 = []
line2 = []
line3 = []
y_vec1 = [0]*len(x_vec)
y_vec2 = [0]*len(x_vec)
y_vec3 = [0]*len(x_vec)

def func_animate(i):
    x = np.linspace(0, 4*np.pi, 1000)
    y = np.sin(2 * (x - 0.1 * i))
    
    line.set_data(x, y)
    
    return line
# l1=plt.plot(xx1,yy1,'r--',label='type1')
# l2=plt.plot(xx2,yy2,'g--',label='type2')
# l3=plt.plot(xx3,yy3,'b--',label='type3')


num=0
start_time=time.time()


data =open('data1.csv', mode='w',newline='')
data_writer = csv.writer(data)
data_writer.writerow(["Frame_num","Time","records1","records2","records3"])
		
while(cap.isOpened()):
	ret, frame = cap.read()
	if ret == False:
		break
	if(num%10!=0):
		num=num+1
		continue
	#rotate or not
	# frame=cv2.transpose(frame)
	# image = cv2.flip(frame,1)
	image=frame
	# pre-process the image by resizing it, converting it to
	# graycale, blurring it, and computing an edge map
	image = imutils.resize(image, height=500)
	gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
	blurred = cv2.GaussianBlur(gray, (5, 5), 0)
	edged = cv2.Canny(blurred, 50, 200, 255)

	# find contours in the edge map, then sort them by their
	# size in descending order
	cnts = cv2.findContours(edged.copy(), cv2.RETR_LIST,
		cv2.CHAIN_APPROX_SIMPLE)
	cnts = imutils.grab_contours(cnts)
	cnts = sorted(cnts, key=cv2.contourArea, reverse=True)
	displayCnt = None

	# loop over the contours
	(x1,y1,w1,h1)=(52,181,135,147)
	(x2,y2,w2,h2)=(252,181,135,147)
	(x3,y3,w3,h3)=(600,150,300,147)
	
	displayCnt1=np.array([[x1,y1],[x1,y1+h1],[x1+w1,y1],[x1+w1,y1+h1]])
	displayCnt2=np.array([[x2,y2],[x2,y2+h2],[x2+w2,y2],[x2+w2,y2+h2]])
	displayCnt3=np.array([[x3,y3],[x3,y3+h3],[x3+w3,y3],[x3+w3,y3+h3]])
	
	warped1 = four_point_transform(gray, displayCnt1)
	output1 = four_point_transform(image, displayCnt1)
	warped2 = four_point_transform(gray, displayCnt2)
	output2 = four_point_transform(image, displayCnt2)
	warped3 = four_point_transform(gray, displayCnt3)
	output3 = four_point_transform(image, displayCnt3)
	
	#cv2.drawContours(image, [displayCnt1], 0, (0, 0, 255), 2)
	#cv2.drawContours(image, [displayCnt2], 0, (0, 0, 255), 2)
	#cv2.drawContours(image, [displayCnt3], 0, (0, 0, 255), 2)


	cv2.imshow("output1", output1)
	cv2.imshow("output2", output2)
	cv2.imshow("output3", output3)
	
	thresh1 = cv2.threshold(warped1, 112, 255,
	cv2.THRESH_BINARY_INV)[1]
	thresh2 = cv2.threshold(warped2, 180, 255,
	cv2.THRESH_BINARY_INV )[1]
	thresh3 = cv2.threshold(warped3, 110, 255,
	cv2.THRESH_BINARY_INV)[1]
	

	#********This is final output a data per 5 secs and draw lines part******
	if num>0 and num%150==0:
		
		if records1:
			average1=mean(records1)
		else:
			average1=-1	
			
		if records2:
			average2=mean(records2)
		else:
			average2=-1
				
		if records3:
			average3=mean(records3)
		else:
			average3=-1
		
		#print(records1)
		current_time=round(cap.get(cv2.CAP_PROP_POS_MSEC),2)
		print("Frame: "+str(num)+" Time: "+
			str(current_time)+
			" records1: "+str(round(average1, 2))+
			" records2: "+str(round(average2, 2))+
			" records3: "+str(round(average3, 2)))
		data_writer.writerow([str(num), str(current_time), 
		str(round(average1, 2)),str(round(average2, 2)),str(round(average3, 2))])
		
		# xx1.append(current_time)
		# xx2.append(current_time)
		# xx3.append(current_time)
		# yy1.append(round(average1, 2))
		# yy2.append(round(average2, 2))
		# yy3.append(round(average3, 2))
		
		y_vec1[-1] = round(average1, 2)
		y_vec2[-1] = round(average2, 2)
		y_vec3[-1] = round(average3, 2)
		#line1,line2,line3 = live_plotter(x_vec,y_vec1,line1,y_vec2,line2,y_vec3,line3)
		y_vec1 = np.append(y_vec1[1:],0.0)
		y_vec2 = np.append(y_vec2[1:],0.0)
		y_vec3 = np.append(y_vec3[1:],0.0)
		



	
		#reset records every 5 secs
		records1=[]
		records2=[]
		records3=[]

		
		
	#This is output1 recognizer
	#************************************************************#
	count1 =1
	digits1 = []
	trigle1=1
	for c in digitCnts1:
		# extract the digit ROI
		
		(x, y, w, h) = c
		roi = thresh1[y:y + h, x:x + w]
		
		# compute the width and height of each of the 7 segments
		# we are going to examine
		(roiH, roiW) = roi.shape
		start_point = (x, y) 
		end_point = (x+w, y+h)
		color = (0, 255, 0) 
		thickness = 1
		(dW, dH) = (3,3)
		dHC = int(2)
		# define the set of 7 segments
		segments = [
			((dW, 0), (w-dW, dH)),	# top
			((0, dH), (dW, (h // 2) - dHC)),	# top-left
			((w - dW, dH), (w-1, (h // 2) - dHC)),	# top-right
			((dW, (h // 2) - dHC) , (w-dW, (h // 2) + dHC)), # center
			((0,(h // 2) + dHC), (dW, h)),	# bottom-left
			((w - dW, (h // 2) + dHC), (w-1, h)),	# bottom-right
			((dW, h-1 ), (w-dW-1, h+1))	# bottom
		]
		on = [0] * len(segments)
		for (start, end) in segments:
			output1 = cv2.line(output1, tuple(map(operator.add,start,start_point)), 
			tuple(map(operator.add,end,start_point)), color, thickness)
		cv2.imshow("output1 no."+str(count1), output1)
		count1=count1+1
		
		for (i, ((xA, yA), (xB, yB))) in enumerate(segments):
		# extract the segment ROI, count the total number of
		# thresholded pixels in the segment, and then compute
		# the area of the segment
			segROI = roi[yA:yB, xA:xB]
			total = cv2.countNonZero(segROI)
			area = (xB - xA) * (yB - yA)
			#print(total / float(area))
		
		# if the total number of non-zero pixels is greater than
		# 50% of the area, mark the segment as "on"
			if total / float(area) > 0.4:
				on[i]= 1
		#print(tuple(on))
		#print(tuple(on))
		if (tuple(on) in DIGITS_LOOKUP.keys()):
			digit = DIGITS_LOOKUP[tuple(on)]
			digits1.append(digit)
		else:
			trigle1=0
			break
	if(trigle1==1):
		s=int(''.join(str(e) for e in digits1))
		records1.append(s)
		#print("Frame: "+str(num)+" Time: "+str(cap.get(cv2.CAP_PROP_POS_MSEC))+"data3: "+str(s))
		#print(type(s))
	#else:
		#print("Frame: "+str(num)+" Time: "+str(cap.get(cv2.CAP_PROP_POS_MSEC))+"data: error")	
	
	#output1 recognizer ends
	#*******************************************************#
	
	
	
	
	
	
	
	
	#This is output2 recognizer
	#************************************************************#
	count2 =1
	digits2 = []
	trigle2=1
	for c in digitCnts2:
		# extract the digit ROI
		
		(x, y, w, h) = c
		roi = thresh2[y:y + h, x:x + w]
		
		# compute the width and height of each of the 7 segments
		# we are going to examine
		(roiH, roiW) = roi.shape
		start_point = (x, y) 
		end_point = (x+w, y+h)
		color = (255, 0, 0) 
		thickness = 1
		(dW, dH) = (3,3)
		dHC = int(2)
		# define the set of 7 segments
		segments = [
			((dW, 0), (w-dW, dH)),	# top
			((0, dH), (dW, (h // 2) - dHC)),	# top-left
			((w - dW, dH), (w-1, (h // 2) - dHC)),	# top-right
			((dW, (h // 2) - dHC+2) , (w-dW, (h // 2) + dHC)), # center
			((0,(h // 2) + dHC), (dW, h)),	# bottom-left
			((w - dW, (h // 2) + dHC), (w-1, h)),	# bottom-right
			((dW-2, h-1 ), (w-dW-2, h))	# bottom
		]
		on = [0] * len(segments)
		for (start, end) in segments:
			output2 = cv2.line(output2, tuple(map(operator.add,start,start_point)), 
			tuple(map(operator.add,end,start_point)), color, thickness)
		cv2.imshow("output2 no."+str(count2), output2)
		count2=count2+1
		
		for (i, ((xA, yA), (xB, yB))) in enumerate(segments):
		# extract the segment ROI, count the total number of
		# thresholded pixels in the segment, and then compute
		# the area of the segment
			segROI = roi[yA:yB, xA:xB]
			total = cv2.countNonZero(segROI)
			area = (xB - xA) * (yB - yA)
			#print(total / float(area))
		
		# if the total number of non-zero pixels is greater than
		# 50% of the area, mark the segment as "on"
			if total / float(area) > 0.4:
				on[i]= 1
		#print(tuple(on))
		#print(tuple(on))
		if (tuple(on) in DIGITS_LOOKUP.keys()):
			digit = DIGITS_LOOKUP[tuple(on)]
			digits2.append(digit)
		else:
			trigle2=0
			break
	if(trigle2==1):
		s=int(''.join(str(e) for e in digits2))
		records2.append(s)
		#print("Frame: "+str(num)+" Time: "+str(cap.get(cv2.CAP_PROP_POS_MSEC))+"data3: "+str(s))
		#print(type(s))
	#else:
		#print("Frame: "+str(num)+" Time: "+str(cap.get(cv2.CAP_PROP_POS_MSEC))+"data: error")	
	
	#output2 recognizer ends
	#*******************************************************#









	
	#This is output3 recognizer
	#************************************************************#
	count3 =1
	digits3 = []
	trigle3=1
	for c in digitCnts3:
		# extract the digit ROI
		
		(x, y, w, h) = c
		roi = thresh3[y:y + h, x:x + w]
		
		# compute the width and height of each of the 7 segments
		# we are going to examine
		(roiH, roiW) = roi.shape
		start_point = (x, y) 
		end_point = (x+w, y+h)
		color = (0, 255, 0) 
		thickness = 1
		(dW, dH) = (3,3)
		dHC = int(2)
		# define the set of 7 segments
		segments = [
			((dW, 0), (w-dW, dH)),	# top
			((0, dH), (dW, (h // 2) - dHC)),	# top-left
			((w - dW, dH), (w-1, (h // 2) - dHC)),	# top-right
			((dW, (h // 2) - dHC) , (w-dW, (h // 2) + dHC)), # center
			((0,(h // 2) + dHC), (dW, h)),	# bottom-left
			((w - dW, (h // 2) + dHC), (w-1, h)),	# bottom-right
			((dW, h-1 ), (w-dW-1, h+1))	# bottom
		]
		on = [0] * len(segments)
		for (start, end) in segments:
			output3 = cv2.line(output3, tuple(map(operator.add,start,start_point)), 
			tuple(map(operator.add,end,start_point)), color, thickness)
		cv2.imshow("output3 no."+str(count3), output3)
		count3=count3+1
		
		for (i, ((xA, yA), (xB, yB))) in enumerate(segments):
		# extract the segment ROI, count the total number of
		# thresholded pixels in the segment, and then compute
		# the area of the segment
			segROI = roi[yA:yB, xA:xB]
			total = cv2.countNonZero(segROI)
			area = (xB - xA) * (yB - yA)
			#print(total / float(area))
		
		# if the total number of non-zero pixels is greater than
		# 50% of the area, mark the segment as "on"
			if total / float(area) > 0.4:
				on[i]= 1
		#print(tuple(on))
		#print(tuple(on))
		if (tuple(on) in DIGITS_LOOKUP.keys()):
			digit = DIGITS_LOOKUP[tuple(on)]
			digits3.append(digit)
		else:
			trigle3=0
			break
			
	if(trigle3==1):
		s=int(''.join(str(e) for e in digits3))
		records3.append(s)
		#print("Frame: "+str(num)+" Time: "+str(cap.get(cv2.CAP_PROP_POS_MSEC))+"data3: "+str(s))
		#print(type(s))
	#else:
		#print("Frame: "+str(num)+" Time: "+str(cap.get(cv2.CAP_PROP_POS_MSEC))+"data: error")	
	
	#output3 recognizer ends
	#*******************************************************#
	

		
		
		
	num=num+1		
	cv2.imshow("thresh1", thresh1)
	cv2.imshow("thresh2", thresh2)
	cv2.imshow("thresh3", thresh3)
	cv2.imshow("output", image)	
	if cv2.waitKey(1) & 0xFF == ord('q'):
            break
if cv2.waitKey(1) & 0xFF == ord('q'):		
	cap.release()
	cv2.destroyAllWindows()
	data.close()
	plt.close('all')

# cv2.imshow("output", output)	
# cv2.imshow("edge", edged)	
# cv2.imshow("thresh", thresh)

