import sys
import os
import glob
import numpy
import matplotlib.pyplot as plt
from math import ceil, log

reports = glob.glob('report_*')
colors = ['r', 'g', 'b']

def extract_optimization(d):
    return '-' + d.split('_')[1]

def read_and_prepare(results_dir, fname):
    with open(os.path.join(results_dir, fname)) as f:
        data = f.readlines()

    return map(lambda line: line.strip(), data)

def norm(results_dir, v):
    return int(v) - int(read_and_prepare(results_dir, 'perf_nop.out')[1])

def pow10ceil(x):
        return 10**ceil(log(x,10))


# PLOT 1

fig1 = plt.figure()
ax1 = fig1.add_subplot(111)
ax1.set_xlabel('Fibonacci number (n)')
ax1.set_ylabel('Time needed [clock cycles]')

additional_yticks = []

def plot_fibonacci(results_dir, variant, color):
    data = read_and_prepare(results_dir, 'perf_fibonacci.out')
    data = data[1:12]

    x = [row.split(' ')[0] for row in data]
    y = [norm(results_dir, row.split(' ')[2]) for row in data]

    ax1.plot(x, y, label='Fibonacci(n) %s' % variant, marker='o', color=color)

def plot_spawn(results_dir, variant):
    data = read_and_prepare(results_dir, 'perf_spawn.out')
    data = data[1:11]

    x = [row.split(' ')[0] for row in data]
    y = [norm(results_dir, row.split(' ')[1]) for row in data]

    ax1.plot(x, y, label='Spawn n tasks %s' % variant)

def plot_interrupt(results_dir, variant):
    data = read_and_prepare(results_dir, 'perf_interrupt.out')
    data = data[1].split(' ')

    ax1.axhline(y=norm(results_dir, data[0]), label='Interrupt enter %s' % variant)
    ax1.axhline(y=norm(results_dir, data[1]), label='Interrupt exit %s' % variant)

def plot_switch(results_dir, variant, color):
    data = read_and_prepare(results_dir, 'perf_switch.out')[1]

    val = float(norm(results_dir, data))
    additional_yticks.append(val)
    ax1.axhline(y=val, label='Task switch %s' % variant, color=color)

for d, c in zip(reports, colors):
    plot_fibonacci(d, extract_optimization(d), c)
    plot_switch(d, extract_optimization(d), c)
#    plot_spawn(d, extract_optimization(d))
#    plot_interrupt(d, extract_optimization(d))

xmin, xmax = ax1.get_xlim()
ax1.xaxis.set_ticks(numpy.arange(xmin, xmax + 1, 1))

ymin, ymax = ax1.get_ylim()
step = pow10ceil(ymax / 10)
yticks = numpy.arange(ymin, ymax + 1, step)
yticks = numpy.append(yticks, additional_yticks)
ax1.yaxis.set_ticks(yticks)

leg1 = ax1.legend(loc='upper left', prop={'size': 9})
plt.tick_params(axis='y', which='major', labelsize=9)
fig1.savefig('libtasking-perf-fibonacci-switch.png')


# PLOT 2

d = 'report_O3'

fig2 = plt.figure()
ax2 = fig2.add_subplot(111)
ax2.set_xlabel('Number of tasks (n)')
ax2.set_ylabel('Average overhead for each increment [clock cycles]')

additional_yticks = []

def plot_mutex(results_dir):
    data = read_and_prepare(results_dir, 'perf_mutex.out')
    data = data[1:11]

    x = [row.split(' ')[0] for row in data]
    y1 = [(float(norm(results_dir, row.split(' ')[5])) - float(norm(results_dir, row.split(' ')[4]))) / float(norm(results_dir, row.split(' ')[3])) for row in data]
    y2 = [(float(norm(results_dir, row.split(' ')[6])) - float(norm(results_dir, row.split(' ')[4]))) / float(norm(results_dir, row.split(' ')[3])) for row in data]

    ax2.plot(x, y1, label='(1) Exclusive access overhead (lock and unlock of a mutex but without sleeping)', color='g')
    ax2.plot(x, y2, label='(2) Exclusive access and sleeping overhead (lock and unlock of a mutex)', marker='o', color='b')

    additional_yticks.append(round(y1[0]))
    additional_yticks.append(round(y2[0]))

plot_mutex(d)

ymin, ymax = ax2.get_ylim()
ax2.set_ylim(ymin, ymax * 1.30)

step = pow10ceil(ymax / 10)

ymin, ymax = ax2.get_ylim()
yticks = numpy.arange(ymin, ymax + 1, step)
yticks = numpy.append(yticks, additional_yticks)
ax2.yaxis.set_ticks(yticks)

leg2 = ax2.legend(loc='upper left', prop={'size': 9})
plt.tick_params(axis='y', which='major', labelsize=9)
fig2.savefig('libtasking-perf-mutex.png')
