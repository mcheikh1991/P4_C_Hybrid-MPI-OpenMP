import matplotlib.pyplot as plt
import numpy as np
import re
import os
from os import listdir, remove
from os.path import isfile, join
from matplotlib.font_manager import FontProperties
from matplotlib import rcParams
rcParams['xtick.direction'] = 'in'
rcParams['ytick.direction'] = 'in'
rcParams['figure.figsize'] = 7.5, 7
rcParams['font.size'] =  12
rcParams['font.family'] = 'serif'


Loc = os.getcwd() 

def sort(main,other):
	main , other = zip(*sorted(zip(main,other)))
	main = np.array(main)
	other = np.array(other)
	return main, other

#---------------------------------------------------------
# Reading the Data  
#---------------------------------------------------------

Col = ['k','b','g','m','r','c','y','#92C6FF','#E24A33', '#001C7F', '.15', '0.40']
Mar = [".","s","o","d","P","v","^","<",">","1","2","3","4","8","+","p","P","*"]
Lin = ['-','.-',':']

i = 0
j = 0
g = 0
k = 0
FilesKilled = []
FilesWorking = []
RunNumber = []

All_Data =  [] # [Part,MPI,OpenMP,N,Answer,GTime,Memory]

Data_Loc = Loc +'/Data/'

# Gets all Files
AllFiles = [f for f in listdir(Data_Loc) if isfile(join(Data_Loc, f))]

for z in ['.e','.po','.pe']:
	ErrorFiles = [File for File in AllFiles if re.findall(z,File) != []]
	#for File in ErrorFiles: 
		#print 'Error File for deletion:',File
		#remove(Data_Loc+File)

# Keeps only the output files ".o"

AllFiles = [File for File in AllFiles if re.findall("\.o",File) != []]

print AllFiles

for File in AllFiles:
	Data_File = open(Data_Loc+File,'r')
	Data_File = Data_File.read()

	#try:
	Summary_Data = re.findall(r'Part[^-][^\n]*',Data_File)
	for Data in Summary_Data:
		Part   = int(re.findall(r'Part[0-9]*',Data)[0].split('Part')[1])
		MPI    = int(re.findall(r'MPI-[0-9]*',Data)[0].split('MPI-')[1])
		OpenMP = int(re.findall(r'OpenMP-[0-9]*',Data)[0].split('OpenMP-')[1])

		Data   = Data.split(':')[1]
		N      = int(Data.split(',')[0])
		Answer = float(Data.split(',')[-3])
		GTime  = float(Data.split(',')[-2])
		Memory = int(Data.split(',')[-1])
		if Part == 1:
			N = N*N
		elif Part ==3:
			GTime = GTime*20
		All_Data.append([Part,MPI,OpenMP,N,Answer,GTime,Memory])

	FilesWorking.append(File)
	#except:
	#	FilesKilled.append(File)
	#	print 'Killed File:',File

print 'Total number of runs: %d' % len(All_Data)


#---------------------------------------------------------
# Plot the Data  
#---------------------------------------------------------
'''
# Plot Part 1
MPI1OpenMP1  = [] # [MPI,N,Time,Memory]
MPI2OpenMP1  = [] # [MPI,N,Time,Memory]
MPI4OpenMP1  = [] # [MPI,N,Time,Memory]
MPI8OpenMP1  = [] # [MPI,N,Time,Memory]
MPI16OpenMP1 = [] # [MPI,N,Time,Memory]
MPI1OpenMP2  = [] # [MPI,N,Time,Memory]
MPI2OpenMP2  = [] # [MPI,N,Time,Memory]
MPI4OpenMP2  = [] # [MPI,N,Time,Memory]
MPI8OpenMP2  = [] # [MPI,N,Time,Memory]
MPI1OpenMP4  = [] # [MPI,N,Time,Memory]
MPI2OpenMP4  = [] # [MPI,N,Time,Memory]
MPI4OpenMP4  = [] # [MPI,N,Time,Memory]
MPI1OpenMP8  = [] # [MPI,N,Time,Memory]
MPI2OpenMP8 = [] # [MPI,N,Time,Memory]
for i in range(len(All_Data)):
	if All_Data[i][0] == 1: # Check if it is part 1
		if All_Data[i][1]

		if All_Data[i][2] == 1: # Count the Openmp Threads
			OpenMP1.append([All_Data[i][2],All_Data[i][-3],All_Data[i][-2],All_Data[i][-1]])
		elif All_Data[i][2] == 2: 
			OpenMP2.append([All_Data[i][2],All_Data[i][-3],All_Data[i][-2],All_Data[i][-1]])
		elif All_Data[i][2] == 4: 
			OpenMP4.append([All_Data[i][2],All_Data[i][-3],All_Data[i][-2],All_Data[i][-1]])
		elif All_Data[i][2] == 8: 
			OpenMP8.append([All_Data[i][2],All_Data[i][-3],All_Data[i][-2],All_Data[i][-1]])
'''

OpenMP = [1,2,4,8]
MPI    = [1,2,4,8,16]
Part   = [1,2,3]
Col    = ['C0','C1','C2','C3','C4','C1','C2','C3','C4','C2','C3','C4','C3','C4']
#Mar    = ['o','s','d','h','v','s','d','h','v','d','h','v','h','v']
Line   = ['-','-','-','-','-', '--','--','--','--', '-.','-.','-.', ':',':']

plt.figure(1)
for p in Part:
	k = 0
	for o in OpenMP:
		for m in MPI:
			Time   = []
			N      = []
			Memory = []
			for i in range(len(All_Data)):
				if All_Data[i][0] == p: # Check if it is correct part
					if m == All_Data[i][1] and o == All_Data[i][2]:
						N.append(All_Data[i][-4])
	  					Time.append(All_Data[i][-2]/60.0)
						Memory.append(All_Data[i][-1])
			if N != []:
				if m == 1 and o ==1:
					print max(N)
				plt.plot(N,Time,linestyle = Line[k],color=Col[k],marker=Mar[k],label='MPI-%d/OpenMP-%d'%(m,o))
				k = k+1

	leg = plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.17), ncol=4, fancybox=False, shadow=False ,fontsize=9)
	leg.get_frame().set_linewidth(0.0)
	plt.xlabel('Total Number of Points')
	if p == 1:
		plt.ylim([-5,500])
	plt.ylabel('Global Time [$min$]')
	plt.savefig('figures/N_Vs_Time_Part_%d.png'%p)
	plt.close()

N_Plot = [39999600001,600000000,700000000] 
plt.figure(2)
for p in Part:
	k = 0
	for o in OpenMP:
		Time   = []
		Threads  = []
		for i in range(len(All_Data)):
			if All_Data[i][0] == p: # Check if it is correct part
				if o == All_Data[i][2] and N_Plot[p-1] == All_Data[i][-4]:
  					Time.append(All_Data[i][-2]/60.0)
					Threads.append(All_Data[i][1]*All_Data[i][2])
		if Time != []:
			Threads,Time = sort(Threads,Time)
			plt.plot(Threads,Time,marker=Mar[k],label='OpenMP-%d'%(o))
			k = k+1

	leg = plt.legend(fancybox=False, shadow=False)
	leg.get_frame().set_linewidth(0.0)
	if p == 1:
		plt.ylim([-5,210])
	plt.xlabel('Total Number of Threads')
	plt.ylabel('Global Time [$min$]')
	plt.savefig('figures/Thread_Vs_Time_Part_%d.png'%p)
	plt.close()

'''

for i in range(len(Number_Cores)):
	if not np.isnan(GTime_Array_Single[i]).all():
		plt.plot(Number_Lines,GTime_Array_Single[i]/3600.0,color=Col[i],marker='s',label='single '+str(Number_Cores[i]))
	if not np.isnan(GTime_Array_Multiple[i]).all():
		plt.plot(Number_Lines,GTime_Array_Multiple[i]/3600.0,color=Col[i],marker='o',linestyle=':',label='multiple '+str(Number_Cores[i]))
plt.xlabel('Number of Lines')
plt.ylabel('Global Time ($hr$)')
plt.xticks(np.arange(200000, 1200000, 200000))
leg = plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.17), ncol=4, fancybox=False, shadow=False)
leg.get_frame().set_linewidth(0.0)

plt.savefig('Num_Lines_Vs_Time.png')
plt.close()

plt.figure(2)
for i in range(len(Number_Cores)):
	if not np.isnan(GTime_Array_Single[i]).all():
		plt.plot(Number_Lines,Single_Node_Memory[i]/1024,color=Col[i],marker='s',label='single '+str(Number_Cores[i]))
	if not np.isnan(GTime_Array_Multiple[i]).all():
		plt.plot(Number_Lines,Multiple_Node_Memory[i]/1024,color=Col[i],marker='o',linestyle=':',label='multiple '+str(Number_Cores[i]))
plt.xlabel('Number of Lines')
plt.ylabel('Memory (Mb) /Core ')
plt.xticks(np.arange(200000, 1200000, 200000))
leg = plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.17), ncol=4, fancybox=False, shadow=False)
leg.get_frame().set_linewidth(0.0)

plt.savefig('Num_Lines_Vs_Memory.png')
plt.close()

#-----------------------------------


Ncor_single = []
GTime_single = []
Ncor_multiple = []
GTime_multiple = []
Avg_Ratio_single = []
Avg_Ratio_multiple = []
d = -1
for MPI_DATA in ALL_MPI_DATA:
	d = d +1
	Summary_Data = MPI_DATA[0]
	Ncor  = Summary_Data[1]
	GTime = Summary_Data[2]
	Lines = Summary_Data[4]
	Type  = Summary_Data[5]
	All_Core_Data = MPI_DATA[1:Ncor+1]

	if Lines == 1000000:
		print Type,Ncor,GTime
		if Type == 'single':
			if Ncor_single.count(Ncor) == 0:
				Ncor_single.append(Ncor)
				GTime_single.append(GTime)
				Avg_Ratio_single.append(1)
			else:
				GTime_single[Ncor_single.index(Ncor)] += GTime
				Avg_Ratio_single[Ncor_single.index(Ncor)] += 1
		else:
			if Ncor_multiple.count(Ncor) == 0:
				Ncor_multiple.append(Ncor)
				GTime_multiple.append(GTime)
				Avg_Ratio_multiple.append(1)
			else:
				GTime_multiple[Ncor_multiple.index(Ncor)] += GTime
				Avg_Ratio_multiple[Ncor_multiple.index(Ncor)] += 1

Ncor_single		= np.array(Ncor_single)
GTime_single	= np.array(GTime_single)
Ncor_multiple	= np.array(Ncor_multiple)
GTime_multiple	= np.array(GTime_multiple)
Avg_Ratio_single   = np.array(Avg_Ratio_single)
Avg_Ratio_multiple = np.array(Avg_Ratio_multiple)

GTime_single   = GTime_single/Avg_Ratio_single
GTime_multiple = GTime_multiple/Avg_Ratio_multiple

Ncor_single,GTime_single = sort(Ncor_single,GTime_single)
Ncor_multiple,GTime_multiple = sort(Ncor_multiple,GTime_multiple)


plt.figure(2)
plt.plot(Ncor_single,GTime_single/3600.0,'b-o',label='single')
plt.plot(Ncor_multiple,GTime_multiple/3600.0,'r-s',label='multiple')
plt.xlabel('Number of Cores')
plt.ylabel('Global Time ($hr$)')
#plt.xlim([0,2.1e9])
#plt.plot([0,5e9],[25000,25000],'g--')
#plt.ylim([0,np.max(ElapsedTime)])
plt.legend(loc='upper right')
plt.savefig('Time_Vs_Num_Cores.png')
plt.close()

plt.figure(3)
plt.semilogx(Ncor_single,GTime_single/3600.0,'b-o',label='single',basex=2)
plt.semilogx(Ncor_multiple,GTime_multiple/3600.0,'r-s',label='multiple',basex=2)
plt.xlabel('Number of Cores')
plt.ylabel('Global Time ($hr$)')
#plt.xlim([0,2.1e9])
#plt.plot([0,5e9],[25000,25000],'g--')
#plt.ylim([0,np.max(ElapsedTime)])
plt.legend(loc='upper right')
plt.savefig('Time_Vs_Num_Cores_Log2.png')
plt.close()
#-----------------------------------

def autolabel(rects):
    """
    Attach a text label above each bar displaying its height
    """
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., height+30,
                '%d%s' % (int(height),'%'),ha='center', va='bottom',size='smaller',color='k')


rcParams['figure.figsize'] = 16, 7

plt.figure(4)
opacity = 0.7
error_config = {'ecolor': '0.3'}
width = 0.8
labels = 'Idle', 'Computational'
colors = ['gold', 'lightskyblue']
colors = ['darkred', 'dodgerblue']
colors = ['darkorange', 'navy']
explode = (0.05, 0) 
d = -1
for MPI_DATA in ALL_MPI_DATA:
	d = d +1
	Summary_Data = MPI_DATA[0]
	Ncor  = Summary_Data[1]
	GTime = Summary_Data[2]
	Lines = Summary_Data[4]
	Type  = Summary_Data[5]
	All_Core_Data = MPI_DATA[1:Ncor+1]
	Comp = []
	Idle = []
	fig, ax = plt.subplots(1,2)
	for Each_Core_Data in All_Core_Data:
		Comp_Time  = Each_Core_Data[4]
		Comp.append(Comp_Time)

	GTime = np.max(Comp)
	Avg_Comp = np.average(Comp)
	Avg_Idle = GTime - Avg_Comp
	sizes = [Avg_Idle,Avg_Comp]
	rects2 = ax[0].bar(np.arange(Ncor), np.array(Comp)/60, width, color=colors[0], alpha=opacity,error_kw=error_config)
	Pie    = ax[1].pie(sizes, explode=explode, labels=labels, autopct='%1.1f%%', shadow=False, startangle=140)
	#rects3 = ax.bar(np.arange(Ncor)+width, Idle, width, color='lightskyblue')#, alpha=opacity,error_kw=error_config)

	# add some text for labels, title and axes ticks
	ax[0].set_ylabel('Computational Time (min)')
	ax[0].set_xlabel('Cores')
	ax[0].set_title('Performance of each core')
	ax[1].set_title('Average performance time for all the cores')
	#ax.set_xticks([0.35, 1.175, 2.175, 3.175,4.175])
	ax[0].set_xticklabels(())
	#ax.legend((rects2[0], rects3[0]), ('Computational', 'Idle'))

	#autolabel(rects2)
	#autolabel(rects3)

	#ax.legend(loc='upper left')
	plt.savefig('figures/'+str(Type)+'_Machine_Performance_'+str(Ncor)+'-'+str(Lines)+'-'+str(d)+'.png')
	plt.close()
'''